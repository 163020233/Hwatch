/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram driver regs op source file \n
 *
 * History: \n
 * 2024-05-14, Create file. \n
 */

#include "errcode.h"
#include "chip_io.h"
#include "common_def.h"
#include "psram_porting.h"
#include "hal_psram_regs_op.h"

uintptr_t g_psram_phy_regs = (uintptr_t)NULL;
uintptr_t g_psram_dmc_regs = (uintptr_t)NULL;
uintptr_t g_psram_axi_regs = (uintptr_t)NULL;
uintptr_t g_psram_dmc_cfg_sfc_regs = (uintptr_t)NULL;
uintptr_t g_psram_dmc_debug_regs = (uintptr_t)NULL;
uintptr_t g_psram_axi_region_regs = (uintptr_t)NULL;
uintptr_t g_psram_axi_qos_regs = (uintptr_t)NULL;
#define PSRAM_0_PHY_CTRL2_ADDR 0x520E6028

/* 项目在 porting 层添加 winbond 程序获取配置标识 */
__attribute__((weak)) bool g_support_wb_psram = false;
__attribute__((weak)) bool psram_port_winbond_flag_get(void)
{
    return g_support_wb_psram;
}

void hal_psram_get_addr(psram_id_t id)
{
    g_psram_phy_regs = psram_port_get_phy_addr(id);
    g_psram_dmc_regs = psram_port_get_dmc_addr(id);
    g_psram_axi_regs = psram_port_get_axi_addr(id);
    g_psram_dmc_cfg_sfc_regs = psram_port_get_dmc_sfg_sfc_addr(id);
    g_psram_dmc_debug_regs = psram_port_get_dmc_debug_addr(id);
    g_psram_axi_region_regs = psram_port_get_axi_rgn_addr(id);
    g_psram_axi_qos_regs = psram_port_get_axi_qos_addr(id);
}

void hal_psram_regs_phy_init_zcal_en(psram_id_t id)
{
    psram_phy_init_t phy_init;
    phy_init.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init;
    // todo 通道0打开zcal_en，通道1关闭.
    if (id == PSRAM_ID_0) {
        phy_init.b.zcal_en = PSRAM_INIT_ZCAL_EN;
    }
    phy_init.b.ck_pulldown = 0x1; // zcal打开时，pulldown需要打开
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init = phy_init.d32;
}

void hal_psram_regs_phy_init_ck_pulldown_set(uint32_t value)
{
    psram_phy_init_t phy_init;
    phy_init.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init;
    phy_init.b.ck_pulldown = value;
    phy_init.b.rxfifo_dis = 0; // 配置完成后同时打开rxfifo写使能开关
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init = phy_init.d32;
}

uint32_t hal_psram_regs_phy_get_id0_zcal(void)
{
    psram_phy_init_t phy_init;
    phy_init.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init;
    uint32_t enable = phy_init.b.zcal_en;
    return enable;
}

// 高频打开dlym_en，低频关闭。
void hal_psram_regs_phy_dlym_en(void)
{
    psram_phy_init_t phy_init;
    phy_init.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init;
    phy_init.b.dlym_en = 0x1;
#ifdef CONFIG_SUPPORT_DLYM_1
    phy_init.b.dlym_en_1 = 0x1;
#endif
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init = phy_init.d32;
}

void hal_psram_regs_phy_init_ctrl(void)
{
    psram_phy_init_ctrl_t ctrl;
    ctrl.d32 = 0;
    ctrl.b.zcal_wait_time = 0xA;
    ctrl.b.zcal_errstopup = 0;
    ctrl.b.zcal_pnsequence = 1;
    ctrl.b.dlym_wait_time = PSRAM_DLYM_WAIT_TIME;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init_ctrl = ctrl.d32;
}

void hal_psram_regs_phy_io_ctrl1(void)
{
    psram_phy_io_ctrl1_t io_ctrl;
    io_ctrl.d32 = 0;
    io_ctrl.b.io_dq0_en = 1;
    io_ctrl.b.io_dq1_en = 1;
    io_ctrl.b.io_dq2_en = 1;
    io_ctrl.b.io_dq3_en = 1;
    io_ctrl.b.io_dq4_en = 1;
    io_ctrl.b.io_dq5_en = 1;
    io_ctrl.b.io_dq6_en = 1;
    io_ctrl.b.io_dq7_en = 1;
    io_ctrl.b.io_dq8_en = 1;
    io_ctrl.b.io_dq9_en = 1;
    io_ctrl.b.io_dq10_en = 1;
    io_ctrl.b.io_dq11_en = 1;
    io_ctrl.b.io_dq12_en = 1;
    io_ctrl.b.io_dq13_en = 1;
    io_ctrl.b.io_dq14_en = 1;
    io_ctrl.b.io_dq15_en = 1;
    io_ctrl.b.io_dqs0_en = 1;
    io_ctrl.b.io_dqs1_en = 1;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl1 = io_ctrl.d32;
}

void hal_psram_regs_phy_io_ctrl3(void)
{
    psram_phy_io_ctrl3_t io_ctrl;
    io_ctrl.d32 = 0;
    io_ctrl.b.io_poc_vref_en = 1;
    io_ctrl.b.io_poc_refsel = 1;
    io_ctrl.b.io_zcal_res_inner = 1;
    io_ctrl.b.io_ck_inmode = SSTL_IN;
    io_ctrl.b.io_cs_inmode = SSTL_IN;
    io_ctrl.b.io_dq_inmode = SSTL_IN;
    io_ctrl.b.io_dqs_inmode = SSTL_IN;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl3 = io_ctrl.d32;
}

void hal_psram_regs_phy_ctrl2_init(void)
{
    psram_phy_ctrl2_t ctrl;
    ctrl.d32 = 0;
    ctrl.b.zcal_pdrv_latch = 0x3;
    ctrl.b.zcal_ndrv_latch = 0x3;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl2 = ctrl.d32;
}

void hal_psram_regs_phy_ctrl2(psram_id_t id)
{
    psram_phy_ctrl2_t ctrl;
    psram_phy_init_info_t info;
    ctrl.d32 = 0;
    if (id == PSRAM_ID_1) {
        info.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_init_info;
        if (info.b.dlym_overflow == 0 && info.b.zcal_ndrv_err == 0 && info.b.zcal_pdrv_err == 0) {
            ctrl.b.zcal_pdrv_latch = ((psram_phy_ctrl2_t *)PSRAM_0_PHY_CTRL2_ADDR)->b.zcal_pdrv_latch;
            ctrl.b.zcal_ndrv_latch = ((psram_phy_ctrl2_t *)PSRAM_0_PHY_CTRL2_ADDR)->b.zcal_ndrv_latch;
            ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl2 = ctrl.d32;
        }
    }
}

void hal_psram_regs_phy_ctrl2_step_1(void)
{
    psram_phy_ctrl2_t ctrl;
    ctrl.d32 = 0;
    ctrl.b.zcal_pdrv_latch = 0x9;
    ctrl.b.zcal_ndrv_latch = 0x6;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl2 = ctrl.d32;
}

void hal_psram_regs_phy_work_mode0(uint32_t psram_freq)
{
    unused(psram_freq);
    psram_phy_work_mode0_t work_mode;
    work_mode.d32 = 0;
    work_mode.b.rxfifo_pop_sel = 1;
    work_mode.b.rxfifo_pop_mode = 0;

    work_mode.b.rdqsg_start_pos = PSRAM_RDQSG_START_POS;
    work_mode.b.rxfifo_rden_pos = PSRAM_RXFIFO_RDEN_POS;
    work_mode.b.debug_en = 1;

    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_work_mode0 = work_mode.d32;
}

void hal_psram_regs_phy_work_mode1(void)
{
    psram_phy_work_mode1_t work_mode;
    work_mode.d32 = 0;
    work_mode.b.dq_swap_en = 0;
    work_mode.b.dqs_swap_en = 0;
    work_mode.b.dm_swap_en = 0;
    work_mode.b.dm_io_use = 0;
    if (psram_port_winbond_flag_get()) {
        work_mode.b.dqs_fdbk_en = 0x1;
        work_mode.b.dqs_fdbk_samp_pos = 0x2;
    } else {
        work_mode.b.dqs_fdbk_en = 0;
        work_mode.b.dqs_fdbk_samp_pos = 0;
    }
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_work_mode1 = work_mode.d32;
}

void hal_psram_regs_phy_ctrl0(uint32_t psram_freq)
{
    psram_phy_ctrl0_t ctrl;
    ctrl.d32 = 0;
#ifdef CONFIG_SUPPORT_DLYM_1
    if (psram_freq) {
        ctrl.b.cs_ext = 0x2;
    } else {
        ctrl.b.cs_ext = 0x0;
    }
#else
    unused(psram_freq);
    ctrl.b.cs_ext = 0x2;
#endif
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl0 = ctrl.d32;
}

 // 高频手动配置初始值
void hal_psram_regs_phy_ctrl1_init_rdqs(void)
{
    psram_phy_ctrl1_t ctrl;
    ctrl.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl1;
#ifdef CONFIG_SUPPORT_DLYM_1
    ctrl.b.rdqs_bcdl_en = 0x1;
    ctrl.b.rdqs_bcdl_en_1 = 0x1;
#endif
    ctrl.b.rdqs_bcdl_code = 0x6;
#ifdef CONFIG_SUPPORT_DLYM_1
    ctrl.b.rdqs_bcdl_code_1 = 0x6;
#endif
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl1 = ctrl.d32;
}

void hal_psram_regs_phy_ctrl1(uint32_t psram_freq)
{
    psram_phy_ctrl1_t ctrl;
    ctrl.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl1;

#ifdef CONFIG_SUPPORT_DLYM_1
    if (psram_freq == 0) { // 低频手动配置
        ctrl.b.rdqs_bcdl_code = 0x6;
        ctrl.b.rdqs_bcdl_code_1 = 0x6;
    }
    ctrl.b.rdqs_bcdl_en = 0x0;
    ctrl.b.cs_bcdl_en = 0x0;
    ctrl.b.ck_bcdl_en = 0x0;
    ctrl.b.rdqs_bcdl_en_1 = 0x0;
#else
    ctrl.b.ck_bcdl_code = 0x9;
    if (psram_freq == 0) { // 低频手动配置
        ctrl.b.rdqs_bcdl_code = 0x3e;
    }
    ctrl.b.ck_bcdl_en = 1;
    ctrl.b.rdqs_bcdl_en = 1;
#endif

    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl1 = ctrl.d32;
}

void hal_psram_regs_phy_ctrl2_step_2(void)
{
    psram_phy_ctrl2_t ctrl;
    ctrl.d32 = 0;
    ctrl.b.zcal_pdrv_latch = PSRAM_PHY_CTRL_PCODE_VAL;
    ctrl.b.zcal_ndrv_latch = PSRAM_PHY_CTRL_PCODE_VAL;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_ctrl2 = ctrl.d32;
}

void hal_psram_regs_phy_io_ctrl0(void)
{
    psram_phy_io_ctrl0_t io_ctrl;
    io_ctrl.d32 = 0;
    io_ctrl.b.io_cs_oe = 0;
    io_ctrl.b.io_ck_oe = 1;
    io_ctrl.b.io_ck_diff = 0;
    io_ctrl.b.io_dq_odtsel = 0;
    io_ctrl.b.io_dq_ronsel = OHMS_4;
    io_ctrl.b.io_dqs_ronsel = OHMS_4;
    io_ctrl.b.io_ck_ronsel = OHMS_4;
    io_ctrl.b.io_cs_ronsel = OHMS_4;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl0 = io_ctrl.d32;
    io_ctrl.b.io_cs_oe = 1;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl0 = io_ctrl.d32;
}

void hal_psram_regs_dmac_cfg_hs_enter_req(void)
{
    psram_dmc_ctrl_hs_t reg;
    reg.d32 = 0;
    reg.b.hs_enter_req = 1;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs = reg.d32;
}

void hal_psram_regs_dmac_cfg_hs_exit_req(void)
{
    psram_dmc_ctrl_hs_t reg;
    reg.d32 = 0;
    reg.b.hs_exit_req = 1;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs = reg.d32;
}

void hal_psram_regs_dmac_cfg_hs_exec_cmd_bp(void)
{
#define PSRAM_DFS_READY_OFFSET    0xC

    psram_dmc_ctrl_hs_t reg;
    reg.d32 = ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs;
    reg.b.exec_cmd_bp = 1;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs = reg.d32;

    for (uint32_t i = 0; i < 0xFFFFF; i++) {
        uint32_t cur_status = ((psram_dmac_debug_regs_t *)g_psram_dmc_debug_regs)->dmc_cur_status;
        if ((cur_status & BIT(PSRAM_DFS_READY_OFFSET)) != 0) { // dfs ready ok
            return;
        }
    }
}

void hal_psram_regs_dmac_cfg_hs_close_cmd_bp(void)
{
    psram_dmc_ctrl_hs_t reg;
    reg.d32 = ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs;
    reg.b.exec_cmd_bp = 0;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_hs = reg.d32;
}

void hal_psram_regs_dmac_cfg_mem_mode(void)
{
    psram_dmc_mem_mode_t mode;
    mode.d32 = 0;
    mode.b.latency_type = 1;
    mode.b.wrap_en = 1;
    mode.b.mem_row = PSRAM_DMAC_MEM_ROW;
    mode.b.mem_col_ext = 1;
    if (psram_port_winbond_flag_get()) {
        mode.b.mem_col = 0x0;
        mode.b.mem_width = 1;
        mode.b.mem_type = 0x2;
    } else {
        mode.b.mem_col = PSRAM_DMAC_MEM_COL;
        mode.b.mem_width = 1;
        mode.b.mem_type = 1;
    }
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_mem_mode = mode.d32;
}

void hal_psram_regs_dmac_cfg_work_mode(void)
{
    psram_dmc_work_mode_t mode;
    mode.d32 = 0;
    mode.b.rd_merge_cnt_auto_clr = 1;
    if (psram_port_winbond_flag_get()) {
        mode.b.ap_vl_opt = 0;
        mode.b.ap_rlat = 0;   /* FPGA ap_ralt 5 / ASIC ap_ralt 7 */
    } else {
        mode.b.ap_vl_opt = 1;
        mode.b.ap_rlat = 5;   /* FPGA ap_ralt 5 / ASIC ap_ralt 7 */
    }
    mode.b.ws_release_opt = 1;
    mode.b.ws_data_ready_num = 0;
    mode.b.mid_push_en = 1;
    mode.b.addr_push_en = 1;
    mode.b.clk_ratio = 0;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_work_mode = mode.d32;
}

void hal_psram_regs_dmac_ws_buf(void)
{
    psram_dmc_ws_buf_t mode;
    mode.d32 = 0;
    mode.b.ws_buf_ptun_en = 1;
    mode.b.ws_buf_lvl = 0;
    mode.b.ws_buf_pri1 = PSRAM_DMAC_WS_BUF_PRI1;
    ((psram_dmac_debug_regs_t *)g_psram_dmc_debug_regs)->dmc_ws_buf = mode.d32;
    writel(&((psram_dmac_debug_regs_t *)g_psram_dmc_debug_regs)->dmc_ws_buf, 0x117);
}

bool hal_psram_regs_dmac_cur_hs_status(void)
{
#define PSRAM_IN_HS_OFFSET    0x9
    uint32_t cur_status = 0;
    for (uint32_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        cur_status = ((psram_dmac_debug_regs_t *)psram_port_get_dmc_debug_addr(i))->dmc_cur_status;
        if ((cur_status & BIT(PSRAM_IN_HS_OFFSET)) == 0) {
            return false;
        }
    }
    return true;
}

void hal_psram_regs_dmac_cfg_timing(void)
{
    psram_dmc_cfg_timing0_t cfg0;
    psram_dmc_cfg_timing1_t cfg1;
    psram_dmc_cfg_timing2_t cfg2;
    psram_dmc_cfg_timing3_t cfg3;
    psram_dmc_cfg_timing4_t cfg4;

    if (psram_port_winbond_flag_get()) {
        cfg0.d32 = PSRAM_WB_DMAC_TIMING0_VAL;
    } else {
        cfg0.d32 = 0;
        cfg0.b.tdqsck = PSRAM_DMAC_TIMING0_TDQSCK;
        cfg0.b.tcphw = PSRAM_DMAC_TIMING0_TCPHW;
        cfg0.b.tcphr = PSRAM_DMAC_TIMING0_TCPHR;
        cfg0.b.twl = PSRAM_DMAC_TIMING0_TWL;
        cfg0.b.trl = PSRAM_DMAC_TIMING0_TRL;
    }

    cfg1.d32 = 0;
    cfg1.b.ths = PSRAM_DMAC_TIMING1_THS;
    cfg1.b.tcss = PSRAM_DMAC_TIMING1_TCSS;

    cfg2.d32 = 0;
    cfg2.b.txhs = PSRAM_DMAC_TIMING2_TXHS;
    cfg2.b.txphs = PSRAM_DMAC_TIMING2_TXPHS;

    cfg3.d32 = 0;
    cfg3.b.tdpd = PSRAM_DMAC_TIMING3_TDPD;
    cfg3.b.trst = PSRAM_DMAC_TIMING3_TRST;

    cfg4.d32 = 0;
    cfg4.b.txpdpd = PSRAM_DMAC_TIMING4_TXPDPD;
    cfg4.b.txdpd = PSRAM_DMAC_TIMING4_TXDPD;

    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_timing0 = cfg0.d32;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_timing1 = cfg1.d32;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_timing2 = cfg2.d32;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_timing3 = cfg3.d32;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_timing4 = cfg4.d32;
}

void hal_psram_regs_dmac_cfg_lat(void)
{
    psram_dmc_cfg_lat_t cfg;
    cfg.d32 = 0;
    cfg.b.wden_lat = 2;   /* wden lat 2 */
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_lat = cfg.d32;
}

void hal_psram_regs_dmac_cfg_ext(void)
{
    psram_dmc_cfg_cs_ext_t cfg;
    cfg.d32 = 0;
    if (psram_port_winbond_flag_get()) {
        cfg.b.wden_ext = 0;
        cfg.b.wr_cs_ext = 0;
    } else {
        cfg.b.wden_ext = PSRAM_DMAC_CS_EXT_WDEN;
        cfg.b.wr_cs_ext = PSRAM_DMAC_CS_EXT_WR_CS;
    }
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_cs_ext = cfg.d32;
}

void hal_psram_regs_dmac_cfg_lp(void)
{
    psram_dmc_cfg_lp_t cfg;
    cfg.d32 = 0;
    cfg.b.dram_clk_dis_en = 1;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_lp = cfg.d32;
}

void hal_psram_regs_axi_region_map(psram_id_t id)
{
    psram_axi_region_map_t attr;
    attr.d32 = 0;
    attr.b.rgn_en = 1;
    attr.b.rgn_size = PSRAM_AXI_REGION_SIZE;
    if (id == PSRAM_ID_0) {
        attr.b.rgn_base_addr = PSRAM_AXI_REGION_BASE_ADDR0;
    }
    if (id == PSRAM_ID_1) {
        attr.b.rgn_base_addr = PSRAM_AXI_REGION_BASE_ADDR1;
    }
    ((psram_axi_region_regs_t *)g_psram_axi_region_regs)->axi_region_map = attr.d32;
}

void hal_psram_regs_axi_region_attr(void)
{
    psram_axi_region_attr_t attr;
    attr.d32 = 0;
    attr.b.ch_mode = 1;
    attr.b.addr_aligned = PSRAM_AXI_REGION_ADDR_ALIGNED;
    ((psram_axi_region_regs_t *)g_psram_axi_region_regs)->axi_region_attrib = attr.d32;
}

void hal_psram_regs_axi_action(void)
{
    psram_axi_action_t cfg;
    cfg.d32 = 0;
    cfg.b.rd_wrap_boundary = PSRAM_AXI_RD_WRAP_BOUNDARY;
    cfg.b.rd_wrap_split_en = 0;
    ((psram_axi_regs_t *)g_psram_axi_regs)->axi_action = cfg.d32;
}

void hal_psram_regs_axi_qos_wrpri(void)
{
    psram_axi_qos_wrpri_t cfg;
    cfg.d32 = 0;
    cfg.b.wr_pri0 = QOS_PRI7;
    cfg.b.wr_pri1 = QOS_PRI6;
    cfg.b.wr_pri2 = QOS_PRI5;
    cfg.b.wr_pri3 = QOS_PRI4;
    cfg.b.wr_pri4 = QOS_PRI3;
    cfg.b.wr_pri5 = QOS_PRI2;
    cfg.b.wr_pri6 = QOS_PRI1;
    cfg.b.wr_pri7 = QOS_PRI0;
    ((psram_axi_qos_regs_t *)g_psram_axi_qos_regs)->axi_qos_wrpri = cfg.d32;
}

void hal_psram_regs_axi_qos_rdpri(void)
{
    psram_axi_qos_rdpri_t cfg;
    cfg.d32 = 0;
    cfg.b.rd_pri0 = QOS_PRI7;
    cfg.b.rd_pri1 = QOS_PRI6;
    cfg.b.rd_pri2 = QOS_PRI5;
    cfg.b.rd_pri3 = QOS_PRI4;
    cfg.b.rd_pri4 = QOS_PRI3;
    cfg.b.rd_pri5 = QOS_PRI2;
    cfg.b.rd_pri6 = QOS_PRI1;
    cfg.b.rd_pri7 = QOS_PRI0;
    ((psram_axi_qos_regs_t *)g_psram_axi_qos_regs)->axi_qos_rdpri = cfg.d32;
}

void hal_psram_dmc_cfg_mr(uint32_t val)
{
    uint32_t cfg = val;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_cfg_mr = cfg;
}

void hal_psram_dmac_mr_reset(void)
{
    psram_dmc_cfg_sfc_cmd_t cfg;
    cfg.d32 = 0;
    cfg.b.sfc_cmd_type = PSRAM_DMAC_SFC_CMD_RESET;
    ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_cmd = cfg.d32;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc = 1;
}

errcode_t hal_dmac_mr_cfg(psram_mr_t mr)
{
    uint32_t val;
    uint32_t cnt = 0;

    ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_cmd = 0;
    ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_addr = (uint32_t)mr;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc = 1;
    val = readl(&(((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc));
    while (cnt <= PSRAM_WAIT_TIMEOUT) {
        val = readl(&(((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc));
        cnt++;
        if (val == 0) {
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t hal_dmac_read_mr_reg(psram_mr_t mr, uint32_t *val)
{
    uint32_t stat;
    uint32_t cnt = 0;

    ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_cmd = 1;
    ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_addr = (uint32_t)mr;
    ((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc = 1;
    stat = readl(&(((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc));
    while (cnt <= PSRAM_WAIT_TIMEOUT) {
        stat = readl(&(((psram_dmac_regs_t *)g_psram_dmc_regs)->dmc_ctrl_sfc));
        cnt++;
        if (stat == 0) {
            break;
        }
    }
    if (cnt > PSRAM_WAIT_TIMEOUT) {
        return ERRCODE_FAIL;
    }
    *val = ((psram_dmac_cfg_sfc_regs_t *)g_psram_dmc_cfg_sfc_regs)->dmc_cfg_sfc_rdata0;
    return ERRCODE_SUCC;
}

void hal_psram_regs_phy_rxfifo_rst(void)
{
    psram_phy_io_ctrl3_t reg;
    reg.d32 = ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl3;
    reg.b.io_poc_rsv = 1;
    ((psram_phy_regs_t *)g_psram_phy_regs)->phy_io_ctrl3 = reg.d32;
}