/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */
#ifndef UX_AT_AUDIO_H
#define UX_AT_AUDIO_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef errcode_t (*at_ux_audio_recorder_process_t)(uint16_t device_id, const char *param);

typedef struct {
    const char *cmd;
    at_ux_audio_recorder_process_t proc;
} ux_at_audio_recorder_map_t;

typedef errcode_t (*at_ux_audio_process_t)(const char *param1, const char *param2, const char *param3);

typedef struct {
    const char *cmd;
    at_ux_audio_process_t proc;
} ux_at_audio_opts_map_t;

errcode_t ux_at_audio_set_process(const ux_at_para4_args_t *args);
errcode_t ux_at_audio_query_process(const ux_at_para4_args_t *args);

at_ret_t at_mute_set_process(const ux_at_para2_args_t *args);
at_ret_t at_mute_query_process(const ux_at_para1_args_t *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
