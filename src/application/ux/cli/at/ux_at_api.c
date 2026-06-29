/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#include <string.h>
#include <stdlib.h>
#include "stdint.h"
#include "at.h"
#include "ux_at_manager.h"
#include "ux_at_recovery.h"
#include "ux_fwk_msg_def.h"
#include "ux_api_fwk.h"

void send_msg_to_ux(uint8_t event_type, uint8_t event_id)
{
    ux_event_msg_t event_msg;
    event_msg.task_src = UX_AT;
    ux_api_fwk_push_event(event_type, event_id, (uint8_t *)&event_msg, sizeof(ux_event_msg_t));
}

at_ret_t at_event_send_ux_msg(const cmd_event_t *cmds, size_t cmd_cnt, const char *para, uint8_t event_type)
{
    if (para == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para, cmds[i].cmd) == 0) {
            send_msg_to_ux(event_type, cmds[i].event_id);
            return AT_RET_OK;
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_cmd_proc_ux_cmd(const ux_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2)
{
    if (para1 == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para1, cmds[i].cmd) == 0) {
            return cmds[i].cmd_handle(para2);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_cmd_proc_audio_cmd(const audio_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
                               const char *para3)
{
    if (para1 == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para1, cmds[i].audio_cmd) == 0) {
            return cmds[i].cmd_handle(para2, para3);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_cmd_proc_ux_query(const cmd_query_t *cmds, size_t cmd_cnt, const char *para)
{
    if (para == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para, cmds[i].cmd) == 0) {
            return cmds[i].query_handle();
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_cmd_proc_audio_query(const audio_query_t *cmds, size_t cmd_cnt, const char *para1, const char *para2)
{
    if (para1 == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para1, cmds[i].audio_cmd) == 0) {
            return cmds[i].query_handle(para2);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_cmd_proc_led_cmd(const led_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
                             const char *para3)
{
    if (para1 == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para1, cmds[i].led_cmd) == 0) {
            return cmds[i].cmd_handle(para2, para3);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

/* 恢厂命令 */
at_ret_t at_cmd_proc_recovery_cmd(const recovery_cmd_t *cmds, size_t cmd_cnt, const char *para1, const char *para2,
                                  const char *para3, const char *para4)
{
    if (para1 == NULL || cmds == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_cnt; i++) {
        if (strcasecmp(para1, cmds[i].recovery_cmd) == 0) {
            return cmds[i].cmd_handle(para2, para3, para4);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}
