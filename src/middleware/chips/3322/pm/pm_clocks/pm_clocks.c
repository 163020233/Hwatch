/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm clock driver adapter code
 */

#include "pm_clocks.h"
#include "errcode.h"
#include "chip_io.h"
#include "log_common.h"
#include "log_def_pf.h"
#include "debug_print.h"
#include "clocks_config.h"
#include "soc_osal.h"
#include "pm_veto.h"
#include "cmsis_os.h"
#include "clocks_switch.h"

static hifi_inform_handler g_hifi_inform_callback = NULL;

void uapi_clocks_init(void)
{
    // xo 32M default trim value 0x49, cfg_xo_core_ctrim_man bit 16:31,len=16
    reg32_setbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_36_REG, 16, 16, 0x49);

    // cfg_xo_core_ctrim_sel bit 7
    reg32_setbits(ULP_AON_FUSE_CTL_RB_FUSE_CTL_AUTO_RD_34_REG, 7, 1, 1);
    // 温漂优化 d_pmu1_rsv d_pmu1_trim_rsv
    reg16_setbits(LP_CTL_ULP_A_RB_PMU_RSV_REG_REG, 0x2, 0x1, 0x1);
    reg16_setbits(ULP_FUSE_CTL_RB_PMU1_TRIM_CFG16_REG, 0x4, 0x5, 0x12);

    clocks_ccrg_init();

    uapi_clocks_config_mode(PM_ID_BSP, PM_MODE_PERFORMACE);

    clocks_set_dsp_freq(HIFI_LPM_CLK_NONE);

    // default close sec clk
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_SEC_PKE_CLKEN_OFFSET);
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_SCE_KM_CLKEN_OFFSET);
}

void uapi_clocks_switch(uint8_t id, switch_type_t config)
{
    hal_clocks_app_perips_config((hal_clocks_app_perips_clk_type_t)id, config);
}

static system_mcu_freq_t uapi_pm_mode_type_translation(pm_mode_t mode)
{
    switch (mode) {
        case PM_MODE_LP:
            return CLOCKS_M32;
        case PM_MODE_NORMAL:
            return CLOCKS_M64;
        case PM_MODE_MIDDLE:
            return CLOCKS_M_H0;
        case PM_MODE_PERFORMACE:
            return CLOCKS_M_H1;
        default:
            return CLOCKS_MCU_FREQ_MAX;
    }
}

static uint64_t g_system_clk_mode_mask[PM_MODE_MAX];

static void pm_mode_user_mask_set(pm_veto_id_t uid, pm_mode_t mode)
{
    if (mode < PM_MODE_MAX) {
        g_system_clk_mode_mask[mode] |= (1 << (uint32_t)uid);
    }
}

static void pm_mode_user_mask_clear(pm_veto_id_t uid, pm_mode_t mode)
{
    for (uint32_t i = PM_MODE_LP; i < PM_MODE_MAX; i++) {
        if (i != mode) {
            g_system_clk_mode_mask[i] &= ~(1 << (uint32_t)uid);
        }
    }
}

static pm_mode_t pm_mode_get_current(void)
{
    for (uint32_t i = (PM_MODE_MAX - 1); i >= PM_MODE_LP; i--) {
        if (g_system_clk_mode_mask[i] != 0) {
            return i;
        }
    }

    return PM_MODE_LP;
}

errcode_t uapi_clocks_config_mode(pm_veto_id_t uid, pm_mode_t mode)
{
#define WAIT_TIMEOUT_MS    10

    if ((mode == PM_MODE_NONE) || (mode >= PM_MODE_MAX)) {
        return ERRCODE_FAIL;
    }

    uint32_t loop_index = 0;
    while (uapi_pm_veto_get_info()->veto_counts.sub_counts[PM_ID_MMC]) {
        (void)osDelay(1);
        if (loop_index > WAIT_TIMEOUT_MS) {
            return ERRCODE_FAIL;
        }
        loop_index++;
    }

    uint32_t irq = osal_irq_lock();
    pm_mode_t current_mode = pm_mode_get_current();
    pm_mode_user_mask_set(uid, mode);
    pm_mode_user_mask_clear(uid, mode);
    pm_mode_t expect_mode = pm_mode_get_current();
    if (current_mode == expect_mode) {
        osal_irq_restore(irq);
        return ERRCODE_SUCC;
    }

    system_mcu_freq_t mcu_freq = uapi_pm_mode_type_translation(expect_mode);
    clocks_set_mcu_freq(mcu_freq);

    osal_irq_restore(irq);
    return ERRCODE_SUCC;
}

void uapi_clocks_dfx_get_mode_info(void)
{
    for (uint8_t i = 0; i < PM_VETO_ID_MAX; i++) {
        if ((g_system_clk_mode_mask[PM_MODE_NORMAL] & (1ULL << i)) != 0) {
            osal_printk("uid %d vote 0\n", i);
        }
        if ((g_system_clk_mode_mask[PM_MODE_MIDDLE] & (1ULL << i)) != 0) {
            osal_printk("uid %d vote 1\n", i);
        }
        if ((g_system_clk_mode_mask[PM_MODE_PERFORMACE] & (1ULL << i)) != 0) {
            osal_printk("uid %d vote 2\n", i);
        }
    }
}

pm_hifi_mode_t uapi_pm_get_hifi_mode(void)
{
    return clocks_get_dsp_freq_level();
}

void uapi_register_hifi_inform_callback(hifi_inform_handler callback)
{
    g_hifi_inform_callback = callback;
}

void uapi_set_hifi_mode(pm_hifi_mode_t mode)
{
    clocks_set_dsp_freq(mode);
    PRINT("set hifi mode = %d.\r\n", mode);

    if (reg16_getbits(LP_CTL_AUDIO_RB_LP_CTL_AUDIO_STS_REG, LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_OFFSET,
        LP_CTL_AUDIO_RB_AUDIO_SYS_CUR_STS_LEN) == 0x3) {
        if (g_hifi_inform_callback != NULL) {
            g_hifi_inform_callback();
        }
    }
}

static pm_hifi_freq_t g_hifi_freq;
pm_hifi_freq_t uapi_pm_get_hifi_freq(pm_hifi_mode_t mode)
{
    system_hifi_clk_cfg_t dsp_freq = clocks_system_hifi_freq_get(mode);

    g_hifi_freq.dsp_src = dsp_freq.hifi_clk.clk_src;
    g_hifi_freq.dsp_div = dsp_freq.hifi_clk.clk_div;
    if (g_hifi_freq.dsp_div == 0) {
        g_hifi_freq.dsp_div = 1;
    }
    g_hifi_freq.dsp_freq_val = clocks_ccrg_get_clk_src_frequency(g_hifi_freq.dsp_src) / g_hifi_freq.dsp_div;

    g_hifi_freq.codec_src = dsp_freq.codec_clk.clk_src;
    g_hifi_freq.codec_div = dsp_freq.codec_clk.clk_div;
    if (g_hifi_freq.codec_div == 0) {
        g_hifi_freq.codec_div = 1;
    }
    g_hifi_freq.codec_freq_val = clocks_ccrg_get_clk_src_frequency(g_hifi_freq.codec_src) / g_hifi_freq.codec_div;

    return g_hifi_freq;
}

errcode_t uapi_pm_set_dpu_freq(pm_dpu_mode_t mode)
{
    // just follow mcpu
    unused(mode);
    return ERRCODE_SUCC;
}

errcode_t uapi_pm_set_npu_freq(pm_npu_mode_t mode)
{
    switch (mode) {
        case PM_MODE_NPU_NONE:
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_NPU_CORE, CLOCKS_SRC_MAX, 0x1);
            break;
        case PM_MODE_NPU_LP:
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_NPU_CORE, CLOCKS_SRC_DLL, 0x1);
            break;
        case PM_MODE_NPU_MIDDLE:
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_NPU_CORE, CLOCKS_SRC_PLL_VCO_DIV, 0x3);
            break;
        case PM_MODE_NPU_HIGH_SPEED:
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_NPU_CORE, CLOCKS_SRC_PLL, 0x4);
            break;
        default:
            break;
    }

    return ERRCODE_SUCC;
}
