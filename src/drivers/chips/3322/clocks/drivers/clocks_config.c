/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  CLOCKS CONFIG PRIVATE SOURCE.
 */
#include "clocks_config.h"
#include "pmu_buck.h"

#define CLOCK_DIV_NONE  0
#define CLOCK_DIV_0     0
#define CLOCK_DIV_1     1
#define CLOCK_DIV_2     2
#define CLOCK_DIV_3     3
#define CLOCK_DIV_4     4
#define CLOCK_DIV_5     5
#define CLOCK_DIV_6     6
#define CLOCK_DIV_7     7
#define CLOCK_DIV_8     8
#define CLOCK_DIV_9     9
#define CLOCK_DIV_10    10
#define CLOCK_DIV_11    11
#define CLOCK_DIV_12    12
#define CLOCK_DIV_13    13
#define CLOCK_DIV_14    14
#define CLOCK_DIV_15    15
#define CLOCK_DIV_16    16
#define CLOCK_DIV_17    17
#define CLOCK_DIV_18    18
#define CLOCK_DIV_19    19
#define CLOCK_DIV_20    20
#define CLOCK_DIV_32    32


#ifndef BUILD_APPLICATION_SSB
/************************** System clock Config ***********************/
static const clocks_clk_cfg_t g_system_all_core_clocks[CLOCKS_CONFIG_MAX][HAL_CLOCKS_MODULE_MAX] = {
    /* All 64M */
    {
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_MCU_CORE
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_LS
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_UART
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_SPI
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_COM_BUS
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_SCR_SEC,
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_SDIOM       sdiom/vicap
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_EMMC        emmc/fmc
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_CAN         only use 20M/40M clock
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_USB_BUS     only work in pll
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_HIFI
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_CODEC
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_XIP_SFC
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_XIP_OPI0
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_DPU, dpu/jpge/h264/vau, 跟随MCU高频
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_XIP_QSPI2, default 50MHz(IO) * 2, 跟随亮灭屏流程
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_XIP_NPU_CORE, default work in pll, 跟随npu上下电流程
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_PAD_OUT0
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_PAD_OUT1
    },
    /* PLL */
    {
        { CLOCKS_SRC_PLL, CLOCK_DIV_3 },       // HAL_CLOCKS_MODULE_MCU_CORE       266.67MHz
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_LS
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_UART
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_MCU_PERP_SPI
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_COM_BUS
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },       // HAL_CLOCKS_MODULE_SCR_SEC,       200MHz
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },       // HAL_CLOCKS_MODULE_SDIOM          CRG 200MHz, IO 50M
#if defined(CFG_DRIVERS_NANDFLASH)
        { CLOCKS_SRC_PLL, CLOCK_DIV_6 },       // HAL_CLOCKS_MODULE_EMMC           Default FMC 2x 133MHz, IO 67MHz
#elif defined(CFG_DRIVERS_MMC)
        { CLOCKS_SRC_PLL, CLOCK_DIV_2 },       // HAL_CLOCKS_MODULE_EMMC           CRG 400MHz, IO 100MHz(EMMC)
#else
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_EMMC
#endif
        { CLOCKS_SRC_PLL, CLOCK_DIV_20 },      // HAL_CLOCKS_MODULE_CAN, default 40M, 跟随CAN初始化和去初始化
        { CLOCKS_SRC_PLL, CLOCK_DIV_3 },       // HAL_CLOCKS_MODULE_USB_BUS        266.67MHz
        { CLOCKS_SRC_PLL, CLOCK_DIV_3 },       // HAL_CLOCKS_MODULE_HIFI
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },        // HAL_CLOCKS_MODULE_CODEC, default 非标采样率
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },       // HAL_CLOCKS_MODULE_XIP_SFC         SFC 2x 200MHz, IO 100MHz
        { CLOCKS_SRC_PLL, CLOCK_DIV_2 },       // HAL_CLOCKS_MODULE_XIP_OPI0        PSRAM 2x 400MHz, IO 200MHz
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },       // HAL_CLOCKS_MODULE_DPU, dpu/jpge/h264/vau, 跟随MCU高频
        { CLOCKS_SRC_PLL, CLOCK_DIV_8 },       // HAL_CLOCKS_MODULE_XIP_QSPI2, default 50MHz(IO) * 2，跟随亮灭屏流程
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },       // HAL_CLOCKS_MODULE_XIP_NPU_CORE, default work in pll, 跟随npu上下电流程
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_PAD_OUT0
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },       // HAL_CLOCKS_MODULE_PAD_OUT1
    },
};

/************************** MCU core clock Config *********************/
static const system_mcu_core_clk_cfg_t g_system_mcu_freq_config[CLOCKS_MCU_FREQ_MAX] = {
    {   // CLOCKS_M32
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // mcu_clk:32M
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // psram_clk: IO 16MHz
#ifdef CONFIG_BOOT_FROM_NANDFLASH
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // flash_clk
#else
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // flash_clk:IO 16MHz
#endif
#if defined(CFG_DRIVERS_NANDFLASH) || defined(CFG_DRIVERS_MMC)
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // emmc/fmc: FMC IO 16MHz, EMMC IO 32M
#else
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // emmc/fmc
#endif
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // sec: 32M
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // dpu/jpge/h264/vau
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // qspi display
    },
    {   // CLOCKS_M64
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },   // mcu_clk:64M
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },   // psram_clk:IO 32MHz
#ifdef CONFIG_BOOT_FROM_NANDFLASH
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // flash_clk
#else
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },   // flash_clk:IO 32MHz
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },   // emmc/fmc: FMC IO 32MHz, EMMC IO 32M
#elif defined(CFG_DRIVERS_MMC)
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // emmc/fmc: FMC IO 32MHz, EMMC IO 32M
#else
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // emmc/fmc
#endif
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // sec: 32M
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // dpu/jpge/h264/vau
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // qspi display
    },
    {   // CLOCKS_M_H0
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_3 },   // mcu_clk:133M
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_3 },   // psram_clk:IO 66MHz
#ifdef CONFIG_BOOT_FROM_NANDFLASH
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },           // flash_clk
#else
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_3 },   // flash_clk:IO 66MHz
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },   // emmc/fmc: FMC IO 32MHz, EMMC IO 32M
#elif defined(CFG_DRIVERS_MMC)
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // emmc/fmc: FMC IO 32MHz, EMMC IO 32M
#else
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // emmc/fmc
#endif
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },    // sec: 32M
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // dpu/jpge/h264/vau
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // qspi display
    },
    {   // CLOCKS_M_H1
        { CLOCKS_SRC_PLL, CLOCK_DIV_3 },   // mcu_clk:266M
        { CLOCKS_SRC_PLL, CLOCK_DIV_2 },   // psram_clk:IO 200MHz
#ifdef CONFIG_BOOT_FROM_NANDFLASH
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // flash_clk
#else
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },   // flash_clk:IO 100MHz
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
        { CLOCKS_SRC_PLL, CLOCK_DIV_6 },   // fmc:IO 67MHz
#elif defined(CFG_DRIVERS_MMC)
        { CLOCKS_SRC_PLL, CLOCK_DIV_2 },   // emmc:IO 100MHz(EMMC)
#else
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },   // emmc/fmc
#endif
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },   // sec: 200M
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },   // dpu/jpge/h264/vau: 200MHz
        { CLOCKS_SRC_PLL, CLOCK_DIV_8 },   // qspi display 100MHz
    },
};

static const clocks_clk_cfg_t g_system_emmc_freq_config[CLOCKS_EMMC_FREQ_MAX] = {
    { CLOCKS_SRC_XO, CLOCK_DIV_1 },      // 32M
    { CLOCKS_SRC_DLL, CLOCK_DIV_1 },     // 64M
    { CLOCKS_SRC_PLL, CLOCK_DIV_2 },     // 100M
};

static const clocks_clk_cfg_t g_system_sdio_freq_config[CLOCKS_SDIO_FREQ_MAX] = {
    { CLOCKS_SRC_XO, CLOCK_DIV_1 },      // 默认使用32M
    { CLOCKS_SRC_PLL, CLOCK_DIV_4 },     // 要求 router配置成200M, hs_ctrl 内部再分出50M
};

static const uint8_t g_system_mcu_freq_need_vol[CLOCKS_MCU_FREQ_MAX] = {
    PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V9,
};

static const system_hifi_clk_cfg_t g_hifi_freq_config[HIFI_LPM_CLK_MAX] = {
    {   // HIFI_LPM_CLK_LOW_LEVEL
        { CLOCKS_SRC_DLL, CLOCK_DIV_1 },  // hifi_clk:64M
#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_16 }, // codec_clk:24.576M
#else
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },   // codec_clk:32M
#endif
    },
    {   // HIFI_LPM_CLK_LEVEL1
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_3 },  // hifi_clk:133M
#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_16 }, // codec_clk:24.576M
#else
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },   // codec_clk:32M
#endif
    },
    {   // HIFI_LPM_CLK_LEVEL2
        { CLOCKS_SRC_PLL, CLOCK_DIV_4 },  // hifi_clk:200M
#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_16 }, // codec_clk:24.576M
#else
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },   // codec_clk:32M
#endif
    },
    {   // HIFI_LPM_CLK_HIGH_LEVEL
        { CLOCKS_SRC_PLL, CLOCK_DIV_3 },  // hifi_clk:266M
#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
        { CLOCKS_SRC_PLL_VCO_DIV, CLOCK_DIV_16 }, // codec_clk:24.576M
#else
        { CLOCKS_SRC_XO, CLOCK_DIV_1 },   // codec_clk:32M
#endif
    },
    {   // HIFI_LPM_CLK_NONE close clk
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },
        { CLOCKS_SRC_MAX, CLOCK_DIV_1 },
    },
};

static const uint8_t g_hifi_freq_need_vol[HIFI_LPM_CLK_MAX] = {
#ifdef CONFIG_AUDIO_STANDARD_SAMPLE_RATE
    PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V9, PMU_0P9_VSET_0V9, PMU_0P9_VSET_0V8
#else
    PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V8, PMU_0P9_VSET_0V9, PMU_0P9_VSET_0V9, PMU_0P9_VSET_0V8
#endif
};

clocks_clk_cfg_t const *clocks_system_all_clocks_get(system_clocks_config_t clk_level)
{
    return g_system_all_core_clocks[clk_level];
}

system_mcu_core_clk_cfg_t clocks_system_mcu_freq_get(system_mcu_freq_t clk_level)
{
    return g_system_mcu_freq_config[clk_level];
}

uint8_t clocks_system_mcu_need_vol_get(system_mcu_freq_t clk_level)
{
    return g_system_mcu_freq_need_vol[clk_level];
}

clocks_clk_cfg_t clocks_system_emmc_freq_get(system_emmc_freq_t clk_level)
{
    return g_system_emmc_freq_config[clk_level];
}

clocks_clk_cfg_t clocks_system_sdio_freq_get(system_sdio_freq_t clk_level)
{
    return g_system_sdio_freq_config[clk_level];
}

system_hifi_clk_cfg_t clocks_system_hifi_freq_get(pm_hifi_mode_t clk_level)
{
    return g_hifi_freq_config[clk_level];
}

uint8_t clocks_system_hifi_need_vol_get(pm_hifi_mode_t clk_level)
{
    return g_hifi_freq_need_vol[clk_level];
}
#endif