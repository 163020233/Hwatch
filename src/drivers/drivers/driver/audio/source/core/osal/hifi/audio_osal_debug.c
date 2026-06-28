/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 */

#include <xtensa/hal.h>
#include <stdarg.h>
#include "audio_debug.h"
#include "securec.h"

#ifdef SAP_IPC_LOG_SUPPORT
#include "log_buffer.h"
#else
#include "serial_dw.h"
#endif

#define MAX_PRINT_SIZE 128

td_void audio_osal_serial_init(td_void)
{
#ifdef SAP_IPC_LOG_SUPPORT
    log_buffer_init();
#else
#if (AUDIO_LOG_SWITCH_X != 0) /* log enable */
    (td_void)serial_init();
#endif /* AUDIO_LOG_SWITCH_X */
#endif /* SAP_IPC_LOG_SUPPORT */
}

td_void audio_osal_print(const char *fmt, ...)
{
#ifdef SAP_IPC_LOG_SUPPORT
    audio_unused(fmt);
#else
#if (AUDIO_LOG_SWITCH_X != 0) /* log enable */
    int ret;
    va_list args;
    char str[MAX_PRINT_SIZE];

    va_start(args, fmt);
    ret = vsnprintf_s(str, sizeof(str), sizeof(str) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        return;
    }

    serial_puts(str);
#else
    audio_unused(fmt);
#endif /* AUDIO_LOG_SWITCH_X */
#endif /* SAP_IPC_LOG_SUPPORT */
}

td_void audio_osal_print_fun(const td_char *core, td_u8 level, const td_char *fn_name, td_u32 line_num,
    const td_char *format, ...)
{
#ifndef SAP_IPC_LOG_SUPPORT
#if (AUDIO_LOG_SWITCH_X != 0) // log enable
    td_s32 ret;
    va_list args;
    td_char buf[MAX_PRINT_SIZE + 1] = {'\0'}; /* add '\n' */
    td_u32 msg_len;

    const char *level_str[] = {
        "ALERT",
        "FATAL",
        "ERROR",
        "WARN ",
        "INFO ",
        "DEBUG",
        "TRACE",
    };

    if (core == TD_NULL || level > AUDIO_LOG_LEVEL || fn_name == TD_NULL) {
        return;
    }

    ret = snprintf_s(buf, MAX_PRINT_SIZE, MAX_PRINT_SIZE - 1,
        "[%s%d][%s][%s:%d] ", core, xthal_get_prid(), level_str[level], fn_name, line_num);
    if (ret < 0) {
        audio_osal_print("func head Call snprintf_s failed with %d\n", ret);
        return;
    }

    msg_len = (td_u32)ret;

    va_start(args, format);
    ret = vsnprintf_s(buf + msg_len, MAX_PRINT_SIZE - msg_len,
        MAX_PRINT_SIZE - msg_len - 1, format, args);
    va_end(args);
    if (ret < 0) {
        audio_osal_print("func log Call vsnprintf_s failed with %d\n", ret);
        return;
    }

    msg_len += (td_u32)ret;
    buf[msg_len] = '\n';

    serial_puts(buf);
    return;
#endif /* AUDIO_LOG_SWITCH_X */
#endif /* SAP_IPC_LOG_SUPPORT */
    audio_unused(core);
    audio_unused(level);
    audio_unused(fn_name);
    audio_unused(line_num);
    audio_unused(format);
}
