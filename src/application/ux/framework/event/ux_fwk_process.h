/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-11， Create file. \n
 */
#ifndef UX_FWK_PROCESS_H
#define UX_FWK_PROCESS_H
#include "ux_event_define.h"
#include "ux_fwk_msg_def.h"
#include "ux_api_fwk.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_ACT_PER_EVENT 4
#define INVALID_INDEX (-1)

/**
 * @brief activity info
 */
typedef struct ux_activity_info_t {
    ux_api_prio_t priority;
    ux_api_module_func fun;
} ux_activity_info_t;

/**
 * @brief event info
 */
typedef struct ux_event_info_t {
    uint8_t activity_count;
    ux_event_type_t event;
    ux_activity_info_t activities[MAX_ACT_PER_EVENT];
} ux_event_info_t;

errcode_t ux_api_fwk_notify_to_manager(ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg);
void ux_api_fwk_send_msg_to_power(uint8_t event_type, uint8_t event_id, acc_type_t mode);
void ux_api_fwk_play_tone(uint8_t event_type, uint8_t event_id, uint8_t tone_id);
void send_notify_to_peer(uint8_t event_type, uint8_t event_id, uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif