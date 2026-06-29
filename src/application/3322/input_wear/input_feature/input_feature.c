/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: input app source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08, Create file. \n
 */
#include "input_feature.h"
#include "osal_task.h"
#include "soc_osal.h"
#include "soc_diag_util.h"
#include "cmsis_os2.h"
#include "debug_print.h"
#include "pm_definition.h"
#include "tcxo.h"
#include "osal_msgqueue.h"

service_callback_func g_input_app_service_cb[APP_INPUT_MAX] = { 0 };
static unsigned long g_app_queue = 0xFFFFFFFF;

uint32_t input_register_service_cb(app_type_t type, service_callback_func callback)
{
    if (callback == NULL) {
        return ERRCODE_FAIL;
    }
    g_input_app_service_cb[type] = callback;
    return ERRCODE_SUCC;
}

static void input_device_task(void const * arg)
{
    unused(arg);
    int32_t ret;
    common_notify_mail_t mail;
    app_type_t input_type;
    uint32_t msgsize = (uint32_t)sizeof(common_notify_mail_t);
    while (1) {
        ret = osal_msg_queue_read_copy(g_app_queue, (void *)&mail, &msgsize, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            continue;
        }
        input_type = mail.comm.app;
        if (input_type >= APP_INPUT_MAX) {
            continue;
        }
        if (g_input_app_service_cb[input_type] != NULL) {
            g_input_app_service_cb[input_type](&mail);
        }
    }
}

uint32_t input_app_init(void)
{
    if (osal_msg_queue_create("app_queue", APP_BOARD_MSG_COUNT, &g_app_queue, 0, sizeof(common_notify_mail_t)) !=
        ERRCODE_SUCC) {
        PRINT("[input app] create mail fail\n");
        return ERRCODE_FAIL;
    }

    osal_task *task_handle =
        osal_kthread_create((osal_kthread_handler)input_device_task, NULL, "input_app", APP_BOARD_TASK_SIZE);
    if (task_handle == NULL) {
        osal_msg_queue_is_full(g_app_queue);
        PRINT("create button app thread fail!\r\n");
        return ERRCODE_FAIL;
    }

    osal_kthread_set_priority(task_handle, APP_BOARD_TASK_PRIO);

    return ERRCODE_SUCC;
}

uint64_t get_app_queue_id(void)
{
    return g_app_queue;
}

uint32_t input_set_workmode(app_type_t type, input_dev_workmode_t work_mode, call_method_t method)
{
    unused(method);
    uint32_t ret;
    INPUT_FEATURE_PRINT_DEBUG(0, "type:%u, setmode=0x%x! \r\n", type, work_mode);
    common_notify_mail_t mail = { 0 };
    mail.comm.app = type;
    mail.comm.event = work_mode;
    mail.comm.set_mode = 1;
    ret = (uint32_t)osal_msg_queue_write_copy(get_app_queue_id(), (void *)&mail, sizeof(common_notify_mail_t), 0);
    if (ret != OSAL_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch app msg send failed! err=0x%x! \r\n", ret);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
