/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: primary port
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "primary_port.h"

#include "primary_sub_port_in.h"
#include "primary_sub_port_out.h"

#include "plugin_common.h"
#include "media_hal_common.h"
#ifdef AUDIO_HAID_SUPPORT
#include "soc_uapi_haid.h"
#include "soc_uapi_anc.h"
#endif
#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_audio_sys.h"
#include "soc_uapi_anc.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioPrimaryPort"
#define PRIMARY_ATTR_BUF_LEN 25
#define PRIMARY_VOLUME_MATRIX_LEN 50
#define PRIMARY_GET_CONFIG_ATTR_BUF_LEN 520
#define ANC_CONFIG_BUF_LEN 516
#define ANC_ATTR_BUF_LEN 2130
#define FLOAT_VOLUME_TYPE 0xFF
#define FLOAT_VOLUME_ACCURACY 1000.0f
#define ANC_INDEX0 0
#define ANC_INDEX1 1
#define ANC_INDEX2 2
#define SSCANF_NUM7 7
#define SSCANF_NUM2 2

typedef struct {
    /* port context */
    bool isOpened;
    /* sub port context */
    AudioHandle subPortInHandle;
    AudioHandle subPortOutHandle;
    AudioHandle subPortTrackHandle;
#ifdef AUDIO_HAID_SUPPORT
    td_handle haidHandle;
#endif
} PrimaryPortContext;

typedef int32_t (*InvokeFunc)(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr);

typedef struct {
    enum PluginInvokeId invokeId;
    InvokeFunc invoke;
} PrimaryInvoke;

#ifdef AUDIO_HAID_SUPPORT
static PluginHaidEventCallBack g_haidPrimaryEventCallBack = NULL;
#endif

#ifndef AUDIO_HAID_SUPPORT
typedef struct {
    int32_t integer;
    int32_t decimal;
    int32_t volumeType;
} HalHaidVolume;
#endif

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
static int32_t g_liveMicMixMode = 0;
static int32_t g_liveMicMicMode = 0;
#endif

static int32_t PrimarySubPortCreateTrack(PrimaryPortContext *portCtx, struct PortPluginAttr *portAttr, bool isMic)
{
    int32_t ret;
    if (isMic) {
        ret = AudioPrimarySubPortInCreateTrack(portCtx->subPortInHandle, portAttr, &portCtx->subPortTrackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInCreateTrack failed");
    } else {
        ret = AudioPrimarySubPortOutCreateTrack(portCtx->subPortOutHandle, portAttr, &portCtx->subPortTrackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutCreateTrack failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimarySubPortDestroyTrack(const PrimaryPortContext *portCtx, AudioHandle trackHandle)
{
    int32_t ret;
    AudioPortTrack *portTrack = (AudioPortTrack *)trackHandle;
    if (portTrack->audioPin == PIN_IN_MIC) {
        ret = AudioPrimarySubPortInDestroyTrack(portCtx->subPortInHandle, trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInDestroyTrack failed");
    } else {
        ret = AudioPrimarySubPortOutDestroyTrack(portCtx->subPortOutHandle, trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutDestroyTrack failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortGetPortPluginCapability(const struct AudioPort *port,
    struct AudioPortCapability *capability)
{
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "Input param port is null");
    CHK_NULL_RETURN(capability, MEDIA_HAL_INVALID_PARAM, "Input param capability is null");
    int32_t ret;
    if (port->dir == PORT_IN) {
        ret = AudioPrimarySubPortInGetCapability(port, capability);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortInGetCapability failed");
    } else if (port->dir == PORT_OUT) {
        ret = AudioPrimarySubPortOutGetCapability(port, capability);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioPrimarySubPortOutGetCapability failed");
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "port direction: 0x%x is invalid", port->dir);
        return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "%s get capability success", port->portName);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortOpen(AudioHandle *portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");

    int32_t ret;
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)MediaHalMalloc(sizeof(PrimaryPortContext));
    if (priPortCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    if (memset_s(priPortCtx, sizeof(PrimaryPortContext), 0, sizeof(PrimaryPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    ret = AudioPrimarySubPortInCreate(&priPortCtx->subPortInHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPrimarySubPortInCreate failed(0x%x)", ret);
        goto FREE;
    }

    ret = AudioPrimarySubPortOutCreate(&priPortCtx->subPortOutHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPrimarySubPortOutCreate failed(0x%x)", ret);
        goto FREE;
    }

    priPortCtx->isOpened = true;
    *portHandle = priPortCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    if (priPortCtx->subPortInHandle != NULL) {
        MediaHalFree(priPortCtx->subPortInHandle);
    }
    MediaHalFree(priPortCtx);
    return ret;
}

static int32_t PrimaryPortCreateTrack(AudioHandle portHandle, enum AudioPortPin audioPortPin,
    struct PortPluginAttr *portAttr, AudioHandle *trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");
    CHK_NULL_RETURN(portAttr, MEDIA_HAL_INVALID_PARAM, "Input param portAttr is null");

    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");
    if (portAttr->type == AUDIO_IN_OTHERS) {
        MEDIA_HAL_LOGE(MODULE_NAME, "portAttr type 0x%x is invalid", portAttr->type);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = 0;
#ifdef AUDIO_SDK_SUPPORT
    ret = AudioSDKInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKInit failed(0x%x)", ret);
        return MEDIA_HAL_ERR;
    }
#endif

    if (!AudioPluginIsValidPin(audioPortPin, "primary")) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pin 0x%x is invalid", audioPortPin);
        return MEDIA_HAL_INVALID_PARAM;
    }
    bool isMic = (audioPortPin == PIN_IN_MIC) ? true : false;
    ret = PrimarySubPortCreateTrack(priPortCtx, portAttr, isMic);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "PrimarySubPortCreateTrack failed");

    *trackHandle = priPortCtx->subPortTrackHandle;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");

    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret = PrimarySubPortDestroyTrack(priPortCtx, trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "PrimarySubPortDestroyTrack failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryPortClose(AudioHandle portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret;

#ifdef AUDIO_SDK_SUPPORT
    ret = AudioSDKDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKDeInit failed");
#endif

    ret = AudioPrimarySubPortInDestroy(priPortCtx->subPortInHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioPrimarySubPortInDestroy failed");

    ret = AudioPrimarySubPortOutDestroy(priPortCtx->subPortOutHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioPrimarySubPortOutDestroy failed");

    priPortCtx->isOpened = false;
    MediaHalFree(priPortCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef MELODY_DEMO_EARPHONE
static int32_t PrimaryTwsSetMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uint32_t twsMode;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u", &twsMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_tws_mode((uapi_audio_tws_mode)twsMode);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws mode failed!");
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_tws_mode succecc! twsMode = %u", twsMode);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTwsSetRole(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uint32_t twsRole;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u", &twsRole);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws Role sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_tws_role((uapi_audio_tws_role)twsRole);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws Role failed!");
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_tws_role succecc! twsRole = %u", twsRole);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTwsSetChannelMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uint32_t channelMode;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u", &channelMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws channel sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_tws_mono_mode(channelMode);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws channel failed!");
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_tws_mono_mode succecc! channelMode = %u", channelMode);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTwsSetProductForm(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary tws invokeAttr->value == NULL");
    uint32_t productForm;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u", &productForm);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws product form sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_product_form((uapi_audio_product_form)productForm);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set tws product form failed!");
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_product_form succecc! productForm = %u", productForm);
    return MEDIA_HAL_OK;
}
#endif

#ifdef AUDIO_HAID_SUPPORT
static int32_t PrimaryHaidSetVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    uapi_haid_volume volume = {};
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d#%d#%d", &volume.integer, &volume.decimal,
        &volume.volume_type);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid volume sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (volume.volume_type == FLOAT_VOLUME_TYPE) {
        uapi_snd_gain sndGain = {};
        float realVloume = volume.integer / FLOAT_VOLUME_ACCURACY;
        VolumePercentToDb(realVloume, &sndGain);
        volume.integer = sndGain.integer;
        volume.decimal = sndGain.decimal;
        volume.volume_type = UAPI_HAID_POST_GAIN;
    }
    ret = uapi_haid_set_volume(priPortCtx->haidHandle, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid volume failed!");
    }
    return ret;
}

static int32_t PrimaryHaidGetVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    uapi_haid_volume volume = {};
    int32_t ret = uapi_haid_get_volume(priPortCtx->haidHandle, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid volume failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d#%d#%d",
        volume.integer, volume.decimal, volume.volume_type);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid volume sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static td_s32 PrimaryPortHaidEventCallBack(td_handle haid, uapi_haid_event_type eventType,
    td_void *param, td_void *context)
{
    MEDIA_HAL_UNUSED(haid);
    CHK_NULL_RETURN(g_haidPrimaryEventCallBack, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    return g_haidPrimaryEventCallBack(eventType, param, context);
}

static int32_t PrimaryHaidCreate(PrimaryPortContext *priPortCtx, const struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    CHK_NULL_RETURN(invokeAttr->length, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->length == NULL");
    td_s32 ret = uapi_haid_init();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid init failed!");
        return ret;
    }
    uapi_haid_attr haid_attr = {};
    ret = uapi_haid_get_default_attr(&haid_attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid get default attr failed!");
        return ret;
    }
    PluginHaidAttr *haidAttr = (PluginHaidAttr *)invokeAttr->value;
    for (size_t i = 0; i < haidAttr->usedLength; i++) {
        if (i >= UAPI_AI_MIC_NUM_MAX) {
            break;
        }
        if (haidAttr->micType[i] == PLUGIN_MIC_AMIC_0) {
            haid_attr.mic_sel[i] = UAPI_AI_AMIC0;
        } else if (haidAttr->micType[i] == PLUGIN_MIC_AMIC_1) {
            haid_attr.mic_sel[i] = UAPI_AI_AMIC1;
        } else if (haidAttr->micType[i] == PLUGIN_MIC_AMIC_2) {
            haid_attr.mic_sel[i] = UAPI_AI_AMIC2;
        } else {
            haid_attr.mic_sel[i] = UAPI_AI_AMIC_MAX;
        }
    }
    haid_attr.haid_voltage = (uapi_haid_voltage)haidAttr->voltage;
    ret = uapi_haid_create(&priPortCtx->haidHandle, &haid_attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid create failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidDestroy(PrimaryPortContext* priPortCtx, const struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    td_s32 ret = uapi_haid_destroy(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid destroy failed!");
        return ret;
    }
    ret = uapi_haid_deinit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary haid init failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetEnable(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    bool enable = false;
    int32_t retValue;
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d", &retValue);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid enable sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (retValue == 1) {
        enable = true;
    }
    ret = uapi_haid_set_enable(priPortCtx->haidHandle, enable);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid enable failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetEnable(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t enable = 0;
    int32_t ret = 0;
    ret = uapi_haid_get_enable(priPortCtx->haidHandle, (uint8_t *)&enable);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid enable failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", enable);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid enable sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetHardwareConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t ret = uapi_haid_set_hardware_config(priPortCtx->haidHandle, (const td_s8 *)invokeAttr->value,
        *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetHardwareConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t ret = uapi_haid_get_hardware_config(priPortCtx->haidHandle,
        (td_s8 *)invokeAttr->value, invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get hardware haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidCalibrationStart(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    int32_t ret = uapi_haid_calibration_start(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary start haid Calibration failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidCalibrationStop(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    int32_t ret = uapi_haid_calibration_stop(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary stop haid Calibration failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetTestConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t ret = uapi_haid_set_test_config(priPortCtx->haidHandle, (const td_s8 *)invokeAttr->value,
        *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid test config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidIoctl(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "input param invokeAttr value is null");
    uapi_haid_ioctl_param *ioctl_param = (uapi_haid_ioctl_param *)invokeAttr->value;
    int32_t ret = uapi_haid_ioctl(priPortCtx->haidHandle, ioctl_param);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid ioctl failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_s32 ret = uapi_haid_set_config(priPortCtx->haidHandle, (const td_s8*)invokeAttr->value, *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    td_s32 ret = uapi_haid_get_config(priPortCtx->haidHandle, (td_s8 *)invokeAttr->value, invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetAllConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_s32 ret = uapi_haid_set_all_config(priPortCtx->haidHandle, (const td_s8*)invokeAttr->value, *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid all config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetAllConfig(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    td_s32 ret = uapi_haid_get_all_config(priPortCtx->haidHandle, (td_s8*)invokeAttr->value, invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid all config failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryTonePlayForExamine(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    td_u32 frequence;
    uapi_haid_volume volume = {};
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%u#%d:%d:%d", &frequence, &volume.integer, &volume.decimal,
        &volume.volume_type);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary play tone sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_haid_sinus_tone_start(priPortCtx->haidHandle, frequence, &volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary play tone failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryToneStopForExamine(PrimaryPortContext *priPortCtx, const struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    td_s32 ret = uapi_haid_sinus_tone_stop(priPortCtx->haidHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary stop tone failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetScene(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    int32_t scene;
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d", &scene);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid scene sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_haid_set_scene(priPortCtx->haidHandle, (uapi_haid_scene)scene);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid scene failed!");
        return ret;
    }
    PrimaryPortHaidEventCallBack(priPortCtx->haidHandle, UAPI_HAID_EVENT_SCENE_CHANGE, &scene, NULL);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetScene(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t scene = 0;
    int32_t ret = 0;
    uapi_haid_scene haidScene;
    ret = uapi_haid_get_scene(priPortCtx->haidHandle, &haidScene);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid scene failed!");
        return ret;
    }
    scene = haidScene;
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", scene);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid scene sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimarySetSpkCaliCoef(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    td_s32 ret = uapi_haid_set_spk_cali_coef(priPortCtx->haidHandle, (const td_u8 *)invokeAttr->value,
        *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set spk cali coef failed!,ret = %x", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidSetMicVolume(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    CHK_NULL_RETURN(invokeAttr->length, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->length == NULL");
    uapi_haid_cali_volume config;
    int32_t ret = memcpy_s(&config, *invokeAttr->length, invokeAttr->value, *invokeAttr->length);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid mic volume memcpy_s failed");
        return ret;
    }
    ret = uapi_haid_set_mic_cali_volume(priPortCtx->haidHandle, &config);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set haid mic volume failed!,ret = %x", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryHaidGetEnvNosie(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t noise = 0;
    int32_t ret = 0;
    ret = uapi_haid_get_env_noise(priPortCtx->haidHandle, &noise);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid env noise failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", noise);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get haid env noise sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}
#endif

#ifdef SUPPORT_ANC
static int32_t PrimaryAncInit(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    int32_t ret = uapi_anc_init();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc init failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncDeinit(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(invokeAttr);
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    int32_t ret = uapi_anc_deinit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc deinit failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncAttrOpen(struct PluginInvokeAttr *invokeAttr, uint32_t configLen)
{
    uapi_anc_attr attr = {};
    int32_t ret = GetParamByValue((uint8_t *)&attr, configLen, invokeAttr->value, *invokeAttr->length, configLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed!");
        return ret;
    }
    ret = uapi_anc_open(&attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_open failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncOpen(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary anc invokeAttr->value == NULL");
    uint8_t ahaMode;
    uint32_t configLen = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u#%u:", &ahaMode, &configLen);
    if (ret != SSCANF_NUM2) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (ahaMode == UAPI_AHA_MODE_ANC_ON) {
        ret = PrimaryAncAttrOpen(invokeAttr, configLen);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "PrimaryAncAttrOpen failed!");
            return ret;
        }
        ret = uapi_anc_set_mode(UAPI_AHA_MODE_ANC_ON);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_set_mode failed!");
            return ret;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_anc_open succecc!");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncClose(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    MEDIA_HAL_UNUSED(invokeAttr);
    int32_t ret = uapi_anc_close();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_close failed!");
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_anc_close succecc!");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncGetAttr(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    uapi_anc_attr attr = {};
    int32_t ret = uapi_anc_get_attr(&attr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_get_attr failed!");
        return ret;
    }
    int32_t prefixLen = sprintf_s(invokeAttr->value, *invokeAttr->length, "%u:", (uint32_t)sizeof(uapi_anc_attr));
    if (prefixLen < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sprintf_s failed");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(invokeAttr->value + prefixLen, *invokeAttr->length - prefixLen, &attr,
        (uint32_t)sizeof(uapi_anc_attr)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryAncSetParam(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary anc invokeAttr->value == NULL");
    uint32_t cmd;
    uint32_t configLen = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u#%u:", &cmd, &configLen);
    if (ret != SSCANF_NUM2) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    uint8_t *buf = (uint8_t *)MediaHalMalloc(configLen);
    if (buf == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed!");
        return MEDIA_HAL_ERR;
    }
    ret = GetParamByValue(buf, configLen, invokeAttr->value, *invokeAttr->length, configLen);
    if (ret != MEDIA_HAL_OK) {
        goto FREE;
    }
    ret = uapi_anc_set_param(cmd, buf, configLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_set_param failed! ret = %d", ret);
        goto FREE;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_anc_set_param succecc! cmd = %u", cmd);
    MediaHalFree(buf);
    return MEDIA_HAL_OK;
FREE:
    MediaHalFree(buf);
    return MEDIA_HAL_ERR;
}

static int32_t PrimaryAncGetParam(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary anc invokeAttr->value == NULL");
    uint32_t cmd;
    uint32_t configLen = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%u#%u:", &cmd, &configLen);
    if (ret != SSCANF_NUM2) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary anc mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    uint8_t *buf = (uint8_t *)MediaHalMalloc(configLen);
    if (buf == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed!");
        return MEDIA_HAL_ERR;
    }
    ret = GetParamByValue(buf, configLen, invokeAttr->value, *invokeAttr->length, configLen);
    if (ret != MEDIA_HAL_OK) {
        goto FREE;
    }
    ret = uapi_anc_get_param(cmd, buf, configLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_anc_get_param failed!");
        goto FREE;
    }
    int32_t prefixLen = sprintf_s(invokeAttr->value, *invokeAttr->length, "%u#%u:", cmd,
        configLen);
    if (prefixLen < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sprintf_s failed");
        goto FREE;
    }
    if (memcpy_s(invokeAttr->value + prefixLen, *invokeAttr->length - prefixLen, buf,
        configLen) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        goto FREE;
    }
    MediaHalFree(buf);
    return MEDIA_HAL_OK;
FREE:
    MediaHalFree(buf);
    return MEDIA_HAL_ERR;
}
#endif

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
static int32_t PrimaryLivemicSetMicMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    int32_t micMode;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%d", &micMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set mic mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_mic_mode((uapi_audio_mic_mode)micMode);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set mic mode failed!");
        return ret;
    }
    g_liveMicMicMode = micMode;
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_mic_mode succecc! micMode = %d", micMode);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryLivemicGetMicMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    int32_t ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", g_liveMicMicMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get mic mode sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryLivemicSetMixMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    int32_t mixMode;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%d", &mixMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set mix mode sscanf_s failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_mic_mix_mode((uapi_audio_mic_mix_mode)mixMode);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set mix mode failed!");
        return ret;
    }
    g_liveMicMixMode = mixMode;
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi_audio_set_mic_mix_mode succecc! mixMode = %d", mixMode);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryLivemicGetMixMode(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    int32_t ret = 0;
    MEDIA_HAL_UNUSED(priPortCtx);
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", g_liveMicMixMode);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary get mix mode sprintf_s failed, ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t PrimaryLivemicSetVolumeMatrix(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    uint32_t paramLen;
    uint32_t port = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "setmatrix = %u#%u:", &port, &paramLen);
    if (ret != SSCANF_NUM2) {
        MEDIA_HAL_LOGE(MODULE_NAME, "PrimaryLivemicSetVolumeMatrix sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (paramLen > sizeof(uapi_audio_volume_matrix)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "paramLen is invalid, paramLen = %u", paramLen);
        return MEDIA_HAL_ERR;
    }
    uint8_t *param = (uint8_t *)MediaHalCalloc(paramLen);
    CHK_NULL_RETURN(param, MEDIA_HAL_INVALID_PARAM, "param malloc failed!");
    ret = GetParamByValue(param, paramLen, invokeAttr->value, *(invokeAttr->length), paramLen);
    if (ret != MEDIA_HAL_OK) {
        MediaHalFree(param);
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed, ret = %d", ret);
        return ret;
    }
    ret = uapi_audio_set_volume_matrix(port, (uapi_audio_volume_matrix *)param);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_audio_set_volume_matrix failed!");
        MediaHalFree(param);
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "PrimaryLivemicSetVolumeMatrix success!");
    MediaHalFree(param);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryLivemicGetVolumeMatrix(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "primary haid invokeAttr->value == NULL");
    uint32_t paramLen;
    uint32_t port = 0;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "getmatrix = %u#%u", &port, &paramLen);
    if (ret != SSCANF_NUM2) {
        MEDIA_HAL_LOGE(MODULE_NAME, "PrimaryLivemicGetVolumeMatrix sscanf_s failed, ret = %d", ret);
        return ret;
    }
    uapi_audio_volume_matrix param = {};
    ret = uapi_audio_get_volume_matrix(port, &param);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_audio_get_volume_matrix failed!");
        return ret;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_VOLUME_MATRIX_LEN, ":", &param);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetVolumeMatrix sprintf_s failed, ret = %d", ret);
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "PrimaryLivemicGetVolumeMatrix success!");
    return MEDIA_HAL_OK;
}

static int32_t PrimaryRecordSetEncodeParam(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    int32_t ret = AudioSDKSetEncodeDataSourceParam(invokeAttr->value, sizeof(uint32_t));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKSetEncodeDataSourceParam failed!");
        return ret;
    }
    return MEDIA_HAL_OK;
}
#endif

#ifdef MELODY_AUDIO_SUPPORT
static int32_t PrimaryDbVolumeParse(struct PluginInvokeAttr *invokeAttr, AudioVolumeContext *tempCtx,
    char *param, uint32_t paramLen)
{
    MEDIA_HAL_UNUSED(paramLen);
    if (invokeAttr == NULL || tempCtx == NULL || param == NULL || invokeAttr->value == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Invalid input parameters");
        return MEDIA_HAL_ERR;
    }
    int32_t dbVolume = 0;
    uint32_t streamType = 0;
    uint32_t channelCount = 0;
    uint32_t volumeType = 0;
    uint32_t port = 0;
    uint32_t volumeModule = 0;
    uint32_t deviceId = 0;
    int32_t ret = 0;
    if (strcmp(param, "recorddbvol") == 0) {
        ret = sscanf_s(invokeAttr->value, "recorddbvol = %d#%u#%u#%u#%u#%u#%u", &dbVolume,
            &streamType, &volumeModule, &channelCount, &volumeType, &port, &deviceId);
    } else {
        ret = sscanf_s(invokeAttr->value, "readdbvol = %d#%u#%u#%u#%u#%u#%u", &dbVolume,
            &streamType, &volumeModule, &channelCount, &volumeType, &port, &deviceId);
    }
    if (ret != SSCANF_NUM7) {
        MEDIA_HAL_LOGE(MODULE_NAME, "db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    tempCtx->audioModule = volumeModule;
    tempCtx->audioType = streamType;
    tempCtx->volume = dbVolume;
    tempCtx->audioChannel = channelCount;
    tempCtx->audioVolType = volumeType;
    tempCtx->audioPort = port;
    tempCtx->deviceId = deviceId;
    return MEDIA_HAL_OK;
}

static int32_t PrimaryRecordDbVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    AudioVolumeContext tempCtx = { 0 };
    char param[] = "recorddbvol";
    int32_t ret = PrimaryDbVolumeParse(invokeAttr, &tempCtx, param, strlen(param) + 1);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    RecordAudioDbVolume(&tempCtx);
    return MEDIA_HAL_OK;
}

static int32_t PrimaryReadDbVolume(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(priPortCtx);
    AudioVolumeContext tempCtx = { 0 };
    char param[] = "readdbvol";
    int32_t ret = PrimaryDbVolumeParse(invokeAttr, &tempCtx, param, strlen(param) + 1);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "db volume sscanf_s failed! ret = %d", ret);
        return MEDIA_HAL_ERR;
    }
    if (!ReadAudioDbVolume(&tempCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "read db volume failed!");
        return MEDIA_HAL_ERR;
    }
    ret = sprintf_s(invokeAttr->value, PRIMARY_ATTR_BUF_LEN, "%d", tempCtx.volume);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "read db volume sprintf_s failed, ret = %d", ret);
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "read db volume: %d", tempCtx.volume);
    return MEDIA_HAL_OK;
}

static int32_t PrimarySetConfigParam(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUNDBAR))
    int32_t ret = AudioSetL2HCParam(invokeAttr->value, *invokeAttr->length);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSetL2HCParam failed!");
        return MEDIA_HAL_ERR;
    }
#endif
    MEDIA_HAL_UNUSED(priPortCtx);
    return MEDIA_HAL_OK;
}

static int32_t PrimarySetGlobalMute(PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr->value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_HA))
    bool mute = true;
    int32_t muteValue = 0;
    int32_t ret = sscanf_s((const char*)invokeAttr->value, "%d", &muteValue);
    if (ret != 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "primary set globa mute sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (muteValue == 0) {
        mute = false;
    }
    ret = uapi_audio_set_global_mute(mute);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_audio_set_global_mute failed!");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi audio set global muteValue[%d] mute[%d]", muteValue, mute);
#endif
    MEDIA_HAL_UNUSED(priPortCtx);
    return MEDIA_HAL_OK;
}
#endif

#ifdef SUPPORT_SEA_AHE_PARAM
static int32_t PrimarySeaSetLibId(const PrimaryPortContext *priPortCtx, struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(priPortCtx, MEDIA_HAL_INVALID_PARAM, "Input param priPortCtx is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    char key[MAX_KEY_STR_SIZE] = {};
    int32_t value;
    int32_t ret = sscanf_s((const char *)invokeAttr->value, "%s = %d", key, MAX_KEY_STR_SIZE, &value);
    if (ret < 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (value <= 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "value invalid");
        return MEDIA_HAL_INVALID_PARAM;
    }
    SetSeaLibId(value);
    return MEDIA_HAL_OK;
}
#endif

static PrimaryInvoke g_primaryInvokeFuncList[] = {
#ifdef AUDIO_HAID_SUPPORT
    {PLUGIN_INVOKE_HAID_CREATE, (InvokeFunc)PrimaryHaidCreate},
    {PLUGIN_INVOKE_HAID_DESTROY, (InvokeFunc)PrimaryHaidDestroy},
    {PLUGIN_INVOKE_HAID_SET_ENABLE, PrimaryHaidSetEnable},
    {PLUGIN_INVOKE_HAID_GET_ENABLE, PrimaryHaidGetEnable},
    {PLUGIN_INVOKE_HAID_SET_VOLUME, PrimaryHaidSetVolume},
    {PLUGIN_INVOKE_HAID_GET_VOLUME, PrimaryHaidGetVolume},
    {PLUGIN_INVOKE_HAID_SET_HARDWARE_CONFIG, PrimaryHaidSetHardwareConfig},
    {PLUGIN_INVOKE_HAID_GET_HARDWARE_CONFIG, PrimaryHaidGetHardwareConfig},
    {PLUGIN_INVOKE_HAID_START_CALIBRATION, PrimaryHaidCalibrationStart},
    {PLUGIN_INVOKE_HAID_STOP_CALIBRATION, PrimaryHaidCalibrationStop},
    {PLUGIN_INVOKE_HAID_SET_TEST_CONFIG, PrimaryHaidSetTestConfig},
    {PLUGIN_INVOKE_HAID_SET_IOCTL, PrimaryHaidIoctl},
    {PLUGIN_INVOKE_HAID_SET_CONFIG, PrimaryHaidSetConfig},
    {PLUGIN_INVOKE_HAID_GET_CONFIG, PrimaryHaidGetConfig},
    {PLUGIN_INVOKE_HAID_SET_ALL_CONFIG, PrimaryHaidSetAllConfig},
    {PLUGIN_INVOKE_HAID_GET_ALL_CONFIG, PrimaryHaidGetAllConfig},
    {PLUGIN_INVOKE_HAID_TONE_PLAY, PrimaryTonePlayForExamine},
    {PLUGIN_INVOKE_HAID_TONE_STOP, (InvokeFunc)PrimaryToneStopForExamine},
    {PLUGIN_INVOKE_HAID_SET_SCENE, PrimaryHaidSetScene},
    {PLUGIN_INVOKE_HAID_GET_SCENE, PrimaryHaidGetScene},
    {PLUGIN_INVOKE_HAID_SET_SPK_CALI_COEF, PrimarySetSpkCaliCoef},
    {PLUGIN_INVOKE_HAID_SET_MIC_VOLUME, (InvokeFunc)PrimaryHaidSetMicVolume},
    {PLUGIN_INVOKE_HAID_GET_ENV_NOISE, (InvokeFunc)PrimaryHaidGetEnvNosie},
#endif
#ifdef MELODY_DEMO_EARPHONE
    {PLUGIN_INVOKE_TWS_SET_MODE, PrimaryTwsSetMode},
    {PLUGIN_INVOKE_TWS_SET_ROLE, PrimaryTwsSetRole},
    {PLUGIN_INVOKE_TWS_SET_CHANNEL, PrimaryTwsSetChannelMode},
    {PLUGIN_INVOKE_TWS_SET_PRODUCT_FORM, PrimaryTwsSetProductForm},
#endif
#ifdef SUPPORT_ANC
    {PLUGIN_INVOKE_ANC_INIT, PrimaryAncInit},
    {PLUGIN_INVOKE_ANC_DEINIT, PrimaryAncDeinit},
    {PLUGIN_INVOKE_ANC_CLOSE, PrimaryAncClose},
    {PLUGIN_INVOKE_ANC_OPEN, PrimaryAncOpen},
    {PLUGIN_INVOKE_ANC_GET_ATTR, PrimaryAncGetAttr},
    {PLUGIN_INVOKE_ANC_SET_PARAM, PrimaryAncSetParam},
    {PLUGIN_INVOKE_ANC_GET_PARAM, PrimaryAncGetParam},
#endif
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    {PLUGIN_INVOKE_LIVEMIC_SET_MIXMODE, PrimaryLivemicSetMixMode},
    {PLUGIN_INVOKE_LIVEMIC_GET_MIXMODE, PrimaryLivemicGetMixMode},
    {PLUGIN_INVOKE_LIVEMIC_SET_MICMODE, PrimaryLivemicSetMicMode},
    {PLUGIN_INVOKE_LIVEMIC_GET_MICMODE, PrimaryLivemicGetMicMode},
    {PLUGIN_INVOKE_SET_VOLUME_MATRIX, PrimaryLivemicSetVolumeMatrix},
    {PLUGIN_INVOKE_GET_VOLUME_MATRIX, PrimaryLivemicGetVolumeMatrix},
    {PLUGIN_INVOKE_SET_ENCODE_PARAM_EXT, PrimaryRecordSetEncodeParam},
#endif
#ifdef MELODY_AUDIO_SUPPORT
    {PLUGIN_INVOKE_RECORD_DB_VOLUME, PrimaryRecordDbVolume},
    {PLUGIN_INVOKE_READ_DB_VOLUME, PrimaryReadDbVolume},
    {PLUGIN_INVOKE_SET_CONFIG_PARAM, PrimarySetConfigParam},
    {PLUGIN_INVOKE_SET_GLOBAL_MUTE, PrimarySetGlobalMute},
#endif
#ifdef SUPPORT_SEA_AHE_PARAM
    {PLUGIN_INVOKE_SEA_SET_LIB_ID, PrimarySeaSetLibId},
#endif
};

static int32_t PrimaryPortInvoke(const AudioHandle portHandle, enum PluginInvokeId invokeId,
    struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext *priPortCtx = (PrimaryPortContext *)portHandle;
    size_t size = sizeof(g_primaryInvokeFuncList) / sizeof(PrimaryInvoke);
    InvokeFunc invokeFunc = NULL;
    for (size_t i = 0; i < size; i++) {
        if (g_primaryInvokeFuncList[i].invokeId == invokeId) {
            invokeFunc = g_primaryInvokeFuncList[i].invoke;
            break;
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found invoke function!, invokeId = %d", invokeId);
        return MEDIA_HAL_INVALID_PARAM;
    }
    return invokeFunc(priPortCtx, invokeAttr);
}

static int32_t PrimaryPortRegCallBack(const AudioHandle portHandle, PluginHaidEventCallBack callBack, void *cookie)
{
#ifdef AUDIO_HAID_SUPPORT
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    PrimaryPortContext* priPortCtx = (PrimaryPortContext*)portHandle;
    td_s32 ret = uapi_haid_register_event_proc(priPortCtx->haidHandle, PrimaryPortHaidEventCallBack, cookie);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "haid register event failed! ret = %x", ret);
        return ret;
    }
    g_haidPrimaryEventCallBack = callBack;
#else
    MEDIA_HAL_UNUSED(portHandle);
    MEDIA_HAL_UNUSED(callBack);
    MEDIA_HAL_UNUSED(cookie);
#endif
    return MEDIA_HAL_OK;
}


AudioPortPlugin g_primaryPortPlugin = {
    .audioPortDir = PORT_OUT_IN,
    .audioPortPins = PIN_IN_MIC | PIN_OUT_SPEAKER,
    .GetPortPluginCapability = PrimaryPortGetPortPluginCapability,
    .Open = PrimaryPortOpen,
    .CreateTrack = PrimaryPortCreateTrack,
    .DestroyTrack = PrimaryPortDestroyTrack,
    .Close = PrimaryPortClose,
    .Invoke = PrimaryPortInvoke,
    .RegCallBack = PrimaryPortRegCallBack,
    .SetMute = NULL,
    .GetMute = NULL,
    .SetVolume = NULL,
    .GetVolume = NULL,
};

const AudioPortPlugin *GetPrimaryPortPluginFuncs(AudioPluginType pluginType)
{
    MEDIA_HAL_UNUSED(pluginType);
    return &g_primaryPortPlugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
