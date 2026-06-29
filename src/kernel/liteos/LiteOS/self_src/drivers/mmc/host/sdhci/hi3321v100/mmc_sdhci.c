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
#include "hci.h"
#include "sdhci/sdhci.h"
#include "sdhci/sdhci_private.h"
#include "hci_helper.h"
#include "mmc_adapt.h"

void suqin_hal_reg_config_bit(uint32_t addr, suqin_switch_type_t on, SUQIN_REG16_POS position)
{
    if (on) {
        suqin_reg16_setbit ((void *)((uintptr_t) addr), position);
    } else {
        suqin_reg16_clrbit ((void *)((uintptr_t) addr), position);
    }
}

void suqin_hal_clocks_config_bit(uint32_t reg_addr, suqin_switch_type_t on, SUQIN_REG16_POS position)
{
    if (on) {
        suqin_reg16_setbit((void *)((uintptr_t)reg_addr), position);
    } else {
        suqin_reg16_clrbit((void *)((uintptr_t)reg_addr), position);
    }
}

void suqin_hal_clocks_sdio_crg_rst_control(suqin_switch_type_t rst_ctl)
{
    suqin_hal_reg_config_bit(CMU_CLOCKS_SDIO_CRG_SOFT_RST_REG, rst_ctl, (SUQIN_REG16_POS)CMU_CLOCKS_SDIO_RST_DEVICE_SD_BIT);
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
    uint32_t pwr = SDHCI_POWER_180;

    switch(vccq) {
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

static void hi_mmc_set_sd_bus_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;
    value = GET_UINT16(div_reg);
    value &= ~SD_BUS_LOAD_DIV_EN;
    WRITE_UINT16(value, div_reg);
    value &= ~SD_BUS_DIV_NUM_MASK;
    value |= (uint16_t)(freq_div << SD_BUS_DIV_NUM_POST_SHIFT);
    WRITE_UINT16(value, div_reg);
    value |= SD_BUS_LOAD_DIV_EN;
    WRITE_UINT16(value, div_reg);
}

static void hi_mmc_set_sdiom_src_clk(uint32_t cfg_clock, uint32_t *freq_div, uint32_t *second_freq_div)
{
    uint32_t clock;
    uint32_t clk_freq;
    uint32_t clk_resource;

    uint8_t index;
    const uint8_t second_reg_max_div = 8;

    uint32_t val = GET_UINT16(SDIOM_CR_REG);
    clk_resource = (val & 0x0F) >> 1;

    if (clk_resource == SDIOM_CR_CTRL_CLK_RESOURCE_PLL) {
        uint32_t div = (val >> SDIOM_CR_CTRL_DIV_OFFSET) & 0x0F;
        if (div < SDIOM_CR_CTRL_MIN_DIV) {
            div = SDIOM_CR_CTRL_MIN_DIV;
        }

        clk_freq = 884736000UL; /* external clock source */
        clk_freq = (clk_freq / div);
    } else if (clk_resource == SDIOM_CR_CTRL_CLK_RESOURCE_XO) {
        clk_freq = SDIOM_SRC_CLK_LOW_FREQ_VAL;
    } else {
        dprintf("%s, err clk resource:[%u]\r\n", __FUNCTION__, clk_resource);
        return;
    }

    for (index = 1; index < second_reg_max_div; ++index) {
        *second_freq_div = index;
        clock = clk_freq / (*second_freq_div);
        *freq_div = clock / cfg_clock;
        if (*freq_div <= 0xFF) {
            break;
        }
    }

#if (EMMC_BOARD_DEBUG_ON)
    if (*freq_div > 0xFF) {
        dprintf("%s, div:[%u] error\r\n", __FUNCTION__, *freq_div);
    }
    dprintf("clk_level:[%u], freq_div:[%u], second_freq_div:[%u], cfg_clock: [%u]!\n",
        clk_freq, *freq_div, *second_freq_div, cfg_clock);
#endif
}

static void hi_mmc_set_first_level_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;

    value = GET_UINT16(div_reg);
    value &= ~SDIO_EMMC_CCLK_EN;
    value &= ~SDIO_EMMC_ORI_DIV_EN;
    WRITE_UINT16(value, div_reg);

    value &= ~SDIO_EMMC_DIV_NUM_MASK;
    value |= (uint16_t)(freq_div << SDIO_EMMC_DIV_NUM_POST_SHIFT);
    value |= EMMC_CARD_CLK_INV_SET;
    WRITE_UINT16(value, div_reg);

    value |= EMMC_CCLK_LOAD_DIV_EN;
    WRITE_UINT16(value, div_reg);
    value |= EMMC_EMMC_CCLK_EN;
    WRITE_UINT16(value, div_reg);

    value = GET_UINT16(div_reg);
    dprintf("%s, reg addr[0x%x], value:[0x%x], freq_div:[%u]\r\n", __FUNCTION__, div_reg, value, freq_div);
}

static void hi_mmc_set_second_level_clk_div(uint32_t div_reg, uint32_t freq_div)
{
    uint16_t value;

    value = GET_UINT16(div_reg);
    value &= ~SDIO_EMMC_CCLK_EN;
    value &= ~SDIO_EMMC_ORI_DIV_EN;
    WRITE_UINT16(value, div_reg);
    if (div_reg == EMMC_DIV_REG) {
        value &= ~EMMC_DIV_NUM_MASK;
        value |= (uint16_t)(freq_div << SDIO_EMMC_DIV_NUM_POST_SHIFT);
    } else {
        value &= ~SDIO_DIV_NUM_MASK;
        value |= (uint16_t)(freq_div << SDIO_DIV_NUM_POST_SHIFT);
    }
    WRITE_UINT16(value, div_reg);
    value |= EMMC_CCLK_LOAD_DIV_EN;
    WRITE_UINT16(value, div_reg);
    value |= EMMC_EMMC_CCLK_EN;
    WRITE_UINT16(value, div_reg);
}

uint32_t hi_mmc_set_clock_with_way2(struct hci_softc *sc, uint32_t cfg_clock)
{
    uint32_t first_freq_div;
    uint32_t freq_div = 0;
    uint32_t second_freq_div;
    uint32_t sec_freq_div = 1;

    hi_mmc_set_sdiom_src_clk(cfg_clock, &freq_div, &sec_freq_div);
    if (sc->num == 0) {
        first_freq_div = EMMC_CCLK_DIV_REG;
        second_freq_div = EMMC_DIV_REG;
        hi_mmc_set_first_level_clk_div(first_freq_div, freq_div);
        hi_mmc_set_second_level_clk_div(second_freq_div, sec_freq_div);
    }

    hi_mmc_set_sd_bus_clk_div(SDIO_BUS_CGDIV_REG, 1);

    return 0;
}

static uint32_t hal_mmc_set_soc_clock(struct hci_softc *sc, uint32_t cfg_clock)
{
    return hi_mmc_set_clock_with_way2(sc, cfg_clock);
}

static void sdhci_use_pll_clk()
{
    uint16_t val;
    const uint16_t div = 3;

    /* disable sdiom clken */
    WRITE_UINT16(GLB_CLKEN_DISABLE, GLB_CLKEN_REG);
    /* set clk_sdiom=clk_pll/3 */
    val = SDIOM_CR_CTRL_CH_ENABLE | SDIOM_CR_CTRL_CLK_RESOURCE_PLL_EN | SDIOM_CR_CTRL_DIV_EN;
    val |= div << SDIOM_CR_CTRL_DIV_OFFSET;
    WRITE_UINT16(val, SDIOM_CR_REG);
    /* enable sdiom clken */
    WRITE_UINT16(GLB_CLKEN_ENABLE, GLB_CLKEN_REG);
}

static uint32_t sdhci_set_clock(struct hci_softc *sc, uint32_t clock)
{
    uint32_t ret;
    static uint8_t inited = false;

    if (clock == 0) {
        device_printf(sc->dev, "set clock invalid param!\n");
        return clock;
    }

    if (clock > SDIOM_SRC_CLK_FREQ_LEVEL_50M_VAL) {
        sdhci_use_pll_clk();
    }

#if EMMC_BOARD_DEBUG_ON
    dprintf("%s, clock: [%u], resource:[0x%x]!\n", __FUNCTION__, clock, GET_UINT16(SDIOM_CR_REG));
#endif

    ret = hal_mmc_set_soc_clock(sc, clock);
    if (ret != 0) {
        device_printf(sc->dev, "set clock fail!\n");
        return 0;
    }

    if ((sc->num == 0) && !inited) {
        uint8_t val = hci_readb(sc, SDHCI_AXI_MBIIU_CTRL);
        val = SDHCI_AXI_MBIIU_BURST_INCR16_EN;
        hci_writeb(sc, SDHCI_AXI_MBIIU_CTRL, val);
        inited = true;
    }

    return clock;
}

static void sdhci_set_drv_phase(uint32_t host_id, uint32_t phase)
{
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
    int value;

    value = hci_readw(sc, SDHCI_EMMC_CTRL);

    if (en) {
        value |= SDHCI_EMMC_CTRL_ENH_STROBE_EN;
    } else {
        value &= ~(SDHCI_EMMC_CTRL_ENH_STROBE_EN);
    }
    hci_writew(sc, SDHCI_EMMC_CTRL, value);
}

static void sdhci_subplatform_init(const struct hci_softc *sc, hci_caps *cfg_info)
{
    device_printf(sc->dev, "LOSCFG_PLATFORM_HI3321V100 selected\n");
    if (sc->num == 0) { /* mmc0: eMMC or SD card */
        cfg_info->max_clk = SDIOM_SRC_CLK_FREQ_LEVEL_98M_VAL;
        cfg_info->min_clk = CONFIG_MMC0_CCLK_MIN;
        sdhci_enhanced_strobe(sc, false);
#ifdef LOSCFG_DRIVERS_EMMC
        /* overwrite the SDHCI standard because of the HISI platform quirk */
        device_printf(sc->dev, "LOSCFG_DRIVERS_EMMC selected\n");
        cfg_info->caps |= HCI_SLOTTYPE_EMBEDDED;
        cfg_info->caps |= HCI_CAN_DO_8BITBUS;
        cfg_info->quirks |= SDHCI_QUIRK_SIG_1V8_ONLY; /* signal 1.2V unsupported */
#else
        /* set the HCI_CAN_SDR104 will result in HS200 being set. */
        /* add quirk to disable it */
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_MMC_HS200;
        cfg_info->quirks |= SDHCI_QUIRK_SIG_3V3_ONLY; /* signal 1.2V 1.8V unsupported */
        /* HCI_QUIRK_ONLY_1_BIT_DATA is used for some customers who only needs 1 bit data bus width */
        cfg_info->caps &= ~HCI_CAN_VDD_180;
#endif
    } else if (sc->num == 1) { /* SDIO device */
        cfg_info->max_clk = CONFIG_MMC1_CCLK_MAX;
        cfg_info->min_clk = CONFIG_MMC1_CCLK_MIN;
        cfg_info->caps |= HCI_SLOTTYPE_SDIO;
        cfg_info->quirks |= SDHCI_QUIRK_SIG_3V3_ONLY; /* signal 1.2V 1.8V unsupported */
        cfg_info->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
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

static void sdhci_set_emmc_ctrl(const struct hci_softc *sc)
{
    int value;

    value = hci_readw(sc, SDHCI_EMMC_CTRL);
    value |= SDHCI_EMMC_CTRL_EMMC;
    hci_writew(sc, SDHCI_EMMC_CTRL, value);
}

static void sdhci_extra_init(struct hci_softc *sc)
{
    uint32_t val;

    printf("%s enter\r\n", __FUNCTION__);

    val = hci_readb(sc, SDHCI_MSHC_CTRL);
    val &= ~SDHCI_CMD_CONFLIT_CHECK;
    hci_writeb(sc, SDHCI_MSHC_CTRL, val);

    if (sc->num == 0) {
        sdhci_set_emmc_ctrl(sc);
    }
}

void suqin_hal_clocks_glb_sdio_soft_reset_control(hal_glb_sdio_soft_rst_type_t hal_glb_sdio_soft_rst,
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
    suqin_hal_clocks_config_bit(reg_addr, (suqin_switch_type_t)rst_control, (SUQIN_REG16_POS)offset);
}

void suqin_pmu_sdio_glb_common_reset(bool turn_on)
{
    if (turn_on) {
        suqin_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_DERESET);
        suqin_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_DERESET);
        suqin_hal_clocks_sdio_crg_rst_control((suqin_switch_type_t)HAL_SOFT_RST_DERESET);
    } else {
        suqin_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_CRG, HAL_SOFT_RST_RESET);
        suqin_hal_clocks_glb_sdio_soft_reset_control(HAL_SOFT_RST6_SDIO_LGC, HAL_SOFT_RST_RESET);
        suqin_hal_clocks_sdio_crg_rst_control((suqin_switch_type_t)HAL_SOFT_RST_RESET);
    }
}

static void suqin_hal_pmu_mcpu_mtop0_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        suqin_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP0_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        suqin_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP0_L_REG, bit);
    }
}

static void suqin_hal_pmu_mcpu_mtop1_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        suqin_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP1_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        suqin_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP1_L_REG, bit);
    }
}

static void suqin_hal_pmu_mcpu_mtop2_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        suqin_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP2_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        suqin_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP2_L_REG, bit);
    }
}

static void suqin_hal_pmu_mcpu_mtop3_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        suqin_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP3_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        suqin_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP3_L_REG, bit);
    }
}

static void suqin_hal_pmu_mcpu_mtop4_perip_reset(hal_pmu_mcpu_perips_type_t perip,
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
        suqin_reg16_clrbit(PMU_TOP_PERIPHERALS_MTOP4_L_REG, bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        suqin_reg16_setbit(PMU_TOP_PERIPHERALS_MTOP4_L_REG, bit);
    }
}

void suqin_hal_pmu_mcpu_perip_reset(hal_pmu_mcpu_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    if (perip <= HAL_PMU_MCPU_PERIP_MDMA) {
        suqin_hal_pmu_mcpu_mtop0_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_I2C_BUS_3) {
        suqin_hal_pmu_mcpu_mtop1_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_QSPI2_1CS) {
        suqin_hal_pmu_mcpu_mtop2_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_UART_L0) {
        suqin_hal_pmu_mcpu_mtop3_perip_reset(perip, request);
    } else if (perip <= HAL_PMU_MCPU_PERIP_SDIO_HOST) {
        suqin_hal_pmu_mcpu_mtop4_perip_reset(perip, request);
#if CHIP_SUSONG
    } else if (perip <= HAL_PMU_MCPU_PERIP_SPI_M3) {
        hal_pmu_mcpu_mtop5_perip_reset(perip, request);
#endif
    } else {
        return;
    }
}

static void sdhci_slot_init(struct hci_softc *sc)
{
    static uint8_t inited = false;
    dprintf("%s enter, sc_num:[%u]\r\n", __FUNCTION__, sc->num);

    if (!inited) {
        suqin_pmu_sdio_glb_common_reset(true);
        inited = true;
    }
    suqin_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_RESET);
    suqin_hal_pmu_mcpu_perip_reset(HAL_PMU_MCPU_PERIP_EMMC, HAL_PMU_PERIP_REQUEST_DERESET);

    sdhci_extra_init(sc);
    sdhci_platform_init(sc);
}

static void sdhci_en_drv_dll_sample(struct hci_softc *sc, uint32_t en)
{
    uint16_t val;

#if EMMC_BOARD_DEBUG_ON
    dprintf("%s, en:[%u]\r\n", __FUNCTION__, en);
#endif

    if (en) {
        hci_setl(sc, SDHCI_AUTO_TUNING_CTRL, SDHCI_SW_TUNING_EN);

        val = GET_UINT16(EMMC_CCLK_DIV_REG);
        val |= EMMC_TUNING_COARSE_MODE;
        WRITE_UINT16(val, EMMC_CCLK_DIV_REG);

        WRITE_UINT16(EMMC_BM0_VAL, EMMC_TUNING_BM0_REG);
        WRITE_UINT16(EMMC_BM1_VAL, EMMC_TUNING_BM1_REG);
    } else {
        hci_clearl(sc, SDHCI_AUTO_TUNING_CTRL, SDHCI_SW_TUNING_EN);
    }
}

static void sdhci_set_sample_phase(struct hci_softc *sc, uint32_t phase)
{
    uint32_t fields;

    fields = hci_readl(sc, SDHCI_AUTO_TUNING_STATUS);
    fields &= ~SDHCI_CENTER_PH_CODE_MASK;
    fields |= (uint32_t)phase;
    hci_writel(sc, SDHCI_AUTO_TUNING_STATUS, fields);
}

static void sdhci_set_mmc_io_driver(const struct hci_softc *sc, uint32_t host_id, enum mmc_bus_timing timing)
{
}

static void sdhci_set_phase(struct hci_softc *sc, uint32_t timing)
{
    uint32_t drv_phase, phase;

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

#if EMMC_BOARD_DEBUG_ON
    device_printf(sc->dev, "timing:[%u], phase: 0x%08x, drv_phase: 0x%08x\n", timing, phase, drv_phase);
#endif

    sdhci_set_drv_phase(sc->num, drv_phase);
    sdhci_en_drv_dll_sample(sc, true);
    sdhci_set_sample_phase(sc, phase);
    sdhci_set_mmc_io_driver(sc, sc->num, timing);
}

void sdhci_pre_set_clock(struct hci_softc *sc)
{
    uint32_t value;

    /* open the clk of card. */
    value = hci_readw(sc, SDHCI_CLOCK_CONTROL);
    value |= (SDHCI_CLOCK_INT_EN | SDHCI_CLOCK_PLL_EN | SDHCI_CLOCK_CARD_EN);
    hci_writew(sc, SDHCI_CLOCK_CONTROL, value);
}

void sdhci_post_clock(struct hci_softc *sc)
{
}

static void sdhci_select_sample_phase(struct hci_softc *sc, uint32_t phase)
{
    printf("%s enter, phase:[%u]\r\n", __FUNCTION__, phase);

    sdhci_card_clk_ctrl(sc, 0);
    sdhci_set_sample_phase(sc, phase);
    sdhci_card_clk_ctrl(sc, 1);
    LOS_Udelay(1);
}

void mmc_dump_regs(struct hci_softc *sc)
{

}

void init_slot_methods(int slot, hci_slot_ops *slot_ops)
{
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
}

