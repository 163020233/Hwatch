/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#ifndef UX_AT_API_H
#define UX_AT_API_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#include "ux_at_recovery.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void send_msg_to_ux(uint8_t event_type, uint8_t event_id);
at_ret_t at_event_send_ux_msg(const cmd_event_t *cmds, size_t cmd_cnt, const char *para, uint8_t event_type);
at_ret_t at_cmd_proc_ux_cmd(const ux_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2);
at_ret_t at_cmd_proc_audio_cmd(const audio_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
    const char *para3);
at_ret_t at_cmd_proc_ux_query(const cmd_query_t *cmds, size_t cmd_cnt, const char *para);
at_ret_t at_cmd_proc_audio_query(const audio_query_t *cmds, size_t cmd_cnt, const char *para1, const char *para2);
at_ret_t at_cmd_proc_led_cmd(const led_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
    const char *para3);
at_ret_t at_cmd_proc_recovery_cmd(const recovery_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
    const char *para3, const char *para4);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
