/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  CLOCKS CORE PRIVATE SOURCE.
 */
#include "clocks_switch.h"
#include <stdint.h>
#include "soc_osal.h"
#include "debug_print.h"
#include "pm_pmu.h"
#include "pm_clocks.h"
#include "psram_v2.h"
#if defined(CFG_DRIVERS_MMC)
#include "mmc_porting.h"
#endif
#include "arch/cache.h"
#include "pmu_mem.h"

static uint8_t g_current_0p8_vol = PMU_0P9_VSET_0V9;
static uint8_t g_0p8_vol_vmin = PMU_0P9_VSET_0V8;
static uint8_t g_hifi_mode = HIFI_LPM_CLK_NONE;
static uint8_t g_last_hifi_mode = HIFI_LPM_CLK_NONE;
static uint8_t g_mcu_mode = CLOCKS_M64;
uint32_t g_stack_switch_sp_save = 0;

void set_clock_0p8_vol_vmin(uint8_t vmin)
{
    g_0p8_vol_vmin = vmin;
}

uint8_t get_clock_0p8_vol_vmin(void)
{
    return g_0p8_vol_vmin;
}

static bool judge_and_raise_0p8_voltage(uint8_t need_vol)
{
    uint8_t vol = MAX(need_vol, g_0p8_vol_vmin);
    if (vol > g_current_0p8_vol) {
        errcode_t ret = ERRCODE_SUCC;
        if (g_hifi_mode < HIFI_LPM_CLK_NONE) {  // 判断 DSP 存在, 则降频到 32M 进行后续切 TMODE 操作
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_HIFI, CLOCKS_SRC_XO, 0x1);
        }
        ret |= uapi_pmu_ldo_set_voltage(PMU_BUCK_LDO_BUCK_0P8, vol); // 先升压, 再切TMODE, 再提频率
        uapi_pmu_mem_0p9_set();
        if (g_hifi_mode < HIFI_LPM_CLK_NONE) {
            system_hifi_clk_cfg_t dsp_freq = clocks_system_hifi_freq_get(g_hifi_mode);
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_HIFI, dsp_freq.hifi_clk.clk_src, dsp_freq.hifi_clk.clk_div);
        }
        if (ret != ERRCODE_SUCC) {
            return false;
        }
        g_current_0p8_vol = vol;
    }
    return true;
}

static bool judge_and_reduce_0p8_voltage(uint8_t need_vol)
{
    uint8_t vol = MAX(need_vol, g_0p8_vol_vmin);
    if (vol < g_current_0p8_vol) {
        errcode_t ret = ERRCODE_SUCC;
        if (g_hifi_mode < HIFI_LPM_CLK_NONE) {  // 判断 DSP 存在, 则降频到 32M 进行后续切 TMODE 操作
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_HIFI, CLOCKS_SRC_XO, 0x1);
        }
        uapi_pmu_mem_0p8_set();
        ret |= uapi_pmu_ldo_set_voltage(PMU_BUCK_LDO_BUCK_0P8, vol); // 先降频率，再切TMODE, 再降压

        if (g_hifi_mode < HIFI_LPM_CLK_NONE) {
            system_hifi_clk_cfg_t dsp_freq = clocks_system_hifi_freq_get(g_hifi_mode);
            clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_HIFI, dsp_freq.hifi_clk.clk_src, dsp_freq.hifi_clk.clk_div);
        }
        if (ret != ERRCODE_SUCC) {
            return false;
        }
        g_current_0p8_vol = vol;
    }
    return true;
}

static void clock_config_after_handler(system_mcu_freq_t clk_level)
{
    // psram 自校准重新初始化
    psram_controller_reinit();

    // psram 关闭反压
    uapi_psram_close_cmd_bp(0);

    // emmc 内部时钟配置初始化
#if defined(CFG_DRIVERS_MMC)
    if (clk_level == CLOCKS_M64) {
        emmc_set_clock(32000000UL); // 32M: 32000000UL
    } else {
        emmc_set_clock(100000000UL); // 100M: 100000000UL
    }
#else
    unused(clk_level);
#endif
}

static void clock_config_before_handler(void)
{
    // psram flush cache
    ArchDCacheFlush();

    // psram 反压
    uapi_psram_exec_cmd_bp(0);
}

errcode_t clocks_set_mcu_freq_ram(system_mcu_freq_t clk_level)
{
    if (clk_level == CLOCKS_MCU_FREQ_MAX) {
        return ERRCODE_FAIL;
    }

    uint8_t mcu_need_voltage = clocks_system_mcu_need_vol_get(clk_level);
    uint8_t dsp_need_voltage = clocks_system_hifi_need_vol_get(g_hifi_mode);

    uint8_t need_vol = MAX(mcu_need_voltage, dsp_need_voltage);
    uint32_t irq_status = osal_irq_lock();
    // Raise the voltage first.
    if (!judge_and_raise_0p8_voltage(need_vol)) {
        osal_irq_restore(irq_status);
        return ERRCODE_FAIL;
    }

    clock_config_before_handler();

    // Config the frequency again.
    system_mcu_core_clk_cfg_t mcu_freq = clocks_system_mcu_freq_get(clk_level);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_MCU_CORE, mcu_freq.mcu_clk.clk_src, mcu_freq.mcu_clk.clk_div);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_OPI0, mcu_freq.psram_clk.clk_src, mcu_freq.psram_clk.clk_div);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_SFC, mcu_freq.flash_clk.clk_src, mcu_freq.flash_clk.clk_div);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC, mcu_freq.emmc_clk.clk_src, mcu_freq.emmc_clk.clk_div); // sfc
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SCR_SEC, mcu_freq.sec_clk.clk_src, mcu_freq.sec_clk.clk_div);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_DPU, mcu_freq.vau_clk.clk_src, mcu_freq.vau_clk.clk_div);
#if defined(SUPPORT_GPU_QSPI)
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_QSPI2, mcu_freq.qspi_display_clk.clk_src,
        mcu_freq.qspi_display_clk.clk_div);
#endif

    clock_config_after_handler(clk_level);

    // Reduce the voltage finally.
    if (!judge_and_reduce_0p8_voltage(need_vol)) {
        osal_irq_restore(irq_status);
        return ERRCODE_FAIL;
    }
    g_mcu_mode = clk_level;
    osal_irq_restore(irq_status);
    return ERRCODE_SUCC;
}

errcode_t clocks_set_dsp_freq(pm_hifi_mode_t clk_level)
{
    if (clk_level >= HIFI_LPM_CLK_MAX) {
        return ERRCODE_FAIL;
    }

    uint8_t mcu_need_voltage = clocks_system_mcu_need_vol_get(g_mcu_mode);
    uint8_t dsp_need_voltage = clocks_system_hifi_need_vol_get(clk_level);

    uint8_t need_vol = MAX(mcu_need_voltage, dsp_need_voltage);
    uint32_t irq_status = osal_irq_lock();
    if (!judge_and_raise_0p8_voltage(need_vol)) {
        osal_irq_restore(irq_status);
        return ERRCODE_FAIL;
    }

    system_hifi_clk_cfg_t dsp_freq = clocks_system_hifi_freq_get(clk_level);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_HIFI, dsp_freq.hifi_clk.clk_src, dsp_freq.hifi_clk.clk_div);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_CODEC, dsp_freq.codec_clk.clk_src, dsp_freq.codec_clk.clk_div);
    g_hifi_mode = clk_level;

    // Reduce the voltage finally.
    if (!judge_and_reduce_0p8_voltage(need_vol)) {
        osal_irq_restore(irq_status);
        return ERRCODE_FAIL;
    }
    if (clk_level != HIFI_LPM_CLK_NONE) {
        g_last_hifi_mode = clk_level;
    }
    osal_irq_restore(irq_status);
    return ERRCODE_SUCC;
}

uint8_t clocks_get_dsp_freq_level(void)
{
    return g_hifi_mode;
}

uint8_t clocks_get_last_dsp_freq_level(void)
{
    return g_last_hifi_mode;
}

errcode_t clocks_set_emmc_freq(system_emmc_freq_t clk_level)
{
    if (clk_level >= CLOCKS_EMMC_FREQ_MAX) {
        return ERRCODE_FAIL;
    }
    clocks_clk_cfg_t emmc_freq = clocks_system_emmc_freq_get(clk_level);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_EMMC, emmc_freq.clk_src, emmc_freq.clk_div);
    return ERRCODE_SUCC;
}

errcode_t clocks_set_sdio_freq(system_sdio_freq_t clk_level)
{
    if (clk_level >= CLOCKS_SDIO_FREQ_MAX) {
        return ERRCODE_FAIL;
    }
    clocks_clk_cfg_t sdio_freq = clocks_system_sdio_freq_get(clk_level);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_SDIOM, sdio_freq.clk_src, sdio_freq.clk_div);
    return ERRCODE_SUCC;
}

errcode_t clocks_set_psram_freq(clocks_src_t clk_src, uint8_t clk_div)
{
    if (clk_src >= CLOCKS_SRC_MAX) {
        return ERRCODE_FAIL;
    }
    clock_config_before_handler();
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_XIP_OPI0, clk_src, clk_div);

    // psram 自校准重新初始化
    psram_controller_reinit();

    // psram 关闭反压
    uapi_psram_close_cmd_bp(0);
    return ERRCODE_SUCC;
}
