/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:  HAL APP PMU DRIVER.
 */

#include "hal_pmu_peripheral.h"
#include "non_os.h"
#include "soc_osal.h"
#include "chip_io.h"
#include "hal_clocks_app.h"
#include "hal_reg_config.h"

static void hal_pmu_mcpu_perip_reset(hal_pmu_mcpu_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    switch_type_t on = (request == HAL_PMU_PERIP_REQUEST_RESET) ? TURN_OFF : TURN_ON;
    uint16_t bit;

    if (perip < HAL_PMU_MCPU_PERP_MTOP1_L_STATT) {
        bit = (uint16_t)perip - HAL_PMU_MCPU_PERP_MTOP0_L_STATT;
        hal_reg_config_bit(M_CTL_RB_MTOP0_L_REG, on, bit);
    } else if (perip < HAL_PMU_MCPU_PERP_MTOP2_L_STATT) {
        bit = (uint16_t)perip - HAL_PMU_MCPU_PERP_MTOP1_L_STATT;
        hal_reg_config_bit(M_CTL_RB_MTOP1_L_REG, on, bit);
    } else if (perip < HAL_PMU_MCPU_PERP_MTOP3_L_STATT) {
        bit = (uint16_t)perip - HAL_PMU_MCPU_PERP_MTOP2_L_STATT;
        hal_reg_config_bit(M_CTL_RB_MTOP2_L_REG, on, bit);
    } else if (perip < HAL_PMU_MCPU_PERP_MSOFT_RST_N1) {
        bit = (uint16_t)perip - HAL_PMU_MCPU_PERP_MTOP3_L_STATT;
        hal_reg_config_bit(M_CTL_RB_MTOP3_L_REG, on, bit);
    } else if (perip < HAL_PMU_MCPU_PERIP_MAX) {
        hal_reg_config_bit(M_CTL_RB_M_SOFT_RST_N1_REG, on, M_CTL_RB_SOFT_RST_PWM_N_OFFSET);
    }
}

static hal_clocks_app_perips_clk_type_t hal_pmu_cmu_get_perips_clk_type(hal_pmu_mcpu_perips_type_t app_clk)
{
    if (app_clk >= HAL_PMU_MCPU_PERIP_MAX) {
        return HAL_CLOCKS_APP_BUTT;
    }

    hal_clocks_app_perips_clk_type_t clk_type[HAL_PMU_MCPU_PERIP_MAX] = {
        HAL_CLOCKS_APP_TIMER_CLK,       // !< HAL_PMU_MCPU_PERIP_TIMER
        HAL_CLOCKS_APP_BUTT,            // !< HAL_PMU_MCPU_PERIP_MDMA
        HAL_CLOCKS_APP_BUTT,            // !< HAL_PMU_MCPU_PERIP_SMDMA
        HAL_CLOCKS_APP_I2C0_CLK,        // !< HAL_PMU_MCPU_PERIP_I2C0
        HAL_CLOCKS_APP_I2C1_CLK,        // !< HAL_PMU_MCPU_PERIP_I2C1
        HAL_CLOCKS_APP_I2C2_CLK,        // !< HAL_PMU_MCPU_PERIP_I2C2
        HAL_CLOCKS_APP_I2C3_CLK,        // !< HAL_PMU_MCPU_PERIP_I2C3
        HAL_CLOCKS_APP_GPIO_CLK,        // !< HAL_PMU_MCPU_PERIP_M_GPIO
        HAL_CLOCKS_APP_UART_H1_CLK,     // !< HAL_PMU_MCPU_PERIP_UART_H1
        HAL_CLOCKS_APP_SPI1_M_CLK,      // !< HAL_PMU_MCPU_PERIP_SPI1_MS
        HAL_CLOCKS_APP_SPI0_MS_CLK,     // !< HAL_PMU_MCPU_PERIP_SPI0_MS
        HAL_CLOCKS_APP_UART_H0_CLK,     // !< HAL_PMU_MCPU_PERIP_UART_H0
        HAL_CLOCKS_APP_UART_L0_CLK,     // !< HAL_PMU_MCPU_PERIP_UART_L0
        HAL_CLOCKS_APP_PWM_CLK,         // !< HAL_PMU_MCPU_PERIP_PWM
    };
    return clk_type[app_clk];
}

static void hal_pmu_mcu_perips_clk_en(hal_pmu_mcpu_perips_type_t app_clk, hal_pmu_perip_request_type_t request)
{
    switch_type_t clk_en = (request == HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER) ? TURN_ON : TURN_OFF;
    hal_clocks_app_perips_clk_type_t clk_type = hal_pmu_cmu_get_perips_clk_type(app_clk);
    if (clk_type >= HAL_CLOCKS_APP_BUTT) { return; }
    hal_clocks_app_perips_config(clk_type, clk_en);
}

void hal_pmu_mcpu_perip_config(hal_pmu_mcpu_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    uint32_t irq_sts = osal_irq_lock();
    switch (request) {
        case HAL_PMU_PERIP_REQUEST_RESET:
        case HAL_PMU_PERIP_REQUEST_DERESET:
            hal_pmu_mcpu_perip_reset(perip, request);
            break;
        case HAL_PMU_PERIP_REQUEST_ENTER_LOW_POWER:
            hal_pmu_mcu_perips_clk_en(perip, request);
            break;
        case HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER:
            // Make sure the peripheral not been reset.
            hal_pmu_mcpu_perip_reset(perip, HAL_PMU_PERIP_REQUEST_DERESET);
            hal_pmu_mcu_perips_clk_en(perip, HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER);
            break;
        default:
            break;
    }
    osal_irq_restore(irq_sts);
}

static void hal_pmu_maon_perip_reset(hal_pmu_maon_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    uint32_t addr = GLB_CTL_M_RB_GPIO_CTL_REG;
    uint16_t bit = GLB_CTL_M_RB_GPIO_RST_SOFT_N_OFFSET + (uint16_t)perip;

    if (request == HAL_PMU_PERIP_REQUEST_RESET) {
        reg16_clrbit((void *)((uintptr_t)addr), bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_DERESET) {
        reg16_setbit((void *)((uintptr_t)addr), bit);
    }
}

static void hal_pmu_maon_perip_sleep(hal_pmu_maon_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    uint32_t addr = GLB_CTL_M_RB_GPIO_CTL_REG;
    uint16_t bit = GLB_CTL_M_RB_GPIO_CLK_EN_OFFSET + (uint16_t)perip;

    if (request == HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER) {
        reg16_setbit((void *)((uintptr_t)addr), bit);
    } else if (request == HAL_PMU_PERIP_REQUEST_ENTER_LOW_POWER) {
        reg16_clrbit((void *)((uintptr_t)addr), bit);
    }
}

void hal_pmu_maon_perip_config(hal_pmu_maon_perips_type_t perip, hal_pmu_perip_request_type_t request)
{
    uint32_t irq_sts = osal_irq_lock();
    switch (request) {
        case HAL_PMU_PERIP_REQUEST_RESET:
        case HAL_PMU_PERIP_REQUEST_DERESET:
            hal_pmu_maon_perip_reset(perip, request);
            break;
        case HAL_PMU_PERIP_REQUEST_ENTER_LOW_POWER:
            hal_pmu_maon_perip_sleep(perip, request);
            break;
        case HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER:
            // Make sure the peripheral not been reset.
            hal_pmu_maon_perip_reset(perip, HAL_PMU_PERIP_REQUEST_DERESET);
            hal_pmu_maon_perip_sleep(perip, HAL_PMU_PERIP_REQUEST_EXIT_LOW_POWER);
            break;
        default:
            break;
    }
    osal_irq_restore(irq_sts);
}
