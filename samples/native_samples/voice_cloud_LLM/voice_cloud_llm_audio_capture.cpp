/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM mic PCM capture via AudioStreamIn. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm_config.h"

#include "audio_manager.h"
#include "audio_manager_service.h"
#include "audio_stream_in.h"
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

extern "C" int32_t VoiceCloudLlmPostAudioFrame(const uint8_t *data, uint32_t len);
extern "C" bool VoiceCloudLlmIsWakeupTestMode(void);
extern "C" bool VoiceCloudLlmIsIdleListening(void);
extern "C" bool VoiceCloudLlmIsUplinkStreaming(void);
extern "C" bool VoiceCloudLlmIsCallInterrupted(void);
extern "C" int32_t VoiceCloudLlmAudioCaptureStop(void);
extern "C" void VoiceCloudLlmPostCallInterrupt(void);

namespace {

const uint32_t VOICE_CLOUD_LLM_CAPTURE_STACK_SIZE = 0x1400;
const osPriority_t VOICE_CLOUD_LLM_CAPTURE_PRIORITY = static_cast<osPriority_t>(30);

class VoiceCloudLlmCaptureInterruptListener : public InterruptListener {
public:
    void OnInterrupt(int32_t type, int32_t hint) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm capture interrupt, type=%d, hint=%d\r\n", type, hint);
        if (type == 1) {
            VoiceCloudLlmAudioCaptureStop();
            VoiceCloudLlmPostCallInterrupt();
        }
    }
};

struct VoiceCloudLlmCaptureContext {
    std::atomic<bool> started;
    std::atomic<bool> stopRequested;
    MediaThreadIdHandle thread;
    std::unique_ptr<Audio::AudioStreamIn> streamIn;
    uint8_t *buffer;
    uint32_t frameBytes;
    AudioSession sessionId;
    AudioInterrupt interrupt;
    bool interruptActive;

    VoiceCloudLlmCaptureContext()
        : started(false), stopRequested(false), thread(nullptr), streamIn(nullptr), buffer(nullptr), frameBytes(0),
          sessionId(AUDIO_SESSION_ID_NONE), interruptActive(false)
    {
    }
};

VoiceCloudLlmCaptureContext g_captureCtx;
AudioManager &g_audioManager = AudioManager::GetInstance();

uint32_t VoiceCloudLlmFrameBytes(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr || config->inputSampleRate == 0 || config->channels == 0 ||
        config->bitsPerSample < 8 || config->uplinkFrameMs == 0) {
        return 0;
    }
    const uint64_t bytesPerFrame = static_cast<uint64_t>(config->inputSampleRate) * config->channels *
        (config->bitsPerSample / 8U) * config->uplinkFrameMs / 1000U;
    if (bytesPerFrame == 0 || bytesPerFrame > UINT32_MAX) {
        return 0;
    }
    return static_cast<uint32_t>(bytesPerFrame);
}

/*
 * Integer-only mean-square computation on 16-bit mono PCM. We compare
 * mean-square against SILENCE_RMS_THRESHOLD² to avoid a sqrt call in the
 * hot capture loop. For a 160 ms / 2560-sample batch this runs ~2560
 * multiply-accumulate ops — negligible on the capture thread.
 */
static uint32_t ComputePcmMeanSquare(const uint8_t *data, uint32_t len)
{
    const int16_t *samples = reinterpret_cast<const int16_t *>(data);
    const uint32_t count = len / sizeof(int16_t);
    uint64_t sumSq = 0;
    for (uint32_t i = 0; i < count; ++i) {
        const int64_t s = static_cast<int64_t>(samples[i]);
        sumSq += static_cast<uint64_t>(s * s);
    }
    if (count == 0) {
        return 0;
    }
    return static_cast<uint32_t>(sumSq / count);
}

void *VoiceCloudLlmAudioCaptureThread(void *argument)
{
    (void)argument;
    uint32_t batchOffset = 0;
    /*
     * Tick of the last frame we actually posted. Starts at 0 so the first
     * batch in a capture session is always posted (now - 0 ≈ uptime >>
     * keep-alive threshold). Updated on every post, irrespective of state.
     */
    uint32_t lastSendTick = 0;
    /*
     * Batch counter since this capture thread started. The energy gate is
     * bypassed for the first VOICE_CLOUD_LLM_ENERGY_GATE_WARMUP_BATCHES so
     * the server VAD gets enough context to detect speech. Auto-resets on
     * thread restart (multi-turn). Incremented once per completed batch,
     * regardless of whether the batch was posted or throttled.
     */
    uint32_t batchCount = 0;
#if VOICE_CLOUD_LLM_SILENCE_RMS_THRESHOLD > 0
    const uint32_t rmsThresholdSq = static_cast<uint32_t>(VOICE_CLOUD_LLM_SILENCE_RMS_THRESHOLD) *
                                    VOICE_CLOUD_LLM_SILENCE_RMS_THRESHOLD;
#endif
    while (!g_captureCtx.stopRequested.load()) {
        if (g_captureCtx.buffer == nullptr || g_captureCtx.frameBytes == 0) {
            osDelay(10);
            continue;
        }

        const uint32_t batchCap = g_captureCtx.frameBytes * VOICE_CLOUD_LLM_UPLINK_BATCH_FRAMES;
        if (batchOffset >= batchCap) {
            batchOffset = 0;
        }
        uint32_t size = g_captureCtx.frameBytes;
        CapturerBuffer capBuffer;
        capBuffer.data = g_captureCtx.buffer + batchOffset;
        capBuffer.size = size;
        capBuffer.frameCount = 0;
        const int32_t ret = g_captureCtx.streamIn->ObtainBuffer(capBuffer, false);
        if (ret <= 0) {
            size = 0;
        } else {
            size = static_cast<uint32_t>(ret);
        }
        if (size == 0) {
            osDelay(10);
            continue;
        }
        batchOffset += size;

        if (batchOffset < batchCap) {
            continue;
        }

        /*
         * Energy gate: outside UPLINK_STREAMING (i.e. during IDLE_LISTENING,
         * pre-session buffering, etc.), silence batches are throttled to
         * VOICE_CLOUD_LLM_SILENCE_KEEPALIVE_MS so the server VAD stays
         * alive without wasting uplink on ambient noise. When the user
         * starts speaking (RMS ≥ threshold), we resume full-rate streaming
         * immediately and server VAD detects the next utterance.
         *
         * During UPLINK_STREAMING all batches are sent unconditionally —
         * the user is actively speaking and every frame matters.
         *
         * The energy gate is bypassed for the first
         * VOICE_CLOUD_LLM_ENERGY_GATE_WARMUP_BATCHES after capture starts,
         * so the server VAD receives enough audio context
         * without waiting across throttled keep-alive intervals.
         * The counter is local to the capture thread — it resets automatically
         * on thread restart (multi-turn) without cross-TU signalling.
         */
        bool shouldPost = true;
#if VOICE_CLOUD_LLM_SILENCE_RMS_THRESHOLD > 0
        if (!VoiceCloudLlmIsUplinkStreaming()) {
            if (batchCount >= VOICE_CLOUD_LLM_ENERGY_GATE_WARMUP_BATCHES) {
                const uint32_t meanSq = ComputePcmMeanSquare(g_captureCtx.buffer, batchOffset);
                if (meanSq < rmsThresholdSq) {
                    const uint32_t now = osKernelGetTickCount();
                    if (now - lastSendTick < VOICE_CLOUD_LLM_SILENCE_KEEPALIVE_MS) {
                        shouldPost = false;
                    }
                }
            }
        }
#endif
        if (shouldPost) {
            lastSendTick = osKernelGetTickCount();
            if (VoiceCloudLlmPostAudioFrame(g_captureCtx.buffer, batchOffset) != ERRCODE_SUCC) {
                WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "voice cloud llm post audio frame fail, size=%u\r\n", batchOffset);
            }
        }
        ++batchCount;
        batchOffset = 0;
    }
    return nullptr;
}

void VoiceCloudLlmCaptureCleanup(void)
{
    if (g_captureCtx.streamIn) {
        (void)g_captureCtx.streamIn->Stop();
    }
    if (g_captureCtx.thread != nullptr) {
        MediaThreadJoin(&g_captureCtx.thread);
    }
    if (g_captureCtx.streamIn) {
        (void)g_captureCtx.streamIn->Release();
        g_captureCtx.streamIn.reset();
    }
    if (g_captureCtx.buffer != nullptr) {
        std::free(g_captureCtx.buffer);
        g_captureCtx.buffer = nullptr;
    }
    g_captureCtx.frameBytes = 0;
    g_captureCtx.sessionId = AUDIO_SESSION_ID_NONE;
    g_captureCtx.stopRequested.store(false);
    g_captureCtx.started.store(false);
    g_captureCtx.thread = nullptr;
    if (g_captureCtx.interruptActive) {
        (void)g_audioManager.DeactivateAudioInterrupt(g_captureCtx.interrupt);
        g_captureCtx.interruptActive = false;
    }
    g_captureCtx.interrupt = {};
}

} // namespace

extern "C" int32_t VoiceCloudLlmAudioCaptureStart(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr) {
        return ERRCODE_FAIL;
    }
    if (g_captureCtx.started.load()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] capture start skipped, already started\r\n");
        return ERRCODE_SUCC;
    }
    if (VoiceCloudLlmIsCallInterrupted()) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] capture start blocked — call is active\r\n");
        return ERRCODE_FAIL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] capture start, rate=%u ch=%u bits=%u frameMs=%u wakeupTestMode=%d\r\n",
        config->inputSampleRate, config->channels, config->bitsPerSample, config->uplinkFrameMs,
        VoiceCloudLlmIsWakeupTestMode() ? 1 : 0);
    if (!AudioManagerInit()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio manager init fail\r\n");
        return ERRCODE_FAIL;
    }

    CapturerInputConfig captureConfig = {};
    captureConfig.audioFormat = PCM;
    captureConfig.streamType = AUDIO_STREAM_VOICE_ASSISTANT;
    captureConfig.sampleRate = config->inputSampleRate;
    captureConfig.sessionID = g_audioManager.MakeSessionId();
    captureConfig.channelCount = config->channels;
    captureConfig.linkDir = AUDIO_UP_LINK;
    captureConfig.disableSEA = VoiceCloudLlmIsWakeupTestMode() && (VOICE_CLOUD_LLM_CAPTURE_DISABLE_SEA != 0);
    captureConfig.micType = 0;
    captureConfig.bitWidth = static_cast<AudioBitWidth>(config->bitsPerSample);
    captureConfig.isAlwaysWake = false;

    if (captureConfig.sessionID == AUDIO_SESSION_ID_NONE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm make session id fail\r\n");
        return ERRCODE_FAIL;
    }

    std::shared_ptr<VoiceCloudLlmCaptureInterruptListener> listener =
        std::make_shared<VoiceCloudLlmCaptureInterruptListener>();
    if (listener.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm capture interrupt listener create fail\r\n");
        return ERRCODE_FAIL;
    }
    g_captureCtx.interrupt = { captureConfig.streamType, captureConfig.sessionID, listener };
    if (g_audioManager.ActivateAudioInterrupt(g_captureCtx.interrupt) == INTERRUPT_FAILED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm capture activate interrupt fail\r\n");
        g_captureCtx.interrupt = {};
        return ERRCODE_FAIL;
    }
    g_captureCtx.interruptActive = true;

    g_captureCtx.streamIn = std::unique_ptr<Audio::AudioStreamIn>(new Audio::AudioStreamIn());
    if (g_captureCtx.streamIn == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream in create fail\r\n");
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }
    if (g_captureCtx.streamIn->Init(captureConfig) != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream in init fail\r\n");
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }
    if (g_captureCtx.streamIn->Start() != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio stream in start fail\r\n");
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }

    g_captureCtx.frameBytes = VoiceCloudLlmFrameBytes(config);
    if (g_captureCtx.frameBytes == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm invalid capture frame size\r\n");
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }

    /*
     * Allocate room for VOICE_CLOUD_LLM_UPLINK_BATCH_FRAMES capture frames so
     * the thread can accumulate a 100 ms PCM batch and post it as one message
     * (see VoiceCloudLlmAudioCaptureThread). Posting per 20 ms frame puts ~50
     * WS sends/sec through TLS+cJSON+base64, which the worker can't drain;
     * batching cuts the rate ~5x.
     */
    const uint32_t captureBufferBytes = g_captureCtx.frameBytes * VOICE_CLOUD_LLM_UPLINK_BATCH_FRAMES;
    g_captureCtx.buffer = static_cast<uint8_t *>(std::malloc(captureBufferBytes));
    if (g_captureCtx.buffer == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm capture buffer alloc fail, bytes=%u\r\n", captureBufferBytes);
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }

    g_captureCtx.stopRequested.store(false);
    MediaThreadattr attr = {"voice_cloud_llm_cap", VOICE_CLOUD_LLM_CAPTURE_STACK_SIZE,
        THREAD_SCHED_INVALID, VOICE_CLOUD_LLM_CAPTURE_PRIORITY, false};
    g_captureCtx.thread = MediaThreadCreate(VoiceCloudLlmAudioCaptureThread, nullptr, &attr);
    if (g_captureCtx.thread == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm capture thread create fail\r\n");
        VoiceCloudLlmCaptureCleanup();
        return ERRCODE_FAIL;
    }

    g_captureCtx.started.store(true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] capture started, frameBytes=%u batchFrames=%u sessionId=%d\r\n",
        g_captureCtx.frameBytes, (uint32_t)VOICE_CLOUD_LLM_UPLINK_BATCH_FRAMES, captureConfig.sessionID);
    return ERRCODE_SUCC;
}

extern "C" int32_t VoiceCloudLlmAudioCaptureStop(void)
{
    if (!g_captureCtx.started.load() && g_captureCtx.thread == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] capture stop skipped, not running\r\n");
        return ERRCODE_SUCC;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] capture stop requested\r\n");
    g_captureCtx.stopRequested.store(true);
    VoiceCloudLlmCaptureCleanup();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] capture stopped\r\n");
    return ERRCODE_SUCC;
}

