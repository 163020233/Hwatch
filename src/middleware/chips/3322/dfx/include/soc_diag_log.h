/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: diag print log
 */

#ifndef SOC_DIAG_LOG_H
#define SOC_DIAG_LOG_H

#include "diag_oam_log.h"
#include "log_def.h"
#include "log_module_id.h"
#include "chip_core_definition.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DIAG_LOG_LEVEL_ALERT    0
#define DIAG_LOG_LEVEL_FATAL    1
#define DIAG_LOG_LEVEL_ERROR    2
#define DIAG_LOG_LEVEL_WARN     3
#define DIAG_LOG_LEVEL_NOTICE   4
#define DIAG_LOG_LEVEL_INFO     5
#define DIAG_LOG_LEVEL_DBG      6
#define DIAG_LOG_LEVEL_TRACE    7
#define DIAG_LOG_LEVEL_EVENT    DIAG_LOG_LEVEL_TRACE

#ifndef CORE_ID
#define CORE_ID CORE
#endif

#define check_default_id(id) (((id) == 0) ? __LINE__ : (id))

#ifdef OAM_LOG_PARAM_MAX_NUM
#undef OAM_LOG_PARAM_MAX_NUM
#endif
#define OAM_LOG_PARAM_MAX_NUM           12

#define var_args_max_12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, ...)     a14
#define var_args_cnt(unused, args...)   var_args_max_12(unused, ##args, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/** msg_id:
 * |-------------------------------------------------------------------------------------------------------|
 * |-H--------------------------------------------- 32 bit ----------------------------------------------L-|
 * |  1 bit(31)  |    4 bits(30 ~ 27)  |    10 bits(26 ~ 17)    |      14 bits(16 ~ 3)      | 3 bits(2 ~ 0)|
 * |  log_flg    |       mod_id        |        file_id         |          line No.         |    level     |
 * |-------------------------------------------------------------------------------------------------------|
 */
#define diag_log_msg_mk_id(mid, fileid, lineno, lv) (((uint32_t)(1UL << 31)) \
    + (((uint32_t)((mid) << 27)) & 0x78000000) + (((uint32_t)((fileid) << 17)) & 0x7fe0000) \
    + ((((uint32_t)(check_default_id(lineno))) << 3) & 0x1fff8) + ((lv) & 0x7))
#define diag_log_msg_mk_id_e(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_ERROR)
#define diag_log_msg_mk_id_w(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_WARN)
#define diag_log_msg_mk_id_n(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_NOTICE)
#define diag_log_msg_mk_id_i(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_INFO)
#define diag_log_msg_mk_id_d(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_DBG)
#define diag_log_msg_mk_id_ev(id) diag_log_msg_mk_id(THIS_MOD_ID, THIS_FILE_ID, id, DIAG_LOG_LEVEL_EVENT)

/** mod_id:
 * |----------------------------------------------------------------------------|
 * |-H-------------------------------- 16 bit --------------------------------L-|
 * |   3 bits(15 ~ 13)   |    5 bits(12 ~ 8)   |          8 bits(7 ~ 0)         |
 * |       core_id       |      sub_mod_id     |             mod_id             |
 * |----------------------------------------------------------------------------|
 */
#define diag_log_msg_mk_mod_id_custom(mod_id, sub_mod, core_id) ((uint16_t)((uint16_t)(mod_id) | \
    (((uint16_t)(sub_mod) & 0x1f) << 8) | ((uint16_t)(core_id) << 13)))
#define diag_log_msg_mk_mod_id(sub_mod) diag_log_msg_mk_mod_id_custom(THIS_MOD_ID, sub_mod, CORE_ID)

#if (defined(DIAG_PRINT_TO_SHELL))
#define diag_io_print(fmt, arg...) printf("[%s:%d] "fmt"\r\n", __func__, __LINE__, ##arg)
#else
#define diag_io_print(fmt, arg...)
#endif

#if (defined(DIAG_PRINT))

#define diag_log_common_with_mod(mod_id, msg_id, arg_cnt, args...) \
    do { \
        if ((arg_cnt) == 0) { \
            oam_log_optim_print0((mod_id), (msg_id)); \
        } else if ((arg_cnt) == 1) { \
            oam_log_optim_print1((mod_id), (msg_id), ##args); \
        } else if ((arg_cnt) == 2) { \
            oam_log_optim_print2((mod_id), (msg_id), ##args); \
        } else if ((arg_cnt) == 3) { \
            oam_log_optim_print3((mod_id), (msg_id), ##args); \
        } else if ((arg_cnt) == 4) { \
            oam_log_optim_print4((mod_id), (msg_id), ##args); \
        } else { \
            oam_log_optim_printx((mod_id), (arg_cnt), (msg_id), ##args); \
        } \
    } while (0)

#define diag_log_common_no_mod(msg_id, arg_cnt, args...) \
    do { \
        if ((arg_cnt) == 0) { \
            oam_log_optim_no_mod_print0((msg_id)); \
        } else if ((arg_cnt) == 1) { \
            oam_log_optim_no_mod_print1((msg_id), ##args); \
        } else if ((arg_cnt) == 2) { \
            oam_log_optim_no_mod_print2((msg_id), ##args); \
        } else if ((arg_cnt) == 3) { \
            oam_log_optim_no_mod_print3((msg_id), ##args); \
        } else if ((arg_cnt) == 4) { \
            oam_log_optim_no_mod_print4((msg_id), ##args); \
        } else { \
            oam_log_optim_no_mod_printx((arg_cnt), (msg_id), ##args); \
        } \
    } while (0)

#define uapi_diag_error_log0(id, fmt) \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_e(0))
#define uapi_diag_error_log1(id, fmt, p1) \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_e(0), p1)
#define uapi_diag_error_log2(id, fmt, p1, p2) \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_e(0), p1, p2)
#define uapi_diag_error_log3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_e(0), &log_msg); \
    } while (0)
#define uapi_diag_error_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_e(0), &log_msg); \
    } while (0)
#define uapi_diag_error_logx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_e(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_e(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_error_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_e(0), log_info, data_size)

#define uapi_diag_warning_log0(id, fmt) \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_w(0))
#define uapi_diag_warning_log1(id, fmt, p1) \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_w(0), p1)
#define uapi_diag_warning_log2(id, fmt, p1, p2) \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_w(0), p1, p2)
#define uapi_diag_warning_log3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_w(0), &log_msg); \
    } while (0)
#define uapi_diag_warning_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_w(0), &log_msg); \
    } while (0)
#define uapi_diag_warning_logx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_w(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_w(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_warning_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_w(0), log_info, data_size)

#define uapi_diag_notice_log0(id, fmt) \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_n(0))
#define uapi_diag_notice_log1(id, fmt, p1) \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_n(0), p1)
#define uapi_diag_notice_log2(id, fmt, p1, p2) \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_n(0), p1, p2)
#define uapi_diag_notice_log3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_n(0), &log_msg); \
    } while (0)
#define uapi_diag_notice_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_n(0), &log_msg); \
    } while (0)
#define uapi_diag_notice_logx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_n(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_n(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_notice_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_n(0), log_info, data_size)

#define uapi_diag_info_log0(id, fmt)  \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_i(0))
#define uapi_diag_info_log1(id, fmt, p1)  \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_i(0), p1)
#define uapi_diag_info_log2(id, fmt, p1, p2)  \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_i(0), p1, p2)
#define uapi_diag_info_log3(id, fmt, p1, p2, p3)  \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_i(0), &log_msg); \
    } while (0)
#define uapi_diag_info_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_i(0), &log_msg); \
    } while (0)
#define uapi_diag_info_logx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_i(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_i(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_info_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_i(0), log_info, data_size)

#define uapi_diag_debug_log0(id, fmt)  \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_d(0))
#define uapi_diag_debug_log1(id, fmt, p1)  \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_d(0), p1)
#define uapi_diag_debug_log2(id, fmt, p1, p2)  \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_d(0), p1, p2)
#define uapi_diag_debug_log3(id, fmt, p1, p2, p3)  \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_i(0), &log_msg); \
    } while (0)
#define uapi_diag_debug_log4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_d(0), &log_msg); \
    } while (0)
#define uapi_diag_debug_logx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_d(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_d(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_debug_log_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_d(0), log_info, data_size)

#define uapi_diag_event0(id, fmt) \
    oam_log_optim_fix_print0(id, diag_log_msg_mk_id_ev(0))
#define uapi_diag_event1(id, fmt, p1) \
    oam_log_optim_fix_print1(id, diag_log_msg_mk_id_ev(0), p1)
#define uapi_diag_event2(id, fmt, p1, p2) \
    oam_log_optim_fix_print2(id, diag_log_msg_mk_id_ev(0), p1, p2)
#define uapi_diag_event3(id, fmt, p1, p2, p3) \
    do { \
        zdiag_log_msg3_t log_msg = {p1, p2, p3}; \
        oam_log_optim_fix_print3(id, diag_log_msg_mk_id_ev(id), &log_msg); \
    } while (0)
#define uapi_diag_event4(id, fmt, p1, p2, p3, p4) \
    do { \
        zdiag_log_msg4_t log_msg = {p1, p2, p3, p4}; \
        oam_log_optim_fix_print4(id, diag_log_msg_mk_id_ev(0), &log_msg); \
    } while (0)
#define uapi_diag_eventx(id, fmt, args...) \
    do { \
        if (id == 0) { \
            diag_log_common_no_mod(diag_log_msg_mk_id_ev(0), var_args_cnt(unused, ##args), ##args); \
        } else { \
            diag_log_common_with_mod(id, diag_log_msg_mk_id_ev(0), var_args_cnt(unused, ##args), ##args); \
        } \
    } while (0)
#define uapi_diag_event_buff(id, log_info, data_size, fmt, type) \
    oam_log_optim_fix_print_buff(id, diag_log_msg_mk_id_ev(0), log_info, data_size)

#else /* DIAG_PRINT */

#define uapi_diag_error_log0(id, fmt) diag_io_print("[ERROR]"fmt"\r\n")
#define uapi_diag_error_log1(id, fmt, p1) diag_io_print("[ERROR]"fmt" %d\r\n", p1)
#define uapi_diag_error_log2(id, fmt, p1, p2) diag_io_print("[ERROR]"fmt" %d  %d\r\n", p1, p2)
#define uapi_diag_error_log3(id, fmt, p1, p2, p3) diag_io_print("[ERROR]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_error_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[ERROR]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_error_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_error_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_warning_log0(id, fmt) diag_io_print("[WARNING]"fmt"\r\n")
#define uapi_diag_warning_log1(id, fmt, p1) diag_io_print("[WARNING]"fmt" %d\r\n", p1)
#define uapi_diag_warning_log2(id, fmt, p1, p2) diag_io_print("[WARNING]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_warning_log3(id, fmt, p1, p2, p3) diag_io_print("[WARNING]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_warning_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[WARNING]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_warning_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_warning_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_notice_log0(id, fmt) diag_io_print("[NOTICE]"fmt"\r\n")
#define uapi_diag_notice_log1(id, fmt, p1) diag_io_print("[NOTICE]"fmt" %d\r\n", p1)
#define uapi_diag_notice_log2(id, fmt, p1, p2) diag_io_print("[NOTICE]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_notice_log3(id, fmt, p1, p2, p3) diag_io_print("[NOTICE]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_notice_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[NOTICE]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_notice_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_notice_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_info_log0(id, fmt) diag_io_print("[INFO]"fmt"\r\n")
#define uapi_diag_info_log1(id, fmt, p1) diag_io_print("[INFO]"fmt" %d\r\n", p1)
#define uapi_diag_info_log2(id, fmt, p1, p2) diag_io_print("[INFO]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_info_log3(id, fmt, p1, p2, p3) diag_io_print("[INFO]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_info_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[INFO]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_info_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_info_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_debug_log0(id, fmt) diag_io_print("[INFO]"fmt"\r\n")
#define uapi_diag_debug_log1(id, fmt, p1) diag_io_print("[INFO]"fmt" %d\r\n", p1)
#define uapi_diag_debug_log2(id, fmt, p1, p2) diag_io_print("[INFO]"fmt" %d %d\r\n", p1, p2)
#define uapi_diag_debug_log3(id, fmt, p1, p2, p3) diag_io_print("[INFO]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_debug_log4(id, fmt, p1, p2, p3, p4) diag_io_print("[INFO]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_debug_logx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_debug_log_buff(id, log_info, data_size, fmt, type)

#define uapi_diag_event0(id, fmt) diag_io_print("[EVENT]"fmt"\r\n")
#define uapi_diag_event1(id, fmt, p1) diag_io_print("[EVENT]"fmt" %d\r\n", p1)
#define uapi_diag_event2(id, fmt, p1, p2) diag_io_print("[EVENT]"fmt" %d  %d\r\n", p1, p2)
#define uapi_diag_event3(id, fmt, p1, p2, p3) diag_io_print("[EVENT]"fmt" %d %d %d\r\n", p1, p2, p3)
#define uapi_diag_event4(id, fmt, p1, p2, p3, p4) diag_io_print("[EVENT]"fmt" %d %d %d %d\r\n", p1, p2, p3, p4)
#define uapi_diag_eventx(id, fmt, args...) diag_io_print(fmt, ##args)
#define uapi_diag_event_buff(id, log_info, data_size, fmt, type)

#endif /* end DIAG_PRINT */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* SOC_DIAG_LOG_H */
