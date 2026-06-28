/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM main state machine entry API. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm.h"
#include "voice_cloud_llm_config.h"
#include "voice_cloud_llm_errcode.h"
#include "voice_cloud_llm_protocol.h"

#include "cmsis_os2.h"
#include "intelligent_voice_asr_engine.h"
#include "media_thread_adapt.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef WEARABLE_LOG_MODULE_SMART_VOICE
#define WEARABLE_LOG_MODULE_SMART_VOICE 0
#endif
/*
 * Tick timestamp on every line. osKernelGetTickCount() returns ms since boot
 * on this CMSIS-OS port; zero-padded to 10 digits so columns stay aligned for
 * the first ~115 days of uptime. Use this to measure: wake→ws-open, ws-open→
 * chat.updated, SPEECH_STOPPED→first audio_delta, audio_delta→audible playback.
 */
#define VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ...)                                    \
    do {                                                                            \
        (void)(mod);                                                                \
        (void)std::printf("[t=%010u] " fmt, osKernelGetTickCount(), ##__VA_ARGS__); \
    } while (0)
/*
 * NOTE: do NOT wrap `fmt` in parentheses when forwarding. String-literal
 * concatenation ("[t=%010u] " fmt) requires fmt to be a bare string literal;
 * parenthesising it ("[t=%010u] " (fmt)) makes the preprocessor produce
 * "literal"("literal") which the compiler reads as a function call —
 * exactly the "called object type 'const char[N]' is not a function" error.
 */
#define WEARABLE_LOGE(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)
#define WEARABLE_LOGW(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)
#define WEARABLE_LOGI(mod, fmt, ...) VOICE_CLOUD_LLM_LOG_PRINT(mod, fmt, ##__VA_ARGS__)

typedef void (*VoiceCloudLlmWsEventCallback)(const uint8_t *data, uint32_t len, bool binary, void *ctx);
int32_t VoiceCloudLlmWsConnect(const VoiceCloudLlmConfig *config, VoiceCloudLlmWsEventCallback callback,
    VoiceCloudLlmWsEventCallback statusCallback, void *ctx);
int32_t VoiceCloudLlmWsSendText(const char *text);
int32_t VoiceCloudLlmWsSendBinary(const uint8_t *data, uint32_t len);
int32_t VoiceCloudLlmWsClose(void);

extern "C" int32_t VoiceCloudLlmAudioCaptureStart(const VoiceCloudLlmConfig *config);
extern "C" int32_t VoiceCloudLlmAudioCaptureStop(void);
extern "C" int32_t VoiceCloudLlmAudioPlayerStart(const VoiceCloudLlmConfig *config);
extern "C" int32_t VoiceCloudLlmAudioPlayerStop(void);
extern "C" int32_t VoiceCloudLlmAudioPlayerFeed(const uint8_t *data, uint32_t len);
extern "C" int32_t VoiceCloudLlmAudioPlayerDrainThenStop(void);

namespace {

const uint32_t VOICE_CLOUD_LLM_STACK_SIZE = 0x2000;
const osPriority_t VOICE_CLOUD_LLM_THREAD_PRIORITY = static_cast<osPriority_t>(30);
const uint32_t VOICE_CLOUD_LLM_SESSION_BUFFER_LEN = 1024;

enum VoiceCloudLlmState {
    VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP = 0,
    VOICE_CLOUD_LLM_STATE_WAKEUP_DETECTED,
    VOICE_CLOUD_LLM_STATE_WS_CONNECTING,
    VOICE_CLOUD_LLM_STATE_CHAT_CREATING,       /* waiting for chat.created from server */
    VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING,/* chat.update sent, waiting for chat.updated */
    VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING,
    /*
     * Order matters for log readability: a normal turn flows 5 → 6 → 7 → 6 →
     * 9 (uplink → await → downlink → await → return-wakeup). AWAITING_RESPONSE
     * sits between uplink and downlink because it's the "Coze is thinking,
     * audio not yet flowing" idle state — pre-DOWNLINK_PLAYING numerically
     * matches its temporal position in the happy path.
     *
     * AWAITING_RESPONSE covers two distinct "waiting on Coze" sub-cases that
     * share the same external behavior (uplink gated, no new turn yet):
     *   - after SPEECH_STOPPED: waiting for the first AUDIO_DELTA
     *   - after AUDIO_STREAM_COMPLETED: waiting for the player tail to drain
     *     and for CHAT_COMPLETED to transition to IDLE_LISTENING (multi-turn)
     * Splitting these into separate states added no logic, just a longer enum.
     * Tell them apart in logs by looking at the previous transition.
     */
    VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE,
    VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING,
    /*
     * Multi-turn idle listening between conversation turns. Capture is running,
     * audio is streaming to the server for VAD, and we're waiting for the next
     * speech_started or an idle timeout. Entered from CHAT_COMPLETED; exits to
     * UPLINK_STREAMING on speech_started, or ReturnToWakeup on idle timeout.
     */
    VOICE_CLOUD_LLM_STATE_IDLE_LISTENING,
    VOICE_CLOUD_LLM_STATE_ERROR_RECOVER,
    VOICE_CLOUD_LLM_STATE_RETURN_WAKEUP,
};

enum VoiceCloudLlmMsgType {
    VOICE_CLOUD_LLM_MSG_WAKEUP = 0,
    VOICE_CLOUD_LLM_MSG_WS_OPEN,
    VOICE_CLOUD_LLM_MSG_SERVER_EVENT,
    VOICE_CLOUD_LLM_MSG_AUDIO_FRAME,
    VOICE_CLOUD_LLM_MSG_STOP,
    VOICE_CLOUD_LLM_MSG_ERROR,
};

struct VoiceCloudLlmMsg {
    VoiceCloudLlmMsgType type;
    VoiceCloudLlmServerEvent event;
    uint8_t *payload;
    uint32_t payloadLen;
};

/*
 * Pre-session capture buffer node. While the WS is still doing TCP/TLS/WS
 * handshake and chat.created/chat.updated round-trips, the mic is already
 * running (see "early capture" path). Captured PCM batches are appended
 * here as a singly-linked list and flushed to the WS the moment chat.updated
 * arrives. This trades a bit of RAM (one batch = 2.56 kB at 16k/mono/16-bit
 * with batchFrames=8) for ~1-2 s of perceived wakeup→record latency.
 */
struct VoiceCloudLlmPendingFrame {
    VoiceCloudLlmPendingFrame *next;
    uint8_t *payload;
    uint32_t payloadLen;
};

struct VoiceCloudLlmContext {
    osMessageQueueId_t queue;
    osThreadId_t workerThread;
    MediaMutexHandle mutex;
    bool started;
    bool running;
    bool asrStarted;
    bool localWakeupEnabled;
    /*
     * Cleared on ReturnToWakeup. Set when the player fails to open during
     * this chat turn (typically uapi_adec_set_attr 0x80005321 after the
     * audio HAL has just exited deep-sleep). The first failure logs once,
     * subsequent AUDIO_DELTA frames in the same turn drop silently instead
     * of retrying ~30 times per reply.
     */
    bool playerStartFailedThisTurn;
    VoiceCloudLlmState state;
    AsrEngineInitParams asrInitParams;
    /*
     * Singly-linked list of capture batches recorded *before* chat.updated
     * arrived. Owned by the worker thread; appended by HandleAudioFrame in
     * pre-streaming states and flushed in SESSION_UPDATED handler. Capped
     * (see VOICE_CLOUD_LLM_PENDING_FRAMES_MAX) so the buffer can't grow
     * unbounded if handshake stalls.
     */
    VoiceCloudLlmPendingFrame *pendingHead;
    VoiceCloudLlmPendingFrame *pendingTail;
    uint32_t pendingCount;
    /*
     * Tick at which the state machine entered IDLE_LISTENING. Used to detect
     * idle timeout: if no speech_started arrives within
     * VOICE_CLOUD_LLM_IDLE_TIMEOUT_MS of this tick, we return to wakeup.
     * Set on entry to IDLE_LISTENING from both SESSION_UPDATED (first round)
     * and CHAT_COMPLETED (multi-turn). Cleared in ReturnToWakeup.
     */
    uint32_t idleListeningStartTick;
};

/*
 * Cap on the pre-session capture queue. At batchFrames=8 (160 ms) and
 * batchBytes=2560, 20 entries hold ~3.2 s of speech buffered while the WS
 * handshake completes. If handshake takes longer than that we drop the
 * oldest batch — the user will hear a small leading clip drop but the WS
 * stays responsive.
 */
const uint32_t VOICE_CLOUD_LLM_PENDING_FRAMES_MAX = 20;

VoiceCloudLlmContext g_voiceCloudCtx = {};

const char *VoiceCloudLlmStateName(VoiceCloudLlmState state)
{
    switch (state) {
        case VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP:         return "IDLE_WAKEUP";
        case VOICE_CLOUD_LLM_STATE_WAKEUP_DETECTED:     return "WAKEUP_DETECTED";
        case VOICE_CLOUD_LLM_STATE_WS_CONNECTING:       return "WS_CONNECTING";
        case VOICE_CLOUD_LLM_STATE_CHAT_CREATING:        return "CHAT_CREATING";
        case VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING: return "SESSION_CONFIGURING";
        case VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING:    return "UPLINK_STREAMING";
        case VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING:    return "DOWNLINK_PLAYING";
        case VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE:      return "AWAITING_RESPONSE";
        case VOICE_CLOUD_LLM_STATE_IDLE_LISTENING:      return "IDLE_LISTENING";
        case VOICE_CLOUD_LLM_STATE_ERROR_RECOVER:       return "ERROR_RECOVER";
        case VOICE_CLOUD_LLM_STATE_RETURN_WAKEUP:       return "RETURN_WAKEUP";
        default:                                        return "?";
    }
}

const char *VoiceCloudLlmServerEventName(VoiceCloudLlmServerEventType type)
{
    switch (type) {
        case VOICE_CLOUD_LLM_EVENT_NONE:                 return "NONE";
        case VOICE_CLOUD_LLM_EVENT_SESSION_CREATED:      return "SESSION_CREATED(chat.created)";
        case VOICE_CLOUD_LLM_EVENT_SESSION_UPDATED:      return "SESSION_UPDATED(chat.updated)";
        case VOICE_CLOUD_LLM_EVENT_SPEECH_STARTED:       return "SPEECH_STARTED";
        case VOICE_CLOUD_LLM_EVENT_SPEECH_STOPPED:       return "SPEECH_STOPPED";
        case VOICE_CLOUD_LLM_EVENT_INPUT_AUDIO_DONE:     return "INPUT_AUDIO_DONE";
        case VOICE_CLOUD_LLM_EVENT_TEXT_DELTA:           return "TEXT_DELTA";
        case VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA:          return "AUDIO_DELTA";
        case VOICE_CLOUD_LLM_EVENT_AUDIO_STREAM_COMPLETED: return "AUDIO_STREAM_COMPLETED";
        case VOICE_CLOUD_LLM_EVENT_CHAT_COMPLETED:       return "CHAT_COMPLETED";
        case VOICE_CLOUD_LLM_EVENT_SERVER_ERROR:         return "SERVER_ERROR";
        default:                                         return "?";
    }
}

void VoiceCloudLlmAsrListener(AsrEngineEvent event, void *data, uint32_t length);
void VoiceCloudLlmOnWsMessage(const uint8_t *data, uint32_t len, bool binary, void *ctx);
void VoiceCloudLlmOnWsStatus(const uint8_t *data, uint32_t len, bool binary, void *ctx);
void VoiceCloudLlmWorker(void *argument);
void VoiceCloudLlmFreeMsgPayload(VoiceCloudLlmMsg *msg);
void VoiceCloudLlmHandleResponseTimeout(VoiceCloudLlmContext *ctx);
void VoiceCloudLlmDeleteRuntime(VoiceCloudLlmContext *ctx);

void VoiceCloudLlmSetState(VoiceCloudLlmContext *ctx, VoiceCloudLlmState state)
{
    if (ctx == nullptr) {
        return;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    const VoiceCloudLlmState prev = ctx->state;
    ctx->state = state;
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
    if (prev != state) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] state %d(%s) -> %d(%s)\r\n",
            prev, VoiceCloudLlmStateName(prev), state, VoiceCloudLlmStateName(state));
    }
}

VoiceCloudLlmState VoiceCloudLlmGetState(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return VOICE_CLOUD_LLM_STATE_ERROR_RECOVER;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    const VoiceCloudLlmState state = ctx->state;
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
    return state;
}

bool VoiceCloudLlmIsRunning(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return false;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    const bool running = ctx->running;
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
    return running;
}

void VoiceCloudLlmSetRunning(VoiceCloudLlmContext *ctx, bool running)
{
    if (ctx == nullptr) {
        return;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    ctx->running = running;
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
}

void VoiceCloudLlmDropOldestAudioFrame(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr || ctx->queue == nullptr) {
        return;
    }
    VoiceCloudLlmMsg old;
    if (osMessageQueueGet(ctx->queue, &old, nullptr, 0) == osOK) {
        VoiceCloudLlmFreeMsgPayload(&old);
    }
}

/*
 * Post a message to the worker queue.
 *
 * For audio-frame messages (which are high-rate and easy to overflow), the policy
 * is "drop oldest, keep newest" — drop a queued frame and retry once. For other
 * message types we never silently discard, so a failure logs and returns false.
 */
bool VoiceCloudLlmPostMsg(VoiceCloudLlmContext *ctx, const VoiceCloudLlmMsg &msg)
{
    if (ctx == nullptr || ctx->queue == nullptr || !VoiceCloudLlmIsRunning(ctx)) {
        return false;
    }
    osStatus_t ret = osMessageQueuePut(ctx->queue, &msg, 0, 0);
    if (ret == osOK) {
        return true;
    }
    if (msg.type == VOICE_CLOUD_LLM_MSG_AUDIO_FRAME) {
        VoiceCloudLlmDropOldestAudioFrame(ctx);
        ret = osMessageQueuePut(ctx->queue, &msg, 0, 0);
        if (ret == osOK) {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
                "voice cloud llm audio queue full, dropped oldest frame\r\n");
            return true;
        }
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm queue put fail, type=%d ret=%d\r\n",
        msg.type, ret);
    return false;
}

uint32_t VoiceCloudLlmGetQueueWaitMs(VoiceCloudLlmContext *ctx)
{
    switch (VoiceCloudLlmGetState(ctx)) {
        case VOICE_CLOUD_LLM_STATE_WS_CONNECTING:
        case VOICE_CLOUD_LLM_STATE_CHAT_CREATING:
        case VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING:
        case VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING:
        case VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING:
        case VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE:
            return VOICE_CLOUD_LLM_RESPONSE_TIMEOUT_MS;
        case VOICE_CLOUD_LLM_STATE_IDLE_LISTENING:
            /*
             * Fallback: if capture thread dies in IDLE_LISTENING the queue goes
             * silent. The primary idle→wakeup path is the per-audio-frame check
             * in HandleAudioFrame (~6 Hz); this timeout is a safety net.
             */
            return VOICE_CLOUD_LLM_IDLE_TIMEOUT_MS;
        default:
            return osWaitForever;
    }
}

bool VoiceCloudLlmPostSimpleMsg(VoiceCloudLlmContext *ctx, VoiceCloudLlmMsgType type)
{
    VoiceCloudLlmMsg msg;
    (void)std::memset(&msg, 0, sizeof(msg));
    msg.type = type;
    return VoiceCloudLlmPostMsg(ctx, msg);
}

bool VoiceCloudLlmPostStopMsg(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr || ctx->queue == nullptr) {
        return false;
    }
    VoiceCloudLlmMsg drainMsg;
    while (osMessageQueueGet(ctx->queue, &drainMsg, nullptr, 0) == osOK) {
        VoiceCloudLlmFreeMsgPayload(&drainMsg);
    }
    VoiceCloudLlmMsg stopMsg;
    (void)std::memset(&stopMsg, 0, sizeof(stopMsg));
    stopMsg.type = VOICE_CLOUD_LLM_MSG_STOP;
    const osStatus_t ret = osMessageQueuePut(ctx->queue, &stopMsg, 0, 0);
    if (ret != osOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm stop queue put fail, ret=%d\r\n", ret);
        return false;
    }
    return true;
}

void VoiceCloudLlmFreeMsgPayload(VoiceCloudLlmMsg *msg)
{
    if (msg == nullptr || msg->payload == nullptr) {
        return;
    }
    std::free(msg->payload);
    msg->payload = nullptr;
    msg->payloadLen = 0;
    msg->event.payload = nullptr;
    msg->event.payloadLen = 0;
    msg->event.payloadAllocated = false;
}

int32_t VoiceCloudLlmStartWakeupRecognition()
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    (void)std::memset(&ctx->asrInitParams, 0, sizeof(ctx->asrInitParams));
    ctx->asrInitParams.enableNpu = false;
    ctx->asrInitParams.language = ASR_ENGINE_CHINESE;
    ctx->asrInitParams.workMode = ASR_ENGINE_ALWAYS_WAKE;
    ctx->asrInitParams.listener = VoiceCloudLlmAsrListener;

    int32_t ret = IntellVoiceAsrEngineInit(&ctx->asrInitParams);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm asr init fail, ret=%d\r\n", ret);
        ctx->asrStarted = false;
        return ret;
    }

    ret = IntellVoiceAsrEngineStartRecognition();
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm asr start fail, ret=%d\r\n", ret);
        (void)IntellVoiceAsrEngineDeInit();
        ctx->asrStarted = false;
        return ret;
    }

    ctx->asrStarted = true;
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP);
    return 0;
}

void VoiceCloudLlmStopAndDeinitAsr()
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    if (!ctx->asrStarted) {
        return;
    }

    const int32_t stopRet = IntellVoiceAsrEngineStopRecognition();
    if (stopRet != 0) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm asr stop fail, ret=%d\r\n", stopRet);
    }
    const int32_t deinitRet = IntellVoiceAsrEngineDeInit();
    if (deinitRet != 0) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm asr deinit fail, ret=%d\r\n", deinitRet);
    }
    ctx->asrStarted = false;
}

bool VoiceCloudLlmCanAcceptWakeup(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return false;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    const bool accept = ctx->running && ctx->state == VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP;
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
    return accept;
}

bool VoiceCloudLlmCanSendAudioFrame(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return false;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexLock(ctx->mutex);
    }
    /*
     * Producer-side gate: accept the batch into the worker queue whenever
     * the mic is meaningfully on. The worker (HandleAudioFrame) decides what
     * to do with it based on state:
     *   - WAKEUP_DETECTED / WS_CONNECTING / CHAT_CREATING / SESSION_CONFIGURING
     *     → push into the pending pre-session buffer, flushed on chat.updated
     *   - UPLINK_STREAMING / DOWNLINK_PLAYING → send to cloud immediately
     *   - anything else → drop
     * If we filter to UPLINK/DOWNLINK only here, the pre-session buffer never
     * sees the batches (the early-capture design becomes a no-op) and the
     * first ~1 s of user speech is lost to PostAudioFrame returning FAIL.
     */
    const bool accept = ctx->running &&
        (ctx->state == VOICE_CLOUD_LLM_STATE_WAKEUP_DETECTED ||
            ctx->state == VOICE_CLOUD_LLM_STATE_WS_CONNECTING ||
            ctx->state == VOICE_CLOUD_LLM_STATE_CHAT_CREATING ||
            ctx->state == VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING ||
            ctx->state == VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING ||
            ctx->state == VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING ||
            ctx->state == VOICE_CLOUD_LLM_STATE_IDLE_LISTENING);
    if (ctx->mutex != nullptr) {
        MediaMutexUnLock(ctx->mutex);
    }
    return accept;
}

void VoiceCloudLlmDrainQueuedAudioFrames(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr || ctx->queue == nullptr) {
        return;
    }
    uint32_t drained = 0;
    while (osMessageQueueGetCount(ctx->queue) > 0) {
        VoiceCloudLlmMsg drain;
        (void)std::memset(&drain, 0, sizeof(drain));
        if (osMessageQueueGet(ctx->queue, &drain, nullptr, 0) != osOK) {
            break;
        }
        if (drain.type == VOICE_CLOUD_LLM_MSG_AUDIO_FRAME) {
            VoiceCloudLlmFreeMsgPayload(&drain);
            ++drained;
            continue;
        }
        /* Non-audio message — re-post so we don't lose state events. */
        const osStatus_t putRet = osMessageQueuePut(ctx->queue, &drain, 0, 0);
        if (putRet != osOK) {
            VoiceCloudLlmFreeMsgPayload(&drain);
        }
        /*
         * Stop here to avoid re-shuffling the same item forever; the worker
         * will pick up the non-audio messages on its next iteration.
         */
        break;
    }
    if (drained > 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] drained %u stale audio frames from queue\r\n", drained);
    }
}

void VoiceCloudLlmPendingClear(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return;
    }
    uint32_t freed = 0;
    VoiceCloudLlmPendingFrame *node = ctx->pendingHead;
    while (node != nullptr) {
        VoiceCloudLlmPendingFrame *next = node->next;
        if (node->payload != nullptr) {
            std::free(node->payload);
        }
        std::free(node);
        ++freed;
        node = next;
    }
    ctx->pendingHead = nullptr;
    ctx->pendingTail = nullptr;
    ctx->pendingCount = 0;
    if (freed > 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] cleared %u pending pre-session audio batches\r\n", freed);
    }
}

void VoiceCloudLlmPendingDropOldest(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr || ctx->pendingHead == nullptr) {
        return;
    }
    VoiceCloudLlmPendingFrame *head = ctx->pendingHead;
    ctx->pendingHead = head->next;
    if (ctx->pendingHead == nullptr) {
        ctx->pendingTail = nullptr;
    }
    if (head->payload != nullptr) {
        std::free(head->payload);
    }
    std::free(head);
    if (ctx->pendingCount > 0) {
        --ctx->pendingCount;
    }
}

bool VoiceCloudLlmPendingAppend(VoiceCloudLlmContext *ctx, const uint8_t *data, uint32_t len)
{
    if (ctx == nullptr || data == nullptr || len == 0) {
        return false;
    }
    while (ctx->pendingCount >= VOICE_CLOUD_LLM_PENDING_FRAMES_MAX) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] pending pre-session buffer full (%u), dropping oldest\r\n",
            ctx->pendingCount);
        VoiceCloudLlmPendingDropOldest(ctx);
    }
    VoiceCloudLlmPendingFrame *node =
        static_cast<VoiceCloudLlmPendingFrame *>(std::malloc(sizeof(VoiceCloudLlmPendingFrame)));
    if (node == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] pending node alloc fail\r\n");
        return false;
    }
    node->next = nullptr;
    node->payloadLen = len;
    node->payload = static_cast<uint8_t *>(std::malloc(len));
    if (node->payload == nullptr) {
        std::free(node);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] pending payload alloc fail, len=%u\r\n", len);
        return false;
    }
    (void)std::memcpy(node->payload, data, len);
    if (ctx->pendingTail == nullptr) {
        ctx->pendingHead = node;
    } else {
        ctx->pendingTail->next = node;
    }
    ctx->pendingTail = node;
    ++ctx->pendingCount;
    return true;
}

/* Forward declaration so the flusher can call HandleAudioFrame's send path. */
bool VoiceCloudLlmSendOneAudioBatch(const uint8_t *payload, uint32_t payloadLen);

uint32_t VoiceCloudLlmPendingFlush(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return 0;
    }
    uint32_t flushed = 0;
    while (ctx->pendingHead != nullptr) {
        VoiceCloudLlmPendingFrame *node = ctx->pendingHead;
        ctx->pendingHead = node->next;
        if (ctx->pendingHead == nullptr) {
            ctx->pendingTail = nullptr;
        }
        if (node->payload != nullptr && node->payloadLen > 0) {
            if (VoiceCloudLlmSendOneAudioBatch(node->payload, node->payloadLen)) {
                ++flushed;
            } else {
                /* On send failure, free the rest of the queue and bail —
                 * caller will run ReturnToWakeup. */
                std::free(node->payload);
                std::free(node);
                ctx->pendingCount = 0;
                VoiceCloudLlmPendingClear(ctx);
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] pending flush aborted after %u batches\r\n", flushed);
                return flushed;
            }
        }
        if (node->payload != nullptr) {
            std::free(node->payload);
        }
        std::free(node);
        if (ctx->pendingCount > 0) {
            --ctx->pendingCount;
        }
    }
    ctx->pendingTail = nullptr;
    ctx->pendingCount = 0;
    if (flushed > 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] flushed %u pre-session audio batches to cloud\r\n", flushed);
    }
    return flushed;
}

void VoiceCloudLlmReturnToWakeup()
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] ReturnToWakeup, localWakeup=%d\r\n", ctx->localWakeupEnabled ? 1 : 0);
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_RETURN_WAKEUP);
    VoiceCloudLlmDrainQueuedAudioFrames(ctx);
    VoiceCloudLlmPendingClear(ctx);
    ctx->playerStartFailedThisTurn = false;
    ctx->idleListeningStartTick = 0;

    /*
     * Order matters here. capture and ASR share AUDIO_STREAM_VOICE_ASSISTANT
     * AudioInterrupt — capture's Deactivate must happen before ASR's Init.
     */
    VoiceCloudLlmAudioCaptureStop();
    VoiceCloudLlmAudioPlayerStop();

    const int32_t wsRet = VoiceCloudLlmWsClose();
    if (wsRet != 0) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm ws close fail, ret=%d\r\n", wsRet);
    }

    VoiceCloudLlmStopAndDeinitAsr();

    if (ctx->localWakeupEnabled) {
        const int32_t wakeRet = VoiceCloudLlmStartWakeupRecognition();
        if (wakeRet != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm wakeup restart fail, ret=%d\r\n",
                wakeRet);
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
            return;
        }
    }
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP);
}

bool VoiceCloudLlmConnectToCloud()
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_WAKEUP_DETECTED);
    VoiceCloudLlmStopAndDeinitAsr();

    /*
     * Early mic start: bring up capture *before* the WS handshake so the
     * user's first words land in the pending buffer instead of being lost
     * to the 1-2 s of TCP+TLS+chat.created+chat.updated round-trips.
     * HandleAudioFrame routes pre-session batches into ctx->pendingHead;
     * SESSION_UPDATED flushes them in one go.
     *
     * Mic shares the AUDIO_STREAM_VOICE_ASSISTANT AudioInterrupt with ASR,
     * but we already StopAndDeinitAsr above, so the interrupt slot is free.
     */
    const int32_t earlyRet = VoiceCloudLlmAudioCaptureStart(VoiceCloudLlmGetConfig());
    if (earlyRet != 0) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] early capture start fail, ret=%d (will retry after chat.updated)\r\n",
            earlyRet);
        /* Non-fatal — fall through; StartAudioPipelines will try again later. */
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] early capture started, will buffer until chat.updated\r\n");
    }

    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_WS_CONNECTING);
    const VoiceCloudLlmConfig *cfg = VoiceCloudLlmGetConfig();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] connecting to cloud, wsUrl='%s' botId='%s' tokenLen=%u\r\n",
        cfg->wsUrl, cfg->botId, static_cast<uint32_t>(std::strlen(cfg->authToken)));
    int32_t ret = VoiceCloudLlmWsConnect(cfg, VoiceCloudLlmOnWsMessage,
        VoiceCloudLlmOnWsStatus, ctx);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] WS connect failed (ret=0x%x); "
            "network may be unready — confirm WiFi/DNS first\r\n",
            ret);
        return false;
    }

    /* WS open; now wait for server's chat.created before sending chat.update. */
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_CHAT_CREATING);
    return true;
}

bool VoiceCloudLlmSendSessionUpdate()
{
    /*
     * Allocated on heap (not stack) because this runs on the worker thread which
     * has a tight stack budget and is also responsible for capture/playback/WS
     * orchestration. A 1 KB stack frame here was contributing to a measured
     * stack-overflow crash on the original 0x1000 stack.
     */
    VoiceCloudLlmSetState(&g_voiceCloudCtx, VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING);
    char *sessionStart = static_cast<char *>(std::malloc(VOICE_CLOUD_LLM_SESSION_BUFFER_LEN));
    if (sessionStart == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm session buffer alloc fail\r\n");
        return false;
    }
    if (!VoiceCloudLlmBuildSessionStart(VoiceCloudLlmGetConfig(), sessionStart, VOICE_CLOUD_LLM_SESSION_BUFFER_LEN)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm build session start fail\r\n");
        std::free(sessionStart);
        return false;
    }

    const int32_t ret = VoiceCloudLlmWsSendText(sessionStart);
    std::free(sessionStart);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm send chat.update fail, ret=%d\r\n",
            ret);
        return false;
    }
    return true;
}

bool VoiceCloudLlmStartAudioPipelines()
{
    /*
     * Mic is already running from VoiceCloudLlmConnectToCloud (early capture)
     * in the common case. AudioCaptureStart short-circuits if already started,
     * so calling it again is safe and acts as a fallback for the rare case
     * where the early start failed (e.g. ASR teardown was slow).
     *
     * The player shares audio-HAL overlay resources with capture on this MCU,
     * so we still defer opening it until the first audio_delta from the
     * server.
     */
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    int32_t ret = VoiceCloudLlmAudioCaptureStart(VoiceCloudLlmGetConfig());
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm capture start fail, ret=%d\r\n", ret);
        return false;
    }
    /*
     * First round: enter IDLE_LISTENING so the server VAD can detect speech
     * from the flushed pre-session audio. We transition to UPLINK_STREAMING
     * only after server sends speech_started. This mirrors the multi-turn
     * pattern (CHAT_COMPLETED → IDLE_LISTENING → SPEECH_STARTED →
     * UPLINK_STREAMING).
     */
    ctx->idleListeningStartTick = osKernelGetTickCount();
    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_IDLE_LISTENING);

    /*
     * Flush whatever capture batches accumulated during handshake. The WS is
     * already ready (SESSION_UPDATED just arrived). If the flush hits a send
     * error, fall back to error recovery.
     */
    const uint32_t flushed = VoiceCloudLlmPendingFlush(ctx);
    if (ctx->pendingCount > 0) {
        /* Flush aborted on send failure — clear and bail. */
        VoiceCloudLlmPendingClear(ctx);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm pending flush failed mid-way (flushed=%u)\r\n", flushed);
        return false;
    }
    return true;
}

/*
 * Open the audio player, tolerating the short window after the audio HAL
 * exits deep-sleep (typical case: server is generating a reply, MCU dips
 * into deep-sleep, first audio_delta wakes it back up and adec hasn't
 * finished re-claiming the pcm_dec/aef overlay yet — uapi_adec_set_attr
 * returns 0x80005321 for ~tens of ms). We retry a few times with a short
 * delay before declaring this turn lost.
 */
int32_t VoiceCloudLlmAudioPlayerStartWithRetry(const VoiceCloudLlmConfig *config)
{
    const int32_t kRetries = 3;
    const uint32_t kBackoffMs = 50;
    int32_t ret = 0;
    for (int32_t attempt = 0; attempt < kRetries; ++attempt) {
        ret = VoiceCloudLlmAudioPlayerStart(config);
        if (ret == 0) {
            if (attempt > 0) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] player start ok after %d retries\r\n", attempt);
            }
            return 0;
        }
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] player start attempt %d/%d fail, ret=%d, "
            "backing off %u ms\r\n", attempt + 1, kRetries, ret, kBackoffMs);
        osDelay(kBackoffMs);
    }
    return ret;
}

void VoiceCloudLlmHandleServerEvent(const VoiceCloudLlmMsg &msg)
{
    const VoiceCloudLlmServerEvent &event = msg.event;
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    /*
     * AUDIO_DELTA is the high-frequency path (~30+ events per turn) and the
     * player thread already prints a richer per-frame line ([player] #N ...).
     * Logging it here too just buries the diagnostic. Other events are rare
     * and worth a line each.
     */
    if (event.type != VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] server event %d(%s) payloadLen=%u\r\n",
            event.type, VoiceCloudLlmServerEventName(event.type), event.payloadLen);
    }
    switch (event.type) {
        case VOICE_CLOUD_LLM_EVENT_SESSION_CREATED:
            /* chat.created — now safe to send chat.update */
            if (!VoiceCloudLlmSendSessionUpdate()) {
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
                VoiceCloudLlmReturnToWakeup();
            }
            break;
        case VOICE_CLOUD_LLM_EVENT_SESSION_UPDATED:
            /*
             * chat.updated — server accepted our session config.
             * Start capture + flush pre-session audio, then enter IDLE_LISTENING
             * to await server VAD speech_started before transitioning to
             * UPLINK_STREAMING.
             */
            if (!VoiceCloudLlmStartAudioPipelines()) {
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
                VoiceCloudLlmReturnToWakeup();
            }
            break;
        case VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA:
            /*
             * First audio_delta of this turn — release the mic overlay (in case
             * speech_stopped did not fire), then bring up the player. Once
             * we're already in DOWNLINK_PLAYING the renderer is up and the mic
             * is released, so skip the idempotent re-calls (which would log
             * "capture stop skipped, not running" / "player start skipped,
             * already started" on every delta — 30+ per turn).
             */
            if (VoiceCloudLlmGetState(ctx) != VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING) {
                VoiceCloudLlmAudioCaptureStop();
                if (ctx->playerStartFailedThisTurn) {
                    /* Already tried this turn and lost — drop silently until ReturnToWakeup. */
                    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING);
                    break;
                }
                const int32_t startRet = VoiceCloudLlmAudioPlayerStartWithRetry(VoiceCloudLlmGetConfig());
                if (startRet != 0) {
                    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE,
                        "voice cloud llm player start fail, ret=%d (dropping audio_delta, "
                        "suppressing further attempts this turn)\r\n", startRet);
                    ctx->playerStartFailedThisTurn = true;
                    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING);
                    break;
                }
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING);
            } else if (ctx->playerStartFailedThisTurn) {
                /* Earlier delta this turn failed to start the player; keep dropping. */
                break;
            }
            if (event.payload != nullptr && event.payloadLen > 0) {
                const int32_t ret = VoiceCloudLlmAudioPlayerFeed(event.payload, event.payloadLen);
                if (ret != 0) {
                    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm player feed fail, ret=%d\r\n",
                        ret);
                }
            }
            break;
        case VOICE_CLOUD_LLM_EVENT_SPEECH_STOPPED:
            /*
             * Drop stale SPEECH_STOPPED that arrived after we already returned
             * to wakeup (or are otherwise no longer streaming). Without this
             * guard, a queued event delivered post-ReturnToWakeup will start
             * the player from IDLE_WAKEUP, get 0x80005321, and burn 150 ms on
             * the retry helper for nothing.
             */
            if (VoiceCloudLlmGetState(ctx) != VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] SPEECH_STOPPED ignored, state=%d\r\n",
                    static_cast<int>(VoiceCloudLlmGetState(ctx)));
                break;
            }
            /*
             * server_vad detected end-of-utterance. We do NOT send
             * input_audio_buffer.complete — server commits automatically.
             *
             * Three things to clean up immediately:
             *   1. Move state to AWAITING_RESPONSE. This shuts the producer-side
             *      gate (CanSendAudioFrame) and the worker-side send path
             *      (HandleAudioFrame) so any AUDIO_FRAME batches already in
             *      the queue or still in flight on the capture thread are
             *      dropped instead of being uploaded as silence. This is the
             *      "we kept sending 6s of input but Coze finished thinking
             *      at 3s" symptom: capture-thread + queue + TLS pipe can
             *      add up to ~1-2 s of post-VAD silence if we keep the
             *      uplink open until INPUT_AUDIO_DONE.
             *   2. Drain any AUDIO_FRAME messages still queued for the
             *      worker — they were enqueued before the gate closed and
             *      would otherwise be sent on the next worker tick. We're
             *      already on the worker thread inside HandleServerEvent,
             *      so a non-blocking drain here is safe (no races).
             *   3. Release the mic. AudioStreamIn doesn't preempt the
             *      capture thread; it returns once stopRequested is set
             *      and the in-progress AudioStreamInObtainBuffer returns.
             *
             * Do NOT start the player yet: the gap between SPEECH_STOPPED
             * and the first AUDIO_DELTA can be many seconds (text_delta
             * arrives first), and an opened renderer that sits idle that
             * long goes stale — subsequent StreamWrite calls return
             * 0xffffffff forever. Player start is deferred to the
             * AUDIO_DELTA handler, where the renderer is fresh.
             */
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE);
            VoiceCloudLlmDrainQueuedAudioFrames(ctx);
            VoiceCloudLlmAudioCaptureStop();
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                "[voice_cloud_llm] SPEECH_STOPPED: uplink gated, queue drained, "
                "capture released; player starts on first AUDIO_DELTA\r\n");
            break;
        case VOICE_CLOUD_LLM_EVENT_INPUT_AUDIO_DONE:
            /* Server's commit ACK; entering wait-for-response phase. */
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE);
            break;
        case VOICE_CLOUD_LLM_EVENT_AUDIO_STREAM_COMPLETED:
            /* This turn's audio is fully sent; drain the player but keep WS for follow-up. */
            VoiceCloudLlmAudioPlayerDrainThenStop();
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE);
            break;
        case VOICE_CLOUD_LLM_EVENT_CHAT_COMPLETED:
            /*
             * This turn finished, but the WS session is still alive for multi-turn.
             * Drain the player (if not already done by AUDIO_STREAM_COMPLETED), then
             * restart capture so the server can VAD-detect the next utterance.
             * Transition to IDLE_LISTENING; idle timeout or speech_started will
             * determine whether we continue or return to wakeup.
             */
            VoiceCloudLlmAudioPlayerDrainThenStop();
            if (VoiceCloudLlmAudioCaptureStart(VoiceCloudLlmGetConfig()) != 0) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "voice cloud llm capture restart for multi-turn fail\r\n");
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
                VoiceCloudLlmReturnToWakeup();
                break;
            }
            ctx->idleListeningStartTick = osKernelGetTickCount();
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_IDLE_LISTENING);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                "[voice_cloud_llm] turn complete, listening for next utterance "
                "(idle timeout=%ums)\r\n", VOICE_CLOUD_LLM_IDLE_TIMEOUT_MS);
            break;
        case VOICE_CLOUD_LLM_EVENT_SERVER_ERROR:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm server error\r\n");
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
            VoiceCloudLlmReturnToWakeup();
            break;
        case VOICE_CLOUD_LLM_EVENT_SPEECH_STARTED:
            /*
             * Cloud VAD detected a new utterance. If we're between turns
             * (IDLE_LISTENING), transition to streaming — the next turn has
             * begun. Clear the per-turn player failure flag so a previous
             * turn's adec issue doesn't suppress audio in this turn.
             */
            if (VoiceCloudLlmGetState(ctx) == VOICE_CLOUD_LLM_STATE_IDLE_LISTENING) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] speech started, new turn begins\r\n");
                ctx->playerStartFailedThisTurn = false;
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING);
            }
            break;
        case VOICE_CLOUD_LLM_EVENT_TEXT_DELTA:
        case VOICE_CLOUD_LLM_EVENT_NONE:
        default:
            break;
    }
}

bool VoiceCloudLlmSendOneAudioBatch(const uint8_t *payload, uint32_t payloadLen)
{
    if (payload == nullptr || payloadLen == 0) {
        return true;
    }
    /* Base64 inflates by 4/3, plus JSON envelope. ~96 bytes is enough for the
     * fixed input_audio_buffer.append shape. */
    const uint32_t bufferLen = ((payloadLen + 2U) / 3U) * 4U + 96U;
    char *audioAppend = static_cast<char *>(std::malloc(bufferLen));
    if (audioAppend == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm audio append alloc fail, len=%u\r\n", bufferLen);
        return false;
    }
    if (!VoiceCloudLlmBuildAudioAppend(payload, payloadLen, audioAppend, bufferLen)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm build audio append fail, len=%u\r\n", payloadLen);
        std::free(audioAppend);
        return false;
    }
    const int32_t ret = VoiceCloudLlmWsSendText(audioAppend);
    std::free(audioAppend);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm send audio frame fail, ret=%d\r\n", ret);
        return false;
    }
    return true;
}

void VoiceCloudLlmHandleAudioFrame(const VoiceCloudLlmMsg &msg)
{
    if (msg.payload == nullptr || msg.payloadLen == 0) {
        return;
    }

    /*
     * Pre-session capture path: WS handshake / chat.created / chat.updated
     * are still in flight, so we can't send yet. Buffer the batch and flush
     * it the moment SESSION_UPDATED arrives. This is what cuts the perceived
     * wakeup→record latency — mic is already collecting speech while the
     * network is still negotiating.
     */
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    const VoiceCloudLlmState curState = VoiceCloudLlmGetState(ctx);
    if (curState == VOICE_CLOUD_LLM_STATE_WAKEUP_DETECTED ||
        curState == VOICE_CLOUD_LLM_STATE_WS_CONNECTING ||
        curState == VOICE_CLOUD_LLM_STATE_CHAT_CREATING ||
        curState == VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING) {
        const bool buffered = VoiceCloudLlmPendingAppend(ctx, msg.payload, msg.payloadLen);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] pre-session capture batch buffered=%d len=%u state=%s pending=%u\r\n",
            buffered ? 1 : 0, msg.payloadLen, VoiceCloudLlmStateName(curState), ctx->pendingCount);
        return;
    }

    /*
     * Once the WS has died or we've already started error-recovery, more audio
     * frames may still be sitting in the worker queue (the capture thread
     * enqueues every 20 ms; recovery doesn't drain them atomically). If we try
     * to send them we hit WEBSOCKET_NO_CONNECTION every time and re-trigger
     * ReturnToWakeup, which loops forever. Drop silently here — the producer's
     * CanSendAudioFrame check will keep new frames out shortly.
     *
     * IDLE_LISTENING is the multi-turn between-turn state: capture is running,
     * audio is streaming to the server for VAD. The idle→wakeup check runs on
     * every audio frame (~6 Hz) rather than relying on the queue timeout (which
     * would never fire while capture is active).
     */
    if (curState == VOICE_CLOUD_LLM_STATE_IDLE_LISTENING) {
        const uint32_t elapsed = osKernelGetTickCount() - ctx->idleListeningStartTick;
        if (elapsed >= VOICE_CLOUD_LLM_IDLE_TIMEOUT_MS) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                "[voice_cloud_llm] idle timeout (%ums elapsed), returning to wakeup\r\n",
                elapsed);
            VoiceCloudLlmReturnToWakeup();
            return;
        }
        /* Still within idle window — keep streaming audio for server VAD. */
        if (!VoiceCloudLlmSendOneAudioBatch(msg.payload, msg.payloadLen)) {
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
            VoiceCloudLlmReturnToWakeup();
        }
        return;
    }

    if (curState != VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING &&
        curState != VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING) {
        return;
    }

    if (!VoiceCloudLlmSendOneAudioBatch(msg.payload, msg.payloadLen)) {
        VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
        VoiceCloudLlmReturnToWakeup();
    }
}

void VoiceCloudLlmHandleResponseTimeout(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return;
    }
    const VoiceCloudLlmState state = VoiceCloudLlmGetState(ctx);
    switch (state) {
        case VOICE_CLOUD_LLM_STATE_WS_CONNECTING:
        case VOICE_CLOUD_LLM_STATE_CHAT_CREATING:
        case VOICE_CLOUD_LLM_STATE_SESSION_CONFIGURING:
        case VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING:
        case VOICE_CLOUD_LLM_STATE_DOWNLINK_PLAYING:
        case VOICE_CLOUD_LLM_STATE_AWAITING_RESPONSE:
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
                "voice cloud llm response timeout, state=%d\r\n", state);
            VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
            VoiceCloudLlmReturnToWakeup();
            return;
        case VOICE_CLOUD_LLM_STATE_IDLE_LISTENING:
            /* Fallback idle timeout — capture may have stalled; return to wakeup. */
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                "voice cloud llm idle timeout (queue silent), returning to wakeup\r\n");
            VoiceCloudLlmReturnToWakeup();
            return;
        default:
            return;
    }
}

void VoiceCloudLlmWorker(void *argument)
{
    VoiceCloudLlmContext *ctx = static_cast<VoiceCloudLlmContext *>(argument);
    VoiceCloudLlmMsg msg;
    while (ctx != nullptr && VoiceCloudLlmIsRunning(ctx)) {
        (void)std::memset(&msg, 0, sizeof(msg));
        const osStatus_t ret = osMessageQueueGet(ctx->queue, &msg, nullptr, VoiceCloudLlmGetQueueWaitMs(ctx));
        if (ret == osErrorTimeout) {
            VoiceCloudLlmHandleResponseTimeout(ctx);
            continue;
        }
        if (ret != osOK) {
            continue;
        }

        if (msg.payload != nullptr) {
            msg.event.payload = msg.payload;
            msg.event.payloadLen = msg.payloadLen;
        }

        switch (msg.type) {
            case VOICE_CLOUD_LLM_MSG_WAKEUP:
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] worker: WAKEUP msg, canAccept=%d state=%s\r\n",
                    VoiceCloudLlmCanAcceptWakeup(ctx) ? 1 : 0,
                    VoiceCloudLlmStateName(VoiceCloudLlmGetState(ctx)));
                if (!VoiceCloudLlmCanAcceptWakeup(ctx)) {
                    break;
                }
                if (!VoiceCloudLlmConnectToCloud()) {
                    VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
                    VoiceCloudLlmReturnToWakeup();
                }
                break;
            case VOICE_CLOUD_LLM_MSG_WS_OPEN:
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] worker: WS_OPEN msg\r\n");
                /* No state transition here — we wait for chat.created instead. */
                break;
            case VOICE_CLOUD_LLM_MSG_SERVER_EVENT:
                VoiceCloudLlmHandleServerEvent(msg);
                break;
            case VOICE_CLOUD_LLM_MSG_AUDIO_FRAME:
                VoiceCloudLlmHandleAudioFrame(msg);
                break;
            case VOICE_CLOUD_LLM_MSG_ERROR: {
                /*
                 * When the WS dies mid-stream we get a burst of ERROR messages —
                 * one from the WsSendText failure, one from OnError, one from
                 * OnClose, plus any in-flight audio frames that fail to send
                 * before the queue drain catches up. The first ERROR runs
                 * ReturnToWakeup and lands us in IDLE_WAKEUP; the rest must be
                 * no-ops or we cycle through teardown/wakeup-restart repeatedly.
                 */
                const VoiceCloudLlmState curState = VoiceCloudLlmGetState(ctx);
                if (curState == VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP) {
                    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
                        "[voice_cloud_llm] worker: ERROR msg ignored, already IDLE_WAKEUP\r\n");
                    break;
                }
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
                    "[voice_cloud_llm] worker: ERROR msg, returning to wakeup (from state=%s)\r\n",
                    VoiceCloudLlmStateName(curState));
                VoiceCloudLlmSetState(ctx, VOICE_CLOUD_LLM_STATE_ERROR_RECOVER);
                VoiceCloudLlmReturnToWakeup();
                break;
            }
            case VOICE_CLOUD_LLM_MSG_STOP:
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] worker: STOP msg, tearing down\r\n");
                VoiceCloudLlmSetRunning(ctx, false);
                VoiceCloudLlmAudioCaptureStop();
                VoiceCloudLlmAudioPlayerStop();
                (void)VoiceCloudLlmWsClose();
                VoiceCloudLlmStopAndDeinitAsr();
                break;
            default:
                break;
        }

        VoiceCloudLlmFreeMsgPayload(&msg);
    }
}

void VoiceCloudLlmAsrListener(AsrEngineEvent event, void *data, uint32_t length)
{
    if (event != ASR_ENGINE_EVENT_VOICE_COMMAND || data == nullptr || length != sizeof(AsrEngineCmdInfo)) {
        return;
    }

    const AsrEngineCmdInfo *cmdInfo = static_cast<const AsrEngineCmdInfo *>(data);
    if (cmdInfo->cmdId != ASR_ENGINE_CMD_WAKEUP_KEYWORD) {
        return;
    }

    if (!VoiceCloudLlmCanAcceptWakeup(&g_voiceCloudCtx)) {
        return;
    }
    if (!VoiceCloudLlmPostSimpleMsg(&g_voiceCloudCtx, VOICE_CLOUD_LLM_MSG_WAKEUP)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm post wakeup fail\r\n");
    }
}

void VoiceCloudLlmOnWsMessage(const uint8_t *data, uint32_t len, bool binary, void *ctx)
{
    VoiceCloudLlmContext *voiceCtx = static_cast<VoiceCloudLlmContext *>(ctx);
    if (voiceCtx != &g_voiceCloudCtx || data == nullptr || len == 0) {
        return;
    }

    VoiceCloudLlmServerEvent event = VoiceCloudLlmParseServerMessage(data, len, binary);
    /*
     * Coze emits a TEXT_DELTA (conversation.message.delta) per token of the
     * model's textual reply, often with an empty `delta` field for control
     * frames. The state machine does not display text, so empty TEXT_DELTAs
     * are pure log noise. Drop them at the boundary instead of paying for
     * a malloc + queue round-trip.
     */
    if (event.type == VOICE_CLOUD_LLM_EVENT_TEXT_DELTA && event.payloadLen == 0) {
        VoiceCloudLlmFreeServerEvent(&event);
        return;
    }
    VoiceCloudLlmMsg msg;
    (void)std::memset(&msg, 0, sizeof(msg));
    msg.type = VOICE_CLOUD_LLM_MSG_SERVER_EVENT;
    msg.event = event;

    if (event.payload != nullptr && event.payloadLen > 0) {
        msg.payload = static_cast<uint8_t *>(std::malloc(event.payloadLen));
        if (msg.payload == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm payload alloc fail, len=%u\r\n",
                event.payloadLen);
            VoiceCloudLlmFreeServerEvent(&event);
            return;
        }
        (void)std::memcpy(msg.payload, event.payload, event.payloadLen);
        msg.payloadLen = event.payloadLen;
        msg.event.payload = msg.payload;
        msg.event.payloadAllocated = false;
    }
    VoiceCloudLlmFreeServerEvent(&event);

    if (!VoiceCloudLlmPostMsg(voiceCtx, msg)) {
        VoiceCloudLlmFreeMsgPayload(&msg);
    }
}

void VoiceCloudLlmOnWsStatus(const uint8_t *data, uint32_t len, bool binary, void *ctx)
{
    (void)data;
    (void)len;
    (void)binary;
    VoiceCloudLlmContext *voiceCtx = static_cast<VoiceCloudLlmContext *>(ctx);
    if (voiceCtx != &g_voiceCloudCtx) {
        return;
    }
    (void)VoiceCloudLlmPostSimpleMsg(voiceCtx, VOICE_CLOUD_LLM_MSG_ERROR);
}

bool VoiceCloudLlmCreateRuntime(VoiceCloudLlmContext *ctx)
{
    MediaMutexAttr mutexAttr = {false};
    ctx->mutex = MediaMutexCreate(&mutexAttr);
    if (ctx->mutex == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm mutex create fail\r\n");
        return false;
    }

    osMessageQueueAttr_t queueAttr;
    (void)std::memset(&queueAttr, 0, sizeof(queueAttr));
    queueAttr.name = "VOICE_CLOUD_LLM_MSG";
    ctx->queue = osMessageQueueNew(VOICE_CLOUD_LLM_QUEUE_DEPTH, sizeof(VoiceCloudLlmMsg), &queueAttr);
    if (ctx->queue == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm queue create fail\r\n");
        MediaMutexDestroy(&ctx->mutex);
        ctx->mutex = nullptr;
        return false;
    }

    VoiceCloudLlmSetRunning(ctx, true);
    osThreadAttr_t attr = {"voice_cloud_llm", osThreadJoinable, nullptr, 0, nullptr,
        VOICE_CLOUD_LLM_STACK_SIZE, VOICE_CLOUD_LLM_THREAD_PRIORITY, 0, 0};
    ctx->workerThread = osThreadNew(VoiceCloudLlmWorker, ctx, &attr);
    if (ctx->workerThread == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm worker create fail\r\n");
        VoiceCloudLlmSetRunning(ctx, false);
        (void)osMessageQueueDelete(ctx->queue);
        ctx->queue = nullptr;
        MediaMutexDestroy(&ctx->mutex);
        ctx->mutex = nullptr;
        return false;
    }
    return true;
}

bool VoiceCloudLlmInitRuntime(VoiceCloudLlmContext *ctx, bool localWakeupEnabled)
{
    if (ctx == nullptr) {
        return false;
    }
    (void)std::memset(ctx, 0, sizeof(*ctx));
    ctx->state = VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP;
    ctx->localWakeupEnabled = localWakeupEnabled;
    if (!VoiceCloudLlmCreateRuntime(ctx)) {
        VoiceCloudLlmDeleteRuntime(ctx);
        return false;
    }
    return true;
}

void VoiceCloudLlmDestroyRuntime(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return;
    }
    if (ctx->queue != nullptr) {
        (void)VoiceCloudLlmPostStopMsg(ctx);
    }
    if (ctx->workerThread != nullptr) {
        (void)osThreadJoin(ctx->workerThread);
    }
    VoiceCloudLlmDeleteRuntime(ctx);
}

void VoiceCloudLlmDeleteRuntime(VoiceCloudLlmContext *ctx)
{
    if (ctx == nullptr) {
        return;
    }
    if (ctx->queue != nullptr) {
        VoiceCloudLlmMsg msg;
        while (osMessageQueueGet(ctx->queue, &msg, nullptr, 0) == osOK) {
            VoiceCloudLlmFreeMsgPayload(&msg);
        }
        (void)osMessageQueueDelete(ctx->queue);
        ctx->queue = nullptr;
    }
    if (ctx->mutex != nullptr) {
        MediaMutexDestroy(&ctx->mutex);
        ctx->mutex = nullptr;
    }
    VoiceCloudLlmPendingClear(ctx);
    ctx->workerThread = nullptr;
    ctx->started = false;
    ctx->running = false;
    ctx->asrStarted = false;
    ctx->localWakeupEnabled = false;
    ctx->state = VOICE_CLOUD_LLM_STATE_IDLE_WAKEUP;
}

} // namespace

extern "C" errcode_t VoiceCloudLlmStart(void)
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] Start requested, alreadyStarted=%d\r\n", ctx->started ? 1 : 0);
    if (ctx->started) {
        return ERRCODE_SUCC;
    }

    const errcode_t precheckRet = VoiceCloudLlmPrecheckConfig(VoiceCloudLlmGetConfig());
    if (precheckRet != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] Start precheck fail, ret=0x%x\r\n", precheckRet);
        return precheckRet;
    }

    if (!VoiceCloudLlmInitRuntime(ctx, true)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] Start runtime init fail\r\n");
        return ERRCODE_FAIL;
    }

    const int32_t ret = VoiceCloudLlmStartWakeupRecognition();
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "[voice_cloud_llm] Start wakeup recognition fail, ret=%d\r\n", ret);
        VoiceCloudLlmDestroyRuntime(ctx);
        return ERRCODE_FAIL;
    }

    ctx->started = true;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] Start ok, awaiting wakeup\r\n");
    return ERRCODE_SUCC;
}

extern "C" errcode_t VoiceCloudLlmStop(void)
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "[voice_cloud_llm] Stop requested, started=%d\r\n", ctx->started ? 1 : 0);
    if (!ctx->started) {
        return ERRCODE_SUCC;
    }

    if (!VoiceCloudLlmPostStopMsg(ctx)) {
        VoiceCloudLlmSetRunning(ctx, false);
        VoiceCloudLlmAudioCaptureStop();
        VoiceCloudLlmAudioPlayerStop();
        (void)VoiceCloudLlmWsClose();
        VoiceCloudLlmStopAndDeinitAsr();
    }

    if (ctx->workerThread != nullptr) {
        (void)osThreadJoin(ctx->workerThread);
    }
    VoiceCloudLlmDeleteRuntime(ctx);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[voice_cloud_llm] Stop done\r\n");
    return ERRCODE_SUCC;
}

extern "C" errcode_t VoiceCloudLlmOnWakeup(void)
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
        "voice cloud llm wakeup request, started=%d, localWakeup=%d\r\n",
        ctx->started, ctx->localWakeupEnabled);

    if (!ctx->started) {
        const errcode_t precheckRet = VoiceCloudLlmPrecheckConfig(VoiceCloudLlmGetConfig());
        if (precheckRet != ERRCODE_SUCC) {
            return precheckRet;
        }
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm direct wakeup test mode init (no local ASR)\r\n");
        if (!VoiceCloudLlmInitRuntime(ctx, false)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm direct wakeup runtime init fail\r\n");
            return ERRCODE_FAIL;
        }
        ctx->started = true;
    }

    if (!VoiceCloudLlmPostSimpleMsg(ctx, VOICE_CLOUD_LLM_MSG_WAKEUP)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE,
            "voice cloud llm wakeup post fail, started=%d, running=%d, queueReady=%d\r\n",
            ctx->started, VoiceCloudLlmIsRunning(ctx), ctx->queue != nullptr);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

extern "C" bool VoiceCloudLlmIsWakeupTestMode(void)
{
    return g_voiceCloudCtx.started && !g_voiceCloudCtx.localWakeupEnabled;
}

extern "C" bool VoiceCloudLlmIsIdleListening(void)
{
    return VoiceCloudLlmGetState(&g_voiceCloudCtx) == VOICE_CLOUD_LLM_STATE_IDLE_LISTENING;
}

extern "C" bool VoiceCloudLlmIsUplinkStreaming(void)
{
    return VoiceCloudLlmGetState(&g_voiceCloudCtx) == VOICE_CLOUD_LLM_STATE_UPLINK_STREAMING;
}

extern "C" int32_t VoiceCloudLlmPostAudioFrame(const uint8_t *data, uint32_t len)
{
    VoiceCloudLlmContext *ctx = &g_voiceCloudCtx;
    if (data == nullptr || len == 0 || !VoiceCloudLlmCanSendAudioFrame(ctx)) {
        return ERRCODE_FAIL;
    }

    VoiceCloudLlmMsg msg;
    (void)std::memset(&msg, 0, sizeof(msg));
    msg.type = VOICE_CLOUD_LLM_MSG_AUDIO_FRAME;
    msg.payload = static_cast<uint8_t *>(std::malloc(len));
    if (msg.payload == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cloud llm audio frame alloc fail, len=%u\r\n", len);
        return ERRCODE_FAIL;
    }
    (void)std::memcpy(msg.payload, data, len);
    msg.payloadLen = len;

    if (!VoiceCloudLlmPostMsg(ctx, msg)) {
        VoiceCloudLlmFreeMsgPayload(&msg);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
