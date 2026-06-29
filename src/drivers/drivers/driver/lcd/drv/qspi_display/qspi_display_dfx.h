/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. */

#ifndef QSPI_DISPLAY_DFX_H
#define QSPI_DISPLAY_DFX_H
#include "td_base.h"
#include "debug_print.h"
#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#ifdef NO_UART_DEBUG
#define qspi_proc_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define qspi_proc_print(fmt, args...) osal_printk(fmt, ##args)
#endif

typedef struct {
    td_u32 clk_div;
    td_u32 out_fmt;
} qspi_disp_dfx_info;

qspi_disp_dfx_info *qspi_disp_dfx_get_info(td_void);
td_void qspi_disp_dfx_set_clk_div(td_u32 clk_div);
#endif