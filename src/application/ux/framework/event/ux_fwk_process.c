/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-11， Create file. \n
 */
#include <stdio.h>
#include "errcode.h"
#include "securec.h"
#include "ux_common_log.h"
#include "ux_fwk_msg.h"
#include "ux_hal_memory.h"
#include "ux_fwk_sync_process.h"
#include "ux_fwk_process.h"

static ux_event_info_t g_event_set[UX_EVENT_TYPE_BUTT];
static int16_t g_event_set_count = 0;

/**
 * @brief 添加事件
 * @param event 事件类型。
 * @return 返回事件在全局事件管理数组中对应的index。
 */
static int16_t ux_fwk_add_event(ux_event_type_t event)
{
    for (int16_t i = 0; i < g_event_set_count; i++) {
        if (g_event_set[i].event == event) {
            return i;
        }
    }
    // 若event尚未存在，则添加，并初始化
    if (g_event_set_count >= UX_EVENT_TYPE_BUTT) {
        ux_log_err("[ux][fwk] event count exceeds event type max : [%u]", g_event_set_count);
        return INVALID_INDEX;
    }
    g_event_set[g_event_set_count].event = event;
    g_event_set[g_event_set_count].activity_count = 0;
    if (memset_s(g_event_set[g_event_set_count].activities, MAX_ACT_PER_EVENT * sizeof(ux_activity_info_t),
        0, MAX_ACT_PER_EVENT * sizeof(ux_activity_info_t)) != 0) {
            ux_log_err("[ux][fwk] memset_s for activities failed");
            return INVALID_INDEX;
    }

    int16_t index = g_event_set_count;
    g_event_set_count++;
    return index;
}

/**
 * @brief 在某一事件下，添加activity处理回调函数，并做优先级排序
 * @param event_index 事件类型在数组中对应的index。
 * @param fun activity注册回调函数。
 * @param priority 事件优先级。
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因。
 */
static errcode_t ux_fwk_add_activity_to_event_prio(int16_t event_index, ux_api_module_func fun, ux_api_prio_t priority)
{
    ux_event_info_t *event_info = &g_event_set[event_index];

    for (int i = 0; i < event_info->activity_count; i++) {
        if (event_info->activities[i].fun == fun && event_info->activities[i].priority == priority) {
            ux_log_warn("[ux][fwk]duplicate event_index=%d prio=%u\n", event_index, priority);
            return ERRCODE_SUCC;
        }
    }
    if (event_info->activity_count >= MAX_ACT_PER_EVENT) {
        ux_log_err("[ux][fwk] activity count exceeds max number for this event : [%u]",
            event_info->activity_count);
        return ERRCODE_FAIL;
    }

    int i = event_info->activity_count;
    while (i > 0 && event_info->activities[i - 1].priority > priority) {
        event_info->activities[i] = event_info->activities[i - 1];
        i--;
    }
    event_info->activities[i].priority = priority;
    event_info->activities[i].fun = fun;
    event_info->activity_count++;
    return ERRCODE_SUCC;
}

errcode_t ux_api_fwk_module_register_prio(ux_event_type_t event, ux_api_module_func fun, ux_api_prio_t priority)
{
    if (event >= UX_EVENT_TYPE_BUTT) {
        ux_log_err("[ux][fwk] event type invalid : [%u]", event);
        return ERRCODE_FAIL;
    }
    if (priority >= UX_API_PRIORITY_MAX) {
        ux_log_err("[ux][fwk] priority invalid : [%u]", priority);
        return ERRCODE_FAIL;
    }

    int16_t event_index = INVALID_INDEX;
    event_index = ux_fwk_add_event(event);
    if (event_index == INVALID_INDEX) {
        return ERRCODE_FAIL;
    }

    return ux_fwk_add_activity_to_event_prio(event_index, fun, priority);
}

errcode_t ux_api_fwk_module_register(ux_event_type_t event, ux_api_module_func fun)
{
    ux_api_prio_t default_prio = UX_API_PRIO_LOW;
    return ux_api_fwk_module_register_prio(event, fun, default_prio);
}

errcode_t ux_api_fwk_process_event(uint8_t event, uint8_t sub_event, uint8_t *data, uint16_t data_len)
{
    errcode_t ret;
    for (int i = 0; i < g_event_set_count; i++) {
        if (g_event_set[i].event != event) {
            continue;
        }
        for (int j = 0; j < g_event_set[i].activity_count; j++) {
            if (g_event_set[i].activities[j].fun == NULL) {
                continue;
            }
            ret = g_event_set[i].activities[j].fun(sub_event, data, data_len);
            if (ret != ERRCODE_SUCC) {
                ux_log_err("[ux][fwk][process_event] fail, event [%d] sub event [%d] activity[%d] ret[%d]\r\n",
                    event, sub_event, j, ret);
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t ux_api_fwk_push_mail(ux_api_comm_mail_t *mail, uint8_t *data, uint16_t data_len)
{
    if (mail == NULL) {
        ux_log_err("[ux][fwk][push mail] mail is null\r\n");
        return ERRCODE_FAIL;
    }

    ux_cmd_msg_t msg;
    msg.mail.comm.event = mail->event;
    msg.mail.comm.sub_event = mail->sub_event;
    msg.mail.comm.res[0] = mail->res[0];
    msg.mail.comm.res[1] = mail->res[1];
    msg.mail.comm.app = mail->app;
    msg.mail.config_len = data_len;
    msg.mail.config_buff = NULL;
    if (data_len > 0) {
        msg.mail.config_buff = ux_hal_memory_acquire(UX_MEM_MAIL, data_len);
        if (msg.mail.config_buff == NULL) {
            ux_log_err("[ux][fwk][push mail] ux_hal_memory_acquire failed:event [%d] sub event [%d] \r\n",
                mail->event, mail->sub_event);
            return ERRCODE_FAIL;
        }

        errno_t ret = memcpy_s(msg.mail.config_buff, msg.mail.config_len, data, data_len);
        if (ret != EOK) {
            if (msg.mail.config_buff != NULL) {
                ux_hal_memory_release(UX_MEM_MAIL, msg.mail.config_buff);
                msg.mail.config_buff = NULL;
            }
            ux_log_err("[ux][fwk][push_mail] memcpy_s failed:event [%d] sub event [%d] \r\n",
                mail->event, mail->sub_event);
            return ERRCODE_FAIL;
        }
    }

    if (sys_send_msg(SYS_MODULE_APP_UX, &msg) != ERRCODE_SUCC) {
        if (msg.mail.config_buff != NULL) {
            ux_hal_memory_release(UX_MEM_MAIL, msg.mail.config_buff);
            msg.mail.config_buff = NULL;
        }
        ux_log_err("[ux][fwk][sys_send_msg] failed:event [%d] sub event [%d]\r\n", mail->event, mail->sub_event);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_api_fwk_push_event(uint8_t event, uint8_t sub_event, uint8_t *data, uint16_t data_len)
{
    ux_api_comm_mail_t mail = { 0 };
    mail.event = event;
    mail.sub_event = sub_event;
    return ux_api_fwk_push_mail(&mail, data, data_len);
}

errcode_t ux_api_fwk_notify_to_manager(ux_event_msg_t *in_event_msg, ux_event_msg_t *out_event_msg)
{
    ux_fwk_set_response_notify_task(in_event_msg->task_src);
    ux_fwk_invaild_response_event();
    ux_api_fwk_process_event(parse_event_type(in_event_msg->event), parse_event(in_event_msg->event),
                             (uint8_t *)in_event_msg, sizeof(ux_event_msg_t));
    ux_event_msg_t *event_msg = ux_fwk_get_response_event();
    out_event_msg->event = event_msg->event;
    out_event_msg->ret_code = event_msg->ret_code;

    ux_fwk_set_response_notify_task(UX_TASK_BUTT);
    return ERRCODE_SUCC;
}

void ux_api_fwk_send_msg_to_power(uint8_t event_type, uint8_t event_id, acc_type_t mode)
{
    unused(event_type);
 
    ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, event_id, (uint8_t *)&mode, sizeof(mode));
}

void ux_api_fwk_play_tone(uint8_t event_type, uint8_t event_id, uint8_t tone_id)
{
    ux_cmd_msg_t msg;
 
    ux_log_err("[ux][manager][ux_send] event type[%d] event id[%d] tone_id[%d]\r\n",
        event_type, event_id, tone_id);
 
    msg.mail.comm.event = event_type;
    msg.mail.comm.sub_event = event_id;
    msg.mail.comm.res[0] = tone_id;
    msg.mail.config_len = 0;
    msg.mail.config_buff = NULL;
 
    if (sys_send_msg(SYS_MODULE_APP_UX, &msg) != ERRCODE_SUCC) {
        ux_log_err(
            "[ux][manager][send] ux_api_fwk_play_tone failed:event type[%d] event id[%d] tone_id[%d]\r\n",
            event_type, event_id, tone_id);
    }
}
