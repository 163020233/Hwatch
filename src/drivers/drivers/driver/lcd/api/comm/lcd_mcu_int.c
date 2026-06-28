/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. */

#include "lcd_mcu_int.h"
#include "securec.h"
#include "osal_spinlock.h"
#include "osal_interrupt.h"
#include "chip_core_irq.h"
#include "tcxo.h"
#include "systick.h"
#include "semactrl.h"
#include "pm_sleep_porting.h"
#include "lcd_dfx.h"
#include "lcd_common.h"

#define MCU_TE_INT_CLR_REG              (GLB_CTL_M_RB_BASE + 0x294)
#define MCU_TE_INT_STOP_CNT_VAL_REG     (GLB_CTL_M_RB_BASE + 0x2A4)
#define MCU_TE_INT_CNT_BYPASS_REG       (GLB_CTL_M_RB_BASE + 0x2A8)
#define TE_SOFT_RESET_REG               (GLB_CTL_M_RB_BASE + 0xB0)

#define MCU_INT_TE_CNT_POS              0
#define MCU_INT_TE_CNT_LEN              6
#define MCU_INT_TE_CNT_BYPASS_POS       6
#define MCU_TE_INT_CLR_POS              3

#define MCU_TE_INT_STOP_CNT_MAX_VAL     0xFFFF

#ifdef GPU_1A22
#define TE_INT_REG                      (GLB_CTL_M_RB_BASE + 0x290)
#define TE_SOFT_RESET_POS               0
#define MCU_INT_TE_CNT_CLR_POS          10
#else
#define TE_SOFT_RESET_POS               6
#endif

#ifdef PRE_FPGA
#define CLK_VIDEO_RTC                   31850
#else
#define CLK_VIDEO_RTC                   32768
#endif

typedef struct {
    osal_spinlock isr_lock;
    td_bool is_inited;
    mcu_int_callback callback;
    uint32_t soft_cycle_num;
    uint32_t cur_cycle_index;
} mcu_int_isr_info;

STATIC mcu_int_isr_info g_mcu_int_isr_info = {.soft_cycle_num = 1};

#ifdef GPU_1A22
void lcd_hal_set_mcu_int_en(td_bool enable)
{
    uapi_reg_setbits((uintptr_t)TE_INT_REG, 5, 1, enable); /* bit5 mask */
    uapi_reg_setbits((uintptr_t)TE_INT_REG, 1, 1, enable); /* bit1 en */
}

void lcd_hal_clr_mcu_int_te_cnt(void)
{
    uapi_reg_setbit((uintptr_t)(MCU_TE_INT_CLR_REG), MCU_INT_TE_CNT_CLR_POS);
}
#endif

STATIC uint32_t lcd_hal_get_mcu_int_stop_cnt(uint32_t ulpsflg2te_time_us)
{
    return (LCD_US_PRE_SECOND / LCD_DEFAULT_FRAME_RATE - ulpsflg2te_time_us) *
        CLK_VIDEO_RTC / LCD_US_PRE_SECOND;
}

void lcd_hal_set_mcuint2te_time(uint32_t mcuint2te_time_us)
{
    uint32_t mcuint2te_time = (mcuint2te_time_us == MCU_TE_INT_STOP_CNT_MAX_VAL) ? MCU_TE_INT_STOP_CNT_MAX_VAL :
        lcd_hal_get_mcu_int_stop_cnt(mcuint2te_time_us);
    uapi_reg_write(MCU_TE_INT_STOP_CNT_VAL_REG, mcuint2te_time);
}

void lcd_hal_set_mcu_te_int_cnt_val(uint32_t mcu_int_te_cnt_val)
{
    /* 0：非bypass 几个TE产生一次MCU INT */
    uapi_reg_setbits((uintptr_t)MCU_TE_INT_CNT_BYPASS_REG, MCU_INT_TE_CNT_POS, MCU_INT_TE_CNT_LEN, mcu_int_te_cnt_val);
    uapi_reg_clrbit((uintptr_t)MCU_TE_INT_CNT_BYPASS_REG, MCU_INT_TE_CNT_BYPASS_POS);
}

void uapi_lcd_set_mcuint2te_time(uint32_t mcuint2te_time_us)
{
    if (lcd_is_mcu_int_enable() == TD_TRUE) {
        lcd_hal_set_mcuint2te_time(mcuint2te_time_us);
    }
}

STATIC td_bool lcd_isr_is_need_bypass_mcu_te_int(void)
{
    if (g_mcu_int_isr_info.soft_cycle_num == 1) {
        return TD_FALSE;
    }

    if (g_mcu_int_isr_info.cur_cycle_index > g_mcu_int_isr_info.soft_cycle_num - 1) {
        g_mcu_int_isr_info.cur_cycle_index = 0;
    }

    if (g_mcu_int_isr_info.cur_cycle_index == 0) {
        g_mcu_int_isr_info.cur_cycle_index++;
        return TD_FALSE;
    }

    g_mcu_int_isr_info.cur_cycle_index++;
    return TD_TRUE;
}

STATIC INLINE void lcd_clear_mcu_int_status(void)
{
    uapi_reg_setbit((uintptr_t)(MCU_TE_INT_CLR_REG), MCU_TE_INT_CLR_POS);
#ifndef GPU_1A22
    osal_irq_clear(MCU_TE_IRQN);
#endif
}

#ifndef GPU_1A22
STATIC INLINE void lcd_reset_te(void)
{
    const td_u32 sema_timoout_us = 200;
    uint32_t irq_sts = osal_irq_lock();
    errcode_t ret = uapi_sema_get(SEMA_3, sema_timoout_us);
    if (ret != ERRCODE_SUCC) {
        lcd_err("lcd get sema err[%d]\n", ret);
        osal_irq_restore(irq_sts);
        return;
    }
    uapi_reg_clrbit16(TE_SOFT_RESET_REG, TE_SOFT_RESET_POS);  // TE复位
    uapi_reg_setbit16(TE_SOFT_RESET_REG, TE_SOFT_RESET_POS);  // TE解复位
    uapi_sema_put(SEMA_3);
    osal_irq_restore(irq_sts);
}
#endif

STATIC td_void lcd_isr_mcu_te_int_main(uint32_t param)
{
    uapi_unused(param);
    lcd_clear_mcu_int_status();
    unsigned long flags = 0;
    osal_spin_lock_irqsave(&g_mcu_int_isr_info.isr_lock, &flags);

    if (lcd_isr_is_need_bypass_mcu_te_int()) {
        osal_spin_unlock_irqrestore(&g_mcu_int_isr_info.isr_lock, &flags);
        return;
    }

    if (g_mcu_int_isr_info.callback != TD_NULL) {
        g_mcu_int_isr_info.callback();
    }

    lcd_dfx_info *dfx_info = lcd_dfx_get_info();
    uint64_t cur_time_us = uapi_systick_get_us();
    dfx_info->mcu_te_cost_us[dfx_info->mcu_te_cnt_total % MCU_TE_COST_DFX_NUM] =
        (uint32_t)(cur_time_us - dfx_info->last_mcu_te_time_us);
    dfx_info->last_mcu_te_time_us = cur_time_us;
    dfx_info->mcu_te_cnt_total++;
    dfx_info->mcu_te_cnt++;

    cur_time_us = uapi_systick_get_us();
    if ((cur_time_us - dfx_info->one_sec_ago) >= dfx_info->print_period * LCD_US_PRE_SECOND) {
        dfx_info->one_sec_ago = cur_time_us;
        lcd_dfx_mcu_int_print_info(g_mcu_int_isr_info.soft_cycle_num);
        dfx_info->mcu_te_cnt = 0;
    }

    osal_spin_unlock_irqrestore(&g_mcu_int_isr_info.isr_lock, &flags);
}

td_void lcd_enable_mcu_int(void)
{
#ifdef GPU_1A22
    lcd_hal_clr_mcu_int_te_cnt();
    lcd_hal_set_mcu_int_en(1);
#else
    lcd_reset_te();
#endif
    pm_port_set_display_intr_en(1);
}

ext_errno lcd_mcu_int_isr_init(void)
{
    if (g_mcu_int_isr_info.is_inited) {
        lcd_enable_mcu_int();
        return EXT_ERR_SUCCESS;
    }

    static td_bool is_first = TD_TRUE;
    if (is_first) {
        td_s32 ret = osal_spin_lock_init(&g_mcu_int_isr_info.isr_lock);
        if (ret != EXT_ERR_SUCCESS) {
            lcd_err("osal_spin_lock_init failed, ret:%d", ret);
            return EXT_ERR_SPINLOCK_NOT_INIT;
        }

        ret = osal_irq_request(MCU_TE_IRQN, (osal_irq_handler)lcd_isr_mcu_te_int_main, TD_NULL, TD_NULL, TD_NULL);
        if (ret != EXT_ERR_SUCCESS) {
            osal_spin_lock_destroy(&g_mcu_int_isr_info.isr_lock);
            lcd_err("osal_irq_request mcu_te_int failed, ret:%d", ret);
            return EXT_ERR_ISR_REQ_IRQ_FAIL;
        }

        osal_irq_enable(MCU_TE_IRQN);
        is_first = TD_FALSE;
    }

    lcd_hal_set_mcuint2te_time(MCU_INT_TO_TE_TIME);
    lcd_enable_mcu_int();
    g_mcu_int_isr_info.is_inited = TD_TRUE;

    return EXT_ERR_SUCCESS;
}

td_void lcd_mcu_int_isr_deinit(void)
{
    if (!g_mcu_int_isr_info.is_inited) {
        return;
    }

#ifdef GPU_1A22
    lcd_hal_set_mcu_int_en(0);
#else
    lcd_clear_mcu_int_status();
    lcd_hal_set_mcuint2te_time(MCU_TE_INT_STOP_CNT_MAX_VAL);
    lcd_hal_set_mcu_te_int_cnt_val(MCU_INT_HARD_MAX_CYCLE_NUM);
#endif
    pm_port_set_display_intr_en(0);
    g_mcu_int_isr_info.is_inited = TD_FALSE;
}

td_void lcd_register_mcu_int_callback(mcu_int_callback callback)
{
    g_mcu_int_isr_info.callback = callback;
}

td_void lcd_set_mcu_int_soft_cycle_num(uint32_t soft_cycle_num)
{
    g_mcu_int_isr_info.soft_cycle_num = soft_cycle_num;
}

td_bool lcd_is_mcu_int_enable(void)
{
    return g_mcu_int_isr_info.is_inited;
}