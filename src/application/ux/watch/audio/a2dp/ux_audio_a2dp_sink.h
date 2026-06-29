/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#ifndef UX_AUDIO_A2DP_SINK_H
#define UX_AUDIO_A2DP_SINK_H
#include "audio_base_type.h"
#include "errcode.h"
#include "ux_errcode.h"
#include "a2dp_sink_service.h"
#include "ux_audio_a2dp.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_audio_a2dp_sink_set_param(AudioFormatConfig *codec);
errcode_t ux_audio_a2dp_sink_start(void);
errcode_t ux_audio_a2dp_sink_stop(void);
int32_t ux_audio_a2dp_sink_get_volume(void);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif