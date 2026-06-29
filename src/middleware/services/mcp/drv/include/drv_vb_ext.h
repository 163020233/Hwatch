/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: vb ext define
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __DRV_VB_EXT_H__
#define __DRV_VB_EXT_H__

#include "drv_vb_struct.h"
#include "drv_ext_vb_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ext_isp_pixel_format pixel_format;
    td_u32 width;
    td_u32 height;
    td_u32 buf_line;
    td_u32 align; /* 0:auto */
    ext_isp_dynamic_range dynamic_range;
    ext_isp_compress_mode compress_mode;
    ext_isp_video_format video_format;
    td_bool is_3dnr_buffer;
    td_bool is_3dnr_buffer_share;
} ext_vb_base_info;

typedef enum {
    EXT_VB_SEG_CMP_PARA_MODE,
    EXT_VB_BAYER_CMP_PARA_MODE,
    EXT_VB_3DNR_CMP_PARA_MODE,
    EXT_VB_TILE_CMP_PARA_MODE,
    EXT_VB_CMP_MODE_PARA_MAX
} ext_vb_cmp_para_mode;

typedef struct {
    td_u32 frame_width;
    td_u32 frame_height;
    td_u32 bitdepth;
    td_u32 cmp_mode;        /* 0 --compress, 1-- bypass raw */
    td_u32 comp_ratio_int_y;
    td_u32 fix_k_y;
    td_u32 comp_ratio_int_c;
    td_u32 fix_k_c;
    td_u32 is_single_cpt_y; /* 1--compress y channel only, 0-- do not cmpress y channel only */
    td_u32 is_single_cpt_c; /* 1--compress c channel only, 0-- do not cmpress c channel only */
} ext_vb_tile32x4_cmp_input;

typedef struct {
    ext_vb_cmp_para_mode mode;
    mcp_soc_mod_id mod_id;
    ext_isp_pixel_format pixel_format;
    ext_isp_compress_mode compress_mode;
    ext_isp_data_bitwidth bit_width;
    td_u32 bayer_mode;
    td_u32 width;
    td_u32 height;
    td_bool mirror;
    td_bool tile; /* only compress by GDC is 1. */
    td_u32 tile_height; /* GDC */
    td_bool cmp_mode; /* 0: get cmp param; 1: get dcmp param */
    ext_vb_tile32x4_cmp_input tile32x4_input;
} ext_vb_global_cmp_input;

typedef struct {
    td_u32 align_mode;
    td_u32 is_lossless;
    td_u32 cmp_mode;
    td_u32 frame_width;
    td_u32 frame_height;
    td_u32 rm_debug_en;

    /* rc parameter */
    td_u32 smth_thr;
    td_u32 still_thr;
    td_u32 big_grad_thr;
    td_u32 smth_pix_num_thr;
    td_u32 still_pix_num_thr;
    td_u32 noise_pix_num_thr;
    td_u32 qp_inc1_bits_thr;
    td_u32 qp_inc2_bits_thr;
    td_u32 buf_fullness_thr_reg0;
    td_u32 buf_fullness_thr_reg1;
    td_u32 buf_fullness_thr_reg2;
    td_u32 qp_rge_reg0;
    td_u32 qp_rge_reg1;
    td_u32 qp_rge_reg2;
    td_u32 bits_offset_reg0;
    td_u32 bits_offset_reg1;
    td_u32 bits_offset_reg2;
    td_u32 est_err_gain_map;
    td_u32 buffer_init_bits;
    td_u32 first_mb_adj_bits;
    td_u32 first_row_adj_bits;
    td_u32 first_col_adj_bits;
    td_u32 smooth_status_thr;
    td_u32 budget_mb_bits;
    td_u32 budget_mb_bits_last;
    td_u32 min_mb_bits;
    td_u32 max_mb_qp;
    td_u32 qp_force_en;
} ext_vb_base_cmp_para;

typedef struct {
    td_u32 chroma_en;
    td_u32 esl_qp;
    td_u32 pix_format;
    td_u32 tile_en;
    td_u32 tile_height;
    td_u32 mirror_en;
    td_u32 smth_jdg_mode;
    td_u32 diff_thr;
    td_u32 budget_seg_bits;
    td_u32 smooth_ref_qp;
    td_u32 bits_diff_thr0;
    td_u32 bits_diff_thr1;
    td_u32 bits_diff_thr2;
    td_u32 bits_diff_thr3;
    td_u32 buf_status_thr0;
    td_u32 buf_status_thr1;
    td_u32 buf_status_thr2;
    td_u32 smooth_ana_thr0;
    td_u32 smooth_ana_thr1;
    td_u32 smooth_qp_thr;
    td_u32 luma_min_value_bits;
    td_u32 luma_delta_bits;
    ext_vb_base_cmp_para base_cmp_para;
} ext_vb_cmp_seg_256x1_output;

typedef struct {
    td_u32 stride_en;
    td_u32 stride;
    td_u32 bayer_mode;
    td_u32 bit_depth;
    ext_vb_base_cmp_para base_cmp_para;
} ext_vb_cmp_bayer_output;

typedef struct {
    td_s32 cmp_mode; /* 1: is raw frame compress mode; 0: is not ... */
    td_s32 is_lossless; /* 1: is lossless compress; 0:is lossy compress */
    td_s32 sample_mode; /* 1:422;2:420 */

    td_u32 frame_width; /* input frame width */
    td_u32 frame_height; /* input frame height */
    td_s32 dw_mode; /* pixel bit depth, 0: 10 bit; 1: 8 bit */

    td_s32 part_cmp_en; /* 0: compress total frame; 1: compress partial frame */
    td_s32 pcmp_start_hpos; /* start horizontal position of partial cmp */
    td_s32 pcmp_end_hpos; /* end horizontal position of partial cmp */

    td_s32 rm_dbg_en; /* 1: rm debug enable; 0: rm debug unable */

    /* register parameter of ratio control */
    td_s32 mb_bits;
    td_s32 pix_diff_thr;
    td_s32 smth_thr;
    td_s32 still_thr;
    td_s32 big_grad_thr;
    td_s32 smth_pix_num_thr;
    td_s32 still_pix_num_thr;
    td_s32 noise_pix_num_thr;
    td_s32 adj_sad_bits_thr;
    td_s32 qp_inc1_bits_thr;
    td_s32 qp_inc2_bits_thr;
    td_s32 qp_dec1_bits_thr;
    td_s32 qp_dec2_bits_thr;
    td_s32 max_qp;
    td_s32 sad_bits_gain;
    td_s32 rc_smth_ngain;
    td_s32 max_trow_bits;
    td_s32 min_sad_thr;
    td_s32 max_sad_thr;
    td_s32 noise_sad;
    td_s32 min_mb_bits;
    td_s32 est_err_gain;
    td_s32 max_est_err_level;
    td_s32 vbv_buf_loss1_thr;
    td_s32 vbv_buf_loss2_thr;
} ext_vb_cmp_3dnr_output_v1;

typedef struct {
    td_u32 frame_width;
    td_u32 frame_height;
    td_u32 bit_depth;
    td_u32 chroma_en;
    td_u32 stride_en;
    td_u32 stride;
    td_u32 align_mode;
    td_u32 cmp_mode;
    td_u32 is_lossless;
    td_u32 pix_format;
    td_u32 adp_mode;

    /* rc parameter */
    td_u32 smth_thr;
    td_u32 still_thr;
    td_u32 big_grad_thr;
    td_u32 diff_thr;
    td_u32 smth_pix_num_thr;
    td_u32 still_pix_num_thr;
    td_u32 noise_pix_num_thr;
    td_u32 grph_group_num_thr;
    td_u32 qp_inc1_bits_thr;
    td_u32 qp_inc2_bits_thr;
    td_u32 raw_bits_penalty;
    td_u32 grph_bits_penalty;
    td_u32 buf_fullness_thr_reg0;
    td_u32 buf_fullness_thr_reg1;
    td_u32 buf_fullness_thr_reg2;
    td_u32 qp_rge_reg0;
    td_u32 qp_rge_reg1;
    td_u32 qp_rge_reg2;
    td_u32 bits_offset_reg0;
    td_u32 bits_offset_reg1;
    td_u32 bits_offset_reg2;
    td_u32 grph_loss_thr;
    td_u32 est_err_gain_map;
    td_u32 buffer_init_bits;
    td_u32 first_mb_adj_bits;
    td_u32 first_row_adj_bits;
    td_u32 first_col_adj_bits;
    td_u32 smooth_status_thr;
    td_u32 budget_mb_bits;
    td_u32 budget_mb_bits_last;
    td_u32 min_mb_bits;
    td_u32 max_mb_qp;
} ext_vb_cmp_3dnr_output_v2;

typedef struct {
    unsigned int frame_width;
    unsigned int frame_height;
    unsigned int slice_width;
    unsigned int slice_height;
    unsigned int cmp_mode;
    unsigned int is_lossless;
    unsigned int pix_format;
    unsigned int bit_depth;

    unsigned int chroma_en;
    unsigned int graphic_en;
    unsigned int conv_en;
    /* rc para */
    unsigned int coeff_skip_en;
    unsigned int rc_lossless_protect_en;
    unsigned int low_power_alg_en;
    unsigned int rdo_lambda_en;
    unsigned int flat_vert_grad_en;
    unsigned int subqp_flag_en;
    unsigned int top_diff_flag_en;

    unsigned int mb_raw_dir;
    unsigned int rdo_lambda_factor;
    unsigned int grph_loss_thr;
    unsigned int smth_thr;
    unsigned int still_thr;
    unsigned int still_thr_1stline;
    unsigned int flat_cancel_thr;
    unsigned int big_grad_thr;
    unsigned int diff_thr;
    unsigned int smth_pix_num_thr;
    unsigned int still_pix_num_thr;
    unsigned int sub_diff_thr1;
    unsigned int sub_min_thr1;
    unsigned int sub_diff_thr2;
    unsigned int sub_min_thr2;
    unsigned int ver_gradthr;
    unsigned int maxqp_gradthr;
    unsigned int low_power_grad_thr;
    unsigned int buffer_init_bits;
    unsigned int budget_mb_bits;
    unsigned int budget_mb_bits_last;
    unsigned int begin_control_blks;
    unsigned int g_delta;
    unsigned int min_mb_bits;
    unsigned int max_mb_qp;
    unsigned int grad_diff_thr1;
    unsigned int grad_diff_thr2;
    unsigned int qp_bits_thr;
    unsigned int qp_inc1_bits_thr;
    unsigned int qp_inc2_bits_thr;
    unsigned int qp_inc4_bits_thr;
    unsigned int qp_dec1_bits_thr;
    unsigned int qp_dec2_bits_thr;
    unsigned int qp_dec4_bits_thr;
    unsigned int near_adjust_en;
    unsigned int near_grad_thr;
    unsigned int near_diff_thr;
    int near_type_thr;
    unsigned int hsgrad_shift;
    unsigned int near_qpmax;
    /* GQB PARA */
    int gqb_bitof_reg0;
    int gqb_bitof_reg1;
    int gqb_bitof_reg2;
    int gqb_bitof_reg3;
    int gqb_bitof_reg4;
    int gqb_bitof_reg5;
    int gqb_bitof_reg6;
    int gqb_bitof_reg7;
    unsigned int gqb_chroma_scale;
    unsigned int raw_bits_penalty;
    unsigned int grph_bits_penalty;
    unsigned int left_pred_penalty;
    unsigned int left_pred_penalty_nsmth;
    unsigned int first_mb_adj_bpp;
    unsigned int first_row_adj_bpp;
    unsigned int first_col_adj_bpp;
    unsigned int buf_fullness_thr_reg0;
    unsigned int buf_fullness_thr_reg1;
    unsigned int buf_fullness_thr_reg2;
    int bits_offset_reg0;
    int bits_offset_reg1;
    int bits_offset_reg2;
    unsigned int qp_rge_reg0;
    unsigned int qp_rge_reg1;
    unsigned int qp_rge_reg2;
    unsigned int smooth_status_thr;
    unsigned int noise_status_thr;
    unsigned int hsfrm_grad_idx;
} ext_vb_cmp_3dnr_output_v3;

typedef struct {
    union {
        ext_vb_cmp_3dnr_output_v1 nr_cmp_output_v1;  //  850
        ext_vb_cmp_3dnr_output_v2 nr_cmp_output_v2;  //  dv500, 1a22使用的压缩与dv500对应
        ext_vb_cmp_3dnr_output_v3 nr_cmp_output_v3;  // 610 ，3dnr压缩使用的是dv500的版本，参数配置使用的是610的版本
    };
} ext_vb_cmp_3dnr_output;

typedef struct {
    td_u32 frame_width;
    td_u32 frame_height;
    td_u32 bitdepth;
    td_u32 cmp_mode;        /* 0 --compress, 1-- bypass raw */
    td_u32 segment_cfg_tar_bit_y;
    td_u32 fix_k_y;
    td_u32 segment_cfg_tar_bit_c;
    td_u32 fix_k_c;
    td_u32 is_single_cpt_y; /* 1--compress y channel only, 0-- do not cmpress y channel only */
    td_u32 is_single_cpt_c; /* 1--compress c channel only, 0-- do not cmpress c channel only */
} ext_vb_cmp_tile32x4_output;

typedef struct {
    ext_vb_cmp_para_mode mode;
    union {
        ext_vb_cmp_seg_256x1_output output_seg_256x1_para;
        ext_vb_cmp_bayer_output output_bayer_para;
        ext_vb_cmp_3dnr_output output_3dnr_para;
        ext_vb_cmp_tile32x4_output output_tile_32x4_para;
    };
} ext_vb_cmp_para_output;

typedef struct {
    td_s32 (*pfn_vb_create_pool)(td_u32 *pool_id,
                                   td_u32 blk_cnt, td_u64 blk_size,
                                   const td_char *buf_name,
                                   ext_vb_remap_mode remap_mode);
    td_s32 (*pfn_vb_destroy_pool)(td_u32 pool_id);
    vb_blk_handle (*pfn_vb_get_blk_by_pool_id)(td_u32 pool_id, td_u32 uid);
    vb_blk_handle (*pfn_vb_get_blk_by_size)(td_u64 blk_size, td_u32 uid);
    vb_blk_handle (*pfn_vb_get_blk_by_size_and_pool_id)(td_u32 pool_id, td_u64 blk_size, td_u32 uid);
    td_s32 (*pfn_vb_get_pool_info)(td_u32 pool_id, ext_vb_pool_info *info);
    td_s32 (*pfn_vb_put_blk)(td_u32 pool_id, td_u64 phy_addr);
    vb_blk_handle (*pfn_vb_phy_to_handle)(td_u64 phy_addr);
    td_void* (*pfn_vb_phy_to_dmabuf)(td_u64 phy_addr);
    td_u32 (*pfn_vb_handle_to_pool_id)(vb_blk_handle handle);
    td_u32 (*pfn_vb_handle_to_blk_id)(vb_blk_handle handle);
    td_u64 (*pfn_vb_handle_to_kern)(vb_blk_handle handle);
    td_u64 (*pfn_vb_handle_to_phys)(vb_blk_handle handle);
    td_u64 (*pfn_vb_handle_to_blk_size)(vb_blk_handle handle);
    td_s32 (*pfn_vb_user_add)(td_u32 pool_id, td_u64 phy_addr, td_u32 uid);
    td_s32 (*pfn_vb_user_sub)(td_u32 pool_id, td_u64 phy_addr, td_u32 uid);
    td_u32 (*pfn_vb_inquire_user_cnt)(vb_blk_handle handle);
    td_u32 (*pfn_vb_inquire_one_user_cnt)(vb_blk_handle handle, td_u32 uid);
    td_u32 (*pfn_vb_inquire_blk_cnt)(td_u32 uid, td_bool is_comm_pool);
    td_bool (*pfn_vb_is_blk_valid)(td_u32 pool_id, td_u64 phy_addr);
    td_bool (*pfn_vb_is_pool_id_valid)(td_u32 pool_id);
    td_s32 (*pfn_vb_inquire_pool)(td_u32 pool_id, ext_vb_pool_status *pool_status);
    td_s32 (*pfn_vb_inquire_pool_user_cnt)(td_u32 pool_id, td_u32 uid, td_u32 *cnt);
    ext_isp_video_supplement *(*pfn_vb_handle_to_supplement)(vb_blk_handle handle);
    td_bool (*pfn_vb_is_supplement_support)(td_u32 mask);
    td_void (*pfn_vb_copy_supplement)(ext_isp_video_frame *dst_v_frame, const ext_isp_video_frame *src_v_frame);

    /* Compression parameters are strongly related to configuration */
    td_s32 (*pfn_vb_get_vb_cfg)(ext_vb_base_info *info, ext_isp_vb_cal_config *vb_cal_config);
    td_s32 (*pfn_vb_get_cmp_cfg)(ext_vb_global_cmp_input *cmp_in, ext_vb_cmp_para_output *y_cmp_para_output,
        ext_vb_cmp_para_output *c_cmp_para_output);
    /* userspace external buffers */
    td_s32 (*pfn_vb_queue_init)(ext_vb_queue *q);
    td_void (*pfn_vb_queue_release)(ext_vb_queue *q);
    td_void (*pfn_vb_queue_buf_done)(ext_vb_buffer *vb, ext_vb_buf_state state);
    td_s32 (*pfn_vb_queue_reqbufs)(ext_vb_queue *q, ext_vb_requestbuffers *req);
    td_s32 (*pfn_vb_queue_querybuf)(ext_vb_queue *q, ext_vb_user_buffer *user_vb);
    td_s32 (*pfn_vb_queue_qbuf)(ext_vb_queue *q, ext_vb_user_buffer *user_vb);
    td_s32 (*pfn_vb_queue_dqbuf)(ext_vb_queue *q, ext_vb_user_buffer *user_vb, td_s32 msec);
    td_s32 (*pfn_vb_queue_streamon)(ext_vb_queue *q);
    td_s32 (*pfn_vb_queue_streamoff)(ext_vb_queue *q);
} vb_export_func;

td_s32 drv_vb_mod_init(td_void);
td_void drv_vb_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif