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
#include "pmu_sub_control.h"
#include "platform_section.h"
#include "fmc_porting.h"

PLT_SEC_DTCM_B static uint8_t g_fmc_init = 0;
PLT_SEC_DTCM_B static uint16_t g_fmc_ctl = 0;

static void fmc_port_clock_config(void)
{
    uapi_pmu_hs_sub_control(HS_SUB_FMC_ID, PMU_POWER_ON_AND_RUN);

    /* fmc clock en */
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
    reg16_clrbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    reg16_setbits(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_NUM_OFFSET, HS_CTL_RB_CFG_HS_FMC_DIV_NUM_LEN, 0x1);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_LOAD_DIV_EN_OFFSET);
    reg16_setbit(HS_CTL_RB_HS_FMC_DIV_REG, HS_CTL_RB_CFG_HS_FMC_DIV_CLKEN_OFFSET);
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

static void fmc_port_config_pad(void)
{
#ifdef TEST_SUITE
    uapi_pin_set_pull(S_HGPIO0, PIN_PULL_DOWN);  // QSPI_D0
    uapi_pin_set_pull(S_HGPIO1, PIN_PULL_UP);  // QSPI_D1
    uapi_pin_set_pull(S_HGPIO2, PIN_PULL_UP);  // QSPI_D2
    uapi_pin_set_pull(S_HGPIO3, PIN_PULL_UP);  // QSPI_D3
    uapi_pin_set_pull(S_HGPIO4, PIN_PULL_UP);  // QSPI_CS
    uapi_pin_set_pull(S_HGPIO9, PIN_PULL_UP);  // QSPI_CLK

    uapi_pin_set_ds(S_HGPIO0, PIN_DS_2);  // QSPI_D0
    uapi_pin_set_ds(S_HGPIO1, PIN_DS_2);  // QSPI_D1
    uapi_pin_set_ds(S_HGPIO2, PIN_DS_2);  // QSPI_D2
    uapi_pin_set_ds(S_HGPIO3, PIN_DS_2);  // QSPI_D3
    uapi_pin_set_ds(S_HGPIO4, PIN_DS_2);  // QSPI_CS
    uapi_pin_set_ds(S_HGPIO9, PIN_DS_2);  // QSPI_CLK
#endif
}

void fmc_port_init(void)
{
    fmc_port_config_pad();
    fmc_port_clock_config();
    fmc_port_config_pinmux();
    g_fmc_init = 1;
}

void fmc_clock_div_set(uint8_t div)
{
    unused(div);
}

void fmc_io_clock_div_set(uint8_t div)
{
    unused(div);
}

errcode_t uapi_fmc_suspend(uintptr_t arg)
{
    unused(arg);
    if (g_fmc_init == 0) {
        return ERRCODE_SUCC;
    }
    g_fmc_ctl = readw(FMC_CTL_BASE_ADDR);
    uapi_pmu_hs_sub_control(HS_SUB_FMC_ID, PMU_POWER_OFF);
    return ERRCODE_SUCC;
}

errcode_t uapi_fmc_resume(uintptr_t arg)
{
    unused(arg);
    if (g_fmc_init == 0) {
        return ERRCODE_SUCC;
    }
    fmc_port_init();
    writew(FMC_CTL_BASE_ADDR, g_fmc_ctl);
    return ERRCODE_SUCC;
}
