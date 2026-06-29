/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: TIMER sample implementation \n
 *
 */
#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "timer_demo.h"

#define TIMER_MS_2_US 1000  // 时间装换系数1ms=1000us
#define TIMER_NUM 2  // 创建定时器的个数

typedef struct timer_info {
    uint32_t start_time;  // 定时开始时间
    uint32_t end_time;    // 定时结束时间
    uint32_t delay_time;  // 定时延时时间
} timer_info_t;           // 定时器时间管理结构体

static uint32_t g_timer_int_count = 0;  // 定时器中断计数
static timer_info_t g_timers_info[2] = {
    {0, 0, 1000},  // 1000us
    {0, 0, 2000}   // 2000us
};                 // 定时器时间管理数组

static void at_write_func(const char *data)
{
    uapi_uart_write(TEST_SUITE_UART_BUS, (const uint8_t *)data, strlen(data), 0);
}

static void at_adapter_sample_print(const char *fmt, ...)
{
    static char log[AT_LOG_MAX_LEN] = {0};
    int32_t str_len;
    int32_t uart_str_len = 0;
    if (fmt == NULL) {
        return;
    }
    va_list args;
    va_start(args, fmt);
    str_len = vsprintf_s(log, AT_LOG_MAX_LEN, fmt, args);
    va_end(args);
    printf("\r\n");

    if (str_len < 0) {
        return;
    }

    /* 添加\n */
    if (str_len + 1 >= AT_LOG_MAX_LEN) {
        return;
    }

    va_start(args, fmt);
    uart_str_len = vsnprintf_s(log, str_len + 1, str_len + 1, fmt, args);
    va_end(args);
    
    if (uart_str_len < 0) {
        return;
    }

    log[str_len] = '\n';
    log[str_len + 1] = '\0';

    at_write_func(log);
}

void at_diting_timer_example_cmd_register(void)
{
    uapi_at_cmd_table_register(g_at_timer_cmd_parse_table, AT_TIMER_FUNC_NUM, EXT_AT_TIMER_CMD_MAX_LEN);
}

static void timer_timeout_callback(uintptr_t data)
{
    at_adapter_sample_print("timer_timeout_callback start");
    // step1 定义timer_index接收定时器的索引值
    uint32_t timer_index = (uint32_t)data;
    // step2 获取定时器结束的实时时间
    g_timers_info[timer_index].end_time = uapi_tcxo_get_ms();
    // step3 计数器+1
    g_timer_int_count++;
}

static errcode_t timer_run_sample(const timer_task_args_t *args)
{
    at_adapter_sample_print("timer_task start");
    // step1：创建定时器索引
    timer_handle_t timer_index[2] = {0};
    // step2：初始化定时器
    errcode_t ret = uapi_timer_init();
    if (ret != ERRCODE_SUCC) {
        at_adapter_print("init timer err!");
        uapi_timer_deinit();
        return ERRCODE_INVALID_PARAM;
    }
    // step3：注册定时器中断
    uapi_timer_adapter(args->timer_index_no, TIMER_1_IRQN, args->timer_pority);
    at_adapter_sample_print("timer1_task start");
    // step4：创建定时器1
    errcode_t ret1 = uapi_timer_create(args->timer_index_no, &timer_index[0]);
    if (ret1 == ERRCODE_INVALID_PARAM) {
        at_adapter_print("create timer1 err!");
        uapi_timer_deinit();
        return ERRCODE_INVALID_PARAM;
    }
    // step5：获取定时器1的开始时间
    g_timers_info[0].start_time = uapi_tcxo_get_ms();
    // step6：启动定时器1
    errcode_t ret2 = uapi_timer_start(timer_index[0], g_timers_info[0].delay_time, timer_timeout_callback, 0);
    if (ret2 == ERRCODE_INVALID_PARAM) {
        at_adapter_print("start timer1 err!");
        uapi_timer_deinit();
        return ERRCODE_INVALID_PARAM;
    }
    at_adapter_sample_print("timer2_task start");
    // step7：创建定时器2
    errcode_t ret3 = uapi_timer_create(args->timer_index_no, &timer_index[1]);
    if (ret3 == ERRCODE_INVALID_PARAM) {
        at_adapter_print("start timer2 err!");
        uapi_timer_deinit();
        return ERRCODE_INVALID_PARAM;
    }
    // step8：获取定时器2的开始时间
    g_timers_info[1].start_time = uapi_tcxo_get_ms();
    // step9：启动定时器2
    errcode_t ret4 = uapi_timer_start(timer_index[1], g_timers_info[1].delay_time, timer_timeout_callback, 1);
    if (ret4 == ERRCODE_INVALID_PARAM) {
        at_adapter_print("start timer2 err!");
        uapi_timer_deinit();
        return ERRCODE_INVALID_PARAM;
    }
    // step10：等待定时器定时结束
    while (g_timer_int_count < TIMER_NUM) {
        osal_msleep(1);
    }

    // step11：停止定时器并删除定时器，并将真实定时时间和设定的定时器时间作对比
    for (uint32_t i = 0; i < TIMER_NUM; i++) {
        uapi_timer_stop(timer_index[i]);
        uapi_timer_delete(timer_index[i]);
        at_adapter_sample_print("real time[%d] = %dms  delay = %dms\r\n",
            i, (g_timers_info[i].end_time - g_timers_info[i].start_time),
            g_timers_info[i].delay_time / TIMER_MS_2_US);
    }
    // step12：去初始化定时器
    uapi_timer_deinit();
    return ERRCODE_SUCC;
}

at_ret_t at_timer_sample(const timer_task_args_t *args)
{
    at_adapter_sample_print("==========  timer example test start  ==========");
    // 调用定时器的sample函数
    if (timer_run_sample(args) == ERRCODE_INVALID_PARAM) {
        return AT_RET_RUN_ERROR;
    }
    return AT_RET_OK;
}