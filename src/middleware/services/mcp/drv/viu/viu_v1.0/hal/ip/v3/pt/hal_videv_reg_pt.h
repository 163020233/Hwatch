/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef __HAL_VIDEV_REG_PT_H__
#define __HAL_VIDEV_REG_PT_H__

#include "drv_mcp_dependency.h"
#include "vicap_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void videv_reg_set_port_input_mode(volatile vicap_regs_type *reg, td_u32 pt_mode);
td_void videv_reg_set_port_yuv444_en(volatile vicap_regs_type *reg, td_bool enable);
td_void videv_reg_set_port_comp_mode(volatile vicap_regs_type *reg, td_u32 mode);
td_void videv_reg_set_data_offset(volatile vicap_regs_type *reg, td_u32 mask_id, td_u32 data_offset, td_bool reverse);
td_void videv_reg_set_data_mask(volatile vicap_regs_type *reg, td_u32 mask_id, td_u32 data_mask);

td_void videv_reg_set_bt656_en(volatile vicap_regs_type *reg, td_bool enable);
td_void videv_reg_set_port_bt656_mode(volatile vicap_regs_type *reg, td_u32 mode);

td_void videv_reg_set_chn_yuv_input_seq(volatile vicap_regs_type *reg, td_u32 yc_seq, td_u32 uv_seq);
td_void videv_reg_set_chn_combine_mode(volatile vicap_regs_type *reg, td_u32 unify_data_enable);
td_void videv_reg_set_unify_timing_cfg(volatile vicap_regs_type *reg, td_u32 sync_mode);
td_void videv_reg_set_chn_sync_unify_hsync(volatile vicap_regs_type *reg, td_u32 unify_hsync_inv,
    td_u32 unify_hsync_and, td_u32 unify_field_inv);
td_void videv_reg_set_chn_sync_unify_vsync(volatile vicap_regs_type *reg, td_u32 unify_vsync_inv,
    td_u32 unify_vsync_mode, td_u32 unify_field_sel);
td_void videv_reg_set_chn_sync_gen_timing_cfg(volatile vicap_regs_type *reg, td_u32 enable, td_u32 mode);

td_void videv_reg_set_chn_hsync_hfb(volatile vicap_regs_type *reg, td_u32 uhsync_hfb);
td_void videv_reg_set_chn_hsync_hact(volatile vicap_regs_type *reg, td_u32 uhsync_hact);
td_void videv_reg_set_chn_hsync_hbb(volatile vicap_regs_type *reg, td_u32 uhsync_hbb);
td_void videv_reg_set_chn_vsync_vfb(volatile vicap_regs_type *reg, td_u32 uvsync_vfb);
td_void videv_reg_set_chn_vsync_vact(volatile vicap_regs_type *reg, td_u32 uvsync_vact);
td_void videv_reg_set_chn_vsync_vbb(volatile vicap_regs_type *reg, td_u32 uvsync_vbb);
td_void videv_reg_set_chn_vsync_vbfb(volatile vicap_regs_type *reg, td_u32 uvsync_vbfb);
td_void videv_reg_set_chn_vsync_vbact(volatile vicap_regs_type *reg, td_u32 uvsync_vbact);
td_void videv_reg_set_chn_vsync_vbbb(volatile vicap_regs_type *reg, td_u32 uvsync_vbbb);

td_void videv_reg_set_port_id_config(volatile vicap_regs_type *reg, td_bool en, td_u32 vc_num);
td_void videv_reg_clear_port_interrupt(volatile vicap_regs_type *reg, td_u32 clr_int);
td_void videv_reg_set_port_en(volatile vicap_regs_type *reg, td_bool en);

td_void videv_reg_set_dev_auto_timing_en(volatile vicap_regs_type *reg, td_bool en);
td_void videv_reg_set_dev_auto_data_en(volatile vicap_regs_type *reg, td_bool en);
td_void videv_reg_set_dev_timing_hfb(volatile vicap_regs_type *reg, td_u32 hfb);
td_void videv_reg_set_dev_timing_hact(volatile vicap_regs_type *reg, td_u32 hact);
td_void videv_reg_set_dev_timing_hbb(volatile vicap_regs_type *reg, td_u32 hbb);
td_void videv_reg_set_dev_timing_vfb(volatile vicap_regs_type *reg, td_u32 vfb);
td_void videv_reg_set_dev_timing_vact(volatile vicap_regs_type *reg, td_u32 vact);
td_void videv_reg_set_dev_timing_vbb(volatile vicap_regs_type *reg, td_u32 vbb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
