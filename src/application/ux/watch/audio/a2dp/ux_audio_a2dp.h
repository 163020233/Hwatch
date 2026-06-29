/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#ifndef UX_AUDIO_A2DP_H
#define UX_AUDIO_A2DP_H
#include "errcode.h"
#include "ux_errcode.h"
#include "audio_base_type.h"
#include "audio_manager_service.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_audio_notify_a2dp_profile_state(AudioDeviceType deviceType, AudioDeviceConnectState connectState);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif