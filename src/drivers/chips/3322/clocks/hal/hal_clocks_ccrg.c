/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:   HAL APP CLOCK PRIVATE SOURCE.
 */
#include "hal_clocks_ccrg.h"
#include "hal_reg_config.h"
#include "soc_osal.h"
#include "hal_clocks_app.h"

/*******************clock ccrg config*******************************/
static const uint32_t g_ccrg_modules_reg_map[HAL_CLOCKS_MODULE_MAX] = {
    COM_CTL_RB_MCU_CORE_CR_CH1_REG,
    COM_CTL_RB_MCU_PERP_LS_CR_REG,
    COM_CTL_RB_MCU_PERP_UART_CR_REG,
    COM_CTL_RB_MCU_PERP_SPI_CR_REG,
    COM_CTL_RB_COM_BUS_CR_CH1_REG,
    COM_CTL_RB_SEC_CR_REG,          // sec ip
    COM_CTL_RB_SDIOM_CR_REG,        // sdio host
    COM_CTL_RB_EMMC_CR_REG,         // emmc & fmc
    COM_CTL_RB_CAN_CR_REG,
    COM_CTL_RB_SCR_USB_BUS_ROUTER_REG,
    COM_CTL_RB_HIFI_CR_CH1_REG,
    COM_CTL_RB_CODEC_CR_CH1_REG,
    COM_CTL_RB_XIP_QSPI_CR_REG,     // norflash
    COM_CTL_RB_XIP_OPI0_CR_REG,     // psram
    COM_CTL_RB_DPU_CR_REG,          // dpu
    COM_CTL_RB_XIP_OPI1_CR_REG,     // qspi2
    COM_CTL_RB_NPU_CR_REG,          // npu
    COM_CTL_RB_PAD_CLK_OUT0_CR_REG,
    COM_CTL_RB_PAD_CLK_OUT1_CR_REG,
};

static uint32_t hal_clocks_ccrg_reg_get(hal_clocks_ccrg_module_t module, hal_clocks_crg_ch_t ch)
{
    if (module == HAL_CLOCKS_MODULE_MAX) {
        return 0;
    }
    uint32_t reg_addr = g_ccrg_modules_reg_map[module];

    switch (module) {
        case HAL_CLOCKS_MODULE_MCU_CORE:
            reg_addr = (ch == HAL_CLOCKS_CRG_CH0) ? COM_CTL_RB_MCU_CORE_CR_CH0_REG : COM_CTL_RB_MCU_CORE_CR_CH1_REG;
            break;
        case HAL_CLOCKS_MODULE_COM_BUS:
            reg_addr = (ch == HAL_CLOCKS_CRG_CH0) ? COM_CTL_RB_COM_BUS_CR_CH0_REG : COM_CTL_RB_COM_BUS_CR_CH1_REG;
            break;
        case HAL_CLOCKS_MODULE_HIFI:
            reg_addr = (ch == HAL_CLOCKS_CRG_CH0) ? COM_CTL_RB_HIFI_CR_CH0_REG : COM_CTL_RB_HIFI_CR_CH1_REG;
            break;
        case HAL_CLOCKS_MODULE_CODEC:
            reg_addr = (ch == HAL_CLOCKS_CRG_CH0) ? COM_CTL_RB_CODEC_CR_CH0_REG : COM_CTL_RB_CODEC_CR_CH1_REG;
            break;
        default:
            break;
    }

    return reg_addr;
}

// This is internal Interface, and the "module" must be dual ch, whick is guaranteed by the caller.
static hal_clocks_crg_ch_t hal_clocks_ccrg_current_ch_get(hal_clocks_ccrg_module_t module)
{
    return reg16_getbits(g_ccrg_modules_reg_map[module], COM_CTL_RB_MCU_CORE_CH_SEL_STS_OFFSET,
        COM_CTL_RB_MCU_CORE_CH_SEL_STS_LEN);
}

uint8_t hal_clocks_ccrg_module_get_src(hal_clocks_ccrg_module_t module)
{
#define COMM_CLK_CH_EN_POS                      0x0
#define COMM_CLK_SRC_POS                        0x1
#define COMM_CLK_SRC_LEN                        0x3

    uint32_t reg_addr;
    hal_clocks_crg_ch_t ch;

    switch (module) {
        case HAL_CLOCKS_MODULE_MCU_CORE:
        case HAL_CLOCKS_MODULE_COM_BUS:
        case HAL_CLOCKS_MODULE_HIFI:
        case HAL_CLOCKS_MODULE_CODEC:
            ch = hal_clocks_ccrg_current_ch_get(module);
            reg_addr = hal_clocks_ccrg_reg_get(module, ch);
            break;
        default:
            reg_addr = g_ccrg_modules_reg_map[module];
            break;
    }

    if (reg16_getbit(reg_addr, COMM_CLK_CH_EN_POS) == 0) {
        return CLOCKS_SRC_MAX;
    }
    return reg16_getbits(reg_addr, COMM_CLK_SRC_POS, COMM_CLK_SRC_LEN);
}

uint8_t hal_clocks_ccrg_module_get_div(hal_clocks_ccrg_module_t module)
{
#define COM_CTL_CLK_DIV_EN_OFFSET                   0x4
#define COM_CTL_CLK_DIV_NUM_OFFSET                  0x5
#define COM_CTL_CODEC_DIV_NUM_LEN                   0x7
#define CLK_DIV_1                                   0x1

    uint32_t reg_addr;
    hal_clocks_crg_ch_t ch;

    switch (module) {
        case HAL_CLOCKS_MODULE_MCU_CORE:
        case HAL_CLOCKS_MODULE_COM_BUS:
        case HAL_CLOCKS_MODULE_HIFI:
        case HAL_CLOCKS_MODULE_CODEC:
            ch = hal_clocks_ccrg_current_ch_get(module);
            reg_addr = hal_clocks_ccrg_reg_get(module, ch);
            break;
        default:
            reg_addr = g_ccrg_modules_reg_map[module];
            break;
    }

    uint8_t div_en = reg16_getbit(reg_addr, COM_CTL_CLK_DIV_EN_OFFSET);
    if (div_en == 0) {
        return CLK_DIV_1;
    }

    uint8_t div = reg16_getbits(reg_addr, COM_CTL_CLK_DIV_NUM_OFFSET, COM_CTL_CODEC_DIV_NUM_LEN);
    if (div == 0) {
        div = 1;
    }
    return div;
}

static void hal_clocks_glb_clken_config(hal_clocks_ccrg_module_t module, switch_type_t on) // todo
{
    switch (module) {
        case HAL_CLOCKS_MODULE_MCU_PERP_LS:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_MCU_PERP_LS_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_MCU_PERP_UART:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_MCU_PERP_UART_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_MCU_PERP_SPI:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_MCU_PERP_SPI_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_SCR_SEC:
            hal_reg_config_bit(COM_CTL_RB_C_CRG_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_SEC_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_SDIOM:
            hal_reg_config_bit(COM_CTL_RB_C_CRG_CLKEN_REG, on, COM_CTL_RB_CFG_COM_SDIOM_CLKEN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_EMMC:
            hal_reg_config_bit(COM_CTL_RB_C_CRG_CLKEN_REG, on, COM_CTL_RB_CFG_COM_EMMC_CLKEN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_CAN:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_CAN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_USB_BUS:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_USB_BUS_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_XIP_SFC:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_XIP_QSPI_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_XIP_OPI0:
            hal_reg_config_bit(XIP_CACHE_CTL_RB_MEM_CLKEN0_REG, on, XIP_CACHE_CTL_RB_CFG_XIPCACHE_OPI0_CLKEN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_DPU:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_DPU_CLKEN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_XIP_QSPI2:
            hal_reg_config_bit(COM_CTL_RB_C_CRG_CLKEN_REG, on, COM_CTL_RB_CFG_COM_XIP_QSPI2_CLKEN_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_NPU_CORE:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_NPU_CORE_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_HIFI:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_HIFI_OFFSET);
            break;
        case HAL_CLOCKS_MODULE_CODEC:
            hal_reg_config_bit(COM_CTL_RB_GLB_CLKEN_REG, on, COM_CTL_RB_CFG_COM_GLB_CLKEN_CODEC_OFFSET);
            break;
        default:
            return;
    }
}

static bool hal_clocks_is_dual_channel_module(hal_clocks_ccrg_module_t module)
{
    if (module == HAL_CLOCKS_MODULE_MCU_CORE || module == HAL_CLOCKS_MODULE_COM_BUS ||
        module == HAL_CLOCKS_MODULE_HIFI || module == HAL_CLOCKS_MODULE_CODEC) {
        return true;
    }
    return false;
}

static void hal_clocks_sub_normal_sel_cfg(hal_clocks_ccrg_module_t module, hal_clocks_crg_ch_t ch)
{
    if (ch == HAL_CLOCKS_CRG_CH_INVALID) {
        return;
    }
    uint8_t pos;
    switch (module) {
        case HAL_CLOCKS_MODULE_MCU_CORE:
            pos = COM_CTL_RB_CFG_COM_MCU_CORE_CH_SEL_OFFSET;
            break;
        case HAL_CLOCKS_MODULE_COM_BUS:
            pos = COM_CTL_RB_CFG_COM_COM_BUS_CH_SEL_OFFSET;
            break;
        case HAL_CLOCKS_MODULE_HIFI:
            pos = COM_CTL_RB_CFG_COM_HIFI_CH_SEL_OFFSET;
            break;
        case HAL_CLOCKS_MODULE_CODEC:
            pos = COM_CTL_RB_CFG_COM_CODEC_CH_SEL_OFFSET;
            break;
        default:
            return;
    }

    if (ch == HAL_CLOCKS_CRG_CH1) {
        hal_reg_config_bit(COM_CTL_RB_C_CRG_CH_SEL_REG, true, (REG16_POS)pos);
    } else {
        hal_reg_config_bit(COM_CTL_RB_C_CRG_CH_SEL_REG, false, (REG16_POS)pos);
    }
}

/***************mcu bus clk div config*****************/
void hal_clocks_ccrg_mcu_bus_div_set(uint8_t div)
{
    uint8_t temp_div = MAX(1, div);
    uint32_t irq_sts = osal_irq_lock();
    reg16_clrbit(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_LOAD_DIV_CLKEN_OFFSET);
    reg16_setbits(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_OFFSET,
        M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_LEN, temp_div);
    reg16_setbit(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_LOAD_DIV_CLKEN_OFFSET);
    osal_irq_restore(irq_sts);
}

uint8_t hal_clocks_ccrg_mcu_bus_div_get(void)
{
    return reg16_getbits(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_OFFSET,
        M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_LEN);
}

/***************ccrg clk config*****************/
static void hal_clocks_ccrg_module_ch_disable(hal_clocks_ccrg_module_t module, hal_clocks_crg_ch_t ch)
{
    uint32_t reg_addr = hal_clocks_ccrg_reg_get(module, ch);
    if (reg_addr != 0) {
        writew(reg_addr, 0);
    }
}

static void hal_clocks_ccrg_module_config(hal_clocks_ccrg_module_t module, hal_clocks_crg_ch_t ch, \
                                          uint8_t clk_src, uint8_t clk_div)
{
#define COM_CTL_CLK_CH_EN_OFFSET                    0x0
#define COM_CTL_CLK_CH_SLE_OFFSET                   0x1
#define COM_CTL_CLK_CH_SLE_LEN                      0x3
#define COM_CTL_CLK_DIV_EN_OFFSET                   0x4
#define COM_CTL_CLK_DIV_NUM_OFFSET                  0x5
#define COM_CTL_CODEC_DIV_NUM_LEN                   0x7
#define CLK_DIV_1                                   0x1

    uint32_t reg_addr = hal_clocks_ccrg_reg_get(module, ch);
    if (reg_addr == 0) {
        return;
    }

    // Disable the clock.
    reg16_clrbit(reg_addr, COM_CTL_CLK_CH_EN_OFFSET);
    // Select the clock source.
    reg16_setbits(reg_addr, COM_CTL_CLK_CH_SLE_OFFSET, COM_CTL_CLK_CH_SLE_LEN, clk_src);
    // Set the clock divisor.
    reg16_clrbit(reg_addr, COM_CTL_CLK_DIV_EN_OFFSET);
    if (clk_div > CLK_DIV_1) {
        reg16_setbits(reg_addr, COM_CTL_CLK_DIV_NUM_OFFSET, COM_CTL_CODEC_DIV_NUM_LEN, clk_div);
        reg16_setbit(reg_addr, COM_CTL_CLK_DIV_EN_OFFSET);
    } else {
        reg16_setbits(reg_addr, COM_CTL_CLK_DIV_NUM_OFFSET, COM_CTL_CODEC_DIV_NUM_LEN, 0);
        reg16_clrbit(reg_addr, COM_CTL_CLK_DIV_EN_OFFSET);
    }
    // Enable the clock.
    reg16_setbit(reg_addr, COM_CTL_CLK_CH_EN_OFFSET);
}

void hal_clocks_ccrg_switch(hal_clocks_ccrg_module_t module, hal_clocks_ccrg_clk_src_t clk_source, uint8_t div)
{
    if (module >= HAL_CLOCKS_MODULE_MAX) {
        return;
    }
    if (clk_source == HAL_CLOCKS_CCRG_CLK_SRC_MAX) {
        // Disable the clock.
        hal_clocks_glb_clken_config(module, TURN_OFF);
        hal_clocks_ccrg_module_ch_disable(module, HAL_CLOCKS_CRG_CH0);
        if (hal_clocks_is_dual_channel_module(module)) {
            hal_clocks_ccrg_module_ch_disable(module, HAL_CLOCKS_CRG_CH1);
        }
        return;
    }
    if (hal_clocks_is_dual_channel_module(module)) {
        // Get target ch.
        hal_clocks_crg_ch_t cur_ch = hal_clocks_ccrg_current_ch_get(module);
        hal_clocks_crg_ch_t target_ch = (cur_ch == HAL_CLOCKS_CRG_CH0 ? HAL_CLOCKS_CRG_CH1 : HAL_CLOCKS_CRG_CH0);
        // Config the clock.
        hal_clocks_ccrg_module_config(module, target_ch, (uint8_t)clk_source, div);
        // Switch to target ch.
        hal_clocks_sub_normal_sel_cfg(module, target_ch);
        // Switch the current ch to 32M
        hal_clocks_ccrg_module_config(module, cur_ch, HAL_CLOCKS_CCRG_CLK_SRC_XO_RC_32M, 0x1);
    } else {
        // Disable the clock.
        hal_clocks_glb_clken_config(module, TURN_OFF);
        // Config the clock.
        hal_clocks_ccrg_module_config(module, HAL_CLOCKS_CRG_CH_INVALID, (uint8_t)clk_source, div);
    }
    // Enable the clock.
    hal_clocks_glb_clken_config(module, TURN_ON);
}