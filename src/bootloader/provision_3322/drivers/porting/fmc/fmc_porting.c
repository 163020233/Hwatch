/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides dma port \n
 *
 * History: \n
 * 2024-6-26， Create file. \n
 */

#include "soc_reg.h"
#include "errcode.h"
#include "common_def.h"
#include "chip_io.h"
#include "platform_core.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "fmc_porting.h"

#define TCXO_DELAY_400 400
#define TCXO_DELAY_10 10
#define TCXO_DELAY_50 50
#define TCXO_DELAY_240UL 240UL

static uint8_t g_fmc_init = 0;

void fmc_reg_config(void)
{
    // power on mcu hs sub
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_ON_SEQ_REG, LP_CTL_MCU_RB_MCU_HS_PWR_EN_MCU_MAN_FRC_ON_SEQ_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_400);

    reg16_setbit(M_CTL_RB_HS_BUS_CLK_EN_REG, M_CTL_RB_CFG_M_HS_SUB_BUS_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    // mcu hs sub soft rst
    if (reg32_getbits(GLB_CTL_M_RB_SOFT_RST_6_REG, 0, 0x2) != 0x3) { // len 2
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_CRG_N_OFFSET);
        uapi_tcxo_delay_us(TCXO_DELAY_50);
        reg16_setbit(GLB_CTL_M_RB_SOFT_RST_6_REG, GLB_CTL_M_RB_CFG_GLBM_SOFT_RST_GLB_SDIO_LGC_N_OFFSET);
        uapi_tcxo_delay_us(TCXO_DELAY_50);
    } else {
        uapi_tcxo_delay_us(TCXO_DELAY_50);
    }
    // fmc router
    reg16_clrbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    writew(COM_CTL_RB_EMMC_CR_REG, 1);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);

    // hs fmc clken
    reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_FMC_BUS_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbits(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_NUM_OFFSET, HS_CTL_RB_CFG_HS_FMC_DIV_NUM_LEN, 0x1);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_10);

#define MODE_QSPI1_VAL  0x2
    
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d0 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO1_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d1 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO2_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d0 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO3_MODE_REG, MODE_QSPI1_VAL); /* qspi1_d1 */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO4_MODE_REG, MODE_QSPI1_VAL); /* qspi1_cs */
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO9_MODE_REG, MODE_QSPI1_VAL); /* qspi1_clk */
    // LDO1-SW0使能, 如果Nand使用板级1p8, 该处使能不用打开。
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_0_EN_OFFSET);
    uapi_tcxo_delay_us(TCXO_DELAY_240UL);
}

void pmu_reg_config(void)
{
    reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_FMC_BUS_CLKEN_OFFSET);
}

static void fmc_port_clock_config(void)
{
    pmu_reg_config();
    fmc_reg_config();
    /* disable FMC */
    reg16_clrbit(HS_CTL_RB_HS_SOFT_RST_N_REG, HS_CTL_RB_CFG_HS_SOFT_RST_FMC_N_OFFSET);
    /* enable FMC */
    reg16_setbit(HS_CTL_RB_HS_SOFT_RST_N_REG, HS_CTL_RB_CFG_HS_SOFT_RST_FMC_N_OFFSET);
}

static void fmc_port_config_pinmux(void)
{
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_D0
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO1_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_D1
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO2_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_D2
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO3_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_D3
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO4_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_CS
    writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO9_MODE_REG, HAL_PIO_FUNC_QSPI1_M2);  // QSPI_CLK
}

void fmc_port_init(void)
{
    fmc_port_clock_config();
    fmc_port_config_pinmux();
    g_fmc_init = 1;
}
