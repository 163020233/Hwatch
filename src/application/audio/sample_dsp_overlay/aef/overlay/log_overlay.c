/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source code file for overlay_log module
 * Author: audio
 */
#include "log_overlay.h"
#include "securec.h"

#define PRINT_MAX_SIZE 1024

#if !defined(SAP_IPC_LOG_SUPPORT) && (AUDIO_LOG_SWITCH_X != 0) /* log enable */
static td_s32 add_fun_log_to_buffer(td_char *s, td_s32 size, const td_char *fmt, ...)
{
    va_list ap;
    td_s32 ret;

    va_start(ap, fmt);
    ret = vsnprintf_s(s, size, size - 1, fmt, ap);
    va_end(ap);
    return ret;
}
#endif /* SAP_IPC_LOG_SUPPORT */

td_void audio_osal_print_fun(const td_char *core, td_u8 level, const td_char *fn_name, td_u32 line_num,
    const td_char *format, ...)
{
#if !defined(SAP_IPC_LOG_SUPPORT) && (AUDIO_LOG_SWITCH_X != 0) /* log enable */
    td_s32 ret;
    va_list args;
    td_char buf[PRINT_MAX_SIZE] = {'\0'};
    td_u32 msg_len;

    char *level_str[] = {
        "ALERT",
        "FATAL",
        "ERROR",
        "WARN ",
        "INFO ",
        "DEBUG",
        "TRACE",
    };

    ret = 0;
    if (core == TD_NULL || level > AUDIO_LOG_LEVEL_TRACE || fn_name == TD_NULL) {
        return;
    }

    ret = add_fun_log_to_buffer(buf, PRINT_MAX_SIZE,
        "[%s][%s][%s:%d] ", core, level_str[level], fn_name, line_num);
    if (ret < 0) {
        printf("func head Call snprintf_s failed with %d\n", ret);
        return;
    }

    msg_len = (td_u32)ret;

    va_start(args, format);
    ret = vsnprintf_s(buf + msg_len, PRINT_MAX_SIZE - msg_len,
        PRINT_MAX_SIZE - msg_len - 1, format, args);
    va_end(args);
    if (ret < 0) {
        printf("func log Call vsnprintf_s failed with %d\n", ret);
        return;
    }

    msg_len += (td_u32)ret;
    buf[msg_len] = '\n';

    printf(buf);
    return;
#else
    audio_unused(core);
    audio_unused(level);
    audio_unused(fn_name);
    audio_unused(line_num);
    audio_unused(format);
#endif /* AUDIO_LOG_SWITCH_X */
}
