/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __MSG_H265_H__
#define __MSG_H265_H__

#define HEVC_I_SLICE 2
#define HEVC_P_SLICE 1
#define HEVC_B_SLICE 0

#define HEVC_MB_SIZE                  16
#define HEVC_APC_SIZE                 16
#define MAX_FRAME_NUM                 32
#define MAX_PMV_SLOT_NUM              MAX_FRAME_NUM
#define HEVC_MAX_CTB_NUM_PER_LINE     (16384 / HEVC_MB_SIZE)
#define HEVC_LOGIC_TILE_ROWS_LIMIT    22
#define HEVC_LOGIC_TILE_COLUMNS_LIMIT 20
#define HEVC_MIN_LOG2_CTB_SIZE        4
#define HEVC_MAX_LOG2_CTB_SIZE        6
#define HEVC_MIN_CTB_SIZE             (1 << HEVC_MIN_LOG2_CTB_SIZE)
#define HEVC_MAX_CTB_SIZE             (1 << HEVC_MAX_LOG2_CTB_SIZE)
#define HEVC_MAX_TILE_COLUMNS         20
#define HEVC_MAX_TILE_ROWS            22
#define HEVC_MAX_META_DATA_SIZE       (1 * 1024)
#define HEVC_TILE_NUM 2
#define HEVC_QMATRIX_NUM 256
#define HEVC_STREAM_NUM 2
#define HEVC_LISTX_SIZE_NUM 2
#define HEVC_APC_NUM 2

typedef struct tag_dec_pic_param_h265 {
    /*  SPS  */
    /*  Start copy for dps, do not change order  */
    td_u32 scaling_list_enabled_flag;
    td_u32 amp_enabled_flag;
    td_u32 sample_adaptive_offset_enabled_flag;
    td_u32 pcm_loop_filter_disabled_flag;
    td_u32 strong_intra_smoothing_enabled_flag;
    td_u32 chroma_format_idc;
    td_u32 pic_width_in_luma_samples;
    td_u32 pic_height_in_luma_samples;
    td_u32 bit_depth_y;
    td_u32 qp_bd_offset_y;
    td_u32 bit_depth_c;
    td_u32 qp_bd_offset_c;
    td_u32 pcm_enabled_flag;
    td_u32 pcm_bit_depth_y;
    td_u32 pcm_bit_depth_c;
    td_u32 log2_min_cb_size_y;
    td_u32 log2_ctb_size_y;
    td_u32 log2_min_trafo_size;
    td_u32 log2_max_trafo_size;
    td_u32 log2_min_ipcm_cb_size_y;
    td_u32 log2_max_ipcm_cb_size_y;
    td_u32 max_transform_hierarchy_depth_inter;
    td_u32 max_transform_hierarchy_depth_intra;
    td_u32 max_cu_depth;
    td_u32 pic_width_in_ctbs_y_minus1;
    td_u32 pic_hight_in_ctbs_y_minus1;
    td_u32 for_copy_end_1;
    /*  End copy  */
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 top_offset;
    td_u32 bottom_offset;
    td_u32 left_offset;
    td_u32 right_offset;

    /*  PPS  */
    /*  Start copy for dps, do not change order  */
    td_u32 sign_data_hiding_enabled_flag;
    td_u32 entropy_coding_sync_enabled_flag;
    td_u32 uniform_spacing_flag;
    td_u32 constrained_intra_pred_flag;
    td_u32 transform_skip_enabled_flag;
    td_u32 cu_qp_delta_enabled_flag;
    td_u32 weighted_pred_flag;
    td_u32 weighted_bipred_flag;
    td_u32 transquant_bypass_enabled_flag;
    td_u32 tiles_enabled_flag;
    td_u32 loop_filter_across_tiles_enabled_flag;
    td_u32 diff_cu_qp_delta_depth;
    td_s32 pps_cb_qp_offset;
    td_s32 pps_cr_qp_offset;
    td_u32 num_tile_columns_minus1;
    td_u32 num_tile_rows_minus1;
    td_u32 log2_parallel_merge_level_minus2;
    td_s16 column_bd[HEVC_MAX_TILE_COLUMNS];
    td_s16 row_bd[HEVC_MAX_TILE_ROWS];
    td_u32 for_copy_end_2;
    /*  End copy  */
    td_u32 log2_min_cu_qp_delta_size;

    /*  Pic  */
    td_s32 this_poc;
    td_s32 pic_type;
    td_u32 fsp_logic_id;
    td_u32 sed_state;
    UADDR stream_base_addr;

    td_u32 apc_fsp_id[HEVC_APC_SIZE];
    td_s32 apc_poc[HEVC_APC_SIZE];
    td_u16 apc_size;
    UADDR apc_fsp_phy_addr[HEVC_APC_SIZE];

    td_u8 tile_id[2][HEVC_MAX_CTB_NUM_PER_LINE]; /* 2 is length */
    td_u16 tile_pos_x[HEVC_LOGIC_TILE_COLUMNS_LIMIT][2]; /* 2 is length */
    td_u16 tile_pos_y[HEVC_LOGIC_TILE_ROWS_LIMIT][2]; /* 2 is length */
    td_u32 qmatrix[4 * 64]; /* 4 * 64 is length */
    td_u32 ref_pic_size;
    td_u32 ref_pic_single_size;
    td_s32 ref_pic_single_fsp_id[HEVC_APC_SIZE];
    UADDR ref_pic_single_phy_addr[HEVC_APC_SIZE];
} dec_pic_param_h265;

typedef struct tag_dec_slice_param_h265 {
    td_s32 stream_id[2]; /* 2 is length */
    td_u32 valid_bitlen[2]; /*  valid bits when dec slice data  2 is length */
    td_u8 bits_offset[2];   /*  offset in 8 bits when this segment stream start  2 is length */
    UADDR bytes_pos[2];     /*  stream VDM will used start bytes ph addr, word addr aligned  2 is length */

    td_u8 dependent_slice_segment_flag;
    td_u8 last_slice_in_group;

    /*  Start copy for dps  */
    td_u32 slice_sao_luma_flag;
    td_u32 slice_sao_chroma_flag;
    td_u32 slice_temporal_mvp_enabled_flag;
    td_u32 mvd_l1_zero_flag;
    td_u32 cabac_init_flag;
    td_u32 collocated_from_l0_flag;
    td_u32 slice_deblocking_filter_disabled_flag;
    td_u32 slice_loop_filter_across_slices_enabled_flag;
    td_s32 slice_qp_y;
    td_u32 slice_type;
    td_u32 independent_slice_segment_addr;
    td_u32 max_num_merge_cand;
    td_s32 slice_cb_qp_offset;
    td_s32 slice_cr_qp_offset;
    td_s32 slice_beta_offset_div2;
    td_s32 slice_tc_offset_div2;
    td_u32 listx_size[2]; /* 2 is length */
    td_u32 for_copy_end;

    /*  End copy for dps  */
    td_u32 slice_segment_address;
    td_u8 num_ref_idx_l0_active_minus1;
    td_u8 num_ref_idx_l1_active_minus1;
    td_u32 slice_segment_tile_address;

    td_u8 is_long_term[2][HEVC_APC_SIZE]; /* 2 is length */
    td_u32 idx_2_fsp_id[2][HEVC_APC_SIZE]; /* 2 is length */
    td_u8 idx_2_apc[2][HEVC_APC_SIZE]; /* 2 is length */
    td_s16 col_pmv_fsp_id;
    UADDR apc_2_mv_addr;

    td_u32 end_ctb_in_slice_raster;
    td_u32 end_ctb_in_slice_tile;
    td_u8 is_low_delay;
    td_u8 col_pic_apc;

    td_u8 luma_log2_weight_denom;
    td_u8 chroma_log2_weight_denom;
    td_s16 luma_weight_l0[HEVC_APC_SIZE];
    td_s16 chroma_weight_l0[HEVC_APC_SIZE][2]; /* 2 is length */
    td_s8 luma_offset_l0[HEVC_APC_SIZE];
    td_s8 chroma_offset_l0[HEVC_APC_SIZE][2]; /* 2 is length */
    td_s16 luma_weight_l1[HEVC_APC_SIZE];
    td_s16 chroma_weight_l1[HEVC_APC_SIZE][2]; /* 2 is length */
    td_s8 luma_offset_l1[HEVC_APC_SIZE];
    td_s8 chroma_offset_l1[HEVC_APC_SIZE][2]; /* 2 is length */

    struct tag_dec_slice_param_h265 *next_slice;
} dec_slice_param_h265;

#endif
