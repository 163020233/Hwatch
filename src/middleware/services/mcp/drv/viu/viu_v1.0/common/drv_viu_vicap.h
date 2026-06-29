/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv_viu_vicap.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef DRV_VIU_VICAP_H
#define DRV_VIU_VICAP_H

#include "drv_isp_video_ext.h"
#include "drv_ioctl_viu.h"
#include "viu_common_mem.h"
#include "viu_public.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_MOD_NAME                      "VIU"
#define VIU_MAX_MMZ_NAME_LEN              16

#define INVALID_DEV                      (-1)

#define VIU_CHN_PIC_QUEUE_MAX_DEPTH       8

#define VIU_EXTCHN_MIN_WIDTH            32
#define VIU_EXTCHN_MIN_HEIGHT           32
#define VIU_EXTCHN_MAX_WIDTH            8192
#define VIU_EXTCHN_MAX_HEIGHT           8192

#define VIU_EXT_CHN_MAX_ZOOMIN          16U
#define VIU_EXT_CHN_MAX_ZOOMOUT         30U

#define VIU_VPSS_DEFAULT_EARLINE        128

#define VIU_COVEREX_MAX_NUM             8
#define VIU_OVERLAYEX_MAX_NUM           8
#define VIU_MAX_ALIGN                   1024

#define chn_index_to_ext(chn_id)      ((chn_id) - VIU_MAX_PHY_CHN_NUM)
#define ext_to_chn_index(ext_chn_id)      ((ext_chn_id) + VIU_MAX_PHY_CHN_NUM)

#define is_phy_pipe(pipe_id) (((pipe_id) < ISP_MAX_PHY_PIPE_NUM) && ((pipe_id) >= 0))
#define is_vir_pipe(pipe_id) (((pipe_id) < ISP_MAX_PIPE_NUM) && ((pipe_id) >= ISP_MAX_PHY_PIPE_NUM))

#define is_cap_frame_start_irq(int_status)  ((int_status) & (td_u32)WCH_FRAMEPULSE_INT)
#define is_cap_frame_end_irq(int_status)    ((int_status) & (td_u32)WCH_CC_INT)
#define is_cap_frame_delay_irq(int_status)  ((int_status) & (td_u32)WCH_FSTART_DLY_INT)

typedef enum {
    VIU_VB_SOURCE_COMMON  = 0,
    VIU_VB_SOURCE_MODULE  = 1,
    VIU_VB_SOURCE_PRIVATE = 2,
    VIU_VB_SOURCE_USER    = 3,
    VIU_VB_SOURCE_BUTT
} viu_vb_source;

typedef struct {
    td_s32 viu_pipe_id;
    td_s32 viu_chn_id;
    viu_vb_source vb_source;
    vb_pool vb_pool_id;
    ext_isp_size size;
    ext_pixel_format pixel_format;
    ext_compress_mode compress_mode;
    ext_video_format video_format;
    ext_dynamic_range dynamic_range;
    td_bool lowdelay;
    td_u32 align;
} viu_get_frame_vb_info;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
    td_u32 y_offset;
    td_u32 c_offset;
} viu_crop_calc;

typedef struct {
    td_bool working;
    td_u64 node_phy_addr;
    td_void *node;
} viu_drv_model_node;

typedef struct {
    viu_drv_model_node model_node[VIU_MAX_SPLIT_NODE_NUM];
    viu_mem_info model_mem;
} viu_drv_model;

td_void viu_drv_get_dev_attr(td_s32 viu_dev_id, ext_viu_dev_attr *viu_dev_attr);

td_s32 viu_drv_check_chip_id(td_s32 viu_dev_id);

td_void viu_drv_get_pipe_bind_dev_output_size(td_s32 pipe_id, ext_isp_size *dev_size);
td_void viu_drv_get_pipe_pre_size(td_s32 pipe_id, ext_isp_size *pipe_pre_size);

td_void viu_drv_set_timg_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr);
td_void viu_drv_get_timg_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr);

td_void viu_drv_set_dev_attach_pipe(td_u8 dev_id, td_u8 pipe_id);
td_void viu_drv_set_dev_detach_pipe(td_u8 dev_id, td_u8 pipe_id);

td_void viu_drv_get_dev_bind_pipe(td_s32 viu_dev_id, ext_viu_dev_bind_pipe *dev_bind_pipe);
td_void viu_drv_set_pipe_bind_dev_id(td_s32 pipe_id, td_s32 viu_dev_id);

ext_isp_wdr_mode viu_drv_get_wdr_mode_from_dev(td_s32 pipe_id);

td_s32 viu_drv_get_pipe_bind_dev_id(td_s32 pipe_id);
td_s32 viu_drv_get_pipe_bind_dev_id_before_init(td_s32 pipe_id);

ext_viu_operating_mode_type viu_drv_get_pipe_mode(td_s32 pipe_id);

td_s32 viu_drv_check_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode);
td_s32 viu_drv_check_pipe_static_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_void viu_drv_set_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_void viu_drv_get_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr);
td_void viu_drv_get_working_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr);

td_s32 viu_drv_pipe_drv_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode);
td_void viu_drv_pipe_drv_ctx_exit(td_s32 pipe_id);

td_s32 viu_drv_malloc_image_process_buffer(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr);
td_void viu_drv_free_image_process_buffer(td_s32 pipe_id);

td_s32 viu_drv_malloc_model_buf(td_s32 pipe_id, viu_drv_model *model);
td_void viu_drv_free_model_buf(viu_drv_model *model);

td_void viu_drv_set_split_model_cfg(td_s32 pipe_id, viu_drv_model *model_node, td_u32 split_num);
td_void viu_drv_set_single_model_cfg(td_s32 pipe_id, viu_drv_model_node *model_node);

td_s32 viu_drv_check_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr);
td_void viu_drv_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr);
td_void viu_drv_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr);
td_void viu_drv_get_working_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr);

td_s32 viu_drv_check_rgb_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_check_yuv_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_check_line_raw_capacity_for_be(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_check_wdr_raw_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame_info, td_u32 vc_num);
td_s32 viu_drv_check_send_raw_in_wdr_mode(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout);

td_s32 viu_drv_check_ext_capacity(td_s32 pipe_id, td_s32 ext_chn, ext_isp_video_frame_info *video_frame);

td_void viu_drv_prepare_bnr_raw_pic_info(td_s32 pipe_id, ext_isp_video_frame_info *bnr_raw_frame);

td_s32 viu_drv_check_pipe_pre_crop_attr(td_s32 pipe_id, const ext_isp_crop_info *crop_info);
td_s32 viu_drv_check_pipe_post_crop_attr(td_s32 pipe_id, const ext_isp_crop_info *crop_info);

td_void viu_drv_get_pipe_output_size(td_s32 pipe_id, ext_isp_size *pipe_output_size);

td_bool viu_drv_cap_is_wrong_int(td_s32 pipe_id, ext_isp_frame_interrupt_type int_type, td_u32 int_status);

td_s32 viu_drv_get_wch_vb_blk(td_s32 pipe_id, viu_vb_source vb_source,
    vb_pool vb_pool_id, ext_isp_video_frame_info *video_frame);

td_bool viu_drv_pipe_frame_rate_ctl(td_s32 pipe_id);

td_s32 viu_drv_check_wdr_capacity(td_s32 pipe_id);

td_void viu_drv_get_isp_detection_size(td_s32 pipe_id, ext_isp_size *valid_size, ext_isp_size *total_size);

td_bool viu_drv_get_fpn_calibrate_start_flag(td_u32 proc_id);

td_void viu_drv_relase_fpn_frame_addr(td_s32 pipe_id);
td_s32 viu_drv_fpn_spin_lock_init(td_void);
td_void viu_drv_destroy_fpn_spin_lock(td_void);

ext_isp_wdr_mode viu_drv_get_wdr_mode_by_pipe(td_s32 pipe_id);
td_bool viu_drv_is_wdr_mode(ext_isp_wdr_mode wdr_mode);
td_u32 viu_drv_get_wdr_vc_num(ext_isp_wdr_mode wdr_mode);
td_s32 viu_drv_prealloc_vb(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);

td_u32 viu_drv_get_pipe_default_vc_number(td_s32 pipe_id);

td_s32 viu_drv_get_mast_pipe_by_pipe(td_s32 pipe_id);

td_s32 viu_drv_check_pipe_support(td_s32 pipe_id);

td_u32 viu_drv_get_pipe_idx_from_bind(td_s32 pipe_id);

td_void viu_drv_prepare_cur_pic_info(td_s32 pipe_id, ext_isp_video_frame_info *cur_frame);
td_void viu_drv_calc_cur_pic_size(td_s32 pipe_id, ext_viu_operating_mode_type mode);

td_void viu_drv_update_working_pipe_drv_attr(td_s32 pipe_id);

td_bool viu_drv_find_suitable_proc(ext_viu_operating_mode_type pipe_mode);

td_u32 viu_drv_get_node_weight(td_s32 pipe_id);

td_bool viu_drv_need_split(td_s32 pipe_id);
td_u32 viu_drv_get_actual_split_num(td_s32 pipe_id);
td_u32 viu_drv_get_max_split_num(td_s32 pipe_id);

td_void viu_drv_prepare3dnr_info(td_s32 pipe_id, ext_isp_video_frame_info *chn0_ref_frame, td_bool lost_frame);
td_void viu_drv_get_pipe_nrx_status(td_s32 pipe_id, td_bool *nrx_en);

td_void viu_drv_get_pipe_frame_interrupt_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr);

td_void viu_drv_update_working_chn_drv_attr(td_s32 pipe_id, td_s32 chn_id);

td_void viu_drv_get_working_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);

td_s32 viu_drv_prepare_all_phy_chn_crop(td_s32 pipe_id);

td_void viu_drv_prepare_next_frame_pipe_attr(td_s32 pipe_id);

td_void viu_drv_prepare_next_frame_chn_attr(td_s32 pipe_id, td_s32 chn_id);

td_u32 viu_drv_get_online_proc_id_by_pipe(td_s32 pipe_id);

td_void viu_drv_proc_start_cap(td_s32 pipe_id);

td_s32 viu_drv_check_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);
td_s32 viu_drv_check_chn_static_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);
td_s32 viu_drv_check_chn_static_attr_for_gdc(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);
td_s32 viu_drv_check_chn_static_attr_for_dis(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);
td_void viu_drv_set_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);
td_void viu_drv_get_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_chn_attr *chn_attr);

td_u32 viu_drv_get_phy_chn_depth(td_s32 pipe_id, td_s32 chn_id);

td_bool viu_drv_is_chn_attr_setted(td_s32 pipe_id, td_s32 chn_id);

td_void viu_drv_get_working_chn_compress_mode(td_s32 pipe_id, td_s32 chn_id, ext_compress_mode *compress_mode);

td_void viu_drv_get_chn_out_put_size(td_s32 pipe_id, td_s32 chn_id, ext_isp_size *size);
td_void viu_drv_get_chn_out_put_size_from_user_set(td_s32 pipe_id, td_s32 chn_id,
    ext_isp_size *in_size, ext_isp_size *size);

td_void viu_drv_prepare_chn_dest_pic_info(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame_info *chn_frame);

td_u32 viu_drv_get_chn_max_width(td_s32 pipe_id, td_s32 chn_id);
td_u32 viu_drv_get_chn_max_height(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_drv_check_chn_crop(td_s32 pipe_id, td_s32 chn_id, ext_viu_crop_info *crop_info);
td_s32 viu_drv_chn_calc_crop(td_s32 pipe_id, td_s32 chn_id, ext_isp_size *in_size);
td_void viu_drv_get_chn_trim_crop_info(td_s32 pipe_id, td_s32 chn_id, ext_isp_rect *crop_rect);
td_void viu_drv_get_chn_crop_calc_resault(td_s32 pipe_id, td_s32 chn_id, viu_crop_calc *crop_calc);

td_void viu_drv_mark_chn_enable_flag(td_s32 pipe_id, td_s32 chn_id, td_bool en);
td_s32 viu_drv_chn_check_online_capacity(td_s32 pipe_id, td_s32 chn_id);
td_s32 viu_drv_chn_check_offline_capacity(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame *frame);

td_bool viu_drv_need_call_gdc_function(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_drv_check_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ext_chn_attr *ext_chn_attr);
td_void viu_drv_set_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ext_chn_attr *ext_chn_attr);
td_void viu_drv_get_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ext_chn_attr *ext_chn_attr);
td_void viu_drv_get_working_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, ext_viu_ext_chn_attr *ext_chn_attr);

td_u32 viu_drv_get_ext_chn_depth(td_s32 pipe_id, td_s32 chn_id);

td_s32 viu_drv_ext_chn_check_capacity(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame *frame);

td_void viu_drv_cap_dev_ctx_init(td_void);

td_bool viu_drv_is_cap_valid_int(td_u32 cap_int_status);

td_s32 viu_drv_get_pipe_by_proc_id(td_u32 proc_id);
td_bool viu_drv_is_proc_id_valid(td_u32 proc_id);
ext_viu_operating_mode_type viu_drv_proc_get_mode(td_u32 proc_id);
td_void *viu_drv_get_proc_reg_by_proc_id(td_u32 proc_id);

td_u32 viu_drv_is_viu_proc_end_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_time_out_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_bus_err_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_dcmp_err_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_frame_start_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_frame_err_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_frame_flow_int(td_u32 int_status);
td_u32 viu_drv_is_viu_proc_lost_cfg_int(td_u32 int_status);

td_bool viu_drv_is_proc_mode_suitable(td_u32 viu_proc_id, ext_viu_operating_mode_type pipe_mode);

td_bool viu_drv_proc_is_wrong_int(ext_viu_operating_mode_type proc_mode, td_u32 int_status);
td_bool viu_drv_proc_is_lost_cfg_int(td_s32 pipe_id, td_u32 int_status);

td_bool viu_drv_is_proc_frame_start_delay_irq(td_u32 proc_int_status);
td_bool viu_drv_is_proc_frame_end_irq(td_u32 proc_int_status);

td_s32 viu_drv_get_frm_vb_blk(viu_get_frame_vb_info *info, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_get_vb(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid);
td_s32 viu_drv_put_vb_with_ret_value(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid);
td_void viu_drv_put_vb(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid);
td_bool viu_drv_chn_frm_rate_ctl(ext_isp_frame_rate_ctrl *frame_rate, td_s32 *lost_frm_key);

td_void viu_drv_set_work_mode(td_u8 pipe_id, td_bool pipe_online, td_bool chn_online);
td_void viu_drv_get_work_mode(td_u8 pipe_id, td_bool *pipe_online, td_bool *chn_online);

td_s32 viu_drv_get_isp_be_buf_num(td_s32 pipe_id);
td_s32 viu_drv_get_be_stt_info(td_s32 pipe_id, td_u32 block, td_void *be_stt);
td_void viu_drv_switch_be_stt_info(td_s32 pipe_id, td_u32 block);
td_s32 viu_drv_get_frame_info_from_isp(td_s32 pipe_id, ext_isp_frame_alg_info *isp_frame);
td_s32 viu_drv_isp_be_end_int_proc(td_s32 pipe_id);
ext_isp_color_gamut viu_drv_get_color_gamut_from_isp(td_s32 pipe_id);
td_void viu_drv_isp_isr(td_s32 irq, td_void *dev_id);
int viu_drv_isp_int_bottom_half(td_s32 irq, td_void *dev_id);
td_bool viu_drv_is_wdr_pool_id(td_s32 pipe_id, td_s32 pool_id);
td_s32 viu_drv_get_wdrvb(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_drv_check_crop_align(ext_isp_rect crop_rect);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_VIU_H */
