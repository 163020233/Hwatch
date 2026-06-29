/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_CONVERT_H__
#define __DRV_VDEC_CONVERT_H__

#include "vfmw_ext.h"
#include "drv_ioctl_vdec.h"
#include "vdec_priv_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

ext_vdec_norm_type vdec_cnvt_disp_norm_2_vdec(td_u32 norm);
ext_vdec_frame_rate_type vdec_cnvt_frmrate_type_2_vdec(vfmw_pts_framrate_type type);
ext_vdec_sampling_type vdec_cnvt_sampling_type_2_vdec(td_u32 scan_type);
ext_vdec_bit_depth vdec_cnvt_bit_depth_2_vdec(td_u32 bit_depth);
ext_vdec_color_space vdec_cnvt_color_space_2_vdec(vfmw_vid_std std, const vfmw_color_desc *color_info,
    td_u32 width, td_u32 height);
td_s32 vdec_cnvt_event_2_vdec(td_s32 chan_id, td_s32 type, const td_void *args, td_s32 len,
    ext_vdec_event *out);
vfmw_vid_std vdec_cnvt_std_2_vfmw(ext_vdec_standard std);
vfmw_dec_mode vdec_cnvt_decmode_2_vfmw(ext_vdec_decode_mode mode);
vfmw_cmp vdec_cnvt_cmp_2_vfmw(ext_vdec_cmp_mode mode);
td_u32 vdec_cnvt_cmd_id_2_vfmw(td_u32 cmd);
vfmw_flush_type vdec_cnvt_flush_type_2_vfmw(ext_vdec_flush_type type);
vfmw_pts_framrate_type vdec_cnvt_frmrate_type_2_vfmw(ext_vdec_frame_rate_type type);
td_u64 vdec_cnvt_event_map_2_vfmw(td_u64 vdec_event_map);
vfmw_yuv_fmt vdec_cnvt_yuv_fmt_2_vfmw(vdec_yuv_fmt vdec_fmt);
td_void vdec_cnvt_ext_frm_2_vdec(vfmw_ext_frm_info *vfmw_ext_frm, ext_vdec_ext_frm_info *vdec_ext_frm);
td_void vdec_cnvt_usrdat_2_vfmw(ext_vdec_user_data *vdec_usr, vfmw_usrdat_ext *vfmw_usr);
td_void vdec_cnvt_usrdat_2_vdec(vfmw_usrdat_ext *vfmw_usr, ext_vdec_user_data *vdec_usr);
td_void vdec_cnvt_bitdepth_cap_2_vdec(td_u32 vfmw_cap, ext_vdec_capability *cap);
td_void vdec_cnvt_frm_layout_cap_2_vdec(td_u32 vfmw_cap, ext_vdec_capability *cap);
vfmw_frm_layout vdec_cnvt_frm_layout_2_vfmw(ext_vdec_frame_layout frm_layout_mode);
vfmw_bit_depth_attr vdec_cnvt_bitdepth_mode_2_vfmw(ext_vdec_bit_depth bit_depth_mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
