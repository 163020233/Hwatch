/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: psram porting source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-06-01, Create file. \n
 */

#include "stdint.h"
#include "hal_psram.h"
#include "soc_reg.h"
#include "psram_v2.h"
#include "chip_io.h"
#include "common_def.h"
#ifndef BUILD_ATE_IMAGE
#include "clocks_ccrg.h"
#endif
#include "psram_porting.h"

#define PSRAM_0_PHY_BASR_ADDR 0x520E6000
#define PSRAM_0_DMC_BASE_ADDR 0x520E5000
#define PSRAM_0_AXI_BASE_ADDR 0x520E0000

static const uintptr_t g_psram_phy_regs_base_addr = PSRAM_0_PHY_BASR_ADDR;
static const uintptr_t g_psram_dmc_regs_base_addr = PSRAM_0_DMC_BASE_ADDR;
static const uintptr_t g_psram_axi_regs_base_addr = PSRAM_0_AXI_BASE_ADDR;
static const uintptr_t g_psram_dmc_cfg_sfc_regs_base_addr = PSRAM_0_DMC_BASE_ADDR + 0xB0;
static const uintptr_t g_psram_dmc_debug_regs_base_addr = PSRAM_0_DMC_BASE_ADDR + 0x200;
static const uintptr_t g_psram_axi_region_regs_base_addr = PSRAM_0_AXI_BASE_ADDR + 0x100;
static const uintptr_t g_psram_axi_qos_regs_base_addr = PSRAM_0_AXI_BASE_ADDR + 0x200;

bool g_support_wb_psram = false;

uintptr_t psram_port_get_phy_addr(psram_id_t id)
{
    unused(id);
    return g_psram_phy_regs_base_addr;
}

uintptr_t psram_port_get_dmc_addr(psram_id_t id)
{
    unused(id);
    return g_psram_dmc_regs_base_addr;
}

uintptr_t psram_port_get_axi_addr(psram_id_t id)
{
    unused(id);
    return g_psram_axi_regs_base_addr;
}

uintptr_t psram_port_get_dmc_sfg_sfc_addr(psram_id_t id)
{
    unused(id);
    return g_psram_dmc_cfg_sfc_regs_base_addr;
}

uintptr_t psram_port_get_dmc_debug_addr(psram_id_t id)
{
    unused(id);
    return g_psram_dmc_debug_regs_base_addr;
}

uintptr_t psram_port_get_axi_rgn_addr(psram_id_t id)
{
    unused(id);
    return g_psram_axi_region_regs_base_addr;
}
uintptr_t psram_port_get_axi_qos_addr(psram_id_t id)
{
    unused(id);
    return g_psram_axi_qos_regs_base_addr;
}

void psram_port_power_on(void)
{
}

#define PSRAM_FREQ_HIGH     0x1
#define PSRAM_FREQ_LOW      0x0
#ifdef BUILD_ATE_IMAGE
#define CLOCKS_SRC_PLL      0x2
#endif

void psram_port_freq_flag_set(void)
{
    if (reg16_getbits(COM_CTL_RB_XIP_OPI0_CR_REG, 0x1, 0x3) == CLOCKS_SRC_PLL) {
        hal_psram_freq_flag_set(PSRAM_FREQ_HIGH);
    } else {
        hal_psram_freq_flag_set(PSRAM_FREQ_LOW);
    }
}

void psram_controller_reinit(void)
{
    psram_port_freq_flag_set();
    for (uint8_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        hal_psram_init(i);
        uapi_psram_controller_init(i);
    }
}

bool psram_port_winbond_flag_get(void)
{
#ifdef CONFIG_SUPPORT_WINBOND_PSRAM
#define PSRAM_WINBOND_TAG                   0x5a6d
#define PSRAM_MANUFACTURER_TAG_REG          LP_CTL_ULP_B_RB_CFG_LP_CTL_ULP_B_GP_REG2_REG
    if (readl(PSRAM_MANUFACTURER_TAG_REG) == PSRAM_WINBOND_TAG) {
        g_support_wb_psram = true;
    }
#endif
    return g_support_wb_psram;
}

void psram_port_winbond_flag_set(bool flag)
{
    g_support_wb_psram = flag;
}

// vendor id:APMEMORY  type:aps256xxn_ob9
#define PSRAM_VENDOR_ID_APM                 0xD
#define AP_APS256XXN_OB9_CONFIG_REG_SUM     2
STATIC uint8_t g_ap_aps256xxn_ob9_config[][2] = {
    { 4, 0x38 }, // 4x refresh freq mr4[3:4](b00--4x refresh, b01--1x refresh, b11--0.5x refresh)
    { 0, 0x10 }, // mr0[4:2] = b100(Read Latency Code 7 max clock 200MHz); mr0[1:0] = b00(Drive Strength MAX(25Ω))
};

// vendor id:DSGMEMORY  type:dsg2586aa_l500e
#define PSRAM_VENDOR_ID_DSG                 0x5
#define DSG_DSG2586AA_CONFIG_REG_SUM        2
uint8_t g_dsg_dsg2586aa_config[][2] = {
    { 4, 0x38 }, // 0.5x refresh freq
    { 0, 0x10 }, // mr0[1:0] = b00(Drive Strength MAX(25Ω))
};

#define PSRAM_VENDOR_ID_WB                  0x6
#define WB_W957D6NKR_CONFIG_REG_SUM         2
uint16_t g_wb_w957d6nkr_config[][2] = {
    { 6, 0xFFC1 }, // 6:CR1 [1:0] = b01(Refresh Freq 4μS)
    { 0, 0x8F25 }, // 0:CR0 [14:12] = b000(Drive Strength  25 ohms)
};


int8_t psram_get_cfg_for_vendor_id(uint8_t id, uint8_t **config_reg, uint8_t *sum)
{
    if (id == PSRAM_VENDOR_ID_APM) {
        *sum = AP_APS256XXN_OB9_CONFIG_REG_SUM;
        *config_reg = (uint8_t *)g_ap_aps256xxn_ob9_config;
        return 0;
    } else if (id == PSRAM_VENDOR_ID_DSG) {
        *sum = DSG_DSG2586AA_CONFIG_REG_SUM;
        *config_reg = (uint8_t *)g_dsg_dsg2586aa_config;
        return 0;
    } else {
        return -1;
    }
}

int8_t psram_winbond_get_cfg_for_vendor_id(uint8_t id, uint16_t **config_reg, uint8_t *sum)
{
    if (id == PSRAM_VENDOR_ID_WB) {
        *sum = WB_W957D6NKR_CONFIG_REG_SUM;
        *config_reg = (uint16_t *)g_wb_w957d6nkr_config;
        return 0;
    } else {
        return -1;
    }
}

uint32_t uapi_psram_read_vendor_id(void)
{
#define IR_0_REG   0
#define MR_1_REG   1

    uint32_t vendor_id = 0;
    if (psram_port_winbond_flag_get()) {
        (void)hal_dmac_read_mr_reg(IR_0_REG, &vendor_id);
        return (vendor_id >> OFFSET_16) & 0xF;
    } else {
        (void)hal_dmac_read_mr_reg(MR_1_REG, &vendor_id);
        return vendor_id & 0x1F;
    }
}