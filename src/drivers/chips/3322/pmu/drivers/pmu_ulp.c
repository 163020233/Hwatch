/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  PMU DRIVER Source
 */

#include "systick_porting.h"
#include "chip_io.h"
#include "chip_core_irq.h"
#include "debug_print.h"
#include "pm_pmu.h"
#include "tcxo.h"
#include "pmu_alarm.h"
#include "rtc_porting.h"
#include "pmu_ulp.h"

extern int hal_gpio_irq_handler(int irq_num, const void *tmp);

#define PM_ULP_WKUP_EN_MASK     (LP_CTL_ULP_A_RB_PWRON_WKUP_SHIP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_ULP_GPIO_INT_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_PWRON_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_VBUS_SENSE_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_VBUS_SENSE_DIS_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_PWRON_DIS_WKUP_EN_MASK)

#define POWER_HOLD_ENTER_RESET  (0x2)

static uint8_t g_ulp_reboot_reason = PMU_ULP_DEFAULT_REASON;

static osal_irq_handler g_ulp_pmu_irq_handlers[ULP_PMU_IRQ_MAX] = { NULL };

errcode_t uapi_pmu_ulp_register_irq(uint32_t irq_id, osal_irq_handler handler)
{
    if (irq_id >= ULP_PMU_IRQ_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    g_ulp_pmu_irq_handlers[irq_id] = handler;
    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_ulp_unregister_irq(uint32_t irq_id)
{
    if (irq_id >= ULP_PMU_IRQ_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    g_ulp_pmu_irq_handlers[irq_id] = NULL;
    return ERRCODE_SUCC;
}

static void ulp_pmu_execute_int_handler(uint32_t irq_id)
{
    if (g_ulp_pmu_irq_handlers[irq_id] != NULL) {
        g_ulp_pmu_irq_handlers[irq_id](ULP2MCU_INT, NULL);
    }
}

bool uapi_pmu_ulp_get_vbus_insert_state(void)
{
    return reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_V_GRM_STS_OFFSET);
}

bool uapi_pmu_ulp_get_pwrkey_press_state(void)
{
    if (reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_STS_OFFSET) == 0) {
        return true; // 按下
    }
    return false; // 松开
}

errcode_t uapi_pmu_ulp_set_pwrkey_reboot_time(uint8_t time)
{
    if (time >= LONG_PRESS_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    reg16_setbits(LP_CTL_ULP_B_RB_PWR_ON_N_FILTER_CTRL_REG, LP_CTL_ULP_B_RB_RE_PWR_TIME_GEAR_OFFSET,
        LP_CTL_ULP_B_RB_RE_PWR_TIME_GEAR_LEN, time);
    return ERRCODE_SUCC;
}

uint8_t uapi_pmu_ulp_get_reboot_reason(void)
{
#define PMU_ULP_DELAY_US       60UL
    if (g_ulp_reboot_reason != PMU_ULP_DEFAULT_REASON) {
        return g_ulp_reboot_reason;
    }

    // VBUS开机
    if (reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_V_GRM_STS_STICK_OFFSET)) {
        g_ulp_reboot_reason = PMU_ULP_VBUS_POWER_UP;
        reg16_setbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_V_GRM_STS_STICK_CLR_OFFSET);
        uapi_tcxo_delay_us(PMU_ULP_DELAY_US);
        reg16_clrbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_V_GRM_STS_STICK_CLR_OFFSET);
        PRINT("vbus power on"NEWLINE);
    }

    // 按键开机
    if (reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_INV_STS_STICK_OFFSET)) {
        g_ulp_reboot_reason = PMU_ULP_PWRKEY_POWER_UP;
        reg16_setbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_STS_INV_STICK_CLR_OFFSET);
        uapi_tcxo_delay_us(PMU_ULP_DELAY_US);
        reg16_clrbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_STS_INV_STICK_CLR_OFFSET);
        PRINT("powerkey press power on"NEWLINE);
    }

    // 长按键重启重启开机
    if (reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
        LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_LONG_GRM_INV_STS_STICK_OFFSET)) {
        g_ulp_reboot_reason = PMU_ULP_PWRKEY_LONG_PRESS_POWER_UP;
        reg16_setbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_LONG_GRM_INV_STS_STICK_CLR_OFFSET);
        uapi_tcxo_delay_us(PMU_ULP_DELAY_US);
        reg16_clrbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_LONG_GRM_INV_STS_STICK_CLR_OFFSET);
        PRINT("powerkey long press power on"NEWLINE);
    }

    // 闹铃重启重启开机
    if (reg16_getbit(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_STS_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_EVT_STS_OFFSET)) {
        g_ulp_reboot_reason = PMU_ULP_SYS_TICK_ALARM_POWER_UP;
        PRINT("alarm power on"NEWLINE);
    }

    // ULP_GPIO重启开机
    if (reg16_getbit(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_STS_REG, LP_CTL_ULP_A_RB_ULP_GPIO_INT_WKUP_EVT_STS_OFFSET)) {
        g_ulp_reboot_reason = PMU_ULP_GPIO_INT_POWER_UP;
        PRINT("ulp gpio power on"NEWLINE);
    }

    do {
        writew(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_CLR_REG, PM_ULP_WKUP_EN_MASK);
    } while (readw(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_STS_REG) != 0);

    do {
        writew(LP_CTL_ULP_A_RB_ULP_WKUP_INT_CLR_REG, PM_ULP_WKUP_EN_MASK);
    } while (readw(LP_CTL_ULP_A_RB_ULP_WKUP_INT_STS_REG) != 0);

    return g_ulp_reboot_reason;
}

static int pm_ulp_pmu_int_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    // 闹铃
    uint16_t val = readw(LP_CTL_ULP_A_RB_ULP_WKUP_INT_STS_REG);
    if (val & LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_INT_STS_MASK) {
        ulp_pmu_execute_int_handler(ULP_SYS_TICK_ALARM_IRQ);
    }

    // 短按键唤醒
    if (val & (LP_CTL_ULP_A_RB_PWRON_WKUP_SHIP_INT_STS_MASK | LP_CTL_ULP_A_RB_PWRON_WKUP_INT_STS_MASK)) {
        ulp_pmu_execute_int_handler(ULP_PWR_KEY_PRESS_IRQ);
    }

    // ULP GPIO唤醒
    if (val & LP_CTL_ULP_A_RB_ULP_GPIO_INT_WKUP_INT_STS_MASK) {
        ulp_pmu_execute_int_handler(ULP_GPIO_IRQ);
    }

    if (val & LP_CTL_ULP_A_RB_VBUS_SENSE_WKUP_INT_STS_MASK) {
        ulp_pmu_execute_int_handler(ULP_VBUS_INSERT_IRQ);
    }

    if (val & LP_CTL_ULP_A_RB_VBUS_SENSE_DIS_WKUP_INT_STS_MASK) {
        ulp_pmu_execute_int_handler(ULP_VBUS_DIS_IRQ);
    }

    if (val & LP_CTL_ULP_A_RB_PWRON_DIS_WKUP_INT_STS_MASK) {
        ulp_pmu_execute_int_handler(ULP_PWR_KEY_DIS_IRQ);
    }

    // ulp wkup int clr
    do {
        writew(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_CLR_REG, PM_ULP_WKUP_EN_MASK);
    } while (readw(LP_CTL_ULP_A_RB_ULP_WKUP_EVT_STS_REG) != 0);

    do {
        writew(LP_CTL_ULP_A_RB_ULP_WKUP_INT_CLR_REG, PM_ULP_WKUP_EN_MASK);
    } while (readw(LP_CTL_ULP_A_RB_ULP_WKUP_INT_STS_REG) != 0);

    osal_irq_clear(ULP2MCU_INT);

    return 0;
}

int __attribute__((weak)) alarm_irq_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    PRINT("alarm handler"NEWLINE);
    return 0;
}

int __attribute__((weak)) pwrkey_press_irq_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    PRINT("pwr key "NEWLINE);
    return 0;
}

int __attribute__((weak)) vbus_sense_insert_irq_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    PRINT("vbus insert "NEWLINE);
    return 0;
}

int __attribute__((weak)) vbus_sense_dis_irq_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    PRINT("vbus dis"NEWLINE);
    return 0;
}

int __attribute__((weak)) pwrkey_dis_irq_handler(int irq_num, const void *tmp)
{
    unused(irq_num);
    unused(tmp);
    PRINT("pwrkey dis"NEWLINE);
    return 0;
}

void uapi_ulp_pmu_init(void)
{
    uint8_t reboot_reason = uapi_pmu_ulp_get_reboot_reason();
    PRINT("pmu ulp reboot reason: 0x%x"NEWLINE, reboot_reason);

    if (reboot_reason == PMU_ULP_SYS_TICK_ALARM_POWER_UP) {
        trigger_alarm();
    }

    writew(LP_CTL_ULP_A_RB_ULP_WKUP_EN_REG, PM_ULP_WKUP_EN_MASK);
    writew(LP_CTL_ULP_A_RB_ULP_WKUP_INT_EN_REG, PM_ULP_WKUP_EN_MASK);
    uapi_alarm_disable();

    writew(LP_CTL_ULP_B_RB_POWER_UP_DOWN_SHIP_MODE_SEL_REG, POWER_HOLD_ENTER_RESET);

    uapi_pmu_ulp_set_pwrkey_reboot_time(LONG_PRESS_12S);
    uapi_pmu_ulp_register_irq(ULP_GPIO_IRQ, (osal_irq_handler)hal_gpio_irq_handler);
    uapi_pmu_ulp_register_irq(ULP_PWR_KEY_PRESS_IRQ, (osal_irq_handler)pwrkey_press_irq_handler);
    uapi_pmu_ulp_register_irq(ULP_VBUS_INSERT_IRQ, (osal_irq_handler)vbus_sense_insert_irq_handler);
    uapi_pmu_ulp_register_irq(ULP_VBUS_DIS_IRQ, (osal_irq_handler)vbus_sense_dis_irq_handler);
    uapi_pmu_ulp_register_irq(ULP_PWR_KEY_DIS_IRQ, (osal_irq_handler)pwrkey_dis_irq_handler);
    uapi_pmu_ulp_register_irq(ULP_SYS_TICK_ALARM_IRQ, (osal_irq_handler)alarm_irq_handler);

    // wkup int enable
    osal_irq_disable(ULP2MCU_INT);
    osal_irq_free(ULP2MCU_INT, NULL);
    osal_irq_request(ULP2MCU_INT, (osal_irq_handler)pm_ulp_pmu_int_handler, NULL, NULL, NULL);
    osal_irq_enable(ULP2MCU_INT);
}

errcode_t uapi_alarm_enable(uint32_t timeout_s)
{
    uint32_t irq_sts = osal_irq_lock();

    reg16_setbit(LP_CTL_ULP_A_RB_ULP_WKUP_EN_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_EN_OFFSET);
    reg16_setbit(LP_CTL_ULP_A_RB_ULP_WKUP_INT_EN_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_INT_EN_OFFSET);
    reg16_clrbit(LP_CTL_ULP_A_RB_ULP_SHIPTOBOOT_BYPASS_CTRL_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_BYPASS_OFFSET);

    uint64_t count = uapi_ulp_systick_get_count();
    count += ((uint64_t)timeout_s * rtc_porting_clock_value_get());

    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_3_REG, (uint16_t)(count >> 48U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_2_REG, (uint16_t)(count >> 32U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_1_REG, (uint16_t)(count >> 16U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_0_REG, (uint16_t)count);

    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_alarm_disable(void)
{
    reg16_clrbit(LP_CTL_ULP_A_RB_ULP_WKUP_EN_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_EN_OFFSET);
    reg16_clrbit(LP_CTL_ULP_A_RB_ULP_WKUP_INT_EN_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_INT_EN_OFFSET);
    reg16_setbit(LP_CTL_ULP_A_RB_ULP_SHIPTOBOOT_BYPASS_CTRL_REG, LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_BYPASS_OFFSET);
    return ERRCODE_SUCC;
}

uint64_t uapi_alarm_get_left_count(void)
{
    uint64_t count_cur = uapi_ulp_systick_get_count();
    uint64_t count_thr = (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_0_REG);
    count_thr |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_1_REG) << 16U;
    count_thr |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_2_REG) << 32U;
    count_thr |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_3_REG) << 48U;
    if (count_thr > count_cur) {
        return (uint64_t)(count_thr - count_cur);
    }
    return 0;
}

void uapi_alarm_set_count(uint64_t count)
{
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_3_REG, (uint16_t)(count >> 48U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_2_REG, (uint16_t)(count >> 32U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_1_REG, (uint16_t)(count >> 16U));
    writew(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_THR_0_REG, (uint16_t)count);
}