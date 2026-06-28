/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:    APPLICATION PMU LDO DRIVER.
 */

#include "pmu_ldo.h"
#include "errcode.h"
#include "common_def.h"
#include "tcxo.h"
#include "soc_reg.h"
#include "chip_io.h"
#include "pmu_buck.h"
#if (!defined BUILD_APPLICATION_SSB)
#include "semactrl.h"
#endif

#define PMU_LDO_WAIT_TIMEOUT_US  300ULL

typedef struct {
    uint32_t reg_addr;
    uint16_t reg_off;
    uint16_t delay_us;
} reg_config_t;

static const reg_config_t g_loadswitch_config_reg[] = {
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_0_EN_OFFSET, 120 }, // PMU_LOAD_SWITCH_SW1P8_0
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_1_EN_OFFSET, 120 }, // IOLDO1 SW1 tsensor
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_2_EN_OFFSET, 120 }, // IOLDO1 SW2 efuse
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_3_EN_OFFSET, 120 }, // IOLDO1 SW3 usb 1p8
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW3P3_0_EN_OFFSET, 120 }, // IOLDO3 SW0 usb 3p3
    { LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW0P9_1_EN_OFFSET, 120 },
};

static errcode_t uapi_pmu_loadswitch_config(pmu_ldo_id_t id, uint8_t state)
{
#if (!defined BUILD_APPLICATION_SSB)
    uapi_sema_get(SEMA_0, 0xFFFF);
#endif
    if (state == 0) {
        reg16_clrbit(g_loadswitch_config_reg[id - PMU_LOAD_SWITCH_BASE].reg_addr,
                     g_loadswitch_config_reg[id - PMU_LOAD_SWITCH_BASE].reg_off);
        if (id == PMU_LOAD_SWITCH_SW3P3_0) {
            reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_70_REG, LP_CTL_GLB_RB_D_PMU_LPREF_IPOLY_EN_OFFSET);
        }
    } else {
        if (id == PMU_LOAD_SWITCH_SW3P3_0) {
            reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_70_REG, LP_CTL_GLB_RB_D_PMU_LPREF_IPOLY_EN_OFFSET);
            uapi_tcxo_delay_us(200UL);
        }
        reg16_setbit(g_loadswitch_config_reg[id - PMU_LOAD_SWITCH_BASE].reg_addr,
                     g_loadswitch_config_reg[id - PMU_LOAD_SWITCH_BASE].reg_off);
        uapi_tcxo_delay_us(g_loadswitch_config_reg[id - PMU_LOAD_SWITCH_BASE].delay_us);
    }
#if (!defined BUILD_APPLICATION_SSB)
    uapi_sema_put(SEMA_0);
#endif
    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_ldo_set_sleep_voltage(pmu_ldo_id_t id, uint8_t vset)
{
    if (id <= PMU_BUCK_LDO_END) {
        return uapi_pmu_buck_ldo_set_eco_voltage(id, vset);
    }
    return ERRCODE_SUCC;
}

uint8_t uapi_pmu_ldo_get_sleep_voltage(pmu_ldo_id_t id)
{
    unused(id);
    return ERRCODE_SUCC;
}

static void pmu_ldo_mipildo_0p8_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_48_REG, LP_CTL_GLB_RB_D_PMU_CLDO2_EN_OFFSET);
        uapi_tcxo_delay_us(200ULL);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_48_REG, LP_CTL_GLB_RB_D_PMU_CLDO2_EN_DELAY_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_48_REG, LP_CTL_GLB_RB_D_PMU_CLDO2_EN_OFFSET);
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_48_REG, LP_CTL_GLB_RB_D_PMU_CLDO2_EN_DELAY_OFFSET);
    }
}

static void pmu_ldo_mipildo_1p8_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_60_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_EN_OFFSET);
        uapi_tcxo_delay_us(200ULL);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_60_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_EN_DELAY_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_60_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_EN_OFFSET);
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_60_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_EN_DELAY_OFFSET);
    }
}

static void pmu_ldo_ioldo0_ctl(uint8_t state)
{
    unused(state);
}

static void pmu_ldo_ioldo1_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_MAN_OFFSET);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_MAN_SEL_OFFSET);
        uapi_tcxo_delay_us(PMU_LDO_WAIT_TIMEOUT_US);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_DELAY_MAN_OFFSET);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_DELAY_MAN_SEL_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_MAN_OFFSET);
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_DELAY_MAN_OFFSET);
    }
}

static void pmu_ldo_ioldo2_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_56_REG, LP_CTL_GLB_RB_D_PMU_IOLDO2_EN_OFFSET);
        uapi_tcxo_delay_us(200ULL);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_56_REG, LP_CTL_GLB_RB_D_PMU_IOLDO2_EN_DELAY_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_56_REG, LP_CTL_GLB_RB_D_PMU_IOLDO2_EN_OFFSET);
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_56_REG, LP_CTL_GLB_RB_D_PMU_IOLDO2_EN_DELAY_OFFSET);
    }
}

static void pmu_ldo_ioldo3_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_MAN_SEL_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_DELAY_MAN_SEL_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_MAN_OFFSET);
        uapi_tcxo_delay_us(PMU_LDO_WAIT_TIMEOUT_US);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_DELAY_MAN_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_MAN_OFFSET);
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_IOLDO3_EN_DELAY_MAN_OFFSET);
    }
}

static void pmu_ldo_ioldo4_ctl(uint8_t state)
{
    unused(state);
}

static void pmu_ldo_ioldo6_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_MAN_OFFSET);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_MAN_SEL_OFFSET);
        uapi_tcxo_delay_us(PMU_LDO_WAIT_TIMEOUT_US);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_DELAY_MAN_OFFSET);
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_SEL_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_DELAY_MAN_SEL_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_MAN_OFFSET);
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_MAN_2_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_DELAY_MAN_OFFSET);
    }
}

static void pmu_ldo_clkldo_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_MAN_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_MAN_SEL_OFFSET);
        uapi_tcxo_delay_us(120UL);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_MAN_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_MAN_SEL_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_MAN_OFFSET);
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_MAN_OFFSET);
    }
}

static void pmu_ldo_pll_hvldo_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_SEL_OFFSET);
        uapi_tcxo_delay_us(120UL);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_OFFSET);
        reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_SEL_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_OFFSET);
        reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_OFFSET);
    }
}

static void pmu_ldo_xldo_ctl(uint8_t state)
{
    unused(state);
}

static void pmu_ldo_intldo2_ctl(uint8_t state)
{
    unused(state);
}

static void pmu_ldo_cldo1_ctl(uint8_t state)
{
    if (state == PMU_TURN_ON) {
        reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_2_REG, LP_CTL_GLB_RB_D_PMU_CLDO1_EN_SLP_VAL_OFFSET);
    } else {
        reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_2_REG, LP_CTL_GLB_RB_D_PMU_CLDO1_EN_SLP_VAL_OFFSET);
    }
}

typedef void (*pmu_ldo_control_func)(uint8_t);

static const pmu_ldo_control_func g_pmu_ldo_control_func[] = {
    pmu_ldo_mipildo_0p8_ctl,
    pmu_ldo_mipildo_1p8_ctl,
    pmu_ldo_ioldo0_ctl,         // ioldo0 sysldo > ulpaon
    pmu_ldo_ioldo1_ctl,
    pmu_ldo_ioldo2_ctl,         // auxadc
    pmu_ldo_ioldo3_ctl,
    pmu_ldo_ioldo4_ctl,
    pmu_ldo_ioldo6_ctl,
    pmu_ldo_clkldo_ctl,         // analdo0
    pmu_ldo_pll_hvldo_ctl,      // paldo0
    pmu_ldo_xldo_ctl,
    pmu_ldo_intldo2_ctl,
    pmu_ldo_cldo1_ctl,
};

static errcode_t uapi_pmu_ldo_control(pmu_ldo_id_t id, uint8_t state)
{
    if ((id < PMU_LDO_BASE) || (id > PMU_LDO_END)) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_pmu_ldo_control_func[id - PMU_LDO_BASE] != NULL) {
        g_pmu_ldo_control_func[id - PMU_LDO_BASE](state);
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_ldo_set_state(pmu_ldo_id_t id, uint8_t state)
{
    if (id <= PMU_BUCK_LDO_END) {
        return uapi_pmu_buck_ldo_set_state(id, state);
    }

    if (id <= PMU_LDO_END) {
        return uapi_pmu_ldo_control(id, state);
    }

    if (id <= PMU_LOAD_SWITCH_END) {
        return uapi_pmu_loadswitch_config(id, state);
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_pmu_ldo_set_voltage(pmu_ldo_id_t id, uint8_t vset)
{
    if (id <= PMU_BUCK_LDO_END) {
        return uapi_pmu_buck_ldo_set_voltage(id, vset);
    }
    return ERRCODE_SUCC;
}

uint8_t uapi_pmu_ldo_get_voltage(pmu_ldo_id_t id)
{
    uint8_t vset = 0;
    errcode_t ret = ERRCODE_FAIL;

    if (id <= PMU_BUCK_LDO_END) {
        ret = uapi_pmu_buck_ldo_get_voltage(id, &vset);
    }

    if (ret != ERRCODE_SUCC) {
        return 0;
    }
    return vset;
}

errcode_t uapi_pmu_usb_power_control(uint8_t state)
{
    // Because of glitch leaded by PMU powering up, the PMU_TURN_ON sequence is moved into usb phy initialization procedure.
    if (state != PMU_TURN_ON) {
        uapi_pmu_ldo_set_state(PMU_LOAD_SWITCH_SW3P3_0, PMU_TURN_OFF);

        uapi_pmu_ldo_set_state(PMU_LDO_IOLDO3, PMU_TURN_OFF);

        uapi_pmu_ldo_set_state(PMU_LOAD_SWITCH_SW1P8_3, PMU_TURN_OFF);
    }
    return ERRCODE_SUCC;
}