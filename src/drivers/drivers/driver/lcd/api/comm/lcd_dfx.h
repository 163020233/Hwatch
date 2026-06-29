/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#ifndef LCD_DFX_H
#define LCD_DFX_H

#include "td_base.h"
#include "debug_print.h"
#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#if defined(CONFIG_LCD_DFX_ERROR)

#ifdef NO_UART_DEBUG
#define lcd_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define lcd_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define lcd_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#else
#define lcd_print(fmt, args...) osal_printk(fmt, ##args)
#define lcd_err(fmt, args...)   osal_printk("[LCD_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define lcd_dbg(fmt, args...)
#endif

#elif defined(CONFIG_LCD_DFX_DEBUG)

#ifdef NO_UART_DEBUG
#define lcd_print(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define lcd_err(fmt, args...)   uapi_diag_error_log(0, fmt, ##args)
#define lcd_dbg(fmt, args...)   uapi_diag_debug_log(0, fmt, ##args)
#else
#define lcd_print(fmt, args...) osal_printk(fmt, ##args)
#define lcd_err(fmt, args...)   osal_printk("[LCD_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define lcd_dbg(fmt, args...)   osal_printk("[LCD_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif

#else

#define lcd_print(fmt, args...)
#define lcd_err(fmt, args...)
#define lcd_dbg(fmt, args...)

#endif

#define MCU_TE_COST_DFX_NUM 10

typedef enum {
    LCD_PRINT_DISABLE = 0,
    LCD_PRINT_OVERALL = 1,
    LCD_PRINT_DETAIL  = 2,
} lcd_print_mode;

typedef struct {
    lcd_print_mode print_mode;
    uint32_t print_period; // 单位：s
    uint64_t one_sec_ago;
    uint32_t mcu_te_cnt_total;
    uint32_t mcu_te_cost_us[MCU_TE_COST_DFX_NUM];
    uint32_t mcu_te_cnt;
    uint64_t last_mcu_te_time_us;
    uint32_t hard_cycle_num;
} lcd_dfx_info;

lcd_dfx_info *lcd_dfx_get_info(td_void);
td_void lcd_dfx_mcu_int_print_info(uint32_t soft_cycle_num);
td_void lcd_dfx_set_print_info(lcd_print_mode print_mode, uint32_t print_period);
td_void lcd_dfx_set_mcu_int_hard_cycle_num(uint32_t hard_cycle_num);
td_void lcd_mcu_int_period_proc(void);

#endif