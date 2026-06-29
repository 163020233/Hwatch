/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#include "device_hal.h"
#include <stdlib.h>
#include <stdio.h>
#include "securec.h"
#include "device_config.h"
#include "audio_manager.h"
#include "audio_adapter.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "media_mem.h"
#include "media_chr_log.h"

#define LOG_MODULE_NAME "AudioDeviceMgr"

#define MAX_AUDIO_FORMAT_NUM 20
#define MAX_AUDIO_ADAPTER_NUM 4
#define MAX_AUDIO_ADAPTER_PORT_NUM 2
#define DEVICE_INVOKE_SIZE (sizeof(g_deviceInvokeNameId) / sizeof(g_deviceInvokeNameId[0]))

typedef struct {
    struct AudioAdapter *audioAdapter;
    const struct AudioAdapterDescriptor *desc;
    AudioCapability capability;
} AudioAdapterDesc;

typedef struct {
    struct AudioManager *audioManager;
    uint32_t adapterNum;
    AudioAdapterDesc audioAdapterDesc[MAX_AUDIO_ADAPTER_NUM];
} AudioHalDesc;

static AudioHalDesc g_audioHalDesc;

typedef struct {
    int32_t frameworkStreamType;
    int32_t halStreamType;
} StreamTypeMap;

static StreamTypeMap g_mapStreamType[] = {
    { AUDIO_STREAM_MUSIC, AUDIO_IN_MUSIC },
    { AUDIO_STREAM_ALARM_SYSTEM, AUDIO_IN_NOTIFICATION_SYSTEM },
    { AUDIO_STREAM_NOTIFICATION, AUDIO_IN_NOTIFICATION_SYSTEM },
    { AUDIO_STREAM_NOTIFICATION_SYSTEM, AUDIO_IN_NOTIFICATION_SYSTEM },
    { AUDIO_STREAM_NOTIFICATION_PROMPT, AUDIO_IN_NOTIFICATION_SYSTEM },
    { AUDIO_STREAM_VOICE_ASSISTANT, AUDIO_IN_VOICE_ASSISTANT },
    { AUDIO_STREAM_TTS, AUDIO_IN_TTS },
    { AUDIO_STREAM_VOICE_RECORD, AUDIO_IN_VOICE_RECORD },
#ifdef SUPPORT_BT
    { AUDIO_STREAM_A2DP_MUSIC, AUDIO_IN_A2DP_MUSIC },
    { AUDIO_STREAM_VOICE_CALL, AUDIO_IN_VOICE_CALL },
    { AUDIO_STREAM_VOICE_CALL_BT_SCO, AUDIO_IN_BT_VOICE_CALL },
#endif
#ifdef SUPPORT_MODEM
    { AUDIO_STREAM_VOICE_CALL_VOLTE, AUDIO_IN_VOICE_CALL_VOLTE },
#endif
#ifdef HI3322_CHIP_V100
    { AUDIO_STREAM_VOICE_CALL_VOLTE_SPI, AUDIO_IN_VOICE_CALL_VOLTE_SPI },
#endif
    { AUDIO_STREAM_LIVE_MIC, AUDIO_IN_LIVE_MIC },
    { AUDIO_STREAM_USB_MUSIC, AUDIO_IN_USB_MUSIC },
    { AUDIO_STREAM_VOICE_CALL_USB, AUDIO_IN_USB_VOICE_CALL },
    { AUDIO_STREAM_NEARLINK_MUSIC, AUDIO_IN_NEARLINK_MUSIC },
    { AUDIO_STREAM_VOICE_CALL_NEARLINK, AUDIO_IN_NEARLINK_VOICE_CALL },
    { AUDIO_STREAM_FITNESS_VIDEO, AUDIO_IN_FITNESS_VIDEO },
    { AUDIO_STREAM_VOICE_RECOGNITION, AUDIO_IN_VOICE_RECOGNITION },
    { AUDIO_STREAM_ALARM_CLOCK, AUDIO_IN_MUSIC },
    { AUDIO_STREAM_RING, AUDIO_IN_MUSIC },
    { AUDIO_STREAM_FITNESS_VIDEO, AUDIO_IN_MUSIC },
    { AUDIO_STREAM_VOICE_PICKUP, AUDIO_IN_PICKUP },
};

typedef struct {
    char *funcName;
    AudioInvokeId invokeId;
    bool needParamLength;
} InvokeNameId;

static InvokeNameId g_deviceInvokeNameId[] = {
    {"HaidCreate", AUDIO_INVOKE_HAID_CREATE, true},
    {"HaidDestroy", AUDIO_INVOKE_HAID_DESTROY, false},
    {"HaidSetEnable", AUDIO_INVOKE_HAID_SET_ENABLE, false},
    {"HaidGetEnable", AUDIO_INVOKE_HAID_GET_ENABLE, true},
    {"HaidSetEventCallback", AUDIO_INVOKE_HAID_SET_EVENT_CALL_BACK, true},
    {"HaidSetVolume", AUDIO_INVOKE_HAID_SET_VOLUME, false},
    {"HaidGetVolume", AUDIO_INVOKE_HAID_GET_VOLUME, true},
    {"HaidSetHardwareConfig", AUDIO_INVOKE_HAID_SET_HARDWARE_CONFIG, true},
    {"HaidGetHardwareConfig", AUDIO_INVOKE_HAID_GET_HARDWARE_CONFIG, true},
    {"HaidCalibrationStart", AUDIO_INVOKE_HAID_START_CALIBRATION, false},
    {"HaidCalibrationStop", AUDIO_INVOKE_HAID_STOP_CALIBRATION, false},
    {"HaidSetTestConfig", AUDIO_INVOKE_HAID_SET_TEST_CONFIG, true},
    {"HaidIoctl", AUDIO_INVOKE_HAID_SET_IOCTL, true},
    {"HaidSetConfig", AUDIO_INVOKE_HAID_SET_CONFIG, true},
    {"HaidGetConfig", AUDIO_INVOKE_HAID_GET_CONFIG, true},
    {"HaidSetAllConfig", AUDIO_INVOKE_HAID_SET_ALL_CONFIG, true},
    {"HaidGetAllConfig", AUDIO_INVOKE_HAID_GET_ALL_CONFIG, true},
    {"HaidTonePlayForExamine", AUDIO_INVOKE_HAID_TONE_PLAY, false},
    {"HaidToneStopForExamine", AUDIO_INVOKE_HAID_TONE_STOP, false},
    {"HaidSetScene", AUDIO_INVOKE_HAID_SET_SCENE, false},
    {"HaidGetScene", AUDIO_INVOKE_HAID_GET_SCENE, true},
    {"HaidSetSpkCaliCoef", AUDIO_INVOKE_HAID_SET_SPK_CALI_COEF, true},
    {"HaidSetMicCaliVolume", AUDIO_INVOKE_HAID_SET_MIC_VOLUME, true},
    {"HaidGetEnvNoise", AUDIO_INVOKE_HAID_GET_ENV_NOISE, false},
    {"TwsSetMode", AUDIO_INVOKE_TWS_SET_MODE, false},
    {"TwsSetRole", AUDIO_INVOKE_TWS_SET_ROLE, false},
    {"TwsSetChannelMode", AUDIO_INVOKE_TWS_SET_CHANNEL, false},
    {"TwsSetProductForm", AUDIO_INVOKE_TWS_SET_PRODUCT_FORM, false},
    {"AncInit", AUDIO_INVOKE_ANC_INIT, false},
    {"AncDeinit", AUDIO_INVOKE_ANC_DEINIT, false},
    {"AncClose", AUDIO_INVOKE_ANC_CLOSE, false},
    {"AncOpen", AUDIO_INVOKE_ANC_OPEN, true},
    {"AncGetAttr", AUDIO_INVOKE_ANC_GET_ATTR, true},
    {"AncSetParam", AUDIO_INVOKE_ANC_SET_PARAM, true},
    {"AncGetParam", AUDIO_INVOKE_ANC_GET_PARAM, true},
    {"LiveMicSetMixMode", AUDIO_INVOKE_LIVEMIC_SET_MIXMODE, false},
    {"LiveMicGetMixMode", AUDIO_INVOKE_LIVEMIC_GET_MIXMODE, true},
    {"LiveMicSetMicMode", AUDIO_INVOKE_LIVEMIC_SET_MICMODE, false},
    {"LiveMicGetMicMode", AUDIO_INVOKE_LIVEMIC_GET_MICMODE, true},
    {"recorddbvolume", AUDIO_INVOKE_RECORD_DB_VOLUME, false},
    {"readdbvolume", AUDIO_INVOKE_READ_DB_VOLUME, true},
    {"setconfigparam", AUDIO_INVOKE_SET_CONFIG_PARAM, true},
#ifdef SUPPORT_A2DPOUT_MUSIC
    {"setaencattr", AUDIO_INVOKE_SET_A2DP_CODEC_ATTR, false},
#endif
    {"SetGlobalMute", AUDIO_INVOKE_SET_GLOBAL_MUTE, false},
    {"setmatrix", AUDIO_INVOKE_SET_VOLUME_MATRIX, true},
    {"getmatrix", AUDIO_INVOKE_GET_VOLUME_MATRIX, true},
    {"setencodeparam", AUDIO_INVOKE_SET_ENCODE_PARAM_EXT, false},
    {"SetSeaLibId", AUDIO_INVOKE_SEA_SET_LIB_ID, true},
};
static AudioDeviceHaidEventCallback g_deviceHaidCallBack = NULL;

int32_t StreamTypeFromFrameworkToHal(int32_t stream)
{
    int32_t retStream = AUDIO_IN_OTHERS;
    for (uint32_t i = 0; i < sizeof(g_mapStreamType) / sizeof(g_mapStreamType[0]); i++) {
        if (g_mapStreamType[i].frameworkStreamType == stream) {
            return g_mapStreamType[i].halStreamType;
        }
    }
    ALOGW("StreamTypeFromFrameworkToHal failed! stream = 0x%x", stream);
    return retStream;
}

static uint32_t ConvertChannelMaskFromHalToFramework(int32_t channelMasks)
{
    uint32_t retChannelMask;
    switch (channelMasks) {
        case AUDIO_CHANNEL_FRONT_LEFT:
            retChannelMask = AUDIO_CHANNEL_MODE_FRONT_LEFT;
            break;
        case AUDIO_CHANNEL_FRONT_RIGHT:
            retChannelMask = AUDIO_CHANNEL_MODE_FRONT_RIGHT;
            break;
        case AUDIO_CHANNEL_STEREO:
            retChannelMask = AUDIO_CHANNEL_MODE_STEREO;
            break;
        default:
            retChannelMask = AUDIO_CHANNEL_MODE_NONE;
            break;
    }
    return retChannelMask;
}

typedef struct {
    int32_t frameworkFormat;
    int32_t halFormat;
} FormatMap;

static FormatMap  g_mapFormat[] = {
    {AAC_LC,         AUDIO_FORMAT_TYPE_AAC_LC},
    {AAC_LD,         AUDIO_FORMAT_TYPE_AAC_LD},
    {AAC_ELD,        AUDIO_FORMAT_TYPE_AAC_ELD},
    {AAC_HE_V1,      AUDIO_FORMAT_TYPE_AAC_HE_V1},
    {AAC_HE_V2,      AUDIO_FORMAT_TYPE_AAC_HE_V2},
    {MP3,            AUDIO_FORMAT_TYPE_MP3},
    {mSBC,           AUDIO_FORMAT_TYPE_MSBC},
    {OPUS,           AUDIO_FORMAT_TYPE_OPUS},
    {VORBIS,         AUDIO_FORMAT_TYPE_VORBIS},
    {FLAC,           AUDIO_FORMAT_TYPE_FLAC},
    {APE,            AUDIO_FORMAT_TYPE_APE},
    {SILK,           AUDIO_FORMAT_TYPE_SILK},
    {SBC,            AUDIO_FORMAT_TYPE_SBC},
    {AMR_WB,         AUDIO_FORMAT_TYPE_AMR_WB},
    {L2HC,           AUDIO_FORMAT_TYPE_L2HC},
    {LHDC,           AUDIO_FORMAT_TYPE_LHDC},
    {LDAC,           AUDIO_FORMAT_TYPE_LDAC},
    {L2HC_VOICE,     AUDIO_FORMAT_TYPE_L2HC_VOICE},
    {G711A,          AUDIO_FORMAT_TYPE_G711A},
    {G711U,          AUDIO_FORMAT_TYPE_G711U},
    {G726,           AUDIO_FORMAT_TYPE_G726},
};

static int32_t FormatFromHalToFramework(int32_t format)
{
    int32_t retFormat = AUDIO_DEFAULT;
    for (uint32_t i = 0; i < sizeof(g_mapFormat) / sizeof(g_mapFormat[0]); i++) {
        if (g_mapFormat[i].halFormat == format) {
            return g_mapFormat[i].frameworkFormat;
        }
    }
    return retFormat;
}

static uint32_t FormatFromFrameworkToHal(int32_t format, const uint32_t sampleFmt)
{
    if (format == PCM) {
        switch (sampleFmt) {
            case 0x10: // for bt sco cvsd codec format
                return AUDIO_FORMAT_TYPE_PCM_16_BIT;
            case 0x18: // 1: sampleFmt
                return AUDIO_FORMAT_TYPE_PCM_24_BIT;
            case 0x20: // 2: sampleFmt
                return AUDIO_FORMAT_TYPE_PCM_32_BIT;
            case 0x21: // 2: sampleFmt
                return AUDIO_FORMAT_TYPE_PCM_32_BIT_FLOAT;
            default:
                return AUDIO_FORMAT_TYPE_PCM_16_BIT;
        }
    }

    uint32_t retFormat = AUDIO_FORMAT_TYPE_PCM_16_BIT;
    for (uint32_t i = 0; i < sizeof(g_mapFormat) / sizeof(g_mapFormat[0]); i++) {
        if (g_mapFormat[i].frameworkFormat == format) {
            return (uint32_t)g_mapFormat[i].halFormat;
        }
    }
    ALOGE("FormatFromFrameworkToHal, format %d not match", format);
    return retFormat;
}

static uint32_t InDeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_INVAILD;
    if ((device & IN_BUILTIN_MIC) != IN_FLAG) {
        retDevice |= PIN_IN_MIC;
    }
#ifdef SUPPORT_BT
    if ((device & IN_BLUETOOTH_SCO) != IN_FLAG ||
        (device & IN_NEARLINK_ISO_CALL) != IN_FLAG) {
        retDevice |= PIN_IN_BLUETOOTH_SCO;
    }
    if ((device & IN_NEARLINK_DONGLE) != IN_FLAG) {
        retDevice |= PIN_IN_NEARLINK_DONGLE;
    }
    if ((device & IN_BLUETOOTH_A2DP) != IN_FLAG ||
        (device & IN_NEARLINK_ISO_MUSIC) != IN_FLAG) {
        retDevice |= PIN_IN_BLUETOOTH_A2DP;
    }
#endif
#ifdef SUPPORT_MODEM
    if ((device & IN_MODEM) != IN_FLAG) {
        retDevice |= PIN_IN_MODEM;
    }
    if ((device & IN_MODEM_HEADSET) != IN_FLAG) {
        retDevice |= PIN_IN_MODEM_HEADSET;
    }
#endif
#ifdef SUPPORT_USB
    if ((device & IN_USB) != IN_FLAG) {
        retDevice |= PIN_IN_USB;
    }
    if ((device & IN_USB_DONGLE) != IN_FLAG) {
        retDevice |= PIN_IN_USB_DONGLE;
    }
#endif
    return retDevice;
}

static uint32_t OutDeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_INVAILD;
    if ((device & OUT_BUILTIN_SPEAKER) != 0) {
        retDevice |= PIN_OUT_SPEAKER;
    }
#ifdef SUPPORT_BT
    if ((device & OUT_BLUETOOTH_SCO) != 0 ||
        (device & OUT_NEARLINK_ISO_CALL) != 0) {
        retDevice |= PIN_OUT_BLUETOOTH_SCO;
    }
    if ((device & OUT_NEARLINK_DONGLE) != 0) {
        retDevice |= PIN_OUT_NEARLINK_DONGLE;
    }
    if ((device & OUT_NEARLINK_DONGLE) != 0) {
        retDevice |= PIN_OUT_NEARLINK_DONGLE;
    }
    if ((device & OUT_BLUETOOTH_A2DP) != 0 ||
        (device & OUT_NEARLINK_ISO_MUSIC) != 0) {
        retDevice |= PIN_OUT_BLUETOOTH_A2DP;
    }
#endif
#ifdef SUPPORT_MODEM
    if ((device & OUT_MODEM) != 0) {
        retDevice |= PIN_OUT_MODEM;
    }
    if ((device & OUT_MODEM_HEADSET) != 0) {
        retDevice |= PIN_OUT_MODEM_HEADSET;
    }
#endif
#ifdef SUPPORT_USB
    if ((device & OUT_USB) != 0) {
        retDevice |= PIN_OUT_USB;
    }
    if ((device & OUT_USB_DONGLE) != 0) {
        retDevice |= PIN_OUT_USB_DONGLE;
    }
#endif
    return retDevice;
}

static uint32_t DeviceFromFrameworkToHal(uint32_t device)
{
    uint32_t retDevice = PIN_INVAILD;
    if ((device & IN_FLAG) != 0) {
        return InDeviceFromFrameworkToHal(device);
    } else {
        return OutDeviceFromFrameworkToHal(device);
    }
    return retDevice;
}

static char *GetAudioAdapterNameByDevice(uint32_t device)
{
    int32_t adapterConfigNum = 0;
    int32_t ret;
    AdapterDeviceMap *adapterList = NULL;
    ret = GetAdapterListFromConfig(&adapterList, &adapterConfigNum);
    if (ret != AUDIO_SUCCESS || adapterList == NULL) {
        return NULL;
    }
    for (int32_t i = 0; i < adapterConfigNum && i < MAX_ADAPTER_NUM; i++) {
        if (adapterList[i].isMatch == false) {
            continue;
        }
        for (uint32_t j = 0; j < adapterList[i].supportDeviceCount; j++) {
            if (((uint32_t)adapterList[i].supportDevice[j] & device) != 0 &&
                ((uint32_t)adapterList[i].supportDevice[j] & device) != IN_FLAG) {
                return adapterList[i].adapterName;
            }
        }
    }
    return NULL;
}
static struct AudioAdapter *GetAudioAdapterByDevice(uint32_t device)
{
    char *supportAdapterName = GetAudioAdapterNameByDevice(device);
    ALOGI("supportAdapterName device:%x", device);
    CHK_NULL_RETURN(supportAdapterName, NULL, "GetAudioAdapterNameByDevice failed, not match");
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (strcmp(supportAdapterName, g_audioHalDesc.audioAdapterDesc[i].desc->adapterName) == 0) {
            return g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        }
    }
    return NULL;
}

static int32_t FindAudioAdapterFromHal(void)
{
    int32_t adapterConfigNum = 0;
    int32_t count = 0;
    int32_t ret;
    AdapterDeviceMap *adapterList = NULL;
    ret = GetAdapterListFromConfig(&adapterList, &adapterConfigNum);
    if (ret != AUDIO_SUCCESS || adapterList == NULL) {
        return AUDIO_ERROR;
    }

    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        for (int32_t j = 0; j < adapterConfigNum; j++) {
            if (strcmp(g_audioHalDesc.audioAdapterDesc[i].desc->adapterName, adapterList[j].adapterName) == 0) {
                adapterList[j].isMatch = true;
                count++;
                break;
            }
        }
    }
    if (count == 0) {
        ALOGE("FindAudioAdapterFromHal failed, not match");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static AudioCodecFormat *ConvertFormatsFromHalToFramework(uint32_t formatNum, const enum AudioFormat *format)
{
    if (format == NULL) {
        return NULL;
    }
    AudioCodecFormat *retFormat = NULL;
    if (formatNum > MAX_AUDIO_FORMAT_NUM) {
        ALOGE("ConvertFormatsFromHalToFramework formatNum:%u is error", formatNum);
        return NULL;
    }
    retFormat = (AudioCodecFormat *)MediaMemMalloc(formatNum * sizeof(AudioCodecFormat));
    CHK_NULL_RETURN(retFormat, NULL, "ConvertFormatsFromHalToFramework malloc failed");
    if (memset_s(retFormat, formatNum * sizeof(AudioCodecFormat), 0, formatNum * sizeof(AudioCodecFormat)) != EOK) {
        ALOGE("memset_s failed");
    }
    for (uint32_t i = 0; i < formatNum; i++) {
        retFormat[i] = (AudioCodecFormat)FormatFromHalToFramework(format[i]);
    }
    return retFormat;
}

static void LoadDevicePortCapability(uint32_t index, const struct AudioPortCapability *capability)
{
    AudioCapability *cap = &g_audioHalDesc.audioAdapterDesc[index].capability;
    cap->channelMasks = ConvertChannelMaskFromHalToFramework(capability->channelMasks);
    cap->bitWidth = 0;
    cap->channel = capability->channelCount;
    cap->sampleRateMasks = capability->sampleRateMasks;
    if (capability->formatNum <= MAX_AUDIO_FORMAT_NUM && capability->formatNum > 0) {
        cap->numFormat = capability->formatNum;
        cap->formats = ConvertFormatsFromHalToFramework(capability->formatNum, capability->formats);
    } else {
        cap->numFormat = 0;
        cap->formats = NULL;
        ALOGI("LoadDevicePortCapability formatNum:%d", capability->formatNum);
    }
}

static int32_t LoadAudioAdapter(const struct AudioAdapterDescriptor *descs, int32_t size)
{
    struct AudioManager *manager = NULL;
    struct AudioAdapter *adapter = NULL;
    uint32_t adapterIndex = 0;

    manager = g_audioHalDesc.audioManager;
    for (int32_t index = 0; index < size; index++) {
        const struct AudioAdapterDescriptor *desc = &descs[index];
        if (desc != NULL && desc->portNum > MAX_AUDIO_ADAPTER_PORT_NUM) {
            ALOGE("Adapter: %d has more port, num is %d", index, desc->portNum);
            return AUDIO_ERROR;
        }
    }

    for (int32_t index = 0; index < size && adapterIndex < MAX_AUDIO_ADAPTER_NUM; index++) {
        const struct AudioAdapterDescriptor *desc = &descs[index];
        if (manager->LoadAdapter(manager, desc, &g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter) == 0) {
            adapter = g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter;
            if (adapter->InitAllPorts(adapter) != 0) {
                ALOGE("Adapter index = %d InitAllPorts failed", index);
                manager->UnloadAdapter(manager, adapter);
                g_audioHalDesc.audioAdapterDesc[adapterIndex].audioAdapter = NULL;
                return AUDIO_ERROR;
            }
            g_audioHalDesc.audioAdapterDesc[adapterIndex].desc = desc;
            adapterIndex++;
        }
    }
    if (adapterIndex == 0) {
        ALOGE("adapter load failed");
        return AUDIO_ERROR;
    }
    g_audioHalDesc.adapterNum = adapterIndex;
    return AUDIO_SUCCESS;
}

int32_t LoadAudioHal(void)
{
    int32_t ret;
    int32_t size = 0;
    struct AudioAdapterDescriptor *descs = NULL;
    struct AudioManager *manager = NULL;
    if (memset_s(&g_audioHalDesc, sizeof(AudioHalDesc), 0, sizeof(AudioHalDesc)) != EOK) {
        ALOGE("memset_s failed");
    }
    g_audioHalDesc.audioManager = GetAudioManagerFuncs();
    CHK_NULL_RETURN(g_audioHalDesc.audioManager, AUDIO_ERROR, "audioManager is NULL");

    manager = g_audioHalDesc.audioManager;
    ret = manager->GetAllAdapters(manager, &descs, &size);
    if (ret != AUDIO_SUCCESS || size > MAX_AUDIO_ADAPTER_NUM) {
        ALOGE("GetAllAdapters return failed, ret:%d, size:%d", ret, size);
        return AUDIO_ERROR;
    }
    if (LoadAudioAdapter(descs, size) != AUDIO_SUCCESS) {
        ALOGE("LoadAudioAdapter failed");
        return AUDIO_ERROR;
    }
    if (FindAudioAdapterFromHal() != AUDIO_SUCCESS) {
        ALOGE("adapter not matach");
        return AUDIO_ERROR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_INIT,
        MEDIA_CHR_DEVICE_MANAGER_INIT, MEDIA_CHR_LOAD_AUDIO_ADPATER);
    return AUDIO_SUCCESS;
}

int32_t UnLoadAudioHal(void)
{
    struct AudioManager *manager = g_audioHalDesc.audioManager;
    struct AudioAdapter *adapter = NULL;
    CHK_NULL_RETURN_NO_LOG(manager, AUDIO_SUCCESS);
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (g_audioHalDesc.audioAdapterDesc[i].capability.formats) {
            MediaMemFree(g_audioHalDesc.audioAdapterDesc[i].capability.formats);
            g_audioHalDesc.audioAdapterDesc[i].capability.formats = NULL;
        }
        adapter = g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        manager->UnloadAdapter(manager, adapter);
    }
    if (memset_s((void *)&g_audioHalDesc, sizeof(AudioHalDesc), 0, sizeof(AudioHalDesc)) != EOK) {
        ALOGE("memset_s failed");
    }
    return AUDIO_SUCCESS;
}

int32_t GetAdapterCapability(void)
{
    struct AudioPort *port = NULL;
    const struct AudioAdapterDescriptor *desc = NULL;
    struct AudioAdapter *adapter = NULL;
    struct AudioPortCapability capability;
    int32_t ret = 0;
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        desc = g_audioHalDesc.audioAdapterDesc[i].desc;
        adapter = g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        if (memset_s(&capability, sizeof(struct AudioPortCapability), 0, sizeof(struct AudioPortCapability)) != EOK) {
            ALOGE("memset_s failed");
        }
        for (uint32_t j = 0; j < desc->portNum; j++) {
            port = &(desc->ports[j]);
            ret = adapter->GetPortCapability(adapter, port, &capability);
            if (ret != 0) {
                continue;
            } else {
                break;
            }
        }
        LoadDevicePortCapability(i, &capability);
    }
    return AUDIO_SUCCESS;
}

AudioCapability *GetDeviceCapability(AudioDeviceType device)
{
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (adapter != NULL && adapter == g_audioHalDesc.audioAdapterDesc[i].audioAdapter) {
            return &g_audioHalDesc.audioAdapterDesc[i].capability;
        }
    }
    return NULL;
}

int32_t OpenInputStream(uint32_t device, AudioStreamConfig attr, Handle *inputStreamHandle)
{
    int32_t ret;
    if (g_audioHalDesc.adapterNum == 0 || inputStreamHandle == NULL) {
        return AUDIO_ERROR;
    }
    struct AudioSampleAttributes param;
    param.type = (enum AudioCategory)StreamTypeFromFrameworkToHal(attr.type);
    param.sampleRate = attr.sampleRate;
    param.format = FormatFromFrameworkToHal(attr.format, attr.sampleFmt);
    param.bitWidth = attr.sampleFmt;
    param.channelCount = attr.channelCount;
    param.interleaved = false;
    param.disableSEA = attr.disableSEA;
    param.micType = attr.micType;
    param.isAlwaysWake = attr.isAlwaysWake;
    param.frameLength = attr.frameLength;
    struct AudioCapture **capture = (struct AudioCapture **)inputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        struct AudioDeviceDescriptor deviceDesc = {};
        deviceDesc.portId = 0;
        deviceDesc.pins = DeviceFromFrameworkToHal(device);
        deviceDesc.desc = NULL;
        ret = adapter->CreateCapture(adapter, &deviceDesc, &param, capture);
        if (ret != 0 || *capture == NULL) {
            ALOGE("openInputStream, ret:%d", ret);
            return AUDIO_ERROR;
        }
    } else {
        ALOGE("no adapter support device 0x%x", device);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t CloseInputStream(uint32_t device, Handle inputStreamHandle)
{
    if (g_audioHalDesc.adapterNum == 0) {
        return AUDIO_ERROR;
    }
    struct AudioCapture *capture = (struct AudioCapture *)inputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        adapter->DestroyCapture(adapter, capture);
    } else {
        ALOGE("no suitable adapter");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t OpenOutputStream(uint32_t device, AudioStreamConfig attr, Handle *outputStreamHandle)
{
    int32_t ret;
    if (g_audioHalDesc.adapterNum == 0 || outputStreamHandle == NULL) {
        return AUDIO_ERROR;
    }
    struct AudioSampleAttributes param;
    param.type = (enum AudioCategory)StreamTypeFromFrameworkToHal(attr.type);
    param.sampleRate = attr.sampleRate;
    param.format = FormatFromFrameworkToHal(attr.format, attr.sampleFmt);
    param.channelCount = attr.channelCount;
    param.interleaved = false;
    param.bitWidth = attr.bitWidth;
    param.deviceId = attr.deviceId;
    param.connectedId = attr.connectedId;
    param.frameLength = attr.frameLength;
    struct AudioRender **render = (struct AudioRender **)outputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        struct AudioDeviceDescriptor deviceDesc = {};
        deviceDesc.portId = 0;
        deviceDesc.pins = DeviceFromFrameworkToHal(device);
        deviceDesc.desc = NULL;
        ret = adapter->CreateRender(adapter, &deviceDesc, &param, render);
        if (ret != 0 || render == NULL) {
            ALOGE("openOutputStream, ret:%d", ret);
            return AUDIO_ERROR;
        }
    } else {
        ALOGE("no adapter support device 0x%x", device);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t CloseOutputStream(uint32_t device, Handle outputStreamHandle)
{
    if (g_audioHalDesc.adapterNum == 0) {
        return AUDIO_ERROR;
    }
    struct AudioRender *render = (struct AudioRender *)outputStreamHandle;
    struct AudioAdapter *adapter = GetAudioAdapterByDevice(device);
    if (adapter != NULL) {
        adapter->DestroyRender(adapter, render);
    } else {
        ALOGE("no suitable adapter");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static struct AudioAdapter *GetAudioAdapter(AudioInvokeId audioInvokeId)
{
    char *supportAdapterName = "Primary";
#ifdef SUPPORT_A2DPOUT_MUSIC
    if (audioInvokeId == AUDIO_INVOKE_SET_A2DP_CODEC_ATTR) {
        supportAdapterName = "Bluetooth";
    }
#else
    AUDIO_UNUSED(audioInvokeId);
#endif
    for (uint32_t i = 0; i < g_audioHalDesc.adapterNum; i++) {
        if (strcmp(supportAdapterName, g_audioHalDesc.audioAdapterDesc[i].desc->adapterName) == 0) {
            return g_audioHalDesc.audioAdapterDesc[i].audioAdapter;
        }
    }
    return NULL;
}

static char *GetInvokeNameByHaidInvokeId(AudioInvokeId audioInvokeId)
{
    char *retValue = NULL;
    for (size_t i = 0; i < DEVICE_INVOKE_SIZE; i++) {
        if (g_deviceInvokeNameId[i].invokeId == audioInvokeId) {
            retValue = g_deviceInvokeNameId[i].funcName;
            break;
        }
    }
    return retValue;
}

static bool GetIsNeedLenParamByHaidInvokeId(AudioInvokeId audioInvokeId)
{
    for (size_t i = 0; i < DEVICE_INVOKE_SIZE; i++) {
        if (g_deviceInvokeNameId[i].invokeId == audioInvokeId) {
            return g_deviceInvokeNameId[i].needParamLength;
        }
    }
    return false;
}

int32_t DeviceHalInvoke(AudioInvokeId audioInvokeId, char *param, uint32_t *paramlength)
{
    struct AudioAdapter *adapter = GetAudioAdapter(audioInvokeId);
    CHK_NULL_RETURN(adapter, AUDIO_INVALID_PARAMS, "get adapter failed, not match");
    char *invokeName = GetInvokeNameByHaidInvokeId(audioInvokeId);
    CHK_NULL_RETURN(invokeName, AUDIO_INVALID_PARAMS, "get invoke name failed, not found ");
    bool needParamLength = GetIsNeedLenParamByHaidInvokeId(audioInvokeId);
    if (!needParamLength) {
        int32_t ret = adapter->SetExtraParams(adapter, AUDIO_EXT_PARAM_KEY_NONE,
            invokeName, param);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set ExtraParams failed!, audioInvokeId:%d, ret:%d", audioInvokeId, ret);
            return ret;
        }
    } else {
        int32_t ret = adapter->GetExtraParams(adapter, AUDIO_EXT_PARAM_KEY_NONE,
            invokeName, param, paramlength);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get ExtraParams failed!, audioInvokeId:%d, ret:%d", audioInvokeId, ret);
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t DeviceHaidEventCallback(enum AudioExtParamKey key, const char *condition, const char *value,
    void *reserved, void *cookie)
{
    AUDIO_UNUSED(condition);
    CHK_NULL_RETURN(g_deviceHaidCallBack, AUDIO_INVALID_PARAMS, "haid call back is null!");
    CHK_NULL_RETURN(value, AUDIO_INVALID_PARAMS, "value is null!");
    if (key != AUDIO_EXT_PARAM_HAID_MANAGER) {
        ALOGE("currently, only the callback function in hearing aid mode is supported.");
        return AUDIO_ERROR;
    }
    int32_t eventType = (int32_t)strtol(value, NULL, 10);
    return g_deviceHaidCallBack(eventType, reserved, cookie);
}

int32_t DeviceHalRegCallBack(AudioDeviceHaidEventCallback callBack, void *context)
{
    struct AudioAdapter* primaryAdapter = GetAudioAdapter(0);
    CHK_NULL_RETURN(primaryAdapter, AUDIO_INVALID_PARAMS, "get adapter failed, not match");
    int32_t ret = primaryAdapter->RegExtraParamObserver(primaryAdapter, DeviceHaidEventCallback, context);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("reg ExtraParams call back failed!, ret:%d", ret);
        return ret;
    }
    g_deviceHaidCallBack = callBack;
    return AUDIO_SUCCESS;
}
