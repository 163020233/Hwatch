/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: SYSTEM CLOCKS SWITCH DRIVER
 *
 * Create: 2020-7-28
 */
#include "clocks_ccrg.h"
#include "common_def.h"
#include "hal_clocks_app.h"
#include "clocks_config.h"
#include "panic.h"
#include "soc_osal.h"
#include "platform.h"
#include "hal_tcxo.h"
#include "pmu_sub_control.h"

static uint8_t g_pll_used_vote_count[CLOCKS_PLL_SRC_MAX] = { 0 };
static clocks_clk_cfg_t g_current_clocks[HAL_CLOCKS_MODULE_MAX] = { 0 };
static clocks_clk_cfg_t g_pm_resume_clock[HAL_CLOCKS_MODULE_MAX];

static clocks_pll_src_t clocks_pll_veto_type_translation(clocks_src_t clk_src)
{
    switch (clk_src) {
        case CLOCKS_SRC_DLL:
            return CLOCKS_PLL_SRC_DLL;
        case CLOCKS_SRC_PLL:
            return CLOCKS_PLL_SRC_FNPLL;
        case CLOCKS_SRC_PLL_VCO_DIV:
            return CLOCKS_PLL_SRC_FNPLL_VCO;
        default:
            return CLOCKS_PLL_SRC_MAX;
    }
}

static void clocks_pll_add_vote(clocks_pll_src_t pll_src)
{
    switch (pll_src) {
        case CLOCKS_PLL_SRC_DLL:
            if (g_pll_used_vote_count[pll_src] == 0) {
                hal_clocks_pll_module_clken(CLOCKS_PLL_SRC_DLL, TURN_ON);
            }
            break;
        case CLOCKS_PLL_SRC_FNPLL:
        case CLOCKS_PLL_SRC_FNPLL_VCO:
            if ((g_pll_used_vote_count[CLOCKS_PLL_SRC_FNPLL] == 0) &&
                (g_pll_used_vote_count[CLOCKS_PLL_SRC_FNPLL_VCO] == 0)) {
                pmu_cmu_pll_reinit_with_retry(PMU_CMU_FNPLL);
            }
            if (g_pll_used_vote_count[pll_src] == 0) {
                hal_clocks_pll_module_clken(pll_src, TURN_ON);
            }
            break;
        default:
            return;
    }
    g_pll_used_vote_count[pll_src]++;
}

static void clocks_pll_remove_vote(clocks_pll_src_t pll_src)
{
    if (pll_src >= CLOCKS_PLL_SRC_MAX) {
        return;
    }
    g_pll_used_vote_count[pll_src]--;
    switch (pll_src) {
        case CLOCKS_PLL_SRC_DLL:
            if (g_pll_used_vote_count[pll_src] == 0) {
                hal_clocks_pll_module_clken(CLOCKS_PLL_SRC_DLL, TURN_OFF);
            }
            break;
        case CLOCKS_PLL_SRC_FNPLL:
        case CLOCKS_PLL_SRC_FNPLL_VCO:
            if (g_pll_used_vote_count[pll_src] == 0) {
                hal_clocks_pll_module_clken(pll_src, TURN_OFF);
            }
            if ((g_pll_used_vote_count[CLOCKS_PLL_SRC_FNPLL] == 0) &&
                (g_pll_used_vote_count[CLOCKS_PLL_SRC_FNPLL_VCO] == 0)) {
                pmu_cmu_pll_deinit(PMU_CMU_FNPLL);
            }
            break;
        default:
            return;
    }
}

static hal_clocks_ccrg_clk_src_t clocks_src_sel_type_translation(clocks_ccrg_module_t module, clocks_src_t clk_src)
{
    if ((module == HAL_CLOCKS_MODULE_CAN) &&
        ((clk_src != CLOCKS_SRC_PLL) && (clk_src != CLOCKS_SRC_XO))) {
        return HAL_CLOCKS_CCRG_CLK_SRC_MAX;
    }

    if ((clk_src == CLOCKS_SRC_32K) &&
        ((module != HAL_CLOCKS_MODULE_PAD_OUT0) && (module != HAL_CLOCKS_MODULE_PAD_OUT1))) {
        return HAL_CLOCKS_CCRG_CLK_SRC_MAX;
    }

    return (hal_clocks_ccrg_clk_src_t)clk_src;
}

static void clocks_ccrg_set_freqency_handler(clocks_ccrg_module_t module,
                                             hal_clocks_ccrg_clk_src_t clk_src, uint8_t clk_div)
{
    if (g_current_clocks[module].clk_src != (clocks_src_t)clk_src) {
        // Add vote.
        clocks_pll_src_t pll_src = clocks_pll_veto_type_translation((clocks_src_t)clk_src);
        clocks_pll_add_vote(pll_src);
        // Config the clock.
        hal_clocks_ccrg_switch(module, clk_src, clk_div);
        // Remove vote.
        pll_src = clocks_pll_veto_type_translation(g_current_clocks[module].clk_src);
        clocks_pll_remove_vote(pll_src);
        g_current_clocks[module].clk_src = (clocks_src_t)clk_src;
    } else {
        // Config the clock.
        hal_clocks_ccrg_switch(module, clk_src, clk_div);
    }
    g_current_clocks[module].clk_div = clk_div;
}

void clocks_ccrg_set_frequency(clocks_ccrg_module_t module, clocks_src_t clk_src, uint8_t clk_div)
{
    if ((g_current_clocks[module].clk_src == clk_src) && (g_current_clocks[module].clk_div == clk_div)) {
        return;
    }
    hal_clocks_ccrg_clk_src_t src_sel;
    // When clk_src equals CLOCKS_SRC_MAX, the clock will be disable the clock.
    if (clk_src == CLOCKS_SRC_MAX) {
        src_sel = HAL_CLOCKS_CCRG_CLK_SRC_MAX;
    } else {
        src_sel = clocks_src_sel_type_translation(module, clk_src);
        // Invalid clock source.
        if (src_sel == HAL_CLOCKS_CCRG_CLK_SRC_MAX) {
            panic(PANIC_CLOCKS_SWITCH, module);
        }
    }
    uint32_t irq_status = osal_irq_lock();
    clocks_ccrg_set_freqency_handler(module, src_sel, clk_div);
    osal_irq_restore(irq_status);
}

uint32_t clocks_ccrg_get_clk_src_frequency(clocks_src_t src)
{
#define XO_32X_CLK    32768ULL
#define XO_1X_CLK     32000000ULL         // System XO 1X 32M clock.
#define XO_2X_CLK     64000000ULL         // System XO 2X 64M clock.

    uint32_t clk_freq = 0;
    uint32_t pll_div = 1;
    switch (src) {
        case CLOCKS_SRC_XO:
            clk_freq = XO_1X_CLK;
            break;
        case CLOCKS_SRC_DLL:
            clk_freq = XO_2X_CLK;
            break;
        case CLOCKS_SRC_PLL:
            clk_freq = pmu_cmu_get_pll_clock_value();
            break;
        case CLOCKS_SRC_PLL_VCO_DIV:
            pll_div = reg16_getbits(COM_CTL_RB_C_CRG_DIV_REG, COM_CTL_RB_CFG_COM_FNPLL_VCO_DIV_OFFSET,
                COM_CTL_RB_CFG_COM_FNPLL_VCO_DIV_LEN);
            pll_div = MAX(1, pll_div);
            clk_freq = pmu_cmu_get_pll_clock_value() / pll_div;
            break;
        case CLOCKS_SRC_32K:
            clk_freq = XO_32X_CLK;
            break;
        case CLOCKS_SRC_PAD_CLKIN:
        case CLOCKS_SRC_MAX:
            break;
    }
    return clk_freq;
}

uint32_t clocks_ccrg_get_frequency(clocks_ccrg_module_t module)
{
    clocks_src_t clk_src = (clocks_src_t)hal_clocks_ccrg_module_get_src((hal_clocks_ccrg_module_t)module);
    uint8_t clk_div = hal_clocks_ccrg_module_get_div(module);
    uint32_t clk_freq = clocks_ccrg_get_clk_src_frequency(clk_src);

    if (clk_div > 1) {
        clk_freq = clk_freq / clk_div;
    }
    return clk_freq;
}

void clocks_ccrg_init(void)
{
    uint32_t irq_status = osal_irq_lock();
    for (uint32_t i = 0; i < HAL_CLOCKS_MODULE_MAX; i++) {
        g_current_clocks[i].clk_src = (clocks_src_t)hal_clocks_ccrg_module_get_src((hal_clocks_ccrg_module_t)i);
        g_current_clocks[i].clk_div = hal_clocks_ccrg_module_get_div((hal_clocks_ccrg_module_t)i);
        g_pm_resume_clock[i].clk_src = CLOCKS_SRC_MAX;

        clocks_pll_src_t pll_src = clocks_pll_veto_type_translation(g_current_clocks[i].clk_src);
        if (pll_src != CLOCKS_PLL_SRC_MAX) {
            g_pll_used_vote_count[pll_src]++;
        }
    }
    osal_irq_restore(irq_status);
}

#define XO_RC_32M_WAIT_MAX_COUNTS 1000
void clocks_suspend(void)
{
    for (uint32_t i = HAL_CLOCKS_MODULE_MCU_PERP_LS; i < HAL_CLOCKS_MODULE_MAX; i++) {
        g_pm_resume_clock[i].clk_src = g_current_clocks[i].clk_src;
        g_pm_resume_clock[i].clk_div = g_current_clocks[i].clk_div;
        // com bus不改变时钟；hifi和codec在dsp打开时跳过不进行配置
        if ((i == HAL_CLOCKS_MODULE_COM_BUS) || (((i == HAL_CLOCKS_MODULE_HIFI) || (i == HAL_CLOCKS_MODULE_CODEC)) &&
            (get_pmu_audio_poweron_flag() == 1))) {
            continue;
        }
        clocks_ccrg_set_freqency_handler((clocks_ccrg_module_t)i, HAL_CLOCKS_CCRG_CLK_SRC_MAX, 1);
    }

    if (g_pm_resume_clock[0].clk_src >= CLOCKS_SRC_DLL) {
        g_pm_resume_clock[0].clk_src = g_current_clocks[0].clk_src;
        g_pm_resume_clock[0].clk_div = g_current_clocks[0].clk_div;
        clocks_ccrg_set_freqency_handler(HAL_CLOCKS_MODULE_MCU_CORE, HAL_CLOCKS_CCRG_CLK_SRC_XO_RC_32M, 1);
    }
    // dsp power on 不切rc 32M, rc 32k场景bt需要校准，也不切
    if (get_pmu_audio_poweron_flag() == 0 &&
        reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) == 1) {
        // 切rc 32M
        reg16_setbit(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG, GLB_CTL_M_RB_CFG_GLBM_XO_RC_SEL_OFFSET);
        // 打开XO检测起振功能
        writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_3_REG, 0xF038);
        // aon 切rc 32M 之后读sts, delay
        for (uint32_t i = 0; i < XO_RC_32M_WAIT_MAX_COUNTS; i++) {
            uint16_t val = readw(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG);
            if (((val & GLB_CTL_M_RB_CFG_GLBM_A32M_MX_CLKA_STS_MASK) == 0) &&
                ((val & GLB_CTL_M_RB_CFG_GLBM_A32M_MX_CLKB_STS_MASK) != 0)) {
                break;
            }
        }
    }
}

void clocks_resume(void)
{
    if (get_pmu_audio_poweron_flag() == 0 &&
        reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) == 1) {
        // max 最大4ms
#define XO_32M_DET_THRESHOLD (8 * 4 * 1000)
        // 回读XO START STOP信号
        uint64_t detct_start = hal_tcxo_get();
        while (1) {
            if (reg16_getbit(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG, 0xF) == 1 &&
                reg16_getbit(PMU2_CMU_CTL_RB_PMU2_GP_REG0_REG, 0xE) == 1) {
                break;
            }
            if (hal_tcxo_get() > (detct_start + XO_32M_DET_THRESHOLD)) {
                writew(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_16_REG, 0x1);
                break;
            }
        }

        // aon sw to xo 32m clock in no trim chip
        reg16_clrbit(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG, GLB_CTL_M_RB_CFG_GLBM_XO_RC_SEL_OFFSET);

        // aon 切xo 32M 之后读sts, delay
        for (uint32_t i = 0; i < XO_RC_32M_WAIT_MAX_COUNTS; i++) {
            uint16_t val = readw(GLB_CTL_M_RB_CLK_32K_32M_EN_SEL_REG);
            if (((val & GLB_CTL_M_RB_CFG_GLBM_A32M_MX_CLKA_STS_MASK) != 0) &&
                ((val & GLB_CTL_M_RB_CFG_GLBM_A32M_MX_CLKB_STS_MASK) == 0)) {
                break;
            }
        }

        // 关闭XO检测起振功能
        writew(LP_CTL_GLB_RB_LP_CTL_GLB_DCXO_3_REG, 0);
    }

    for (uint32_t i = 0; i < HAL_CLOCKS_MODULE_MAX; i++) {
        // com bus不恢复；hifi和codec在dsp打开时跳过不恢复配置
        if ((i == HAL_CLOCKS_MODULE_COM_BUS) || (((i == HAL_CLOCKS_MODULE_HIFI) || (i == HAL_CLOCKS_MODULE_CODEC)) &&
            (get_pmu_audio_poweron_flag() == 1))) {
            continue;
        }
        g_current_clocks[i].clk_src = CLOCKS_SRC_PAD_CLKIN; // set invalid clk src
        clocks_ccrg_set_freqency_handler((clocks_ccrg_module_t)i,
            (hal_clocks_ccrg_clk_src_t)g_pm_resume_clock[i].clk_src, g_pm_resume_clock[i].clk_div);
        g_pm_resume_clock[i].clk_src = CLOCKS_SRC_MAX;
    }
}