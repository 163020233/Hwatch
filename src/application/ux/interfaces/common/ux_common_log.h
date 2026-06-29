/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: provides ux common log print function
 * Author: AuthorNameMagicTag
 * Create: 2023-04-07
 */

#ifndef UX_COMMON_LOG_H
#define UX_COMMON_LOG_H

#include "soc_diag_util.h"
#ifdef UX_DEBUG_AT_LOGOUT
#include "app_at_uart.h"
#endif

#ifdef WATCH_UART_LOG
#include "debug_print.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
#define SUCCESS 0                                        /**< 成功 */
#define FAIL (-1)                                        /**< 失败 */
#define OS_NO_WAIT 0x0U                                  /**< 操作系统不等待 */

#if defined(UX_DEBUG_AT_LOGOUT)
#define ux_log_err(fmt, args...) \
do { \
    at_uart_printf(fmt, ##args); \
    at_uart_printf("\n"); \
}while (0)

#define ux_log_warn(fmt, args...) \
do { \
    at_uart_printf(fmt, ##args); \
    at_uart_printf("\n"); \
}while (0)

#define ux_log_info(fmt, args...) \
do { \
    at_uart_printf(fmt, ##args); \
    at_uart_printf("\n"); \
}while (0)

#define ux_log_debug(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)

#elif defined(WATCH_UART_LOG)
#define ux_log_err(fmt, args...)        PRINT("[UX Err/] " fmt "\n", ##args)
#define ux_log_warn(fmt, args...)       PRINT("[UX Warn/] " fmt "\n", ##args)
#define ux_log_info(fmt, args...)       PRINT("[UX Info/] " fmt "\n", ##args)
#define ux_log_debug(fmt, args...)      PRINT("[UX Debug/] " fmt "\n", ##args)

#else

#ifdef SUPPORT_APP_SUB_MOD
#define ux_log_err(fmt, args...) uapi_diag_error_log(APP_SUB_MOD_UX, fmt, ##args)
#define ux_log_warn(fmt, args...) uapi_diag_warning_log(APP_SUB_MOD_UX, fmt, ##args)
#define ux_log_info(fmt, args...) uapi_diag_info_log(APP_SUB_MOD_UX, fmt, ##args)
#define ux_log_debug(fmt, args...) uapi_diag_debug_log(APP_SUB_MOD_UX, fmt, ##args)

#else
#define ux_log_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define ux_log_warn(fmt, args...) uapi_diag_warning_log(0, fmt, ##args)
#define ux_log_info(fmt, args...) uapi_diag_info_log(0, fmt, ##args)
#define ux_log_debug(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#endif
#endif

#ifdef UT_TEST_DEBUG
#undef ux_log_err       // 注销旧定义
#undef ux_log_warn       // 注销旧定义
#undef ux_log_info       // 注销旧定义
#undef ux_log_debug       // 注销旧定义
#define ux_log_err(fmt, args...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#define ux_log_warn(fmt, args...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#define ux_log_info(fmt, args...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#define ux_log_debug(fmt, args...) printf("[%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##args)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
