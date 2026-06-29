/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio manager c wrapper
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#include "a2dp_sink_service.h"

#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "securec.h"
#include "audio_errors.h"
#include "audio_manager.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "audio_base_type.h"
#include "media_thread_adapt.h"
#include "bts_br_gap.h"
#include "bts_a2dp_source.h"
#include "bts_a2dp_sink.h"
#include "bts_a2dp_source.h"
#include "media_mem.h"
#include "media_log.h"

using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MODULE_NAME "AudioManagerCWrapper"
#define BUFFER_SIZE            120
#define VQE_PREFIX_LEN         18
#define PEQ_PREFIX_LEN         50
#define NV_AUDIO_PEQ_PARAM_SIZE 41
#define NV_AUDIO_VQE_PARAM_SIZE 53
#define PEQ_PARAM_SIZE         (NV_AUDIO_PEQ_PARAM_SIZE * sizeof(uint32_t))
#define VQE_PARAM_SIZE         (NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t))
#define SEA_BUFFER_SIZE        (VQE_PARAM_SIZE + VQE_PREFIX_LEN)
#define AEF_BUFFER_SIZE        (PEQ_PARAM_SIZE + PEQ_PREFIX_LEN)
#define PEQ_PREFIX_LEN         50
#define MEDIA_UNUSED(x) ((void)(x))

static AudioManager &g_amInstance = AudioManager::GetInstance();
static MediaMutexHandle g_a2dpSinkMutex = nullptr;
constexpr uint8_t AUDIO_APP_ID = 0x11;

typedef struct {
    AudioSession sessionId;
    AudioInterrupt interrupt;
    std::shared_ptr<AudioStreamOut> a2dpAsohandle;
    std::shared_ptr<AudioStreamIn> a2dpAsihandle;
    bool isA2dpSinkPause;
    bool isA2dpSinkStopped;
    bool isA2dpSinkResumed;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
} AudioManagerA2dpContext;

typedef struct {
    bd_addr_t bdAddr;
    uint8_t   snkConnState;
    uint8_t   snkStreamState;
    uint8_t   srcConnState;
    uint8_t   srcStreamState;
    uint32_t  streamHandle;
    MediaMutexHandle stateMutex;
} AudioManagerA2dpState;

static AudioManagerA2dpContext *g_a2dpCtx = nullptr;
static AudioRendererConfig  g_a2dpRenderConfig = {};

static void A2dpSinkConnStateCbk(const bd_addr_t *bdAddr, uint8_t connState);
static void A2dpSinkStreamStateCbk(const bd_addr_t *bdAddr, uint32_t streamHandle, uint8_t state);
static void A2dpSrcConnStateCbk(const bd_addr_t *bdAddr, uint8_t connState);
static void A2dpSrcStreamStateCbk(const bd_addr_t *bdAddr, uint32_t stream_handle, uint8_t state);
extern errcode_t a2dp_snk_register_callbacks_multi (uint8_t app_id, a2dp_snk_callbacks_t *func);
extern errcode_t a2dp_src_register_callbacks_multi (uint8_t app_id, a2dp_src_callbacks_t *func);

static a2dp_snk_callbacks_t g_sinkCallbacks = {
    .connectstate_changed_cb = A2dpSinkConnStateCbk,
    .start_stream_request_cb = nullptr,
    .configuration_changed_cb = nullptr,
    .stream_state_changed_cb = A2dpSinkStreamStateCbk,
};

static a2dp_src_callbacks_t g_sourceCallbacks = {
    .connectstate_changed_cb = A2dpSrcConnStateCbk,
    .configuration_changed_cb = nullptr,
    .stream_state_changed_cb = A2dpSrcStreamStateCbk,
    .start_stream_request_cb = nullptr,
};

static AudioManagerA2dpState g_btA2dpState = {};

class AudioManagerA2dpInterruptListener : public InterruptListener {
public:
    explicit AudioManagerA2dpInterruptListener(AudioManagerA2dpContext *context)
    {
        a2dpContext = context;
    };
    ~AudioManagerA2dpInterruptListener() override {} ;

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("type:%d hint:%d", type, hint);
        if (g_a2dpCtx == nullptr || a2dpContext == nullptr) {
            MEDIA_ERR_LOG("context is NULL");
            return;
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("a2dp sink INTERRUPT_HINT_PAUSE");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkPause = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("a2dp sink INTERRUPT_HINT_RESUME");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkResumed = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("a2dp sink INTERRUPT_HINT_STOP");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkStopped = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        }
    }
private:
    AudioManagerA2dpContext *a2dpContext = nullptr;
};

static void A2dpSinkConnStateCbk(const bd_addr_t *bdAddr, uint8_t connState)
{
    MediaMutexLock(g_btA2dpState.stateMutex);
    if (memcpy_s(&g_btA2dpState.bdAddr, sizeof(bd_addr_t), bdAddr, sizeof(bd_addr_t)) != 0) {
        MEDIA_ERR_LOG("memcpy fail");
        MediaMutexUnLock(g_btA2dpState.stateMutex);
        return;
    }
    MEDIA_INFO_LOG("a2dp_sink connect state[%x]", connState);
    g_btA2dpState.snkConnState = connState;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
}

static void A2dpSinkStreamStateCbk(const bd_addr_t *bdAddr, uint32_t streamHandle, uint8_t state)
{
    MEDIA_UNUSED(bdAddr);
    MEDIA_INFO_LOG("a2dp_sink stream state[%x]", state);
    MediaMutexLock(g_btA2dpState.stateMutex);
    g_btA2dpState.streamHandle = streamHandle;
    g_btA2dpState.snkStreamState = state;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
}

static void A2dpSrcConnStateCbk(const bd_addr_t *bdAddr, uint8_t connState)
{
    MEDIA_UNUSED(bdAddr);
    MEDIA_INFO_LOG("a2dp_src connect state[%x]", connState);
    MediaMutexLock(g_btA2dpState.stateMutex);
    g_btA2dpState.srcConnState = connState;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
}

static void A2dpSrcStreamStateCbk(const bd_addr_t *bdAddr, uint32_t streamHandle, uint8_t state)
{
    MEDIA_UNUSED(bdAddr);
    MEDIA_INFO_LOG("a2dp_src stream state[%x]", state);
    MediaMutexLock(g_btA2dpState.stateMutex);
    g_btA2dpState.streamHandle = streamHandle;
    g_btA2dpState.srcStreamState = state;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
}

static bool CheckA2dpSinkConnectState(void)
{
    MediaMutexLock(g_btA2dpState.stateMutex);
    bool ret = g_btA2dpState.snkStreamState == A2DP_STREAM_START ? true : false;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
    return ret;
}
static bool CheckA2dpSourceConnectState(void)

{
    MediaMutexLock(g_btA2dpState.stateMutex);
    bool ret = g_btA2dpState.srcStreamState >= A2DP_STREAM_OPENED &&
        g_btA2dpState.srcStreamState < A2DP_STREAM_CLOSED ? true : false;
    MediaMutexUnLock(g_btA2dpState.stateMutex);
    return ret;
}

static int32_t AudioManagerA2dpSinkContextInit(void)
{
    if (g_a2dpCtx != nullptr) {
        MEDIA_INFO_LOG("a2dp sink context has been inited!");
        return AUDIO_SUCCESS;
    }
    g_a2dpCtx = new AudioManagerA2dpContext;
    if (g_a2dpCtx == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return AUDIO_ERROR;
    }
    g_a2dpCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_a2dpCtx->a2dpAsohandle = nullptr;
    g_a2dpCtx->isA2dpSinkPause = false;
    g_a2dpCtx->isA2dpSinkStopped = false;
    g_a2dpCtx->isA2dpSinkResumed = false;
    g_a2dpCtx->mutex = MediaMutexCreate(nullptr);
    if (g_a2dpCtx->mutex == nullptr) {
        MEDIA_ERR_LOG("create schmutex failed");
        goto EXIT2;
    }
    g_a2dpCtx->cond = MediaThreadCondCreate();
    if (g_a2dpCtx->cond == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        goto EXIT1;
    }
    return AUDIO_SUCCESS;
EXIT1:
    (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
EXIT2:
    MediaMemFree(g_a2dpCtx);
    g_a2dpCtx = nullptr;
    return AUDIO_ERROR;
}

int32_t AudioManagerA2dpSinkSetParam(AudioFormatConfig config)
{
    g_a2dpRenderConfig.audioFormat = config.audioFormat;
    g_a2dpRenderConfig.sampleRate = config.sampleRate;
    g_a2dpRenderConfig.channelCount = config.channelCount;
    g_a2dpRenderConfig.streamType = AUDIO_STREAM_A2DP_MUSIC;
    g_a2dpRenderConfig.linkDir = AUDIO_DOWN_LINK;
    g_a2dpRenderConfig.bitWidth = BIT_WIDTH_16;
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkSetAefParam(int32_t effectType, const char *param, uint32_t length)
{
    if (param == nullptr) {
        MEDIA_ERR_LOG("aef param null");
        return AUDIO_ERROR;
    }
    if (g_a2dpCtx == nullptr || g_a2dpCtx->a2dpAsohandle == nullptr) {
        MEDIA_ERR_LOG("aso handle is null!");
        return AUDIO_ERROR;
    }
    char *aefBuf = (char *)MediaMemMalloc(AEF_BUFFER_SIZE);
    if (aefBuf == nullptr) {
        MEDIA_ERR_LOG("malloc fail");
        return AUDIO_ERROR;
    }
    if (memset_s(aefBuf, AEF_BUFFER_SIZE, 0, AEF_BUFFER_SIZE) != EOK) {
        MEDIA_ERR_LOG("memset_s fail");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_SIZE, "%s = %d#%u:", "setaefparam", effectType, length);
    if (prefixLen < 0) {
        MEDIA_ERR_LOG("sprintf_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefBuf + prefixLen, AEF_BUFFER_SIZE - prefixLen, param, length) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_a2dpCtx->a2dpAsohandle->SetParam(aefBuf, AEF_BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("set param failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    MediaMemFree(aefBuf);
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkSetAefEnable(int32_t effectType, bool enable)
{
    char buf[BUFFER_SIZE] = { 0 };
    int32_t ret = sprintf_s(buf, BUFFER_SIZE, "%s = %d#%d", "aefenable", enable, effectType);
    if (ret < 0) {
        MEDIA_ERR_LOG("sprintf_s failed");
        return AUDIO_ERROR;
    }
    if (g_a2dpCtx == nullptr || g_a2dpCtx->a2dpAsohandle == nullptr) {
        MEDIA_ERR_LOG("aso handle is null!");
        return AUDIO_ERROR;
    }
    ret = g_a2dpCtx->a2dpAsohandle->SetParam(buf, BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("set aso param failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static void WaitA2dpStartWhenDelay(AudioManagerA2dpContext *a2dpContext)
{
    while (true) {
        if (a2dpContext->isA2dpSinkResumed ||
            a2dpContext->isA2dpSinkStopped) {
            break;
        }
        MEDIA_INFO_LOG("a2dp sink delay start");
        MediaThreadCondWait(a2dpContext->cond, a2dpContext->mutex);
        MEDIA_INFO_LOG("a2dp sink delay end");
    }
}

static int32_t DisConnectBluetoothA2dpDevice(void)
{
    if (CheckA2dpSourceConnectState()) {
        int32_t ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_A2DP,
            AUDIO_DEVICE_DISCONNECTED);
        if (ret != AUDIO_SUCCESS) {
            MEDIA_INFO_LOG("set OUT_BLUETOOTH_A2DP disConnected fail.");
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t ConnectBluetoothA2dpDevice(void)
{
    if (CheckA2dpSourceConnectState()) {
        int32_t ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_A2DP,
            AUDIO_DEVICE_CONNECTED);
        if (ret != AUDIO_SUCCESS) {
            MEDIA_INFO_LOG("set OUT_BLUETOOTH_A2DP connected fail.");
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSessionInitInner(AudioManagerA2dpContext *a2dpContext)
{
    AudioSession sessionID = g_amInstance.MakeSessionId();
    if (sessionID == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("get sessionid fail.");
        return AUDIO_ERROR;
    }
    g_a2dpRenderConfig.sessionID = sessionID;
    std::shared_ptr<AudioManagerA2dpInterruptListener> listener =
        std::make_shared<AudioManagerA2dpInterruptListener>(a2dpContext);
    if (listener == nullptr || listener.get() == nullptr) {
        MEDIA_ERR_LOG("listener is nullptr");
        return AUDIO_ERROR;
    }
    a2dpContext->interrupt = { AUDIO_STREAM_A2DP_MUSIC, sessionID, listener };
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSessionInit(AudioManagerA2dpContext *a2dpContext)
{
    MEDIA_INFO_LOG("process in");
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audio manager init failed");
        return AUDIO_ERROR;
    }
    int32_t ret = 0;
    while (true) {
        ret = AudioManagerA2dpSessionInitInner(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            return AUDIO_ERROR;
        }
        /* After the delayed stream is woken up, the focus needs to be applied for again
       because the focus may be outdated and the policy needs to be obtained again. */
        int32_t strategy = g_amInstance.ActivateAudioInterrupt(a2dpContext->interrupt);
        if (strategy == INTERRUPT_FAILED) {
            MEDIA_ERR_LOG("activate audio interrupt faild");
            return AUDIO_ERROR;
        }
        if (strategy == INTERRUPT_DELAYED) {
            WaitA2dpStartWhenDelay(a2dpContext);
            if (a2dpContext->isA2dpSinkStopped) {
                goto EXIT;
            }
            continue;
        }
        break;
    }
    a2dpContext->isA2dpSinkResumed = false;
    a2dpContext->isA2dpSinkPause = false;
    ret = DisConnectBluetoothA2dpDevice();
    if (ret != AUDIO_SUCCESS) {
        goto EXIT;
    }
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
EXIT:
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
    MEDIA_INFO_LOG("Session init failed!DeactivateAudioInterrupt");
    return AUDIO_ERROR;
}

static void AudioManagerA2dpStreamOutDeinit(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsohandle != nullptr) {
        a2dpContext->a2dpAsohandle->Release();
        a2dpContext->a2dpAsohandle.reset();
        a2dpContext->a2dpAsohandle = nullptr;
    }
}

static int32_t AudioManagerA2dpInit(AudioManagerA2dpContext *a2dpContext, bool isNeedActivate)
{
    MEDIA_INFO_LOG("process in");
    int32_t ret = AUDIO_SUCCESS;
    if (isNeedActivate) {
        ret = AudioManagerA2dpSessionInit(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            MEDIA_ERR_LOG("session init fail ");
            return ret;
        }
    }

    a2dpContext->a2dpAsohandle = std::make_shared<AudioStreamOut>();
    if (a2dpContext->a2dpAsohandle == nullptr || a2dpContext->a2dpAsohandle.get() == nullptr) {
        MEDIA_ERR_LOG("audio stream out is nullptr");
        goto exit;
    }
    ret = a2dpContext->a2dpAsohandle->Init(g_a2dpRenderConfig);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("AudioStreamOutInit fail ");
        AudioManagerA2dpStreamOutDeinit(a2dpContext);
        goto exit;
    }

    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
exit:
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
    return ret;
}

static void AudioManagerA2dpReleaseHandle(AudioManagerA2dpContext *a2dpContext)
{
    AudioManagerA2dpStreamOutDeinit(a2dpContext);
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
}

static int32_t AudioManagerA2dpSinkStart(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsohandle == nullptr) {
        MEDIA_ERR_LOG("asi or aso handle has been destroyed!");
        return AUDIO_SUCCESS;
    }
    int32_t ret = a2dpContext->a2dpAsohandle->Play();
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("AudioStreamOutStart fail.");
        AudioManagerA2dpReleaseHandle(a2dpContext);
        return ret;
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkStop(AudioManagerA2dpContext *a2dpContext)
{
    int32_t ret = a2dpContext->a2dpAsohandle->Stop();
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("AudioStreamOutStop fail.");
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpDeinit(AudioManagerA2dpContext *a2dpContext, bool isNeedDeactivate)
{
    AudioManagerA2dpStreamOutDeinit(a2dpContext);
    if (isNeedDeactivate) {
        ConnectBluetoothA2dpDevice();
        int32_t ret = g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
        if (ret != AUDIO_SUCCESS) {
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkResume(AudioManagerA2dpContext *a2dpContext)
{
    MEDIA_INFO_LOG("process in");
    if (!CheckA2dpSinkConnectState()) {
        MEDIA_ERR_LOG("a2dp sink has been disconnected.not need to resume");
        return AUDIO_SUCCESS;
    }
    if (a2dpContext->a2dpAsohandle != nullptr) {
        MEDIA_ERR_LOG("a2dp is existed, not need to resume.");
        return AUDIO_SUCCESS;
    }
    int32_t ret = AudioManagerA2dpInit(a2dpContext, false);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("a2dp sink init fail.");
        return AUDIO_ERROR;
    }
    ret = AudioManagerA2dpSinkStart(a2dpContext);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("a2dp sink start fail.");
        return AUDIO_ERROR;
    }
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkPause(AudioManagerA2dpContext *a2dpContext)
{
    MEDIA_INFO_LOG("process in");
    if (a2dpContext->a2dpAsohandle == nullptr) {
        MEDIA_INFO_LOG("asi or aso handle has been destroy!");
        return AUDIO_SUCCESS;
    }
    AudioManagerA2dpSinkStop(a2dpContext);
    AudioManagerA2dpDeinit(a2dpContext, false);
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkAllStop(AudioManagerA2dpContext *a2dpContext)
{
    MEDIA_INFO_LOG("process in");
    if (a2dpContext->a2dpAsohandle == nullptr) {
        // The stream has been destroyed. You need to delete the focus.
        AudioManagerA2dpDeinit(a2dpContext, true);
        return AUDIO_SUCCESS;
    }
    AudioManagerA2dpSinkStop(a2dpContext);
    AudioManagerA2dpDeinit(a2dpContext, true);
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
}

static int32_t ProcessA2dpInterrupt(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->isA2dpSinkResumed) {
        int32_t ret = AudioManagerA2dpSinkResume(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            MEDIA_ERR_LOG("a2dp sink resume failed.");
            a2dpContext->isA2dpSinkResumed = false;
            return AUDIO_ERROR;
        }
        a2dpContext->isA2dpSinkResumed = false;
    }
    if (a2dpContext->isA2dpSinkPause) {
        AudioManagerA2dpSinkPause(a2dpContext);
        a2dpContext->isA2dpSinkPause = false;
    }
    return AUDIO_SUCCESS;
}

static void *A2dpSinkThread(void *priv)
{
    AudioManagerA2dpContext *a2dpContext = (AudioManagerA2dpContext *)priv;
    MediaMutexLock(a2dpContext->mutex);

    g_a2dpCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_a2dpCtx->a2dpAsohandle = nullptr;
    g_a2dpCtx->isA2dpSinkPause = false;
    g_a2dpCtx->isA2dpSinkStopped = false;
    g_a2dpCtx->isA2dpSinkResumed = false;
    MEDIA_INFO_LOG("start thread");
    int32_t ret = AudioManagerA2dpInit(a2dpContext, true);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("a2dp sink init fail.");
        goto EXIT;
    }
    ret = AudioManagerA2dpSinkStart(a2dpContext);
    if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("a2dp sink start fail.");
        goto EXIT;
    }
    while (true) {
        if (a2dpContext->isA2dpSinkStopped) {
            AudioManagerA2dpSinkAllStop(a2dpContext);
            break;
        }
        MediaThreadCondWait(a2dpContext->cond, a2dpContext->mutex);
        ret = ProcessA2dpInterrupt(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            goto EXIT;
        }
    }
EXIT:
    MediaMutexUnLock(a2dpContext->mutex);
    MEDIA_INFO_LOG("exit thread");
    return nullptr;
}

int32_t AudioManagerA2dpSinkCreate(void)
{
    MEDIA_INFO_LOG("process in");
    MediaInitStaticMutexLock(&g_a2dpSinkMutex);
    MediaMutexLock(g_a2dpSinkMutex);
    if (g_a2dpCtx != nullptr) {
        MEDIA_ERR_LOG("a2dp sink is exist, need wait some time to create!");
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_SUCCESS;
    }

    int32_t ret = AudioManagerA2dpSinkContextInit();
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_ERROR;
    }

    MediaThreadattr attr = { "A2dpSinkThread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    g_a2dpCtx->taskHandle = MediaThreadCreate(A2dpSinkThread, g_a2dpCtx, &attr);
    if (g_a2dpCtx->taskHandle == nullptr) {
        MEDIA_ERR_LOG("create voice call volte  thread failed");
        (void)MediaThreadCondDestroy(&g_a2dpCtx->cond);
        (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
        MediaMemFree(g_a2dpCtx);
        g_a2dpCtx = nullptr;
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(g_a2dpSinkMutex);
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkDestroy(void)
{
    MEDIA_INFO_LOG("process in");
    MediaInitStaticMutexLock(&g_a2dpSinkMutex);
    MediaMutexLock(g_a2dpSinkMutex);
    if (g_a2dpCtx == nullptr) {
        MEDIA_ERR_LOG("a2dp context is null, execute start command first");
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_SUCCESS;
    }
    MEDIA_INFO_LOG("A2dp sink stop signal one");
    MediaMutexLock(g_a2dpCtx->mutex);
    g_a2dpCtx->isA2dpSinkStopped = true;
    MediaThreadCondSignal(g_a2dpCtx->cond);

    MediaMutexUnLock(g_a2dpCtx->mutex);
    MediaThreadJoin(&g_a2dpCtx->taskHandle);
    (void)MediaThreadCondDestroy(&g_a2dpCtx->cond);
    (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
    delete(g_a2dpCtx);
    g_a2dpCtx = nullptr;
    MediaMutexUnLock(g_a2dpSinkMutex);
    MEDIA_INFO_LOG("process out");
    return AUDIO_SUCCESS;
}

void AudioManagerA2dpSinkRegistCallback()
{
    if (g_btA2dpState.stateMutex != nullptr) {
        return;
    }
    g_btA2dpState.stateMutex = MediaMutexCreate(nullptr);
    if (g_btA2dpState.stateMutex == nullptr) {
        MEDIA_ERR_LOG("create state mutex failed");
        return;
    }
    errcode_t ret = a2dp_snk_register_callbacks_multi(AUDIO_APP_ID, &g_sinkCallbacks);
    if (ret != ERRCODE_SUCC) {
        MEDIA_ERR_LOG("regist A2DP sink music callback failed!");
        return;
    }
    ret = a2dp_src_register_callbacks_multi(AUDIO_APP_ID, &g_sourceCallbacks);
    if (ret != ERRCODE_SUCC) {
        MEDIA_ERR_LOG("regist A2DP source music callback failed!");
        return;
    }
}

#ifdef __cplusplus
}
#endif
