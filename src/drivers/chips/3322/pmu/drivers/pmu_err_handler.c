/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:    APPLICATION PMU ULP DRIVER.
 */

#include "pmu_err_handler.h"
#include "hal_pmu_interrupt.h"
#include "soc_reg.h"
#include "chip_core_irq.h"
#include "osal_interrupt.h"
#include "arch_port.h"
#include "panic.h"
#include "tcxo.h"
#include "print_config.h"

#define CHECK_COUNT 1000
#define PMU_ERR_TIMEOUT 10000

#define GRM_CFG_TIME 0x10   // 500us
// bit 0-9
#define PMU1_1MS_GRM_CTRL_POS 0
#define PMU1_1MS_GRM_CTRL_LEN 10
// bit 0-5
#define LP_1MS_GRM_CTRL_POS 0
#define LP_1MS_GRM_CTRL_LEN 6

typedef struct {
    uint32_t reg_addr;
    uint16_t bit_pos;
} pmu_cmu_int_reg_t;

const pmu_cmu_int_reg_t g_pmu_cmu_int_reg_config[] = {
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_INTLDO3_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_INTLDO2_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_CODECLDO3_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_CODECLDO1_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_CODECLDO0_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_CLDO2_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_CLDO1_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_D_PMU_ANALDO0_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK2_SCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK2_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK1_SCP_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK1_OCP_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK0_SCP_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_REG, LP_CTL_GLB_RB_BUCK0_OCP_FRM_ULP_GRM_INT_EN_OFFSET},

    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_UVLO_OUT_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_UVLO_FILTER_OUT_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_PALDO0_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_OVP_OUT_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_MICLDO0_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_OCP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO4_OCP_FRM_ULP_GRM_INT_EN_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_GRM_INT_EN_1_REG, LP_CTL_GLB_RB_D_PMU_IOLDO3_OCP_GRM_INT_EN_OFFSET},
};

static void pmu_cmu_err_irq_enable(void)
{
    for (uint32_t i = 0; i < sizeof(g_pmu_cmu_int_reg_config) / sizeof(pmu_cmu_int_reg_t); i++) {
        reg16_setbit(g_pmu_cmu_int_reg_config[i].reg_addr, g_pmu_cmu_int_reg_config[i].bit_pos);
    }
}

const pmu_cmu_int_reg_t g_pmu_cmu_en_reg_config[] = {
    // ovp uvlo en
    {LP_CTL_ULP_A_RB_OVP_ULVO_CTRL_MAN_REG, LP_CTL_ULP_A_RB_ULVO_EN_SEL_OFFSET},
    {LP_CTL_ULP_A_RB_OVP_ULVO_CTRL_MAN_REG, LP_CTL_ULP_A_RB_ULVO_EN_MAN_OFFSET},
    {LP_CTL_ULP_A_RB_OVP_ULVO_CTRL_MAN_REG, LP_CTL_ULP_A_RB_OVP_EN_SEL_OFFSET},
    {LP_CTL_ULP_A_RB_OVP_ULVO_CTRL_MAN_REG, LP_CTL_ULP_A_RB_OVP_EN_MAN_OFFSET},

    // ocp en
    {LP_CTL_GLB_RB_LP_CTL_GLB_DEF_50_REG, LP_CTL_GLB_RB_D_PMU_IOLDO1_EN_OCP_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_DEF_56_REG, LP_CTL_GLB_RB_D_PMU_IOLDO2_EN_OCP_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_DEF_58_REG, LP_CTL_GLB_RB_D_PMU_IOLDO3_EN_OCP_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_DEF_60_REG, LP_CTL_GLB_RB_D_PMU_IOLDO5_EN_OCP_OFFSET},
    {LP_CTL_GLB_RB_LP_CTL_GLB_DEF_62_REG, LP_CTL_GLB_RB_D_PMU_IOLDO6_EN_OCP_OFFSET},
};

static void pmu_cmu_err_en_enable(void)
{
    for (uint32_t i = 0; i < sizeof(g_pmu_cmu_en_reg_config) / sizeof(pmu_cmu_int_reg_t); i++) {
        reg16_setbit(g_pmu_cmu_en_reg_config[i].reg_addr, g_pmu_cmu_en_reg_config[i].bit_pos);
    }

    // 降低滤毛刺时间，配置后buck类过流才可触发
    reg32_setbits(LP_CTL_ULP_B_RB_PMU1_1MS_GRM_CTRL_REG, PMU1_1MS_GRM_CTRL_POS, PMU1_1MS_GRM_CTRL_LEN, GRM_CFG_TIME);
    reg32_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_LP_GRM_REG, LP_1MS_GRM_CTRL_POS, LP_1MS_GRM_CTRL_LEN, GRM_CFG_TIME);
}

static uint32_t pmu_err_get_status(pmu_cmu_err_int_type_t int_type)
{
    return hal_pmu_err_irq_get_status(int_type) & hal_pmu_err_irq_get_enable_status(int_type);
}

static void pmu_err_irq_handler_clear(pmu_cmu_err_int_type_t int_type)
{
    uint64_t time = uapi_tcxo_get_ms();
    uint32_t check_cnt = CHECK_COUNT;
    while (1) {
        if (pmu_err_get_status(int_type) != 0) {
            hal_pmu_err_irq_clear(int_type);
        } else {
            return;
        }

        check_cnt--;
        if (check_cnt == 0) {
            if ((time + PMU_ERR_TIMEOUT) < uapi_tcxo_get_ms()) {  // 超过PMU_ERR_TIMEOUT时间还未恢复
                uint32_t int_status = pmu_err_get_status(int_type);
                oam_err_log("[PMU][ERROR]pmu_err : interrupt type is %d, status is 0x%x\r\n", int_type, int_status);
                panic(PANIC_PMU_CMU, int_status);
            } else {
                check_cnt = CHECK_COUNT;
            }
        }
    }
}

static void pmu_err_irq_handler(void)
{
    uint32_t grm_int_status = pmu_err_get_status(PMU_CMU_GRM_INT);
    if (grm_int_status != 0) {
        oam_err_log("[PMU][ERROR]pmu_err : grm interrupt status is 0x%x\r\n", grm_int_status);
        hal_pmu_err_irq_clear(PMU_CMU_GRM_INT);
        pmu_err_irq_handler_clear(PMU_CMU_GRM_INT);
    }

    uint32_t raw_int_status = pmu_err_get_status(PMU_CMU_RAW_INT);
    if (raw_int_status != 0) {
        oam_err_log("[PMU][ERROR]pmu_err : raw interrupt status is 0x%x\r\n", raw_int_status);
        hal_pmu_err_irq_clear(PMU_CMU_RAW_INT);
        pmu_err_irq_handler_clear(PMU_CMU_RAW_INT);
    }
}

void pmu_err_handler_init(void)
{
    hal_pmu_err_irq_clear(PMU_CMU_GRM_INT);
    pmu_cmu_err_irq_enable();
    pmu_cmu_err_en_enable();

    osal_irq_request(PMU_CMU_ERR_IRQN, (osal_irq_handler)pmu_err_irq_handler, NULL, NULL, NULL);
    osal_irq_set_priority(PMU_CMU_ERR_IRQN, irq_prio(PMU_CMU_ERR_IRQN));
    osal_irq_enable(PMU_CMU_ERR_IRQN);
}

void pmu_err_handler_deinit(void)
{
    osal_irq_disable(PMU_CMU_ERR_IRQN);
    osal_irq_free(PMU_CMU_ERR_IRQN, NULL);
}
