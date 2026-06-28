/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __MSG_H264_H__
#define __MSG_H264_H__

#define P_SLICE 0
#define B_SLICE 1
#define I_SLICE 2

#define H264_MB_SIZE            16
#define H264_QMATRIX_NUM        56
#define H264_MIN_WIDTH_IN_MB    (H264_MIN_PIX_WIDTH >> 4)
#define H264_MIN_HEIGHT_IN_MB   (H264_MIN_PIX_HEIGHT >> 4)
#define H264_MAX_WIDTH_IN_MB    (H264_MAX_PIX_WIDTH >> 4)
#define H264_MAX_HEIGHT_IN_MB   (H264_MAX_PIX_HEIGHT >> 4)


#define H264_SED_TOP_INTER_OFFSET  0

#define H264_MAX_MB_NUM_IN_PIC  (H264_MAX_WIDTH_IN_MB * H264_MAX_HEIGHT_IN_MB)
#define H264_APC_SIZE           16
#define H264_MIN_LEVEL          10
#define H264_MAX_LEVEL          41
#define H264_MAX_META_DATA_SIZE (1 * 1024)

#define H264_APC_POC_NUM 2
#define H264_DEC_STREAM_NUM 2
#define H264_DEC_LIST_NUM 2
#define H264_DEC_LIST_LEN 32
#define H264_DEC_WP_SIZE 32
#define H264_MAX_LDY_NUM 16


typedef struct {
    td_u8 list_structure;     /*   pic structure of this list element. 0: frame, 1: top_field, 2: bottom_field,  */
    td_u8 is_used;            /*   0: empty, 1: top, 2: bottom, 3: both fields (or frame)--output  */
    td_u8 frame_structure;    /*   0: frame, 1: top_field, 2: bottom_field, 3: field pair  */
    td_u8 frame_is_long_term; /*   0: not long term ref,  1: long term ref  */
    td_u8 top_structure;      /*   0: frame, 1: top_field, 2: bottom_field, 3: field pair  */
    td_u8 top_is_long_term;   /*   0: not long term ref,  1: long term ref  */
    td_u8 btm_structure;      /*   0: frame, 1: top_field, 2: bottom_field, 3: field pair  */
    td_u8 btm_is_long_term;   /*   0: not long term ref,  1: long term ref  */
} h264_dec_list;

typedef struct {
    /*   SPS  */
    td_u32 pic_width_in_mb;
    td_u32 pic_height_in_mb;
    td_u8 chroma_format_idc; /*  0:400, 1:420  */
    td_u8 frame_mbs_only_flag;
    td_u8 mb_adaptive_frame_field_flag;

    /*   PPS  */
    td_u8 entropy_coding_mode_flag;
    td_u8 weighted_pred_flag;
    td_u8 weighted_bipred_idc;
    td_u8 constrained_intra_pred_flag;
    td_u8 transform_8x8_mode_flag;
    td_u32 frame_height;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 top_offset;
    td_u32 bottom_offset;

    /*   Pic  */
    td_u8 nal_ref_idc;
    td_u8 cabac_qp_err_dec_en;
    td_u8 new_pic_flag;
    td_u8 pic_structure;
    td_u8 mbaff_frame_flag;
    td_u8 pic_type;
    td_s32 sed_state;
    td_s32 this_poc;
    td_s32 top_poc;
    td_s32 bottom_poc;
    td_u32 fsp_logic_param_id;
    UADDR stream_base_addr;

    td_u16 apc_size;
    td_u32 apc_fsp_id[H264_APC_SIZE];
    td_s32 apc_poc[2][H264_APC_SIZE]; /* 2 is length */
    UADDR apc_fsp_phy_addr[H264_APC_SIZE];
    td_u8 ref_pic_type[H264_APC_SIZE];

    td_u32 qmatrix[H264_QMATRIX_NUM];

    td_u32 ref_pic_single_size;
    td_s32 ref_pic_single_fsp_id[H264_APC_SIZE];
    UADDR ref_pic_single_phy_addr[H264_APC_SIZE];
    td_u8 sample_type;
} dec_pic_param_h264;

typedef struct tag_dec_slice_param_h264 {
    td_s32 stream_id[2]; /* 2 is length */
    td_u32 valid_bitlen[2]; /*   valid bits when dec slice data  2 is length */
    td_u8 bits_offset[2];   /*   offset in 32 bits when this segment stream start  */
    UADDR bytes_pos[2];     /*   stream VDM will used start bytes ph addr, word addr aligned  2 is length */

    td_u32 first_mb_in_slice;
    td_u32 last_mb_in_slice;
    td_u8 slice_type;

    td_u8 cabac_init_idc;
    td_u8 num_ref_idx_l0_active_minus1;
    td_u8 num_ref_idx_l1_active_minus1;
    td_u8 direct_spatial_mv_pred_flag;
    td_u8 direct_8x8_inference_flag;

    td_s8 slice_qpy;
    td_s8 chroma_qp_index_offset;
    td_s8 second_chroma_qp_index_offset;

    td_u8 disable_deblocking_filter_idc;
    td_s8 slice_alpha_c0_offset_div2;
    td_s8 slice_beta_offset_div2;

    h264_dec_list declist[2][32]; /* 2 and 32 are length */
    td_u8 list_x_size[2]; /* 2 is an index */
    td_u32 idx_2_fsp_id[2][H264_APC_SIZE * 2]; /* 2 is length 2 :for double */
    td_u8 idx_2_apc[2][H264_APC_SIZE * 2]; /* 2 is length 2 :for double */
    td_u8 apc_2_ref_idx[H264_APC_SIZE * 2]; /* 2 :for double */
    td_s16 col_pmv_fsp_id;
    UADDR apc_2_mv_addr;

    td_u8 weight_flag;
    td_u8 luma_log2_weight_denom;
    td_u8 chroma_log2_weight_denom;
    td_s16 wp_weight_l0_y[32]; /* 32 is length */
    td_s16 wp_weight_l0_u[32]; /* 32 is length */
    td_s16 wp_weight_l0_v[32]; /* 32 is length */
    td_s16 wp_weight_l1_y[32]; /* 32 is length */
    td_s16 wp_weight_l1_u[32]; /* 32 is length */
    td_s16 wp_weight_l1_v[32]; /* 32 is length */
    td_s8 wp_offset_l0_y[32]; /* 32 is length */
    td_s8 wp_offset_l0_u[32]; /* 32 is length */
    td_s8 wp_offset_l0_v[32]; /* 32 is length */
    td_s8 wp_offset_l1_y[32]; /* 32 is length */
    td_s8 wp_offset_l1_u[32]; /* 32 is length */
    td_s8 wp_offset_l1_v[32]; /* 32 is length */

    struct tag_dec_slice_param_h264 *next_slice;
} dec_slice_param_h264;

#endif
