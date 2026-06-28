/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux event sync process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-30， Create file. \n
 */

#include <stddef.h>
#include "securec.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "ux_api_fwk.h"
#include "ux_fwk_sync_process.h"

// 定义全局变量
static ux_task_t g_src_task_notify = UX_TASK_BUTT;
static ux_event_msg_t g_response_event;
static ux_sync_msg_callback_t g_ux_task_sync_msg_list[UX_TASK_BUTT];


/**
 * 设置相应通知消息的task
 * @param response_task 响应的task
 */
void ux_fwk_set_response_notify_task(ux_task_t response_task)
{
    g_src_task_notify = response_task;
}

/**
 * 获取相应通知消息的task
 * @return 返回响应的task任务
 */
ux_task_t ux_fwk_get_response_notify_task(void)
{
    return g_src_task_notify;
}

/**
 * 获取相应通知消息的事件
 * @return 返回事件
 */
ux_event_msg_t *ux_fwk_get_response_event(void)
{
    return &g_response_event;
}

/**
 * 设置相应通知消息的事件
 * @param event_msg 事件消息
 */
void ux_fwk_set_response_event(ux_event_msg_t *event_msg)
{
    g_response_event.task_src = event_msg->task_src;
    g_response_event.event = event_msg->event;
    g_response_event.ret_code = event_msg->ret_code;
    errno_t ret = memcpy_s(g_response_event.para, UX_EVENT_MSG_PARA_LENGTH, event_msg->para, UX_EVENT_MSG_PARA_LENGTH);
    if (ret != EOK) {
        ux_log_err("[ux][manager][send] ux_fwk_set_response_event memcpy_s failed\r\n");
    }
}

/**
 * 使响应事件失效
 */
void ux_fwk_invaild_response_event(void)
{
    g_response_event.task_src = UX_TASK_BUTT;
    g_response_event.event = INVALID_EVENT;
    g_response_event.ret_code = UX_EVENT_RET_CODE_BUTT;
    (void)memset_s(g_response_event.para, UX_EVENT_MSG_PARA_LENGTH, 0, UX_EVENT_MSG_PARA_LENGTH);
}

/**
 * 同步消息处理
 * @param dest 目标
 * @param in_event_msg 输入事件消息
 * @param out_event_msg 输出事件消息
 * @return 返回错误代码
 */
errcode_t ux_fwk_sync_msg_process(ux_task_t dest, ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg)
{
    if (dest >= UX_TASK_BUTT) {
        ux_log_err("[ux_sync_msg]invalid dest: [%u]", dest);
        return ERRCODE_FAIL;
    }
    if (g_ux_task_sync_msg_list[dest] != NULL) {
        return g_ux_task_sync_msg_list[dest](in_event_msg, out_event_msg);
    }
    ux_log_warn("[ux_sync_msg]g_ux_task_sync_msg_list[%u] is null!\n", dest);
    return ERRCODE_FAIL;
}

/**
 * 获取同步消息列表
 * @return 返回同步消息列表
 */
ux_sync_msg_callback_t *ux_fwk_get_sync_msg_list(void)
{
    return g_ux_task_sync_msg_list;
}

/**
 * 注册同步消息处理器
 * @param task_type 任务类型
 * @param cb 回调函数
 * @return 返回错误代码
 */
errcode_t ux_fwk_register_sync_msg_handler(ux_task_t task_type, ux_sync_msg_callback_t cb)
{
    if (task_type >= UX_TASK_BUTT) {
        ux_log_err("[ux][event][ux_fwk_register_sync_msg_handler] invalid task_type：[%d]", task_type);
        return ERRCODE_FAIL;
    }

    ux_sync_msg_callback_t *send_list = ux_fwk_get_sync_msg_list();
    send_list[task_type] = cb;
    return ERRCODE_SUCC;
}

/**
 * 推送同步事件
 * @param dest 目标
 * @param event_type 事件类型
 * @param event_id 事件ID
 * @param ret_code 返回代码
 * @return 返回错误代码
 */
errcode_t ux_fwk_push_sync_event(ux_task_t dest, uint8_t event_type, uint8_t event_id, uint8_t ret_code)
{
    ux_event_msg_t in_msg = {0};
    in_msg.event = get_event(event_type, event_id);
    in_msg.ret_code = ret_code;

    ux_task_t src_task = ux_fwk_get_response_notify_task();
    if (src_task == dest) {
        ux_fwk_set_response_event(&in_msg);
        ux_log_warn("[ux][fwk][push_sync_event]src:[%d] equal dest:[%d]", src_task, dest);
        return ERRCODE_SUCC;
    }

    ux_event_msg_t out_event;
    out_event.event = INVALID_EVENT;
    ux_fwk_sync_msg_process(dest, &in_msg, &out_event);
    if (out_event.event != INVALID_EVENT) {
        ux_api_fwk_process_event(parse_event_type(out_event.event), parse_event(out_event.event),
                                 &out_event.ret_code, sizeof(uint16_t));
    }
    return ERRCODE_SUCC;
}

/**
 * 推送同步消息
 * @param msg 消息
 */
void ux_fwk_push_sync_msg(ux_event_msg_t *msg)
{
    if (msg == NULL) {
        ux_log_err("[ux][manager][send] ux_fwk_push_sync_msg send failed:msg is null\r\n");
        return;
    }

    ux_task_t src_task = ux_fwk_get_response_notify_task();
    if (src_task == msg->task_src) {
        ux_log_warn("[ux][manager][send] task equal.\n");
        ux_fwk_set_response_event(msg);
        return;
    }

    ux_event_msg_t out_event;
    out_event.event = INVALID_EVENT;
    ux_fwk_sync_msg_process(msg->task_src, msg, &out_event);
    if (out_event.event != INVALID_EVENT) {
        ux_log_warn("[ux][manager][send] ux_api_fwk_process_event.\n");
        ux_api_fwk_process_event(parse_event_type(out_event.event), parse_event(out_event.event),
                                 &out_event.ret_code, sizeof(uint16_t));
    }
}

