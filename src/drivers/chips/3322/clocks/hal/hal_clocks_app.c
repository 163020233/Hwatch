/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:   HAL APP CLOCK DRIVER.
 */

#include "hal_clocks_app.h"
#include "hal_reg_config.h"

void hal_clocks_mcrg_aux_adc_div_set(uint8_t div)
{
    uint8_t temp_div = MAX(1, div);
    reg16_setbit(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_EN_OFFSET);
    reg16_clrbit(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_LOAD_EN_OFFSET);
    reg16_setbits(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_NUM_OFFSET,
        M_CTL_RB_CFG_M_AUX_ADC_DIV_NUM_LEN, temp_div);
    reg16_setbit(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_LOAD_EN_OFFSET);
    reg16_clrbit(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_LOAD_EN_OFFSET);
}

uint8_t hal_clocks_mcrg_aux_adc_div_get(void)
{
    return (uint8_t)reg16_getbits(M_CTL_RB_M_DIV3_REG, M_CTL_RB_CFG_M_AUX_ADC_DIV_NUM_OFFSET,
        M_CTL_RB_CFG_M_AUX_ADC_DIV_NUM_LEN);
}

/***************pll clk en config*****************/
void hal_clocks_pll_module_clken(clocks_pll_src_t pll_src, switch_type_t clk_en)
{
    switch (pll_src) {
        case CLOCKS_PLL_SRC_DLL:
            hal_reg_config_bit(COM_CTL_RB_COM_DLL2_REG, clk_en, (REG16_POS)COM_CTL_RB_CFG_COM_XO_DLL2_CLKEN_OFFSET);
            return;
        case CLOCKS_PLL_SRC_FNPLL:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, clk_en, (REG16_POS)COM_CTL_RB_CFG_COM_FNPLL_CLKEN_OFFSET);
            return;
        case CLOCKS_PLL_SRC_FNPLL_VCO:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, clk_en, (REG16_POS)COM_CTL_RB_CFG_COM_FNPLL_VCO_CLKEN_OFFSET);
            hal_reg_config_bit(LP_CTL_GLB_RB_LP_CTL_GLB_CMU_3_REG, clk_en, POS_0);
            reg16_setbits(COM_CTL_RB_C_CRG_DIV_REG, COM_CTL_RB_CFG_COM_FNPLL_VCO_DIV_OFFSET,
                          COM_CTL_RB_CFG_COM_FNPLL_VCO_DIV_LEN, 0x2);
            reg16_setbits(COM_CTL_RB_C_CRG_DIV_REG, COM_CTL_RB_CFG_COM_FNPLL_VCO_PSRAM_DIV_OFFSET,
                          COM_CTL_RB_CFG_COM_FNPLL_VCO_PSRAM_DIV_LEN, 0x2);
            return;
        default:
            return;
    }
}

/*******************clock perp config*******************************/
void hal_clocks_app_perips_config(hal_clocks_app_perips_clk_type_t app_clk, switch_type_t on)
{
    if (app_clk < HAL_CLOCKS_APP_MCLK_EN1_BASE) {
        hal_reg_config_bit(M_CTL_RB_M_CLKEN0_REG, on, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN0_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_MCLK_EN2_BASE) {
        hal_reg_config_bit(M_CTL_RB_M_CLKEN1_REG, on, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN1_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_XIP_BASE) {
        hal_reg_config_bit(M_CTL_RB_M_CLKEN2_REG, on, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN2_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_BUTT) {
        hal_reg_config_bit(XIP_CACHE_CTL_RB_MEM_CLKEN0_REG, on, (REG16_POS)(app_clk - HAL_CLOCKS_APP_XIP_BASE));
    }
}

bool hal_clocks_app_perips_get_config(hal_clocks_app_perips_clk_type_t app_clk)
{
    if (app_clk < HAL_CLOCKS_APP_MCLK_EN1_BASE) {
        return reg16_getbit(M_CTL_RB_M_CLKEN0_REG, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN0_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_MCLK_EN2_BASE) {
        return reg16_getbit(M_CTL_RB_M_CLKEN1_REG, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN1_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_XIP_BASE) {
        return reg16_getbit(M_CTL_RB_M_CLKEN2_REG, (REG16_POS)(app_clk - HAL_CLOCKS_APP_MCLK_EN2_BASE));
    } else if (app_clk < HAL_CLOCKS_APP_BUTT) {
        return reg16_getbit(XIP_CACHE_CTL_RB_MEM_CLKEN0_REG, (REG16_POS)(app_clk - HAL_CLOCKS_APP_XIP_BASE));
    }
    return false;
}