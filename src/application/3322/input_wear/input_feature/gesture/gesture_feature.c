/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: gesture application source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-31, Create file. \n
 */

#include "stdlib.h"
#include "securec.h"
#include "errcode.h"
#include "td_type.h"
#include "cmsis_os2.h"
#include "osal_msgqueue.h"
#include "soc_osal.h"
#include "debug_print.h"
#include "input_manager.h"
#include "power_display_service.h"
#include "time.h"
#include "sys/time.h"
#include "debug_print.h"
#include "gesture_feature.h"

report_event_cb g_gesture_event_cb = NULL;
#define GESTURE_BOARD_TASK_PRIO 5
#define GESTURE_BOARD_TASK_SIZE 0x800
#define GESTURE_BOARD_MSG_COUNT 16
#define GESTURE_MAIL_LENGTH 8
#define GESTURE_TIME_SCREEN_OFF 20000
#define GESTURE_ACTIVE_TIME 8
#define ACTTIVE_FLAG 1
#define VALID_FLAG 1
#define MAX_GESTURE 4
#define GESTURE_VALID_TIME 15
#define GESTURE_VALID_TIMES 1

uint32_t g_last_pred_time = 0;       // 本次激活最後一個手勢發生時間
uint32_t g_last_activation_flag;     // 上一次激活狀態
uint32_t g_current_activation_times; // 本次激活時間
uint32_t g_current_time_pred_count;  // 只記錄本次激活的前5秒的手勢個數，用於防虛景

uint32_t gesture_init(void)
{
    input_register_service_cb(APP_GESTURE, gesture_service_callback);
    return ERRCODE_SUCC;
}

uint32_t gesture_close(void)
{
    return ERRCODE_SUCC;
}

uint32_t gesture_register_report_event_cb(report_event_cb callback)
{
    PRINT("gesture_register_report_event_cb\r\n");
    g_gesture_event_cb = callback;
    return ERRCODE_SUCC;
}

uint32_t gesture_unregister_report_event_cb(void)
{
    g_gesture_event_cb = NULL;
    return ERRCODE_SUCC;
}

bool has_valid_pred(uint16_t valid_flag, uint16_t activation_flag, int32_t pred)
{
    if (activation_flag == ACTTIVE_FLAG && valid_flag == VALID_FLAG && pred < MAX_GESTURE) {
        return true;
    } else {
        return false;
    }
}

uint32_t get_current_timestamp(void) // 0时区timestamp
{
    uint32_t timestamp = 0;
    struct timeval current_time = { 0 };
    if (gettimeofday(&current_time, NULL)) {
        return 0;
    }
    struct tm tm;
    localtime_r(&current_time.tv_sec, &tm);
    timestamp = (uint32_t)current_time.tv_sec;
    return timestamp;
}

void gesture_report_event_to_gui(uint16_t valid_flag, uint16_t activation_flag, uint32_t pred)
{
    if (g_gesture_event_cb != NULL) {
        bool is_valid_pred = has_valid_pred(valid_flag, activation_flag, pred);
        if (is_valid_pred) {
            input_dev_data_t send_data;
            send_data.pred = pred;
            send_data.validFlag = valid_flag;
            send_data.activationFlag = activation_flag;
            send_data.type = INDEV_TYPE_GESTURE;
            g_gesture_event_cb(&send_data);
        }
    }
}

// 手势事件发送给input线程处理的外部接口
void gesture_send_msg_to_app(uint16_t valid_flag, uint16_t activation_flag, int32_t pred)
{
    gesture_input_result result;
    result.valid_flag = valid_flag;
    result.activation_flag = activation_flag;
    result.pred = pred;
    common_notify_mail_t mail = { 0 };
    mail.comm.app = APP_GESTURE;
    memcpy_s(mail.data, GESTURE_MAIL_LENGTH, &result, GESTURE_MAIL_LENGTH);
    int32_t ret = osal_msg_queue_write_copy(get_app_queue_id(), (void *)&mail, sizeof(common_notify_mail_t), 0);
    if (ret != OSAL_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_GESTURE, "gesture app msg send failed! err=0x%x! \r\n", ret);
    }
}

// input线程的回调线程回调处理
void gesture_service_callback(common_notify_mail_t *notify_msg)
{
    gesture_input_result *result;
    result = malloc(sizeof(gesture_input_result));
    if (result == NULL) {
        return;
    }
    memcpy_s(result, GESTURE_MAIL_LENGTH, notify_msg->data, GESTURE_MAIL_LENGTH);
    gesture_report_event_to_gui(result->valid_flag, result->activation_flag, result->pred);
    free(result);
}
