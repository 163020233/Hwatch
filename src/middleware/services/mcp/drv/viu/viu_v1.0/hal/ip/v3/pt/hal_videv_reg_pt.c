/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_reg of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#include "hal_videv_reg_pt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */


td_void videv_reg_set_port_input_mode(volatile vicap_regs_type *reg, td_u32 pt_mode)
{
    u_pt_intf_mod o_intf_mod;

    o_intf_mod.u32 = reg->pt_intf_mod.u32;
    o_intf_mod.bits.pt_mode = pt_mode;
    reg->pt_intf_mod.u32 = o_intf_mod.u32;
}

td_void videv_reg_set_port_yuv444_en(volatile vicap_regs_type *reg, td_bool enable)
{
    u_pt_yuv444_cfg o_yuv444_cfg;

    o_yuv444_cfg.u32 = reg->pt_yuv444_cfg.u32;
    o_yuv444_cfg.bits.yuv44_enable = enable;
    reg->pt_yuv444_cfg.u32 = o_yuv444_cfg.u32;
}

td_void videv_reg_set_port_comp_mode(volatile vicap_regs_type *reg, td_u32 mode)
{
    u_pt_unify_data_cfg o_unify_data_cfg;

    o_unify_data_cfg.u32 = reg->pt_unify_data_cfg.u32;
    o_unify_data_cfg.bits.unify_data_comp_num = mode;
    reg->pt_unify_data_cfg.u32 = o_unify_data_cfg.u32;
}

td_void videv_reg_set_data_offset(volatile vicap_regs_type *reg, td_u32 mask_id,
    td_u32 data_offset, td_bool reverse)
{
    u_pt_offset0 offset0;
    u_pt_offset1 offset1;
    u_pt_offset2 offset2;

    if (mask_id == 0) {
        offset0.u32 = reg->pt_offset0.u32;
        offset0.bits.offset0 = data_offset;
        offset0.bits.offset0_rev = reverse;
        reg->pt_offset0.u32 = offset0.u32;
    } else if (mask_id == 1) {
        offset1.u32 = reg->pt_offset1.u32;
        offset1.bits.offset1 = data_offset;
        offset1.bits.offset1_rev = reverse;
        reg->pt_offset1.u32 = offset1.u32;
    } else { /* offset2 */
        offset2.u32 = reg->pt_offset2.u32;
        offset2.bits.offset2 = data_offset;
        offset2.bits.offset2_rev = reverse;
        reg->pt_offset2.u32 = offset2.u32;
    }
}

td_void videv_reg_set_data_mask(volatile vicap_regs_type *reg, td_u32 mask_id, td_u32 data_mask)
{
    u_pt_offset0 offset0;
    u_pt_offset1 offset1;
    u_pt_offset2 offset2;

    data_mask = data_mask >> 16; /* get high 16 bit */

    if (mask_id == 0) {
        offset0.u32 = reg->pt_offset0.u32;
        offset0.bits.offset0_mask = data_mask;
        reg->pt_offset0.u32 = offset0.u32;
    } else if (mask_id == 1) {
        offset1.u32 = reg->pt_offset1.u32;
        offset1.bits.offset1_mask = data_mask;
        reg->pt_offset1.u32 = offset1.u32;
    } else if (mask_id == 2) { /* offset2 */
        offset2.u32 = reg->pt_offset2.u32;
        offset2.bits.offset2_mask = data_mask;
        reg->pt_offset2.u32 = offset2.u32;
    }
}

td_void videv_reg_set_bt656_en(volatile vicap_regs_type *reg, td_bool enable)
{
    u_pt_bt656_cfg o_pt_bt656_cfg;

    o_pt_bt656_cfg.u32 = reg->pt_bt656_cfg.u32;
    o_pt_bt656_cfg.bits.bt656_enable = enable;
    reg->pt_bt656_cfg.u32 = o_pt_bt656_cfg.u32;
}

td_void videv_reg_set_port_bt656_mode(volatile vicap_regs_type *reg, td_u32 mode)
{
    u_pt_bt656_cfg pt_bt656_cfg;

    pt_bt656_cfg.u32 = reg->pt_bt656_cfg.u32;
    pt_bt656_cfg.bits.bt656_mode = mode;
    reg->pt_bt656_cfg.u32 = pt_bt656_cfg.u32;
}

td_void videv_reg_set_chn_yuv_input_seq(volatile vicap_regs_type *reg, td_u32 yc_seq, td_u32 uv_seq)
{
    u_pt_unify_data_cfg o_unify_data_cfg;

    o_unify_data_cfg.u32 = reg->pt_unify_data_cfg.u32;
    o_unify_data_cfg.bits.unify_data_yc_seq = yc_seq;
    o_unify_data_cfg.bits.unify_data_uv_seq = uv_seq;
    reg->pt_unify_data_cfg.u32 = o_unify_data_cfg.u32;
}

/* 设置亮色分离/复合模式, 涉及寄存器: u_ch_intf_cfg0.port_scan_mod */
td_void videv_reg_set_chn_combine_mode(volatile vicap_regs_type *reg, td_u32 unify_data_enable)
{
    u_pt_unify_data_cfg o_unify_data_cfg;

    o_unify_data_cfg.u32 = reg->pt_unify_data_cfg.u32;
    o_unify_data_cfg.bits.unify_data_enable = unify_data_enable;
    reg->pt_unify_data_cfg.u32 = o_unify_data_cfg.u32;
}

/* 设置vi port的同步信息
类型:非必配 bt656 bt1120
涉及寄存器: ch_intf_cfg0.port_vsync、ch_intf_cfg0.port_vsync_neg、ch_intf_cfg0.port_hsync
           ch_intf_cfg0.port_hsync_neg
每个设备对应的两个通道都应该设置 */
td_void videv_reg_set_unify_timing_cfg(volatile vicap_regs_type *reg, td_u32 sync_mode)
{
    u_pt_unify_timing_cfg o_unify_timing_cfg;

    o_unify_timing_cfg.u32 = reg->pt_unify_timing_cfg.u32;

    /* mipi, lvds, hispi使用固定时序配置(通过mipi进行时序的归一化) */
    if (sync_mode == 0) {
        o_unify_timing_cfg.u32 = 0x0;
    } else {
        /****************************************************************************************************
        pt_unify_timing_cfg.hsync_sel: 固定为0
        pt_unify_timing_cfg.hsync_inv: 输入hsyc低时配1，其他配0
        pt_unify_timing_cfg.hsync_and: vsync有效方式配1，其他配0
        pt_unify_timing_cfg.hsync_mode: 固定为1(上升沿)
        pt_unify_timing_cfg.vsync_sel: 固定为0(来源于vsync)
        pt_unify_timing_cfg.vsync_inv: 输入vsync为低，配1；其他配0。
        pt_unify_timing_cfg.vsync_mode: toggle配2，其他配1
        pt_unify_timing_cfg.field_sel: bt601隔行vsync为pulse或者valid配2，为隔行toggle时配1，其余时序配0。
        pt_unify_timing_cfg.field_inv: 一般不用配，用来标识奇偶场，如果程序中用1表示奇场，
                                                        则如果外部输入0表示奇场，则需要设置反相
        ****************************************************************************************************/
        o_unify_timing_cfg.bits.unify_de_sel = 1;
        o_unify_timing_cfg.bits.unify_de_inv = 0;
        o_unify_timing_cfg.bits.unify_hsync_sel = 0;
        o_unify_timing_cfg.bits.unify_hsync_inv = 0;
        o_unify_timing_cfg.bits.unify_hsync_and = 0;
        o_unify_timing_cfg.bits.unify_hsync_mode = 1;
        o_unify_timing_cfg.bits.unify_vsync_sel = 0;
        o_unify_timing_cfg.bits.unify_vsync_inv = 0;
        o_unify_timing_cfg.bits.unify_vsync_mode = 1;
        o_unify_timing_cfg.bits.unify_field_sel = 0;
        o_unify_timing_cfg.bits.unify_field_inv = 0;
    }

    reg->pt_unify_timing_cfg.u32 = o_unify_timing_cfg.u32;
}

td_void videv_reg_set_chn_sync_unify_hsync(volatile vicap_regs_type *reg,
    td_u32 unify_hsync_inv, td_u32 unify_hsync_and, td_u32 unify_field_inv)
{
    u_pt_unify_timing_cfg o_unify_timing_cfg;

    o_unify_timing_cfg.u32 = reg->pt_unify_timing_cfg.u32;
    o_unify_timing_cfg.bits.unify_hsync_inv = unify_hsync_inv;
    o_unify_timing_cfg.bits.unify_hsync_and = unify_hsync_and;
    o_unify_timing_cfg.bits.unify_field_inv = unify_field_inv;
    reg->pt_unify_timing_cfg.u32 = o_unify_timing_cfg.u32;
}

td_void videv_reg_set_chn_sync_unify_vsync(volatile vicap_regs_type *reg,
    td_u32 unify_vsync_inv, td_u32 unify_vsync_mode, td_u32 unify_field_sel)
{
    u_pt_unify_timing_cfg o_unify_timing_cfg;

    o_unify_timing_cfg.u32 = reg->pt_unify_timing_cfg.u32;
    o_unify_timing_cfg.bits.unify_vsync_inv = unify_vsync_inv;
    o_unify_timing_cfg.bits.unify_vsync_mode = unify_vsync_mode;
    o_unify_timing_cfg.bits.unify_field_sel = unify_field_sel; /* 3 means not need field signal */
    reg->pt_unify_timing_cfg.u32 = o_unify_timing_cfg.u32;
}

td_void videv_reg_set_chn_sync_gen_timing_cfg(volatile vicap_regs_type *reg,
    td_u32 enable, td_u32 mode)
{
    u_pt_gen_timing_cfg o_gen_timing_cfg;

    o_gen_timing_cfg.u32 = reg->pt_gen_timing_cfg.u32;
    o_gen_timing_cfg.bits.gen_enable = enable;
    o_gen_timing_cfg.bits.gen_mode = mode;
    o_gen_timing_cfg.bits.gen_hsync_mode = mode;
    o_gen_timing_cfg.bits.gen_vsync_mode = mode;
    reg->pt_gen_timing_cfg.u32 = o_gen_timing_cfg.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_hsync_hfb
 *  description : set the value of the member ch_intf_hfb.hfb
 *  input       : unsigned int uhsync_hfb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_hsync_hfb(volatile vicap_regs_type *reg, td_u32 uhsync_hfb)
{
    u_pt_intf_hfb o_intf_hfb;

    o_intf_hfb.u32 = reg->pt_intf_hfb.u32;
    o_intf_hfb.bits.hfb = uhsync_hfb;
    reg->pt_intf_hfb.u32 = o_intf_hfb.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_hsync_hact
 *  description : set the value of the member ch_intf_hact.hact
 *  input       : unsigned int uhsync_hact: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_hsync_hact(volatile vicap_regs_type *reg, td_u32 uhsync_hact)
{
    u_pt_intf_hact o_intf_hact;

    o_intf_hact.u32 = reg->pt_intf_hact.u32;
    o_intf_hact.bits.hact = uhsync_hact;
    reg->pt_intf_hact.u32 = o_intf_hact.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_hsync_hbb
 *  description : set the value of the member ch_intf_hbb.hbb
 *  input       : unsigned int uhsync_hbb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_hsync_hbb(volatile vicap_regs_type *reg, td_u32 uhsync_hbb)
{
    u_pt_intf_hbb o_intf_hbb;

    o_intf_hbb.u32 = reg->pt_intf_hbb.u32;
    o_intf_hbb.bits.hbb = uhsync_hbb;
    reg->pt_intf_hbb.u32 = o_intf_hbb.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_vsync_vfb
 *  description : set the value of the member ch_intf_vfb.vfb
 *  input       : unsigned int uvsync_vfb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vfb(volatile vicap_regs_type *reg, td_u32 uvsync_vfb)
{
    u_pt_intf_vfb o_intf_vfb;

    o_intf_vfb.u32 = reg->pt_intf_vfb.u32;
    o_intf_vfb.bits.vfb = uvsync_vfb;
    reg->pt_intf_vfb.u32 = o_intf_vfb.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_vsync_vact
 *  description : set the value of the member ch_intf_vact.vact
 *  input       : unsigned int uvsync_vact: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vact(volatile vicap_regs_type *reg, td_u32 uvsync_vact)
{
    u_pt_intf_vact o_intf_vact;

    o_intf_vact.u32 = reg->pt_intf_vact.u32;
    o_intf_vact.bits.vact = uvsync_vact;
    reg->pt_intf_vact.u32 = o_intf_vact.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_vsync_vbb
 *  description : set the value of the member ch_intf_vbb.vbb
 *  input       : unsigned int uvsync_vbb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vbb(volatile vicap_regs_type *reg, td_u32 uvsync_vbb)
{
    u_pt_intf_vbb o_intf_vbb;

    o_intf_vbb.u32 = reg->pt_intf_vbb.u32;
    o_intf_vbb.bits.vbb = uvsync_vbb;
    reg->pt_intf_vbb.u32 = o_intf_vbb.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_vsync_vbfb
 *  description : set the value of the member ch_intf_vbfb.vbfb
 *  input       : unsigned int uvsync_vbfb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vbfb(volatile vicap_regs_type *reg, td_u32 uvsync_vbfb)
{
    u_pt_intf_vbfb o_intf_vbfb;

    o_intf_vbfb.u32 = reg->pt_intf_vbfb.u32;
    o_intf_vbfb.bits.vbfb = uvsync_vbfb;
    reg->pt_intf_vbfb.u32 = o_intf_vbfb.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_vsync_vbact
 *  description : set the value of the member ch_intf_vbact.vact
 *  input       : unsigned int uvsync_vbact: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vbact(volatile vicap_regs_type *reg, td_u32 uvsync_vbact)
{
    u_pt_intf_vbact o_intf_vbact;

    o_intf_vbact.u32 = reg->pt_intf_vbact.u32;
    o_intf_vbact.bits.vbact = uvsync_vbact;
    reg->pt_intf_vbact.u32 = o_intf_vbact.u32;
}

/*******************************************************************************
 *  function    : viu_hal_set_chn_vsync_vbbb
 *  description : set the value of the member ch_intf_vbbb.vbbb
 *  input       : unsigned int uvsync_vbbb: 16 bits
 *  每个设备对应的两个通道都应该设置
 ******************************************************************************/
td_void videv_reg_set_chn_vsync_vbbb(volatile vicap_regs_type *reg, td_u32 uvsync_vbbb)
{
    u_pt_intf_vbbb o_intf_vbbb;

    o_intf_vbbb.u32 = reg->pt_intf_vbbb.u32;
    o_intf_vbbb.bits.vbbb = uvsync_vbbb;
    reg->pt_intf_vbbb.u32 = o_intf_vbbb.u32;
}

td_void videv_reg_set_port_id_config(volatile vicap_regs_type *reg, td_bool en, td_u32 vc_num)
{
    u_pt_id_cfg pt_id_cfg;

    pt_id_cfg.u32 = reg->pt_id_cfg.u32;
    pt_id_cfg.bits.id_enable = en;
    pt_id_cfg.bits.id_mode = 0;
    pt_id_cfg.bits.id = 0;
    pt_id_cfg.bits.id_max = vc_num;
    pt_id_cfg.bits.id_reset = en;
    reg->pt_id_cfg.u32 = pt_id_cfg.u32;
}

td_void videv_reg_clear_port_interrupt(volatile vicap_regs_type *reg, td_u32 clr_int)
{
    u_pt_int o_pt_int;

    o_pt_int.u32 = clr_int;
    reg->pt_int.u32 = o_pt_int.u32;
}

td_void videv_reg_set_port_en(volatile vicap_regs_type *reg, td_bool en)
{
    u_pt_intf_mod o_intf_mod;

    o_intf_mod.u32 = reg->pt_intf_mod.u32;
    o_intf_mod.bits.pt_enable = en;
    reg->pt_intf_mod.u32 = o_intf_mod.u32;
}

td_void videv_reg_set_dev_auto_timing_en(volatile vicap_regs_type *reg, td_bool en)
{
    u_pt_gen_timing_cfg dev_gen_cfg;

    dev_gen_cfg.u32 = reg->pt_gen_timing_cfg.u32;
    dev_gen_cfg.bits.gen_enable = en;
    reg->pt_gen_timing_cfg.u32 = dev_gen_cfg.u32;
}

td_void videv_reg_set_dev_auto_data_en(volatile vicap_regs_type *reg, td_bool en)
{
    u_pt_gen_data_cfg gen_data_cfg;

    gen_data_cfg.u32 = reg->pt_gen_data_cfg.u32;
    gen_data_cfg.bits.gen_data_enable = en;
    reg->pt_gen_data_cfg.u32 = gen_data_cfg.u32;
}

td_void videv_reg_set_dev_timing_hfb(volatile vicap_regs_type *reg, td_u32 hfb)
{
    u_pt_intf_hfb dev_gen_hfb;

    dev_gen_hfb.u32 = reg->pt_intf_hfb.u32;
    dev_gen_hfb.bits.hfb = hfb;
    reg->pt_intf_hfb.u32 = dev_gen_hfb.u32;
}

td_void videv_reg_set_dev_timing_hact(volatile vicap_regs_type *reg, td_u32 hact)
{
    reg->pt_intf_hact.u32 = hact;
}

td_void videv_reg_set_dev_timing_hbb(volatile vicap_regs_type *reg, td_u32 hbb)
{
    u_pt_intf_hbb dev_gen_hbb;

    dev_gen_hbb.u32 = reg->pt_intf_hbb.u32;
    dev_gen_hbb.bits.hbb = hbb;
    reg->pt_intf_hbb.u32 = dev_gen_hbb.u32;
}

td_void videv_reg_set_dev_timing_vfb(volatile vicap_regs_type *reg, td_u32 vfb)
{
    u_pt_intf_vfb dev_gen_vfb;

    dev_gen_vfb.u32 = reg->pt_intf_vfb.u32;
    dev_gen_vfb.bits.vfb = vfb;
    reg->pt_intf_vfb.u32 = dev_gen_vfb.u32;
}

td_void videv_reg_set_dev_timing_vact(volatile vicap_regs_type *reg, td_u32 vact)
{
    u_pt_intf_vact dev_gen_vact;

    dev_gen_vact.u32 = reg->pt_intf_vact.u32;
    dev_gen_vact.bits.vact = vact;
    reg->pt_intf_vact.u32 = dev_gen_vact.u32;
}

td_void videv_reg_set_dev_timing_vbb(volatile vicap_regs_type *reg, td_u32 vbb)
{
    u_pt_intf_vbb dev_gen_vbb;

    dev_gen_vbb.u32 = reg->pt_intf_vbb.u32;
    dev_gen_vbb.bits.vbb = vbb;
    reg->pt_intf_vbb.u32 = dev_gen_vbb.u32;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
