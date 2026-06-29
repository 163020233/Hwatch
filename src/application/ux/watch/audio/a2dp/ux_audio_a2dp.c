/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */

#include "ux_audio_a2dp.h"
#include "ux_common_log.h"
errcode_t ux_audio_notify_a2dp_profile_state(AudioDeviceType deviceType, AudioDeviceConnectState connectState)
{
    ux_log_info("[ux_audio_notify_a2dp_profile_state] Notify media, type:%d, connect_state:%d.\n",
        deviceType, connectState);
    int err = AudioManagerSetDeviceConnectionState(deviceType, connectState);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_notify_a2dp_profile_state] Fail to notify audio a2dp connect state 0x%x.\n", err);
        return UX_ERR_NOTIFY_FAIL;
    }
    return UX_ERR_SUCCESS;
}