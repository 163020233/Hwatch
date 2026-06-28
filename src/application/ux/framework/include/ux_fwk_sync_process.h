/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event sync process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-11， Create file. \n
 */
#ifndef UX_FWK_SYNC_PROCESS_H
#define UX_FWK_SYNC_PROCESS_H
#include "ux_fwk_msg_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef errcode_t (*ux_sync_msg_callback_t)(ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg);

void ux_fwk_set_response_notify_task(ux_task_t response_task);
ux_task_t ux_fwk_get_response_notify_task(void);
void ux_fwk_set_response_event(ux_event_msg_t *event_msg);
void ux_fwk_invaild_response_event(void);
ux_event_msg_t *ux_fwk_get_response_event(void);
errcode_t ux_fwk_register_sync_msg_handler(ux_task_t task_type, ux_sync_msg_callback_t cb);
errcode_t ux_fwk_sync_msg_process(ux_task_t dest, ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg);
ux_sync_msg_callback_t *ux_fwk_get_sync_msg_list(void);
void ux_fwk_push_sync_msg(ux_event_msg_t *msg);
errcode_t ux_fwk_push_sync_event(ux_task_t dest, uint8_t event_type, uint8_t event_id, uint8_t ret_code);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif