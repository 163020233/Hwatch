/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MMC of board
 *
 * Create: 2025-03-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT,ESTRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef CHIP_SOCMN2
#define CHIP_SOCMN2 1
#endif

#include "soc/mmc.h"
#include "hci.h"
#include "sdhci.h"
#include "sdhci_private.h"
#include "hci_helper.h"
#include "mmc_adapt.h"
#include "chip_core_definition.h"
#include "common_def.h"
#include "clocks_switch.h"
#include "clocks_config_priv.h"
#include "gpio.h"
#include "debug_print.h"
#include "clocks_core.h"
#include "pmu_ldo.h"
#include "socmn2_soc_reg.h"
#include "mmc_private.h"
#include "barrier.h"

#define CLOCKS_EMMC_FREQ_32M_VAL 32000000  // sdio:32M
#define CLOCKS_EMMC_FREQ_64M_VAL 64000000  // sdio:64M
#define CLOCKS_EMMC_FREQ_H_VAL   200000000 // sdio:200M
#define EMMC_SAVE_BUS_WIDTH_1BIT        0
#define EMMC_SAVE_BUS_WIDTH_4BIT        1
#define EMMC_SAVE_BUS_WIDTH_8BIT        2
#define EMMC_SAVE_BUS_TIMING_NORMAL     0
#define EMMC_SAVE_BUS_TIMING_MMC_HS     1
#define EMMC_SAVE_BUS_TIMING_MMC_HS200  3
#define EMMC_SAVE_BUS_TIMING_MMC_DDR52  4
#define EMMC_SAVE_BUS_TIMING_MMC_HS400  7
#define EMMC_CMD0_RESET_WAIT_TIME       900
#define SET_CMD_TIMEOUT_START_POS       24
#define SET_CMD_TIMEOUT_LEN             3
#define EMMC_HOST_CCLK_DIV        0x52063130
#define RC_CLK_VALUE              49152000
#define XO_CLK_VALUE              32000000
#define XO_DLL2_CLK_VALUE         64000000
#define PLL_CLK_VALUE             884736000
#define PM_SDIOM_CLK_REG          0x55000628
#define PM_EMMC_DIV_REG           0x52000120
#define BLOCK_SIZE                      0x4
#define HOST_CONTROL_BLOCK_GAP_CONTROL  0x28
#define CLOCK_CONTROL                   0x2c
#define INTERRUPT_STATUS_ENABLE         0x34
#define INTERRUPT_SIGNAL_ENABLE         0x38
#define PROTOCOL_MIN_DELAY_US         5000
#define MMC_POWER_OFF_DELAY_US        2000

#define MMC_SUM_NUM             2
#define MMC_NUM0_SDIO           0
#define MMC_NUM1_EMMC           1
// SDIO HOST
#define SDIO_HOST_BASE_ADDR     0x52100000
#define EMMC_BASE_ADDR          0x52101000

/* HS_CTL_RB Base address of Module's Register */
#define HS_CTL_RB_BASE                       (0x52106000)
#define HS_CTL_RB_SD_HOST_CCLK_DIV_REG         (0x52000110)
#define HS_CTL_RB_HS_CLK_EN_REG                (HS_CTL_RB_BASE + 0x24)
#define HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG       (HS_CTL_RB_BASE + 0x130)
#define HS_CTL_RB_CFG_HS_SDIO_CLKEN_OFFSET            0

typedef struct {
    uint16_t host_clk_div;
    uint16_t block_size; // 0x4
    uint32_t host_ctrl_blk_gap_ctrl; // 0x28
    uint32_t clock_ctrl; // 0x2c
    uint32_t int_stat_enable; // 0x34
    uint32_t int_signal_enable; // 0x38
    uint32_t mmc_rx_tune_ctl;
    uint32_t mmc_card_tune_ctl;
    uint32_t auto_cfg_by_pass;
} mmc_save_info_t;

static mmc_save_info_t g_mmc_cur_info[MMC_SUM_NUM] = { 0 };
bool g_mmc_sleep_inited = false;
bool g_mmc_inited = false;

void mmc_set_first_init_flag(bool flag)
{
    g_mmc_inited = flag;
}

bool mmc_get_first_init_flag(void)
{
    return g_mmc_inited;
}

void mmc_set_sleep_init_flag(bool flag)
{
    g_mmc_sleep_inited = flag;
}

bool mmc_get_sleep_init_flag(void)
{
    return g_mmc_sleep_inited;
}

mmc_save_info_t* sdhci_get_save_info(void)
{
    return &g_mmc_cur_info[0];
}

void mmc_set_ext_clk_switch(uint8_t mmc_type, bool enable)
{
    if (mmc_type == MMC_NUM0_SDIO) {
        /* no sdio in 3372 */
        return;
    }

    if (mmc_type == MMC_NUM1_EMMC) {
        /* 动态开关emmc时钟 */
        if (enable) {
            brandy_reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET);
        } else {
            brandy_reg16_clrbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET);
        }
    }
}

void mmc_save_info(uint8_t mmc_type)
{
    uint32_t base_addr = 0;
    uint32_t clk_addr = 0;
    mmc_save_info_t *ptr = NULL;

    if (mmc_type == MMC_NUM0_SDIO) {
        base_addr = SDIO_HOST_BASE_ADDR;
        clk_addr = HS_CTL_RB_SD_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM0_SDIO];
    } else {
        base_addr = EMMC_BASE_ADDR;
        clk_addr = HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM1_EMMC];
    }

    mmc_set_ext_clk_switch(mmc_type, true);
    /* Save tuning */
    if (mmc_type == MMC_NUM1_EMMC) {
        ptr->mmc_card_tune_ctl = readl(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG);
        ptr->mmc_rx_tune_ctl = readl(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG);
        ptr->auto_cfg_by_pass = readl(base_addr + EMMC_CKG_CTRL_AND_STS); // 0x124
    }
    ptr->host_clk_div = readw(clk_addr);
    ptr->block_size = readw(base_addr + BLOCK_SIZE); // 0x4
    ptr->host_ctrl_blk_gap_ctrl = readl(base_addr + HOST_CONTROL_BLOCK_GAP_CONTROL); // 0x28
    ptr->clock_ctrl = readl(base_addr + CLOCK_CONTROL); // 0x2c
    ptr->int_stat_enable = readl(base_addr + INTERRUPT_STATUS_ENABLE); // 0x34
    ptr->int_signal_enable = readl(base_addr + INTERRUPT_SIGNAL_ENABLE); // 0x38
    mmc_set_ext_clk_switch(mmc_type, false);
}

void mmc_recovery_init(uint8_t mmc_type)
{
    uint32_t base_addr = 0;
    uint32_t clk_addr = 0;
    mmc_save_info_t *ptr = NULL;

    if (mmc_type == MMC_NUM0_SDIO) {
        base_addr = SDIO_HOST_BASE_ADDR;
        clk_addr = HS_CTL_RB_SD_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM0_SDIO];
        reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_SDIO_CLKEN_OFFSET);
    } else {
        base_addr = EMMC_BASE_ADDR;
        clk_addr = HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM1_EMMC];
    }

    mmc_set_ext_clk_switch(mmc_type, true);
    /* Recovery tuning */
    if (mmc_type == MMC_NUM1_EMMC) {
        writel(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, ptr->mmc_card_tune_ctl);
        writel(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, ptr->mmc_rx_tune_ctl);
        writel(base_addr + EMMC_CKG_CTRL_AND_STS, ptr->auto_cfg_by_pass);
    }
    writew(clk_addr, ptr->host_clk_div);
    writel(base_addr + CLOCK_CONTROL, ptr->clock_ctrl); // 0x2C
    writew(base_addr + BLOCK_SIZE, ptr->block_size); // 0x4
    writel(base_addr + HOST_CONTROL_BLOCK_GAP_CONTROL, ptr->host_ctrl_blk_gap_ctrl); // 0x28
    writel(base_addr + INTERRUPT_STATUS_ENABLE, ptr->int_stat_enable); // 0x34
    writel(base_addr + INTERRUPT_SIGNAL_ENABLE, ptr->int_signal_enable); // 0x38
    mmc_set_ext_clk_switch(mmc_type, false);
}

void emmc_recovery_init(void)
{
    mmc_recovery_init(MMC_NUM1_EMMC);
}

void emmc_save_info(void)
{
    mmc_save_info(MMC_NUM1_EMMC);
}

void brandy_hal_reg_config_bit(uint32_t addr, brandy_switch_type_t on, BRANDY_REG16_POS position)
{
    if (on != 0) {
        brandy_reg16_setbit ((void *)((uintptr_t) addr), position);
    } else {
        brandy_reg16_clrbit ((void *)((uintptr_t) addr), position);
    }
}

void brandy_hal_clocks_config_bit(uint32_t reg_addr, brandy_switch_type_t on, BRANDY_REG16_POS position)
{
    if (on != 0) {
        brandy_reg16_setbit((void *)((uintptr_t)reg_addr), position);
    } else {
        brandy_reg16_clrbit((void *)((uintptr_t)reg_addr), position);
    }
}

void brandy_hal_clocks_sdio_crg_rst_control(brandy_switch_type_t rst_ctl)
{
    brandy_hal_reg_config_bit(CMU_CLOCKS_SDIO_CRG_SOFT_RST_REG, rst_ctl,
                              (BRANDY_REG16_POS)CMU_CLOCKS_SDIO_RST_DEVICE_SD_BIT);
}

static void hal_mmc_set_preset_val(struct hci_softc *sc, bool en)
{
    unsigned short ctrl_val = 0;

    ctrl_val = hci_readw(sc, SDHCI_HOST_CONTROL2);
    if (en) {
        ctrl_val |= SDHCI_CTRL2_PRESET_VALUE;
    } else {
        ctrl_val &= ~SDHCI_CTRL2_PRESET_VALUE;
    }

    hci_writew(sc, SDHCI_HOST_CONTROL2, ctrl_val);
}

static int mmc_voltage_switch(struct hci_softc *sc, uint32_t vccq)
{
    unsigned int pwr = SDHCI_POWER_180;

    switch (vccq) {
        case vccq_180:
            pwr = SDHCI_POWER_180;
            break;
        case vccq_330:
            pwr = SDHCI_POWER_330;
            break;
        default:
            device_printf(sc->dev, "error voltage:[%u].\n", vccq);
            break;
    }

#if (EMMC_BOARD_DEBUG_ON)
    device_printf(sc->dev, "vccq:[%u], pwr:[%u].\n", vccq, pwr);
#endif

    hal_mmc_set_preset_val(sc, false);

    hci_writeb(sc, SDHCI_POWER_CONTROL, 0);
    hci_writeb(sc, SDHCI_POWER_CONTROL, (uint8_t)pwr);

#ifdef LOSCFG_DRIVERS_SD
    /* sd卡供电切换成1.8V */

    if (pwr == SDHCI_POWER_180) {
        pmu_odio_power_set(TURN_ON, HAL_PMU_ODIO_LEVEL_1P8V);
        LOS_Udelay(PROTOCOL_MIN_DELAY_US); /* 协议要求大于5ms */
    }
#endif
    pwr |= SDHCI_POWER_ON;
    hci_writeb(sc, SDHCI_POWER_CONTROL, (uint8_t)pwr);
    LOS_Udelay(MSHC_WAIT_INT_PER_MSEC);

    return 0;
}

static void mmc_set_sd_bus_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;
    value = OS_GET_UINT16(div_reg);
    value &= ~SD_BUS_LOAD_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value &= ~SD_BUS_DIV_NUM_MASK;
    value |= (uint16_t)(freq_div << SD_BUS_DIV_NUM_POST_SHIFT);
    OS_WRITE_UINT16(value, div_reg);
    value |= SD_BUS_LOAD_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
}

static void mmc_set_sdiom_src_clk(uint32_t cfg_clock, uint32_t *freq_div)
{
    system_emmc_freq_t emmc_freq_level;
    if (cfg_clock <= SDIOM_SRC_CLK_LOW_FREQ_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_32M;
        *freq_div = SDIOM_SRC_CLK_LOW_FREQ_VAL / cfg_clock;
        if (SDIOM_SRC_CLK_LOW_FREQ_VAL % cfg_clock != 0) { *freq_div += 1; }
    } else if (cfg_clock == SDIOM_SRC_CLK_FREQ_LVL_64M_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_64M;
        *freq_div = 1;
    } else if (cfg_clock <= SDIOM_SRC_CLK_HIGH_FREQ_LVL_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_H;
        *freq_div = SDIOM_SRC_CLK_HIGH_FREQ_LVL_VAL / cfg_clock;
        if (SDIOM_SRC_CLK_HIGH_FREQ_LVL_VAL % cfg_clock != 0) { *freq_div += 1; }
    } else {
        emmc_freq_level = CLOCKS_EMMC_FREQ_32M;
        *freq_div = 1;
    }
    if (!clocks_set_sdio_freq((system_sdio_freq_t)emmc_freq_level)) {
#if (EMMC_BOARD_DEBUG_ON)
        dprintf("%s, sdiom_freq_level:[%u] error\r\n", __FUNCTION__, emmc_freq_level);
#endif
    }
}

static void mmc_set_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;
    value = OS_GET_UINT16(div_reg);
    value &= ~SDIO_EMMC_CCLK_EN;
    value &= ~SDIO_EMMC_ORI_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value &= ~SDIO_EMMC_DIV_NUM_MASK;
    value |= (uint16_t)(freq_div << SDIO_EMMC_DIV_NUM_POST_SHIFT);
    // emmc phase do nothing, to do
    OS_WRITE_UINT16(value, div_reg);
    value |= EMMC_CCLK_LOAD_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value |= EMMC_EMMC_CCLK_EN;
    OS_WRITE_UINT16(value, div_reg);
#if (EMMC_BOARD_DEBUG_ON)
    dprintf("%s, freq_div:[%u]  value:[%x]!\n", __FUNCTION__, freq_div, value);
#endif
}

unsigned int mmc_set_clock_with_way2(struct hci_softc *sc, unsigned int cfg_clock)
{
    uint32_t freq_div = 0;
    uint32_t div_reg = HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG; // default emmc reg
    uint32_t real_clock = cfg_clock;
    if (cfg_clock > sc->caps.max_clk) {
        dprintf("%s, clock:[%u] is greater than the max_clk:[%u]!\n", __FUNCTION__, cfg_clock, sc->caps.max_clk);
        real_clock = sc->caps.max_clk;
    }
    mmc_set_sdiom_src_clk(real_clock, &freq_div);
#ifdef DPALCFG_DRIVERS_SDIO
    if (sc->num == 1) {
        div_reg = HS_CTL_RB_SD_HOST_CCLK_DIV_REG;
    }
#endif
    mmc_set_clk_div(div_reg, freq_div);
    mmc_set_sd_bus_clk_div(SDIO_BUS_CGDIV_REG, 1);
    return 0;
}

static unsigned int hal_mmc_set_soc_clock(struct hci_softc *sc, unsigned int cfg_clock)
{
    return mmc_set_clock_with_way2(sc, cfg_clock);
}

static uint32_t sdhci_set_clock(struct hci_softc *sc, uint32_t clock)
{
    unsigned int ret;

    if (clock == 0) {
        device_printf(sc->dev, "set clock invalid param!\n");
        return clock;
    }
    dprintf("%s, clock: [%u], resource:[0x%x]!\n", __FUNCTION__, clock, OS_GET_UINT16(SDIOM_CR_REG));
    ret = hal_mmc_set_soc_clock(sc, clock);
    if (ret != 0) {
        device_printf(sc->dev, "set clock fail!\n");
        return 0;
    }

    // The fixed clock is less than or equal to 52 MHz, and the tuning clock is not used.
    if (clock <= SDIOM_SRC_CLK_FREQ_LVL_50M_VAL) {
        brandy_reg16_clrbit(HOST_CTRL2, SAMPLE_CLK_SEL_BIT);
    }
    return clock;
}

static void sdhci_set_drv_phase(uint32_t host_id, uint32_t phase)
{
    unused(host_id);
    unused(phase);
}

static void sdhci_card_clk_ctrl(const struct hci_softc *sc, int action)
{
    uint32_t val;

    val = hci_readw(sc, SDHCI_CLOCK_CONTROL);
    if (action == 0) {
        val &= ~SDHCI_CLOCK_CARD_EN;
    } else {
        val |= SDHCI_CLOCK_CARD_EN;
    }
    hci_writew(sc, SDHCI_CLOCK_CONTROL, val);

    LOS_Udelay(1); /* delay 1 us */

    val = hci_readw(sc, SDHCI_CLOCK_CONTROL);
    if (action == 0) {
        val &= ~0x1;
    } else {
        val |= 0x1;
    }
    hci_writew(sc, SDHCI_CLOCK_CONTROL, val);
}

static void sdhci_enhanced_strobe(const struct hci_softc *sc, uint32_t en)
{
    uint32_t value;

    value = hci_readw(sc, SDHCI_EMMC_CTRL);

    if (en != 0) {
        value |= SDHCI_EMMC_CTRL_ENH_STROBE_EN;
    } else {
        value &= ~(SDHCI_EMMC_CTRL_ENH_STROBE_EN);
    }
    hci_writew(sc, SDHCI_EMMC_CTRL, value);
}

static void sdhci_subplatform_init(const struct hci_softc *sc, hci_caps *cfg_info)
{
    device_printf(sc->dev, "LOSCFG_PLATFORM selected\n");
    if (sc->num == 0) { /* mmc0: eMMC or SD card */
#if defined(PRE_FPGA)
        cfg_info->max_clk = SDIOM_SRC_CLK_LOW_FREQ_VAL;
#elif defined(LOSCFG_DRIVERS_SD) /* sd max clk 52M */
        cfg_info->max_clk = SDIOM_SRC_CLK_FREQ_LVL_49M_VAL;
#else /* emmc max clk  98M */
        cfg_info->max_clk = SDIOM_SRC_CLK_FREQ_LVL_98M_VAL;
#endif
        cfg_info->min_clk = CONFIG_MMC0_CCLK_MIN;
        sdhci_enhanced_strobe(sc, false);
        /* overwrite the SDHCI standard because of the platform quirk */
        device_printf(sc->dev, "LOSCFG_DRIVERS_EMMC selected\n");
#ifdef LOSCFG_DRIVERS_SD /* sd */
        cfg_info->caps |= HCI_SLOTTYPE_REMOVABLE;
#else /* emmc */
        cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
#endif
        cfg_info->quirks |= SDHCI_QUIRK_SIG_1V8_ONLY; /* signal 1.2V unsupported */
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_AUTO_STOP;
    }
}

static void sdhci_platform_init(struct hci_softc *sc)
{
    hci_caps *cfg_info = &sc->caps;

    cfg_info->quirks |= SDHCI_QUIRK_WRITE_PROTECT;
    cfg_info->quirks |= SDHCI_QUIRK_MISSING_CAPS;
#ifdef CONFIG_SINGLE_BIT_MOD_MMC
    cfg_info->quirks |= SDHCI_QUIRK_ONLY_1_BIT_DATA;
#endif
    cfg_info->caps = hci_readl(sc, SDHCI_CAPABILITIES);
    cfg_info->caps &= ~SDHCI_SLOTTYPE_MASK;
    cfg_info->caps &= ~HCI_CAN_DO_8BITBUS;
    cfg_info->caps &= ~SDHCI_CAN_DO_ADMA2_64BIT;

    cfg_info->caps2 = hci_readl(sc, SDHCI_CAPABILITIES2);
    cfg_info->caps2 &= ~HCI_CAN_MMC_HS400;
    cfg_info->caps2 &= ~HCI_CAN_MMC_HS400ES;

    sdhci_subplatform_init(sc, cfg_info);
}

static void sdhci_extra_init(struct hci_softc *sc)
{
    unused(sc);
}

void brandy_hal_clocks_glb_sdio_soft_reset_control(hal_glb_sdio_soft_rst_type_t hal_glb_sdio_soft_rst,
                                                   hal_soft_rst_control_type_t rst_control)
{
    uint32_t reg_addr;
    uint8_t offset;

    switch (hal_glb_sdio_soft_rst) {
        case HAL_SOFT_RST6_SDIO_CRG:
            reg_addr = CMU_CLOCK_SOFT_RST6_REG;
            offset = CMU_CLOCK_SOFT_RST_GLB_SDIO_CRG_BIT;
            break;
        case HAL_SOFT_RST6_SDIO_LGC:
            reg_addr = CMU_CLOCK_SOFT_RST6_REG;
            offset = CMU_CLOCK_SOFT_RST_GLB_SDIO_LGC_BIT;
            break;
        default:
            return;
    }
    brandy_hal_clocks_config_bit(reg_addr, (brandy_switch_type_t)rst_control, (BRANDY_REG16_POS)offset);
}

void brandy_pmu_sdio_glb_common_reset(bool turn_on)
{
    if (turn_on) {
        brandy_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_DERESET);
        brandy_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_DERESET);
        brandy_hal_clocks_sdio_crg_rst_control((brandy_switch_type_t)HAL_SOFT_RST_DERESET);
    } else {
        brandy_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_RESET);
        brandy_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_RESET);
        brandy_hal_clocks_sdio_crg_rst_control((brandy_switch_type_t)HAL_SOFT_RST_RESET);
    }
}

static void brandy_hal_pmu_mcpu_mtop0_perip_reset(hal_pmu_mcpu_perips_type_t perip,
                                                  hal_pmu_perip_request_type_t request)
{
    uint16_t bit;

    switch (perip) {
        case HAL_PMU_MCPU_PERIP_TIMER:
            bit = PMU_MTOP0_TIMER_RST;
            break;
        case HAL_PMU_MCPU_PERIP_SMDMA:
            bit = PMU_MTOP0_SMDMA_RST;
            break;
        case HAL_PMU_MCPU_PERIP_MDMA:
            bit = PMU_MTOP0_MDMA_RST;
            break;
        default:
            return;
    }

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        brandy_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP0_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        brandy_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP0_L_REG, bit);
    }
}

static void brandy_hal_pmu_mcpu_mtop1_perip_reset(hal_pmu_mcpu_perips_type_t perip,
                                                  hal_pmu_perip_request_type_t request)
{
    uint16_t bit;

    switch (perip) {
        case HAL_PMU_MCPU_PERIP_I2C_BUS_0:
            bit = PMU_MTOP1_I2C0_RST;
            break;
        case HAL_PMU_MCPU_PERIP_I2C_BUS_1:
            bit = PMU_MTOP1_I2C1_RST;
            break;
        case HAL_PMU_MCPU_PERIP_I2C_BUS_2:
            bit = PMU_MTOP1_I2C2_RST;
            break;
        case HAL_PMU_MCPU_PERIP_I2C_BUS_3:
            bit = PMU_MTOP1_I2C3_RST;
            break;
        default:
            return;
    }

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        brandy_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP1_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        brandy_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP1_L_REG, bit);
    }
}

static void brandy_hal_pmu_mcpu_mtop2_perip_reset(hal_pmu_mcpu_perips_type_t perip,
                                                  hal_pmu_perip_request_type_t request)
{
    uint16_t bit;

    switch (perip) {
        case HAL_PMU_MCPU_PERIP_SPI_MST:
            bit = PMU_MTOP2_SPI0_RST;
            break;
        case HAL_PMU_MCPU_PERIP_SPI_MS_0:
            bit = PMU_MTOP2_SPI1_RST;
            break;
        case HAL_PMU_MCPU_PERIP_SPI_MS_1:
            bit = PMU_MTOP2_SPI2_RST;
            break;
        case HAL_PMU_MCPU_PERIP_QSPI2_1CS:
            bit = PMU_MTOP2_QSPI2_1CS_RST;
            break;
        default:
            return;
    }

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        brandy_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP2_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        brandy_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP2_L_REG, bit);
    }
}

static void brandy_hal_pmu_mcpu_mtop3_perip_reset(hal_pmu_mcpu_perips_type_t perip,
                                                  hal_pmu_perip_request_type_t request)
{
    uint16_t bit;

    switch (perip) {
        case HAL_PMU_MCPU_PERIP_UART_H0:
            bit = PMU_MTOP3_UART_H0_RST;
            break;
        case HAL_PMU_MCPU_PERIP_UART_H1:
            bit = PMU_MTOP3_UART_H1_RST;
            break;
        case HAL_PMU_MCPU_PERIP_UART_L0:
            bit = PMU_MTOP3_UART_L0_RST;
            break;
        default:
            return;
    }

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        brandy_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP3_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        brandy_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP3_L_REG, bit);
    }
}

static void brandy_hal_pmu_mcpu_mtop4_perip_reset(hal_pmu_mcpu_perips_type_t perip,
                                                  hal_pmu_perip_request_type_t request)
{
    uint16_t bit;

    switch (perip) {
        case HAL_PMU_MCPU_PERIP_EMMC:
            bit = PMU_MTOP4_EMMC_RST;
            break;
        case HAL_PMU_MCPU_PERIP_SDIO_HOST:
            bit = PMU_MTOP4_SDIO_HOST_RST;
            break;
        default:
            return;
    }

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        brandy_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP4_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        brandy_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP4_L_REG, bit);
    }
}

void brandy_hal_pmu_mcpu_perip_reset(hal_pmu_mcpu_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    if (perip <= HAL_PMU_MCPU_PERIP_MDMA) {
        brandy_hal_pmu_mcpu_mtop0_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_I2C_BUS_3) {
        brandy_hal_pmu_mcpu_mtop1_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_QSPI2_1CS) {
        brandy_hal_pmu_mcpu_mtop2_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_UART_L0) {
        brandy_hal_pmu_mcpu_mtop3_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_SDIO_HOST) {
        brandy_hal_pmu_mcpu_mtop4_perip_reset(perip, request);
#if CHIP_LIBRA
    } else if (perip <= HAL_PMU_MCPU_PERIP_SPI_M3) {
        hal_pmu_mcpu_mtop5_perip_reset(perip, request);
#endif
    } else {
        return;
    }
}

#define SDIO_HOST_CKDIV 0x5200014c
#define CLB_CLKEN1 0x57000084
#define SD_HOST_CCLK_DIV 0x52000110

void sd_clk_config(void)
{
    // step0：0x5210102C的bit2配置为0
    reg16_clrbit(EMMC_REG_BASE_ADDESS + 0x2C, 2);
    // step1：0x52106130 bit 11：4 配置为 0x4
    reg16_setbits(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, HS_CTL_RB_EMMC_CCLK_ORI_DIV_NUM_OFFSET,
        HS_CTL_RB_EMMC_CCLK_ORI_DIV_NUM_LEN, 0x4);
    // step2：0x52106130 bit 1 配置为 0x1
    reg16_setbit(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, 1);
    // step3：0x52106130 bit 0 配置为 0x1
    reg16_setbit(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, 0);
    // step4：0x52106130 bit 14：12 配置为 0x6
    reg16_setbits(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, HS_CTL_RB_EMMC_TUNING_COARSE_MODE_OFFSET,
        HS_CTL_RB_EMMC_TUNING_COARSE_MODE_LEN, 0x6);
    // step5：0x5210613C bit12 配置为0x0
    reg16_clrbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, 12);
    // step6：0x5210613C bit10：4 配置为0x50
    reg16_setbits(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_TUNING_CCLK_RX_SEL_OFFSET,
        HS_CTL_RB_EMMC_TUNING_CCLK_RX_SEL_LEN, 0x50);
    // step7：0x5210613C bit12 配置为0x1
    reg16_setbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, 12);
    // step8：0x5210613C bit0 配置为0x1
    reg16_setbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, 0);
    // step9：0x5210613C bit13 配置为0x0
    reg16_clrbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, 13);

    // step10 0x52106140 配置bit12为0
    reg16_clrbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, 12);
    reg16_setbits(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_OFFSET,
        HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_LEN, 0x4);
    // step11 0x52106140 配置bit12为1
    reg16_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, 12);
    reg16_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, 0);
    // step12：0x5210102C的bit2配置为1
    reg16_setbit(EMMC_REG_BASE_ADDESS + 0x2C, 2);
}

static void sdhci_slot_init(struct hci_softc *sc)
{
#define CFG_EMMC_CKG_CTRL_AND_STS_POS 16
#define CFG_EMMC_CKG_CTRL_AND_STS_LEN 8
    static uint8_t inited = false;
    uint32_t val;

    dprintf("%s enter, sc_num:[%u]\r\n", __FUNCTION__, sc->num);

    if (inited == 0) {
        brandy_pmu_sdio_glb_common_reset(true);
        inited = true;
    }

    if (sc->num == 0) {
        brandy_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_RESET);
        brandy_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_DERESET);
        uapi_reg_setbits(EMMC_REG_BASE_ADDESS + EMMC_CKG_CTRL_AND_STS,
            CFG_EMMC_CKG_CTRL_AND_STS_POS, CFG_EMMC_CKG_CTRL_AND_STS_LEN, 0xff);
#ifdef DPALCFG_DRIVERS_SDIO
    } else if (sc->num == 1) {
        brandy_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_SDIO_HOST, HAL_PMU_PERIP_REQUEST_RESET);
        brandy_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_SDIO_HOST, HAL_PMU_PERIP_REQUEST_DERESET);
#endif
    }

    /* 配置emmc时钟 */
    uapi_reg_write(M_CTL_RB_SDIO_HOST_CKDIV_REG, M_CTL_RB_CFG_M_SDIO_BUS_CLKEN_MASK); // SDIO总线时钟使能
    reg16_setbit(GLB_CTL_M_RB_CLB_CLKEN1_REG, GLB_CTL_M_RB_CFG_GLBM_SDIOTM_CLKEN_OFFSET);
    reg16_setbit(COM_CTL_RB_GLB_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_SDIOM_OFFSET);
    uapi_reg_write(M_CTL_RB_SD_HOST_CCLK_DIV_REG, 0x1013); // sdio host clk en

    reg16_setbit(EMMC_REG_BASE_ADDESS + 0x2C, 0x0);
    reg16_setbit(EMMC_REG_BASE_ADDESS + 0x2C, 0x2);

#if defined(LOSCFG_DRIVERS_SD)
    /* SD卡相位配置 */
    sd_clk_config();
#else
    /* emmc使能时钟取反选择 */
    reg16_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_HOST_CARD_CLK_INV_SEL_OFFSET);
#endif
    val = OS_GET_UINT16(COM_GP_REG0_ADDR);
    if (val != EMMC_SDIO_RAM_CONTROL_MASK) {
        val = EMMC_SDIO_RAM_CONTROL_MASK;
        OS_WRITE_UINT16((uint16_t)val, COM_GP_REG0_ADDR);
    }
    if (sc->num == 0) {
        uapi_reg_setbits(EMMC_REG_BASE_ADDESS + 0x108, SET_CMD_TIMEOUT_START_POS, SET_CMD_TIMEOUT_LEN, 0);
    } else if (sc->num == 1) {
        uapi_reg_setbits(SDIO_HOST_REG_BASE_ADDESS + 0x108, SET_CMD_TIMEOUT_START_POS, SET_CMD_TIMEOUT_LEN, 0);
    }

    sdhci_extra_init(sc);
    sdhci_platform_init(sc);
}

static void sdhci_en_drv_dll_sample(struct hci_softc *sc, uint32_t en)
{
    uint16_t val = 0;

#if EMMC_BOARD_DEBUG_ON
    dprintf("%s, en:[%u]\r\n", __FUNCTION__, en);
#endif
    if (en != 0) {
        hci_setl(sc, SDHCI_AUTO_TUNING_CTRL, SDHCI_SW_TUNING_EN);

        val = OS_GET_UINT16(EMMC_CCLK_DIV_REG);
        val |= EMMC_TUNING_COARSE_MODE;
        OS_WRITE_UINT16(val, EMMC_CCLK_DIV_REG);

        OS_WRITE_UINT16(EMMC_TUNING_BM0_VAL, EMMC_TUNING_BM0_REG);
        OS_WRITE_UINT16(EMMC_TUNING_BM1_VAL, EMMC_TUNING_BM1_REG);
    } else {
        hci_clearl(sc, SDHCI_AUTO_TUNING_CTRL, SDHCI_SW_TUNING_EN);
    }
}

static void sdhci_set_sample_phase(struct hci_softc *sc, unsigned int phase)
{
    uint32_t fields;

    fields = (((phase & 0x7) << 0x8) | 0x1);
    uapi_reg_write(0x5210613c, fields);
    uapi_reg_write(0x5210613c, fields | (0x1 << 0xC));
}

static bool sdhci_judge_dll_sample(uint32_t timing)
{
    if (timing == bus_timing_mmc_hs || timing == bus_timing_sd_hs || timing == bus_timing_normal ||
        timing == bus_timing_uhs_sdr25) {
        return false;
    } else {
        return true;
    }
}

static void sdhci_set_phase(struct hci_softc *sc, uint32_t timing)
{
    uint32_t drv_phase, phase;
    bool en_flag = true;

    if (timing == 0) { return; }
    if (sc->num == 0) {
        /* eMMC and SD card */
        if (timing == bus_timing_mmc_hs400es) {
            drv_phase = DRV_PHASE_112P5_DEGREE;
            phase = sc->tuning_phase;
            sdhci_enhanced_strobe(sc, true);
        } else {
            sdhci_enhanced_strobe(sc, false);
            if (timing == bus_timing_mmc_hs400) {
                drv_phase = DRV_PHASE_112P5_DEGREE;
                phase = sc->tuning_phase;
            } else if (timing == bus_timing_mmc_hs200) {
                drv_phase = DRV_PHASE_258P75_DEGREE;
                phase = sc->tuning_phase;
            } else if (timing == bus_timing_mmc_hs) {
                drv_phase = DRV_PHASE_180_DEGREE;
                phase = SAMPLE_PHASE;
            } else if (timing == bus_timing_sd_hs) {
                drv_phase = DRV_PHASE_225_DEGREE;
                phase = SAMPLE_PHASE;
            } else if (timing == bus_timing_normal) {
                drv_phase = DRV_PHASE_180_DEGREE;
                phase = 0;
            } else {
                drv_phase = DRV_PHASE_225_DEGREE;
                phase = SAMPLE_PHASE;
            }
        }
    } else {
        /* SDIO device */
        if ((timing == bus_timing_sd_hs) ||
            (timing == bus_timing_uhs_sdr25)) {
            drv_phase = DRV_PHASE_180_DEGREE;
            phase = SAMPLE_PHASE;
        } else {
            /* UHS_SDR12 */
            drv_phase = DRV_PHASE_180_DEGREE;
            phase = 0;
        }
    }
    en_flag = sdhci_judge_dll_sample(timing);
#if EMMC_BOARD_DEBUG_ON
    device_printf(sc->dev, "timing:%u, phase:0x%x, drv_phase:0x%x, en_flag:%d \n", timing, phase, drv_phase, en_flag);
#endif
    sdhci_set_drv_phase(sc->num, drv_phase);
    sdhci_en_drv_dll_sample(sc, en_flag);
    sdhci_set_sample_phase(sc, phase);
}

void sdhci_pre_set_clock(struct hci_softc *sc)
{
    unsigned int value;

    // open the clk of card:
    value = hci_readw(sc, SDHCI_CLOCK_CONTROL);
    value |= (SDHCI_CLOCK_INT_EN | SDHCI_CLOCK_PLL_EN | SDHCI_CLOCK_CARD_EN);
    hci_writew(sc, SDHCI_CLOCK_CONTROL, value);
}

void sdhci_post_clock(struct hci_softc *sc)
{
    unused(sc);
}

static void sdhci_select_sample_phase(struct hci_softc *sc, unsigned int phase)
{
    sdhci_card_clk_ctrl(sc, 0);
    sdhci_set_sample_phase(sc, phase);
    sdhci_card_clk_ctrl(sc, 1);
    LOS_Udelay(1); /* delay 1 us */
}

void mmc_dump_regs(struct hci_softc *sc)
{
}

void mmc_ext_clk_switch(struct hci_softc *sc, bool enable)
{
    struct sdhci_slot *slot = (struct sdhci_slot*)sc->slot;
    if (slot == NULL || slot->dev == NULL) {
        return;
    }

    struct mmc_softc *mmc_sc = device_get_softc(slot->dev);
    if (mmc_sc == NULL) {
        return;
    }

    if (mmc_sc->card_init_status != MMC_CARD_INIT) { return; }

    /* 动态开关emmc时钟 */
    if (enable) {
        brandy_reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET);
    } else {
        brandy_reg16_clrbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_EMMC_CLKEN_OFFSET);
    }
}

#define SAMP_PHASE_DEF        15
#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
#define TUNE_CORASE_MAX        4  /* 粗调 0 2 4 6 */
#define TUNE_FINE_MAX        11 /* 细调 【0 ~ 10】 */
#define TUNE_EDGE_MAX        (TUNE_CORASE_MAX * TUNE_FINE_MAX) /* Active phase scale: 0 ~ TUNE_EDGE_PHASES-1 */
int g_tuning_result_arr[TUNE_EDGE_MAX] = {0};
#else
#define TUNE_EDGE_MAX        8 /* Active phase scale: 0 ~ TUNE_EDGE_PHASES-1 */
#endif
#define TUNE_EDGE_NOT_FUND    TUNE_EDGE_MAX

#ifndef uapi_reg_write
#define uapi_reg_write(addr, val)              (*(volatile unsigned int *)(uintptr_t)(addr) = (val))
#endif
#ifndef uapi_reg_read
#define uapi_reg_read(addr, val)               ((val) = *(volatile unsigned int *)(uintptr_t)(addr))
#endif

typedef struct {
    uint32_t left;
    uint32_t right;
    uint32_t phase;
} tune_result_socmn2;

static const uint32_t g_tune_blk_4bits_socmn2[] = {
    0x00ff0fff, 0xccc3ccff, 0xffcc3cc3, 0xeffefffe,
    0xddffdfff, 0xfbfffbff, 0xff7fffbf, 0xefbdf777,
    0xf0fff0ff, 0x3cccfc0f, 0xcfcc33cc, 0xeeffefff,
    0xfdfffdff, 0xffbfffdf, 0xfff7ffbb, 0xde7b7ff7
};

static const uint32_t g_tune_blk_8bits_socmn2[] = {
    0xff00ffff, 0x0000ffff, 0xccccffff, 0xcccc33cc,
    0xcc3333cc, 0xffffcccc, 0xffffeeff, 0xffeeeeff,
    0xffddffff, 0xddddffff, 0xbbffffff, 0xbbffffff,
    0xffffffbb, 0xffffff77, 0x77ff7777, 0xffeeddbb,
    0x00ffffff, 0x00ffffff, 0xccffff00, 0xcc33cccc,
    0x3333cccc, 0xffcccccc, 0xffeeffff, 0xeeeeffff,
    0xddffffff, 0xddffffff, 0xffffffdd, 0xffffffbb,
    0xffffbbbb, 0xffff77ff, 0xff7777ff, 0xeeddbb77
};


static void tune_basic_socmn2_reg_op(struct sdhci_slot *slot)
{
    uint16_t host_ctrl2;
    uint32_t reg;

    WR4(slot, SDHCI_INT_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    host_ctrl2 = RD2(slot, SDHCI_HOST_CONTROL2);
    host_ctrl2 &= ~SDHCI_CTRL2_SAMPLING_CLOCK;
    WR2(slot, SDHCI_HOST_CONTROL2, host_ctrl2);

    reg = RD4(slot, SDHCI_AUTO_TUNING_CTRL);
    reg |= SDHCI_SW_TUNING_EN;
    WR4(slot, SDHCI_AUTO_TUNING_CTRL, reg);

    uapi_reg_read(0x52106130, reg);
    uapi_reg_write(0x52106130, (reg | 0x6000));

    uapi_reg_read(0x5210614c, reg);
    uapi_reg_write(0x5210614c, (reg | 0x2));
}

static void tune_basic_socmn2_build_cmd(struct sdhci_slot *slot, struct mmc_command *tune_cmd)
{
    uint32_t opcode = slot->tune_cmd->opcode;
    if (slot == NULL || tune_cmd == NULL) {
        return;
    }
    (void)memset_s(tune_cmd, sizeof(*tune_cmd), 0, sizeof(*tune_cmd));
    tune_cmd->opcode = opcode;
    tune_cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;
    tune_cmd->data = slot->tune_data;
}

static void tune_basic_socmn2_build_data(struct sdhci_slot *slot, struct mmc_data *tune_data,
    uint8_t *tune_data_buf, uint32_t data_buf_max_len)
{
    struct mmc_ios *ios = &slot->host.ios;
    uint32_t opcode = slot->tune_cmd->opcode;

    if (slot == NULL || tune_data == NULL || tune_data_buf == NULL) {
        return;
    }

    (void)memset_s(tune_data, sizeof(*tune_data), 0, sizeof(*tune_data));
    tune_data->len = (opcode == MMC_SEND_TUNING_BLOCK_HS200 && ios->bus_width == bus_width_8) ?
        data_buf_max_len : MMC_TUNING_LEN;

    tune_data->data = (void *)tune_data_buf;
    tune_data->flags = MMC_DATA_READ;
}

static void tune_basic_socmn2_build_req(struct sdhci_slot *slot, struct mmc_command *tune_cmd)
{
    if (slot == NULL || tune_cmd == NULL) {
        return;
    }
    struct mmc_request *tune_req = slot->tune_req;
    (void)memset_s(tune_req, sizeof(*tune_req), 0, sizeof(*tune_req));
    tune_req->cmd = tune_cmd;
    tune_req->done = sdhci_req_wakeup;
    tune_req->done_data = slot;
    slot->req = tune_req;
    slot->flags = 0;
}

static void tune_basic_socmn2_wait(struct sdhci_slot *slot, struct mmc_command *tune_cmd)
{
    uint32_t ret;
    if (slot == NULL || tune_cmd == NULL) {
        return;
    }
#define SDHCI_TUNINT_REQ_TIMEOUT  (LOSCFG_BASE_CORE_TICK_PER_SECOND / 5)
#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
    if (!OS_SCHEDULER_ACTIVE) {
        if (slot->req != NULL) {
            tune_cmd->error = MMC_ERR_NO_REP;
            slot_printf(slot, "tuning timeout\n");
            sdhci_dump_regs(slot);
            SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
            slot->req = NULL;
        }
    } else
#endif
    {
        ret = LOS_EventRead(&slot->req_event, SDHCI_ACQUIRE_HOST, LOS_WAITMODE_AND | LOS_WAITMODE_CLR,
            SDHCI_TUNINT_REQ_TIMEOUT);
        if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            tune_cmd->error = MMC_ERR_NO_REP;
            slot_printf(slot, "tuning timeout\n");
            sdhci_dump_regs(slot);
            SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
            slot->req = NULL;
        }
    }
}

static uint32_t find_longest_in_loop(uint8_t record[], uint32_t length)
{
    uint32_t i = 0;
    /* 避免从0开始切断收尾相接的连续成功相位 */
    for (i = 0; i != length; ++i) {
        if (record[i] != 0) {
            break;
        }
    }

    /* 全部相位成功选择0相位 */
    if (i == length) {
        return 0;
    }

    uint32_t begin = i;
    uint32_t len = 0;
    uint32_t max_len = 0;
    uint32_t max_point = 0;
    for (i = 0; i <= length; ++i) {
        /* 遍历length + 1个相位，连续成功的相位len增加，失败时如果len比max_len大，更新max_point */
        uint32_t j = begin + i;
        j = j >= length ? j - length : j;
        if (record[j] == 0) {
            len++;
        } else {
            if (len <= max_len) {
                len = 0;
            } else {
                max_len = len;
                uint32_t mid_off = (len + 1) >> 1;
                max_point = j < mid_off ? j + TUNE_EDGE_MAX - mid_off : j - mid_off;
                len = 0;
            }
        }
    }
    return max_point;
}

static int tune_basic_socmn2(device_t reqdev, void *arg)
{
    struct sdhci_slot *slot = device_get_ivars(reqdev);
    tune_result_socmn2 *t = (tune_result_socmn2 *)arg;
    int tune_result;
    uint32_t reg;
    uint32_t opcode = slot->tune_cmd->opcode;
    struct hci_softc *sc = device_get_softc(slot->bus);
    struct mmc_data *tune_data = slot->tune_data;
    struct mmc_command *tune_cmd = slot->tune_cmd;
    uint8_t tune_data_buf[MMC_TUNING_LEN_HS200];
    struct mmc_ios *ios = &slot->host.ios;
    uint8_t record[TUNE_EDGE_MAX] = {0};

    tune_basic_socmn2_reg_op(slot);

    for (uint32_t i = 0; i < TUNE_EDGE_MAX; i++) {
        if (slot->ops->select_sample_phase != NULL)
            slot->ops->select_sample_phase(sc, i);

        /* edge detect 使能 */
        reg = RD4(slot, 0x130);
        reg |= 0x1;
        WR4(slot, 0x130, reg);

        tune_basic_socmn2_build_cmd(slot, tune_cmd);
        tune_basic_socmn2_build_data(slot, tune_data, tune_data_buf, MMC_TUNING_LEN_HS200);
        tune_basic_socmn2_build_req(slot, tune_cmd);
        tune_result = 0;
        sdhci_start(slot);
        tune_basic_socmn2_wait(slot, tune_cmd);

        if (tune_cmd->error) {
            tune_result = tune_cmd->error;
        }

        const void *cmp_data_buf = (opcode == MMC_SEND_TUNING_BLOCK_HS200 && ios->bus_width == bus_width_8) ?
            (const void *)g_tune_blk_8bits_socmn2 : (const void *)g_tune_blk_4bits_socmn2;

        if (memcmp(cmp_data_buf, (void *)tune_data_buf, tune_data->len) != 0) {
            tune_result = -EIO;
        }

        reg = RD4(slot, 0x134);
        if (reg & 0x80000000) {
            tune_result = -EIO;
        }
        dprintf("tuning index = %d, tune_result %d\n", i, tune_result);

        dprintf("edge reg 0x%x\ntune_result %d\n", reg, tune_result);
        record[i] = tune_result;
    }

    t->phase = find_longest_in_loop(record, TUNE_EDGE_MAX);
    dprintf("selected phase %d\n", t->phase);
    if (t->phase == TUNE_EDGE_MAX) {
        return -1;
    } else {
        return 0;
    }
}

static int retune_socmn2(device_t brdev, device_t reqdev, bool reset)
{
    unused(reset);
    return tune_edge(brdev, reqdev);
}

void mmc_ext_power_up_down(struct hci_softc *sc, enum mmc_power_mode flag)
{
    unused(sc);
    if (flag == power_on) {
        uapi_gpio_set_val(S_AGPIO5, GPIO_LEVEL_HIGH);
    } else if (flag == power_off) {
        pmu_odio_power_set(TURN_ON, HAL_PMU_ODIO_LEVEL_3P3V);
        uapi_gpio_set_val(S_AGPIO5, GPIO_LEVEL_LOW);
        LOS_Udelay(MMC_POWER_OFF_DELAY_US);   /* delay 2 ms */
    }
}

void init_slot_methods(int slot, hci_slot_ops *slot_ops)
{
    unused(slot);

    slot_ops->pre_clock = sdhci_pre_set_clock;
    slot_ops->post_clock = sdhci_post_clock;
    slot_ops->set_clock = sdhci_set_clock;
    slot_ops->extra_init = sdhci_extra_init;
    slot_ops->slot_init = sdhci_slot_init;
    slot_ops->set_phase = sdhci_set_phase;
    slot_ops->enable_drv_sample = sdhci_en_drv_dll_sample;
    slot_ops->select_sample_phase = sdhci_select_sample_phase;
    slot_ops->switch_volt = mmc_voltage_switch;
    slot_ops->dump_regs = mmc_dump_regs;
#ifdef LOSCFG_DRIVERS_MMC_EXT_CLK_SWITCH
    slot_ops->drv_ext_clk_switch = mmc_ext_clk_switch;
#endif
    slot_ops->hci_custom_retune = retune_socmn2;
    slot_ops->hci_custom_tunebasic = tune_basic_socmn2;
#ifdef LOSCFG_DRIVERS_MMCSD_EXT_POWER_UP_DOWN
    slot_ops->drv_ext_power_up_down = mmc_ext_power_up_down;
#endif
}
