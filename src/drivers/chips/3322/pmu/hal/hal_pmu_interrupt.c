/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: PMU INTERRUPT FUNCTIONS
 */

#include "hal_pmu_interrupt.h"
#include "non_os.h"
#include "hal_reg_config.h"
#include "common_def.h"
#include "platform_core.h"
#include "soc_reg.h"

#define PMU_CMU_INT_CLEAR_CFG_LOW 0xFFFF
#define PMU_CMU_INT_CLEAR_CFG_HIGH 0x1FF
#define PMU_CMU_REG_MAX 16

void hal_pmu_cmu_err_irq_enable(uint32_t reg_addr, uint16_t offset, switch_type_t on)
{
    if (on == TURN_ON) {
        reg16_setbit(reg_addr, offset);
    } else if (on == TURN_OFF) {
        reg16_clrbit(reg_addr, offset);
    }
}

void hal_pmu_err_irq_clear(pmu_cmu_err_int_type_t int_type)
{
    uint32_t addr_low = 0;
    uint32_t addr_high = 0;
    if (int_type == PMU_CMU_GRM_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_1STICK_CLR_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_1STICK_CLR_1_REG;
    } else if (int_type == PMU_CMU_RAW_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_CLR_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_CLR_1_REG;
    } else {
        return;
    }

    writew(addr_low, PMU_CMU_INT_CLEAR_CFG_LOW);
    writew(addr_high, PMU_CMU_INT_CLEAR_CFG_HIGH);
}

uint32_t hal_pmu_err_irq_get_status(pmu_cmu_err_int_type_t int_type)
{
    uint32_t addr_low = 0;
    uint32_t addr_high = 0;
    if (int_type == PMU_CMU_GRM_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_1STICK_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_1STICK_1_REG;
    } else if (int_type == PMU_CMU_RAW_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_1_REG;
    } else {
        return 0;
    }

    uint16_t value_low = readw(addr_low);
    uint16_t value_high = readw(addr_high);
    return ((uint32_t)value_high << PMU_CMU_REG_MAX) | value_low;
}

uint32_t hal_pmu_err_irq_get_enable_status(pmu_cmu_err_int_type_t int_type)
{
    uint32_t addr_low = 0;
    uint32_t addr_high = 0;
    if (int_type == PMU_CMU_GRM_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG;
    } else if (int_type == PMU_CMU_RAW_INT) {
        addr_low = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_INT_EN_REG;
        addr_high = LP_CTL_GLB_RB_LP_CTL_GLB_RAW_INT_EN_1_REG;
    } else {
        return 0;
    }

    uint16_t value_low = readw(addr_low);
    uint16_t value_high = readw(addr_high);
    return ((uint32_t)value_high << PMU_CMU_REG_MAX) | value_low;
}
