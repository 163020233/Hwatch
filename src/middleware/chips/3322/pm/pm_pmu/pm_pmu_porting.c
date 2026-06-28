/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm pmu driver adapter code
 */

#include "pm_pmu.h"
#include "chip_io.h"
#include "platform_core.h"
#include "calendar_porting.h"
#include "pmu_ulp.h"
#include "tcxo.h"
#include "pmu_mem.h"
#include "efuse.h"
#include "debug_print.h"
#ifdef PMU_ERR_HANDLER_ENABLED
#include "pmu_err_handler.h"
#endif

static uint32_t get_ioldo4_calibration_efuse_value(uint32_t *adj_value)
{
#define IOLDO4_ADJ_VAULE (13 / 8)
#define IOLDO4_ADJ_VAULE_OFFSET (0x5)
#define IOLDO4_ADJ_VAULE_LEN (3)
    uint8_t ioldo4_vset = 0;
    uint8_t value = 0;
    for (uint32_t i = 0; i < IOLDO4_ADJ_VAULE_LEN; i++) {
        uint32_t ret = uapi_efuse_read_bit(&ioldo4_vset, IOLDO4_ADJ_VAULE, IOLDO4_ADJ_VAULE_OFFSET + i);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        value = value | (ioldo4_vset << i);
    }
    *adj_value = value;
    return ERRCODE_SUCC;
}
#if ENABLE_LOW_POWER == YES
static void shutdown_bt_ram_and_em(void)
{
    uapi_pmu_mem_control(PMU_MEM_BT_1_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_2_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_3_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_4_RAM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_1_EM, PMU_MEM_SHUT_DOWN);
    uapi_pmu_mem_control(PMU_MEM_BT_2_EM, PMU_MEM_SHUT_DOWN);
    return;
}
#endif
void uapi_pmu_init(void)
{
    uapi_ulp_pmu_init();

    uapi_pmu_buck_opt();

    // clkldo paldo上电等待时间恢复成默认值
    reg16_setbits(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_13_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_TIME_OFFSET,
        LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_TIME_LEN, 0x4);
    reg16_setbits(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_TIME_OFFSET,
        LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_TIME_LEN, 0x4);

    // clkldo paldo 电压
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_D_PMU_ANALDO0_1_REG, LP_CTL_GLB_RB_D_PMU_ANALDO0_VSET_OFFSET,
        LP_CTL_GLB_RB_D_PMU_ANALDO0_VSET_LEN, 0xC); // 0.9v
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_D_PMU_PALDO0_1_REG, LP_CTL_GLB_RB_D_PMU_PALDO0_VSET_OFFSET,
        LP_CTL_GLB_RB_D_PMU_PALDO0_VSET_LEN, 0x8); // 1.8v

    // d_pmu_ioldo3_ictr
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_58_REG, LP_CTL_GLB_RB_D_PMU_IOLDO3_ICTR_OFFSET,
        LP_CTL_GLB_RB_D_PMU_IOLDO3_ICTR_LEN, 0x3);

    // ioldo4 adj
    uint32_t ioldo4_adj;
    uint32_t ret = get_ioldo4_calibration_efuse_value(&ioldo4_adj);
    if (ret == ERRCODE_SUCC) {
        reg16_setbits(LP_CTL_ULP_A_RB_IOLDO4_CFG_REG_REG, LP_CTL_ULP_A_RB_D_PMU_IOLDO4_VSET_OFFSET,
            LP_CTL_ULP_A_RB_D_PMU_IOLDO4_VSET_LEN, ioldo4_adj);
    } else {
        PRINT("get ioldo4 adj fail,:%x.", ret);
    }
#if ENABLE_LOW_POWER == YES
    // io en
    uapi_pmu_ldo_set_state(PMU_LDO_IOLDO1, PMU_TURN_ON);
    uapi_pmu_ldo_set_state(PMU_LDO_CLDO1, PMU_TURN_ON);

    uapi_pmu_ldo_set_state(PMU_BUCK_LDO_BUCK_1P9, PMU_BUCK_LDO_STATE_ECO);
    uapi_pmu_ldo_set_state(PMU_BUCK_LDO_BUCK_0P8, PMU_BUCK_LDO_STATE_ECO);
    uapi_pmu_ldo_set_state(PMU_BUCK_LDO_BUCK_1P0, PMU_BUCK_LDO_STATE_ECO);

    uapi_pmu_ldo_set_sleep_voltage(PMU_BUCK_LDO_BUCK_1P9, PMU_1P9_VSET_1V9);
    uapi_pmu_ldo_set_sleep_voltage(PMU_BUCK_LDO_BUCK_0P8, PMU_0P9_VSET_0V7_25);
    uapi_pmu_ldo_set_sleep_voltage(PMU_BUCK_LDO_BUCK_1P0, PMU_1P0_VSET_1V0);

    // ioldo1 睡眠不进直通, buck1p9不降压
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_SW_SLP_VAL_OFFSET);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_54_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_VSET_SLP_OFFSET,
                  LP_CTL_GLB_RB_D_PMU_IOLDO1_VSET_SLP_LEN, 0x8);

    // ioldo6 睡眠不进直通, buck1p9不降压
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_3_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_SW_SLP_VAL_OFFSET);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_66_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_VSET_SLP_OFFSET,
                  LP_CTL_GLB_RB_D_PMU_IOLDO6_VSET_SLP_LEN, 0x8);

    // cldo1 睡眠不进直通, buck1p0不降压
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_2_REG, LP_CTL_GLB_RB_D_PMU_CLDO1_EN_SW_SLP_VAL_OFFSET);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_46_REG, LP_CTL_GLB_RB_D_PMU_CLDO1_VSET_SLP_OFFSET,
                  LP_CTL_GLB_RB_D_PMU_CLDO1_VSET_SLP_LEN, 0x7);

    // buck2 sleep slp val
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_BUCK2_EN_SLP_VAL_OFFSET);

    // sysldo 0.75v
    reg16_setbits(LP_CTL_ULP_A_RB_SYSLDO_VSET_CFG_REG_REG, LP_CTL_ULP_A_RB_D_PMU_SYSLDO_VSET_OFFSET,
                  LP_CTL_ULP_A_RB_D_PMU_SYSLDO_VSET_LEN, 0x4);
    shutdown_bt_ram_and_em();
#endif

#ifdef PMU_ERR_HANDLER_ENABLED
    pmu_err_handler_init();
#endif
}
