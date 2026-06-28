/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MMC of board
 * Author: Huawei LiteOS Team
 * Create: 2021-03-17
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
#include "mmc.h"
#include "hci.h"
#include "sdhci/sdhci.h"
#include "sdhci/sdhci_private.h"
#include "hci_helper.h"
#include "mmc_adapt.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define REG_MISC_CTRL18     0x48

#define SDIO0_PWRSW_SEL_1V8     (1u << 1)
#define SDIO0_PWR_CARD_1V8      (1u << 1)
#define SDIO0_PWRSW_EN          (1u << 4)
#define SDIO0_PWRSW_CTRL_1V8    (1u << 5)

#define REG_CTRL_BASE (0x1F000000)

#define REG_CTRL_SDIO0_CLK      (REG_CTRL_BASE + 0x104c)
#define REG_CTRL_SDIO0_CMD      (REG_CTRL_BASE + 0x1050)
#define REG_CTRL_SDIO0_DATA0    (REG_CTRL_BASE + 0x1054)
#define REG_CTRL_SDIO0_DATA1    (REG_CTRL_BASE + 0x1058)
#define REG_CTRL_SDIO0_DATA2    (REG_CTRL_BASE + 0x105c)
#define REG_CTRL_SDIO0_DATA3    (REG_CTRL_BASE + 0x1060)

#define REG_CTRL_SDIO1_CLK      (REG_CTRL_BASE + 0x106c)
#define REG_CTRL_SDIO1_CMD      (REG_CTRL_BASE + 0x1070)
#define REG_CTRL_SDIO1_DATA0    (REG_CTRL_BASE + 0x1074)
#define REG_CTRL_SDIO1_DATA1    (REG_CTRL_BASE + 0x1078)
#define REG_CTRL_SDIO1_DATA2    (REG_CTRL_BASE + 0x107c)
#define REG_CTRL_SDIO1_DATA3    (REG_CTRL_BASE + 0x1080)

#define REG_CTRL_SDIO2_CLK      (REG_CTRL_BASE + 0x10b0)
#define REG_CTRL_SDIO2_CARD_POW (REG_CTRL_BASE + 0x10b4)
#define REG_CTRL_SDIO2_CMD      (REG_CTRL_BASE + 0x10b8)
#define REG_CTRL_SDIO2_DATA3    (REG_CTRL_BASE + 0x10bc)

#define REG_CTRL_SDIO2_DATA0    (REG_CTRL_BASE + 0x10ac)
#define REG_CTRL_SDIO2_DATA1    (REG_CTRL_BASE + 0x1084)
#define REG_CTRL_SDIO2_DATA2    (REG_CTRL_BASE + 0x10a0)

#define REG_COMBPHY_BASE        (0x10290000)
#define REG_IOCTL_RONSEL_1_0    (REG_COMBPHY_BASE + 0x264)
#define REG_IOCTL_OD_RONSEL_2   (REG_COMBPHY_BASE + 0x268)

static int sdhci_voltage_switch(struct hci_softc *sc, enum mmc_vccq vccq)
{
    unsigned int ctrl;

    if (vccq == vccq_330) {
        dev_loge(sc->dev, "set voltage to 3.3V\n");

        ctrl = GET_UINT32(REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
        ctrl |= SDIO0_PWRSW_EN;
        ctrl &= ~(SDIO0_PWRSW_CTRL_1V8 | SDIO0_PWRSW_SEL_1V8 | SDIO0_PWR_CARD_1V8);
        WRITE_UINT32(ctrl, REG_MISC_CTRL_BASE + REG_MISC_CTRL18);

        LOS_Udelay(5000); /* delay 5000 us */

        ctrl = GET_UINT32(REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
        if ((ctrl & SDIO0_PWRSW_EN) && !(ctrl & SDIO0_PWRSW_CTRL_1V8) &&
            !(ctrl & SDIO0_PWRSW_SEL_1V8) && !(ctrl & SDIO0_PWR_CARD_1V8)) {
            return 0;
        }
        dev_loge(sc->dev, "slot%u: 3.3V regulator output did not became stable\n", sc->num);
        return -EAGAIN;
    } else if (vccq == vccq_180) {
        dev_loge(sc->dev, "set voltage to 1.8V\n");

        ctrl = GET_UINT32(REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
        ctrl |= SDIO0_PWRSW_CTRL_1V8;
        WRITE_UINT32(ctrl, REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
        LOS_Udelay(5000); /* delay 5000 us */

        ctrl |= SDIO0_PWRSW_SEL_1V8 | SDIO0_PWR_CARD_1V8;
        WRITE_UINT32(ctrl, REG_MISC_CTRL_BASE + REG_MISC_CTRL18);

        ctrl = GET_UINT32(REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
        if ((ctrl & SDIO0_PWRSW_SEL_1V8) && (ctrl & SDIO0_PWRSW_CTRL_1V8) &&
            (ctrl & SDIO0_PWR_CARD_1V8)) {
            return 0;
        }
        dev_loge(sc->dev, "slot%u: 1.8V regulator output did not became stable\n", sc->num);

        return -EAGAIN;
    } else {
        return -EAGAIN;
    }
}

static unsigned int sdhci_get_realclock(const struct hci_softc *sc, unsigned int clock, unsigned int *temp)
{
/* internal quartering */
#define MMC_SDIO0_CKSEL_400KHZ    0
#define MMC_SDIO0_CKSEL_100MHZ    1
#define MMC_SDIO0_CKSEL_198MHZ    2
#define MMC_SDIO0_CKSEL_396MHZ    3
#define MMC_SDIO0_CKSEL_750MHZ    4
#define MMC_SDIO0_CKSEL_600MHZ    5
#define MMC_SDIO0_CKSEL_792MHZ    6
#define MMC_SDIO0_CKSEL_1600KHZ   7
    uint32_t real_clock;

    if (clock >= MMC_FREQ_198M) {
        *temp = MMC_SDIO0_CKSEL_792MHZ;
        real_clock = MMC_FREQ_198M;
    } else if (clock >= MMC_FREQ_187P5M) {
        *temp = MMC_SDIO0_CKSEL_750MHZ;
        real_clock = MMC_FREQ_187P5M;
    } else if (clock >= MMC_FREQ_150M) {
        *temp = MMC_SDIO0_CKSEL_600MHZ;
        real_clock = MMC_FREQ_150M;
    } else if (clock >= MMC_FREQ_99M) {
        *temp = MMC_SDIO0_CKSEL_396MHZ;
        real_clock = MMC_FREQ_99M;
    } else if (clock >= MMC_FREQ_49P5M) {
        *temp = MMC_SDIO0_CKSEL_198MHZ;
        real_clock = MMC_FREQ_49P5M;
    } else if (clock >= MMC_FREQ_25M) {
        *temp = MMC_SDIO0_CKSEL_100MHZ;
        real_clock = MMC_FREQ_25M;
    } else if (clock >= MMC_FREQ_400K) {
        *temp = MMC_SDIO0_CKSEL_1600KHZ;
        real_clock = MMC_FREQ_400K;
    } else if (clock >= MMC_FREQ_100K) {
        *temp = MMC_SDIO0_CKSEL_400KHZ;
        real_clock = MMC_FREQ_100K;
    } else {
        dev_loge(sc->dev, "no suitable clock for host%u, clk=%u\n", sc->num, clock);
        return 0;
    }

    return real_clock;
}

static uint32_t sdhci_set_clock(struct hci_softc *sc, uint32_t clock)
{
    uint32_t reg;
    uint32_t temp;
    uint32_t value;
    uint16_t val;
    /* four mmc host */
    uint32_t cfg_array[4] = {PERI_CRG123, PERI_CRG133, PERI_CRG106, PERI_CRG143};
    uint32_t real_clock;

    if (sc->num >= ARRAY_SIZE(cfg_array)) {
        dev_loge(sc->dev, "sc->num = %u is not supported!\n", sc->num);
        return 0;
    }

    real_clock = sdhci_get_realclock(sc, clock, &temp);
    if (!real_clock) {
        return 0;
    }

    reg = cfg_array[sc->num];
    value = GET_UINT32(reg);
    value &= ~(MMC_FREQ_MASK << MMC_FREQ_SHIFT);
    value |= ((temp & MMC_FREQ_MASK) << MMC_FREQ_SHIFT);
    value |= SDIO0_CKEN; /* clock enable */
    WRITE_UINT32(value, reg);

    if (sc->num == 2) { /* sdio1 SD mem-version2.0 */
        val = hci_readw(sc, SDHCI_EMMC_CTRL);
        val &= ~SDHCI_EMMC_CTRL_EMMC;
        val |= SDHCI_EMMC_CTRL_EMMC;
        hci_writew(sc, SDHCI_EMMC_CTRL, val);
        /* set drv strength to 50ohm */
        WRITE_UINT32(0, REG_IOCTL_RONSEL_1_0);
        WRITE_UINT32(0x6ff, REG_IOCTL_OD_RONSEL_2);
    }

    dev_loge(sc->dev, "set real clock = %u, request clock = %u, clk_div = %u, sc->num = %u, crg_value = 0x%x.\n",
        real_clock, clock, temp, sc->num, value);
    return real_clock;
}

void sdhci_hisi_set_samp_phase(struct hci_softc *sc, unsigned int phase)
{
    unsigned int reg;
    reg = hci_readl(sc, SDHCI_AUTO_TUNING_STATUS);
    reg &= ~SDHCI_CENTER_PH_CODE_MASK;
    reg |= phase;
    hci_writel(sc, SDHCI_AUTO_TUNING_STATUS, reg);
}

void sdhci_platform_set_io(const struct hci_softc *sc)
{
    /* clk, cmd, data0, data1, data2, data3 default value for sdio2 */
    uint32_t sdio2[] = {0x1565, 0x1545, 0x1545, 0x1545, 0x1545, 0x1545};
    int i = 0;
    switch (sc->num) {
        case MMC0:
            /* enable SDIO0 pin power in CTRL18 reg */
            WRITE_UINT32(0x11, REG_MISC_CTRL_BASE + REG_MISC_CTRL18);
            break;
        case MMC1:
            break;
        case MMC2: /* emmc */
            return;
        case MMC3:
            WRITE_UINT32(sdio2[i++], REG_CTRL_SDIO2_CLK);
            WRITE_UINT32(sdio2[i++], REG_CTRL_SDIO2_CMD);
            WRITE_UINT32(sdio2[i++], REG_CTRL_SDIO2_DATA0);
            WRITE_UINT32(sdio2[i++], REG_CTRL_SDIO2_DATA1);
            WRITE_UINT32(sdio2[i++], REG_CTRL_SDIO2_DATA2);
            WRITE_UINT32(sdio2[i], REG_CTRL_SDIO2_DATA3);
            return;
        default:
            return;
    }
}

static void sdhci_set_drv_phase(struct hci_softc *sc, uint32_t phase)
{
    uint32_t reg, val;

    reg = PERI_CRG117;

    val = GET_UINT32(reg);
    val &= ~PERI_SAM_DRV_MASK;
    val |= (phase << PERI_SAM_DRV_SHFT);
    WRITE_UINT32(val, reg);
}

static void sdhci_crg_init(struct hci_softc *sc)
{
#define DEFAULT_PHASE 0x10
    uint32_t value;
    uint32_t reg;
    /* four mmc host */
    uint32_t cfg_array[4] = {PERI_CRG123, PERI_CRG133, PERI_CRG106, PERI_CRG143};

    if (sc->num >= ARRAY_SIZE(cfg_array)) {
        dev_loge(sc->dev, "sc->num = %u is not supported!\n", sc->num);
        return;
    }

    /* open the clock gate */
    reg = cfg_array[sc->num];
    value = GET_UINT32(reg);
    value |= PERI_CRG106_EMMC_CKEN;
    WRITE_UINT32(value, reg);

    /* crg_reset, dll_reset, sampl_reset */
    reg = cfg_array[sc->num];
    value = GET_UINT32(reg);
    value |= PERI_CRG106_EMMC_CRG_REQ;
    value |= PERI_CRG106_EMMC_DLL_RST;
    value |= PERI_CRG106_EMMC_SAM_RST;
    WRITE_UINT32(value, reg);

    /* wait the card clk close for 1000us */
    LOS_Udelay(1000);

    /* reset of host control is done */
    value = GET_UINT32(reg);
    value &= ~PERI_CRG106_EMMC_CRG_REQ;
    WRITE_UINT32(value, reg);

    /* wait it done for 1000us */
    LOS_Udelay(1000);

    /* close the clock gate */
    value = hci_readl(sc, SDHCI_CLOCK_CONTROL);
    value &= ~SDHCI_CLOCK_INT_EN;
    value &= ~SDHCI_CLOCK_CARD_EN;
    hci_writel(sc, SDHCI_CLOCK_CONTROL, value);

    sdhci_set_clock(sc, MMC_FREQ_198M);

    /* SAM/DRV */
    sdhci_set_drv_phase(sc, DEFAULT_PHASE);

    /* wait the clock switch over for 1000 us */
    LOS_Udelay(1000);

    /* open the clk of interface */
    value = hci_readl(sc, SDHCI_CLOCK_CONTROL);
    value |= SDHCI_CLOCK_INT_EN;
    hci_writel(sc, SDHCI_CLOCK_CONTROL, value);

    /* open the clk of card */
    value = hci_readl(sc, SDHCI_CLOCK_CONTROL);
    value |= SDHCI_CLOCK_CARD_EN;
    hci_writel(sc, SDHCI_CLOCK_CONTROL, value);

    /* wait the phase switch over 1000us */
    LOS_Udelay(1000);

    value = GET_UINT32(MISC_REG_CTRL3);
    value &= ~(MISC_REG_CTRL3_SDIO2_BYPASS | MISC_REG_CTRL3_SDIO1_BYPASS \
            | MISC_REG_CTRL3_SDIO0_BYPASS);
    WRITE_UINT32(value, MISC_REG_CTRL3);
}

static void sdhci_platform_init(struct hci_softc *sc)
{
    hci_caps *cfg_info = &sc->caps;
    cfg_info->quirks |= SDHCI_QUIRK_WRITE_PROTECT;
    cfg_info->quirks |= SDHCI_QUIRK_MISSING_CAPS;
    cfg_info->quirks |= SDHCI_QUIRK_BROKEN_UHS_DDR50;
    cfg_info->caps = hci_readl(sc, SDHCI_CAPABILITIES);
    cfg_info->caps2 = hci_readl(sc, SDHCI_CAPABILITIES2);

    switch (sc->num) {
        case MMC0:
            cfg_info->max_clk = CONFIG_MMC0_CCLK_MAX;
            cfg_info->min_clk = CONFIG_MMC0_CCLK_MIN;
            break;
#if USE_MMC1
        case MMC1:
            cfg_info->max_clk = CONFIG_MMC1_CCLK_MAX;
            cfg_info->min_clk = CONFIG_MMC1_CCLK_MIN;
            break;
#endif
#if USE_MMC2
        case MMC2:
            cfg_info->max_clk = CONFIG_MMC2_CCLK_MAX;
            cfg_info->min_clk = CONFIG_MMC2_CCLK_MIN;
            cfg_info->caps |= SDHCI_CAN_DO_8BITBUS;
            cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
            break;
#endif
#if USE_MMC3
        case MMC3:
            cfg_info->max_clk = CONFIG_MMC2_CCLK_MAX;
            cfg_info->min_clk = CONFIG_MMC2_CCLK_MIN;
            cfg_info->caps |= SDHCI_CAN_DO_8BITBUS;
            cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
            break;
#endif
        default:
            break;
    }
}

static void slot_init(struct hci_softc *sc)
{
    sdhci_crg_init(sc);
    sdhci_platform_init(sc);
    sdhci_platform_set_io(sc);
    return;
}

void init_slot_methods(int slot, hci_slot_ops *slot_ops)
{
    slot_ops->pre_clock = NULL;
    slot_ops->post_clock = NULL;
    slot_ops->set_clock = sdhci_set_clock;
    slot_ops->slot_init = slot_init;
    slot_ops->set_phase = NULL;
    slot_ops->enable_drv_sample = NULL;
    slot_ops->select_sample_phase = sdhci_hisi_set_samp_phase;
    slot_ops->switch_volt = sdhci_voltage_switch;
    return;
}
