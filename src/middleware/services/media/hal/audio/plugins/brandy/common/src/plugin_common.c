/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: audio plugin common
* Author: Media Software Group
* Create: 2021-05-18
*/
#include "plugin_common.h"
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "media_hal_common.h"
#include "audio_types.h"

#include "primary_port.h"
#include "bluetooth_port.h"
#ifdef MELODY_DEMO_EARPHONE
#include "efuse.h"
#include "efuse_porting.h"
#include "sysc_license.h"
#endif
#ifndef MELODY_AUDIO_SUPPORT
#include "modem_port.h"
#elif defined(SUPPORT_USB)
#include "usb_port.h"
#endif
#include "product.h"
#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif
#ifdef SUPPORT_UX_CONFIG
#include "ux_config.h"
#endif
#ifdef SUPPORT_NV
#include "key_id.h"
#include "nv.h"
#ifdef MELODY_AUDIO_SUPPORT
#if defined(SUPPORT_FEATURE_CONFIG)
#include "key_struct.h"
#include "feature_config.h"
#else
#include "hearing_aid_key_struct.h"
#endif
#else
#include "common.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioPortPluginCommon"
#define WAIT_CREATE_AENC_LOOP_CNT 100
#define WAIT_CREATE_AENC_SUCCESS_US 10000
#define VOLUME_PERCENT_BASE 1000.0
#define VOLUME_MULTIPLIER 125
#define AUDIO_CHANNEL_COUNT_1 1
#define AUDIO_CHANNEL_COUNT_2 2
#define AUDIO_SAMPLE_RATE_48K 48000
#define AUDIO_DONGLE_SCENE_BITRATE_64 64
#define SSCANF_NUM1 1
#define SSCANF_NUM6 6
#define AUDIO_SAMPLE_BITDEPTH_32 32
#define AUDIO_SAMPLE_BITDEPTH_24 24
#define AUDIO_SAMPLE_BITDEPTH_16 16
#define MAX_NV_TO_CHAR_NUM 256
#define SSCANF_NUM3   3
#define MS_PER_SECOND   1000
#define AUDIO_FRAME_LENGTH_MAG_COEFF 10
#define INDEX_1 1
#define INDEX_2 2

static int32_t g_subPortTrackInOpened = 0;
static MediaHalMutexHandle g_subPortTrackInLock = NULL;
static MediaHalMutexHandle g_subPortTrackOutLock = NULL;
static bool g_audioSdkIsInited = false;
#ifdef NOT_SUPPORT_MULTI_AENC
static bool g_isCanCreateAenc = true;
#endif
static int32_t g_subPortSeaOpenCount = 0;
td_handle g_seaHandle = 0;
static MediaHalMutexHandle g_audioSdkInitLock = NULL;
static LpwkPhsSets g_lpwkPhsSets = { 0 };
#ifdef SUPPORT_NV
#ifndef MELODY_DEMO_BMIC_TX
static audio_vqe_param_t g_vqeParam = { 0 };
#ifdef SUPPORT_SEA_AHE_PARAM
static uint8_t g_anrMode = 0;
static audio_ahe_user_param_t g_aheUserParam = { 0 };
static nr_mic4stream_param_t g_aheNrParam = { 0 };
static audio_agc_param_t g_aheAgcParam = { 0 };
#define AUDIO_NR_MODE_MAX 3
static uint16_t g_aheNrNvIdMap[AUDIO_NR_MODE_MAX] = {
    NV_ID_AUDIO_NR_OFF_MIC4STREAM_PARA,
    NV_ID_AUDIO_NR_ONLY_MIC4STREAM_PARA,
    NV_ID_AUDIO_NR_WNR_MIC4STREAM_PARA
};
static uint16_t g_aheAgcNvIdMap[AUDIO_NR_MODE_MAX] = {
    NV_ID_AUDIO_NR_OFF_MIC4STREAM_AGC_PARA,
    NV_ID_AUDIO_NR_ONLY_MIC4STREAM_AGC_PARA,
    NV_ID_AUDIO_NR_WNR_MIC4STREAM_AGC_PARA
};
#endif
#endif
#endif

static uapi_sea_lib_id g_seaLibId = UAPI_SEA_LIB_SEE;

#ifndef MELODY_AUDIO_SUPPORT
typedef struct {
    uapi_ai_rx_type rxType;
    uint32_t channels;
    uint32_t bitDepth;
    uint32_t sampleRate;
    uint32_t samplePerFrame;
    uint32_t seaLibId;
} AudioAIAttr;

static AudioAIAttr g_aiAttr[] = {
    {
        .rxType = BUILTIN_AI_PORT_ATTR_ADC_RX_TYPE,
        .channels = BUILTIN_AI_PCM_ATTR_AUDIO_CH,
        .bitDepth = BUILTIN_AI_PCM_ATTR_BIT_DEPTH,
        .sampleRate = BUILTIN_AI_PCM_ATTR_SAMPLE_RATE,
        .samplePerFrame = BUILTIN_AI_PCM_ATTR_SAMPLE_PER_FRAME,
        .seaLibId = UAPI_SEA_LIB_SEE
    },
#ifdef SUPPORT_SEA_AHE_PARAM
    {
        .rxType = BUILTIN_AI_PORT_ATTR_ADC_RX_TYPE,
        .channels = AUDIO_AHE_AI_ATTR_CH,
        .bitDepth = AUDIO_AHE_AI_ATTR_BIT_DEPTH,
        .sampleRate = AUDIO_AHE_AI_ATTR_SAMPLE_RATE,
        .samplePerFrame = AUDIO_AHE_AI_ATTR_SAMPLE_PER_FRAME,
        .seaLibId = UAPI_SEA_LIB_AHE
    }
#endif
};
#endif

static uint8_t g_disableSea = 0;
#ifdef SUPPORT_VOLUME_CALIBRATION
static int32_t g_maxVolumeDb = 0;
#endif
#ifdef SUPPORT_AUDIO_EFFECT_PARAM
#if SND_PORT_AEF_TYPE == UAPI_AEF_TYPE_SWS
static audio_sws_param_t g_aefParam = { 0 };
#else
static audio_hvs_param_t g_aefParam = { 0 };
#endif
#endif

#define VOLUME_UNIT 100
#define VOLUME_DB_MIN (-120)
#define VOLUME_DB_MAX 0
#define PERCENT_TO_GAIN_LOG_FACTOR 20
#define MAGNIFY_TIMES 1000

#ifndef CONFIG_AUDIO_AI_REF_PORT
#define CONFIG_AUDIO_AI_REF_PORT UAPI_SND_OUT_PORT_DAC0
#endif

#ifndef CONFIG_AUDIO_AI_FRAME_PER_SEC
#define CONFIG_AUDIO_AI_FRAME_PER_SEC SAMPLERATE_TO_PTNUMPERFRM
#endif
#ifdef MELODY_DEMO_EARPHONE
#define DIE_ID_LENGTH 20
static uapi_acodec_lhdc_dec_priv_cfg g_licenseInfo = {};
#endif

#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUNDBAR))
static uapi_audio_pcm_format g_adecPrivate = {};
static uapi_acodec_l2hc_priv_cfg g_l2hcAencConfig = {};
static uapi_acodec_l2hc_priv_cfg g_l2hcAdecConfig = {};
static bool g_l2hcAencIsSetted = false;
static bool g_l2hcAdecIsSetted = false;
#endif

#ifdef MELODY_AUDIO_SUPPORT

typedef struct {
    PluginMicType micType;
    uapi_ai_amic_sel micSel;
} MicTypeConvert;

typedef struct {
    AencType aencType;
    uint32_t port;
} AencPortType;

#if ((defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)) || \
     defined(MELODY_DEMO_HA) && defined(SUPPORT_NV))
static MicTypeConvert g_micType[] = {
    {PLUGIN_MIC_AMIC_0, UAPI_AI_AMIC0},
    {PLUGIN_MIC_AMIC_1, UAPI_AI_AMIC1},
    {PLUGIN_MIC_AMIC_2, UAPI_AI_AMIC2},
    {PLUGIN_MIC_AMIC_3, UAPI_AI_AMIC3},
};
#endif

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
static int32_t g_dataSource = UAPI_AENC_DATA_SOURCE_BMIC_HDR_OUT;
#endif

static AudioVolumeContext g_audioVolumeCtx[] = {
    {MEDIA_VOLUME_MODULE_AO, 0, 0, UAPI_SND_OUT_PORT_DAC0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_AO, 0, 0, UAPI_SND_OUT_PORT_DAC0, 0, UAPI_AUDIO_VOL_ANA_PGA, 0},
    {MEDIA_VOLUME_MODULE_AO, 0, 0, UAPI_SND_OUT_PORT_DAC0, 0, UAPI_AUDIO_VOL_DIG_PGA, 0},
    {MEDIA_VOLUME_MODULE_AO, 0, AUDIO_IN_LIVE_MIC, UAPI_SND_OUT_PORT_CAST0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_AI, 0, 0, 0, UAPI_AUDIO_CH_ID0, UAPI_AUDIO_VOL_ANA_PGA, 0},
    {MEDIA_VOLUME_MODULE_AI, 0, 0, 0, UAPI_AUDIO_CH_ID1, UAPI_AUDIO_VOL_ANA_PGA, 0},
    {MEDIA_VOLUME_MODULE_AI, 0, 0, 0, UAPI_AUDIO_CH_ID0, UAPI_AUDIO_VOL_DIG_PGA, 0},
    {MEDIA_VOLUME_MODULE_AI, 0, 0, 0, UAPI_AUDIO_CH_ID0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_AI, 0, 0, 0, UAPI_AUDIO_CH_ID1, UAPI_AUDIO_VOL_DIG_PGA, 0},
    {MEDIA_VOLUME_MODULE_AENC, 0, AUDIO_IN_LIVE_MIC, UAPI_SND_OUT_PORT_CAST0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_AENC, 0, AUDIO_IN_LIVE_MIC, UAPI_AI_PORT_ADC0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_AENC, 0, AUDIO_IN_VOICE_RECORD, UAPI_AI_PORT_ADC0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_ADEC, 0, AUDIO_IN_LIVE_MIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_ADEC, 1, AUDIO_IN_LIVE_MIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_ADEC, 2, AUDIO_IN_LIVE_MIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_ADEC, 3, AUDIO_IN_LIVE_MIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_NOTIFICATION_SYSTEM, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_MUSIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_A2DP_MUSIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_NEARLINK_MUSIC, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_BT_VOICE_CALL, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
    {MEDIA_VOLUME_MODULE_TRACK, 0, AUDIO_IN_NEARLINK_VOICE_CALL, 0, 0, UAPI_AUDIO_VOL_SOFT_GAIN, 0},
};

static AencPortType g_aencType[] = {
    {AENC_TYPE_MODEM, UAPI_AI_PORT_ADC0},
    {AENC_TYPE_PRIMARY, UAPI_AI_PORT_ADC0},
    {AENC_TYPE_USB, UAPI_SND_OUT_PORT_CAST0},
    {AENC_TYPE_BT, UAPI_AI_PORT_ADC0},
};

static bool AudioVolumeConfigCompare(const AudioVolumeContext *ctx, AudioModule audioModule,
    uint32_t audioParam1, uint32_t audioParam2)
{
    if (ctx->audioModule == audioModule && audioParam1 == audioParam2) {
        return true;
    }
    return false;
}

static int32_t AudioSdkAdecGetIndex(const AudioVolumeContext *ctx)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
        AudioVolumeContext tempCtx = g_audioVolumeCtx[i];
        if (ctx->audioType == AUDIO_IN_LIVE_MIC) {
            if (tempCtx.deviceId == ctx->deviceId &&
                AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioType, ctx->audioType)) {
                return i;
            }
        } else {
            if (AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioType, ctx->audioType)) {
                return i;
            }
        }
    }
    return -1;
}

static int32_t GetAudioVolumeIndex(const AudioVolumeContext *ctx)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
        AudioVolumeContext tempCtx = g_audioVolumeCtx[i];
        switch (ctx->audioModule) {
            case MEDIA_VOLUME_MODULE_AO:
                if (tempCtx.audioPort == ctx->audioPort &&
                    AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioVolType, ctx->audioVolType)) {
                    return i;
                }
                break;
            case MEDIA_VOLUME_MODULE_AI:
                if (tempCtx.audioChannel == ctx->audioChannel &&
                    AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioVolType, ctx->audioVolType)) {
                    return i;
                }
                break;
            case MEDIA_VOLUME_MODULE_ADEC:
                return AudioSdkAdecGetIndex(ctx);
            case MEDIA_VOLUME_MODULE_AENC:
                if (AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioType, ctx->audioType) &&
                    tempCtx.audioPort == ctx->audioPort) {
                    return i;
                }
                break;
            case MEDIA_VOLUME_MODULE_TRACK:
                if (AudioVolumeConfigCompare(ctx, tempCtx.audioModule, tempCtx.audioType, ctx->audioType)) {
                    return i;
                }
                break;
            default:
                break;
        }
    }
    return -1;
}

static void WriteVolumeDataToNv(void)
{
#ifndef MELODY_DEMO_EARPHONE
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    audio_stream_common_t *nvData = (audio_stream_common_t *)MediaHalCalloc(nvDataSize);
    if (nvData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nvData malloc failed!");
        return;
    }
    uint32_t index = 0;
    uint32_t volumeSize = ARRAY_SIZE(g_audioVolumeCtx);
    for (uint32_t i = 0; i < volumeSize; i++) {
        int32_t value = g_audioVolumeCtx[i].volume;
        if (index <= nvDataSize) {
            nvData->value[index] = (int16_t)i;
            nvData->value[index + 1] = (int16_t)value;
            index += 0x2;
        }
    }
    errcode_t err = uapi_nv_write(NV_ID_MEDIA_STREAM_LIVE_MIC_VOLUME,
        (uint8_t *)nvData, nvDataSize);
    if (err != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_nv_write failed!");
    }
    MediaHalFree(nvData);
#endif
}

void RecordAudioDbVolume(const AudioVolumeContext *ctx)
{
    int32_t index = GetAudioVolumeIndex(ctx);
    if (index < 0 || (uint32_t)index >= ARRAY_SIZE(g_audioVolumeCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Not found support volume!, index = %d", index);
        return;
    }
    g_audioVolumeCtx[index].volume = ctx->volume;
    MEDIA_HAL_LOGI(MODULE_NAME, "record db Volume = %d, index = %d", g_audioVolumeCtx[index].volume, index);
    WriteVolumeDataToNv();
}

bool ReadAudioDbVolume(AudioVolumeContext *ctx)
{
    int32_t index = GetAudioVolumeIndex(ctx);
    if (index < 0 || (uint32_t)index >= ARRAY_SIZE(g_audioVolumeCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Not found support volume!, index = %d", index);
        return false;
    }
    ctx->volume = g_audioVolumeCtx[index].volume;
    MEDIA_HAL_LOGI(MODULE_NAME, "read db Volume = %d, index = %d", ctx->volume, index);
    return true;
}

#ifdef SUPPORT_NV
static void RecordVolume(const audio_stream_common_t *nvData)
{
    uint32_t index = 0;
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    uint32_t volumeSize = ARRAY_SIZE(g_audioVolumeCtx);
    for (uint32_t i = 0; i < volumeSize; i++) {
        int16_t key = 0;
        int16_t newValue = 0;
        if (index <= nvDataSize) {
            key = nvData->value[index];
            newValue = nvData->value[index + 1];
            index += 0x2;
        }
        g_audioVolumeCtx[key].volume = newValue;
    }
}

static void UpdateAudioDbVolume(void)
{
    uint16_t readLen = 0;
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    audio_stream_common_t *nvData = (audio_stream_common_t *)MediaHalCalloc(nvDataSize);
    if (nvData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nvData malloc failed!");
        return;
    }
    errcode_t nvRet = uapi_nv_read(NV_ID_MEDIA_STREAM_LIVE_MIC_VOLUME, nvDataSize,
        &readLen, (uint8_t *)nvData);
    if (nvRet == ERRCODE_SUCC) {
        RecordVolume(nvData);
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "hal volume nv get value wrong, ret[%d]", nvRet);
    }
    MediaHalFree(nvData);
}
#endif

static int32_t DbVolumeParse(char *value, AudioVolumeContext *tempCtx)
{
    int32_t ret = sscanf_s(value, "%d#%u#%u#%u#%u#%u", &tempCtx->volume, &tempCtx->audioModule,
        &tempCtx->audioChannel, &tempCtx->audioVolType, &tempCtx->audioPort, &tempCtx->deviceId);
    if (ret != SSCANF_NUM6) {
        MEDIA_HAL_LOGE(MODULE_NAME, "db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioSDKRecordAudioDbVolume(char *value, enum AudioCategory type)
{
    AudioVolumeContext tempCtx = {};
    int32_t ret = DbVolumeParse(value, &tempCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "DbVolumeParse failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    tempCtx.audioType = type;
    RecordAudioDbVolume(&tempCtx);
    return MEDIA_HAL_OK;
}

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
int32_t AudioSDKSetDbVolume(td_handle handle, char *value, enum AudioCategory type)
{
    AudioVolumeContext tempCtx = {};
    int32_t ret = DbVolumeParse(value, &tempCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "DbVolumeParse failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    tempCtx.audioType = type;
    uapi_audio_volume_cfg volumeConfig = {0, 0, 0, 0, 0};
    GetVolumeGain(tempCtx.volume, &volumeConfig.integer, &volumeConfig.decimal);
    volumeConfig.port = tempCtx.audioPort;
    volumeConfig.ch_id = tempCtx.audioChannel;
    volumeConfig.vol_type = tempCtx.audioVolType;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_audio_set_volume(handle, &volumeConfig));
    MEDIA_HAL_LOGI(MODULE_NAME, "set db volume integer:%d decimal:%d", volumeConfig.integer, volumeConfig.decimal);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_audio_set_volume failed");
    RecordAudioDbVolume(&tempCtx);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKGetDbVolume(char *value, uint32_t valueLen, enum AudioCategory type)
{
    int32_t dbVolume = 0;
    uint32_t channelCount = 0;
    uint32_t volumeType = 0;
    uint32_t port = 0;
    uint32_t volumeModule = 0;
    uint32_t deviceId = 0;
    int32_t ret = sscanf_s(value, "getdbvol = %d#%u#%u#%u#%u#%u", &dbVolume, &volumeModule,
        &channelCount, &volumeType, &port, &deviceId);
    if (ret != SSCANF_NUM6) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    AudioVolumeContext tempCtx = {volumeModule, deviceId, type, port,
        channelCount, volumeType, dbVolume};
    if (!ReadAudioDbVolume(&tempCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "read db volume failed!");
        return MEDIA_HAL_ERR;
    }
    dbVolume = tempCtx.volume;
    ret = sprintf_s(value, valueLen, "%d", dbVolume);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get db volume sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioSDKSetEncodeDataSourceParam(char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    int32_t ret = sscanf_s(value, "setencodeparam = %d", &g_dataSource);
    if (ret != SSCANF_NUM1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKSetEncodeDataSourceParam sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "set datasource success ! g_dataSource = %d", g_dataSource);
    return MEDIA_HAL_OK;
}
#endif

static void AudioSetVolume(AudioHandle handle, uint32_t index)
{
#ifdef AUDIO_SDK_SUPPORT
    uapi_audio_volume_cfg volumeConfig = {0, 0, 0, 0, 0};
    volumeConfig.ch_id = g_audioVolumeCtx[index].audioChannel;
    volumeConfig.port = g_audioVolumeCtx[index].audioPort;
    volumeConfig.vol_type = g_audioVolumeCtx[index].audioVolType;
    GetVolumeGain(g_audioVolumeCtx[index].volume, &volumeConfig.integer, &volumeConfig.decimal);
    int32_t ret = uapi_audio_set_volume((uintptr_t)handle, &volumeConfig);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set volume failed! volume = %d, index = %u",
            g_audioVolumeCtx[index].volume, index);
        return;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "volume = %d, index = %u", g_audioVolumeCtx[index].volume, index);
#else
    MEDIA_HAL_UNUSED(handle);
    MEDIA_HAL_UNUSED(index);
#endif
}

static void AudioSdkSetAencVolume(AudioHandle handle, AencType aencType, enum AudioCategory type)
{
    // Aenc目前的port只有两个，不是ADC0就是CAST0。
    uint32_t port = 0;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_aencType); i++) {
        if (aencType == g_aencType[i].aencType) {
            port = g_aencType[i].port;
            break;
        }
    }
    uint32_t volumeModule = MEDIA_VOLUME_MODULE_AENC;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
        if (volumeModule == g_audioVolumeCtx[i].audioModule) {
            if (port == g_audioVolumeCtx[i].audioPort && type == g_audioVolumeCtx[i].audioType) {
                AudioSetVolume(handle, i);
            }
        }
    }
}

static void AudioSdkSetAdecVolume(AudioHandle handle, uint32_t deviceId, enum AudioCategory type)
{
    // Adec需要考虑是不是直播麦的场景，如果是直播麦就需要进行deviceId的匹配
    uint32_t volumeModule = MEDIA_VOLUME_MODULE_ADEC;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
        if (type == AUDIO_IN_LIVE_MIC) {
            if (volumeModule == g_audioVolumeCtx[i].audioModule &&
                deviceId == g_audioVolumeCtx[i].deviceId &&
                type == g_audioVolumeCtx[i].audioType) {
                    AudioSetVolume(handle, i);
            }
        } else if (volumeModule == g_audioVolumeCtx[i].audioModule &&
            type == g_audioVolumeCtx[i].audioType) {
            AudioSetVolume(handle, i);
        }
    }
}

static void AudioSdkSetTrackVolumeInner(AudioHandle handle, enum AudioCategory type, uapi_snd_gain sndGain)
{
    // Track是设置各种通路sound的音量
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
#ifdef MELODY_DEMO_BMIC_RX
        MEDIA_HAL_UNUSED(sndGain);
        MEDIA_HAL_UNUSED(type);
        if (g_audioVolumeCtx[i].audioModule  == MEDIA_VOLUME_MODULE_AO) {
            AudioSetVolume(handle, i);
        }
#else
        if (g_audioVolumeCtx[i].audioModule == MEDIA_VOLUME_MODULE_TRACK &&
            type == g_audioVolumeCtx[i].audioType) {
            int32_t ret = uapi_snd_set_track_volume((uintptr_t)handle, &sndGain, NULL);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_track_volume failed!");
                return;
            }
            MEDIA_HAL_LOGI(MODULE_NAME, "streamType = 0x%x, set volume sndGain.integer = %d, decimal = %d success",
                type, sndGain.integer, sndGain.decimal);
            break;
        }
#endif
    }
}
#endif

static void AudioSdkSetAIVolume(AudioHandle handle, uint32_t channel)
{
#ifdef MELODY_AUDIO_SUPPORT
    // AI模块声道数是1时，两个声道都是1，而声道数是2时，一个声道id为1，一个为2。
    uint32_t channelTemp = channel;
#ifdef SUPPORT_FEATURE_CONFIG
    channelTemp = CONFIG_AUDIO_AI_CHANNEL_CNT;
#endif
    // TWS客户也需要调试数字增益与模拟增益
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_EARPHONE)
    uint32_t audioChannel1 = UAPI_AUDIO_CH_ID0;
    uint32_t audioChannel2 = UAPI_AUDIO_CH_ID0;
    if (channelTemp == AUDIO_CHANNEL_COUNT_2) {
        audioChannel2 = UAPI_AUDIO_CH_ID1;
    }
#endif
    uint32_t volumeType = UAPI_AUDIO_VOL_SOFT_GAIN;
    uint32_t volumeModule = MEDIA_VOLUME_MODULE_AI;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioVolumeCtx); i++) {
        int32_t ret = 0;
        if (volumeModule == g_audioVolumeCtx[i].audioModule) {
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_EARPHONE)
            if ((audioChannel1 == g_audioVolumeCtx[i].audioChannel ||
                audioChannel2 == g_audioVolumeCtx[i].audioChannel) &&
                volumeType != g_audioVolumeCtx[i].audioVolType) {
                AudioSetVolume(handle, i);
            }
#else
            if (volumeType == g_audioVolumeCtx[i].audioVolType) {
                MEDIA_HAL_UNUSED(channelTemp);
                uapi_ai_gain gain = {};
                GetVolumeGain(g_audioVolumeCtx[i].volume, &gain.integer, &gain.decimal);
                ret = uapi_ai_set_volume((uint32_t)(uintptr_t)handle, &gain);
            }
#endif
        }
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "uapi_ai_set_volume failed!");
        }
    }
#else
    MEDIA_HAL_UNUSED(channel);
    uapi_ai_gain gain = {};
    gain.integer = DEFAULT_AI_VOL_DB_INTERGER;
    gain.decimal = 0;
    MEDIA_HAL_LOGI(MODULE_NAME, "set default AI volume.integer = %d,  decimal = %d", gain.integer, gain.decimal);
    if (uapi_ai_set_volume((uint32_t)handle, &gain) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_set_volume failed");
    }
#endif
}

bool IsRawFormat(enum AudioFormat audioFormat)
{
    // The 24-bit PCM is processed as the ES stream and decoded by the ADEC.
    // The 16-bit PCM is processed as the ES stream for BT SCO msbc and cvsd pipeline same.
    if (audioFormat == AUDIO_FORMAT_TYPE_PCM_8_BIT) {
        return true;
    }
    return false;
}

#ifdef AUDIO_SDK_SUPPORT
void VolumePercentToDb(float volume, uapi_snd_gain *sndGain)
{
    if (sndGain == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sndGain is null");
        return;
    }
    double tmpDb = PERCENT_TO_GAIN_LOG_FACTOR * log(volume * VOLUME_UNIT / VOLUME_UNIT);
    if (tmpDb < VOLUME_DB_MIN) {
        tmpDb = VOLUME_DB_MIN;
    }
    (void)GetVolumeGain((int32_t)(tmpDb * MAGNIFY_TIMES), &sndGain->integer, &sndGain->decimal);
}

typedef struct {
    enum AudioCategory audioCategory;
    uapi_snd_aef_profile audioProfile;
} AudioCategoryPair;

#ifdef SUPPORT_MUTIL_SOUND
static enum AudioCategory g_audioSupportSound1Scene[] = {
    AUDIO_IN_MUSIC,
    AUDIO_IN_NOTIFICATION_SYSTEM
};
#else
static enum AudioCategory g_audioSupportSound1Scene[] = {};
#endif

static AudioCategoryPair g_audioCategoryPairs[] = {
    { AUDIO_IN_MUSIC, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_A2DP_MUSIC, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_NOTIFICATION_SYSTEM, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_NEARLINK_MUSIC, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_VOICE_RECORD, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_VOICE_RECOGNITION, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_FITNESS_VIDEO, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_USB_MUSIC, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_VOICE_CALL, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_BT_VOICE_CALL, UAPI_SND_AEF_PROFILE_VOIP },
    { AUDIO_IN_USB_VOICE_CALL, UAPI_SND_AEF_PROFILE_VOIP },
    { AUDIO_IN_NEARLINK_VOICE_CALL, UAPI_SND_AEF_PROFILE_VOIP},
    { AUDIO_IN_VOICE_ASSISTANT, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_VOICE_CALL_VOLTE, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_LIVE_MIC, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_VOICE_CALL_VOLTE_SPI, UAPI_SND_AEF_PROFILE_VOIP },
    { AUDIO_IN_OTHERS, UAPI_SND_AEF_PROFILE_MUSIC},
    { AUDIO_IN_TTS, UAPI_SND_AEF_PROFILE_MUSIC}
};

static AudioFormatMapping g_audioFormatMap[] = {
    {AUDIO_FORMAT_TYPE_PCM_8_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_TYPE_PCM_16_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_TYPE_PCM_24_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_TYPE_PCM_32_BIT_FLOAT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_TYPE_AAC_MAIN, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_AAC_LC, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_AAC_LD, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_AAC_ELD, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_AAC_HE_V1, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_AAC_HE_V2, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_TYPE_SBC, UAPI_ACODEC_ID_SBC},
    {AUDIO_FORMAT_TYPE_MP3, UAPI_ACODEC_ID_MP3},
    {AUDIO_FORMAT_TYPE_MSBC, UAPI_ACODEC_ID_MSBC},
    {AUDIO_FORMAT_TYPE_FLAC, UAPI_ACODEC_ID_FLAC},
    {AUDIO_FORMAT_TYPE_OPUS, UAPI_ACODEC_ID_OPUS},
    {AUDIO_FORMAT_TYPE_VORBIS, UAPI_ACODEC_ID_VORBIS},
    {AUDIO_FORMAT_TYPE_APE, UAPI_ACODEC_ID_APE},
    {AUDIO_FORMAT_TYPE_SILK, UAPI_ACODEC_ID_SILK},
    {AUDIO_FORMAT_TYPE_L2HC, UAPI_ACODEC_ID_L2HC},
    {AUDIO_FORMAT_TYPE_AMR_WB, UAPI_ACODEC_ID_AMRWB},
    {AUDIO_FORMAT_TYPE_LHDC, UAPI_ACODEC_ID_LHDC},
    {AUDIO_FORMAT_TYPE_LDAC, UAPI_ACODEC_ID_LDAC},
    {AUDIO_FORMAT_TYPE_L2HC_VOICE, UAPI_ACODEC_ID_L2HC_VOICE},
    {AUDIO_FORMAT_TYPE_G711A, UAPI_ACODEC_ID_G711},
    {AUDIO_FORMAT_TYPE_G711U, UAPI_ACODEC_ID_G722},
    {AUDIO_FORMAT_TYPE_G726,  UAPI_ACODEC_ID_G726},
};

bool AudioConvertAudioFormatToCodecId(enum AudioFormat audioFormat, uapi_acodec_id *codecId)
{
    if (codecId == NULL) {
        return false;
    }
    for (uint32_t i = 0; i < sizeof(g_audioFormatMap) / sizeof(AudioFormatMapping); i++) {
        if (g_audioFormatMap[i].audioFormat == audioFormat) {
            *codecId = g_audioFormatMap[i].codecId;
            return true;
        }
    }
    MEDIA_HAL_LOGW(MODULE_NAME, "Invalid audioFormat: 0x%x", audioFormat);
    return false;
}

bool FindSupportSound1Scene(enum AudioCategory audioCategory)
{
    for (uint32_t i = 0; i < sizeof(g_audioSupportSound1Scene) / sizeof(enum AudioCategory); i++) {
        if (g_audioSupportSound1Scene[i] == audioCategory) {
            return true;
        }
    }
    return false;
}

static void UpdateVqeParam(void)
{
#ifndef MELODY_DEMO_BMIC_TX
    uint16_t len = 0;
    errcode_t nvRet = uapi_nv_read(NV_ID_AUDIO_VQE_PARAM,
        (uint16_t)sizeof(audio_vqe_param_t), &len, (uint8_t *)&g_vqeParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nv read sea param faild!, ret:%u", nvRet);
    }
#ifdef SUPPORT_SEA_AHE_PARAM
    nvRet = uapi_nv_read(NV_ID_AUDIO_ANR_MODE_PARA, sizeof(uint8_t), &len, &g_anrMode);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nv read ahe mode param faild!");
    }
    nvRet = uapi_nv_read(NV_ID_AUDIO_AHE_USR_PARAM, (uint16_t)sizeof(audio_ahe_user_param_t), &len, &g_aheUserParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nv read ahe user param faild!");
    }
    uint16_t nvId = g_anrMode < AUDIO_NR_MODE_MAX ?
        g_aheNrNvIdMap[g_anrMode] : NV_ID_AUDIO_NR_OFF_MIC4STREAM_PARA;
    nvRet = uapi_nv_read(nvId, (uint16_t)sizeof(nr_mic4stream_param_t), &len, &g_aheNrParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nv read ahe nr param faild!");
    }
    nvId = g_anrMode < AUDIO_NR_MODE_MAX ?
        g_aheAgcNvIdMap[g_anrMode] : NV_ID_AUDIO_NR_OFF_MIC4STREAM_AGC_PARA;
    nvRet = uapi_nv_read(nvId, (uint16_t)sizeof(audio_agc_param_t), &len, &g_aheAgcParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "nv read ahe agc param faild!");
    }
#endif
#endif
}

static void UpdateAefParam(void)
{
#ifdef SUPPORT_AUDIO_EFFECT_PARAM
    uint16_t len = 0;
    uint16_t nvKey = 0;
    uint16_t nvSize = 0;
    if (SND_PORT_AEF_TYPE == UAPI_AEF_TYPE_SWS) {
        nvKey = (uint16_t)NV_ID_AUDIO_AEF_SWS_PARAM;
        nvSize = (uint16_t)sizeof(audio_sws_param_t);
        MEDIA_HAL_LOGI(MODULE_NAME, "nv read sws");
    } else {
        nvKey = (uint16_t)NV_ID_AUDIO_AEF_HVS_PARAM;
        nvSize = (uint16_t)sizeof(audio_hvs_param_t);
        MEDIA_HAL_LOGI(MODULE_NAME, "nv read hvs");
    }
    errcode_t nvRet = uapi_nv_read(nvKey, nvSize, &len, (uint8_t *)&g_aefParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "nv read aef param faild!, ret:%u", nvRet);
    }
#endif
}

static void UpdateAudioParam(void)
{
#ifdef MELODY_AUDIO_SUPPORT
    UpdateAudioDbVolume();
#endif
    uint16_t len = 0;
    errcode_t nvRet = uapi_nv_read(NV_ID_FTM_CONFIG, sizeof(uint8_t), &len, &g_disableSea);
    if (nvRet != ERRCODE_SUCC) {
        g_disableSea = 0;
    }
    UpdateVqeParam();
    UpdateAefParam();
#ifdef SUPPORT_VOLUME_CALIBRATION
    nvRet = uapi_nv_read(NV_ID_MAX_VOL_CONFIG, sizeof(int32_t), &len, &g_maxVolumeDb);
    if (nvRet != ERRCODE_SUCC) {
        MEDIA_HAL_LOGW(MODULE_NAME, "nv read max volume faild!");
        g_maxVolumeDb = 0;
    }
#endif
}

static int32_t AudioSDKInputInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_init failed");
        return ret;
    }
#ifdef AUDIO_SEA_SUPPORT
#ifdef SUPPORT_NV
    UpdateAudioParam();
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_sea_init failed");
        goto AI_DEINIT;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_init failed");
        goto SEA_DEINIT;
    }
    return MEDIA_HAL_OK;
SEA_DEINIT:
#ifdef AUDIO_SEA_SUPPORT
    (void)uapi_sea_deinit;
AI_DEINIT:
#endif
    (void)uapi_ai_deinit();
    return ret;
}

static int32_t AudioSDKInputDeInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_aenc_deinit failed");

#ifdef AUDIO_SEA_SUPPORT
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_deinit failed");
#endif

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_ai_deinit failed");

    return MEDIA_HAL_OK;
}

static int32_t AudioSDKOutputInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adec_init failed");
        return ret;
    }

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_init failed");
        goto ADEC_DEINIT;
    }

    return MEDIA_HAL_OK;

ADEC_DEINIT:
    (void)uapi_adec_deinit();
    return ret;
}

static int32_t AudioSDKOutputDeInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_snd_deinit failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adec_deinit failed");

    return MEDIA_HAL_OK;
}

int32_t AudioSDKInit(void)
{
    int32_t ret;

    MediaHalInitStaticMutexLock(&g_audioSdkInitLock);
    MediaHalMutexLock(g_audioSdkInitLock);
    if (g_audioSdkIsInited) {
        MEDIA_HAL_LOGI(MODULE_NAME, "audio sdk have inited");
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_init());
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(g_audioSdkInitLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adp_init failed");
        return ret;
    }

    ret = AudioSDKInputInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKInputInit failed");
        goto ADP_DEINIT;
    }

    ret = AudioSDKOutputInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKOutputInit failed");
        goto INPUT_DEINIT;
    }

    g_audioSdkIsInited = true;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_INIT,
        MEDIA_CHR_DEVICE_MANAGER_INIT, MEDIA_CHR_SDK_INIT);
    return MEDIA_HAL_OK;

INPUT_DEINIT:
    (void)AudioSDKInputDeInit();
ADP_DEINIT:
    (void)uapi_adp_deinit();
    MediaHalMutexUnLock(g_audioSdkInitLock);
    return ret;
}

int32_t AudioSDKDeInit(void)
{
    int32_t ret;
    MediaHalInitStaticMutexLock(&g_audioSdkInitLock);
    MediaHalMutexLock(g_audioSdkInitLock);

    if (!g_audioSdkIsInited) {
        MEDIA_HAL_LOGI(MODULE_NAME, "audio sdk not inited");
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }

    ret = AudioSDKInputDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKInputDeInit failed");

    ret = AudioSDKOutputDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKOutputDeInit failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adp_deinit failed");

    g_audioSdkIsInited = false;
#ifdef MELODY_AUDIO_SUPPORT
    WriteVolumeDataToNv();
#endif
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, MEDIA_CHR_SDK_DEINIT, 0);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#if (defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)) || \
    defined(MELODY_DEMO_HA) && defined(SUPPORT_NV)
static void SetAIMicAttr(const AIOpenAttr *aiOpenAttr, uapi_ai_attr *aiAttr)
{
    uint32_t j = 0;
    for (uint32_t i = 0; i < UAPI_AI_MIC_NUM_MAX; i++) {
        if (i > CONFIG_AUDIO_AI_CHANNEL_CNT) {
            aiAttr->port_attr.adc.mic_sel[i] = UAPI_AI_AMIC_MAX;
            continue;
        }
        for (; j < ARRAY_SIZE(g_micType); j++) {
            if ((aiOpenAttr->micType & g_micType[j].micType) != 0) {
                aiAttr->port_attr.adc.mic_sel[i] = g_micType[j].micSel;
                j++;
                break;
            }
        }
    }
    // 如果 声道 等于 2 且 是直播麦 或者 录音， 两个MIC 是相等的；
    if (aiAttr->pcm_attr.channels == AUDIO_CHANNEL_COUNT_2 &&
        (aiOpenAttr->inputAttr.type == AUDIO_IN_VOICE_RECORD || aiOpenAttr->inputAttr.type == AUDIO_IN_LIVE_MIC)) {
        aiAttr->port_attr.adc.mic_sel[INDEX_1] = aiAttr->port_attr.adc.mic_sel[0];
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "mic0 = %d, mic1 = %d", aiAttr->port_attr.adc.mic_sel[0], aiAttr->port_attr.adc.mic_sel[INDEX_1]);
}
#endif

static int32_t ConfigAIAttr(const AIOpenAttr *aiOpenAttr, uapi_ai_port *aiPort, uapi_ai_attr *aiAttr)
{
#ifdef MELODY_AUDIO_SUPPORT
#ifdef SUPPORT_ADC0_AI_PORT
    *aiPort = CONFIG_AUDIO_AI_IN_PORT;
#else
    // workaround: currently, melody ai port is UAPI_AI_PORT_LPADC0, and then it'll be unified with brandy.
    *aiPort = UAPI_AI_PORT_LPADC0;
    if (aiOpenAttr->micType != 0) {
        *aiPort = UAPI_AI_PORT_LPADC1;
    }
#endif
#else
    MEDIA_HAL_UNUSED(aiOpenAttr);
    *aiPort = BUILTIN_AI_PORT;
#endif
    int32_t ret = uapi_ai_get_default_attr(*aiPort, aiAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_get_default_attr failed! ret = %d", ret);
        return ret;
    }
    GetBuiltinAIAttr(aiAttr, aiOpenAttr);
#if (defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)) || \
    defined(MELODY_DEMO_HA) && defined(SUPPORT_NV)
    SetMicArrangementOrder(aiAttr);
    if (aiOpenAttr->micType != 0) {
        SetAIMicAttr(aiOpenAttr, aiAttr);
    }
#endif
#ifdef CONFIG_BOARD_DEMO
    aiAttr->port_attr.adc.mic_sel[0] = UAPI_AI_AMIC1;
    aiAttr->port_attr.adc.mic_sel[1] = UAPI_AI_AMIC2;
#endif
#ifdef MELODY_DEMO_SOUNDBAR
        // soundbar 使用AI 输入，需要配置I2S参数
        uapi_audio_i2s_attr soundbarI2s = {
            .master = 0,
            .i2s_mode = UAPI_AUDIO_I2S_TDM_MODE,
            .mclk = UAPI_AUDIO_I2S_MCLK_256_FS,
            .bclk = UAPI_AUDIO_I2S_BCLK_8_DIV,
            .bit_depth = UAPI_AUDIO_BIT_DEPTH_32,
            .channels = UAPI_AUDIO_CHANNEL_4,
            .pcm_sample_rise_edge = 1,
            .pcm_delay_cycle = UAPI_AUDIO_I2S_PCM_1_DELAY,
        };
        aiAttr->port_attr.i2s.i2s_attr = soundbarI2s;
        *aiPort = UAPI_AI_PORT_I2S1;
#endif

    return ret;
}

int32_t AudioSDKAIOpen(td_handle *aiHandle, const AIOpenAttr *aiOpenAttr)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackInLock);
    MediaHalMutexLock(g_subPortTrackInLock);
    if (g_subPortTrackInOpened == 0) {
        CHK_NULL_RETURN(aiOpenAttr, MEDIA_HAL_INVALID_PARAM, "Input param port is null");
        uapi_ai_attr aiAttr = {};
        uapi_ai_port aiPort;
        int32_t ret = ConfigAIAttr(aiOpenAttr, &aiPort, &aiAttr);
        if (ret != MEDIA_HAL_OK) {
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return ret;
        }
        if ((aiOpenAttr->audioCategory & AUDIO_IN_BT_VOICE_CALL) == AUDIO_IN_BT_VOICE_CALL ||
            (aiOpenAttr->audioCategory & AUDIO_IN_VOICE_RECOGNITION) == AUDIO_IN_VOICE_RECOGNITION ||
            (aiOpenAttr->audioCategory & AUDIO_IN_VOICE_ASSISTANT) == AUDIO_IN_VOICE_ASSISTANT ||
            (aiOpenAttr->audioCategory & AUDIO_IN_VOICE_CALL_VOLTE_SPI) == AUDIO_IN_VOICE_CALL_VOLTE_SPI ||
#ifndef MELODY_AUDIO_SUPPORT
            (aiOpenAttr->audioCategory & AUDIO_IN_VOICE_RECORD) == AUDIO_IN_VOICE_RECORD ||
#endif
            (aiOpenAttr->audioCategory & AUDIO_IN_NEARLINK_VOICE_CALL) == AUDIO_IN_NEARLINK_VOICE_CALL ||
            (aiOpenAttr->audioCategory & AUDIO_IN_PICKUP) == AUDIO_IN_PICKUP) {
            aiAttr.ref_attr.enable = aiOpenAttr->isNeedOpenSea ? TD_TRUE : TD_FALSE;
#ifdef MELODY_AUDIO_SUPPORT
            aiAttr.ref_attr.port = CONFIG_AUDIO_AI_REF_PORT;
            if (aiOpenAttr->isNeedSetRXType) {
                aiAttr.port_attr.adc.rx_type = UAPI_AI_RX_DEFAULT;
            }
#elif defined AUDIO_SEA_SUPPORT
            aiAttr.ref_attr.enable = TD_TRUE;
            aiAttr.ref_attr.port = BUILTIN_SND_OUT_PORT;
#endif
#endif
        } else {
            aiAttr.ref_attr.enable = TD_FALSE;
        }
        if (uapi_ai_open(aiHandle, aiPort, &aiAttr) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_open failed");
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return MEDIA_HAL_ERR;
        }
        MEDIA_HAL_LOGI(MODULE_NAME, "bit_depth = %u, channels = %u, rate = %u, sample_per_frame = %u", aiAttr.pcm_attr.bit_depth,
            aiAttr.pcm_attr.channels, aiAttr.pcm_attr.sample_rate, aiAttr.pcm_attr.sample_per_frame);
    }
    g_subPortTrackInOpened++;
    AudioSdkSetAIVolume((AudioHandle)(uintptr_t)*aiHandle, aiOpenAttr->inputAttr.channelCount);
    MediaHalMutexUnLock(g_subPortTrackInLock);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAIClose(td_handle aiHandle)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackInLock);
    MediaHalMutexLock(g_subPortTrackInLock);
    g_subPortTrackInOpened--;
    if (g_subPortTrackInOpened == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_close(aiHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_close failed");
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return ret;
        }
        MEDIA_HAL_LOGT(MODULE_NAME, "ai close success");
    }
    MediaHalMutexUnLock(g_subPortTrackInLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static uapi_audio_scene GetScencTypeByCategoryInMusic(enum AudioCategory audioCategory)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
#ifdef MELODY_AUDIO_SUPPORT
            return UAPI_AUDIO_SCENE_UNKNOWN;
#else
            return UAPI_AUDIO_SCENE_LOCAL_MUSIC;
#endif
        case AUDIO_IN_A2DP_MUSIC:
            return UAPI_AUDIO_SCENE_BT_MUSIC_SNK;
        case AUDIO_IN_NEARLINK_MUSIC:
#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
            return UAPI_AUDIO_SCENE_SLE_MUSIC_SRC;
#else
            (void)uapi_audio_set_tws_mode(UAPI_AUDIO_TWS_MODE_DUAL_TRANSMIT);
            return UAPI_AUDIO_SCENE_SLE_MUSIC_SINK;
#endif
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "not support audioCategory = %d", audioCategory);
            return UAPI_AUDIO_SCENE_UNKNOWN;
    }
}

uapi_audio_scene GetScencTypeByCategory(const struct PortPluginAttr *inputAttr, bool toggleValid, bool isNearLinkUplink)
{
    switch (inputAttr->type) {
        case AUDIO_IN_MUSIC:
        case AUDIO_IN_A2DP_MUSIC:
        case AUDIO_IN_NEARLINK_MUSIC:
            return GetScencTypeByCategoryInMusic(inputAttr->type);
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            return toggleValid ? UAPI_AUDIO_SCENE_SYNC_TONE : UAPI_AUDIO_SCENE_LOCAL_TONE;
        case AUDIO_IN_VOICE_RECORD:
#ifdef MELODY_DEMO_BMIC_TX
            return UAPI_AUDIO_SCENE_LIVE_MIC_REC;
#else
            return UAPI_AUDIO_SCENE_UNKNOWN;
#endif
        case AUDIO_IN_BT_VOICE_CALL:
        case AUDIO_IN_VOICE_CALL_VOLTE_SPI:
            return UAPI_AUDIO_SCENE_BT_VOIP;
        case AUDIO_IN_NEARLINK_VOICE_CALL:
#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
            if (isNearLinkUplink) {
                return UAPI_AUDIO_SCENE_SLE_DONGLE_UL;
            }
            return UAPI_AUDIO_SCENE_SLE_MUSIC_SRC;
#else
            MEDIA_HAL_UNUSED(isNearLinkUplink);
            (void)uapi_audio_set_tws_mode(UAPI_AUDIO_TWS_MODE_DUAL_TRANSMIT);
            if (inputAttr->audioFormat == AUDIO_FORMAT_TYPE_L2HC_VOICE) {
                return UAPI_AUDIO_SCENE_SLE_VOIP;
            }
            return UAPI_AUDIO_SCENE_SLE_MUSIC_SINK;
#endif
        case AUDIO_IN_USB_MUSIC:
        case AUDIO_IN_USB_VOICE_CALL:
        case AUDIO_IN_VOICE_ASSISTANT:
        case AUDIO_IN_VOICE_CALL_VOLTE:
        case AUDIO_IN_VOICE_RECOGNITION:
        case AUDIO_IN_FITNESS_VIDEO:
        case AUDIO_IN_VOICE_CALL:
        case AUDIO_IN_PICKUP:
            return UAPI_AUDIO_SCENE_UNKNOWN;
        case AUDIO_IN_LIVE_MIC:
            return UAPI_AUDIO_SCENE_LIVE_MIC;
        case AUDIO_IN_TTS:
            return UAPI_AUDIO_SCENE_LOCAL_MUSIC;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "not support type = %d", inputAttr->type);
            return UAPI_AUDIO_SCENE_UNKNOWN;
    }
}

int32_t AudioSDKSetAIScene(td_handle aihandle, const struct PortPluginAttr *inputAttr, bool isNearLinkUplink)
{
    uapi_audio_scene_config asc = {
        .scene_type = GetScencTypeByCategory(inputAttr, false, isNearLinkUplink),
        .scene_id = 0,
    };
#ifdef MELODY_DEMO_BMIC_TX
    if (inputAttr->type == AUDIO_IN_LIVE_MIC) {
        asc.scene_id = 0;
    } else if (inputAttr->type == AUDIO_IN_VOICE_RECORD) {
        asc.scene_id = 1;
    }
#endif
    int32_t ret = uapi_audio_set_audio_scene(aihandle, &asc);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_audio_set_audio_scene failed, ret = 0x%x", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioSDKSetAOScene(td_handle adpHandle, td_handle sndHandle, const struct PortPluginAttr *inputAttr,
    bool toggleValid)
{
    uapi_audio_scene_config asc = {
        .scene_type = GetScencTypeByCategory(inputAttr, toggleValid, false),
        .scene_id = inputAttr->deviceId,
        .scene_sub_id = inputAttr->connectedId,
    };
    int32_t ret = uapi_audio_set_audio_scene(adpHandle, &asc);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_audio_set_audio_scene failed, ret = 0x%x", ret);
        return ret;
    }
    if (sndHandle == 0) {
        return ret;
    }
    ret = uapi_snd_set_enable(sndHandle, true);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_enable failed, ret = 0x%x", ret);
        return ret;
    }
    return ret;
}

static uapi_snd_aef_profile ConvertAudioCategoryToAudioEffectProfile(const enum AudioCategory audioCategory)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioCategoryPairs); i++) {
        if (audioCategory == g_audioCategoryPairs[i].audioCategory) {
            return g_audioCategoryPairs[i].audioProfile;
        }
    }

    return UAPI_SND_AEF_PROFILE_MAX;
}

static int32_t SetSndAefProfile(td_handle sndHandle, const enum AudioCategory audioCategory,
    const uapi_snd_attr *sndAttr)
{
    int32_t ret = MEDIA_HAL_OK;
    if (audioCategory == AUDIO_IN_PICKUP) {
        MEDIA_HAL_LOGI(MODULE_NAME, "no need to set aef param with AUDIO_IN_PICKUP type");
        return MEDIA_HAL_OK;
    }
#ifndef MELODY_AUDIO_SUPPORT
    if ((audioCategory & AUDIO_IN_COMMUNICATION) == AUDIO_IN_COMMUNICATION) {
        ret = uapi_snd_set_track_mode(sndHandle, sndAttr->port_attr[0].out_port, SND_TRACK_MODE_BT_SCO);
    } else if ((audioCategory & AUDIO_IN_MEDIA) == AUDIO_IN_MEDIA) {
        ret = uapi_snd_set_track_mode(sndHandle, sndAttr->port_attr[0].out_port, SND_TRACK_MODE_LOCAL_MUSIC);
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set track mode failed, ret = %d", ret);
        return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(sndAttr);
#endif
    uapi_snd_aef_profile profile = ConvertAudioCategoryToAudioEffectProfile(audioCategory);
    if (profile == UAPI_SND_AEF_PROFILE_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio category %d", audioCategory);
        return MEDIA_HAL_INVALID_PARAM;
    }
    // 提示音不需要开启aef音效，会消耗内存
    if (audioCategory != AUDIO_IN_NOTIFICATION_SYSTEM) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_aef_profile(sndHandle, profile));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_aef_profile(%d) failed: 0x%x", profile, ret);
        }
    }
#ifndef MELODY_AUDIO_SUPPORT
#ifndef HI3322_CHIP_V100
    ret = uapi_snd_set_port_aef_enable(sndHandle, BUILTIN_SND_OUT_PORT,
        SND_PORT_AEF_TYPE, SND_PORT_AEF_ENABLE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_port_aef_enable(%d) failed: 0x%x", SND_PORT_AEF_TYPE, ret);
    }
#endif
    ret = uapi_snd_set_aef_bypass(sndHandle, UAPI_SND_OUT_PORT_ALL, false);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_aef_bypass(%d) failed: 0x%x", SND_PORT_AEF_TYPE, ret);
    }
#ifdef SUPPORT_AUDIO_EFFECT_PARAM
    SetAefExtraParam(sndHandle);
#endif
#endif
    return MEDIA_HAL_OK;
}

static int32_t GetSoundUsedCnt(const AOdevContext *aoDevCtx)
{
    if (FindSupportSound1Scene(aoDevCtx->audioCategory)) {
        return aoDevCtx->snd1UsedCnt;
    } else {
        return aoDevCtx->snd0UsedCnt;
    }
}

static void RemoveSoundCnt(AOdevContext *aoDevCtx)
{
    if (FindSupportSound1Scene(aoDevCtx->audioCategory)) {
        aoDevCtx->snd1UsedCnt--;
    } else {
        aoDevCtx->snd0UsedCnt--;
    }
}

static void RecordSoundHandle(AOdevContext *aoDevCtx, td_handle sndHandle)
{
    if (FindSupportSound1Scene(aoDevCtx->audioCategory)) {
        if (sndHandle != 0) {
            aoDevCtx->sndHandle1 = sndHandle;
        }
        aoDevCtx->snd1UsedCnt++;
    } else {
        if (sndHandle != 0) {
            aoDevCtx->sndHandle0 = sndHandle;
        }
        aoDevCtx->snd0UsedCnt++;
    }
}
static void InitSoundId(AOdevContext *aoDevCtx)
{
    if (FindSupportSound1Scene(aoDevCtx->audioCategory)) {
        aoDevCtx->sndDevId = UAPI_SND_1;
    } else {
        aoDevCtx->sndDevId = UAPI_SND_0;
    }
}

int32_t AudioSDKAODevOpen(AOdevContext *aoDevCtx, const struct PortPluginAttr *inputAttr)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackOutLock);
    MediaHalMutexLock(g_subPortTrackOutLock);
    td_handle sndHandle = GetSoundHandle(aoDevCtx);
    if (GetSoundUsedCnt(aoDevCtx) == 0) {
        int32_t ret;
        uapi_snd_attr sndAttr = {};
        InitSoundId(aoDevCtx);
        if (sndHandle != 0) {
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return MEDIA_HAL_OK;
        }
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_default_attr(aoDevCtx->sndDevId, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_get_default_attr failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
        GetBuiltinAOAttr(&sndAttr, inputAttr);
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(&(sndHandle), aoDevCtx->sndDevId, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_open failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
        ret = SetSndAefProfile(sndHandle, aoDevCtx->audioCategory, &sndAttr);
        if (ret != MEDIA_HAL_OK) {
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
        MEDIA_HAL_LOGT(MODULE_NAME, "sound[%d] open success", aoDevCtx->sndDevId);
    }
    RecordSoundHandle(aoDevCtx, sndHandle);
    MediaHalMutexUnLock(g_subPortTrackOutLock);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAODevClose(AOdevContext *aoDevCtx)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackOutLock);
    MediaHalMutexLock(g_subPortTrackOutLock);
    RemoveSoundCnt(aoDevCtx);
    if (GetSoundUsedCnt(aoDevCtx) == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(GetSoundHandle(aoDevCtx)));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_close failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
        enum AudioCategory audioCategory = aoDevCtx->audioCategory;
        if (FindSupportSound1Scene(audioCategory)) {
            aoDevCtx->sndHandle1 = 0;
        } else {
            aoDevCtx->sndHandle0 = 0;
        }
    }
    MediaHalMutexUnLock(g_subPortTrackOutLock);
    MEDIA_HAL_LOGT(MODULE_NAME, "sound[%d] close success", aoDevCtx->sndDevId);
    return MEDIA_HAL_OK;
}

td_handle GetSoundHandle(const AOdevContext *aoDevCtx)
{
    if (FindSupportSound1Scene(aoDevCtx->audioCategory)) {
        return aoDevCtx->sndHandle1;
    } else {
        return aoDevCtx->sndHandle0;
    }
}

int32_t AudioSDKAdpOpen(td_handle *adpHandle)
{
    int32_t ret;
    uapi_adp_attr adpAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_get_def_attr(&adpAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_get_def_attr failed");
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_create(adpHandle, &adpAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_create failed");

    MEDIA_HAL_LOGT(MODULE_NAME, "create adpHandle: 0x%x", *adpHandle);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAdpClose(td_handle adpHandle)
{
    int32_t ret;
    MEDIA_HAL_LOGI(MODULE_NAME, "adpHandle: 0x%x", adpHandle);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_destroy(adpHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_destroy failed");

    MEDIA_HAL_LOGT(MODULE_NAME, "destroy adpHandle: 0x%x", adpHandle);
    return MEDIA_HAL_OK;
}

static bool IsSupportedCodecFormat(struct PortPluginAttr inputAttr)
{
    if (inputAttr.audioFormat == AUDIO_FORMAT_TYPE_PCM_24_BIT) {
        return false;
    }

    return true;
}

#ifdef MELODY_DEMO_EARPHONE
static int32_t AudioSDKAcquireLhdcLicense(uapi_acodec_lhdc_dec_priv_cfg *licenseInfo)
{
    if (efuse_read_item(EFUSE_DIE_ID, licenseInfo->uuid, DIE_ID_LENGTH) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "efuse_read_item failed!");
        return MEDIA_HAL_ERR;
    }
    licenseInfo->uuid_bytes = DIE_ID_LENGTH;
    lic_info *info = (lic_info *)MediaHalCalloc(sizeof(lic_info));
    if (info == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc lic_info failed!");
        return MEDIA_HAL_ERR;
    }
    if (sysc_get_license(info) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sysc_get_license failed!");
        MediaHalFree(info);
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(licenseInfo->lhdc_license, UAPI_CODEC_LHDC_LICENSE_MAX_SIZE, &info->lic, info->len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed!, len = %d", info->len);
        MediaHalFree(info);
        return MEDIA_HAL_ERR;
    }
    licenseInfo->lhdc_license_bytes = info->len;
    uint16_t len = 0;
    errcode_t nvRet = uapi_nv_read(NV_ID_UX_BS_NAME, DEVICE_NAME_LEN, &len, licenseInfo->bt_name);
    if (nvRet != ERRCODE_SUCC || len > DEVICE_NAME_LEN) {
        MEDIA_HAL_LOGE(MODULE_NAME, "btName read failed!, len = %d", len);
        MediaHalFree(info);
        return MEDIA_HAL_ERR;
    }
    licenseInfo->bt_name_bytes = len;
    MediaHalFree(info);
    return MEDIA_HAL_OK;
}
#endif

#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
static void AudioSDKAdecDongleInitAttr(struct PortPluginAttr inputAttr, uapi_adec_attr *adecAttr)
{
    if (!inputAttr.isDongleUpLink) { // 如果是dongle下行，adec是PCM格式，需要配置private_data参数
        adecAttr->codec_id = UAPI_ACODEC_ID_PCM;
        adecAttr->param.bit_depth = AUDIO_SAMPLE_BITDEPTH_24;
        adecAttr->param.channels = AUDIO_CHANNEL_COUNT_2;
        g_adecPrivate.channels = AUDIO_CHANNEL_COUNT_2;
        g_adecPrivate.bit_depth = AUDIO_SAMPLE_BITDEPTH_32;
        g_adecPrivate.sample_rate = AUDIO_SAMPLE_RATE_48K;
        g_adecPrivate.sample_per_frame = AUDIO_SAMPLE_RATE_48K / CONFIG_AUDIO_AI_FRAME_PER_SEC;
        adecAttr->param.private_data = &g_adecPrivate;
        adecAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
    } else if (adecAttr->codec_id == UAPI_ACODEC_ID_L2HC && g_l2hcAdecIsSetted) {
        adecAttr->param.private_data = &g_l2hcAdecConfig;
        adecAttr->param.private_data_size = sizeof(uapi_acodec_l2hc_priv_cfg);
    }
}
#endif

static int32_t AudioSDKAdecInitAttr(uapi_acodec_id adecCodecId,
    struct PortPluginAttr inputAttr, uapi_adec_attr *adecAttr)
{
#ifdef SUPPORT_UX_CONFIG
    adecAttr->max_mute_dur_s = SOUND_MUTE_DURATION;
#endif
    adecAttr->codec_id = adecCodecId;
    if (inputAttr.audioFormat == AUDIO_FORMAT_TYPE_FLAC) {
        adecAttr->frame_mode_input = true;
    }
    adecAttr->param.sample_rate = inputAttr.sampleRate;
    adecAttr->param.channels = inputAttr.channelCount;
    if (inputAttr.type == AUDIO_IN_LIVE_MIC) {
        adecAttr->param.channels = 1;
    }
    adecAttr->param.bit_depth = inputAttr.bitWidth;
#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    AudioSDKAdecDongleInitAttr(inputAttr, adecAttr);
#endif
#ifdef MELODY_DEMO_EARPHONE
    if (adecAttr->codec_id == UAPI_ACODEC_ID_LHDC) { // 如果是LHDC格式的解码需要配置license
        if (AudioSDKAcquireLhdcLicense(&g_licenseInfo) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAcquireLhdcLicense failed!");
            return MEDIA_HAL_ERR;
        }
        adecAttr->param.private_data = &g_licenseInfo;
        adecAttr->param.private_data_size = sizeof(uapi_acodec_lhdc_dec_priv_cfg);
    } else if (adecAttr->codec_id == UAPI_ACODEC_ID_PCM && inputAttr.type == AUDIO_IN_USB_VOICE_CALL) {
        adecAttr->param.bit_depth = inputAttr.bitWidth;
        adecAttr->param.channels = inputAttr.channelCount;
        g_adecPrivate.channels = AUDIO_CHANNEL_COUNT_2;
        g_adecPrivate.bit_depth = AUDIO_SAMPLE_BITDEPTH_24;
        g_adecPrivate.sample_rate = AUDIO_SAMPLE_RATE_48K;
        g_adecPrivate.sample_per_frame = AUDIO_SAMPLE_RATE_48K / CONFIG_AUDIO_AI_FRAME_PER_SEC;
        adecAttr->param.private_data = &g_adecPrivate;
        adecAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
    } else if ((adecAttr->codec_id == UAPI_ACODEC_ID_L2HC_VOICE || adecAttr->codec_id == UAPI_ACODEC_ID_L2HC) &&
        g_l2hcAdecIsSetted) {
        adecAttr->param.private_data = &g_l2hcAdecConfig;
        adecAttr->param.private_data_size = sizeof(uapi_acodec_l2hc_priv_cfg);
    }
#endif
    if (inputAttr.isNeedSetExtraParam) {
        if (inputAttr.extraData != NULL && inputAttr.extraDataSize != 0) {
            adecAttr->param.private_data = inputAttr.extraData;
            adecAttr->param.private_data_size = inputAttr.extraDataSize;
        }
    }
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAdecOpen(td_handle *adecHandle, struct PortPluginAttr inputAttr)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_create(adecHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_create failed");

    uapi_adec_attr adecAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_get_attr(*adecHandle, &adecAttr));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adec_get_attr failed");

    uapi_acodec_id adecCodecId;
    if (!AudioConvertAudioFormatToCodecId(inputAttr.audioFormat, &adecCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto CLOSE;
    }

    if (!IsSupportedCodecFormat(inputAttr)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupported codec format");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto CLOSE;
    }
    ret = AudioSDKAdecInitAttr(adecCodecId, inputAttr, &adecAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAdecInitAttr failed");
        goto CLOSE;
    }
#ifdef MELODY_AUDIO_SUPPORT
    AudioSdkSetAdecVolume((AudioHandle)(uintptr_t)*adecHandle, inputAttr.deviceId, inputAttr.type);
#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "adec_set_attr: codec_id: 0x%x, sample_rate: %d, channels: %d, bit_depth: %d, frameLength: %u",
        adecAttr.codec_id, adecAttr.param.sample_rate, adecAttr.param.channels, adecAttr.param.bit_depth, inputAttr.frameLength);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_set_attr(*adecHandle, &adecAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adec_set_attr failed: 0x%x, codec id: 0x%x", ret, adecCodecId);
        goto CLOSE;
    }
    (void)AudioSDKSetAOScene(*adecHandle, 0, &inputAttr, 0);
#if defined(CONFIG_AAC_MPEG4_SUPPORT) && (CONFIG_AAC_MPEG4_SUPPORT == 1)
    if (adecCodecId == UAPI_ACODEC_ID_AAC) {
        bool aacUnSupport = false;
        // set aac unsupport sign false to open the decoding of AAC MPEG4
        (void)uapi_adec_set_param((uintptr_t)*adecHandle, UAPI_ADEC_PARAM_PROFILE_CONSTRAINT_FLAG, &aacUnSupport, sizeof(bool));
    }
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

CLOSE:
    MEDIA_HAL_DOFUNC_TIME((void)uapi_adec_destroy(*adecHandle));
    return ret;
}

int32_t AudioSDKAdecClose(td_handle adecHandle)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_destroy(adecHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_destroy failed");

    MEDIA_HAL_LOGT(MODULE_NAME, "adec destroy success");
    return MEDIA_HAL_OK;
}

#ifndef MELODY_DEMO_BMIC_TX
#ifdef SUPPORT_SEA_AHE_PARAM
static int32_t SetSeaAheParam(td_handle seaHandle)
{
    td_u32 size = g_aheUserParam.size * (uint32_t)sizeof(g_aheUserParam.data[0]);
    const td_void *param = (const td_void*)g_aheUserParam.data;
    int32_t ret = uapi_sea_set_param(seaHandle, UAPI_SEA_AFE_SEE_RW_COMMON_PARAM_CMD, param, size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea param faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    size = g_aheNrParam.size * (uint32_t)sizeof(g_aheNrParam.data[0]);
    param = (const td_void*)g_aheNrParam.data;
    ret = uapi_sea_set_param(seaHandle, UAPI_SEA_AFE_SEE_RW_NR_PARAM_CMD, param, size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea param faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    size = g_aheAgcParam.size * (uint32_t)sizeof(g_aheAgcParam.data[0]);
    param = (const td_void*)g_aheAgcParam.data;
    ret = uapi_sea_set_param(seaHandle, UAPI_SEA_AFE_SEE_RW_AGC_PARAM_CMD, param, size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea param faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}
#endif

static int32_t SetSeaExtraParam(td_handle seaHandle)
{
#ifdef SUPPORT_NV
#ifdef SUPPORT_SEA_AHE_PARAM
    if (g_seaLibId == UAPI_SEA_LIB_AHE) {
        return SetSeaAheParam(seaHandle);
    }
#endif
    uint32_t size = g_vqeParam.size * (uint32_t)sizeof(g_vqeParam.data[0]);
    int32_t ret = uapi_sea_set_param(seaHandle, UAPI_SEA_AFE_SEE_RW_PARAM_CMD,
        (const td_void *)g_vqeParam.data, (td_u32)size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "uapi set sea param faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
#else
    MEDIA_HAL_UNUSED(seaHandle);
#endif
    return MEDIA_HAL_OK;
}
#endif

#ifdef MELODY_AUDIO_SUPPORT
static int32_t SetSeaAfeAttr(td_handle seaHandle)
{
#ifdef SUPPORT_NV
    if (g_disableSea == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "sea does not need to be disabled in normal mode.");
        return MEDIA_HAL_OK;
    }
#endif
#ifdef MELODY_DEMO_BMIC_TX
    uapi_sea_afe_attr afeAttr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_ANR | UAPI_SEA_AFE_OPT_AGC |
            UAPI_SEA_AFE_OPT_WNR | UAPI_SEA_AFE_OPT_PEQ,
    };
#else

    uapi_sea_afe_attr afeAttr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = 0,
    };
#endif
#ifdef MELODY_DEMO_HA
    afeAttr.u.see.options = UAPI_SEA_AFE_OPT_AGC | UAPI_SEA_AFE_OPT_ANR | UAPI_SEA_AFE_OPT_AEC |
        UAPI_SEA_AFE_OPT_WNR | UAPI_SEA_AFE_OPT_PEQ;
#endif
    int32_t ret = uapi_sea_set_afe_attr(seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}
#endif

static void InitSeaAttr(uapi_sea_attr *seaAttr, const struct PortPluginAttr *inputAttr)
{
#ifdef SUPPORT_FEATURE_CONFIG
    seaAttr->in_pcm.channels = CONFIG_AUDIO_AI_CHANNEL_CNT;
    if ((inputAttr->type & AUDIO_IN_BT_VOICE_CALL) == AUDIO_IN_BT_VOICE_CALL ||
        (inputAttr->type & AUDIO_IN_NEARLINK_VOICE_CALL) == AUDIO_IN_NEARLINK_VOICE_CALL) {
        seaAttr->ref_pcm.channels = CONFIG_AUDIO_SEA_REF_PCM_CHANNEL_CNT;
    } else {
        seaAttr->ref_pcm.channels = 0;
    }
#else
    if ((inputAttr->type & AUDIO_IN_COMMUNICATION) == AUDIO_IN_COMMUNICATION ||
        (inputAttr->type & AUDIO_IN_VOICE_RECOGNITION) == AUDIO_IN_VOICE_RECOGNITION ||
        (inputAttr->type & AUDIO_IN_VOICE_ASSISTANT) == AUDIO_IN_VOICE_ASSISTANT ||
        (inputAttr->type & AUDIO_IN_VOICE_RECORD) == AUDIO_IN_VOICE_RECORD ||
        (inputAttr->type & AUDIO_IN_PICKUP) == AUDIO_IN_PICKUP) {
        seaAttr->ref_pcm.channels = inputAttr->channelCount;
    } else {
        seaAttr->ref_pcm.channels = 0;
    }
#endif
}

LpwkPhsSets *GetLpwkPhsSets(void)
{
    return &g_lpwkPhsSets;
}

static int32_t AudioSDKSeaAttr(td_handle seaHandle, enum AudioCategory type)
{
#ifdef MELODY_AUDIO_SUPPORT
    if (SetSeaAfeAttr(seaHandle) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "set sea aef attr failed");
    }
#endif
#ifndef MELODY_DEMO_BMIC_TX
    if (SetSeaExtraParam(seaHandle) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "set sea param failed");
    }
#endif
    if (type == AUDIO_IN_VOICE_RECOGNITION) {
        uapi_sea_item_sets itemSets;
        itemSets.size = LPWK_PHRASE_COUNT;
        itemSets.array = g_lpwkPhsSets.phrase;
        int32_t ret = uapi_sea_get_phrase_sets(seaHandle, &itemSets);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_sea_get_phrase_sets failed[%x]", ret);
            return MEDIA_HAL_ERR;
        }
        g_lpwkPhsSets.phsCount = itemSets.count;
    }
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKSeaLoadEngine(enum AudioCategory type)
{
    int32_t ret = uapi_sea_load_engine(g_seaLibId, "imedia_2mic");
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_load_engine failed!");
    if (type == AUDIO_IN_VOICE_RECOGNITION) {
        ret = uapi_sea_load_engine(UAPI_SEA_LIB_KWS, "imedia_keyword");
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_load_engine failed!");
    }
    return ret;
}

static void AudioSDKSeaUnLoadEngine(enum AudioCategory type)
{
    (void)uapi_sea_unload_engine(g_seaLibId, "imedia_2mic");
    if (type == AUDIO_IN_VOICE_RECOGNITION) {
        (void)uapi_sea_unload_engine(UAPI_SEA_LIB_KWS, "imedia_keyword");
    }
}

static void AudioSDKInitSeaEng(uapi_sea_eng_sel *seaEng, enum AudioCategory type)
{
    if (seaEng == NULL) {
        return;
    }
    seaEng->afe.type = UAPI_SEA_AFE_SEE;
    seaEng->afe.lib_id = g_seaLibId;
    if (type == AUDIO_IN_VOICE_RECOGNITION) {
        seaEng->aai[0].type = UAPI_SEA_AAI_KWS;
        seaEng->aai[0].lib_id = UAPI_SEA_LIB_KWS;
    }
}

int32_t AudioSDKSeaOpen(td_handle *seaHandle, struct PortPluginAttr inputAttr)
{
    MediaHalMutexLock(g_audioSdkInitLock);
    if (g_subPortSeaOpenCount > 0) {
        *seaHandle = g_seaHandle;
        g_subPortSeaOpenCount++;
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }
    uapi_sea_attr seaAttr = {};
    uapi_sea_eng_sel seaEng = {};
    AudioSDKInitSeaEng(&seaEng, inputAttr.type);
    int32_t ret = AudioSDKSeaLoadEngine(inputAttr.type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "load sea engine failed");
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return ret;
    }
    MEDIA_HAL_DOFUNC_TIME(ret =  uapi_sea_get_default_attr(&seaEng, &seaAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea default attr failed");
        goto SEA_UNLOAD_ENGINE;
    }
    InitSeaAttr(&seaAttr, &inputAttr);
    MEDIA_HAL_DOFUNC_TIME(ret =  uapi_sea_create(seaHandle, &seaEng, &seaAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create sea failed");
        goto SEA_UNLOAD_ENGINE;
    }
    ret = AudioSDKSeaAttr(*seaHandle, inputAttr.type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKSeaAttr failed");
        goto SEA_DESTORY;
    }
    g_seaHandle = *seaHandle;
    g_subPortSeaOpenCount++;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGT(MODULE_NAME, "create sea success");
    return MEDIA_HAL_OK;
SEA_DESTORY:
    uapi_sea_destroy(*seaHandle);
SEA_UNLOAD_ENGINE:
    MediaHalMutexUnLock(g_audioSdkInitLock);
    AudioSDKSeaUnLoadEngine(inputAttr.type);
    return ret;
}

int32_t AudioSDKSeaClose(td_handle seaHandle, enum AudioCategory type)
{
    MediaHalMutexLock(g_audioSdkInitLock);
    int32_t ret;
    g_subPortSeaOpenCount--;
    if (g_subPortSeaOpenCount > 0) {
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_destroy(seaHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy sea failed");
    AudioSDKSeaUnLoadEngine(type);
    g_seaHandle = 0;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGT(MODULE_NAME, "destroy sea success");
    return MEDIA_HAL_OK;
}

#ifdef NOT_SUPPORT_MULTI_AENC
static int32_t WaitCreateAnec(void)
{
    int32_t cnt = 0;
    while (cnt < WAIT_CREATE_AENC_LOOP_CNT) {
        MediaHalMutexLock(g_audioSdkInitLock);
        if (g_isCanCreateAenc) {
            g_isCanCreateAenc = false;
            MediaHalMutexUnLock(g_audioSdkInitLock);
            return MEDIA_HAL_OK;
        }
        MediaHalMutexUnLock(g_audioSdkInitLock);
        usleep(WAIT_CREATE_AENC_SUCCESS_US);
        cnt++;
    }
    return MEDIA_HAL_ERR;
}
#endif

#ifdef MELODY_AUDIO_SUPPORT
static void AencAttrLiveMicPrivateData(uapi_aenc_attr *aencAttr, uapi_audio_pcm_format *aencPrivate,
    AencType aencType, struct PortPluginAttr *inputAttr)
{
#ifdef MELODY_DEMO_BMIC_TX
    if (aencAttr->codec_id == UAPI_ACODEC_ID_PCM) {
        aencPrivate->channels = aencAttr->param.channels;
        aencPrivate->bit_depth = aencAttr->param.bit_depth;
        aencPrivate->sample_rate = aencAttr->param.sample_rate;
        aencPrivate->sample_per_frame = ((uint32_t)aencAttr->param.sample_rate) * (inputAttr->frameLength) /
            (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
        aencAttr->param.private_data = aencPrivate;
        aencAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
        aencAttr->param.channels = AUDIO_CHANNEL_COUNT_1;
        aencAttr->param.bit_depth = AUDIO_SAMPLE_BITDEPTH_32;
        aencAttr->param.sample_rate = AUDIO_SAMPLE_RATE_48K;
        aencAttr->param.samples_per_frame = AUDIO_SAMPLE_RATE_48K * (inputAttr->frameLength) /
            (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
    }
    return;
#endif
#ifdef MELODY_DEMO_BMIC_RX
    if (aencAttr->codec_id == UAPI_ACODEC_ID_PCM) {
        aencPrivate->channels = aencAttr->param.channels;
        aencPrivate->bit_depth = aencAttr->param.bit_depth;
        aencPrivate->sample_rate = aencAttr->param.sample_rate;
        aencPrivate->sample_per_frame = ((uint32_t)aencAttr->param.sample_rate) * (inputAttr->frameLength) /
            (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
        aencAttr->param.private_data = aencPrivate;
        aencAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
        aencAttr->param.channels = aencAttr->param.channels;
        aencAttr->param.bit_depth = AUDIO_SAMPLE_BITDEPTH_32;
        aencAttr->param.sample_rate = AUDIO_SAMPLE_RATE_48K;
        aencAttr->param.samples_per_frame = AUDIO_SAMPLE_RATE_48K * (inputAttr->frameLength) /
            (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
    }
    return;
#endif
    MEDIA_HAL_UNUSED(aencAttr);
    MEDIA_HAL_UNUSED(aencPrivate);
    MEDIA_HAL_UNUSED(aencType);
    MEDIA_HAL_UNUSED(inputAttr);
}

static void AencAttrPrivateData(uapi_aenc_attr *aencAttr, uapi_audio_pcm_format *aencPrivate,
    AencType aencType, struct PortPluginAttr *inputAttr)
{
    AencAttrLiveMicPrivateData(aencAttr, aencPrivate, aencType, inputAttr);
#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUNDBAR))
    if (aencAttr->codec_id == UAPI_ACODEC_ID_PCM && inputAttr->type == AUDIO_IN_USB_VOICE_CALL) {
        aencPrivate->channels = AUDIO_CHANNEL_COUNT_2;
        aencPrivate->bit_depth = AUDIO_SAMPLE_BITDEPTH_24;
        aencPrivate->sample_rate = aencAttr->param.sample_rate;
        aencPrivate->sample_per_frame = ((uint32_t)aencAttr->param.sample_rate) / CONFIG_AUDIO_AI_FRAME_PER_SEC;
        aencAttr->param.private_data = aencPrivate;
        aencAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
        aencAttr->param.channels = AUDIO_CHANNEL_COUNT_2;
        aencAttr->param.bit_depth = AUDIO_SAMPLE_BITDEPTH_32;
        aencAttr->param.sample_rate = AUDIO_SAMPLE_RATE_48K;
        aencAttr->param.samples_per_frame = AUDIO_SAMPLE_RATE_48K / CONFIG_AUDIO_AI_FRAME_PER_SEC;
    } else if (aencType == AENC_TYPE_DONGLE_UPLINK) {
        aencAttr->codec_id = UAPI_ACODEC_ID_PCM;
        aencPrivate->channels = aencAttr->param.channels;
        aencPrivate->bit_depth = AUDIO_SAMPLE_BITDEPTH_24;
        aencPrivate->sample_rate = aencAttr->param.sample_rate;
        aencPrivate->sample_per_frame = ((uint32_t)aencAttr->param.sample_rate) / CONFIG_AUDIO_AI_FRAME_PER_SEC;
        aencAttr->param.private_data = aencPrivate;
        aencAttr->param.private_data_size = sizeof(uapi_audio_pcm_format);
        aencAttr->param.channels = AUDIO_CHANNEL_COUNT_1;
        aencAttr->param.bit_depth = AUDIO_SAMPLE_BITDEPTH_16;
        aencAttr->param.sample_rate = AUDIO_SAMPLE_RATE_48K;
        aencAttr->param.samples_per_frame = AUDIO_SAMPLE_RATE_48K / CONFIG_AUDIO_AI_FRAME_PER_SEC;
    } else if ((aencAttr->codec_id == UAPI_ACODEC_ID_L2HC || aencAttr->codec_id == UAPI_ACODEC_ID_L2HC_VOICE) &&
        g_l2hcAencIsSetted) {
        aencAttr->param.private_data = &g_l2hcAencConfig;
        aencAttr->param.private_data_size = sizeof(uapi_acodec_l2hc_priv_cfg);
    }
    return;
#endif
}
#endif

static int32_t AudioSDKAencSetBitRate(td_handle *aencHandle, uapi_aenc_attr aencAttr)
{
    int32_t ret;
    if (aencAttr.codec_id == UAPI_ACODEC_ID_L2HC) {
        static uint32_t bitRate = AUDIO_DONGLE_SCENE_BITRATE_64;
        ret = uapi_aenc_set_param(*aencHandle, UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild, ret: %d", ret);
            return MEDIA_HAL_ERR;
        }
    }
#ifndef MELODY_AUDIO_SUPPORT
    if (aencAttr.codec_id == UAPI_ACODEC_ID_OPUS) {
        static uint32_t bitRate = OPUS_ENC_BIT_RATE;
        ret = uapi_aenc_set_param(*aencHandle, UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild, ret: 0x%x", ret);
            return MEDIA_HAL_ERR;
        }
    }
#endif
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKAencSetParam(enum AudioCategory type, td_handle *aencHandle)
{
#ifdef MELODY_DEMO_BMIC_TX
    if (type != AUDIO_IN_LIVE_MIC && type != AUDIO_IN_VOICE_RECORD) {
        return MEDIA_HAL_OK;
    }
    // 直播麦dataSource固定为0，录音的aenc 可以取0,1,2值
    int32_t dataSource = 0;
    if (type == AUDIO_IN_VOICE_RECORD) {
        dataSource = g_dataSource;
    }
    uapi_audio_scene_config asc = {};
    asc.scene_id = (type == AUDIO_IN_LIVE_MIC) ? 0 : 1;
    asc.scene_type = (type == AUDIO_IN_LIVE_MIC) ? UAPI_AUDIO_SCENE_LIVE_MIC : UAPI_AUDIO_SCENE_LIVE_MIC_REC;
    (void)uapi_audio_set_audio_scene(*aencHandle, &asc);
    int32_t ret = uapi_aenc_set_param(*aencHandle, UAPI_AENC_PARAM_DATA_SOURCE_CFG, &dataSource, sizeof(int32_t));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc data source faild, ret: %d", ret);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi set aenc data source success, type: 0x%x, dataSource: %d", type, dataSource);
#endif
    MEDIA_HAL_UNUSED(type);
    MEDIA_HAL_UNUSED(aencHandle);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAencOpen(td_handle *aencHandle, uapi_aenc_attr aencAttr, AencType aencType,
    struct PortPluginAttr *inputAttr)
{
    int32_t ret;
#ifdef NOT_SUPPORT_MULTI_AENC
    ret = WaitCreateAnec();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wait create anec time out!");
        return ret;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_create(aencHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_create failed, ret = 0x%x", ret);
#ifdef NOT_SUPPORT_MULTI_AENC
        g_isCanCreateAenc = true;
#endif
        return ret;
    }
#ifdef MELODY_AUDIO_SUPPORT
    AudioSdkSetAencVolume((AudioHandle)(uintptr_t)*aencHandle, aencType, inputAttr->type);
    uapi_audio_pcm_format aencPrivate;
    AencAttrPrivateData(&aencAttr, &aencPrivate, aencType, inputAttr);
#else
    MEDIA_HAL_UNUSED(aencType);
    MEDIA_HAL_UNUSED(inputAttr);
#endif
    MEDIA_HAL_LOGT(MODULE_NAME, "codec_id: 0x%x", aencAttr.codec_id);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_set_attr(*aencHandle, &aencAttr));
    MEDIA_HAL_LOGT(MODULE_NAME, "aenc_set_attr: codec_id: 0x%x, sample_rate: %d, channels: %d, bit_depth: %d, frameLength: %u",
        aencAttr.codec_id, aencAttr.param.sample_rate, aencAttr.param.channels, aencAttr.param.bit_depth, inputAttr->frameLength);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_set_attr failed: 0x%x", ret);
        goto CLOSE;
    }
    ret = AudioSDKAencSetBitRate(aencHandle, aencAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAencSetBitRate failed: %d", ret);
        goto CLOSE;
    }
    ret = AudioSDKAencSetParam(inputAttr->type, aencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAencSetParam failed: %d", ret);
        goto CLOSE;
    }
    MEDIA_HAL_LOGT(MODULE_NAME, "create aenc success");
    return MEDIA_HAL_OK;

CLOSE:
    MEDIA_HAL_DOFUNC_TIME((void)uapi_aenc_destroy(*aencHandle));
#ifdef NOT_SUPPORT_MULTI_AENC
    g_isCanCreateAenc = true;
#endif
    return ret;
}

uapi_aenc_attr GetAencAttrInfo(uapi_acodec_id aencCodecId, struct PortPluginAttr inputAttr)
{
    uapi_aenc_attr aencAttr = {};
    if (inputAttr.type == AUDIO_IN_VOICE_RECORD ||
        inputAttr.type == AUDIO_IN_VOICE_ASSISTANT ||
        inputAttr.type == AUDIO_IN_VOICE_CALL_VOLTE_SPI) {
#ifdef MELODY_DEMO_BMIC_TX
        aencAttr.frame_mode_output = 0;
#else
        aencAttr.frame_mode_output = 1;
#endif
    }
#ifdef MELODY_DEMO_EARPHONE
    if (inputAttr.type == AUDIO_IN_NEARLINK_VOICE_CALL && inputAttr.audioFormat == AUDIO_FORMAT_TYPE_L2HC_VOICE) {
        aencAttr.frame_mode_output = 1;
    }
#endif
#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR))
    aencAttr.frame_mode_output = 1;
#endif
    aencAttr.codec_id = aencCodecId;
    aencAttr.param.interleaved = true;
    aencAttr.param.channels = inputAttr.channelCount;
    aencAttr.param.bit_depth = inputAttr.bitWidth;
    aencAttr.param.sample_rate = inputAttr.sampleRate;
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    // 直播麦为了满足2发1收动态切换8发1收，需要将帧长动态可配
    aencAttr.param.samples_per_frame = ((uint32_t)inputAttr.sampleRate) * (inputAttr.frameLength) /
        (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
#else
    aencAttr.param.samples_per_frame = ((uint32_t)inputAttr.sampleRate) / CONFIG_AUDIO_AI_FRAME_PER_SEC;
#endif
    aencAttr.param.private_data = NULL;
    aencAttr.param.private_data_size = 0;
#ifdef HI3322_CHIP_V100
    static uint32_t bitRate = 0;
    if (aencCodecId == UAPI_ACODEC_ID_MP3) {
        bitRate = MP3_ENC_BIT_RATE;
        aencAttr.param.private_data = &bitRate;
        aencAttr.param.private_data_size = sizeof(bitRate);
    }
#endif
    return aencAttr;
}

int32_t AudioSDKAencClose(td_handle aencHandle)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_destroy(aencHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_destroy failed");
#ifdef NOT_SUPPORT_MULTI_AENC
    MediaHalMutexLock(g_audioSdkInitLock);
    g_isCanCreateAenc = true;
    MediaHalMutexUnLock(g_audioSdkInitLock);
#endif
    MEDIA_HAL_LOGT(MODULE_NAME, "destroy aenc success");
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAttach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle)
{
    int32_t ret;
    switch (modType) {
        case AUDIO_MOD_AI:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_attach_output");
            break;
        case AUDIO_MOD_AI_SEA:
            if (g_subPortSeaOpenCount <= 1 && g_seaHandle == dstHandle) {
                MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_attach_output(srcHandle, dstHandle));
                CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_attach_output");
            }
            break;
        case AUDIO_MOD_ADP:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_attach_output failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_attach_output failed");
            break;
        case AUDIO_MOD_AENC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_attach_output failed");
            break;
        case AUDIO_MOD_SEA:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_attach_output(srcHandle, UAPI_SEA_OUTPUT_ASR_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_attach_output failed");
            break;
        case AUDIO_MOD_SEA_REC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_attach_output(srcHandle, UAPI_SEA_OUTPUT_REC_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_attach_output failed");
            break;
        case AUDIO_MOD_SOUND:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_attach_output(srcHandle, UAPI_SND_OUT_PORT_CAST0, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_attach_output failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid module type %d", modType);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGT(MODULE_NAME, "attach type:%d success", modType);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKDetach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle)
{
    int32_t ret;
    switch (modType) {
        case AUDIO_MOD_AI:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_detach_output failed");
            break;
        case AUDIO_MOD_AI_SEA:
            if (g_subPortSeaOpenCount <= 1 && g_seaHandle == dstHandle) {
                MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_detach_output(srcHandle, dstHandle));
                CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_detach_output failed");
            }
            break;
        case AUDIO_MOD_ADP:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_detach_output failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_detach_output failed");
            break;
        case AUDIO_MOD_AENC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_detach_output failed");
            break;
        case AUDIO_MOD_SEA:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_detach_output(srcHandle, UAPI_SEA_OUTPUT_ASR_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_detach_output failed");
            break;
        case AUDIO_MOD_SEA_REC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_detach_output(srcHandle, UAPI_SEA_OUTPUT_REC_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_attach_output failed");
            break;
        case AUDIO_MOD_SOUND:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_detach_output(srcHandle, UAPI_SND_OUT_PORT_CAST0, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_detach_output failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid module type %d", modType);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGT(MODULE_NAME, "detach type:%d success", modType);
    return MEDIA_HAL_OK;
}

static bool AudioPluginIsValidBlueToothPin(enum AudioPortPin pin, const char *pluginName)
{
    if (strcmp("bluetooth", pluginName) != 0) {
        return false;
    }
    if (pin == PIN_IN_BLUETOOTH_SCO || pin == PIN_OUT_BLUETOOTH_SCO ||
        pin == PIN_OUT_BLUETOOTH_A2DP || pin == PIN_IN_BLUETOOTH_A2DP ||
        pin == PIN_IN_NEARLINK_DONGLE || pin == PIN_OUT_NEARLINK_DONGLE) {
        return true;
    }
    return false;
}

bool AudioPluginIsValidPin(enum AudioPortPin pin, const char *pluginName)
{
    if (strcmp("primary", pluginName) == 0) {
        if (pin == PIN_IN_MIC || pin == PIN_OUT_SPEAKER) {
            return true;
        }
    } else if (strcmp("modem", pluginName) == 0) {
        if (pin == PIN_IN_MODEM || pin == PIN_OUT_MODEM ||
            pin == PIN_IN_MODEM_HEADSET || pin == PIN_OUT_MODEM_HEADSET) {
            return true;
        }
    } else if (strcmp("usb", pluginName) == 0) {
        if (pin == PIN_IN_USB || pin == PIN_OUT_USB ||
            pin == PIN_IN_USB_DONGLE || pin == PIN_OUT_USB_DONGLE) {
            return true;
        }
    }

    return AudioPluginIsValidBlueToothPin(pin, pluginName);
}

int32_t ParseKeyValueForExtra(const struct InvokeAttr *invokeAttr,
    char *key, int32_t keyLen, char *value, int32_t valueLen)
{
    MEDIA_HAL_UNUSED(keyLen);
    MEDIA_HAL_UNUSED(valueLen);
    int32_t ret = sscanf_s((const char *)invokeAttr->request, "%s = %s", key, MAX_KEY_STR_SIZE,
        value, MAX_KEY_VALUE_SIZE);
    if (ret == MEDIA_HAL_ERR) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sscanf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t IsNeedSetExtraFlag(enum AudioFormat audioFormat, bool *IsSetExtraParam)
{
#ifdef AUDIO_SDK_SUPPORT
    uapi_acodec_id adecCodecId;
    if (!AudioConvertAudioFormatToCodecId(audioFormat, &adecCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        return MEDIA_HAL_INVALID_PARAM;
    }
    *IsSetExtraParam = (adecCodecId == UAPI_ACODEC_ID_FLAC) ? true : false;
#endif
    return MEDIA_HAL_OK;
}

int32_t SetExtraData(struct PortPluginAttr *inputAttr, const char *value, uint32_t valueLen)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    if (valueLen <= 1) {
        MEDIA_HAL_LOGI(MODULE_NAME, "valueLen invalid ExtraData not need set");
        return MEDIA_HAL_OK;
    }
    if (inputAttr->extraData != NULL) {
        MediaHalFree(inputAttr->extraData);
        inputAttr->extraData = NULL;
        inputAttr->extraDataSize = 0;
    }
    inputAttr->extraData = MediaHalMalloc(valueLen);
    if (inputAttr->extraData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, " malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    (void)memset_s(inputAttr->extraData, valueLen, 0, valueLen);
    int32_t tempPoint = 0;
    char temp[3] = {0}; /* 3 byte */
    char *endPtr = NULL;
    unsigned long strNum;
    uint32_t outdataNum = 0;
    for (uint32_t i = 0; i < strlen(value) / 2; i++) { /* 2 char --> 1 byte(hexadecimal) */
        temp[0] = *(value + tempPoint);
        temp[1] = *(value + tempPoint + 1);
        strNum = strtoul(temp, &endPtr, 16); /* 16: hexadecimal */
        ret = memcpy_s(inputAttr->extraData + outdataNum,
            valueLen, &strNum, sizeof(unsigned char));
        if (ret != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
            return MEDIA_HAL_ERR;
        }
        outdataNum += (uint32_t)sizeof(unsigned char);
        tempPoint += 2; /* 2 char --> 1 byte(hexadecimal) */
    }
    inputAttr->extraDataSize = outdataNum;
#endif
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
int32_t SaveToggleConfig(uapi_audio_toggle_config *toggleConfig, const char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    CHK_NULL_RETURN(toggleConfig, MEDIA_HAL_INVALID_PARAM, "input param toggleConfig is null");
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t toggleCount = 0;
    int32_t toggleValid = 0;
    int32_t ret = sscanf_s(value, "%d:%u", &toggleValid, &toggleCount);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "save toggle config sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    toggleConfig->toggle_valid = false;
    if (toggleValid != 0) {
        toggleConfig->toggle_valid = true;
    }
    toggleConfig->toggle_count = toggleCount;
    return MEDIA_HAL_OK;
}

int32_t UpdateCurrentPosition(td_handle trackHandle, struct AudioTimeStamp *time)
{
    int32_t ret;
    int64_t pts = 0;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_play_pts(trackHandle, &pts));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get sound track pts failed");
    int64_t sec = 0;
    int64_t nsec = 0;
    ret = MediaHalConvertUSecToSecAndNSec(pts, &sec, &nsec);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "convert time from us to sec and nsec failed");
        return MEDIA_HAL_ERR;
    }

    time->tvSec = sec;
    time->tvNSec = nsec;

    return MEDIA_HAL_OK;
}

bool isMultipleSeaOpen(void)
{
    return g_subPortSeaOpenCount > 1;
}

bool isMultipleAIOpen(void)
{
    return g_subPortTrackInOpened > 1;
}

int32_t AudioSDKSetAIMute(td_handle aiHandle, bool mute)
{
    td_bool isMute = (mute == true) ? TD_TRUE : TD_FALSE;
    int32_t ret = uapi_ai_set_mute(aiHandle, isMute);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_set_mute failed");
    MEDIA_HAL_LOGI(MODULE_NAME, "set mute %d success", mute);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER,
        MEDIA_CHR_AM_SET_MIC_MUTE, MEDIA_CHR_SDK_SET_MIC_MUTE);
    return MEDIA_HAL_OK;
}

int32_t AudioSDKGetAIMute(td_handle aiHandle, bool *mute)
{
    td_bool isMute = TD_FALSE;
    int32_t ret = uapi_ai_get_mute(aiHandle, &isMute);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_get_mute failed");
    *mute = (isMute == TD_TRUE) ? true : false;
    MEDIA_HAL_LOGI(MODULE_NAME, "Get mute %d success", mute);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER,
        MEDIA_CHR_AM_GET_MIC_MUTE, MEDIA_CHR_SDK_GET_MIC_MUTE);
    return MEDIA_HAL_OK;
}

#ifdef SUPPORT_VOLUME_CALIBRATION
int32_t GetRealDbVolume(uapi_snd_gain *sndGain)
{
    CHK_NULL_RETURN(sndGain, MEDIA_HAL_INVALID_PARAM, "sndGain is null");
    uapi_snd_gain sndMaxGain = { 0 };
    GetVolumeGain(GetMaxVolumeDb(), &sndMaxGain.integer, &sndMaxGain.decimal);
    sndGain->integer += sndMaxGain.integer;
    sndGain->decimal += sndMaxGain.decimal;
    if (abs(sndGain->decimal) > VOLUME_ACCURACY) {
        sndGain->integer += sndGain->decimal / VOLUME_ACCURACY;
        sndGain->decimal = sndGain->decimal % VOLUME_ACCURACY;
    }
    return MEDIA_HAL_OK;
}
#endif

int32_t AudioSDKSetTrackVolume(td_handle trackHandle, float volume, enum AudioCategory type)
{
    uapi_snd_gain sndGain = {};
    VolumePercentToDb(volume, &sndGain);
#ifdef SUPPORT_VOLUME_CALIBRATION
    int32_t res = GetRealDbVolume(&sndGain);
    CHK_FAILED_RETURN(res, MEDIA_HAL_OK, res, "get real db volume failed");
#endif
#ifdef MELODY_AUDIO_SUPPORT
    AudioSdkSetTrackVolumeInner((AudioHandle)(uintptr_t)trackHandle, type, sndGain);
#else
    MEDIA_HAL_UNUSED(type);
    int32_t ret = uapi_snd_set_track_volume(trackHandle, &sndGain, NULL);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_track_volume failed!");
        return ret;
    }
#endif
    return MEDIA_HAL_OK;
}

int32_t GetVolumeGain(int32_t volume, td_s32 *integer, td_s32 *decimal)
{
    if (integer == NULL || decimal == NULL) {
        return MEDIA_HAL_ERR;
    }
    *integer = volume / VOLUME_ACCURACY;
    *decimal = volume % VOLUME_ACCURACY;
    *decimal -= *decimal % VOLUME_MULTIPLIER;
    return MEDIA_HAL_OK;
}

int32_t GetParamByValue(uint8_t *param, uint32_t paramLen, const char *value,
    uint32_t valueLen, uint32_t configLen)
{
    uint32_t prefixlen = 0;
    for (uint32_t i = 0; i < valueLen; i++) {
        if (value[i] == ':') {
            prefixlen = i + 1;
            break;
        }
    }
    if (prefixlen == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "prefix is not found.");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(param, paramLen, value + prefixlen, configLen) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set spk cali coef memcpy_s failed!");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t SetSeaEnable(td_handle seaHandle, const char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    int32_t seaStatus = 0;
    int32_t ret;
#ifdef MELODY_AUDIO_SUPPORT
    uapi_sea_afe_type seaType;
    ret = sscanf_s(value, "%d#%d", &seaType, &seaStatus);
#else
    ret = sscanf_s(value, "%d", &seaStatus);
#endif
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set sea enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC | UAPI_SEA_AFE_OPT_AEC | UAPI_SEA_AFE_OPT_PEQ |
            UAPI_SEA_AFE_OPT_WNR | UAPI_SEA_AFE_OPT_ANR | UAPI_SEA_AFE_OPT_BFM,
    };
    if (seaStatus == 0) {
        afeAttr.u.see.options = 0;
    }
#ifdef MELODY_AUDIO_SUPPORT
    ret = uapi_sea_set_afe_attr(seaHandle, seaType, &afeAttr);
#else
    ret = uapi_sea_set_afe_attr(seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
#endif
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_SET_PARAM_STREAM, MEDIA_CHR_SET_SEA_ENABLE);
    return MEDIA_HAL_OK;
}

#ifdef SUPPORT_VOLUME_CALIBRATION
int32_t GetMaxVolumeDb(void)
{
#ifdef SUPPORT_NV
    return g_maxVolumeDb;
#else
    return 0;
#endif
}
#endif

int32_t ExtraParamsInvoke(AudioHandle portHandle, struct InvokeAttr *invokeAttr,
    struct AudioPortInvoke invokeList[], uint32_t listSize)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param handle is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    char keyFlag[MAX_KEY_STR_SIZE] = { 0 };
    char *value = (char *)MediaHalCalloc(MAX_KEY_VALUE_SIZE);
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "value malloc failed!");
    int32_t ret = ParseKeyValueForExtra(invokeAttr, keyFlag, MAX_KEY_STR_SIZE, value, MAX_KEY_VALUE_SIZE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ParseKeyValueForExtra failed!");
        MediaHalFree(value);
        return MEDIA_HAL_ERR;
    }
    AudioPortInvokeFunc invokeFunc = NULL;
    for (size_t i = 0; i < listSize; i++) {
        if (strcmp(invokeList[i].invokeName, keyFlag) == 0) {
            invokeFunc = invokeList[i].invokefunc;
            if (invokeList[i].isNeedParseKeyValue) {
                ret = invokeFunc(portHandle, value, (uint32_t)(strlen(value) + 1));
                MediaHalFree(value);
                return ret;
            }
            ret =  invokeFunc(portHandle, invokeAttr->request, (uint32_t)(strlen(invokeAttr->request) + 1));
            MediaHalFree(value);
            return ret;
        }
    }
    MediaHalFree(value);
    if (invokeFunc == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found invoke function!");
        return MEDIA_HAL_INVALID_PARAM;
    }
    return MEDIA_HAL_OK;
}
#endif

#if (defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)) || \
    defined(MELODY_DEMO_HA) && defined(SUPPORT_NV)
void SetMicArrangementOrder(uapi_ai_attr *aiAttr)
{
    audio_mic_t micCfg = { UAPI_AI_AMIC0, UAPI_AI_AMIC1 };
    uint16_t len = 0;
    errcode_t nvRet = uapi_nv_read(NV_ID_MEDIA_MIC_CFG, (uint16_t)sizeof(audio_mic_t), &len, (uint8_t *)&micCfg);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "nv read mic sel param faild!");
    }
    aiAttr->port_attr.adc.mic_sel[0] = micCfg.main_mic;
    aiAttr->port_attr.adc.mic_sel[1] = micCfg.second_mic;
}
#endif

int32_t GetBuiltinAIAttr(uapi_ai_attr *aiAttr, const AIOpenAttr *aiOpenAttr)
{
#ifndef MELODY_AUDIO_SUPPORT
    MEDIA_HAL_UNUSED(aiOpenAttr);
    for (uint32_t i = 0; i < ARRAY_SIZE(g_aiAttr); i++) {
        if (g_seaLibId == g_aiAttr[i].seaLibId) {
            aiAttr->port_attr.adc.rx_type = g_aiAttr[i].rxType;
            aiAttr->pcm_attr.channels = g_aiAttr[i].channels;
            aiAttr->pcm_attr.bit_depth = g_aiAttr[i].bitDepth;
            aiAttr->pcm_attr.sample_rate = g_aiAttr[i].sampleRate;
            aiAttr->pcm_attr.sample_per_frame = g_aiAttr[i].samplePerFrame;
            return MEDIA_HAL_OK;
        }
    }
    return MEDIA_HAL_ERR;
#else
#ifdef SUPPORT_FEATURE_CONFIG
    MEDIA_HAL_UNUSED(aiOpenAttr);
    aiAttr->pcm_attr.bit_depth = CONFIG_AUDIO_AI_BIT_WIDTH;
    aiAttr->pcm_attr.channels = CONFIG_AUDIO_AI_CHANNEL_CNT;
    aiAttr->pcm_attr.sample_rate = CONFIG_AUDIO_AI_SAMPLE_RATE;
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)
    if (aiOpenAttr->inputAttr.type == AUDIO_IN_NEARLINK_VOICE_CALL ||
        aiOpenAttr->inputAttr.type == AUDIO_IN_NEARLINK_MUSIC) {
        aiAttr->pcm_attr.sample_rate = aiOpenAttr->inputAttr.sampleRate;
    }
    // 为了产测验证，需要将sea关闭，对应的声道数量需要调整
    if (aiOpenAttr->inputAttr.type == AUDIO_IN_BT_VOICE_CALL && !aiOpenAttr->isNeedOpenSea) {
        aiAttr->pcm_attr.channels = AUDIO_CHANNEL_COUNT_1;
    }
#endif
#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_SOUNDBAR))
    if (aiOpenAttr->inputAttr.type == AUDIO_IN_USB_VOICE_CALL) {
        aiAttr->pcm_attr.bit_depth = AUDIO_SAMPLE_BITDEPTH_32;
        aiAttr->pcm_attr.channels = AUDIO_CHANNEL_COUNT_2;
        aiAttr->port_attr.adc.mic_sel[0] = UAPI_AI_AMIC0;
        aiAttr->port_attr.adc.mic_sel[1] = UAPI_AI_AMIC1;
    }
#endif
#ifdef MELODY_DEMO_EARPHONE
    // mic_ldo参数使用config宏进行静态使用，默认config宏为yes,对应mic_ldo打开，为false表示对应mic_ldo关闭
    aiAttr->port_attr.adc.mic_ldo[0] = UAPI_AI_MIC_LDO0 ;
    aiAttr->port_attr.adc.mic_ldo[INDEX_1] = UAPI_AI_MIC_LDO1;
    aiAttr->port_attr.adc.mic_ldo[INDEX_2] = UAPI_AI_MIC_LDO2;
#ifndef CONFIG_ENABLE_AUDIO_AI_MIC_LDO_0
    aiAttr->port_attr.adc.mic_ldo[0] = UAPI_AI_MIC_LDO_MAX;
#endif
#ifndef CONFIG_ENABLE_AUDIO_AI_MIC_LDO_1
    aiAttr->port_attr.adc.mic_ldo[INDEX_1] = UAPI_AI_MIC_LDO_MAX;
#endif
#ifndef CONFIG_ENABLE_AUDIO_AI_MIC_LDO_2
    aiAttr->port_attr.adc.mic_ldo[INDEX_2] = UAPI_AI_MIC_LDO_MAX;
#endif
#endif
#else
    aiAttr->vad_attr.enable = TD_FALSE;
    if (aiOpenAttr->inputAttr.type == AUDIO_IN_VOICE_RECOGNITION &&
        !aiOpenAttr->inputAttr.isAlwaysWake) {
        aiAttr->vad_attr.enable = TD_TRUE;
        aiAttr->port_attr.adc.rx_type = UAPI_AI_RX_MAD;
    }
    aiAttr->pcm_attr.bit_depth = aiOpenAttr->inputAttr.bitWidth;
    aiAttr->pcm_attr.channels = aiOpenAttr->inputAttr.channelCount;
    aiAttr->pcm_attr.sample_rate = aiOpenAttr->inputAttr.sampleRate;
#endif
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    aiAttr->pcm_attr.sample_per_frame = aiAttr->pcm_attr.sample_rate * (aiOpenAttr->inputAttr.frameLength) /
        (MS_PER_SECOND * AUDIO_FRAME_LENGTH_MAG_COEFF);
#else
    aiAttr->pcm_attr.sample_per_frame = aiAttr->pcm_attr.sample_rate / CONFIG_AUDIO_AI_FRAME_PER_SEC;
#endif
#endif
    return MEDIA_HAL_OK;
}

#ifndef MELODY_AUDIO_SUPPORT
static int32_t GetBuiltinSndOutAttr(uapi_snd_attr *sndAttr)
{
    sndAttr->port_num = BUILTIN_SND_OUT_PORT_NUM;
    sndAttr->port_attr[0].out_port = BUILTIN_SND_OUT_PORT;
    sndAttr->channels = BUILTIN_SND_OUT_AUDIO_CH;
    sndAttr->bit_depth = BUILTIN_SND_OUT_BIT_DEPTH;
    sndAttr->sample_rate = BUILTIN_SND_OUT_SAMPLE_RATE;
    sndAttr->port_attr[0].u.i2s_attr.attr.bclk = BUILTIN_SND_OUT_I2S_ATTR_I2S_BCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.bit_depth = BUILTIN_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->port_attr[0].u.i2s_attr.attr.channels = BUILTIN_SND_OUT_I2S_ATTR_AUDIO_CH;
    sndAttr->port_attr[0].u.i2s_attr.attr.i2s_mode = BUILTIN_SND_OUT_I2S_ATTR_I2S_STD_MODE;
    sndAttr->port_attr[0].u.i2s_attr.attr.master = BUILTIN_SND_OUT_I2S_ATTR_MASTER;
    sndAttr->port_attr[0].u.i2s_attr.attr.mclk = BUILTIN_SND_OUT_I2S_ATTR_I2S_MCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_delay_cycle = BUILTIN_SND_OUT_I2S_ATTR_I2S_PCM_DELAY;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_sample_rise_edge = BUILTIN_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE;
    return MEDIA_HAL_OK;
}

static int32_t GetExternalAOPickUpTypeAttr(uapi_snd_attr *sndAttr)
{
#ifdef SUPPORT_SEA_AHE_PARAM
    sndAttr->port_num = EXTERNAL_SND_OUT_PORT_NUM;
    sndAttr->port_attr[0].out_port = EXTERNAL_SND_OUT_PORT;
    sndAttr->channels = EXTERNAL_SND_OUT_I2S_ATTR_CHANNEL;
    sndAttr->bit_depth = EXTERNAL_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->sample_rate = EXTERNAL_SND_OUT_AUDIO_SAMPLE_RATE;
    sndAttr->port_attr[0].u.i2s_attr.attr.bclk = EXTERNAL_SND_OUT_I2S_ATTR_BCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.bit_depth = EXTERNAL_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->port_attr[0].u.i2s_attr.attr.channels = EXTERNAL_SND_OUT_I2S_ATTR_CHANNEL;
    sndAttr->port_attr[0].u.i2s_attr.attr.i2s_mode = EXTERNAL_SND_OUT_I2S_ATTR_I2S_MODE;
    sndAttr->port_attr[0].u.i2s_attr.attr.master = EXTERNAL_SND_OUT_I2S_ATTR_MASTER;
    sndAttr->port_attr[0].u.i2s_attr.attr.mclk = EXTERNAL_SND_OUT_I2S_ATTR_MCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_delay_cycle = EXTERNAL_SND_OUT_I2S_ATTR_I2S_PCM_DELAY;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_sample_rise_edge = EXTERNAL_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE;
    return MEDIA_HAL_OK;
#else
    return GetBuiltinSndOutAttr(sndAttr);
#endif
}
#endif

int32_t GetBuiltinAOAttr(uapi_snd_attr *sndAttr, const struct PortPluginAttr *inputAttr)
{
#ifndef MELODY_AUDIO_SUPPORT
    if (inputAttr->type == AUDIO_IN_PICKUP) {
        return GetExternalAOPickUpTypeAttr(sndAttr);
    }
    return GetBuiltinSndOutAttr(sndAttr);
#else
#ifdef SUPPORT_FEATURE_CONFIG
    sndAttr->channels = CONFIG_AUDIO_AO_CHANNEL_CNT;
    sndAttr->bit_depth = CONFIG_AUDIO_AO_BIT_WIDTH;
    sndAttr->sample_rate = CONFIG_AUDIO_AO_SAMPLE_RATE;
    sndAttr->port_num = CONFIG_AUDIO_AO_PORT_NUM;
    if (CONFIG_AUDIO_AO_PORT_NUM > 1) {
        sndAttr->port_attr[0].out_port = CONFIG_AUDIO_AO_OUT_PORT_1;
        sndAttr->port_attr[1].out_port = CONFIG_AUDIO_AO_OUT_PORT_2;
    } else {
        sndAttr->port_attr[0].out_port = CONFIG_AUDIO_AO_OUT_PORT_1;
    }
#else
    sndAttr->channels = inputAttr->channelCount;
    sndAttr->bit_depth = AUDIO_BIT_WIDTH_16_BIT;
    sndAttr->sample_rate = inputAttr->sampleRate;
#endif
    if (inputAttr->type == AUDIO_IN_NOTIFICATION_SYSTEM ||
        inputAttr->type == AUDIO_IN_MUSIC) {
        sndAttr->port_attr[0].out_port = UAPI_SND_OUT_PORT_DAC1;
    } else if (inputAttr->type == AUDIO_IN_USB_VOICE_CALL) {
        sndAttr->channels = AUDIO_CHANNEL_COUNT_2;
        sndAttr->bit_depth = AUDIO_SAMPLE_BITDEPTH_24;
    }
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX)
    if (sndAttr->port_attr[0].out_port >=  UAPI_SND_OUT_PORT_DAC0 &&
        sndAttr->port_attr[0].out_port < UAPI_SND_OUT_PORT_I2S0) {
        sndAttr->port_attr[0].u.dac_attr.ana_out_sel[0] = CONFIG_AUDIO_AO_DAC_OUT_PORT_HS_0;
        sndAttr->port_attr[0].u.dac_attr.ana_out_sel[1] = CONFIG_AUDIO_AO_DAC_OUT_PORT_HS_1;
    }
#endif
#endif
    return MEDIA_HAL_OK;
}

#ifndef MELODY_AUDIO_SUPPORT
int32_t GetExternAIAttr(uapi_ai_attr *aiAttr)
{
    aiAttr->port_attr.i2s.i2s_attr.master = EXTERNAL_AI_I2S_ATTR_MASTER;
    aiAttr->port_attr.i2s.i2s_attr.i2s_mode = EXTERNAL_AI_I2S_ATTR_I2S_MODE;
    aiAttr->port_attr.i2s.i2s_attr.mclk = EXTERNAL_AI_I2S_ATTR_MCLK;
    aiAttr->port_attr.i2s.i2s_attr.bclk = EXTERNAL_AI_I2S_ATTR_BCLK;
    aiAttr->port_attr.i2s.i2s_attr.bit_depth = EXTERNAL_AI_I2S_ATTR_BIT_DEPTH;
    aiAttr->port_attr.i2s.i2s_attr.channels = EXTERNAL_AI_I2S_ATTR_AUDIO_CH;
    aiAttr->port_attr.i2s.i2s_attr.pcm_sample_rise_edge = EXTERNAL_AI_I2S_ATTR_SAMPLE_RISE_EDGE;
    aiAttr->port_attr.i2s.i2s_attr.pcm_delay_cycle = EXTERNAL_AI_I2S_ATTR_PCM_DELAY_CYCLE;
    aiAttr->pcm_attr.channels = EXTERNAL_AI_PCM_ATTR_CHANNELS;
    aiAttr->pcm_attr.bit_depth = EXTERNAL_AI_PCM_ATTR_BIT_DEPTH;
    aiAttr->pcm_attr.sample_rate = EXTERNAL_AI_PCM_ATTR_SAMPLE_RATE;
    aiAttr->pcm_attr.sample_per_frame = aiAttr->pcm_attr.sample_rate / EXTERNAL_AI_PCM_ATTR_FRAME_PER_SEC;
    aiAttr->ref_attr.enable = TD_FALSE;
    return MEDIA_HAL_OK;
}

int32_t GetExternAOAttr(uapi_snd_attr *sndAttr)
{
    sndAttr->port_num = EXTERNAL_SND_OUT_PORT_NUM;
    sndAttr->port_attr[0].out_port = EXTERNAL_SND_OUT_PORT;
    sndAttr->port_attr[0].u.i2s_attr.attr.bit_depth = EXTERNAL_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->port_attr[0].u.i2s_attr.attr.channels = EXTERNAL_SND_OUT_I2S_ATTR_CHANNEL;
    sndAttr->port_attr[0].u.i2s_attr.attr.i2s_mode = EXTERNAL_SND_OUT_I2S_ATTR_I2S_MODE;
    sndAttr->port_attr[0].u.i2s_attr.attr.mclk = EXTERNAL_SND_OUT_I2S_ATTR_MCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.master = EXTERNAL_SND_OUT_I2S_ATTR_MASTER;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_sample_rise_edge = EXTERNAL_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE;
    sndAttr->port_attr[0].u.i2s_attr.attr.bclk = EXTERNAL_SND_OUT_I2S_ATTR_BCLK;
    sndAttr->channels = EXTERNAL_SND_OUT_AUDIO_CHANNEL;
    sndAttr->bit_depth = EXTERNAL_SND_OUT_AUDIO_BIT_DEPTH;
    sndAttr->sample_rate = EXTERNAL_SND_OUT_AUDIO_SAMPLE_RATE;
    return MEDIA_HAL_OK;
}
#endif
const AudioPortPlugin *GetPortPluginFuncs(AudioPluginType pluginType)
{
    if (pluginType == AUDIO_PLUGIN_PRIMARY) {
        return GetPrimaryPortPluginFuncs(pluginType);
    } else if (pluginType == AUDIO_PLUGIN_BLUETOOTH) {
#ifdef SUPPORT_BT
        return GetBluetoothPortPluginFuncs(pluginType);
#else
        return NULL;
#endif
    } else if (pluginType == AUDIO_PLUGIN_MODEM) {
#ifdef SUPPORT_MODEM
        return GetModemPortPluginFuncs(pluginType);
#else
        return NULL;
#endif
    } else if (pluginType == AUDIO_PLUGIN_USB) {
#ifdef SUPPORT_USB
        return GetUsbPortPluginFuncs(pluginType);
#else
        return NULL;
#endif
    }

    MEDIA_HAL_LOGE(MODULE_NAME, "invalid plugin type %d", pluginType);
    return NULL;
}

void CreateAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, isBt ? MEDIA_CHR_CREATE_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_CREATE_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_A2DP_MUSIC:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, MEDIA_CHR_CREATE_BT_SINK_STREAM_IN);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, isBt ? MEDIA_CHR_CREATE_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_CREATE_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_RECOGNITION:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, MEDIA_CHR_CREATE_ASR_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_RECORD:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, MEDIA_CHR_CREATE_RECORD_STREAM_IN);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, MEDIA_CHR_CREATE_VIDEO_STREAM_OUT);
            break;
        case AUDIO_IN_BT_VOICE_CALL:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, isOutput ? MEDIA_CHR_CREATE_LOCAL_SCO_STREAM_IN
                : MEDIA_CHR_CREATE_BT_SCO_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_ASSISTANT:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM, MEDIA_CHR_CREATE_VOICE_ASSISTANT_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_CALL_VOLTE:
            LOG_MEDIA(MEDIA_CHR_INIT_STREAM,
                isOutput ? (isBt ? MEDIA_CHR_CREATE_BT_CAT1_STREAM_OUT : MEDIA_CHR_CREATE_LOCAL_CAT1_STREAM_OUT) :
                (isBt ? MEDIA_CHR_CREATE_BT_CAT1_STREAM_IN : MEDIA_CHR_CREATE_LOCAL_CAT1_STREAM_IN)
            );
            break;
        default:
            break;
    }
}

void StartAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, isBt ? MEDIA_CHR_START_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_START_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_A2DP_MUSIC:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, MEDIA_CHR_START_BT_SINK_STREAM_IN);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, isBt ? MEDIA_CHR_START_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_START_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_RECOGNITION:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, MEDIA_CHR_START_ASR_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_RECORD:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, MEDIA_CHR_START_RECORD_STREAM_IN);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, MEDIA_CHR_START_VIDEO_STREAM_OUT);
            break;
        case AUDIO_IN_BT_VOICE_CALL:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, isOutput ? MEDIA_CHR_START_LOCAL_SCO_STREAM_IN
                : MEDIA_CHR_START_BT_SCO_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_ASSISTANT:
            LOG_MEDIA(MEDIA_CHR_START_STREAM, MEDIA_CHR_START_VOICE_ASSISTANT_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_CALL_VOLTE:
            LOG_MEDIA(MEDIA_CHR_START_STREAM,
                isOutput ? (isBt ? MEDIA_CHR_START_BT_CAT1_STREAM_OUT : MEDIA_CHR_START_LOCAL_CAT1_STREAM_OUT) :
                (isBt ? MEDIA_CHR_START_BT_CAT1_STREAM_IN : MEDIA_CHR_START_LOCAL_CAT1_STREAM_IN)
            );
            break;
        default:
            break;
    }
}

void PauseAudioLinkChr(enum AudioCategory audioCategory, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_PAUSE_STREAM, isBt ? MEDIA_CHR_PAUSE_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_PAUSE_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_PAUSE_STREAM, isBt ? MEDIA_CHR_PAUSE_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_PAUSE_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_PAUSE_STREAM, MEDIA_CHR_PAUSE_VIDEO_STREAM_OUT);
            break;
        default:
            break;
    }
}

void ResumeAudioLinkChr(enum AudioCategory audioCategory, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_RESUME_STREAM, isBt ? MEDIA_CHR_RESUME_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_RESUME_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_RESUME_STREAM, isBt ? MEDIA_CHR_RESUME_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_RESUME_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_RESUME_STREAM, MEDIA_CHR_RESUME_VIDEO_STREAM_OUT);
            break;
        default:
            break;
    }
}

void StopAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, isBt ? MEDIA_CHR_STOP_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_STOP_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_A2DP_MUSIC:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_BT_SINK_STREAM_IN);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, isBt ? MEDIA_CHR_STOP_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_STOP_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_RECOGNITION:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_ASR_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_RECORD:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_RECORD_STREAM_IN);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_VIDEO_STREAM_OUT);
            break;
        case AUDIO_IN_BT_VOICE_CALL:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, isOutput ? MEDIA_CHR_STOP_LOCAL_SCO_STREAM_IN
                : MEDIA_CHR_STOP_BT_SCO_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_ASSISTANT:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_VOICE_ASSISTANT_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_CALL_VOLTE:
            LOG_MEDIA(MEDIA_CHR_STOP_STREAM,
                isOutput ? (isBt ? MEDIA_CHR_STOP_BT_CAT1_STREAM_OUT : MEDIA_CHR_STOP_LOCAL_CAT1_STREAM_OUT) :
                (isBt ? MEDIA_CHR_STOP_BT_CAT1_STREAM_IN : MEDIA_CHR_STOP_LOCAL_CAT1_STREAM_IN)
            );
            break;
        default:
            break;
    }
}

void DestoryAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt)
{
    switch (audioCategory) {
        case AUDIO_IN_MUSIC:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, isBt ? MEDIA_CHR_RELEASE_MUSIC_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_RELEASE_LOCAL_MUSIC_STREAM_OUT);
            break;
        case AUDIO_IN_A2DP_MUSIC:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_BT_SINK_STREAM_IN);
            break;
        case AUDIO_IN_NOTIFICATION_SYSTEM:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, isBt ? MEDIA_CHR_RELEASE_NOTIFY_BT_SOURCE_STREAM_OUT
                : MEDIA_CHR_RELEASE_LOCAL_NOTIFY_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_RECOGNITION:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_ASR_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_RECORD:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_RECORD_STREAM_IN);
            break;
        case AUDIO_IN_FITNESS_VIDEO:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_VIDEO_STREAM_OUT);
            break;
        case AUDIO_IN_BT_VOICE_CALL:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, isOutput ? MEDIA_CHR_RELEASE_LOCAL_SCO_STREAM_IN
                : MEDIA_CHR_RELEASE_BT_SCO_STREAM_OUT);
            break;
        case AUDIO_IN_VOICE_ASSISTANT:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_VOICE_ASSISTANT_STREAM_IN);
            break;
        case AUDIO_IN_VOICE_CALL_VOLTE:
            LOG_MEDIA(MEDIA_CHR_RELEASE_STREAM,
                isOutput ? (isBt ? MEDIA_CHR_RELEASE_BT_CAT1_STREAM_OUT : MEDIA_CHR_RELEASE_LOCAL_CAT1_STREAM_OUT) :
                (isBt ? MEDIA_CHR_RELEASE_BT_CAT1_STREAM_IN : MEDIA_CHR_RELEASE_LOCAL_CAT1_STREAM_IN)
            );
            break;
        default:
            break;
    }
}

#ifdef SUPPORT_AUDIO_EFFECT_PARAM
void SetAefExtraParam(td_handle soundHandle)
{
#ifdef SUPPORT_NV
    uint32_t size = g_aefParam.size * (uint32_t)sizeof(g_aefParam.data[0]);
    int32_t ret = uapi_snd_set_aef_param(soundHandle, BUILTIN_SND_OUT_PORT, SND_PORT_AEF_TYPE,
        (const td_void *)g_aefParam.data, (td_u32)size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "uapi set aef param faild, ret: 0x%x", ret);
    }
#endif
}
#endif

#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE)) || defined(MELODY_DEMO_SOUNDBAR)
int32_t AudioSetL2HCParam(char *value, uint32_t valueLen)
{
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t cmd = 0;
    uint32_t paramLen = 0;
    int32_t isEncode = 0;
    int32_t ret = sscanf_s(value, "setconfigparam = %u#%u#%d:", &cmd, &paramLen, &isEncode);
    if (ret != SSCANF_NUM3) {
        MEDIA_HAL_LOGE(MODULE_NAME, "setconfigparam sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (cmd != AUDIO_STREAM_CONFIG_L2HC_CMD) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cmd = %u!", cmd);
        return MEDIA_HAL_ERR;
    }
#if defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_SOUNDBAR)
    // dongle上行是adec需要设置，下行是aenc需要设置，和tws需要设置的模块相反。
    isEncode = !isEncode;
#endif
    if (isEncode == 0) {
        ret = GetParamByValue((uint8_t *)&g_l2hcAdecConfig, paramLen, value, valueLen, paramLen);
        g_l2hcAdecIsSetted = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "adec protocol = %u, vendor_id = %u, version_id = %u, aggregate_enable = %u, frame_len = %u\r\n",
            g_l2hcAdecConfig.version.protocol, g_l2hcAdecConfig.version.vendor_id, g_l2hcAdecConfig.version.version_id,
            g_l2hcAdecConfig.aggregate_enable, g_l2hcAdecConfig.frame_len);
    } else {
        ret = GetParamByValue((uint8_t *)&g_l2hcAencConfig, paramLen, value, valueLen, paramLen);
        g_l2hcAencIsSetted = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "aenc protocol = %u, vendor_id = %u, version_id = %u, aggregate_enable = %u, frame_len = %u\r\n",
            g_l2hcAencConfig.version.protocol, g_l2hcAencConfig.version.vendor_id, g_l2hcAencConfig.version.version_id,
            g_l2hcAencConfig.aggregate_enable, g_l2hcAencConfig.frame_len);
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}
#endif

#ifdef SUPPORT_SEA_AHE_PARAM
void SetSeaLibId(int32_t libId)
{
    g_seaLibId = libId;
    MEDIA_HAL_LOGT(MODULE_NAME, "Set libid:%d!", g_seaLibId);
}

int32_t GetSeaLibId(void)
{
    return g_seaLibId;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
