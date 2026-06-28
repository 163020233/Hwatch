/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv_viu.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef DRV_VIU_H
#define DRV_VIU_H

#include "drv_viu_vicap.h"
#include "drv_region_ext.h"
#include "drv_viu_ext.h"
#include "drv_gdc_ext.h"
#include "viu_inner.h"
#include "viu_drv_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 layer;
    ext_rgn_area_type area_type;
    td_u32 color;
    union {
        ext_isp_rect dst_rect;
        ext_rgn_quadrangle quad_rangle;
    };
} viu_drv_cover_attr;

typedef struct {
    td_bool modify;
    td_u32 num;
    ext_rgn_type rgn_type;
    viu_drv_cover_attr cover[VIU_COVEREX_MAX_NUM];
} viu_drv_cover_info;

typedef struct {
    ext_isp_rect rect;
    td_u32 layer;
    td_u8  global_alpha; /* be the same as vgs_ext.h ext_vgs_osd_opt */
    td_u8  alpha0;
    td_u8  alpha1;
    td_u64 phy_addr;
    td_u32 bg_color;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    ext_isp_pixel_format pixel_fmt;
    td_u16 color_lut[RGN_COLOR_LUT_NUM];
} viu_drv_osd_attr;

typedef struct {
    td_bool modify;
    td_u32 num;
    ext_isp_pixel_format pixel_fmt;
    ext_rgn_type rgn_type;
    viu_drv_osd_attr osd[VIU_OVERLAYEX_MAX_NUM];
} viu_drv_osd_info;

td_bool viu_drv_is_proc_frame_start_irq(td_u32 proc_int_status, td_bool fpn_calibrate);
td_void viu_drv_get_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);

td_s32 viu_drv_check_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);

td_s32 viu_drv_set_fpn_correction_attr(td_s32 pipe_id, ext_isp_fpn_attr *correct_attr);
td_s32 viu_drv_set_fpn_calibrate_attr(td_s32 pipe_id, ext_isp_fpn_calibrate_attr *calibrate);

td_void viu_drv_update_online_wdr_crop(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_void viu_drv_update_wdr_cmp_bayer_format(td_s32 pipe_id, ext_isp_bayer_format bayer_format);
td_void wdr_drv_update_wdr_cmp_attr(td_s32 viu_pipe_id, ext_isp_video_frame_info *cmpframe,
    ext_vb_cmp_para_output *y_cmp_cfg, ext_vb_cmp_para_output *c_cmp_cfg);
td_void viu_drv_prepare_wdr_pic_info(td_s32 pipe_id, td_u32 vc_num, ext_isp_video_frame_info *wdr_frame);

td_s32 viu_drv_wdr_pool_init(td_s32 pipe_id);
td_s32 viu_drv_wdr_pool_de_init(td_s32 pipe_id);

td_void viu_drv_get_split_attr(td_s32 pipe_id, viu_pipe_split_attr *split_attr);

td_s32 viu_drv_check_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nr_x_param);
td_void viu_drv_set_pipe_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nr_x_param);
td_s32 viu_drv_get_pipe_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nr_x_param);
td_void viu_drv_update_pipe_nrx_param_flag(td_s32 pipe_id);
td_void viu_drv_get_nr_interface_type(td_s32 pipe_id, viu_nr_intf *nr_intf);
td_void viu_drv_get_nr_version(td_s32 pipe_id, viu_nr_intf nr_intf, ext_viu_nr_version *nr_version);

#ifdef SOCT_VIU_DEBUG_SUPPORT
td_void viu_drv_set_image_quality_cfg(td_s32 pipe_id, viu_img_quality_cfg *image_quality_cfg);
td_s32 viu_drv_copy_image_quality_cfg(td_s32 pipe_id, viu_img_quality_cfg *image_quality_cfg);
td_s32 viu_drv_get_image_quality_cfg(td_s32 pipe_id, viu_img_quality_cfg *image_quality_cfg);
td_void viu_drv_set_dis_after3_dnr(td_s32 pipe_id, td_s32 chn_id, td_bool after3_dnr);
td_void viu_drv_get_dis_after3_dnr(td_s32 pipe_id, td_s32 chn_id, td_bool *after3_dnr);
#endif

td_void viu_drv_get_be_cfg_local(td_s32 pipe_id, isp_cfg_rec *be_buf, isp_be_sync_para *be_sync_param);

td_void viu_drv_flush_be_stt_info_buf(td_s32 pipe_id);
td_void viu_drv_put_free_be_buf(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf);

td_void viu_drv_put_region(td_s32 pipe_id, td_s32 chn_id, ext_rgn_type rgn_type);
td_void viu_drv_get_chn_coverex_info(td_s32 pipe_id, td_s32 chn_id, viu_drv_cover_info *chn_coverex);
td_u32 viu_drv_get_chn_coverex_num(td_s32 pipe_id, td_s32 chn_id);
td_void viu_drv_prepare_chn_coverex_info(td_s32 pipe_id, td_s32 chn_id);

td_void viu_drv_prepare_chn_osd_info(td_s32 pipe_id, td_s32 chn_id);
td_void viu_drv_get_chn_osd_info(td_s32 pipe_id, td_s32 chn_id, viu_drv_osd_info *chn_osd);
td_u32 viu_drv_get_chn_osd_num(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_drv_dis_pool_de_init(td_s32 pipe_id, td_u32 pool_id);
td_void viu_drv_set_dis_pool_id(td_s32 pipe_id, td_s32 chn_id, td_u32 pool_id);
td_s32 viu_drv_get_dis_pool_id(td_s32 pipe_id, td_s32 chn_id);
td_s32 viu_drv_dis_get_vb(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_dis_pool_init(td_s32 pipe_id, td_s32 chn_id, td_u32 buf_num);
td_void viu_drv_set_dis_dsf_config(td_s32 pipe_id, td_s32 chn_id, ext_dis_dsf_hw_cfg *dsf_hw_cfg);
td_void viu_drv_set_dis_en(td_s32 pipe_id, td_s32 chn_id, td_bool dis_enable);
td_void viu_drv_prepare_chn_dsf_pic_info(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame *chn_frame);
td_void viu_drv_update_last_buf_ok_flag(td_s32 pipe_id, td_s32 chn_id, td_bool buf_ok);
td_s32 viu_drv_dis_check_config(ext_dis_config *dis_config);

td_s32 viu_drv_check_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_ldc_attr *ldc_attr, ext_isp_size *size);
td_void viu_drv_set_chn_ldc_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_ldc_attr *ldc_attr);
td_void viu_drv_get_chn_ldc_attr_set(td_s32 pipe_id, td_s32 chn_id, ext_viu_ldc_attr *ldc_attr);
td_void viu_drv_get_chn_ldc_drv_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_ldc_attr *ldc_drv_attr);
td_void viu_drv_get_chn_ldc_attr_use(td_s32 pipe_id, td_s32 chn_id, ext_viu_ldc_attr *ldc_attr);
td_void viu_drv_get_chn_ldc_enalbe_use(td_s32 pipe_id, td_s32 chn_id, td_bool *enable);
td_void viu_drv_get_ldc_operation_attr(td_s32 pipe_id, td_s32 chn_id, ext_gdc_ldc_opt *operation_attr);

td_void viu_drv_get_rotation_operation_attr(td_s32 pipe_id, td_s32 chn_id, ext_gdc_pmf_opt *pmf_opt);

td_void viu_drv_set_chn_spread_attr(td_s32 pipe_id, td_s32 chn_id, viu_gdc_spread_attr *gdc_spread_attr);
td_void viu_drv_get_chn_spread_attr_set(td_s32 pipe_id, td_s32 chn_id, ext_gdc_spread_attr *gdc_spread_attr);
td_void viu_drv_get_chn_spread_attr_use(td_s32 pipe_id, td_s32 chn_id, ext_gdc_spread_attr *gdc_spread_attr);
td_void viu_drv_get_chn_spread_enalbe_use(td_s32 pipe_id, td_s32 chn_id, td_bool *enable);
td_void viu_drv_get_spread_operation_attr(td_s32 pipe_id, td_s32 chn_id,
    ext_gdc_spread_attr *spread_attr, ext_gdc_pmf_opt *pmf_opt);

td_s32 viu_drv_check_gdc_support_pixel_format(ext_isp_pixel_format pixel_format);
td_bool viu_drv_is_gdc_cfg_update(td_s32 pipe_id, td_s32 chn_id);
td_void viu_drv_set_gdc_cfg_update_flag(td_s32 pipe_id, td_s32 chn_id, td_bool gdc_config_update);

td_s32 viu_drv_get_be_cfg(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf, isp_be_sync_para *be_sync_param);
td_s32 viu_drv_hold_be_cfg(td_s32 pipe_id, isp_cfg_rec *be_cfg_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_VIU_H */
