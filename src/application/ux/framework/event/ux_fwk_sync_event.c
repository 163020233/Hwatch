/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux event wait process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-30， Create file. \n
 */

#include <stddef.h>
#include "ux_errcode.h"
#include "ux_fwk_msg.h"
#include "securec.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "osal_mutex.h"
#include "osal_timer.h"
#include "osal_wait.h"
#include "cmsis_os2.h"
#include "ux_api_fwk.h"
#include "ux_hal_memory.h"
#include "ux_fwk_sync_event.h"

#define UX_FWK_EVENT_WAIT_MAX 5000 // ms
#define UX_FWK_EVENT_INVLAID 0xFFFF // 无效事件

/* 数传通道同步消息 */
typedef struct {
    bool valid;
    bool init;
    uint16_t event;
    osal_mutex mutex;
    osal_wait wait;
    ux_fwk_sync_event_msg_t *msg;
} ux_fwk_sync_event_ctx_t;

ux_fwk_sync_event_ctx_t g_sync_event_ctx;
static ux_fwk_sync_event_handler g_sync_event_funcs[UX_FWK_SYNC_EVENT_MAX] = { NULL };

static void ux_fwk_sync_event_lock(void)
{
    if (g_sync_event_ctx.mutex.mutex != NULL) {
        osal_mutex_lock(&g_sync_event_ctx.mutex);
    }
}

static void ux_fwk_sync_event_unlock(void)
{
    if (g_sync_event_ctx.mutex.mutex != NULL) {
        osal_mutex_unlock(&g_sync_event_ctx.mutex);
    }
}

static errcode_t ux_fwk_sync_event_process(uint8_t event, uint8_t *data, uint16_t data_len)
{
    uint16_t device_id;
    ux_fwk_sync_event_msg_t sync_event_msg;
    if (data == NULL || data_len == 0) {
        ux_log_err("[ux][sync][event] data msg null");
        return ERRCODE_INVALID_PARAM;
    }

    if (data_len > sizeof(ux_fwk_sync_event_msg_t)) {
        ux_log_err("[ux][sync][event] data msg over max");
        return ERRCODE_INVALID_PARAM;
    }

    if (memcpy_s(&sync_event_msg, sizeof(ux_fwk_sync_event_msg_t), data, data_len) != EOK) {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_process]msg memcpy fail");
        return ERRCODE_MEMCPY;
    }

    if (event >= UX_FWK_SYNC_EVENT_START && event < UX_FWK_SYNC_EVENT_MAX) {
        device_id = sync_event_msg.device_id;
    } else {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_process] event invalid!");
        return UX_ERR_INVALID_PARAMETER;
    }
    if (g_sync_event_funcs[event] == NULL) {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_process] cd invalid!");
        return UX_ERR_INVALID_PARAMETER;
    }
    g_sync_event_funcs[event](device_id, sync_event_msg.param, sync_event_msg.param_len);
    return UX_ERR_SUCCESS;
}

/**
 * 初始化
 */
void fwk_sync_event_init(void)
{
    int32_t ret = osal_mutex_init(&g_sync_event_ctx.mutex);
    if (ret != 0) {
        g_sync_event_ctx.mutex.mutex = NULL;
        return;
    }
    if (memset_s(&g_sync_event_ctx.wait, sizeof(g_sync_event_ctx.wait), 0, sizeof(g_sync_event_ctx.wait)) != EOK) {
        ux_log_err("[ux][sync][event]memset_s failed.\r\n");
        goto MUTEX_EXIT;
    }
    ret = osal_wait_init(&g_sync_event_ctx.wait);
    if (ret != 0) {
        g_sync_event_ctx.wait.wait = NULL;
        ux_log_err("[ux][sync][event] osal_wait_init failed\n");
        goto MUTEX_EXIT;
    }
    g_sync_event_ctx.msg = (void *)ux_hal_memory_acquire(UX_MEM_CMD_EVENT, sizeof(ux_fwk_sync_event_msg_t));
    if (g_sync_event_ctx.msg == NULL) {
        ux_log_err("[ux][sync][event]ux_hal_memory_acquire failed.\r\n");
        goto WAIT_EXIT;
    }
    g_sync_event_ctx.init = true;
    g_sync_event_ctx.valid = false;
    g_sync_event_ctx.msg->ack_code = ERRCODE_SUCC;
    return;

WAIT_EXIT:
    osal_wait_destroy(&g_sync_event_ctx.wait);
    g_sync_event_ctx.wait.wait = NULL;

MUTEX_EXIT:
    osal_mutex_destroy(&g_sync_event_ctx.mutex);
    g_sync_event_ctx.mutex.mutex = NULL;
    return;
}

void ux_fwk_sync_event_init(void)
{
    fwk_sync_event_init();
    ux_api_fwk_module_register(UX_EVENT_TYPE_CMD_SYNC, ux_fwk_sync_event_process);
}

/**
 * 去初始化
 */
void ux_fwk_sync_event_deinit(void)
{
    ux_fwk_sync_event_lock();
    if (g_sync_event_ctx.msg != NULL) {
        ux_hal_memory_release(UX_MEM_CMD_EVENT, g_sync_event_ctx.msg);
        g_sync_event_ctx.msg = NULL;
    }
    g_sync_event_ctx.init = false;
    g_sync_event_ctx.valid = false;
    ux_fwk_sync_event_unlock();
    osal_wait_destroy(&g_sync_event_ctx.wait);
    g_sync_event_ctx.wait.wait = NULL;
    osal_mutex_destroy(&g_sync_event_ctx.mutex);
    g_sync_event_ctx.mutex.mutex = NULL;

    return;
}

static void ux_fwk_sync_event_init_event(uint8_t event, uint8_t sub_event)
{
    g_sync_event_ctx.msg->ack_code = ERRCODE_FAIL;
    g_sync_event_ctx.valid = false;
    g_sync_event_ctx.event = get_event(event, sub_event);

    return;
}

static uint32_t ux_fwk_event_get_current_time(void)
{
    uint64_t jiffies = osal_get_jiffies();
    return osal_jiffies_to_msecs((uint32_t)jiffies);
}

static int32_t ux_fwk_event_wait_condition(const void *param)
{
    unused(param);
    return g_sync_event_ctx.valid;
}

static bool ux_fwk_event_wait_vaild(void)
{
    return g_sync_event_ctx.valid;
}

static errcode_t ux_fwk_sync_event_get_errcode(void)
{
    return g_sync_event_ctx.msg->ack_code;
}

static errcode_t ux_fwk_sync_event_prepare_result(uint8_t *data, uint16_t data_len)
{
    if (ux_fwk_event_wait_vaild() == true) {
        errcode_t ret = ux_fwk_sync_event_get_errcode();
        if (ret != ERRCODE_SUCC) {
            ux_log_err("[ux][sync][event]ux_fwk_sync_event_get_errcode failed.ret=0x%x\r\n", ret);
            return ret;
        } else {
            if (g_sync_event_ctx.msg->param_len > data_len) {
                ux_log_err("[ux][sync][event][ux_fwk_sync_event_request] param len [%d] greater than data len[%d]\r\n",
                           g_sync_event_ctx.msg->param_len, data_len);
                return ERRCODE_FAIL;
            }
            if (memcpy_s(data, data_len, g_sync_event_ctx.msg->param, g_sync_event_ctx.msg->param_len) != EOK) {
                ux_log_err("[ux][sync][event][ux_fwk_sync_event_request] memcpy_s failed");
                return ERRCODE_FAIL;
            }
        }
    }

    return ERRCODE_SUCC;
}

static errcode_t ux_fwk_sync_event_prepare_msg(ux_fwk_sync_event_msg_t *msg, uint16_t device_id,
                                               uint8_t *data, uint16_t data_len)
{
    if (data_len > UX_FWK_SYNC_EVENT_MSG_LEN_MAX) {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_prepare_msg]data_len over max");
        return ERRCODE_FAIL;
    }
    if (memcpy_s(&msg->param, UX_FWK_SYNC_EVENT_MSG_LEN_MAX, data, data_len) != EOK) {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_prepare_msg]msg memcpy fail");
        return ERRCODE_MEMCPY;
    }
    msg->param_len = data_len;
    msg->device_id = device_id;

    return ERRCODE_SUCC;
}

bool ux_fwk_sync_event_is_ux_task(void)
{
    osThreadId_t thread_id = osThreadGetId();
    const char *task_name = osThreadGetName(thread_id);
    if (strcasecmp("ux_task", task_name) == 0) {
        return true;
    }

    return false;
}

errcode_t ux_fwk_sync_event_response(uint8_t event_cmd, uint8_t *data, uint16_t data_len, errcode_t ack_code)
{
    errcode_t ret = ERRCODE_SUCC;
    if (g_sync_event_ctx.init == false) {
        ux_log_err("[ux][sync][event] not init \r\n");
        return ERRCODE_FAIL;
    }
    uint16_t response_event = get_event(UX_EVENT_TYPE_CMD_SYNC, event_cmd);
    if (response_event != g_sync_event_ctx.event) {
        ux_log_err("[ux][sync][event] response event not request event\r\n");
        return ERRCODE_FAIL;
    }
    g_sync_event_ctx.msg->param_len = UX_FWK_SYNC_EVENT_MSG_LEN_MAX;
    if (data_len != 0 && data_len < UX_FWK_SYNC_EVENT_MSG_LEN_MAX) {
        if (memcpy_s(g_sync_event_ctx.msg->param, UX_FWK_SYNC_EVENT_MSG_LEN_MAX, data, data_len) != EOK) {
            ux_log_err("[ux][sync][event][ux_fwk_sync_event_response] memcpy_s failed\r\n");
            ret = ERRCODE_FAIL;
        }
    }

    if (data_len > UX_FWK_SYNC_EVENT_MSG_LEN_MAX) {
        ux_log_err("[ux][sync][event][ux_fwk_sync_event_response] data len over max, data_len=%d\r\n", data_len);
        ret = ERRCODE_FAIL;
    } else {
        // 0 也是合法值
        g_sync_event_ctx.msg->param_len = data_len;
    }

    if (g_sync_event_ctx.wait.wait != NULL) {
        ux_log_info("[ux][sync][event][osal_wait_wakeup] wakeup\r\n");
        osal_wait_wakeup(&g_sync_event_ctx.wait);
    }
    g_sync_event_ctx.valid = true;
    g_sync_event_ctx.msg->ack_code = ack_code;
    return ret;
}

errcode_t ux_fwk_sync_event_request(uint8_t event_cmd, uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    ux_api_comm_mail_t mail;
    uint32_t end_time;
    mail.event = UX_EVENT_TYPE_CMD_SYNC;
    mail.sub_event = event_cmd;
    ux_fwk_sync_event_msg_t sync_event_msg;

    uint32_t start_time = ux_fwk_event_get_current_time();
    if (g_sync_event_ctx.init == false) {
        ux_log_err("[ux][sync][event] not init ");
        return ERRCODE_FAIL;
    }
    
    errcode_t ret = ux_fwk_sync_event_prepare_msg(&sync_event_msg, device_id, data, data_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ux_fwk_sync_event_lock();
    ux_fwk_sync_event_init_event(UX_EVENT_TYPE_CMD_SYNC, event_cmd);
    ret = ux_api_fwk_push_mail(&mail, (uint8_t *)&sync_event_msg, sizeof(ux_fwk_sync_event_msg_t));
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][sync][event]ux_api_fwk_push_mail failed.\r\n");
        ux_fwk_sync_event_unlock();
        return ret;
    }
    // ux task 中调用，不等待，不然会死锁，无人唤醒
    if (ux_fwk_sync_event_is_ux_task() == true) {
        ux_log_info("[ux][sync][event] is ux task");
        ux_fwk_sync_event_unlock();
        return ERRCODE_SUCC;
    }
    int32_t wait_ret = osal_wait_timeout_interruptible(&g_sync_event_ctx.wait,
        (osal_wait_condition_func)ux_fwk_event_wait_condition, NULL, UX_FWK_EVENT_WAIT_MAX);
    if (wait_ret <= 0) {
        end_time = ux_fwk_event_get_current_time();
        ux_log_err("[ux][sync][event]osal_wait_timeout cost:[%d] ms.\r\n", (end_time - start_time));
        ux_fwk_sync_event_unlock();
        return ERRCODE_FAIL;
    }
    ret = ux_fwk_sync_event_prepare_result(data, data_len);
    ux_fwk_sync_event_unlock();
    end_time = ux_fwk_event_get_current_time();
    ux_log_info("[ux][sync][event]request time cost:[%d] ms.\r\n", (end_time - start_time));
    return ret;
}

errcode_t ux_fwk_sync_event_register_handler(ux_fwk_sync_event_cmd_t event, ux_fwk_sync_event_handler handler)
{
    if (event < UX_FWK_SYNC_EVENT_START || event >= UX_FWK_SYNC_EVENT_MAX) {
        ux_log_err("[ux][sync][event] ux_fwk_sync_event_register_handler event invalid!");
        return UX_ERR_INVALID_PARAMETER;
    }

    if (handler == NULL) {
        ux_log_err("[ux][sync][event] ux_fwk_sync_event_register_handler handler is NULL!");
        return UX_ERR_INVALID_PARAMETER;
    }

    g_sync_event_funcs[event] = handler;
    return UX_ERR_SUCCESS;
}