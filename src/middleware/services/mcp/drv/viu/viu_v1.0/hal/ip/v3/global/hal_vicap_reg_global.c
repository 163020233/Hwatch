/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_reg of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#include "hal_vicap_reg_global.h"
#include "viu_public.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void vicap_reg_set_mac_mmu_ctrl(volatile vicap_regs_type *reg, td_bool is_mmu)
{
    viu_unused(is_mmu);
    u_axi_rd_prot axi_rd_prot;
    u_axi_wr_prot axi_wr_prot;

    /* all of the regs in this function are secure regs, default value is ok */
    axi_rd_prot.u32 = reg->axi_rd_prot.u32;
    axi_rd_prot.bits.src0_prot = 1;
    reg->axi_rd_prot.u32 = axi_rd_prot.u32;

    axi_wr_prot.u32 = reg->axi_wr_prot.u32;
    axi_wr_prot.bits.ch0_prot = 1;
    axi_wr_prot.bits.ch1_prot = 1;
    axi_wr_prot.bits.isp0_stt_prot = 1;
    axi_wr_prot.bits.isp1_stt_prot = 1;
    reg->axi_wr_prot.u32 = axi_wr_prot.u32;
}

td_void vicap_reg_set_outstanding(volatile vicap_regs_type *reg,
    td_u32 routstanding, td_u32 woutstanding)
{
    u_axi_cfg t;

    t.u32 = reg->axi_cfg.u32;
    t.bits.r_outstanding = routstanding - 1;
    t.bits.w_outstanding = woutstanding - 1;
    reg->axi_cfg.u32 = t.u32;
}

td_void vicap_reg_low_power_enable(volatile vicap_regs_type *reg, td_bool enable)
{
    u_wk_mode t;

    t.u32 = reg->wk_mode.u32;
    t.bits.power_mode = enable;
    reg->wk_mode.u32 = t.u32;
}

td_void vicap_reg_set_int_mask(volatile vicap_regs_type *reg, td_u32 int_mask)
{
    u_vicap_int0_mask vicap_int0_mask;

    vicap_int0_mask.u32 = reg->vicap_int0_mask.u32;
    vicap_int0_mask.u32 = int_mask | (vicap_int0_mask.u32 & ~int_mask);
    reg->vicap_int0_mask.u32 = vicap_int0_mask.u32;
}

td_u32 vicap_reg_get_cap_int_status(volatile vicap_regs_type *reg)
{
    u_vicap_int0 vicap_int0;

    vicap_int0.u32 = reg->vicap_int0_masked.u32;

    return vicap_int0.u32;
}

td_void vicap_reg_set_afifo_crop_xy(volatile vicap_regs_type *reg, td_u32 x, td_u32 y)
{
    u_vicap_afifo0_start t;

    t.u32 = reg->vicap_afifo_crop[0].vicap_afifo_start.u32;
    t.bits.afifo0_x_start = x;
    t.bits.afifo0_y_start = y;
    reg->vicap_afifo_crop[0].vicap_afifo_start.u32 = t.u32;
}

td_void vicap_reg_set_afifo_crop_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h)
{
    u_vicap_afifo0_size t;

    t.u32 = reg->vicap_afifo_crop[0].vicap_afifo_size.u32;
    t.bits.afifo0_width = w - 1;
    t.bits.afifo0_height = h - 1;
    reg->vicap_afifo_crop[0].vicap_afifo_size.u32 = t.u32;
}

td_void vicap_reg_set_isp_sel(volatile vicap_regs_type *reg, td_s32 cap_id, td_u32 sel)
{
    u_cmux_sel un_cmux_sel;

    un_cmux_sel.u32 = reg->cmux_sel.u32;

    switch (cap_id) {
        case 0: /* pipe0 */
            un_cmux_sel.bits.chn0_sel = sel;
            break;

        default: /* pipe1 */
            un_cmux_sel.bits.chn1_sel = sel;
            break;
    }

    reg->cmux_sel.u32 = un_cmux_sel.u32;
}

/* magic num mean chn_id */
td_void vicap_reg_set_isp0_mode(volatile vicap_regs_type *reg, td_bool isp_bypass, td_u32 mode)
{
    u_isp_mode isp_mode;

    isp_mode.u32 = reg->isp_mode.u32;
    isp_mode.bits.isp0_mode = isp_bypass;
    isp_mode.bits.isp0_in_yuv_mode = mode;
    reg->isp_mode.u32 = isp_mode.u32;
}

td_void vicap_reg_set_isp1_mode(volatile vicap_regs_type *reg, td_bool isp_bypass, td_u32 mode)
{
    u_isp_mode isp_mode;

    isp_mode.u32 = reg->isp_mode.u32;
    isp_mode.bits.isp1_mode = isp_bypass;
    isp_mode.bits.isp1_in_yuv_mode = mode;
    reg->isp_mode.u32 = isp_mode.u32;
}

td_void vicap_reg_set_online_sel(volatile vicap_regs_type *reg, td_u32 wdr_sel)
{
    viu_unused(wdr_sel);
    u_online0_cfg online0_cfg;

    online0_cfg.u32 = reg->online0_cfg.u32;
    online0_cfg.bits.online0_en = 0x1;
    reg->online0_cfg.u32 = online0_cfg.u32;
}

td_void vicap_reg_set_line_afifo_en(volatile vicap_regs_type *reg, td_bool fifo_en)
{
    u_line_afifo_mode line_afifo_mode;

    line_afifo_mode.u32 = reg->line_afifo_mode.u32;
    line_afifo_mode.bits.line_afifo0_en = fifo_en;
    reg->line_afifo_mode.u32 = line_afifo_mode.u32;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
