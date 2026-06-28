/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#ifndef MIPI_TX_DFX_H
#define MIPI_TX_DFX_H

#include "td_base.h"
#include "debug_print.h"
#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

/* mipi log define */
#ifdef NO_UART_DEBUG
#define mipi_proc_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define mipi_proc_print(fmt, args...) osal_printk(fmt, ##args)
#endif

#if defined(MIPI_TX_ERROR)

#ifdef NO_UART_DEBUG
#define mipi_tx_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define mipi_tx_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#define mipi_tx_print(fmt, args...)
#else
#define mipi_tx_err(fmt, args...)   osal_printk("[MIPI_TX_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mipi_tx_dbg(fmt, args...)
#define mipi_tx_print(fmt, args...)
#endif

#elif defined(MIPI_TX_DEBUG)

#ifdef NO_UART_DEBUG
#define mipi_tx_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define mipi_tx_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#define mipi_tx_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define mipi_tx_err(fmt, args...)   osal_printk("[MIPI_TX_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mipi_tx_dbg(fmt, args...)   osal_printk("[MIPI_TX_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define mipi_tx_print(fmt, args...) osal_printk(fmt, ##args)
#endif

#else

#define mipi_tx_print(fmt, args...)
#define mipi_tx_err(fmt, args...)
#define mipi_tx_dbg(fmt, args...)

#endif

#define CMD_IN_DATA_TRANS_CMD_MAX_NUM 8
#define MIPI_TX_STA_ERR_PRINT_MAX_CNT 25

typedef enum {
    MIPI_TX_CMD_TYPE_READ = 0,
    MIPI_TX_CMD_TYPE_WRITE,
} mipi_tx_cmd_type;

typedef struct {
    uint8_t cmd_type; // mipi_tx_cmd_type
    uint8_t cmd;
} mipi_tx_cmd_info;

typedef struct {
    uint32_t exit_ulps_cnt_ext; // 外部设置的ulps强制退出脉冲计数
    uint32_t cmd_in_data_trans_cnt; // cmd命令下发时mipi tx处于送显状态计数
    mipi_tx_cmd_info cmd_in_data_trans[CMD_IN_DATA_TRANS_CMD_MAX_NUM]; // 最近的送显状态下发的cmd命令
    uint32_t sus_sta_err_cnt; // suspend时未处于ulps状态err计数
    uint32_t sus_sta_err_cnt_ext; // suspend时外部控制导致未处于ulps状态err计数
    uint32_t pld_err_cnt;
} mipi_tx_dfx_info;

mipi_tx_dfx_info *mipi_tx_dfx_get_info(td_void);
td_void mipi_tx_dfx_record_cmd_in_data_trans(mipi_tx_cmd_info cmd_info);

#endif