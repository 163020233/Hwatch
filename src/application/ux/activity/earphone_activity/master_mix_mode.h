/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux master mix mode \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-11， Create file. \n
 */
#ifndef MASTER_MIX_MODE_H
#define MASTER_MIX_MODE_H
#include "stdint.h"
#include "errcode.h"
#include "ux_system_manager.h"
#include "haid_media.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*mix_mode_execute_t)(void);

typedef struct {
    void (*play_music)(void);
    void (*pause_music)(void);
    void (*start_call)(void);
    void (*sound_to_haid)(void);
    void (*start_haid)(haid_work_mode_t work_mode);
    void (*stop_fitting)(void);
    void (*call_phone_active)(void);
    void (*call_earphone_active)(void);
    void (*wear_in_active)(void);
    void (*enter_medical)(void);
    void (*exit_medical)(void);
    void (*init_process)(void);
    void (*sound_mute_haid_ctrl)(haid_work_mode_t work_mode);
    void (*sound_unmute_haid_ctrl)(void);
} ux_mode_funcs_t;

ux_mode_funcs_t *get_mix_mode_funcs(audio_aid_mix_state_t mode);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif