/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: haid event handle
 * Author: AuthorNameMagicTag
 * Create: 2023-02-24
 */

#ifndef HAID_EVENT_HANDLE_H
#define HAID_EVENT_HANDLE_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_fwk_msg_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void haid_notify_fsm_event(uint16_t event, const uint8_t *param, uint16_t param_len);
void haid_fit_force_stop(void);
errcode_t haid_event_handle(ux_event_msg_t *in_event, ux_event_msg_t *out_event);
bool haid_register_fit_cb(uint8_t cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
