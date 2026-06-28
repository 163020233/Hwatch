/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: primary sub port in
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "primary_sub_port_in.h"
#include "securec.h"
#include "plugin_common.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"
#if defined(BRANDY_PRODUCT_EVB) || defined (HI3322_CHIP_V100)
#include "product_evb_standard.h"
#include "pm_clocks.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME "AudioPrimarySubPortIn"

/* need fix: used for external buffer allocation, internal means samples per frame */
#define AUDIO_HAL_FRAME_COUNT 1024

#define VOLUME_UNIT 100
#define MIC_VOLUME   "micvolume"
#define MIC_TYPE     "mictype"
#define SEA_ENABLE   "seaenable"
#define GET_SEA_PARAM "getseaparam"
#define SET_SEA_PARAM "setseaparam"
#define SET_BIT_RATE  "setbitrate"
#define SET_DB_VOLUME "setdbvol"
#define GET_DB_VOLUME "getdbvol"
#define SET_DENOISE_ENABLE "setdenoisenable"
#define GET_DENOISE_ENABLE "getdenoisenable"
#define SET_LOWCUT_ENABLE "setlowcutenable"
#define GET_LOWCUT_ENABLE "getlowcutenable"
#define SET_HIFI_ENABLE "sethifienable"
#define GET_HIFI_ENABLE "gethifienable"
#define SET_VOICE_ENHANCE_ENABLE "setvoiceenhanceenable"
#define GET_VOICE_ENHANCE_ENABLE "getvoiceenhanceenable"
#define ATTACH_BACKEND  "attachbackend"
#define SET_ENCODE_PARAM "setencodeparam"
#define SEA_PARAM_LEN 350
#define DEFAULTE_LEN 128
#define VOLUME_ACCURACY 1000
#define AUDIO_SEA_AFE_VQE_CMD 4
#define AUDIO_SEA_AFE_AHE_CMD 5
#define OGG_HEAD_LEN 4
#define AMRWB_FRAME_SIZE 61
#define DEFAULT_BITRATE (23850)
#define SSCANF_NUM3 3

static const struct {
    uint32_t mode;
    uint32_t bitRate;
} g_amrwbSupportTable[] = {
    {0, 6600},
    {1, 8850},
    {2, 12650},
    {3, 14250},
    {4, 15850},
    {5, 18250},
    {6, 19850},
    {7, 23050},
    {8, 23850}
};


static enum AudioFormat g_supportFormats[] = {
    AUDIO_FORMAT_TYPE_PCM_16_BIT,
    AUDIO_FORMAT_TYPE_MSBC,
};

typedef struct {
    /* track impl */
    AudioPortTrack common;
    /* ai context */
    uint64_t frameIndex;
#ifdef AUDIO_SDK_SUPPORT
    td_handle aiHandle;
    struct AudioTimeStamp aiTimeStamp;
    /* adp context */
    td_handle outputAdpHandle;
    /* sea context */
    td_handle seaHandle;
    /* aenc context */
    td_handle aencHandle;
    uapi_acodec_id aencCodecId;
    td_handle pickupTrackHandle;
#endif
    /* attr context */
    bool isNeedOpenSea;
    uint32_t micType;
    struct PortPluginAttr inputAttr;
    AudioCaptureCallback callback;
    AudioHandle caller;
    MediaHalThreadIdHandle taskHandle;
    MediaHalMutexHandle mutex;
    MediaHalThreadCondHandle condHandle;
    int32_t seaEventType;
    int32_t aiEventType;
    void *param;
    bool taskActive;
    int32_t offset;
} SubPortTrackInContext;

typedef struct {
#ifdef AUDIO_SDK_SUPPORT
    td_handle aiHandle;
#endif
    td_handle reserved;
} SubPortInContext;

typedef struct {
    enum AudioFormat audioFormat;
    uint32_t sampleRate;
    uint32_t channelCount;
    int32_t seaLibId;
} SubPortTrackInCapabilities;

#if !defined(MELODY_DEMO_BMIC_TX) && !defined(MELODY_DEMO_SOUND_BOX)
static SubPortTrackInCapabilities g_caps[] = {
    {AUDIO_FORMAT_TYPE_SILK, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_MP3, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_SBC, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_24_BIT, 48000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT, 48000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT_FLOAT, 48000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_L2HC, 48000, 1, UAPI_SEA_LIB_SEE },
    {AUDIO_FORMAT_TYPE_MSBC, 16000, 1, UAPI_SEA_LIB_SEE },
    {AUDIO_FORMAT_TYPE_AMR_WB, 16000, 1, UAPI_SEA_LIB_SEE },
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 8000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT, 48000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_SBC, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_OPUS, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_L2HC_VOICE, 16000, 1, UAPI_SEA_LIB_SEE},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 8000, 1, UAPI_SEA_LIB_AHE},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 16000, 1, UAPI_SEA_LIB_AHE},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, 48000, 1, UAPI_SEA_LIB_AHE},
    {AUDIO_FORMAT_TYPE_PCM_24_BIT, 48000, 1, UAPI_SEA_LIB_AHE},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT, 48000, 1, UAPI_SEA_LIB_AHE},
    {AUDIO_FORMAT_TYPE_OPUS, 16000, 1, UAPI_SEA_LIB_AHE},
};

static bool IsSupported(const struct PortPluginAttr *inputAttr)
{
    int32_t seaLibId = UAPI_SEA_LIB_SEE;
#ifdef SUPPORT_SEA_AHE_PARAM
    seaLibId = GetSeaLibId();
#endif
    for (uint32_t i = 0; i < ARRAY_SIZE(g_caps); i++) {
        if (inputAttr->audioFormat == g_caps[i].audioFormat &&
            inputAttr->sampleRate == g_caps[i].sampleRate &&
            inputAttr->channelCount == g_caps[i].channelCount &&
            seaLibId == g_caps[i].seaLibId) {
            return true;
        }
    }
    return false;
}
#endif

static int32_t SubPortTrackInInit(SubPortTrackInContext *portTrackInCtx, const struct PortPluginAttr *inputAttr)
{
    /* check whether the input attribute is supported */
#if !defined(MELODY_DEMO_BMIC_TX) && !defined(MELODY_DEMO_SOUND_BOX)
    if (!IsSupported(inputAttr)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "invalid input attribute");
        return MEDIA_HAL_INVALID_PARAM;
    }
#endif
    portTrackInCtx->inputAttr = *inputAttr;

    /* The default bit width is 16 bits. The 24-bit width is processed separately. */
    portTrackInCtx->inputAttr.bitWidth = inputAttr->bitWidth;

    portTrackInCtx->common.audioPin = PIN_IN_MIC;
    portTrackInCtx->common.GetFrameCount = AudioPrimarySubPortInGetFrameCount;
    portTrackInCtx->common.GetLatency = NULL;
    portTrackInCtx->common.EnableTrack = AudioPrimarySubPortInEnableTrack;
    portTrackInCtx->common.PauseTrack = NULL;
    portTrackInCtx->common.ResumeTrack = NULL;
    portTrackInCtx->common.FlushTrack = NULL;
    portTrackInCtx->common.DisableTrack = AudioPrimarySubPortInDisableTrack;
    portTrackInCtx->common.AcquireFrame = AudioPrimarySubPortInAcquireFrame;
    portTrackInCtx->common.SendFrame = NULL;
    portTrackInCtx->common.GetPosition = AudioPrimarySubPortInGetPosition;
    portTrackInCtx->common.SetMute = AudioPrimarySubPortInSetMute;
    portTrackInCtx->common.GetMute = AudioPrimarySubPortInGetMute;
    portTrackInCtx->common.SetVolume = AudioPrimarySubPortInSetVolume;
    portTrackInCtx->common.GetVolume = AudioPrimarySubPortInGetVolume;
    portTrackInCtx->common.Invoke = AudioPrimarySubPortInInvoke;
    portTrackInCtx->common.RegCaptureCallback = AudioPrimarySubPortInRegCallback;
    portTrackInCtx->taskHandle = NULL;
    portTrackInCtx->mutex = NULL;
    portTrackInCtx->condHandle = NULL;
    portTrackInCtx->seaEventType = -1;
    portTrackInCtx->aiEventType = -1;
    portTrackInCtx->param = NULL;
    portTrackInCtx->taskActive = false;
    portTrackInCtx->isNeedOpenSea = true;
    MEDIA_HAL_LOGI(MODULE_NAME, "disableSEA =%d, micType = %d",
        inputAttr->disableSEA, inputAttr->micType);
    if (inputAttr->disableSEA) {
        portTrackInCtx->isNeedOpenSea = false;
    }
    portTrackInCtx->micType = inputAttr->micType;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef SUPPORT_AUDIO_RECOGNITION
static int32_t KwsCmdMatch(SubPortTrackInContext *portTrackInCtx)
{
    uapi_sea_kws_event_param *eventParam = (uapi_sea_kws_event_param *)portTrackInCtx->param;
    uapi_sea_item phraseItem = {};
    bool isFound = false;
    LpwkPhsSets *lpwkPhsSets = GetLpwkPhsSets();
    CHK_NULL_RETURN(lpwkPhsSets, MEDIA_HAL_INVALID_PARAM, "lpwkPhsSets is NULL!");
    for (uint32_t i = 0; i < lpwkPhsSets->phsCount; i++) {
        if (eventParam->phrase_id == lpwkPhsSets->phrase[i].id) {
            phraseItem = lpwkPhsSets->phrase[i];
            isFound = true;
        }
    }
    if (!isFound) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support command word,type = %d!", portTrackInCtx->aiEventType);
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->callback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support callback,callback is NULL!");
        return MEDIA_HAL_ERR;
    }
    (void)portTrackInCtx->callback(AUDIO_CAPTURE_VOICE_COMMAND, (void *)(uintptr_t)phraseItem.id,
        sizeof(phraseItem.id), portTrackInCtx->caller);
    return MEDIA_HAL_OK;
}

static int32_t KwsStartAIAndSea(SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->inputAttr.isAlwaysWake) {
        return MEDIA_HAL_OK;
    }
    (void)uapi_set_hifi_mode(HIFI_LPM_CLK_HIGH_LEVEL);
    int32_t ret = uapi_ai_start(portTrackInCtx->aiHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_start failed");
    ret = uapi_sea_start(portTrackInCtx->seaHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_start failed");
    (void)portTrackInCtx->callback(AUDIO_CAPTURE_VOICE_BEGIN, NULL, 0, portTrackInCtx->caller);
    return MEDIA_HAL_OK;
}

static int32_t KwsStopAIAndSea(SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->inputAttr.isAlwaysWake) {
        return MEDIA_HAL_OK;
    }
    int32_t ret = uapi_sea_stop(portTrackInCtx->seaHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_stop failed");
    ret = uapi_ai_stop(portTrackInCtx->aiHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_stop failed");
    (void)uapi_set_hifi_mode(HIFI_LPM_CLK_LEVEL1);
    ret = uapi_ai_reset_vad(portTrackInCtx->aiHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_reset_vad failed");
    (void)portTrackInCtx->callback(AUDIO_CAPTURE_VOICE_END, NULL, 0, portTrackInCtx->caller);
    return MEDIA_HAL_OK;
}

static void HandleAIEvent(SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = 0;
    switch (portTrackInCtx->aiEventType) {
        case -1:
            break;
        case UAPI_AI_EVENT_VAD_VALID:
            ret = KwsStartAIAndSea(portTrackInCtx);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "start ai and sea failed!");
                portTrackInCtx->callback(AUDIO_CAPTURE_ERROR_OCCUR, NULL, 0, portTrackInCtx->caller);
            }
            break;
        case UAPI_AI_EVENT_VAD_TIMEOUT:
            portTrackInCtx->callback(AUDIO_CAPTURE_ERROR_OCCUR, NULL, 0, portTrackInCtx->caller);
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "not support AI event type[%d]", portTrackInCtx->aiEventType);
            break;
    }
}

static void HandleSeaEvent(SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = 0;
    switch (portTrackInCtx->seaEventType) {
        case UAPI_SEA_EVENT_KWS_MATCH:
            ret = KwsCmdMatch(portTrackInCtx);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "kws cmd failed!");
                portTrackInCtx->callback(AUDIO_CAPTURE_ERROR_OCCUR, NULL, 0, portTrackInCtx->caller);
            }
            break;
        case UAPI_SEA_EVENT_VAD_END:
            ret = KwsStopAIAndSea(portTrackInCtx);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "kws stop failed!");
                portTrackInCtx->callback(AUDIO_CAPTURE_ERROR_OCCUR, NULL, 0, portTrackInCtx->caller);
            }
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "not support SEA event type[%d]", portTrackInCtx->seaEventType);
            break;
    }
}

static void *KwsEventListenerThread(void *ctx)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)ctx;
    if (portTrackInCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portTrackInCtx is NULL!");
        return NULL;
    }
    while (true) {
        MediaHalMutexLock(portTrackInCtx->mutex);
        if (!portTrackInCtx->taskActive) {
            MediaHalMutexUnLock(portTrackInCtx->mutex);
            break;
        }
        HandleAIEvent(portTrackInCtx);
        HandleSeaEvent(portTrackInCtx);
        MediaHalThreadCondWait(portTrackInCtx->condHandle, portTrackInCtx->mutex);
        MediaHalMutexUnLock(portTrackInCtx->mutex);
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "KwsEventListenerThread exit!");
    return NULL;
}

static int32_t CreateEventListenerThread(SubPortTrackInContext *portTrackInCtx)
{
    portTrackInCtx->mutex = MediaHalMutexCreate(NULL);
    if (portTrackInCtx->mutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create mutex failed!");
        return MEDIA_HAL_ERR;
    }
    portTrackInCtx->condHandle = MediaHalThreadCondCreate();
    if (portTrackInCtx->condHandle == NULL) {
        MediaHalMutexDestroy(&portTrackInCtx->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "create send mutex failed");
        return MEDIA_HAL_ERR;
    }
    MediaHalThreadattr attr = { "KwsEventListener", 0x1000, THREAD_SCHED_INVALID, 0, false };
    if (portTrackInCtx->taskHandle != NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "KwsEventListener still exists, cannot be created!");
        return MEDIA_HAL_ERR;
    }
    portTrackInCtx->taskActive = true;
    portTrackInCtx->taskHandle = MediaHalThreadCreate(KwsEventListenerThread, portTrackInCtx, &attr);
    if (portTrackInCtx->taskHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create KwsEventListener failed!");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "CreateEventListenerThread success!");
    return MEDIA_HAL_OK;
}

static void DestroyEventListenerThread(SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->taskHandle == NULL) {
        return;
    }
    MediaHalMutexLock(portTrackInCtx->mutex);
    portTrackInCtx->taskActive = false;
    MediaHalThreadCondSignal(portTrackInCtx->condHandle);
    MediaHalMutexUnLock(portTrackInCtx->mutex);
    MediaHalThreadJoin(&portTrackInCtx->taskHandle);
    portTrackInCtx->taskHandle = NULL;
    MediaHalMutexDestroy(&portTrackInCtx->mutex);
    MediaHalThreadCondDestroy(&portTrackInCtx->condHandle);
    MEDIA_HAL_LOGI(MODULE_NAME, "DestroyEventListenerThread success!");
}

static int32_t AIEventCB(uint32_t ai, uapi_ai_event_type eventType, const void *param, void *context)
{
    MEDIA_HAL_UNUSED(ai);
    MEDIA_HAL_LOGI(MODULE_NAME, "AI Event type [%d]!", eventType);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)context;
    MediaHalMutexLock(portTrackInCtx->mutex);
    portTrackInCtx->aiEventType = (int32_t)eventType;
    portTrackInCtx->param = (void *)param;
    MediaHalThreadCondSignal(portTrackInCtx->condHandle);
    MediaHalMutexUnLock(portTrackInCtx->mutex);
    return MEDIA_HAL_OK;
}

static int32_t SeaEventCB(uint32_t sea, uapi_sea_event_type eventType, void *param, void *context)
{
    MEDIA_HAL_UNUSED(sea);
    MEDIA_HAL_LOGI(MODULE_NAME, "Sea Event type [%d]!", eventType);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)context;
    MediaHalMutexLock(portTrackInCtx->mutex);
    portTrackInCtx->seaEventType = (int32_t)eventType;
    portTrackInCtx->param = param;
    MediaHalThreadCondSignal(portTrackInCtx->condHandle);
    MediaHalMutexUnLock(portTrackInCtx->mutex);
    return MEDIA_HAL_OK;
}
#endif

#ifdef AUDIO_SEA_SUPPORT
static int32_t AudioSDKOpenSeaInternal(SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->isNeedOpenSea) {
        int32_t ret = AudioSDKSeaOpen(&portTrackInCtx->seaHandle, portTrackInCtx->inputAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "sea open failed");
            return MEDIA_HAL_ERR;
        }
#ifdef SUPPORT_AUDIO_RECOGNITION
        ret = uapi_sea_register_event_proc(portTrackInCtx->seaHandle, SeaEventCB, portTrackInCtx);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "register sea event failed");
            (void)AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
            return MEDIA_HAL_ERR;
        }
#endif
    }
    return MEDIA_HAL_OK;
}
#endif

static int32_t AudioSDKOpenInternal(SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdpOpen(&portTrackInCtx->outputAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "output adp open failed");
    uapi_aenc_attr aencAttr = {};
    uapi_acodec_id aencCodecId;
#ifdef AUDIO_SEA_SUPPORT
    ret = AudioSDKOpenSeaInternal(portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sea open failed");
        goto ADP_CLOSE;
    }
#endif
    if (!AudioConvertAudioFormatToCodecId(portTrackInCtx->inputAttr.audioFormat, &aencCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto SEA_CLOSE;
    }
    aencAttr = GetAencAttrInfo(aencCodecId, portTrackInCtx->inputAttr);
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_LIVE_MIC ||
        portTrackInCtx->inputAttr.type == AUDIO_IN_VOICE_RECORD) {
        aencAttr.param.channels = 1;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat: 0x%x, sampleRate: %d, channelCount: %u, bitWidth: %u",
        portTrackInCtx->inputAttr.audioFormat, portTrackInCtx->inputAttr.sampleRate, aencAttr.param.channels, portTrackInCtx->inputAttr.bitWidth);

    ret = AudioSDKAencOpen(&portTrackInCtx->aencHandle, aencAttr, AENC_TYPE_PRIMARY, &(portTrackInCtx->inputAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc open failed");
        goto SEA_CLOSE;
    }
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif
    return MEDIA_HAL_OK;
#ifdef AUDIO_SDK_SUPPORT
SEA_CLOSE:
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea) {
        (void)AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
    }
ADP_CLOSE:
#endif
    (void)AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    return ret;
#endif
}

static int32_t AudioSDKCloseInternal(SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
    }
#endif
    if (portTrackInCtx->inputAttr.type != AUDIO_IN_VOICE_RECOGNITION) {
        ret = AudioSDKAencClose(portTrackInCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    }
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif

    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
#ifdef AUDIO_SEA_SUPPORT
static int32_t AudioSeaAttachAenc(SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#ifdef MELODY_DEMO_BMIC_TX
    if ((portTrackInCtx->inputAttr.type & AUDIO_IN_COMMUNICATION) == AUDIO_IN_COMMUNICATION) {
        ret = AudioSDKAttach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_SEA_REC, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
    }
#else
    ret = AudioSDKAttach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
#endif
    return ret;
}

static int32_t AudioSeaDeattachAenc(const SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#ifdef MELODY_DEMO_BMIC_TX
    if ((portTrackInCtx->inputAttr.type & AUDIO_IN_COMMUNICATION) == AUDIO_IN_COMMUNICATION) {
        ret = AudioSDKDetach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_SEA_REC, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
    }
#else
    ret = AudioSDKDetach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
#endif
    return ret;
}
#endif

static int32_t AudioSDKAttachInternal(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKAttach(AUDIO_MOD_AI_SEA, aiHandle, portTrackInCtx->seaHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai attach sea failed");
        ret = AudioSeaAttachAenc(portTrackInCtx);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "sea attach aenc failed");
            goto AI_DETACH_SEA;
        }
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_AI, aiHandle, portTrackInCtx->aencHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai attach aenc failed");
    }
#else
    ret = AudioSDKAttach(AUDIO_MOD_AI, aiHandle, portTrackInCtx->aencHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai attach aenc failed");
#endif

    ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackInCtx->aencHandle, portTrackInCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
#ifdef AUDIO_SEA_SUPPORT
        if (portTrackInCtx->isNeedOpenSea) {
            goto SEA_DETACH_AENC;
        }
#else
        goto AI_DETACH_AENC;
#endif
    }

    return MEDIA_HAL_OK;

#ifdef AUDIO_SEA_SUPPORT
SEA_DETACH_AENC:
    if (portTrackInCtx->isNeedOpenSea) {
        (void)AudioSeaDeattachAenc(portTrackInCtx);
    }
AI_DETACH_SEA:
    if (portTrackInCtx->isNeedOpenSea) {
        (void)AudioSDKDetach(AUDIO_MOD_AI_SEA, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
    }
#else
AI_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
#endif
    return ret;
}
#endif

#ifdef AUDIO_SDK_SUPPORT
static int32_t EstablishVoiceAssistantipeline(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
#ifdef SUPPORT_AUDIO_RECOGNITION
    int32_t ret = CreateEventListenerThread(portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CreateEventListenerThread failed");
        return MEDIA_HAL_ERR;
    }
    ret = uapi_ai_register_event_proc(aiHandle, AIEventCB, portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "register ai event failed");
        DestroyEventListenerThread(portTrackInCtx);
        return MEDIA_HAL_ERR;
    }
#else
    MEDIA_HAL_UNUSED(aiHandle);
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif
    return MEDIA_HAL_OK;
}

static int32_t EstablishPickUpPipeline(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->isNeedOpenSea) {
        int32_t ret = AudioSDKSeaOpen(&portTrackInCtx->seaHandle, portTrackInCtx->inputAttr);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKSeaOpen failed");
        ret = AudioSDKAttach(AUDIO_MOD_AI_SEA, aiHandle, portTrackInCtx->seaHandle);
        if (ret != MEDIA_HAL_OK) {
            AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttachInternal failed");
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t EstablishPipeline(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret;
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_VOICE_RECOGNITION) {
        ret = EstablishVoiceAssistantipeline(aiHandle, portTrackInCtx);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "EstablishVoiceAssistantipeline failed");
#ifdef AUDIO_SEA_SUPPORT
        ret = AudioSDKOpenSeaInternal(portTrackInCtx);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKOpenSeaInternal failed");
        ret = AudioSDKAttach(AUDIO_MOD_AI_SEA, aiHandle, portTrackInCtx->seaHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttachInternal failed");
            goto AUDIO_SDK_CLOSE;
        }
#endif
        return MEDIA_HAL_OK;
    }
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_PICKUP) {
        return EstablishPickUpPipeline(aiHandle, portTrackInCtx);
    }
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_VOICE_ASSISTANT &&
        !portTrackInCtx->inputAttr.isAlwaysWake) {
        ret = EstablishVoiceAssistantipeline(aiHandle, portTrackInCtx);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKOpenInternal failed");
    }
    ret = AudioSDKOpenInternal(portTrackInCtx);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKOpenInternal failed");

    ret = AudioSDKAttachInternal(aiHandle, portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttachInternal failed");
        goto AUDIO_SDK_CLOSE;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

AUDIO_SDK_CLOSE:
    (void)AudioSDKCloseInternal(portTrackInCtx);
    return ret;
}
#endif

static int32_t PickUpDetach(const SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = MEDIA_HAL_OK;
    if (!portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->pickupTrackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea detach track failed");
        return MEDIA_HAL_OK;
    }
#ifdef AUDIO_SEA_SUPPORT
    ret = AudioSDKDetach(AUDIO_MOD_AI_SEA, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach sea failed");
    ret = AudioSDKDetach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->pickupTrackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea detach track failed");
    ret = AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
    return MEDIA_HAL_OK;
#endif
#endif
}

static int32_t AIDetach(const SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = MEDIA_HAL_OK;
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_PICKUP) {
        return PickUpDetach(portTrackInCtx);
    }
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_VOICE_RECOGNITION) {
#ifdef AUDIO_SEA_SUPPORT
        if (portTrackInCtx->isNeedOpenSea) {
            ret = AudioSDKDetach(AUDIO_MOD_AI_SEA, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
            CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach sea failed");
            ret = AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
            CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
        }
#endif
        return MEDIA_HAL_OK;
    }
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKDetach(AUDIO_MOD_AI_SEA, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach sea failed");
        ret = AudioSeaDeattachAenc(portTrackInCtx);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea detach aenc failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach aenc failed");
    }
#else
    ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach aenc failed");
#endif
    ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackInCtx->aencHandle, portTrackInCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKSeaClose(portTrackInCtx->seaHandle, portTrackInCtx->inputAttr.type);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
    }
#endif
    ret = AudioSDKAencClose(portTrackInCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    ret = AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetFrameCount(const AudioHandle trackHandle, uint64_t *count)
{
    MEDIA_HAL_UNUSED(trackHandle);

    *count = AUDIO_HAL_FRAME_COUNT;

    MEDIA_HAL_LOGI(MODULE_NAME, "frame count: %llu", *count);
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SEA_SUPPORT
static int32_t AudioSeaStart(SubPortTrackInContext *portTrackInCtx)
{
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGI(MODULE_NAME, "unspport sea!");
        return MEDIA_HAL_OK;
    }
    if (isMultipleSeaOpen()) {
        MEDIA_HAL_LOGI(MODULE_NAME, "multiple sea create!");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = MEDIA_HAL_OK;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_start(portTrackInCtx->seaHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_sea_start failed, ret:%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}
#endif

static int32_t AudioAencStart(SubPortTrackInContext *portTrackInCtx)
{
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_VOICE_RECOGNITION) {
        MEDIA_HAL_LOGI(MODULE_NAME, "no need to start aenc with type:AUDIO_IN_VOICE_RECOGNITION!");
        return MEDIA_HAL_OK;
    }
    int32_t ret = MEDIA_HAL_OK;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackInCtx->aencHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "uapi_aenc_start failed, ret:%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInEnableTrack(AudioHandle trackHandle)
{
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_ERR, "primary in handle is NULL!");
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    (void)AudioSDKSetAIScene(portTrackInCtx->aiHandle, &portTrackInCtx->inputAttr, false);
    if (portTrackInCtx->inputAttr.type == AUDIO_IN_PICKUP) {
        MEDIA_HAL_LOGI(MODULE_NAME, "pick up type only create track");
        return MEDIA_HAL_OK;
    }
    int32_t ret = MEDIA_HAL_OK;
    if (!isMultipleAIOpen()) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackInCtx->aiHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_start failed");
    }
#ifdef AUDIO_SEA_SUPPORT
    if (AudioSeaStart(portTrackInCtx) != MEDIA_HAL_OK) {
        goto AI_STOP;
    }
#endif
    if (AudioAencStart(portTrackInCtx) != MEDIA_HAL_OK) {
        goto SEA_STOP;
    }
#ifndef MELODY_AUDIO_SUPPORT
        portTrackInCtx->offset = portTrackInCtx->inputAttr.audioFormat == AUDIO_FORMAT_TYPE_OPUS ?
            OGG_HEAD_LEN : 0;
#endif
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    StartAudioLinkChr(portTrackInCtx->inputAttr.type, false, false);
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
SEA_STOP:
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea && !isMultipleSeaOpen()) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_stop(portTrackInCtx->seaHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_stop failed");
    }
AI_STOP:
#endif
    if (!isMultipleAIOpen()) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackInCtx->aiHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_ai_stop failed");
    }
    return ret;
#endif
}

static bool AudioTypeHasAenc(enum AudioCategory type)
{
    return type != AUDIO_IN_VOICE_RECOGNITION && type != AUDIO_IN_PICKUP;
}

int32_t AudioPrimarySubPortInDisableTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    if (!isMultipleAIOpen()) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackInCtx->aiHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_stop failed");
    }
#ifdef AUDIO_SEA_SUPPORT
    if (portTrackInCtx->isNeedOpenSea && !isMultipleSeaOpen()) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_stop(portTrackInCtx->seaHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_stop failed");
    }
#endif
    if (AudioTypeHasAenc(portTrackInCtx->inputAttr.type)) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackInCtx->aencHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_stop failed");
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    StopAudioLinkChr(portTrackInCtx->inputAttr.type, false, false);
    return MEDIA_HAL_OK;
}

static int32_t CheckParam(SubPortTrackInContext *portTrackInCtx, uint8_t *buffer,
    uint64_t requestBytes, uapi_stream_buf *audioStream)
{
    if (((uint32_t)requestBytes) < audioStream->size) {
        audioStream->size = requestBytes;
    }

    if (portTrackInCtx->inputAttr.audioFormat == AUDIO_FORMAT_TYPE_AMR_WB) {
        if (audioStream->size < AMRWB_FRAME_SIZE) {
            MEDIA_HAL_LOGI(MODULE_NAME, "audioStream->size:%u", audioStream->size);
            audioStream->size = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
    }

    if (requestBytes > SIZE_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input buffer size overflow");
        return MEDIA_HAL_INVALID_PARAM;
    }
    if (memcpy_s(buffer, (size_t)requestBytes, audioStream->data + portTrackInCtx->offset,
        audioStream->size - portTrackInCtx->offset) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }

    int64_t sec = 0;
    int64_t nsec = 0;
    int32_t ret = MediaHalConvertUSecToSecAndNSec(audioStream->pts, &sec, &nsec);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "convert time from us to sec and nsec failed");
        return MEDIA_HAL_ERR;
    }

    portTrackInCtx->aiTimeStamp.tvSec = sec;
    portTrackInCtx->aiTimeStamp.tvNSec = nsec;
    portTrackInCtx->frameIndex++;
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    uapi_stream_buf audioStream = {};
    int32_t ret = uapi_adp_acquire_stream(portTrackInCtx->outputAdpHandle, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        *replyBytes = 0;
        return MEDIA_HAL_ERR_AGAIN;
    }
    ret = CheckParam(portTrackInCtx, buffer, requestBytes, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "param is invalid");
        (void)uapi_adp_release_stream(portTrackInCtx->outputAdpHandle, &audioStream);
        return ret;
    }
    *replyBytes = audioStream.size - (uint32_t)portTrackInCtx->offset;
    ret = uapi_adp_release_stream(portTrackInCtx->outputAdpHandle, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "release stream failed: 0x%x", ret);
    }
#else
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetPosition(AudioHandle trackHandle,
    uint64_t *frames, struct AudioTimeStamp *time)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    time->tvSec = portTrackInCtx->aiTimeStamp.tvSec;
    time->tvNSec = portTrackInCtx->aiTimeStamp.tvNSec;
    *frames = portTrackInCtx->frameIndex;
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInSetMute(AudioHandle trackHandle, bool mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    return AudioSDKSetAIMute(portTrackInCtx->aiHandle, mute);
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER,
        MEDIA_CHR_AM_SET_MIC_MUTE, MEDIA_CHR_SDK_SET_MIC_MUTE);
    MEDIA_HAL_LOGI(MODULE_NAME, "set mute %d success", mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetMute(AudioHandle trackHandle, bool *mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    return AudioSDKGetAIMute(portTrackInCtx->aiHandle, mute);
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "get mute %d success", *mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInSetVolume(AudioHandle trackHandle, float volume)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    uapi_ai_gain gain = {};
    GetVolumeGain((int32_t)volume, &gain.integer, &gain.decimal);
    MEDIA_HAL_LOGI(MODULE_NAME, "set mic volume integer:%d decimal:%d", gain.integer, gain.decimal);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_set_volume(portTrackInCtx->aiHandle, &gain));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_set_volume failed");
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER,
        MEDIA_CHR_AM_SET_VOLUME, MEDIA_CHR_SDK_SET_UP_VOLUME);
    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f success", volume);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetVolume(AudioHandle trackHandle, float *volume)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    uapi_ai_gain gain = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_get_volume(portTrackInCtx->aiHandle, &gain));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_get_volume failed");

    *volume = (float)gain.integer;
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER,
        MEDIA_CHR_AM_GET_VOLUME, MEDIA_CHR_SDK_GET_UP_VOLUME);
    MEDIA_HAL_LOGI(MODULE_NAME, "get volume %f success", *volume);
    return MEDIA_HAL_OK;
}
#ifdef AUDIO_SDK_SUPPORT
static int32_t SetMicVolume(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t micVolumeInteger = 0;
    int32_t micVolumeDecimal = 0;
    uint32_t micType = 0;
    int32_t ret = sscanf_s(value, "%u#%d#%d", &micType, &micVolumeInteger, &micVolumeDecimal);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set mic volume sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->micType != micType) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the mic type is incorrect.!");
        return MEDIA_HAL_ERR;
    }
    uapi_ai_gain volume;
    volume.integer = micVolumeInteger;
    volume.decimal = micVolumeDecimal;
    uapi_ai_cali_volume cailVolume;
    cailVolume.mic_num = 1;
    cailVolume.volume[0] = volume;
    ret = uapi_ai_set_mic_cali_volume(portTrackInCtx->aiHandle, &cailVolume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set mic volume failed! ret = %d, micVolumeInteger = %d, micVolumeDecimal = %d",
            ret, micVolumeInteger, micVolumeDecimal);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t ChangeMicAiPort(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t micType = 0;
    int32_t ret = sscanf_s(value, "%u", &micType);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "change mic Type sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackInCtx->aiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai stop failed");
    if (portTrackInCtx->isNeedOpenSea) {
        (void)AudioSDKDetach(AUDIO_MOD_AI_SEA, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
    } else {
        (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
    }
    ret = AudioSDKAIClose(portTrackInCtx->aiHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai close failed");
    AIOpenAttr aiOpenAttr = { portTrackInCtx->inputAttr.type, micType, portTrackInCtx->isNeedOpenSea,
        true, portTrackInCtx->inputAttr};
    ret = AudioSDKAIOpen(&portTrackInCtx->aiHandle, &aiOpenAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai open failed");
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai attach aenc failed");
        AudioSDKAIClose(portTrackInCtx->aiHandle);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackInCtx->aiHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai start failed");
        AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
        AudioSDKAIClose(portTrackInCtx->aiHandle);
        return MEDIA_HAL_ERR;
    }
    portTrackInCtx->micType = micType;
    return MEDIA_HAL_OK;
}

static int32_t PrimarySetSeaEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
    return SetSeaEnable(portTrackInCtx->seaHandle, value, valueLen);
}

#ifdef MELODY_AUDIO_SUPPORT
#if !defined(MELODY_DEMO_BMIC_TX) && !defined(MELODY_DEMO_BMIC_RX)
static int32_t SetAIDbVolume(SubPortTrackInContext *portTrackInCtx, uint32_t dbVolume, char *value)
{
#ifdef MELODY_DEMO_EARPHONE
    MEDIA_HAL_UNUSED(portTrackInCtx);
    MEDIA_HAL_UNUSED(dbVolume);
    MEDIA_HAL_UNUSED(value);
#else
    uapi_ai_gain sndGain = { 0 };
    GetVolumeGain(dbVolume, &sndGain.integer, &sndGain.decimal);
    MEDIA_HAL_LOGI(MODULE_NAME, "set db volume integer:%d decimal:%d", sndGain.integer, sndGain.decimal);
    int32_t ret = uapi_ai_set_volume(portTrackInCtx->aiHandle, &sndGain);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_set_volume failed");
    ret = AudioSDKRecordAudioDbVolume(value, portTrackInCtx->inputAttr.type);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKRecordAudioDbVolume failed");
#endif
    return MEDIA_HAL_OK;
}

static int32_t GetAIDbVolume(SubPortTrackInContext *portTrackInCtx, char *value, uint32_t valueLen)
{
    int32_t dbVolume = 0;
    uapi_ai_gain sndGain = { 0 };
    int32_t ret = uapi_ai_get_volume(portTrackInCtx->aiHandle, &sndGain);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_get_volume failed");
    dbVolume = sndGain.integer * VOLUME_ACCURACY + sndGain.decimal;
    MEDIA_HAL_LOGI(MODULE_NAME, "get db volume integer:%d decimal:%d, dbVolume = %d",
        sndGain.integer, sndGain.decimal, dbVolume);
    ret = sprintf_s(value, valueLen, "%d", dbVolume);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get db volume sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}
#endif
#endif

static int32_t SetDbVolume(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
#ifdef MELODY_AUDIO_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t dbVolume = 0;
    uint32_t volumeModule = 0;
    int32_t ret = sscanf_s(value, "%d#%u#", &dbVolume, &volumeModule);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    td_handle handle = portTrackInCtx->aencHandle;
    if (volumeModule == MEDIA_VOLUME_MODULE_AI) {
        handle = portTrackInCtx->aiHandle;
    }
    ret = AudioSDKSetDbVolume(handle, value, portTrackInCtx->inputAttr.type);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKSetDbVolume failed");
#else
    ret = SetAIDbVolume(portTrackInCtx, dbVolume, value);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "set AI db volume failed !");
#endif
#else
    MEDIA_HAL_UNUSED(value);
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    return MEDIA_HAL_OK;
}

static int32_t GetDbVolume(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
#ifdef MELODY_AUDIO_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t ret;
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    ret = AudioSDKGetDbVolume(value, valueLen, portTrackInCtx->inputAttr.type);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, " AudioSDKGetDbVolume failed !");
#else
    ret = GetAIDbVolume(portTrackInCtx, value, valueLen);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get AI db volume failed !");
#endif
#else
    MEDIA_HAL_UNUSED(value);
    MEDIA_HAL_UNUSED(valueLen);
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    return MEDIA_HAL_OK;
}

static int32_t SetSeaParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t paramLen = 0;
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    int32_t ret = 0;
    int32_t cmd = 0;
    ret = sscanf_s(value, "%s = %d#%u:", configFlag, MAX_KEY_STR_SIZE, &cmd, &paramLen);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set sea param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uint8_t *param = (uint8_t *)MediaHalCalloc(SEA_PARAM_LEN);
    CHK_NULL_RETURN(param, MEDIA_HAL_INVALID_PARAM, "param malloc failed!");
    ret = GetParamByValue(param, SEA_PARAM_LEN, value, valueLen, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MediaHalFree(param);
        return ret;
    }
    ret = uapi_sea_set_param(portTrackInCtx->seaHandle, cmd, (const td_void *)param, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set sea param failed! ret = %d", ret);
        MediaHalFree(param);
        return ret;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_GET_PARAM_STREAM, MEDIA_CHR_SET_SEA_PARAM);
    MediaHalFree(param);
    return MEDIA_HAL_OK;
}

static uint32_t AmrwbGetBitRate(uint32_t mode)
{
    size_t i = 0;
    for (; i < sizeof(g_amrwbSupportTable) /
        sizeof(g_amrwbSupportTable[0]); i++) {
        if (g_amrwbSupportTable[i].mode == mode) {
            return g_amrwbSupportTable[i].bitRate;
        }
    }
    return DEFAULT_BITRATE; // 配置模式错误时按照默认23.85的码率配置
}

static int32_t SetBitRate(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t bitRate = 0;
    int32_t ret = sscanf_s(value, "%u", &bitRate);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set bit rate sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->inputAttr.audioFormat == AUDIO_FORMAT_TYPE_AMR_WB) {
        bitRate = AmrwbGetBitRate(bitRate);
    }
    ret = uapi_aenc_set_param(portTrackInCtx->aencHandle, UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_SET_PARAM_STREAM, MEDIA_CHR_SET_BIT_RATE);
    return MEDIA_HAL_OK;
}

#ifdef MELODY_DEMO_BMIC_TX
static int32_t SetAencParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t num = 0;
    uint32_t aencParamTypeLen = 0;
    uapi_aenc_param_type paramType;
    int32_t ret = sscanf_s(value, "setencodeparam = %u#%u#%u:", &num, &paramType, &aencParamTypeLen);
    if (ret != SSCANF_NUM3) {
        MEDIA_HAL_LOGE(MODULE_NAME, "setencodeparam sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (paramType != UAPI_AENC_PARAM_DATA_SOURCE_CFG) {
        MEDIA_HAL_LOGE(MODULE_NAME, "paramType is invalid, paramType = %d", paramType);
        return MEDIA_HAL_ERR;
    }
    uapi_aenc_data_source config;
    if (aencParamTypeLen != sizeof(uapi_aenc_data_source)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aencParamTypeLen is invalid, aencParamTypeLen = %u", aencParamTypeLen);
        return MEDIA_HAL_INVALID_PARAM;
    }
    ret = GetParamByValue((uint8_t *)&config, aencParamTypeLen, value, valueLen, aencParamTypeLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed!");
        return MEDIA_HAL_ERR;
    }
    ret = uapi_aenc_set_param(portTrackInCtx->aencHandle, UAPI_AENC_PARAM_DATA_SOURCE_CFG, &config,
        sizeof(uapi_aenc_data_source));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_set_param failed!");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}
#endif

static int32_t SetDenoiseEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
    int32_t seaStatus = 0;
    int32_t ret = sscanf_s(value, "%d", &seaStatus);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set anr enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    if (seaStatus == 0) {
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_ANR;
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_WNR;
    } else {
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_WNR;
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_ANR;
    }
    ret = uapi_sea_set_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetDenoiseEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not get sea options!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    td_s32 ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    bool enable = false;
    if (afeAttr.u.see.options & UAPI_SEA_AFE_OPT_WNR) {
        enable = true;
    }
    ret = sprintf_s(value, valueLen, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get Denoise enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t SetLowCutEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
    int32_t seaStatus = 0;
    int32_t ret = sscanf_s(value, "%d", &seaStatus);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set anr enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    if (seaStatus == 0) {
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_EQ_HP;
    } else {
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_EQ_HP;
    }
    ret = uapi_sea_set_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetLowCutEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not get sea options!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    td_s32 ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    bool enable = false;
    if (afeAttr.u.see.options & UAPI_SEA_AFE_OPT_EQ_HP) {
        enable = true;
    }
    ret = sprintf_s(value, valueLen, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get low cut enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t SetHiFiEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
    int32_t seaStatus = 0;
    int32_t ret = sscanf_s(value, "%d", &seaStatus);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set hifi enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    if (seaStatus == 0) {
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_HIFI;
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_PEQ;
    } else {
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_PEQ;
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_HIFI;
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_AH;
    }
    ret = uapi_sea_set_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetHiFiEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not get sea options!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    td_s32 ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    bool enable = false;
    if (afeAttr.u.see.options & UAPI_SEA_AFE_OPT_HIFI) {
        enable = true;
    }
    ret = sprintf_s(value, valueLen, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get hifi enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t SetVoiceEnhanceEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
    int32_t seaStatus = 0;
    int32_t ret = sscanf_s(value, "%d", &seaStatus);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set voice enhance enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea afe attr failed!");
        return ret;
    }
    if (seaStatus == 0) {
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_AH;
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_PEQ;
    } else {
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_PEQ;
        afeAttr.u.see.options |= UAPI_SEA_AFE_OPT_AH;
        afeAttr.u.see.options &= ~UAPI_SEA_AFE_OPT_HIFI;
    }
    ret = uapi_sea_set_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetVoiceEnhanceEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not get sea options!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {};
    td_s32 ret = uapi_sea_get_afe_attr(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea aef attr failed!");
        return ret;
    }
    bool enable = false;
    if (afeAttr.u.see.options & UAPI_SEA_AFE_OPT_AH) {
        enable = true;
    }
    ret = sprintf_s(value, valueLen, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get voice enhance enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetSeaParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t paramLen = 0;
    int32_t ret = 0;
    int32_t cmd = 0;
    ret = sscanf_s(value, "%s = %d#%u:", configFlag, MAX_KEY_STR_SIZE, &cmd, &paramLen);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGT(MODULE_NAME, "get sea param cmd = %d", cmd);
    ret = uapi_sea_get_param(portTrackInCtx->seaHandle, cmd, (td_void *)value, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea param failed! ret = %x", ret);
        return ret;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_GET_PARAM_STREAM, MEDIA_CHR_GET_SEA_PARAM);
    return MEDIA_HAL_OK;
}

static int32_t PickUpStreamStart(SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret = uapi_ai_start(portTrackInCtx->aiHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_startfailed! ret = %x", ret);
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->isNeedOpenSea) {
        ret = uapi_sea_start(portTrackInCtx->seaHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_sea_start! ret = %x", ret);
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t AttachBackend(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(portTrackInCtx, MEDIA_HAL_INVALID_PARAM, "portTrackInCtx is null");
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t backendHandle = 0;
    int32_t ret = sscanf_s(value, "%s = %u", configFlag, MAX_KEY_STR_SIZE, &backendHandle);
    if (ret <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AttachBackend sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    if (portTrackInCtx->isNeedOpenSea) {
        ret = AudioSDKAttach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, backendHandle);
        if (ret != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AttachBackend attach failed!");
            return MEDIA_HAL_ERR;
        }
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, backendHandle);
        if (ret != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AttachBackend attach failed!");
            return MEDIA_HAL_ERR;
        }
    }
    portTrackInCtx->pickupTrackHandle = backendHandle;
    return PickUpStreamStart(portTrackInCtx);
}

static struct AudioPortInvoke g_primaryInInvokeFuncList[] = {
    {MIC_VOLUME, SetMicVolume, true},
    {MIC_TYPE, ChangeMicAiPort, true},
    {SEA_ENABLE, PrimarySetSeaEnable, true},
    {SET_DB_VOLUME, SetDbVolume, true},
    {GET_DB_VOLUME, GetDbVolume, false},
    {SET_SEA_PARAM, SetSeaParam, false},
    {SET_DENOISE_ENABLE, SetDenoiseEnable, true},
    {GET_DENOISE_ENABLE, GetDenoiseEnable, false},
    {SET_LOWCUT_ENABLE, SetLowCutEnable, true},
    {GET_LOWCUT_ENABLE, GetLowCutEnable, false},
    {SET_HIFI_ENABLE, SetHiFiEnable, true},
    {GET_HIFI_ENABLE, GetHiFiEnable, false},
    {SET_VOICE_ENHANCE_ENABLE, SetVoiceEnhanceEnable, true},
    {GET_VOICE_ENHANCE_ENABLE, GetVoiceEnhanceEnable, false},
    {GET_SEA_PARAM, GetSeaParam, false},
    {SET_BIT_RATE, SetBitRate, true},
    {ATTACH_BACKEND, AttachBackend, false},
#ifdef MELODY_DEMO_BMIC_TX
    {SET_ENCODE_PARAM, SetAencParam, false},
#endif
};
#endif

int32_t AudioPrimarySubPortInInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
        {
            SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
            *((uint32_t *)invokeAttr->request) = portTrackInCtx->outputAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "data channel id: 0x%x success", *((uint32_t *)invokeAttr->request));
            break;
        }
        case INVOKE_ID_SET_EXTRA_PARAMS:
        case INVOKE_ID_GET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_primaryInInvokeFuncList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(trackHandle, invokeAttr, g_primaryInInvokeFuncList, (uint32_t)size);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "extra param invoke failed!");
            break;
        }
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(invokeID);
    MEDIA_HAL_UNUSED(invokeAttr);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInRegCallback(AudioHandle trackHandle, AudioCaptureCallback callback, AudioHandle caller)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    portTrackInCtx->callback = callback;
    portTrackInCtx->caller = caller;
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInCreate(AudioHandle *portHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)MediaHalMalloc(sizeof(SubPortInContext));
    if (portInCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret;
    if (memset_s(portInCtx, sizeof(SubPortInContext), 0, sizeof(SubPortInContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    *portHandle = portInCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    MediaHalFree(portInCtx);
    return ret;
}

int32_t AudioPrimarySubPortInDestroy(AudioHandle portHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)portHandle;

    MediaHalFree(portInCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    MEDIA_HAL_UNUSED(port);
    capability->formats =  g_supportFormats;
#ifdef AUDIO_MANAGER_DUMP_INFO
    for (uint32_t i = 0; i < sizeof(g_supportFormats) / sizeof(g_supportFormats[0]); i++) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary supportFormats:%x", g_supportFormats[i]);
    }
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#ifdef AUDIO_SDK_SUPPORT
static int32_t AudioPrimarySubPortInCreateAI(SubPortInContext *portInCtx, SubPortTrackInContext *portTrackInCtx,
    enum AudioCategory type)
{
    AIOpenAttr *aiOpenAttr = (AIOpenAttr *)MediaHalCalloc(sizeof(AIOpenAttr));
    if (aiOpenAttr == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc AIOpenAttr failed");
        return MEDIA_HAL_NO_MEM;
    }
    aiOpenAttr->audioCategory = type;
    aiOpenAttr->micType = portTrackInCtx->micType;
    aiOpenAttr->isNeedOpenSea = portTrackInCtx->isNeedOpenSea;
    aiOpenAttr->isNeedSetRXType = false;
    aiOpenAttr->inputAttr = portTrackInCtx->inputAttr;
    MEDIA_HAL_LOGI(MODULE_NAME, "frameLength = %u", portTrackInCtx->inputAttr.frameLength);
    int32_t ret = AudioSDKAIOpen(&portInCtx->aiHandle, aiOpenAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAIOpen failed");
    }
    MediaHalFree(aiOpenAttr);
    return ret;
}
#endif

int32_t AudioPrimarySubPortInCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    AudioHandle *trackHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)portHandle;
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)MediaHalCalloc(sizeof(SubPortTrackInContext));
    if (portTrackInCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    int32_t ret = MEDIA_HAL_OK;
    if (memset_s(portTrackInCtx, sizeof(SubPortTrackInContext), 0, sizeof(SubPortTrackInContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE_TRACK_IN;
    }
    ret = SubPortTrackInInit(portTrackInCtx, inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackInInit failed");
        goto FREE_TRACK_IN;
    }
#ifdef AUDIO_SDK_SUPPORT
    ret = AudioPrimarySubPortInCreateAI(portInCtx, portTrackInCtx, inputAttr->type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAIOpen failed");
        goto FREE_TRACK_IN;
    }
    /* pcm: ai + adp, es: ai + sea + aenc + adp */
    ret = EstablishPipeline(portInCtx->aiHandle, portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "EstablishPipeline failed: 0x%x", ret);
        goto AI_CLOSE;
    }
    portTrackInCtx->aiHandle = portInCtx->aiHandle;
    *trackHandle = &portTrackInCtx->common;
#endif
    CreateAudioLinkChr(portTrackInCtx->inputAttr.type, false, false);
    return MEDIA_HAL_OK;
AI_CLOSE:
#ifdef AUDIO_SDK_SUPPORT
    (void)AudioSDKAIClose(portInCtx->aiHandle);
#endif
FREE_TRACK_IN:
    MediaHalFree(portTrackInCtx);
    return ret;
}

int32_t AudioPrimarySubPortInDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
#ifdef SUPPORT_AUDIO_RECOGNITION
    DestroyEventListenerThread(portTrackInCtx);
#endif
    int32_t ret = AIDetach(portTrackInCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AIDetach failed");

    ret = AudioSDKAIClose(portTrackInCtx->aiHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKAIClose failed");
    DestoryAudioLinkChr(portTrackInCtx->inputAttr.type, false, false);
    MediaHalFree(portTrackInCtx);
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    MEDIA_HAL_UNUSED(portHandle);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif