/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: WATCHDOG sample implementation \n
 *
 */
#include "pinctrl.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog_demo.h"

#define WDT_MODE 1                 // 看么狗模式0：reset 1：interrupt
#define TIME_OUT 8                 // 超时时间单位秒
#define WDT_TASK_DURATION_MS 5000  // 喂狗间隔时间

#define CONFIG_WDT_TIMEOUT_SAMPLE 1  // 是否启用看门狗超时测试
#define CONFIG_WDT_KICK_SAMPLE 1     // 是否启用看门狗喂狗测试

#define WDT_TASK_PRIO 1            // 任务优先级
#define WDT_TASK_STACK_SIZE 0x1000  // 任务堆栈大小

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

void at_diting_watchdog_example_cmd_register(void)
{
    uapi_at_cmd_table_register(g_at_watchdog_cmd_parse_table, AT_WATCHDOG_FUNC_NUM, EXT_AT_WATCHDOG_CMD_MAX_LEN);
}

// 看么狗超时回调函数
static errcode_t watchdog_callback(uintptr_t param)
{
    UNUSED(param);
    at_adapter_sample_print("watchdog kick timeout!");
    return ERRCODE_SUCC;
}

static void *watchdog_demo(void *arg)
{
    const wd_init_args_t *args = (const wd_init_args_t *)arg;

    // step1：初始化看门狗功能
    errcode_t ret = uapi_watchdog_init(TIME_OUT);
    if (ret == ERRCODE_INVALID_PARAM) {
        uapi_watchdog_deinit();
        at_adapter_sample_print("param is error, timeout is %d.", TIME_OUT);
        return NULL;
    }
    // step2：使能看门狗功能
    errcode_t ret1 = uapi_watchdog_enable((wdt_mode_t)WDT_MODE);
    if (ret1 == ERRCODE_FAIL) {
        at_adapter_print("enable watchdog err!");
        uapi_watchdog_deinit();
        return NULL;
    }
    // step3: 注册看门狗回调函数
    errcode_t ret2 = uapi_register_watchdog_callback(watchdog_callback);
    if (ret2 == ERRCODE_FAIL) {
        at_adapter_print("register watchdog calllback err!");
        uapi_watchdog_deinit();
        return NULL;
    }
    at_adapter_sample_print("init watchdog");
    // step4：决定是否喂狗
    if (args->kick_flag == 1) {
        goto KICK;
    }
#if defined(CONFIG_WDT_TIMEOUT_SAMPLE)
    // while循环，模拟系统故障，等待看门狗复位
    while (1) {
    };
#endif

#if defined(CONFIG_WDT_KICK_SAMPLE)
    // while循环，每隔一段时间喂狗一次，模拟系统正常运行
KICK:
    while (1) {
        osal_msleep(WDT_TASK_DURATION_MS);
        (void)uapi_watchdog_kick();
        at_adapter_sample_print("kick success");
    }
#endif
    // step5：去使能看门狗功能
    uapi_watchdog_deinit();
    return NULL;
}

at_ret_t at_watchdog_sample(wd_init_args_t *args)
{
    at_adapter_sample_print("==========  watchdog sample test start!  ==========");
    osal_task *task_handle = NULL;
    // step1 上锁，防止多线程资源抢占
    osal_kthread_lock();
    // step2 创建线程任务
    task_handle =
        osal_kthread_create((osal_kthread_handler)watchdog_demo, (void *)args, "WatchdogDemo", WDT_TASK_STACK_SIZE);
    // step3 设置线程任务优先级
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, WDT_TASK_PRIO);
    }
    // step4 解锁
    osal_kthread_unlock();

    return AT_RET_OK;
}