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

#include "soc/mmc.h"
#include "hci.h"
#include "sdhci.h"
#include "sdhci_private.h"
#include "hci_helper.h"
#include "chip_core_definition.h"
#include "common_def.h"
#include "clocks_switch.h"
#include "gpio.h"
#include "debug_print.h"
#include "clocks_core.h"
#include "pmu_ldo.h"
#include "soc_reg.h"
#include "mmc_private.h"
#include "barrier.h"

#define MMC_SUM_NUM             2
#define MMC_NUM0_SDIO           0
#define MMC_NUM1_EMMC           1

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
            reg16_setbit(M_CTL_RB_EMMC_CCLK_DIV_REG, M_CTL_RB_EMMC_HOST_CLKEN_OFFSET);
        } else {
            reg16_clrbit(M_CTL_RB_EMMC_CCLK_DIV_REG, M_CTL_RB_EMMC_HOST_CLKEN_OFFSET);
        }
    }
}

void mmc_save_info(uint8_t mmc_type)
{
    unused(mmc_type);
}

void mmc_recovery_init(uint8_t mmc_type)
{
    unused(mmc_type);
}

void emmc_recovery_init(void)
{
}

void emmc_save_info(void)
{
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

    pwr |= SDHCI_POWER_ON;
    hci_writeb(sc, SDHCI_POWER_CONTROL, (uint8_t)pwr);
    LOS_Udelay(MSHC_WAIT_INT_PER_MSEC);

    return 0;
}

static void mmc_host_set_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;
    value = OS_GET_UINT16(div_reg);
    value &= ~SDIO_HOST_CLKEN;
    value &= ~SDIO_CCLK_ORI_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value &= ~SDIO_CCLK_ORI_LOAD_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value &= ~SDIO_EMMC_DIV_NUM_MASK;
    value |= (uint16_t)(freq_div << SDIO_EMMC_DIV_NUM_POST_SHIFT);
    OS_WRITE_UINT16(value, div_reg);
    value |= SDIO_CCLK_ORI_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value |= SDIO_CCLK_ORI_LOAD_DIV_EN;
    OS_WRITE_UINT16(value, div_reg);
    value |= SDIO_HOST_CLKEN;
    OS_WRITE_UINT16(value, div_reg);

    value = OS_GET_UINT16(div_reg);
}

uint32_t sdhci_set_clock(struct hci_softc *sc, uint32_t clock)
{
    bool tuning_on;
    system_freq_level_t sdiom_freq_level;
    uint32_t freq_div;

    tuning_on = clock > SDIOM_SRC_CLK_FREQ_LVL_50M_VAL;
    uapi_reg_setbits(M_CTL_RB_EMMC_CCLK_DIV_REG, M_CTL_RB_SDIO_TUNING_COARSE_MODE_OFFSET,
        M_CTL_RB_SDIO_TUNING_COARSE_MODE_LEN, tuning_on);

    if (clock <= SDIOM_SRC_CLK_LOW_FREQ_VAL) {
        sdiom_freq_level = CLOCKS_FREQ_32M;
        freq_div = SDIOM_SRC_CLK_LOW_FREQ_VAL / clock;
    } else if (clock <= SDIOM_SRC_CLK_FREQ_LVL_50M_VAL) {
        sdiom_freq_level = CLOCKS_FREQ_PLL_D8;
        freq_div = SDIOM_SRC_CLK_FREQ_LVL_50M_VAL / clock;
    } else {
        sdiom_freq_level = CLOCKS_FREQ_PLL_D1;
        freq_div = 0x4;
    }

#if (EMMC_BOARD_DEBUG_ON)
    dprintf("clk_level:[%u], freq_div:[%u], cfg_clock: [%u]!\n",
        sdiom_freq_level, *freq_div, clock);
#endif
    (void)clocks_set_sdio_freq(sdiom_freq_level);
    mmc_host_set_clk_div(M_CTL_RB_SD_HOST_CCLK_DIV_REG, freq_div);
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
#else /* emmc max clk  98M */
        cfg_info->max_clk = SDIOM_SRC_CLK_FREQ_LVL_98M_VAL;
#endif
        cfg_info->min_clk = CONFIG_MMC0_CCLK_MIN;
        sdhci_enhanced_strobe(sc, false);
        /* overwrite the SDHCI standard because of the platform quirk */
        device_printf(sc->dev, "LOSCFG_DRIVERS_EMMC selected\n");
        cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
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

void mmc_clocks_glb_sdio_soft_reset_control(hal_glb_sdio_soft_rst_type_t hal_glb_sdio_soft_rst,
    hal_soft_rst_control_type_t rst_control)
{
    uint32_t reg_addr;
    uint8_t offset;
    reg_addr = GLB_CTL_M_RB_SOFT_RST_6_REG;

    switch (hal_glb_sdio_soft_rst) {
        case HAL_SOFT_RST6_SDIO_CRG:
            offset = GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET;
            break;
        case HAL_SOFT_RST6_SDIO_LGC:
            offset = GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET;
            break;
        default:
            return;
    }

    if (rst_control != 0) {
        reg16_setbit((void *)((uintptr_t)reg_addr), offset);
    } else {
        reg16_clrbit((void *)((uintptr_t)reg_addr), offset);
    }
}

void pmu_sdio_glb_common_reset(bool turn_on)
{
    if (turn_on) {
        mmc_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_DERESET);
        mmc_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_DERESET);
    } else {
        mmc_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_RESET);
        mmc_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_RESET);
    }
}

static void hal_pmu_mcpu_mtop4_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        reg16_clrbit(M_CTL_RB_MTOP4_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        reg16_setbit(M_CTL_RB_MTOP4_L_REG, bit);
    }
}

static void sdhci_slot_init(struct hci_softc *sc)
{
    static uint8_t inited = false;

    dprintf("%s enter, sc_num:[%u]\r\n", __FUNCTION__, sc->num);

    if (inited == 0) {
        pmu_sdio_glb_common_reset(true);
        inited = true;
    }

    if (sc->num == 0) {
        hal_pmu_mcpu_mtop4_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_RESET);
        hal_pmu_mcpu_mtop4_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_DERESET);
#ifdef DPALCFG_DRIVERS_SDIO
    } else if (sc->num == 1) {
        hal_pmu_mcpu_mtop4_perip_reset(HAL_PMU_MCPU_PERIP_SDIO_HOST, HAL_PMU_PERIP_REQUEST_RESET);
        hal_pmu_mcpu_mtop4_perip_reset(HAL_PMU_MCPU_PERIP_SDIO_HOST, HAL_PMU_PERIP_REQUEST_DERESET);
#endif
    }

    /* 配置emmc时钟 */
    uapi_reg_write(M_CTL_RB_SDIO_HOST_CKDIV_REG, 0x33); // SDIO总线时钟使能
    uapi_reg_write(GLB_CTL_M_RB_BASE, 0x7fcc); // sdio_tm时钟使能
    uapi_reg_write(M_CTL_RB_SD_HOST_CCLK_DIV_REG, 0x3013); // sdio host clk en
    hci_writel(sc, SDHCI_HOST_CONTROL, 0x100);
    uapi_reg_write(M_CTL_RB_SDIO_TUNING_CLK_CTL_REG, 0x1f1);
    uapi_reg_write(M_CTL_RB_SD_HOST_CCLK_DIV_REG, 0x304f);
    hci_writel(sc, SDHCI_CLOCK_CONTROL, 0xc0007);
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

        val = OS_GET_UINT16(M_CTL_RB_EMMC_CCLK_DIV_REG);
        val |= EMMC_TUNING_COARSE_MODE;
        OS_WRITE_UINT16(val, M_CTL_RB_EMMC_CCLK_DIV_REG);

        OS_WRITE_UINT16(EMMC_TUNING_BM0_VAL, M_CTL_RB_SD_HOST_TUNING_BM0_REG);
        OS_WRITE_UINT16(EMMC_TUNING_BM1_VAL, M_CTL_RB_SD_HOST_TUNING_BM1_REG);
    } else {
        hci_clearl(sc, SDHCI_AUTO_TUNING_CTRL, SDHCI_SW_TUNING_EN);
    }
}

static inline uint32_t sdhci_phase_to_bitmap(uint32_t phase)
{
    return (phase & 0x1 ? 0x000f : 0x0) |
           (phase & 0x2 ? 0x00f0 : 0x0) |
           (phase & 0x4 ? 0x0f00 : 0x0) |
           (phase & 0x8 ? 0xf000 : 0x0);
}

static void sdhci_set_sample_phase(struct hci_softc *sc, unsigned int phase)
{
    uint32_t bitmap1 = sdhci_phase_to_bitmap(phase);
    uapi_reg_write(M_CTL_RB_SD_HOST_TUNING_BM1_REG, bitmap1);
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
        reg16_setbit(M_CTL_RB_EMMC_CCLK_DIV_REG, M_CTL_RB_EMMC_HOST_CLKEN_OFFSET);
    } else {
        reg16_clrbit(M_CTL_RB_EMMC_CCLK_DIV_REG, M_CTL_RB_EMMC_HOST_CLKEN_OFFSET);
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
}
