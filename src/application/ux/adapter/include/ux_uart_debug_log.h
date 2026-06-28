/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: provides ux common log print function
 * Author: AuthorNameMagicTag
 * Create: 2023-04-07
 */

#ifndef UX_UART_DEBUG_LOG_H
#define UX_UART_DEBUG_LOG_H
#include "app_at_uart.h"

#undef ux_log_err
#undef ux_log_warn
#undef ux_log_info
#undef ux_log_debug
#define ux_log_err(fmt, args...) at_uart_printf(fmt, ##args)
#define ux_log_warn(fmt, args...) at_uart_printf(fmt, ##args)
#define ux_log_info(fmt, args...) at_uart_printf(fmt, ##args)
#define ux_log_debug(fmt, args...) at_uart_printf(fmt, ##args)
#endif
