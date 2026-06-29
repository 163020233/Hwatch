/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub a2dp uplink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "bt_a2dp_uplink.h"
#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioBluetoothA2dpUplinkPort"
#define SSCANF_NUM2 2

int32_t CreateTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "CreateTrackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t DestroyTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portTrackCtx->a2dpSinkMemId;
    int32_t ret = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_detach_audio_port failed");
#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "Destroy Track success");
    return MEDIA_HAL_OK;
}

int32_t EnableTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "EnableTrackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t DisableTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "DisableTrackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetMuteInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackSetMuteInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetMuteInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, bool* mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackGetMuteInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetVolumeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f", volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "TrackSetVolumeInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetVolumeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, float* volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackGetVolumeInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t PauseTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef BT_HAL_SUPPORT
    MEDIA_HAL_LOGI(MODULE_NAME, "success, cur id[%d] bt devState[%d], bt pause is unnecessary",
        portTrackCtx->trackId, portTrackCtx->subPortCtx->btDevState);
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    return MEDIA_HAL_OK;
}

int32_t ResumeTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t SendFrameInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "SendFrameInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t GetPositionInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "GetPositionInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

int32_t InvokeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    char key[MAX_KEY_STR_LEN] = { 0 };
    uint32_t scoShmId = 0;
    int32_t ret;
    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = sscanf_s((const char*)invokeAttr->request, "%s = %u", key, MAX_KEY_STR_LEN, &scoShmId);
            if (ret != SSCANF_NUM2) {
                MEDIA_HAL_LOGI(MODULE_NAME, "sscanf_s failed");
                return ret;
            }
            MEDIA_HAL_LOGI(MODULE_NAME, "extra param key = %s, value = 0x%x", key, scoShmId);
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    portTrackCtx->a2dpSinkMemId = scoShmId;
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = scoShmId;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_attach_audio_port failed");
#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t RegCallbackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle,
    TrackCallback callback, const AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "RegCallbackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

