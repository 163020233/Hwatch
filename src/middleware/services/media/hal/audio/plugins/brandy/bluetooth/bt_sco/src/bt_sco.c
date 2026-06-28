/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub sco header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "bt_sco.h"
#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioBluetoothSCOSPort"
#define SSCANF_NUM2 2

int32_t CreateTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t DestroyTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = (portTrackCtx->subPortType == SUB_INPUT_PORT_BT_SCO) ? SCO_IN : SCO_OUT;
    td_u32 ret = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_detach_audio_port failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t EnableTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t DisableTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetMuteInSCOScene(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetMuteInSCOScene(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetVolumeInSCOScene(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetVolumeInSCOScene(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t PauseTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    td_u32 ret = bt_pause_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_pause_audio_stream failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t ResumeTrackInSCOScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t SendFrameInSCOScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t GetPositionInSCOScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t InvokeInSCOScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    int32_t ret;
    char key[MAX_KEY_STR_LEN] = { 0 };
    uint32_t scoShmId = 0;

    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = sscanf_s((const char *)invokeAttr->request, "%s = %u", key, MAX_KEY_STR_LEN, &scoShmId);
            if (ret != SSCANF_NUM2) {
                MEDIA_HAL_LOGI(MODULE_NAME, "sscanf_s failed");
                return ret;
            }
            MEDIA_HAL_LOGI(MODULE_NAME, "extra param value = %d", scoShmId);
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = (portTrackCtx->subPortType == SUB_INPUT_PORT_BT_SCO) ? SCO_IN : SCO_OUT;
    portParam.share_mem_id = scoShmId;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_attach_audio_port failed");
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t RegCallbackInSCOScene(AudioHandle trackHandle, TrackCallback callback,
    const AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

