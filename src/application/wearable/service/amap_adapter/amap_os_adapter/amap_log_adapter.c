/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Amap SDK log adapter
 * Create Date : 2025-11
 */

#include "amap_log_adapter.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/time.h>
#include "securec.h"

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#else
#include "debug_print.h"
#endif
#if !defined(SOCMN2_XTS) && !defined(SOCMN2_OHOS)
#include "wearable_onlinelog.h"
#endif

#define LOG_BUFFER_SIZE 256
#define MILLISECONDS_PER_SECOND 1000ULL
#define MICROSECONDS_PER_MILLISECOND 1000ULL

uint64_t amap_get_system_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t ms = (uint64_t)tv.tv_sec * MILLISECONDS_PER_SECOND + (uint64_t)tv.tv_usec / MICROSECONDS_PER_MILLISECOND;
    return ms;
}

static int v_worker(AmapLogLevel level, const char *fmt, va_list args)
{
    char log_buf[LOG_BUFFER_SIZE] = {0};

    int msg_len = vsnprintf_s(log_buf, LOG_BUFFER_SIZE, LOG_BUFFER_SIZE - 1, fmt, args);
    if (msg_len < 0) {
        return -1;
    }

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG) && !defined(CONFIG_ONLINE_LOG)

    if (level == AMAP_LOG_LEVEL_ERROR) {
        uapi_diag_error_log(0, "%s", log_buf);
    } else if (level == AMAP_LOG_LEVEL_WARN) {
        uapi_diag_warning_log(0, "%s", log_buf);
    } else if (level == AMAP_LOG_LEVEL_INFO) {
        uapi_diag_info_log(0, "%s", log_buf);
    } else if (level == AMAP_LOG_LEVEL_DEBUG) {
        uapi_diag_debug_log(0, "%s", log_buf);
    }

#elif (CONFIG_ONLINE_LOG == 1) && !defined(_WIN32)

    if (level == AMAP_LOG_LEVEL_ERROR) {
        ONLINE_PRINT(ONLINE_ERROR, "%s\n", log_buf);
    } else if (level == AMAP_LOG_LEVEL_WARN) {
        ONLINE_PRINT(ONLINE_WARN, "%s\n", log_buf);
    } else if (level == AMAP_LOG_LEVEL_INFO) {
        ONLINE_PRINT(ONLINE_INFO, "%s\n", log_buf);
    } else if (level == AMAP_LOG_LEVEL_DEBUG) {
        ONLINE_PRINT(ONLINE_DEBUG, "%s\n", log_buf);
    }

#else

    PRINT("%s\n", log_buf);

#endif

    return msg_len;
}

int amap_log_output(AmapLogLevel level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = v_worker(level, fmt, args);
    va_end(args);
    return ret;
}

int amap_log_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = v_worker(AMAP_LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);

    return ret;
}