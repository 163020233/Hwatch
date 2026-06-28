/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:   PMU Buck DRIVER
 */

#include "pmu_buck.h"
#include "common_def.h"
#include "soc_reg.h"
#include "pm_pmu_porting.h"
#include "chip_io.h"
#include "tcxo.h"
#include "soc_osal.h"

#define BUCK_VSET_WAIT_TIMEOUT_MS  10ULL
#define BUCK_VSET_WAIT_TIMEOUT_US  300ULL

typedef struct {
    uint32_t reg_addr;
    uint16_t reg_off;
    uint16_t reg_len;
} reg_config_t;

static const reg_config_t g_buck_sleep_eco_reg[] = {
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_12_REG, LP_CTL_GLB_RB_BUCK0_VSET_SLP_DISABLE_OFFSET, 1},
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_24_REG, LP_CTL_GLB_RB_BUCK1_VSET_SLP_DISABLE_OFFSET, 1},
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_37_REG, LP_CTL_GLB_RB_BUCK2_VSET_SLP_DISABLE_OFFSET, 1},
};

static const reg_config_t g_buck_clamp_sel_reg[] = {
    { LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_BUCK0_CLAMP_SEL_SLP_VAL_OFFSET, 1},
    { LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_BUCK1_CLAMP_SEL_SLP_VAL_OFFSET, 1},
    { LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_BUCK2_CLAMP_SEL_SLP_VAL_OFFSET, 1},
};

errcode_t uapi_pmu_buck_ldo_set_state(uint32_t bus, uint8_t state)
{
    switch (state) {
        case PMU_BUCK_LDO_STATE_ON:
            reg16_clrbit(g_buck_clamp_sel_reg[bus].reg_addr, g_buck_clamp_sel_reg[bus].reg_off); // disable sleep
            reg16_setbit(g_buck_sleep_eco_reg[bus].reg_addr, g_buck_sleep_eco_reg[bus].reg_off); // disable sleep
            break;
        case PMU_BUCK_LDO_STATE_ECO:
            reg16_setbit(g_buck_clamp_sel_reg[bus].reg_addr, g_buck_clamp_sel_reg[bus].reg_off); // enable sleep
            reg16_clrbit(g_buck_sleep_eco_reg[bus].reg_addr, g_buck_sleep_eco_reg[bus].reg_off); // enable sleep
            break;
        default:
            break;
    }
    return ERRCODE_SUCC;
}

static errcode_t pmu_buck_ldo_0p8_vset_adj(uint8_t voltage)
{
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_25_REG,
                  LP_CTL_GLB_RB_BUCK1_VSET_WORK_OFFSET,
                  LP_CTL_GLB_RB_BUCK1_VSET_WORK_LEN,
                  voltage);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_12_REG, LP_CTL_MCU_RB_BUCK1_VSET_MCU_REQ_OFFSET);

    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_25_REG, LP_CTL_GLB_RB_BUCK1_VSET_STEP_OFFSET,
                  LP_CTL_GLB_RB_BUCK1_VSET_STEP_LEN, 1);

    uapi_tcxo_delay_us(BUCK_VSET_WAIT_TIMEOUT_US);

    uint64_t start_time = uapi_tcxo_get_ms();
    while (reg16_getbits(LP_CTL_GLB_RB_LP_CTL_GLB_STS_1_REG,
                         LP_CTL_GLB_RB_BUCK1_VSET_STS_OFFSET,
                         LP_CTL_GLB_RB_BUCK1_VSET_STS_LEN) != voltage) {
        if (uapi_tcxo_get_ms() > start_time + BUCK_VSET_WAIT_TIMEOUT_MS) {
            osal_printk("0p8 vset 0x%x set fail\r\n", voltage);
            break;
        }
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_buck_ldo_set_voltage(uint32_t bus, uint8_t voltage)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t irq_sts = osal_irq_lock();
    switch (bus) {
        case PMU_BUCK_LDO_BUCK_0P8:
            ret = pmu_buck_ldo_0p8_vset_adj(voltage);
            break;
        default:
            break;
    }
    osal_irq_restore(irq_sts);
    return ret;
}

static const reg_config_t g_buck_current_vset_reg[] = {
    { LP_CTL_GLB_RB_LP_CTL_GLB_STS_1_REG, LP_CTL_GLB_RB_BUCK0_VSET_STS_OFFSET, LP_CTL_GLB_RB_BUCK0_VSET_STS_LEN},
    { LP_CTL_GLB_RB_LP_CTL_GLB_STS_1_REG, LP_CTL_GLB_RB_BUCK1_VSET_STS_OFFSET, LP_CTL_GLB_RB_BUCK1_VSET_STS_LEN},
    { LP_CTL_GLB_RB_LP_CTL_GLB_STS_2_REG, LP_CTL_GLB_RB_BUCK2_VSET_STS_OFFSET, LP_CTL_GLB_RB_BUCK2_VSET_STS_LEN},
};

errcode_t uapi_pmu_buck_ldo_get_voltage(uint32_t bus, uint8_t *voltage)
{
    if (bus > PMU_BUCK_LDO_END) {
        return ERRCODE_INVALID_PARAM;
    }
    *voltage = reg16_getbits(g_buck_current_vset_reg[bus].reg_addr,
                             g_buck_current_vset_reg[bus].reg_off,
                             g_buck_current_vset_reg[bus].reg_len);
    return ERRCODE_SUCC;
}

static const reg_config_t g_buck_sleep_vset_reg[] = {
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_12_REG, LP_CTL_GLB_RB_BUCK0_VSET_SLP_OFFSET, LP_CTL_GLB_RB_BUCK0_VSET_SLP_LEN},
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_24_REG, LP_CTL_GLB_RB_BUCK1_VSET_SLP_OFFSET, LP_CTL_GLB_RB_BUCK1_VSET_SLP_LEN},
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_37_REG, LP_CTL_GLB_RB_BUCK2_VSET_SLP_OFFSET, LP_CTL_GLB_RB_BUCK2_VSET_SLP_LEN},
};

errcode_t uapi_pmu_buck_ldo_set_eco_voltage(uint32_t bus, uint8_t voltage)
{
    if (bus > PMU_BUCK_LDO_END) {
        return ERRCODE_INVALID_PARAM;
    }
    reg16_setbits(g_buck_sleep_vset_reg[bus].reg_addr, g_buck_sleep_vset_reg[bus].reg_off,
                  g_buck_sleep_vset_reg[bus].reg_len, voltage);
    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_buck_ldo_get_eco_voltage(uint32_t bus, uint8_t *voltage)
{
    if (bus > PMU_BUCK_LDO_END) {
        return ERRCODE_INVALID_PARAM;
    }
    *voltage = reg16_getbits(g_buck_sleep_vset_reg[bus].reg_addr, g_buck_sleep_vset_reg[bus].reg_off,
                             g_buck_sleep_vset_reg[bus].reg_len);
    return ERRCODE_SUCC;
}

static void uapi_pmu_buck2_opt(void)
{
    // buck 2
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_33_REG, LP_CTL_GLB_RB_BUCK2_OCP_PERIOD_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_OCP_PERIOD_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_33_REG, LP_CTL_GLB_RB_BUCK2_OCP_LOWPOWER_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_OCP_LOWPOWER_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_37_REG, LP_CTL_GLB_RB_BUCK2_TON_MIN_OFFSET,
        LP_CTL_GLB_RB_BUCK2_TON_MIN_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_27_REG, LP_CTL_GLB_RB_BUCK2_CMP_DSEL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_CMP_DSEL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_33_REG, LP_CTL_GLB_RB_BUCK2_NDR_SEL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_NDR_SEL_LEN, 0xA);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_29_REG, LP_CTL_GLB_RB_BUCK2_DMD_NOR_CMP_ISEL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_DMD_NOR_CMP_ISEL_LEN, 0x7);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_34_REG, LP_CTL_GLB_RB_BUCK2_PDR_SEL_OFFSET,
        LP_CTL_GLB_RB_BUCK2_PDR_SEL_LEN, 0xA);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_30_REG, LP_CTL_GLB_RB_BUCK2_DT_H_OFFSET, LP_CTL_GLB_RB_BUCK2_DT_H_LEN,
        0xF);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_35_REG, 0x2, 0x1, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_28_REG, LP_CTL_GLB_RB_BUCK2_DMD_ECO_PERIOD_SHIELD_OFFSET,
        LP_CTL_GLB_RB_BUCK2_DMD_ECO_PERIOD_SHIELD_LEN, 0x1);
}

static void uapi_pmu_buck1_opt(void)
{
    // buck 1
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_21_REG, LP_CTL_GLB_RB_BUCK1_OCP_LOWPOWER_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK1_OCP_LOWPOWER_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_21_REG, LP_CTL_GLB_RB_BUCK1_OCP_PERIOD_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK1_OCP_PERIOD_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_24_REG, LP_CTL_GLB_RB_BUCK1_TON_MIN_OFFSET,
        LP_CTL_GLB_RB_BUCK1_TON_MIN_LEN, 0x3);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_17_REG, LP_CTL_GLB_RB_BUCK1_DMD_NOR_CMP_AZ_OFFSET,
        LP_CTL_GLB_RB_BUCK1_DMD_NOR_CMP_AZ_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_17_REG, LP_CTL_GLB_RB_BUCK1_DMD_NOR_CMP_ISEL_OFFSET,
        LP_CTL_GLB_RB_BUCK1_DMD_NOR_CMP_ISEL_LEN, 0x7);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_21_REG, LP_CTL_GLB_RB_BUCK1_PDR_SEL_OFFSET,
        LP_CTL_GLB_RB_BUCK1_PDR_SEL_LEN, 0xa);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_20_REG, LP_CTL_GLB_RB_BUCK1_NDR_SEL_OFFSET,
        LP_CTL_GLB_RB_BUCK1_NDR_SEL_LEN, 0xa);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_18_REG, LP_CTL_GLB_RB_BUCK1_DT_H_OFFSET, LP_CTL_GLB_RB_BUCK1_DT_H_LEN,
        0xf);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_22_REG, 0x2, 0x1, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_16_REG, LP_CTL_GLB_RB_BUCK1_DMD_ECO_PERIOD_SHIELD_OFFSET,
        LP_CTL_GLB_RB_BUCK1_DMD_ECO_PERIOD_SHIELD_LEN, 0x1);
}

static void uapi_pmu_buck0_opt(void)
{
    // buck 0
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_9_REG, LP_CTL_GLB_RB_BUCK0_OCP_LOWPOWER_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK0_OCP_LOWPOWER_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_9_REG, LP_CTL_GLB_RB_BUCK0_OCP_PERIOD_CTRL_OFFSET,
        LP_CTL_GLB_RB_BUCK0_OCP_PERIOD_CTRL_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_12_REG, LP_CTL_GLB_RB_BUCK0_TON_MIN_OFFSET,
        LP_CTL_GLB_RB_BUCK0_TON_MIN_LEN, 0x3);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_4_REG, LP_CTL_GLB_RB_BUCK0_DMD_NOR_CMP_AZ_OFFSET,
        LP_CTL_GLB_RB_BUCK0_DMD_NOR_CMP_AZ_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_9_REG, LP_CTL_GLB_RB_BUCK0_PDR_SEL_OFFSET,
        LP_CTL_GLB_RB_BUCK0_PDR_SEL_LEN, 0xa);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_6_REG, LP_CTL_GLB_RB_BUCK0_DT_H_OFFSET, LP_CTL_GLB_RB_BUCK0_DT_H_LEN,
        0xf);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_5_REG, LP_CTL_GLB_RB_BUCK0_DMD_NOR_CMP_ISEL_OFFSET,
        LP_CTL_GLB_RB_BUCK0_DMD_NOR_CMP_ISEL_LEN, 0xF);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_4_REG, LP_CTL_GLB_RB_BUCK0_DMD_ECO_PERIOD_SHIELD_OFFSET,
        LP_CTL_GLB_RB_BUCK0_DMD_ECO_PERIOD_SHIELD_LEN, 0x1);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_10_REG, 0x2, 0x1, 0x1);
}

void uapi_pmu_buck_opt(void)
{
    uapi_pmu_buck2_opt();
    uapi_pmu_buck1_opt();
    uapi_pmu_buck0_opt();

    // RC32k纹波优化 电压检测使能开关
    reg16_setbits(GLB_CTL_M_RB_BUCK_OPT_CTRL0_REG, GLB_CTL_M_RB_VOL_TEMP_SENSE_N_OFFSET,
                  GLB_CTL_M_RB_VOL_TEMP_SENSE_N_LEN, 0x1);
    reg16_setbits(GLB_CTL_M_RB_BUCK_OPT_CTRL0_REG, GLB_CTL_M_RB_VOL_DECT_OFFSET,
                  GLB_CTL_M_RB_VOL_DECT_LEN, 0x1);
}
