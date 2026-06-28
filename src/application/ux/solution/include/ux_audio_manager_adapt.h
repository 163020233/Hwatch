/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provides watchdog driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-29, Create file. \n
 */
#ifndef UX_AUDIO_MANAGER_ADAPT_H
#define UX_AUDIO_MANAGER_ADAPT_H

#include "stdint.h"
#include "ux_fwk_msg_def.h"
#include "ux_bs_generic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uint16_t in_event;
    uint8_t audio_event;
} ux_audio_map_t;

errcode_t ux_audio_handle_manager_event(ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg);
void ux_audio_service_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif