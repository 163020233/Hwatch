/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM cloud PCM playback via AudioStreamOut. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm_config.h"

#include "audio_manager.h"
#include "audio_manager_service.h"
#include "audio_stream_out.h"
#include "cmsis_os2.h"
#include "errcode.h"
#include "media_thread_adapt.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

using Audio::AudioInterrupt;
using Audio::AudioManager;
using Audio::InterruptListener;

extern "C" bool VoiceCloudLlmIsCallInterrupted(void);
extern "C" int32_t VoiceCloudLlmAudioPlayerStop(void);
extern "C" void VoiceCloudLlmPostCallInterrupt(void);

#ifndef WEARABLE_LOG_MODULE_SMART_VOICE
#define WEARABLE_LOG_MODULE_SMART_VOICE 0
#endif
#define VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ...)                                 \
    do {                                                                         \
        (void)(mod);                                                             \
        (void)std::printf("[t=%010u] " fmt, osKernelGetTickCount(), ##__VA_ARGS__); \
    } while (0)
/* fmt MUST be forwarded unparenthesised — see note in voice_cloud_llm.cpp. */
#define WEARABLE_LOGE(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)
#define WEARABLE_LOGW(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)
#define WEARABLE_LOGI(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)

namespace {

const uint32_t VOICE_CLOUD_LLM_PLAYER_STACK_SIZE = 0x1400;
const osPriority_t VOICE_CLOUD_LLM_PLAYER_PRIORITY = static_cast<osPriority_t>(30);
const uint32_t VOICE_CLOUD_LLM_PLAYER_DRAIN_GRACE_MS = 200;
const uint32_t VOICE_CLOUD_LLM_PLAYER_PUT_MESSAGE_DELAY_MS = 20;
const uint32_t VOICE_CLOUD_LLM_PLAYER_PUT_MESSAGE_MAX_TIMES = 10;

/*
 * Gap-smoothing buffers. When the cloud is slow or the network jitters, the
 * player thread detects an empty queue via non-blocking dequeue while the DAC
 * ring still has audio.  g_fadeChunk is filled with a linear ramp from the
 * last real sample down to zero and written as a bridge; subsequent gap
 * iterations write g_zeroChunk.  Both live in .bss — zero heap cost.
 */
static uint8_t g_zeroChunk[640] = {0};
static uint8_t g_fadeChunk[640];

/* Fade duration: 80 samples = 5 ms at 16 kHz.  Long enough to prevent an
 * audible pop from the DC discontinuity, short enough that the listener
 * barely perceives the transition. */
const uint32_t kGapFadeSamples = 80;
/* After this many consecutive silence chunks (10 s at neutral 20 ms pacing)
 * the gap is probably a dead-cloud, not transient jitter.  Log a warning and
 * keep running — the Worker's 30 s timeout will eventually tear down. */
const uint32_t kGapMaxSilenceChunks = 500;

class VoiceCloudLlmPlayerInterruptListener : public InterruptListener {
public:
    void OnInterrupt(int32_t type, int32_t hint) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm player interrupt, type=%d, hint=%d\r\n", type, hint);
        if (type == 1) {
            VoiceCloudLlmAudioPlayerStop();
            VoiceCloudLlmPostCallInterrupt();
        }
    }
};

struct VoiceCloudLlmPcmFrame {
    uint8_t *data;
    uint32_t len;
};

struct VoiceCloudLlmPlayerContext {
    std::atomic<bool> started;
    std::atomic<bool> stopRequested;
    std::atomic<bool> drainRequested;
    osMessageQueueId_t queue;
    MediaThreadIdHandle thread;
    std::unique_ptr<Audio::AudioStreamOut> streamOut;
    AudioSession sessionId;
    AudioRendererConfig renderConfig;
    AudioInterrupt interrupt;
    bool interruptActive;
    uint32_t chunkBytes;

    VoiceCloudLlmPlayerContext()
        : started(false), stopRequested(false), drainRequested(false),
          queue(nullptr), thread(nullptr),
          streamOut(nullptr), sessionId(AUDIO_SESSION_ID_NONE), interruptActive(false), chunkBytes(0)
    {
        std::memset(&renderConfig, 0, sizeof(renderConfig));
    }
};

VoiceCloudLlmPlayerContext g_playerCtx;
AudioManager &g_audioManager = AudioManager::GetInstance();

void VoiceCloudLlmFreeFrame(VoiceCloudLlmPcmFrame *frame)
{
    if (frame == nullptr || frame->data == nullptr) {
        return;
    }
    std::free(frame->data);
    frame->data = nullptr;
    frame->len = 0;
}

bool VoiceCloudLlmQueueFrame(const uint8_t *data, uint32_t len)
{
    if (data == nullptr || len == 0 || g_playerCtx.queue == nullptr) {
        return false;
    }

    VoiceCloudLlmPcmFrame frame = {};
    frame.data = static_cast<uint8_t *>(std::malloc(len));
    if (frame.data == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player frame alloc fail, len=%u\r\n", len);
        return false;
    }
    (void)std::memcpy(frame.data, data, len);
    frame.len = len;

    uint32_t try_times = 0;
    while (osMessageQueuePut(g_playerCtx.queue, &frame, 0, 0) != osOK) {
        try_times++;
        if (try_times >= VOICE_CLOUD_LLM_PLAYER_PUT_MESSAGE_MAX_TIMES) {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player queue put fail\r\n");
            VoiceCloudLlmFreeFrame(&frame);
            return false;
        }
        osDelay(VOICE_CLOUD_LLM_PLAYER_PUT_MESSAGE_DELAY_MS);
    }
    return true;
}

void VoiceCloudLlmFlushPlayerQueue()
{
    if (g_playerCtx.queue == nullptr) {
        return;
    }

    VoiceCloudLlmPcmFrame frame = {};
    while (osMessageQueueGet(g_playerCtx.queue, &frame, nullptr, 0) == osOK) {
        VoiceCloudLlmFreeFrame(&frame);
    }
}

void *VoiceCloudLlmAudioPlayerThread(void *argument)
{
    (void)argument;
    /*
     * Diagnostic state. We want to answer three questions from the log:
     *   1. Is the cloud delivery rate matching the playback rate? — compare
     *      arrival_gap_ms (time between successive frames pulled from queue)
     *      against frame.len/32 (real-time playback duration at 16k/16-bit).
     *   2. Is our DAC pacing holding up? — under steady state we expect rty=0
     *      and firstFail=UINT32_MAX. Any rty>0 means another thread preempted
     *      the player past its pacing window and the ring went full; the
     *      safety retry caught it.
     *   3. How deep is our PlayerFeed queue when each frame is pulled? —
     *      q=0 on arrival means we're already starved (cloud is the
     *      bottleneck); q>3 means cloud burst, we're falling behind.
     *
     * Note on the misleading middleware log line:
     *   audio_manager_service.cpp logs "audio stream out play failed 0x%x"
     *   from BOTH Play() and StreamWrite() — same message, two call sites.
     *   When you see that line during DOWNLINK, it's almost always the
     *   StreamWrite path under a jitter spike, and our safety retry catches
     *   it: look for a matching "[player] #N ... rty=>0" on the next line —
     *   that confirms recovery. err=0 in that summary means the frame still
     *   played fully. Only "[player] write fail after 1 retry" indicates an
     *   actual dropped tail.
     */
    uint32_t prevArrivalTick = 0;
    uint32_t totalFrames = 0;
    /*
     * DAC-rate pacing anchor. We track the tick at which the most recent
     * StreamWrite completed; before the next chunk we sleep until ~(chunkMs -
     * safety) has elapsed since then, so the ring drains roughly one chunk
     * before we top it up again. This keeps usage to 1-2 chunks deep in the
     * 6 kB / ~188 ms TTS ring, instead of burst-writing 8-13 chunks until
     * the driver returns 0xffffffff and we retry. Anchor persists across
     * frames so the cadence carries over a frame boundary if the queue had
     * back-to-back frames waiting.
     */
    uint32_t lastWriteCompleteTick = 0;
    bool pacingArmed = false;
    const uint32_t chunkBytes = g_playerCtx.chunkBytes > 0 ? g_playerCtx.chunkBytes : 1U;
    const uint32_t chunkMs = chunkBytes / 32U; /* 16k mono 16-bit => 32 B/ms */
    /* Leave ~2 ms safety so thread-scheduling jitter doesn't push us past
     * the next chunk's deadline (which would re-trigger driver underrun). */
    const uint32_t paceMs = (chunkMs > 2U) ? (chunkMs - 2U) : 0U;
    /*
     * Gap pacing: during a silence gap we write at exactly the DAC consumption
     * rate (chunkMs, 20 ms) so the ring buffer stays neutral — it neither
     * drains (underrun risk) nor fills (overflow risk).  Without this, a
     * busy-spin on silence writes would either jam the ring or under-drive it.
     */
    const uint32_t paceMsNeutral = chunkMs;
    /*
     * Gap-smoothing state. lastSample tracks the final sample of the most
     * recently written chunk so the fade-out ramp can start from exactly
     * the value the DAC is about to consume — no DC discontinuity.
     * gapActive gates between fade-out generation (first gap iteration)
     * and zero-silence sustain (subsequent iterations). silenceCount
     * caps the gap so we don't loop forever on a dead cloud.
     */
    int16_t lastSample = 0;
    bool gapActive = false;
    uint32_t silenceCount = 0;

    /*
     * Ring-buffer depth estimator.  Tracks how many ms of audio are currently
     * buffered inside the DAC's hardware ring (~192 ms / 6 kB at 16k/mono/
     * 16-bit).  The estimate drives dynamic write pacing so the ring stays
     * ~80 ms deep under normal conditions, absorbing up to 80 ms of cloud
     * jitter before the DAC runs dry and a gap is triggered.
     *
     * Model (open-loop, self-correcting):
     *   write success  → ringEstMs += chunkMs
     *   wall-clock tick → ringEstMs -= elapsed
     *   clamp to [0, kRingCapacityMs]
     *
     * When ringEstMs is below kRingTargetMs the pacing switches to fast-fill
     * (2 ms gap between writes), rapidly rebuilding the buffer.  Above
     * kRingOverflowMs the pacing goes neutral (20 ms) to let the DAC drain.
     * During gaps pacing is always neutral — the estimator updates but does
     * not influence the silence-write cadence.
     */
    int32_t ringEstMs = 0;
    uint32_t prevWriteDoneTick = 0;
    const int32_t kRingCapacityMs = 192;   // 6 kB / 32 B-per-ms
    const int32_t kRingTargetMs = 80;      // fast-fill below this
    const int32_t kRingOverflowMs = 160;   // slow down above this
    const int32_t kPaceFillMs = 2;         // nearly back-to-back (safe: ring
                                           // can absorb ~9 unchoked chunks)

    /* Update the ring estimator after a successful StreamWrite at tick `now`.
     * Must be called every time lastWriteCompleteTick is advanced. */
    auto updateRingEst = [&](uint32_t now) {
        if (prevWriteDoneTick > 0 && now > prevWriteDoneTick) {
            const int32_t consumed =
                static_cast<int32_t>(now - prevWriteDoneTick);
            ringEstMs = (ringEstMs > consumed)
                ? (ringEstMs - consumed) : 0;
        }
        ringEstMs += static_cast<int32_t>(chunkMs);
        if (ringEstMs > kRingCapacityMs) {
            ringEstMs = kRingCapacityMs;
        }
        prevWriteDoneTick = now;
    };

    while (true) {
        if (g_playerCtx.stopRequested.load()) {
            break;
        }
        if (g_playerCtx.drainRequested.load() && g_playerCtx.queue != nullptr &&
            osMessageQueueGetCount(g_playerCtx.queue) == 0) {
            break;
        }

        VoiceCloudLlmPcmFrame frame = {};
        const osStatus_t ret = osMessageQueueGet(g_playerCtx.queue, &frame, nullptr, 20);
        if (ret != osOK) {
            if (!pacingArmed) {
                /*
                 * Player hasn't received any audio yet (pre-start idle).
                 * No ring to feed — just yield and keep waiting.
                 */
                osDelay(5);
                continue;
            }
            /*
             * ---------- Gap detected while playback is active ----------
             * The last real chunk is still in the ring (we pace writes to
             * leave ~2 ms of slack), so the DAC hasn't run dry yet.  We
             * have one chunk-window to bridge the transition before a hard
             * DC jump causes an audible pop.
             */
            if (!gapActive) {
                /* Gap START: generate fade-out chunk from lastSample → 0. */
                const uint32_t sampleCount = chunkBytes / 2U;
                int16_t *samples = reinterpret_cast<int16_t *>(g_fadeChunk);
                for (uint32_t i = 0; i < kGapFadeSamples && i < sampleCount; ++i) {
                    const int32_t scale = static_cast<int32_t>(kGapFadeSamples - 1U - i);
                    const int32_t denom = static_cast<int32_t>(kGapFadeSamples - 1U);
                    samples[i] = static_cast<int16_t>(
                        static_cast<int32_t>(lastSample) * scale / denom);
                }
                for (uint32_t i = kGapFadeSamples; i < sampleCount; ++i) {
                    samples[i] = 0;
                }
                gapActive = true;
                silenceCount = 0;
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[player] gap begin, lastSample=%d\r\n", lastSample);
            }
            /* Neutral pacing: write silence at exactly DAC consumption rate. */
            if (paceMsNeutral > 0U) {
                const uint32_t since = osKernelGetTickCount() - lastWriteCompleteTick;
                if (since < paceMsNeutral) {
                    osDelay(paceMsNeutral - since);
                }
            }
            const uint8_t *silenceData =
                (silenceCount == 0) ? g_fadeChunk : g_zeroChunk;
            uint64_t gapWrittenSize = 0;
            AudioRendererConfig gapRenderConfig = {};
            const int32_t wRet = g_playerCtx.streamOut->StreamWrite(
                silenceData, chunkBytes, gapWrittenSize, gapRenderConfig);
            if (wRet == ERRCODE_SUCC) {
                lastWriteCompleteTick = osKernelGetTickCount();
                updateRingEst(lastWriteCompleteTick);
            }
            ++silenceCount;
            if (silenceCount == kGapMaxSilenceChunks) {
                WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[player] gap %u silence chunks (~%u s), "
                    "cloud may be dead\r\n",
                    static_cast<unsigned>(kGapMaxSilenceChunks),
                    static_cast<unsigned>(kGapMaxSilenceChunks * chunkMs / 1000U));
            }
            continue;
        }

        if (g_playerCtx.stopRequested.load()) {
            VoiceCloudLlmFreeFrame(&frame);
            break;
        }

        if (frame.data != nullptr && frame.len > 0) {
            /*
             * Gap END: insert a fade-in chunk BEFORE the real data so the
             * DAC transitions smoothly from silence (0) to the first sample
             * of the incoming frame — no DC jump, and no in-place mutation
             * of received audio.
             *
             * The fade-in chunk ramps linearly from 0 to firstSample over
             * kGapFadeSamples samples, then holds at firstSample for the
             * remainder of the chunk.  This guarantees the last sample of
             * the fade-in equals the first sample of the real frame, so
             * the boundary between the inserted chunk and the real data is
             * perfectly continuous.
             */
            if (gapActive) {
                const int16_t firstSample =
                    *reinterpret_cast<const int16_t *>(frame.data);
                const uint32_t sampleCount = chunkBytes / 2U;
                int16_t *fadeOut = reinterpret_cast<int16_t *>(g_fadeChunk);
                for (uint32_t i = 0; i < kGapFadeSamples && i < sampleCount; ++i) {
                    const int32_t scaled = static_cast<int32_t>(firstSample)
                        * static_cast<int32_t>(i)
                        / static_cast<int32_t>(kGapFadeSamples - 1U);
                    fadeOut[i] = static_cast<int16_t>(scaled);
                }
                for (uint32_t i = kGapFadeSamples; i < sampleCount; ++i) {
                    fadeOut[i] = firstSample;
                }
                /* Match the silence pacing so the fade-in lands in the ring
                 * exactly where the last zero ends. */
                if (paceMsNeutral > 0U) {
                    const uint32_t since = osKernelGetTickCount()
                        - lastWriteCompleteTick;
                    if (since < paceMsNeutral) {
                        osDelay(paceMsNeutral - since);
                    }
                }
                uint64_t fadeWrittenSize = 0;
                AudioRendererConfig fadeRenderConfig = {};
                const int32_t wRet = g_playerCtx.streamOut->StreamWrite(
                    g_fadeChunk, chunkBytes, fadeWrittenSize, fadeRenderConfig);
                if (wRet == ERRCODE_SUCC) {
                    lastWriteCompleteTick = osKernelGetTickCount();
                    updateRingEst(lastWriteCompleteTick);
                }
                gapActive = false;
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[player] gap end, silence=%u chunks (%u ms), "
                    "firstSample=%d\r\n",
                    static_cast<unsigned>(silenceCount),
                    static_cast<unsigned>(silenceCount * chunkMs),
                    firstSample);
            }
            const uint32_t arrivalTick = osKernelGetTickCount();
            const uint32_t arrivalGapMs = (prevArrivalTick == 0)
                ? 0U : (arrivalTick - prevArrivalTick);
            const uint32_t queueRemaining =
                (g_playerCtx.queue != nullptr) ? osMessageQueueGetCount(g_playerCtx.queue) : 0U;
            const uint32_t playbackMs = frame.len / 32U; /* 16k mono 16-bit => 32 B/ms */
            /*
             * AUDIO_STREAM_TTS expects fixed-size frame writes (640 B at 16k
             * mono 16-bit). Cloud audio_delta payloads are variable (14..14400 B
             * per event), and writing those raw through StreamWrite returns
             * AUDIO_ERROR (0xffffffff). Split into chunkBytes-sized writes.
             *
             * Ring-buffer pressure policy: the TTS renderer ring is ~6 kB
             * (about 192 ms at 16k/mono/16-bit) and we cannot resize it via
             * AudioRendererConfig — there is no buffer-size field. Cloud
             * audio_delta payloads routinely exceed 6 kB (up to 8192 B). If we
             * burst-write all 13 chunks back-to-back the ring fills after
             * ~9 chunks and the driver returns 0xffffffff for every subsequent
             * write until the DAC catches up. That works (we retry once after
             * 20 ms) but spams 9-12 driver-level errors per frame and leaves
             * zero headroom for thread-scheduling jitter.
             *
             * New strategy: actively pace writes to DAC consumption rate. After
             * each successful StreamWrite, sleep until ~(chunkMs - 2 ms) has
             * elapsed before the next write, so the ring drains ~chunkBytes
             * before we top it up. Net throughput is identical (DAC-bound
             * either way) but every individual write succeeds on the first
             * try, the ring stays 1-2 chunks deep instead of full, and a 20 ms
             * preemption no longer underruns the DAC. Failure retry is kept
             * as a safety net for extreme jitter (osDelay(20) + retry once).
             *
             * Pacing anchor (lastWriteCompleteTick / pacingArmed) persists
             * across frames so back-to-back frames in the queue inherit the
             * cadence — important when q≥2 and we'd otherwise burst the
             * first chunks of frame N+1 right after frame N completes.
             */
            const uint32_t chunk = chunkBytes;
            uint32_t offset = 0;
            uint32_t totalRetries = 0;
            uint32_t firstFailOffset = UINT32_MAX;
            const uint32_t writeStartTick = osKernelGetTickCount();
            int32_t lastErr = 0;
            uint64_t writtenSize = 0;
            AudioRendererConfig streamWriteRenderConfig = {};
            while (offset < frame.len && !g_playerCtx.stopRequested.load()) {
                const uint32_t remain = frame.len - offset;
                const uint32_t writeLen = remain < chunk ? remain : chunk;
                if (pacingArmed) {
                    /* Dynamic pacing: choose gap based on ring depth. */
                    uint32_t targetPace;
                    if (ringEstMs < kRingTargetMs) {
                        targetPace = static_cast<uint32_t>(kPaceFillMs);
                    } else if (ringEstMs < kRingOverflowMs) {
                        targetPace = paceMs;          // normal, 18 ms
                    } else {
                        targetPace = paceMsNeutral;   // neutral, 20 ms
                    }
                    if (targetPace > 0U) {
                        const uint32_t since =
                            osKernelGetTickCount() - lastWriteCompleteTick;
                        if (since < targetPace) {
                            osDelay(targetPace - since);
                        }
                    }
                }
                writtenSize = 0;
                streamWriteRenderConfig = {};
                int32_t writeRet = g_playerCtx.streamOut->StreamWrite(
                    frame.data + offset, writeLen, writtenSize, streamWriteRenderConfig);
                if (writeRet != ERRCODE_SUCC) {
                    if (firstFailOffset == UINT32_MAX) {
                        firstFailOffset = offset;
                    }
                    /* Jitter safety net: one DAC frame of backoff + one retry. */
                    osDelay(VOICE_CLOUD_LLM_UPLINK_FRAME_MS);
                    writtenSize = 0;
                    streamWriteRenderConfig = {};
                    writeRet = g_playerCtx.streamOut->StreamWrite(
                        frame.data + offset, writeLen, writtenSize, streamWriteRenderConfig);
                    totalRetries++;
                }
                if (writeRet != ERRCODE_SUCC) {
                    lastErr = writeRet;
                    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
                        "[player] write fail after 1 retry, ret=%d len=%u "
                        "(off=%u of %u), dropping tail\r\n",
                        writeRet, writeLen, offset, frame.len);
                    break;
                }
                lastWriteCompleteTick = osKernelGetTickCount();
                pacingArmed = true;
                updateRingEst(lastWriteCompleteTick);
                /*
                 * Capture the last written sample so a potential fade-out
                 * can start from exactly this amplitude — no DC jump.
                 */
                if (writeLen >= 2U) {
                    const int16_t *samples = reinterpret_cast<const int16_t *>(
                        frame.data + offset);
                    lastSample = samples[(writeLen / 2U) - 1U];
                }
                offset += writeLen;
            }
            const uint32_t writeElapsedMs = osKernelGetTickCount() - writeStartTick;
            /*
             * Per-frame summary. Read this as: every audio_delta from cloud is
             * one line. Columns:
             *   #N            frame index since player start
             *   len=          payload bytes (=cloud audio_delta size)
             *   plyMs=        real-time playback duration of this payload
             *   gapMs=        ms since previous frame pulled from queue
             *   q=            queue depth AFTER this frame was pulled
             *   wrMs=         wall time spent in chunked-write loop
             *   rty=          total retries across all chunks in this frame
             *   firstFail=    offset at which the first write failure happened
             *                 (UINT32_MAX = no failures)
             *   err=          last giving-up errcode (0 on success)
             * Healthy: gapMs ≈ plyMs ± jitter, q≤1, wrMs ≈ plyMs (DAC-paced),
             *          rty=0, firstFail=UINT32_MAX, err=0.
             * Underrun risk: gapMs >> plyMs (cloud slow, ring goes empty).
             * Jitter spike: rty>0 means a chunk write missed its pacing window
             *               (other thread preempted us > paceMs) and the ring
             *               had no room — the safety retry caught it.
             * Backlog: q>3 sustained means cloud bursting faster than DAC.
             */
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                "[player] #%u len=%u plyMs=%u gapMs=%u q=%u wrMs=%u rty=%u firstFail=%u err=%d\r\n",
                totalFrames, frame.len, playbackMs, arrivalGapMs, queueRemaining,
                writeElapsedMs, totalRetries,
                (firstFailOffset == UINT32_MAX) ? 0xFFFFFFFFU : firstFailOffset, lastErr);
            prevArrivalTick = arrivalTick;
            ++totalFrames;
        }
        VoiceCloudLlmFreeFrame(&frame);
    }
    return nullptr;
}

void VoiceCloudLlmPlayerCleanup()
{
    if (g_playerCtx.thread != nullptr) {
        MediaThreadJoin(&g_playerCtx.thread);
    }
    if (g_playerCtx.drainRequested.load()) {
        osDelay(VOICE_CLOUD_LLM_PLAYER_DRAIN_GRACE_MS);
    }
    VoiceCloudLlmFlushPlayerQueue();
    if (g_playerCtx.streamOut) {
        (void)g_playerCtx.streamOut->Stop();
    }
    if (g_playerCtx.streamOut) {
        (void)g_playerCtx.streamOut->Release();
        g_playerCtx.streamOut.reset();
    }
    if (g_playerCtx.interruptActive) {
        (void)g_audioManager.DeactivateAudioInterrupt(g_playerCtx.interrupt);
        g_playerCtx.interruptActive = false;
    }
    g_playerCtx.interrupt = {};
    if (g_playerCtx.queue != nullptr) {
        (void)osMessageQueueDelete(g_playerCtx.queue);
        g_playerCtx.queue = nullptr;
    }
    g_playerCtx.sessionId = AUDIO_SESSION_ID_NONE;
    g_playerCtx.stopRequested.store(false);
    g_playerCtx.drainRequested.store(false);
    g_playerCtx.started.store(false);
    g_playerCtx.chunkBytes = 0;
    g_playerCtx.thread = nullptr;
}

uint32_t VoiceCloudLlmFrameBytes(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr || config->outputSampleRate == 0 || config->channels == 0 ||
        config->bitsPerSample < 8 || config->uplinkFrameMs == 0) {
        return 0;
    }
    const uint64_t bytesPerFrame = static_cast<uint64_t>(config->outputSampleRate) * config->channels *
        (config->bitsPerSample / 8U) * config->uplinkFrameMs / 1000U;
    if (bytesPerFrame == 0 || bytesPerFrame > UINT32_MAX) {
        return 0;
    }
    return static_cast<uint32_t>(bytesPerFrame);
}

} // namespace

extern "C" int32_t VoiceCloudLlmAudioPlayerStart(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr) {
        return ERRCODE_FAIL;
    }
    if (g_playerCtx.started.load()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] player start skipped, already started\r\n");
        return ERRCODE_SUCC;
    }
    if (VoiceCloudLlmIsCallInterrupted()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] player start blocked — call is active\r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] player start, rate=%u ch=%u bits=%u frameMs=%u\r\n",
        config->outputSampleRate, config->channels, config->bitsPerSample, config->uplinkFrameMs);
    if (!AudioManagerInit()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio manager init fail\r\n");
        return ERRCODE_FAIL;
    }

    g_playerCtx.queue = osMessageQueueNew(VOICE_CLOUD_LLM_QUEUE_DEPTH, sizeof(VoiceCloudLlmPcmFrame), nullptr);
    if (g_playerCtx.queue == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player queue create fail\r\n");
        return ERRCODE_FAIL;
    }

    g_playerCtx.sessionId = g_audioManager.MakeSessionId();
    if (g_playerCtx.sessionId == AUDIO_SESSION_ID_NONE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm make session id fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }

    g_playerCtx.renderConfig.audioFormat = PCM;
    g_playerCtx.renderConfig.streamType = AUDIO_STREAM_TTS;
    g_playerCtx.renderConfig.sampleRate = config->outputSampleRate;
    g_playerCtx.renderConfig.sessionID = g_playerCtx.sessionId;
    g_playerCtx.renderConfig.channelCount = config->channels;
    g_playerCtx.renderConfig.deviceId = 0;
    g_playerCtx.renderConfig.connectedId = 0;
    g_playerCtx.renderConfig.linkDir = AUDIO_DOWN_LINK;
    g_playerCtx.renderConfig.sampleFmt = 0;
    g_playerCtx.renderConfig.bitWidth = static_cast<AudioBitWidth>(config->bitsPerSample);
    // Keep compatibility with AudioRendererConfig versions that do not expose isSync.

    std::shared_ptr<VoiceCloudLlmPlayerInterruptListener> listener =
        std::make_shared<VoiceCloudLlmPlayerInterruptListener>();
    if (listener.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player interrupt listener create fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }
    g_playerCtx.interrupt = { g_playerCtx.renderConfig.streamType, g_playerCtx.sessionId, listener };
    if (g_audioManager.ActivateAudioInterrupt(g_playerCtx.interrupt) == INTERRUPT_FAILED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player activate interrupt fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }
    g_playerCtx.interruptActive = true;

    g_playerCtx.streamOut = std::unique_ptr<Audio::AudioStreamOut>(new Audio::AudioStreamOut());
    if (g_playerCtx.streamOut == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream out create fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }
    if (g_playerCtx.streamOut->Init(g_playerCtx.renderConfig) != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream out init fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }
    if (g_playerCtx.streamOut->Play() != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream out play fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }

    const uint32_t frameBytes = VoiceCloudLlmFrameBytes(config);
    if (frameBytes == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm invalid player frame size\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }
    g_playerCtx.chunkBytes = frameBytes;

    g_playerCtx.stopRequested.store(false);
    g_playerCtx.drainRequested.store(false);
    MediaThreadattr attr = {"voice_cloud_llm_play", VOICE_CLOUD_LLM_PLAYER_STACK_SIZE,
        THREAD_SCHED_INVALID, VOICE_CLOUD_LLM_PLAYER_PRIORITY, false};
    g_playerCtx.thread = MediaThreadCreate(VoiceCloudLlmAudioPlayerThread, nullptr, &attr);
    if (g_playerCtx.thread == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player thread create fail\r\n");
        VoiceCloudLlmPlayerCleanup();
        return ERRCODE_FAIL;
    }

    g_playerCtx.started.store(true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] player started, sessionId=%d chunkBytes=%u\r\n",
        g_playerCtx.sessionId, g_playerCtx.chunkBytes);
    return ERRCODE_SUCC;
}

extern "C" int32_t VoiceCloudLlmAudioPlayerFeed(const uint8_t *data, uint32_t len)
{
    if (!g_playerCtx.started.load() || g_playerCtx.stopRequested.load() || g_playerCtx.drainRequested.load()) {
        return ERRCODE_FAIL;
    }
    if (!VoiceCloudLlmQueueFrame(data, len)) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

extern "C" int32_t VoiceCloudLlmAudioPlayerDrainThenStop(void)
{
    if (!g_playerCtx.started.load() && g_playerCtx.thread == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] player drain skipped, not running\r\n");
        return ERRCODE_SUCC;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] player drain-then-stop, pending=%u\r\n",
        g_playerCtx.queue != nullptr ? osMessageQueueGetCount(g_playerCtx.queue) : 0U);
    g_playerCtx.started.store(false);
    g_playerCtx.stopRequested.store(false);
    g_playerCtx.drainRequested.store(true);
    VoiceCloudLlmPlayerCleanup();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] player drained and stopped\r\n");
    return ERRCODE_SUCC;
}

extern "C" int32_t VoiceCloudLlmAudioPlayerStop(void)
{
    if (!g_playerCtx.started.load() && g_playerCtx.thread == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] player stop skipped, not running\r\n");
        return ERRCODE_SUCC;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] player stop requested\r\n");
    g_playerCtx.started.store(false);
    g_playerCtx.drainRequested.store(false);
    g_playerCtx.stopRequested.store(true);
    VoiceCloudLlmPlayerCleanup();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] player stopped\r\n");
    return ERRCODE_SUCC;
}
