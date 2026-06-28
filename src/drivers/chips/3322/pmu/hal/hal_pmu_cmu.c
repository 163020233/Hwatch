/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Pmu and cmu hal driver source file.
 */

#include "hal_pmu_cmu.h"
#include "platform_core.h"
#include "hal_reg_config.h"

#define PMU_REG_HIGH_16             16
#define PMU_CMU_PLL_LOCK_DELAY_US   150ULL

/*************************PMU PLL config**********************************/
uint16_t hal_pmu_cmu_get_ana_status(void)
{
    return readw(LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_REG);
}

void hal_pmu_cmu_clear_ana_int_status(void)
{
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_RAW_1STICK_CLR_REG, 0xF);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_GRM_1STICK_CLR_REG, 0xF);
}

void hal_pmu_cmu_analog_status_raw_interrupt_enable(uint16_t pos)
{
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_RAW_INT_EN_REG, POS(pos));
}

void hal_pmu_cmu_analog_status_raw_interrupt_disable(uint16_t pos)
{
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_RAW_INT_EN_REG, POS(pos));
}

void hal_pmu_analog_status_grm_interrupt_enable(uint16_t position, switch_type_t on)
{
    if (on != 0) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, POS(position));
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, POS(position));
    }
}
