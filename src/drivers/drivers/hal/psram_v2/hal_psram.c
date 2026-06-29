/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram hal source file \n
 *
 * History: \n
 * 2024-05-15, Create file. \n
 */

#include "common_def.h"
#include "hal_psram_regs_op.h"
#include "hal_psram.h"

uint32_t g_psram_freq_flag = 0;

void hal_psram_init(psram_id_t id)
{
    hal_psram_get_addr(id);
}

void hal_psram_phy_init(psram_id_t id)
{
#if defined(PRE_ASIC)
    hal_psram_regs_phy_ctrl2_init();
    if (g_psram_freq_flag == 1) {
        hal_psram_regs_phy_ctrl1_init_rdqs();
        hal_psram_regs_phy_dlym_en();
    } else {
#if defined(CONFIG_PSRAM_NOT_SUPPORT_ZQCAL)
        hal_psram_regs_phy_ctrl1_init_rdqs();
#endif
    }
#if !defined(CONFIG_PSRAM_NOT_SUPPORT_ZQCAL)
    hal_psram_regs_phy_init_zcal_en(id);
    uint32_t count = 0;
    while (hal_psram_regs_phy_get_id0_zcal() != 0 && count < 0x1000) {
        count++; // for delay
    }
    hal_psram_regs_phy_ctrl2(id);
#else
    unused(id);
#endif
    hal_psram_regs_phy_init_ck_pulldown_set(0);
    hal_psram_regs_phy_init_ctrl();
    hal_psram_regs_phy_ctrl0(g_psram_freq_flag);
    hal_psram_regs_phy_ctrl1(g_psram_freq_flag);
    hal_psram_regs_phy_io_ctrl1();
    hal_psram_regs_phy_io_ctrl3();
    hal_psram_regs_phy_work_mode0(g_psram_freq_flag);
    hal_psram_regs_phy_work_mode1();
#else // FPGA
    unused(id);
    hal_psram_regs_phy_init_ctrl();
    hal_psram_regs_phy_io_ctrl1();
    hal_psram_regs_phy_io_ctrl3();
    hal_psram_regs_phy_ctrl2_step_1();
    hal_psram_regs_phy_work_mode0(g_psram_freq_flag);
    hal_psram_regs_phy_work_mode1();
    hal_psram_regs_phy_ctrl2_step_2();
#endif
    hal_psram_regs_phy_io_ctrl0();
}

void hal_psram_dmac_init(void)
{
    hal_psram_regs_dmac_cfg_mem_mode();
    hal_psram_regs_dmac_cfg_work_mode();
    hal_psram_regs_dmac_ws_buf();
    hal_psram_regs_dmac_cfg_timing();
    hal_psram_regs_dmac_cfg_lat();
    hal_psram_regs_dmac_cfg_ext();
    hal_psram_regs_dmac_cfg_lp();
}

void hal_psram_axi_init(psram_id_t id)
{
    hal_psram_regs_axi_region_map(id);
    hal_psram_regs_axi_region_attr();
    hal_psram_regs_axi_action();
    hal_psram_regs_axi_qos_wrpri();
    hal_psram_regs_axi_qos_rdpri();
}

void hal_psram_freq_flag_set(uint32_t value)
{
    g_psram_freq_flag = value;
}