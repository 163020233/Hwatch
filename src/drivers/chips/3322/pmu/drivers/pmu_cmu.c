/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description:  PMU cmu DRIVER
 */

#include "pmu_cmu.h"
#include "soc_osal.h"
#include "common_def.h"
#include "tcxo.h"
#include "hal_reg_config.h"
#include "arch_barrier.h"
#if !defined(BUILD_APPLICATION_SSB) && !defined(BUILD_APPLICATION_LOADER)
#include "pm_pmu.h"
#endif

#define PMU_OFFSET_16    16

#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
#define PMU_CMU_PLL_CLK 786432000       // 786.432MHz
#else
#define PMU_CMU_PLL_CLK 794000000       // 794M
#endif

typedef struct pll_cfg {
    uint32_t refdiv;
    uint32_t fbdiv;
    uint32_t frac;
    uint8_t postdiv1;
    uint8_t postdiv2;
    uint16_t pll_mode;
    uint32_t cfg_0;
    uint32_t cfg_1;
    uint32_t cfg_2;
    uint32_t cfg_3;
    uint32_t cfg_4;
} pll_cfg_t;

#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
// 786.432
static pll_cfg_t g_pll_cfg = {
    .refdiv = 0x2,
    .fbdiv = 0x18,
    .frac = 0x9374BC,
    .postdiv1 = 0x0,
    .postdiv2 = 0x0,
    .pll_mode = 0x0,
    .cfg_0 = 0x48000004,
    .cfg_1 = 0x00B40000,
    .cfg_2 = 0x20111FA8,
    .cfg_3 = 0x2404FF20,
    .cfg_4 = 0x0802013F,
};
#else
// 794
static pll_cfg_t g_pll_cfg = {
    .refdiv = 0x2,
    .fbdiv = 0x18,
    .frac = 0xD00000,
    .postdiv1 = 0x0,
    .postdiv2 = 0x0,
    .pll_mode = 0x0,
    .cfg_0 = 0x48000004,
    .cfg_1 = 0x00B40000,
    .cfg_2 = 0x20111FA8,
    .cfg_3 = 0x2404FF20,
    .cfg_4 = 0x0802013F,
};
#endif

#define PMU_CMU_PLL_LOCK_DELAY_US               10000ULL

static uint32_t reg_status_check_timeout(uint32_t addr, uint16_t offset, uint8_t on, uint32_t time_us)
{
#define CHECK_REG_DELAY_TIMEOUT_US  10
    uint32_t timeout_us = 0;
    while (reg16_getbit(addr, offset) != on) {
        if (timeout_us > time_us) {
            return ERRCODE_FAIL;
        }
        uapi_tcxo_delay_us(CHECK_REG_DELAY_TIMEOUT_US);
        timeout_us += CHECK_REG_DELAY_TIMEOUT_US;
    }
    return ERRCODE_SUCC;
}

void pmu_cmu_pll_reinit_with_retry(pmu_cmu_core_t pll)
{
#define PLL_UNLOCK_RETRY_TIME      5

    for (uint32_t i = 0; i < PLL_UNLOCK_RETRY_TIME; i++) {
        uint32_t ret = pmu_cmu_pll_reinit(pll);
        if (ret == ERRCODE_SUCC) {
            return;
        } else {
            pmu_cmu_pll_deinit(pll);
        }
    }
}

static void pmu_cmu_pll_reinit_step2(void)
{
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_PD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_FNPLL_PD_MAN_SEL_OFFSET);

    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_REG, LP_CTL_GLB_RB_FNPLL_BYPASS_OFFSET);    /* 12: FNPLL_BYPASS */

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUTPOSTDIVPD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_FNPLL_FOUTPOSTDIVPD_MAN_SEL_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT2XPD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_FNPLL_FOUT2XPD_MAN_SEL_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT4PHASEPD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_FNPLL_FOUT4PHASEPD_MAN_SEL_OFFSET);
}

static void pmu_cmu_pll_reinit_step3(void)
{
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_19_REG, g_pll_cfg.refdiv);                            /* refdiv */

    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_REG, LP_CTL_GLB_RB_FNPLL_FBDIV_OFFSET,
        LP_CTL_GLB_RB_FNPLL_FBDIV_LEN, g_pll_cfg.fbdiv);                                        /* fbdiv */

    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_17_REG, (g_pll_cfg.frac & 0xFFFF));                   /* frac l */
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_18_REG, LP_CTL_GLB_RB_FNPLL_FRAC_RB16_1_OFFSET,
        LP_CTL_GLB_RB_FNPLL_FRAC_RB16_1_LEN, (g_pll_cfg.frac & 0xFF0000) >> PMU_OFFSET_16);     /* frac h */

    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_18_REG, LP_CTL_GLB_RB_FNPLL_POSTDIV1_OFFSET,
        LP_CTL_GLB_RB_FNPLL_POSTDIV1_LEN, g_pll_cfg.postdiv1);                                  /* postdiv1 */
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_18_REG, LP_CTL_GLB_RB_FNPLL_POSTDIV2_OFFSET,
        LP_CTL_GLB_RB_FNPLL_POSTDIV2_LEN, g_pll_cfg.postdiv2);                                  /* postdiv2 */

    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_18_REG, LP_CTL_GLB_RB_FNPLL_PLL_MODE_OFFSET,
        LP_CTL_GLB_RB_FNPLL_PLL_MODE_LEN, g_pll_cfg.pll_mode);                                  /* pll_mode */

    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_1_REG, (g_pll_cfg.cfg_0 & 0xFFFF)); // cfg0
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_2_REG, (g_pll_cfg.cfg_0 & 0xFFFF0000) >> PMU_OFFSET_16);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_3_REG, (g_pll_cfg.cfg_1 & 0xFFFF)); // cfg1
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_4_REG, (g_pll_cfg.cfg_1 & 0xFFFF0000) >> PMU_OFFSET_16);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_5_REG, (g_pll_cfg.cfg_2 & 0xFFFF)); // cfg2
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_6_REG, (g_pll_cfg.cfg_2 & 0xFFFF0000) >> PMU_OFFSET_16);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_7_REG, (g_pll_cfg.cfg_3 & 0xFFFF)); // cfg3
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_8_REG, (g_pll_cfg.cfg_3 & 0xFFFF0000) >> PMU_OFFSET_16);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_9_REG, (g_pll_cfg.cfg_4 & 0xFFFF)); // cfg4
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_10_REG, (g_pll_cfg.cfg_4 & 0xFFFF0000) >> PMU_OFFSET_16);
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_11_REG, 0x4046); // cfg5
    writew(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_12_REG, 0x0000);
    uapi_tcxo_delay_us(1UL);
}

uint32_t pmu_cmu_pll_reinit(pmu_cmu_core_t pll)
{
    unused(pll);

    uint32_t irq = osal_irq_lock();
    /* step 1 clk ldo power on */
#if defined(BUILD_APPLICATION_SSB) || defined(BUILD_APPLICATION_LOADER)
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_MAN_SEL_OFFSET);
    uapi_tcxo_delay_us(200UL);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_ANALDO0_EN_DELAY_MAN_SEL_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_SEL_OFFSET);
    uapi_tcxo_delay_us(200UL);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_SEL_OFFSET);
#else
    /* analdo */
    uapi_pmu_ldo_set_state(PMU_LDO_CLKLDO0, PMU_TURN_ON);

    /* paldo0 */
    uapi_pmu_ldo_set_state(PMU_LDO_PLL_HVLDO, PMU_TURN_ON);
#endif

    /* dcxo */
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT4_EN_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT4_EN_MAN_SEL_OFFSET);

    /* clk iso */
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_CKO_ISO_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_SEL_REG, LP_CTL_MCU_RB_FNPLL_CKO_ISO_MAN_SEL_OFFSET); /* 解iso */

    /* step 2 pd */
    pmu_cmu_pll_reinit_step2();

    /* step 3 */
    pmu_cmu_pll_reinit_step3();

    /* step 4 */
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_PD_MAN_OFFSET);
    uapi_tcxo_delay_us(30UL);

    /* step 5 */
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_CKO_ISO_MAN_OFFSET);
    uint32_t ret = reg_status_check_timeout(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_18_REG, LP_CTL_GLB_RB_FNPLL_LOCK_OFFSET,
        TURN_ON, PMU_CMU_PLL_LOCK_DELAY_US); /* 7: FNPLL_LOCK */
    if (ret != 0) {
        osal_irq_restore(irq);
        return ERRCODE_FAIL;
    }

    /* step 6 */
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUTPOSTDIVPD_MAN_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT2XPD_MAN_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT4PHASEPD_MAN_OFFSET);
    reg16_clrbit(LP_CTL_GLB_RB_LP_CTL_GLB_FNPLL_REG, LP_CTL_GLB_RB_FNPLL_BYPASS_OFFSET); /* 12: FNPLL_BYPASS */

    osal_irq_restore(irq);

    return ERRCODE_SUCC;
}

void pmu_cmu_pll_deinit(pmu_cmu_core_t pll)
{
    unused(pll);

    uint32_t irq = osal_irq_lock();

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_CKO_ISO_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_PD_MAN_OFFSET);

    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUTPOSTDIVPD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT2XPD_MAN_OFFSET);
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_FNPLL_FOUT4PHASEPD_MAN_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_DCXO_XOCLKOUT4_EN_MAN_OFFSET);

#if defined(BUILD_APPLICATION_SSB) || defined(BUILD_APPLICATION_LOADER)
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_OFFSET);

    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_MAN_OFFSET);
    reg16_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_REG, LP_CTL_MCU_RB_D_PMU_PALDO0_EN_DELAY_MAN_OFFSET);
#else
    /* paldo0 */
    uapi_pmu_ldo_set_state(PMU_LDO_PLL_HVLDO, PMU_TURN_OFF);

    /* analdo */
    uapi_pmu_ldo_set_state(PMU_LDO_CLKLDO0, PMU_TURN_OFF);
#endif

    osal_irq_restore(irq);
}

uint32_t pmu_cmu_get_pll_clock_value(void)
{
    return PMU_CMU_PLL_CLK;
}
