/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux task init src code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-03-15, Create file. \n
 */
#include "osal_msgqueue.h"
#include "securec.h"
#include "common_def.h"
#include "ux_common_log.h"
#include "ux_hal_memory.h"
#include "ux_fwk_process.h"
#include "ux_fwk_msg.h"

#define SYS_MODULE_APP_UX     8

unsigned long g_ux_msg_queue = 0;

void ux_msg_queue_init(void)
{
    int32_t ret = osal_msg_queue_create("ux_msg", UX_MAX_MSG_SIZE, &g_ux_msg_queue, 0, sizeof(ux_cmd_msg_t));
    if (ret != 0) {
        ux_log_err("[ux][ux_msg]ux msg queue create fail\n");
    }
}

/* 消息发送 */
errcode_t sys_send_msg(uint8_t msg_queue, const void *msg_ptr)
{
    /* 为兼容旧代码, 发动到ux_task消息队列的第一个入参需要是SYS_MODULE_APP_UX */
    if ((msg_queue != SYS_MODULE_APP_UX) || (msg_ptr == NULL)) {
        ux_log_err("[ux][ux_msg]send msg null or not ux queue\n");
        return ERRCODE_FAIL;
    }

    if (g_ux_msg_queue == 0) {
        ux_log_err("[ux][ux_msg]ux msg queue empty\n");
        return ERRCODE_FAIL;
    }

    int32_t ret = osal_msg_queue_write_copy(g_ux_msg_queue, (void *)msg_ptr, sizeof(ux_cmd_msg_t), 0);
    if (ret != 0) {
        ux_log_err("[ux][ux_msg]send msg fail, ret %d\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/* 消息接收 */
errcode_t sys_get_msg(uint8_t msg_queue, void *msg_ptr, uint32_t timeout)
{
    /* 为兼容旧代码, 从ux_task消息队列取消息的第一个入参需要是SYS_MODULE_APP_UX */
    if ((msg_queue != SYS_MODULE_APP_UX) || (msg_ptr == NULL)) {
        ux_log_err("[ux][ux_msg]get msg null or not ux queue\n");
        return ERRCODE_FAIL;
    }

    if (g_ux_msg_queue == 0) {
        ux_log_err("[ux][ux_msg]ux msg queue empty\n");
        return ERRCODE_FAIL;
    }

    uint32_t msg_size = sizeof(ux_cmd_msg_t);
    int32_t ret = osal_msg_queue_read_copy(g_ux_msg_queue, msg_ptr, &msg_size, timeout);
    if (ret != 0 || msg_size != sizeof(ux_cmd_msg_t)) {
        ux_log_err("[ux][ux_msg]get msg fail, ret %d\n", ret);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static errcode_t ux_event_handle(ux_cmd_msg_t *msg)
{
    ux_log_info("[ux_msg]event[%d] sub_event[%d]", msg->mail.comm.event, msg->mail.comm.sub_event);
    return ux_api_fwk_process_event(msg->mail.comm.event, msg->mail.comm.sub_event,
                                    (uint8_t*)msg->mail.config_buff, msg->mail.config_len);
}

/* 消息分发 */
errcode_t ux_msg_handle(ux_cmd_msg_t *msg)
{
    errcode_t ret = ERRCODE_SUCC;

    if (msg->mail.comm.event < UX_EVENT_TYPE_BUTT) {
        ret = ux_event_handle(msg);
    }

    if (msg->mail.config_buff != NULL && msg->mail.config_len > 0) {
        ux_hal_memory_release(UX_MEM_MAIL, msg->mail.config_buff);
        msg->mail.config_buff = NULL;
    }

    return ret;
}
