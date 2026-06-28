/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: MMC of hi1a21_app board
 * Author: Huawei LiteOS Team
 * Create: 2024-07-16
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
#include "mmcreg.h"
#include "mmcsd.h"
#include "mmcvar.h"
#include "mmc_subr.h"
#include "mmc_subr_bus.h"
#include "mmc_private.h"
#include "sdhci_private.h"
#include "hci_helper.h"
#include "chip_io.h"
#include "barrier.h"

#define MAX_LOOP_TIME 0xfffff
#define CLOCKS_EMMC_FREQ_32M_VAL 32000000
#define CLOCKS_EMMC_FREQ_64M_VAL 64000000
#define CLOCKS_EMMC_FREQ_H_VAL   200000000
#define CLOCKS_EMMC_ROUTER_VAL   800000000
typedef enum {
    CLOCKS_EMMC_FREQ_32M,      // sdio:32M
    CLOCKS_EMMC_FREQ_64M,      // sdio:64M
    CLOCKS_EMMC_FREQ_H,        // 200M
    CLOCKS_EMMC_FREQ_MAX,
} system_emmc_freq_t;

#ifndef unused
#define unused(var)     (void)(var)
#endif
#define uapi_reg_read_val32(addr)              (*(volatile unsigned int*)(uintptr_t)(addr))
#define uapi_reg_clrbit(addr, pos)             ((uapi_reg_read_val32(addr)) &= ~((unsigned int)(1) << (pos)))
#define uapi_reg_setbit(addr, pos)             ((uapi_reg_read_val32(addr)) |= ((unsigned int)(1) << (pos)))
#define uapi_reg_clrbits(addr, pos, bits)      (uapi_reg_read_val32(addr) &= \
    ~((((unsigned int)1 << (bits)) - 1) << (pos)))
#define uapi_reg_setbits(addr, pos, bits, val) (uapi_reg_read_val32(addr) =           \
    (uapi_reg_read_val32(addr) & (~((((unsigned int)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned int)((val) & (((unsigned int)1 << (bits)) - 1)) << (pos)))


#define HS_CTL_RB_BASE                                      0x52063000
#define HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG                    (HS_CTL_RB_BASE+ 0x130)
#define HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG              (HS_CTL_RB_BASE+ 0x140)
#define HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG               (HS_CTL_RB_BASE+ 0x13C)
#define HS_CTL_RB_SD_HOST_CCLK_DIV_REG                      (HS_CTL_RB_BASE + 0x90)
#define HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_LEN              3
#define HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_OFFSET           4
#define HS_CTL_RB_EMMC_SAMPLE_CCLK_RX_SEL_OFFSET            0
#define HS_CTL_RB_EMMC_SAMPLE_CARD_CLK_SEL_OFFSET           0
#define HS_CTL_RB_EMMC_HOST_CARD_CLK_INV_SEL_OFFSET         13
#define HS_CTL_RB_EMMC_HOST_RX_CLK_INV_SEL_OFFSET           13
#define HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_UPDATA_OFFSET    12

#define EMMC_COARSE_CARD_CLK_POS        13
#define EMMC_COARSE_RX_CLK_POS          4
#define EMMC_COARSE_RX_CLK_LEN          4
#define EMMC_FINE_RX_CLK_POS            8
#define EMMC_FINE_RX_CLK_LEN            3
#define EMMC_CARD_COARSE_FIXED_VALUE    0x5
#define EMMC_COARSE_RX_CLK_EN_POS          14

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

    hal_mmc_set_preset_val(sc, false);

    hci_writeb(sc, SDHCI_POWER_CONTROL, 0);
    hci_writeb(sc, SDHCI_POWER_CONTROL, (uint8_t)pwr);
    pwr |= SDHCI_POWER_ON;
    hci_writeb(sc, SDHCI_POWER_CONTROL, (uint8_t)pwr);
    LOS_Udelay(MSHC_WAIT_INT_PER_MSEC);

    return 0;
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

__attribute__((weak)) void uapi_awake_emmc_card(void)
{
}

void emmc_tuning_reg_cfg(uint32_t emmc_clk)
{
    if (emmc_clk > SDIOM_SRC_CLK_FREQ_LVL_50M_VAL) { // 开启tuning
        // 打开 rx card时钟
        uapi_reg_setbit(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, EMMC_COARSE_CARD_CLK_POS);
        uapi_reg_setbit(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, EMMC_COARSE_RX_CLK_EN_POS);
        // Card调相采用固定配置
        uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_HOST_CARD_CLK_INV_SEL_OFFSET);
        uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_UPDATA_OFFSET);
        // [4, 6] 写 5 固定使用225
        uapi_reg_setbits(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG,
            HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_OFFSET,
            HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_LEN,
            EMMC_CARD_COARSE_FIXED_VALUE);
        uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_TUNING_CARD_CLK_SEL_UPDATA_OFFSET);
        uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_SAMPLE_CARD_CLK_SEL_OFFSET);
        // RX使用调相算法, 遍历出最优的相位
        uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_HOST_RX_CLK_INV_SEL_OFFSET);
        uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_SAMPLE_CCLK_RX_SEL_OFFSET);
    } else { // 不开启tuning, 探卡走该分支
        // 关闭RX调相
        uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_HOST_RX_CLK_INV_SEL_OFFSET);
        uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_SAMPLE_CCLK_RX_SEL_OFFSET);
        // 关闭Card调相
        uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_HOST_CARD_CLK_INV_SEL_OFFSET);
        uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, HS_CTL_RB_EMMC_SAMPLE_CARD_CLK_SEL_OFFSET);
    }
}

static config_emmc_router g_emmc_router = NULL;

void emmc_couter_config_register(config_emmc_router router)
{
    if (router == NULL) {
        return;
    }
    g_emmc_router = router;
}

static uint32_t sdhci_set_emmc_clock(struct hci_softc *sc, uint32_t clock)
{
    uint32_t freq_div = 0;
    system_emmc_freq_t emmc_freq_level = CLOCKS_EMMC_FREQ_32M;
    if (clock <= CLOCKS_EMMC_FREQ_32M_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_32M;
        freq_div = CLOCKS_EMMC_FREQ_32M_VAL / clock;
        if (CLOCKS_EMMC_FREQ_32M_VAL % clock != 0) {
            freq_div += 1;
        }
    } else if (clock <= CLOCKS_EMMC_FREQ_64M_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_64M;
        freq_div = 1;
    } else if (clock <= CLOCKS_EMMC_FREQ_H_VAL) {
        emmc_freq_level = CLOCKS_EMMC_FREQ_H;
        freq_div = CLOCKS_EMMC_ROUTER_VAL / clock;
        if (CLOCKS_EMMC_ROUTER_VAL % clock != 0) {
            freq_div += 1;
        }
    }
    // Tuning code here
    emmc_tuning_reg_cfg(clock);
    if (g_emmc_router != NULL) {
        g_emmc_router(emmc_freq_level);
    }
    mmc_set_clk_div(HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG, freq_div);
    return clock;
}

static uint32_t sdhci_set_sdio_clock(struct hci_softc *sc, uint32_t clock)
{
    uint32_t freq_div = 0;
    if (clock <= SDIOM_SRC_CLK_LOW_FREQ_VAL) {
        freq_div = SDIOM_SRC_CLK_LOW_FREQ_VAL / clock;
        if (SDIOM_SRC_CLK_LOW_FREQ_VAL % clock != 0) {
            freq_div += 1;
        }
    }
    mmc_set_clk_div(HS_CTL_RB_SD_HOST_CCLK_DIV_REG, freq_div);
    return clock;
}

static uint32_t sdhci_set_clock(struct hci_softc *sc, uint32_t clock)
{
    if (clock == 0) {
        device_printf(sc->dev, "set clock invalid param!\n");
        return clock;
    }
    uint32_t fixed_clock = clock;
    if (fixed_clock > sc->caps.max_clk) {
        dprintf("clock:[%u] is greater than the max_clk:[%u]!\n", fixed_clock, sc->caps.max_clk);
        fixed_clock = sc->caps.max_clk;
    }
    if (sc->num == 0) {
        return sdhci_set_emmc_clock(sc, fixed_clock);
    }
    return sdhci_set_sdio_clock(sc, fixed_clock);
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
#ifdef EMMC_SDIO_USE_LOW_FREQ
        cfg_info->max_clk = CLOCKS_EMMC_FREQ_32M_VAL;
#else
        cfg_info->max_clk = CLOCKS_EMMC_FREQ_H_VAL;
#endif
        cfg_info->min_clk = CONFIG_MMC0_CCLK_MIN;
        sdhci_enhanced_strobe(sc, false);
        /* overwrite the SDHCI standard because of the platform quirk */
        device_printf(sc->dev, "LOSCFG_DRIVERS_EMMC selected\n");
        cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
        cfg_info->caps |= HCI_CAN_DO_8BITBUS;
        cfg_info->quirks |= SDHCI_QUIRK_SIG_1V8_ONLY; /* signal 1.2V unsupported */
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
    } else if (sc->num == 1) { /* SDIO device */
#ifdef EMMC_SDIO_USE_LOW_FREQ
        cfg_info->max_clk = SDIOM_SRC_CLK_LOW_FREQ_VAL;
#else
        cfg_info->max_clk = SDIOM_SRC_CLK_LOW_FREQ_VAL;
#endif
        cfg_info->min_clk = CONFIG_MMC1_CCLK_MIN;
        cfg_info->quirks |= SDHCI_QUIRK_SIG_3V3_ONLY; /* signal 1.2V 1.8V unsupported */
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
        cfg_info->caps |= HCI_SLOTTYPE_SDIO;
        cfg_info->caps &= ~HCI_CAN_VDD_180;
        cfg_info->caps &= ~HCI_CAN_DO_8BITBUS;
    }
}

static void sdhci_platform_init(struct hci_softc *sc)
{
    hci_caps *cfg_info = &sc->caps;

    cfg_info->quirks |= SDHCI_QUIRK_WRITE_PROTECT;
    cfg_info->quirks |= SDHCI_QUIRK_MISSING_CAPS;

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

static void sdhci_pre_set_clock(struct hci_softc *sc)
{
    unsigned int value;
    // open the clk of card:
    value = hci_readw(sc, SDHCI_CLOCK_CONTROL);
    value |= (SDHCI_CLOCK_INT_EN | SDHCI_CLOCK_CARD_EN);
    hci_writew(sc, SDHCI_CLOCK_CONTROL, value);
}

static void sdhci_slot_init(struct hci_softc *sc)
{
    dprintf("%s enter, sc_num:[%u]\r\n", __FUNCTION__, sc->num);
#define CFG_CKG_BYP_SDHCR_POS 23
#define CFG_EMMC_CKG_CTRL_AND_STS_POS 16
#define CFG_EMMC_CKG_CTRL_AND_STS_LEN 8
    if (sc->num == 0) {
        /* Set this parameter to 0 to obtain the maximum timeout interval. */
        uapi_reg_setbits(EMMC_REG_BASE_ADDESS + 0x108, SET_CMD_TIMEOUT_START_POS, SET_CMD_TIMEOUT_LEN, 0);
        uapi_reg_setbits(EMMC_REG_BASE_ADDESS + EMMC_CKG_CTRL_AND_STS,
            CFG_EMMC_CKG_CTRL_AND_STS_POS, CFG_EMMC_CKG_CTRL_AND_STS_LEN, 0xff);
    } else if (sc->num == 1) {
        uapi_reg_setbits(SDIO_HOST_REG_BASE_ADDESS + 0x108, SET_CMD_TIMEOUT_START_POS, SET_CMD_TIMEOUT_LEN, 1);
        // 规避控制器问题, 复用DATA1线为中断线场景，data1作为中断被拉低后不能及时拉高，导致device给host发数据失败。
        uapi_reg_setbit(SDIO_HOST_REG_BASE_ADDESS + 0x124, CFG_CKG_BYP_SDHCR_POS);
    }
    sdhci_pre_set_clock(sc);
    sdhci_extra_init(sc);
    sdhci_platform_init(sc);
}

static void sdhci_en_drv_dll_sample(struct hci_softc *sc, uint32_t en)
{
    uint16_t val = 0;

    EMMC_BOARD_DEBUG("%s, en:[%u]\r\n", __FUNCTION__, en);

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

#define	HS_CTL_RB_EMMC_TUNING_CCLK_RX_SEL_UPDATA_OFFSET	        12

static void sdhci_set_sample_phase(struct hci_softc *sc, unsigned int phase)
{
    if (sc->num == 1) { // emmc做调相， sdio暂时没适配
        return;
    }
    uapi_reg_clrbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_TUNING_CCLK_RX_SEL_UPDATA_OFFSET);
    uint32_t corase_val = (phase / 11) * 2 ; // 11个细调 粗调 0 2 4 6
    uint32_t fine_val = phase % 11; // 11个细调 细调 [0 ~ 10]
    uapi_reg_setbits(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG,
        EMMC_COARSE_RX_CLK_POS, EMMC_COARSE_RX_CLK_LEN, fine_val);
    uapi_reg_setbits(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG,
        EMMC_FINE_RX_CLK_POS, EMMC_FINE_RX_CLK_LEN, corase_val);
    uapi_reg_setbit(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, HS_CTL_RB_EMMC_TUNING_CCLK_RX_SEL_UPDATA_OFFSET);
}

static void sdhci_set_mmc_io_driver(const struct hci_softc *sc, uint32_t host_id, enum mmc_bus_timing timing)
{
    unused(sc);
    unused(host_id);
    unused(timing);
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

    if (timing == 0) {
        return;
    }
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
        if ((timing == bus_timing_sd_hs) || (timing == bus_timing_uhs_sdr25)) {
            drv_phase = DRV_PHASE_180_DEGREE;
            phase = SAMPLE_PHASE;
        } else {
            /* UHS_SDR12 */
            drv_phase = DRV_PHASE_180_DEGREE;
            phase = 0;
        }
    }
    en_flag = sdhci_judge_dll_sample(timing);
    sdhci_set_drv_phase(sc->num, drv_phase);
    sdhci_en_drv_dll_sample(sc, en_flag);
    sdhci_set_sample_phase(sc, phase);
    sdhci_set_mmc_io_driver(sc, sc->num, timing);
}

static void sdhci_post_clock(struct hci_softc *sc)
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

static void mmc_dump_regs(struct hci_softc *sc)
{
}

static uint8_t g_mmc0_clk_switch_cnt = 0;
static uint8_t g_mmc1_clk_switch_cnt = 0;

/* 此处弱函数为解决os调用驱动层产生的编译问题 实现在驱动层 */
__attribute__((weak)) void uapi_clocks_switch(uint8_t id, switch_type_t config)
{
    unused(id);
    unused(config);
}

void mmc_ext_clk_switch(struct hci_softc *sc, bool enable)
{
    uint8_t clk_bit = 0;
    uint8_t clk_lock_num = 0;
    struct sdhci_slot *slot = (struct sdhci_slot*)sc->slot;
    if (slot == NULL || slot->dev == NULL) {
        return;
    }
    struct mmc_softc *mmc_sc = device_get_softc(slot->dev);
    if (mmc_sc == NULL) {
        return;
    }
    if (sc->num == 0x1) { // sdio do not support the switch of clock. switch pin function
         if (enable) {
             brandy_reg16_clrbit(EXTERNAL_SDIO_CLK_PIN_REG, EXTERNAL_SDIO_CLK_PIN_BIT);
         } else {
             brandy_reg16_setbit(EXTERNAL_SDIO_CLK_PIN_REG, EXTERNAL_SDIO_CLK_PIN_BIT);
         }
        return;
    }
    if (mmc_sc->card_init_status != MMC_CARD_INIT) { return; }

    if (sc->num == 0x0) { // for emmc
        clk_bit = 0x4;
        clk_lock_num = g_mmc0_clk_switch_cnt;
    } else { // for sdio
        clk_bit = 0x0;
        clk_lock_num = g_mmc1_clk_switch_cnt;
    }
    if (enable) {
        uapi_clocks_switch(CLOCKS_ID_DEV_EMMC, TURN_ON);
        brandy_reg16_setbit(SOC_EMMC_CR_CLK, SOC_CR_CH_ENABLE);
        brandy_reg16_setbit(EXTERNAL_EMMC_CCLK_DIV, clk_bit);
        clk_lock_num++;
    } else {
        if (clk_lock_num > 1) {
            clk_lock_num--;
        } else if (clk_lock_num == 1) {
            brandy_reg16_clrbit(EXTERNAL_EMMC_CCLK_DIV, clk_bit);
            brandy_reg16_clrbit(SOC_EMMC_CR_CLK, SOC_CR_CH_ENABLE);
            clk_lock_num = 0;
        }
    }
#ifdef CFG_EMMC_CARD_SLEEP_AWAKE
    g_emmc_cmd_count_when_sleep++;
#endif
    if (sc->num == 0x0) { // for emmc
        g_mmc0_clk_switch_cnt = clk_lock_num;
    } else { // for sdio
        g_mmc1_clk_switch_cnt = clk_lock_num;
    }
    dsb();
}

enum emmc_info_type {
    EMMC_INFO_TYPE_CID = 0,
    EMMC_INFO_TYPE_CSD,
    EMMC_INFO_TYPE_EXT_CSD,
};

static struct mmc_softc* get_emmc_softc(uint32_t index)
{
    struct hci_softc *hci_sc = get_mmc_resource(index);
    if (hci_sc == NULL) {
        return NULL;
    }
    struct sdhci_slot *slot = (struct sdhci_slot*)hci_sc->slot;
    if (slot == NULL) {
        return NULL;
    }
    struct mmc_softc *mmc_host = device_get_softc(slot->dev);
    if (mmc_host == NULL) {
        return NULL;
    }
    return mmc_host;
}
static struct mmcsd_softc* get_emmc_sdsc(uint32_t index)
{
    struct mmc_softc* mmc_host = get_emmc_softc(index);
    if (mmc_host == NULL) {
        return NULL;
    }
    struct mmcsd_softc *sc = NULL;
    for (int i = 0; i < mmc_host->child_count; i++) {
        device_t child = mmc_host->child_list[i];
        if (child == NULL) {
            continue;
        }
        int card_type = mmc_get_card_type(child);
        if (card_type == mode_mmc) {
            sc = device_get_softc(child);
            if (sc == NULL) {
                continue;
            }
            break;
        }
    }
    return sc;
}

static int get_mmc_xid(uint32_t id_flag, uint8_t *id)
{
    struct mmcsd_softc* sd_sc = get_emmc_sdsc(0);
    if (sd_sc == NULL || sd_sc->dev == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    uint8_t *tmpId = NULL;
    uint32_t id_len = 0;
    struct mmc_ivars *ivar = NULL;
    ivar = device_get_ivars(sd_sc->dev);
    if (ivar == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    if (id_flag == EMMC_INFO_TYPE_CID) {
        tmpId = (uint8_t *)ivar->raw_cid;
        id_len = 0x4 * sizeof(uint32_t);
    }
    if (id_flag == EMMC_INFO_TYPE_CSD) {
        tmpId = (uint8_t *)ivar->raw_csd;
        id_len = 0x4 * sizeof(uint32_t);
    }
    if (id_flag == EMMC_INFO_TYPE_EXT_CSD) {
        tmpId = (uint8_t *)ivar->raw_ext_csd;
        id_len = 0x200;
    }
    memcpy_s(id, id_len, tmpId, id_len);
    return MMC_ERR_NONE;
}

int get_emmc_cid(uint32_t *raw_cid)
{
    return get_mmc_xid(EMMC_INFO_TYPE_CID, (uint8_t *)raw_cid);
}

int get_emmc_csd(uint32_t *raw_csd)
{
    return get_mmc_xid(EMMC_INFO_TYPE_CSD, (uint8_t *)raw_csd);
}

int get_emmc_exe_csd(uint8_t *raw_csd)
{
    int err;
    struct mmcsd_softc* sd_sc = get_emmc_sdsc(0);
    if (sd_sc == NULL || sd_sc->dev == NULL || sd_sc->mmcbus == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    uapi_awake_emmc_card();
    MMCBUS_ACQUIRE_BUS(sd_sc->mmcbus, sd_sc->dev);
    err = mmc_send_ext_csd(sd_sc->mmcbus, sd_sc->dev, raw_csd);
    MMCBUS_RELEASE_BUS(sd_sc->mmcbus, sd_sc->dev);
    return err;
}

int get_emmc_info(uint32_t args, uint8_t *emmc_info)
{
    struct mmcsd_softc* sd_sc = get_emmc_sdsc(0);
    if (sd_sc == NULL || sd_sc->dev == NULL || sd_sc->mmcbus == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    uapi_awake_emmc_card();
    return mmc_gen_cmd(sd_sc->mmcbus, sd_sc->dev, args, emmc_info);
}

int set_emmc_ecsd_info(uint8_t fields, uint8_t value)
{
    struct mmc_softc* mmc_sc = get_emmc_softc(0);
    if (mmc_sc == NULL || mmc_sc->dev == NULL || mmc_sc->child_list == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    struct mmc_ivars *ivar = device_get_ivars(mmc_sc->child_list[0]);
    if (ivar == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    uapi_awake_emmc_card();
    return mmc_switch(mmc_sc->dev, mmc_sc->dev, ivar->rca, EXT_CSD_CMD_SET_NORMAL,
        fields, value, ivar->cmd6_time, true);
}

void hi_set_emmc_clock(uint32_t clock)
{
    struct hci_softc *hci_sc = get_mmc_resource(0);
    struct sdhci_slot *slot = (struct sdhci_slot *)hci_sc->slot;
    if (hci_sc == NULL || slot == NULL) {
        return;
    }
    mmc_ext_clk_switch(hci_sc, true);
    host_set_clock(slot, clock);
    mmc_ext_clk_switch(hci_sc, false);
}

static int hi_mmc_wait_for_command(struct mmc_softc *sc, uint32_t opcode,
    uint32_t arg, uint32_t flags, uint32_t *resp, int retries)
{
    struct mmc_command cmd;
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    cmd.opcode = opcode;
    cmd.arg = arg;
    cmd.flags = flags;
    cmd.data = NULL;
    return mmc_wait_for_cmd(sc->dev, sc->dev, &cmd, retries);
}

int hi_mmc_switch_sleep_awake(uint8_t sleep)
{
#define SLEEP_AWAKE_RCA_POS 16
#define SLEEP_AWAKE_FLAG_POS 15
    struct mmc_softc *mmc_sc = get_emmc_softc(0);
    if (mmc_sc == NULL || mmc_sc->child_list == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    struct mmc_ivars *ivar = device_get_ivars(mmc_sc->child_list[0]);
    if (ivar == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    int flags = (ivar->rca ? MMC_RSP_R1B : MMC_RSP_NONE) | MMC_CMD_AC;
    mmc_sc->retune_paused++;
    int err = hi_mmc_wait_for_command(mmc_sc, MMC_SLEEP_AWAKE,
        ((uint32_t)(ivar->rca)) << SLEEP_AWAKE_RCA_POS | (uint32_t)sleep << SLEEP_AWAKE_FLAG_POS, flags, NULL, 3);
    mmc_sc->retune_paused--;
    return (err);
}

int hi_mmc_select_card(uint8_t selected)
{
#define SELECT_CARD_RCA_POS 16
    struct mmc_softc *mmc_sc = get_emmc_softc(0);
    if (mmc_sc == NULL || mmc_sc->child_list == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    struct mmc_ivars *ivar = device_get_ivars(mmc_sc->child_list[0]);
    if (ivar == NULL) {
        return MMC_ERR_UNPLUGGED;
    }
    uint16_t rca = ivar->rca;
    if (selected == UNSELECT_EMMC_CARD) {
        rca = 0;
    }
    int flags = (rca ? MMC_RSP_R1B : MMC_RSP_NONE) | MMC_CMD_AC;
    mmc_sc->retune_paused++;
    int err = hi_mmc_wait_for_command(mmc_sc, MMC_SELECT_CARD, (uint32_t)rca << SELECT_CARD_RCA_POS,
        flags, NULL, 3);
    mmc_sc->retune_paused--;
    return (err);
}

uint32_t hi_sleep_emmc_card(void)
{
    struct mmcsd_softc* sd_sc = get_emmc_sdsc(0);
    uint32_t rets = MMC_ERR_UNPLUGGED;
    if (sd_sc == NULL || sd_sc->mmcbus == NULL || sd_sc->dev == NULL) {
        return rets;
    }
    MMCBUS_ACQUIRE_BUS(sd_sc->mmcbus, sd_sc->dev);
    if (hi_mmc_select_card(UNSELECT_EMMC_CARD) == MMC_ERR_NONE) {
        rets = hi_mmc_switch_sleep_awake(1);
    }
    if (rets == MMC_ERR_NONE) {
        g_emmc_cmd_count_when_sleep = 0;
    }
    MMCBUS_RELEASE_BUS(sd_sc->mmcbus, sd_sc->dev);
    return rets;
}

uint32_t hi_awake_emmc_card(void)
{
    struct mmcsd_softc* sd_sc = get_emmc_sdsc(0);
    uint32_t rets = MMC_ERR_UNPLUGGED;
    if (sd_sc == NULL || sd_sc->mmcbus == NULL || sd_sc->dev == NULL) {
        return rets;
    }
    MMCBUS_ACQUIRE_BUS(sd_sc->mmcbus, sd_sc->dev);
    if (hi_mmc_switch_sleep_awake(0) == MMC_ERR_NONE) {
        rets = hi_mmc_select_card(SELECT_EMMC_CARD);
    }
    MMCBUS_RELEASE_BUS(sd_sc->mmcbus, sd_sc->dev);
    return rets;
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
