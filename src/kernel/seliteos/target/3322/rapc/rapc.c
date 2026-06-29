/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: rapc source for SELiteOS.
 */
#include "asm/memmap_config.h"
#include "asm/register_config.h"
#include "los_base.h"
#include "rapc.h"

static void rapc_set_enable(bool flag)
{
    tcm_apc_en_reg_t apc_en;
    bool need_change = false;

    READ_UINT16(apc_en.u16, TCM_APC_EN);
    if (apc_en.bits.region_apc_en != flag) {
        apc_en.bits.region_apc_en = flag;
        need_change = true;
    }
    if (apc_en.bits.itcm_apc_enhance_en != flag) {
        apc_en.bits.itcm_apc_enhance_en = flag;
        need_change = true;
    }

    if (need_change) {
        WRITE_UINT16(apc_en.u16, TCM_APC_EN);
    }
}

static void rapc_set_tcm_addr(uintptr_t tcm_start_addr, uintptr_t tcm_end_addr, uint32_t reg_addr)
{
    uintptr_t addr_mask = 0xF8000; // use to get addr bit 15 to bit 19
    uint8_t shift_bits = 15;
    tcm_apc_addr_reg_t tcm_apc_addr;

    tcm_apc_addr.bits.tcm_start_addr = (tcm_start_addr & addr_mask) >> shift_bits;
    tcm_apc_addr.bits.tcm_end_addr = (tcm_end_addr & addr_mask) >> shift_bits;

    WRITE_UINT16(tcm_apc_addr.u16, reg_addr);
}

void rapc_set_tee_ctrl(bool flag)
{
    rapc_set_enable(flag);
    if (flag) {
        rapc_set_tcm_addr(TEE_ITCM_START_ADDR, TEE_ITCM_END_ADDR, ITCM_APC_ADDR);
        rapc_set_tcm_addr(TEE_DTCM_START_ADDR, TEE_DTCM_END_ADDR, DTCM_APC_ADDR);
    } else {
        rapc_set_tcm_addr(0, 0, ITCM_APC_ADDR);
        rapc_set_tcm_addr(0, 0, DTCM_APC_ADDR);
    }
}

#ifdef TZPC_PROTECT_CONFIG
void tzpc_set_tee_ctrl(bool flag)
{
    tzpc_prot_cfg_reg_t tzpc_prot_cfg;
    READ_UINT16(tzpc_prot_cfg.u16, TZPC_PROT_CFG);

    tzpc_prot_cfg.bits.efuse_tzpc_prot_cfg = (!flag); // if set, efuse can only be accessed by TEE CPU
    WRITE_UINT16(tzpc_prot_cfg.u16, TZPC_PROT_CFG);
}
#endif
