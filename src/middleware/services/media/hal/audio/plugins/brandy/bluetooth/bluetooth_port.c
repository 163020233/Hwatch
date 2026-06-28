/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: bluetooth port
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "bluetooth_port.h"

#include "bluetooth_sub_port.h"

#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioBluetoothPort"

typedef struct {
    /* port context */
    bool isOpened;
    /* sub port context */
    AudioHandle subPortHandle;
    /* sub port track context */
    AudioHandle subPortTrackHandle;
} BluetoothPortContext;

static int32_t BluetoothPortGetPortPluginCapability(const struct AudioPort *port,
    struct AudioPortCapability *capability)
{
    CHK_NULL_RETURN(port, MEDIA_HAL_INVALID_PARAM, "Input param port is null");
    CHK_NULL_RETURN(capability, MEDIA_HAL_INVALID_PARAM, "Input param capability is null");

    int32_t ret = AudioBluetoothSubPortGetCapability(port, capability);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioBluetoothSubPortGetCapability failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "get capability success");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothPortOpen(AudioHandle *portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");

    int32_t ret;
    BluetoothPortContext *btPortCtx = (BluetoothPortContext *)MediaHalMalloc(sizeof(BluetoothPortContext));
    if (btPortCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    if (memset_s(btPortCtx, sizeof(BluetoothPortContext), 0, sizeof(BluetoothPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }
    ret = AudioBluetoothSubPortCreate(&btPortCtx->subPortHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioBluetoothSubPortCreate failed(0x%x)", ret);
        goto FREE;
    }

    btPortCtx->isOpened = true;
    *portHandle = btPortCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    MediaHalFree(btPortCtx);
    return ret;
}

static int32_t BluetoothPortCreateTrack(AudioHandle portHandle, enum AudioPortPin audioPortPin,
    struct PortPluginAttr *portAttr, AudioHandle *trackHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_INVALID_PARAM, "Input param trackHandle is null");
    CHK_NULL_RETURN(portAttr, MEDIA_HAL_INVALID_PARAM, "Input param portAttr is null");

    int32_t ret = 0;
#ifdef AUDIO_SDK_SUPPORT
    ret = AudioSDKInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKInit failed(0x%x)", ret);
        return MEDIA_HAL_ERR;
    }
#endif

    if (!AudioPluginIsValidPin(audioPortPin, "bluetooth")) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pin 0x%x is invalid", audioPortPin);
        return MEDIA_HAL_INVALID_PARAM;
    }

    BluetoothPortContext *btPortCtx = (BluetoothPortContext *)portHandle;
    CHK_FAILED_RETURN(btPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    ret = AudioBluetoothSubPortCreateTrack(btPortCtx->subPortHandle, portAttr, audioPortPin,
        &btPortCtx->subPortTrackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioBluetoothSubPortCreateTrack failed");

    *trackHandle = btPortCtx->subPortTrackHandle;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    BluetoothPortContext *btPortCtx = (BluetoothPortContext *)portHandle;
    CHK_FAILED_RETURN(btPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");
    int32_t ret = AudioBluetoothSubPortDestroyTrack(btPortCtx->subPortHandle, trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioBluetoothSubPortDestroyTrack failed");
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothPortClose(AudioHandle portHandle)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    BluetoothPortContext *btPortCtx = (BluetoothPortContext *)portHandle;
    CHK_FAILED_RETURN(btPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");

    int32_t ret;

#ifdef AUDIO_SDK_SUPPORT
    ret = AudioSDKDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKDeInit failed");
#endif

    ret = AudioBluetoothSubPortDestroy(btPortCtx->subPortHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioBluetoothSubPortDestroy failed");

    btPortCtx->isOpened = false;
    MediaHalFree(btPortCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static enum InvokeID ConvertInvokeID(enum PluginInvokeId invokeId)
{
    switch (invokeId) {
        case PLUGIN_INVOKE_SET_A2DP_CODEC_ATTR:
            return INVOKE_ID_SET_A2DP_CODEC_ATTR;
        default:
            return INVOKE_ID_BUTT;
    }
}

static int32_t BluetoothPortInvoke(const AudioHandle portHandle, enum PluginInvokeId invokeId,
    struct PluginInvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param portHandle is null");
    BluetoothPortContext *priPortCtx = (BluetoothPortContext *)portHandle;
    CHK_FAILED_RETURN(priPortCtx->isOpened, true, MEDIA_HAL_ERR, "port not open");
    struct InvokeAttr attr = {};
    attr.request = invokeAttr->value;
    enum InvokeID id =  ConvertInvokeID(invokeId);
    return AudioBluetoothSubPortInvoke(priPortCtx->subPortHandle, id, &attr);
}

AudioPortPlugin g_bluetoothPortPlugin = {
    .audioPortDir = PORT_OUT_IN,
    .audioPortPins = PIN_IN_BLUETOOTH_SCO | PIN_OUT_BLUETOOTH_SCO |
                     PIN_OUT_BLUETOOTH_A2DP | PIN_IN_BLUETOOTH_A2DP |
                     PIN_IN_NEARLINK_DONGLE | PIN_OUT_NEARLINK_DONGLE, // mask of PIN
    .GetPortPluginCapability = BluetoothPortGetPortPluginCapability,
    .Open = BluetoothPortOpen,
    .CreateTrack = BluetoothPortCreateTrack,
    .DestroyTrack = BluetoothPortDestroyTrack,
    .Close = BluetoothPortClose,
    .Invoke = BluetoothPortInvoke,
    .RegCallBack = NULL,
    .SetMute = NULL,
    .GetMute = NULL,
    .SetVolume = NULL,
    .GetVolume = NULL,
};

const AudioPortPlugin *GetBluetoothPortPluginFuncs(AudioPluginType pluginType)
{
    MEDIA_HAL_UNUSED(pluginType);
    return &g_bluetoothPortPlugin;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

