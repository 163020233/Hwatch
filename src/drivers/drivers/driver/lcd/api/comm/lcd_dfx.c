/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#include "lcd_dfx.h"
#include "systick.h"
#include "lcd_common.h"

STATIC lcd_dfx_info g_lcd_dfx_info = {.print_period = 1, .hard_cycle_num = 1};

lcd_dfx_info *lcd_dfx_get_info(td_void)
{
    return &g_lcd_dfx_info;
}

td_void lcd_dfx_mcu_int_print_info(uint32_t soft_cycle_num)
{
    if (g_lcd_dfx_info.print_mode == LCD_PRINT_DISABLE) {
        return;
    }
    lcd_print("-------------------------------------\n");
    lcd_print("mcu_te_interrupt(us)      :%u\n", g_lcd_dfx_info.mcu_te_cost_us[0]);
    lcd_print("mcu_te_cnt                :%u\n", g_lcd_dfx_info.mcu_te_cnt);
    lcd_print("hard_cycle_num            :%u\n", g_lcd_dfx_info.hard_cycle_num);
    lcd_print("soft_cycle_num            :%u\n", soft_cycle_num);

    if (g_lcd_dfx_info.print_mode == LCD_PRINT_DETAIL) {
        for (td_u32 i = 0; i < MCU_TE_COST_DFX_NUM; i++) {
            lcd_print("mcu_te_interrupt(%u): %u\n", i, g_lcd_dfx_info.mcu_te_cost_us[i]);
        }
    }
}

td_void lcd_dfx_set_print_info(lcd_print_mode print_mode, uint32_t print_period)
{
    g_lcd_dfx_info.print_mode = print_mode;
    g_lcd_dfx_info.print_period = print_period;
}

td_void lcd_dfx_set_mcu_int_hard_cycle_num(uint32_t hard_cycle_num)
{
    g_lcd_dfx_info.hard_cycle_num = hard_cycle_num;
}

td_void lcd_mcu_int_period_proc(void)
{
    uint64_t cur_time_us = uapi_systick_get_us();
    lcd_dfx_info *dfx_info = lcd_dfx_get_info();
    if (dfx_info->mcu_te_cnt_total >= 3) { // print 3 mcu int cost
        uint32_t index0 = (dfx_info->mcu_te_cnt_total - 1) % MCU_TE_COST_DFX_NUM;
        uint32_t index1 = (dfx_info->mcu_te_cnt_total - 2) % MCU_TE_COST_DFX_NUM;
        uint32_t index2 = (dfx_info->mcu_te_cnt_total - 3) % MCU_TE_COST_DFX_NUM;

        lcd_print("cur_time %u %u last_mcu_int %u %u, cost %d %d %d\r\n",
            (uint32_t)(cur_time_us >> 32), (uint32_t)cur_time_us, // Shift right by 32 bits
            (uint32_t)(dfx_info->last_mcu_te_time_us >> 32), (uint32_t)dfx_info->last_mcu_te_time_us, // Shift right by 32 bits
            dfx_info->mcu_te_cost_us[index0], dfx_info->mcu_te_cost_us[index1], dfx_info->mcu_te_cost_us[index2]);
    }
}