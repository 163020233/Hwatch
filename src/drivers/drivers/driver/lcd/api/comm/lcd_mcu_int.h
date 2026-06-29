/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#ifndef LCD_MCU_INT_H
#define LCD_MCU_INT_H

#include "td_base.h"
#include "soc_lcd_api.h"

#define MCU_INT_HARD_MAX_CYCLE_NUM 60

ext_errno lcd_mcu_int_isr_init(void);
td_void lcd_mcu_int_isr_deinit(void);
td_void lcd_register_mcu_int_callback(mcu_int_callback callback);
td_void lcd_set_mcu_int_soft_cycle_num(uint32_t soft_cycle_num);
td_bool lcd_is_mcu_int_enable(void);
void lcd_hal_set_mcu_te_int_cnt_val(uint32_t mcu_int_te_cnt_val);
void lcd_hal_set_mcuint2te_time(uint32_t mcuint2te_time_us);

#endif