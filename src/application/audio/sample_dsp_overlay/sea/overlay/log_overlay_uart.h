/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: head of overlay log for uart
 * Author: audio
 */

#ifndef __LOG_OVERLAY_UART_H__
#define __LOG_OVERLAY_UART_H__

#include "audio_type.h"

#define AUDIO_LOG_LEVEL_ERROR 2
#define AUDIO_LOG_LEVEL_TRACE 6

#if (SAP_OS_TYPE == os_type_xtensa)
#define SAP_CORE "X"
/* Switch for log print in xtensa */
#ifndef AUDIO_LOG_SWITCH_X
#define AUDIO_LOG_SWITCH_X 1 /* 0:disable, 1:enable */
/* Default log level in xtensa */
#ifndef AUDIO_LOG_LEVEL
#define AUDIO_LOG_LEVEL AUDIO_LOG_LEVEL_ERROR
#endif /* AUDIO_LOG_LEVEL */
#endif /* AUDIO_LOG_SWITCH_X */

#else /* SAP_OS_TYPE */
#define SAP_CORE "A"
/* Switch for log print in arm */
#ifndef AUDIO_LOG_SWITCH_A
#ifdef SOC_PLF_TYPE_PROT
#define AUDIO_LOG_SWITCH_A 1 /* 0:disable, 1:enable */
#else
#define AUDIO_LOG_SWITCH_A 1 /* 0:disable, 1:enable */
#endif /* SOC_PLF_TYPE_PROT */
/* Default log level in arm */
#ifndef AUDIO_LOG_LEVEL
#define AUDIO_LOG_LEVEL AUDIO_LOG_LEVEL_ERROR
#endif /* AUDIO_LOG_LEVEL */
#endif /* AUDIO_LOG_SWITCH_A */

#endif /* SAP_OS_TYPE */

td_void audio_osal_print_fun(const td_char *core, td_u8 level, const td_char *fn_name, td_u32 line_num,
    const td_char *format, ...);

#define audio_log_print(level, fmt...)                                      \
    do {                                                                    \
        audio_osal_print_fun(SAP_CORE, level, __FUNCTION__, __LINE__, fmt); \
    } while (0)

#if (AUDIO_LOG_LEVEL == AUDIO_LOG_LEVEL_ERROR)
#define audio_log_err(fmt...)   audio_log_print(AUDIO_LOG_LEVEL_ERROR, fmt)
#else
#define audio_log_err(fmt...)
#endif

#define audio_log0(level, fmt...) audio_log_print(level, fmt)
#define audio_log1(level, fmt...) audio_log_print(level, fmt)
#define audio_log2(level, fmt...) audio_log_print(level, fmt)
#define audio_log3(level, fmt...) audio_log_print(level, fmt)
#define audio_log4(level, fmt...) audio_log_print(level, fmt)

#define audio_func_enter() audio_log0(AUDIO_LOG_LEVEL_TRACE, "enter...")
#define audio_func_exit()  audio_log0(AUDIO_LOG_LEVEL_TRACE, "exit...")

/* function trace log, strictly prohibited to expand */
#define sap_err_log_ret(err_code) audio_log_err("Error Code: [0x%08X]\n", (td_u32)(err_code))
/* function trace log, print the called function name when function is error */
#define sap_err_log_fun(func, err_code) audio_log_err("Call %s Failed, Error Code: [0x%08X]\n", #func, \
    (td_u32)(err_code))
/* Function trace log, print the pointer name when pointer is null */
#define sap_err_log_null_pointer(val) audio_log_err("%s = %p,  Null Pointer!\n", #val, val)

/* Used for displaying more detailed error information */
#define sap_err_log_s32(val) audio_log_err("%s = %d", #val, val)
#define sap_err_log_u32(val) audio_log_err("%s = %u", #val, val)
#define sap_err_log_s64(val) audio_log_err("%s = %lld", #val, val)
#define sap_err_log_u64(val) audio_log_err("%s = %llu", #val, val)
#define sap_err_log_h32(val) audio_log_err("%s = 0x%08X", #val, (td_u32)(val))
#define sap_err_log_h64(val) audio_log_err("%s = 0x%016llX", #val, val)
#define sap_err_log_str(val) audio_log_err("%s = %s", #val, val)
#define sap_err_log_void(val) audio_log_err("%s = %p", #val, val)
#define sap_err_log_float(val) audio_log_err("%s = %f", #val, val)
#define sap_err_log_bool(val) audio_log_err("%s = %s", #val, (val) ? "True" : "False")
#define sap_err_log_info(val) audio_log_err("<%s>", val)

#endif /* __LOG_OVERLAY_UART_H__ */
