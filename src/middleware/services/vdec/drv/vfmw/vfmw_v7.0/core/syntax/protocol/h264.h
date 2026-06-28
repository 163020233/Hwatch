/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_H264_H__
#define __SYN_H264_H__

#include "syn_base.h"
#include "stm_intf.h"
#include "syn_bs.h"
#include "msg_h264.h"
#include "usrdata.h"
#include "stm_base.h"
#include "dps.h"
#include "syn_parse.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************ h264 function define *********************/
/*  H264_PVR_SUPPORT NOT define
 *  H264_USRDATA_SUPPORT NOT define
 *  H264_RESOLUTION_CHANGE_SUPPORT NOT define
 */
/******************************************************************/

#define LUMA_STORGAM_NUM 32 /* add from vdm_drv.h V5R1 */

typedef struct { /* add from vdm_drv.h V5R1 */
    td_u32 luma_sum_high;
    td_u32 luma_sum_low;
    td_u32 luma_historgam[LUMA_STORGAM_NUM];
} luma_info;

#define H264_MAX_NUM_REF_PICS  16
#define H264_STREAM_BASE_ALIGN 3
#define H264_ERR_FS_OUT_AGAIN  (-3)
#define H264_NOTDEC            (-2)
#define H264_ERR               (-1)
#define H264_OK                0
#define H264_ALREADY_OUT       1
#define H264_OUTPUT_FAIL       2
#define H264_MEET_EOS          3

/********************* error information define *******************/
#define SPS_ERR          (-1)
#define SE_ERR           (-1)
#define PPS_ERR          (-1)
#define VLC_CODE_ERR     (-1)
#define FLC_ERR          (-1)
#define IDR_ERR          (-1)
#define SLICE_HEADER_ERR (-1)
#define MMCO_NOT_EQUAL   (-1)
#define RECOVERPOINT_ERR (-1)
#define REDUNDANTPIC_ERR (-1)
#define PIC_INITIAL_ERR  (-1)
#define INITDPB_ERR      (-1)
#define OUTPUT_ERR       (-1)

/******************* some define of start contrl ******************/
#define START_FRAME 0

#define MAX_BYTES_START   (512 * 1024) /* 512*1024 bytes received, start VDM to dec */
#define GET_ONE_NALU_SIZE (4 * 1024)   /* SCD_SEG_BLANK_LEN  vfmw_min len of combind pack */

/***************************** define *****************************/
#define PIC_DECODING 1
#define EMPTY        0

#define BASELINE                  66
#define MAINPROFILE               77
#define EXTENDED                  88
#define HIGHPROFILE               100 /* !< YUV 4:2:0/8 "High" */
#define HIGH10P                   110 /* !< YUV 4:2:0/10 "High 10" */
#define HIGH422P                  122 /* !< YUV 4:2:2/10 "High 4:2:2" */
#define HIGH444P                  144 /* !< YUV 4:4:4/12 "High 4:4:4" */
#define HIGH444PP                 244
#define SCALABLE_BASELINE_PROFILE 83 /* add for svc base profile */
#define SCALABLE_HIGH_PROFILE     86 /* add for svc high profile */

/* define para */
/* FRAME       defined  (0) in syn_base.h */
#define TOP_FIELD    1
#define BOTTOM_FIELD 2
#define FIELD_PAIR   3

#define MAX_DPB_LEN       33
#define MAX_LIST_SIZE     33
#define MAX_FRAME_STORE   40 /* Maximun vdm slot */
#define MAX_PMV_STORE     17 /* 16 for vfmw_max dpb + 1 for dec_pic */
#define VO_HOLD_FRAME_NUM 3  /* vo hold the number of frames */
#define MAX_MMCO_LEN      100
#define MAX_CYCLE_NUM     400

#define NALU_TYPE_SLICE    1
#define NALU_TYPE_DPA      2
#define NALU_TYPE_DPB      3
#define NALU_TYPE_DPC      4
#define NALU_TYPE_IDR      5
#define NALU_TYPE_SEI      6
#define NALU_TYPE_SPS      7
#define NALU_TYPE_PPS      8
#define NALU_TYPE_AUD      9
#define NALU_TYPE_EOSEQ    10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL     12
#define NALU_TYPE_SPSEXT   13
#define NALU_TYPE_AUX      19
#define NALU_TYPE_EOPIC    30 /* end curr pic, followed by 0x4853504943454E44 */
#define NALU_TYPE_TSTEND   31 /* end test, start last pic */
#define NALU_TYPE_SUB_SPS  15
#define NALU_TYPE_SLC_EXT  20

/* define decode mode */
#define IMODE   2 /* only dec I pic */
#define IPMODE  1 /* dec I pic, p pic and ref b pic, no ref b pic not dec */
#define IPBMODE 0 /* all pic dec */

#define H264_MAX_SPS_NUM 32
#define H264_MAX_PPS_NUM 256
#define H264_MEM_NUM     (H264_MAX_SPS_NUM + H264_MAX_PPS_NUM)

/* for image format clean bits */
#define CLEAN_FRAME_TYPE      (~3)
#define CLEAN_VIDEO_FORMAT    (~(7 << 5))
#define CLEAN_SOURCE_FORMAT   (~(3 << 8))
#define CLEAN_FIELD_VALID     (~(3 << 10))
#define CLEAN_TOP_FILED_FIRST (~(3 << 12))
#define CLEAN_ASPECT_RATIO    (~(7 << 14))

/************************ structure define ************************/
typedef enum {
    SEI_BUFFERING_PERIOD = 0,
    SEI_PIC_TIMING,
    SEI_PAN_SCAN_RECT,
    SEI_FILLER_PAYLOAD,
    SEI_USER_DATA_REGISTERED_ITU_T_T35,
    SEI_USER_DATA_UNREGISTERED,
    SEI_RECOVERY_POINT,
    SEI_DEC_REF_PIC_MARKING_REPETITION,
    SEI_SPARE_PIC,
    SEI_SCENE_INFO,
    SEI_SUB_SEQ_INFO,
    SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
    SEI_SUB_SEQ_CHARACTERISTICS,
    SEI_FULL_FRAME_FREEZE,
    SEI_FULL_FRAME_FREEZE_RELEASE,
    SEI_FULL_FRAME_SNAPSHOT,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
    SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
    SEI_FILM_GRAIN_CHARACTERISTICS,
    SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,
    SEI_STEREO_VIDEO_INFO,
    SEI_FRAME_PACKING_ARRANGEMENT = 45,
    SEI_INDUSTRY_FEATURES1_PRIVATE = 0xe5,
    SEI_INDUSTRY_FEATURES1_USER_DATA = 0xf0,
    SEI_CUSTOMIZED_TYPE_START = 0xf8, /* customized sei type start */
    SEI_CUSTOMIZED_SPLICE_INFO, /* customized sei type for spliced stream sp420 + A8 */
} sei_type;

typedef struct {
    td_u8 *stream_buffer;    /* [MAX_CODED_FRAME_SIZE], actual codebuffer for read bytes */
    td_u32 bitstream_length; /* over codebuffer length, byte oriented, UVLC only */
    UADDR stream_phy_addr;   /* physic addr of stream */
    td_u32 valid_bitlen;     /* valid bits when dec slice data */
    td_u32 bits_offset;      /* offset in 32 bits when this segment stream start */
    UADDR bytes_pos;         /* stream VDM will used start bytes ph addr, word addr aligned */
    td_s32 stream_id;
} h264_bit_stream;

typedef struct {
    td_u8 nal_used_segment; /* which segment using decode */
    td_u8 is_valid;
    td_u8 nal_unit_type;
    td_u8 svc_extension_flag;
    td_u8 nal_integ;
    td_u8 dsp_check_pic_over_flag;
    h264_bit_stream stream[2]; /* 2 :a number */
    td_u32 nal_bitoffset;
    td_u32 nal_segment; /* how many stream segment received in this nal */
    td_u32 nal_trail_zero_bit_num;
    td_u32 nal_ref_idc;
    td_u32 forbidden_bit;
    td_s32 raw_stream_len;
} h264_nalu;

typedef struct {
    td_u8 cbr_flag[32]; /* 32 :a number */
    td_u8 bit_rate_scale;
    td_u8 cpb_size_scale;
    td_u32 cpb_cnt_minus1;
    td_u32 bit_rate_value_minus1[32]; /* 32 :a number */
    td_u32 cpb_size_value_minus1[32]; /* 32 :a number */
    td_u32 initial_cpb_removal_delay_length_minus1;
    td_u32 cpb_removal_delay_length_minus1;
    td_u32 dpb_output_delay_length_minus1;
    td_u32 time_offset_length;
} h264_hdr_params;

typedef struct {
    td_u8 aspect_ratio_info_present_flag;
    td_u8 overscan_info_present_flag;
    td_u8 overscan_appropriate_flag;
    td_u8 video_signal_type_present_flag;
    td_u8 video_full_range_flag;
    td_u8 colour_description_present_flag;
    td_u8 chroma_location_info_present_flag;
    td_u8 nal_hrd_parameters_present_flag;
    td_u8 timing_info_present_flag;
    td_u8 fixed_frame_rate_flag;
    td_u8 vcl_hrd_parameters_present_flag;
    td_u8 pic_struct_present_flag;
    td_u8 bitstream_restriction_flag;
    td_u8 motion_vectors_over_pic_boundaries_flag;
    td_u8 low_delay_hrd_flag;
    td_u8 aspect_ratio_idc;
    td_u32 sar_width;
    td_u32 sar_height;
    td_u32 aspect_ratio;
    td_u32 video_format;
    td_u32 colour_primaries;
    td_u32 transfer_characteristics;
    td_u32 matrix_coefficients;
    td_u32 chroma_sample_loc_type_top_field;
    td_u32 chroma_sample_loc_type_bottom_field;
    td_u32 num_units_in_tick;
    td_u32 time_scale;
    td_u32 max_bytes_per_pic_denom;
    td_u32 max_bits_per_mb_denom;
    td_u32 log2_max_mv_length_vertical;
    td_u32 log2_max_mv_length_horizontal;
    td_u32 num_reorder_frames;
    td_u32 max_dec_frame_buffering;
    h264_hdr_params nal_hrd_parameters;
    h264_hdr_params vcl_hrd_parameters;
} h264_uvi_seq_params;

typedef struct {
    td_u8 constrained_set0_flag;                /* u(1) */
    td_u8 constrained_set1_flag;                /* u(1) */
    td_u8 constrained_set2_flag;                /* u(1) */
    td_u8 constrained_set3_flag;                /* u(1) */
    td_s8 seq_scaling_list_present_flag[12];    /* u(1)  12 :a number */
    td_u8 delta_pic_order_always_zero_flag;     /* u(1) */
    td_u8 gaps_in_frame_num_value_allowed_flag; /* u(1) */
    td_u8 frame_mbs_only_flag;                  /* u(1) */
    td_u8 mb_adaptive_frame_field_flag;         /* u(1) */
    td_u8 direct_8x8_inference_flag;            /* u(1) */
    td_u8 frame_cropping_flag;                  /* u(1) */
    td_u8 vui_parameters_present_flag;          /* u(1) */
    td_u8 is_valid;                             /* the sps content of this sps_id is geted or not */
    td_u8 is_refreshed;
    td_u8 seq_scaling_matrix_present_flag;

    td_u32 profile_idc;          /* u(8) */
    td_u32 level_idc;            /* u(8) */
    td_u32 seq_parameter_set_id; /* ue(v) */
    td_u32 chroma_format_idc;
    td_u32 bit_depth_luma;   /* ue(v) */
    td_u32 bit_depth_chroma; /* ue(v) */
#ifdef VFMW_INDUSTRY_FEATURES1_SUPPORT
    td_u8 scaling_list4x4[6][16]; /* se(v)  6 :a number 16 :a number */
    td_u8 scaling_list8x8[6][64]; /* se(v)  6 :a number 64 :a number */
#else
    td_u32 scaling_list4x4[6][16]; /* se(v)  6 :a number 16 :a number */
    td_u32 scaling_list8x8[6][64]; /* se(v)  6 :a number 64 :a number */
#endif
    td_u32 qmatrix4x4[6][4];                      /* 16 bytes within 4 words */
    td_u32 qmatrix8x8[2][16];                     /* 64 bytes within 16words  2 :a number 16 :a number */
    td_u32 log2_max_frame_num_minus4;             /* ue(v) */
    td_u32 pic_order_cnt_type;                    /* ue(v) */
    td_u32 log2_max_pic_order_cnt_lsb_minus4;     /* ue(v) */
    td_s32 offset_for_non_ref_pic;                /* se(v) */
    td_s32 offset_for_top_to_bottom_field;        /* se(v) */
    td_u32 num_ref_frames_in_pic_order_cnt_cycle; /* ue(v) */
    td_s32 offset_for_ref_frame[256];             /* se(v)  256 :a number */
    td_u32 num_ref_frames;                        /* ue(v) */
    td_u32 pic_width_in_mbs_minus1;               /* ue(v) */
    td_u32 pic_height_in_map_units_minus1;        /* ue(v) */
    td_u32 frame_crop_left_offset;                /* ue(v) */
    td_u32 frame_crop_right_offset;               /* ue(v) */
    td_u32 frame_crop_top_offset;                 /* ue(v) */
    td_u32 frame_crop_bottom_offset;              /* ue(v) */
    td_u32 dpb_size_plus1;                        /* calculate from level_idc */
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 disp_center_x;
    td_u32 disp_center_y;
    h264_uvi_seq_params vui_seq_parameters;
} h264_sps;

typedef struct {
    td_u8 entropy_coding_mode_flag;               /* u(1) */
    td_u8 pic_order_present_flag;                 /* u(1) */
    td_u8 weighted_pred_flag;                     /* u(1) */
    td_u8 deblocking_filter_control_present_flag; /* u(1) */
    td_u8 constrained_intra_pred_flag;            /* u(1) */
    td_s8 pic_scaling_list_present_flag[12];      /* u(1)  12 :a number */
    td_u8 redundant_pic_cnt_present_flag;         /* u(1) */
    td_u8 pic_scaling_matrix_present_flag;
    td_u8 is_valid; /* the Pps content of this Pps_id is geted or not */
    td_u8 is_refreshed;
    td_u32 pic_parameter_set_id;         /* ue(v) */
    td_u32 seq_parameter_set_id;         /* ue(v) */
    td_u32 num_slice_groups_minus1;      /* ue(v) */
    td_u32 num_ref_idx_l0_active_minus1; /* ue(v) */
    td_u32 num_ref_idx_l1_active_minus1; /* ue(v) */
    td_u32 weighted_bipred_idc;          /* u(2) */
    td_s32 pic_init_qp_minus26;          /* se(v) */
    td_s32 chroma_qp_index_offset;       /* se(v) */
    td_s32 second_chroma_qp_index_offset;
    td_u32 transform_8x8_mode_flag;
#ifdef VFMW_INDUSTRY_FEATURES1_SUPPORT
    td_u8 scaling_list4x4[6][16]; /* se(v)  6 :a number 16 :a number */
    td_u8 scaling_list8x8[6][64]; /* se(v)  6 :a number 64 :a number */
#else
    td_u32 scaling_list4x4[6][16]; /* se(v)  6 :a number 16 :a number */
    td_u32 scaling_list8x8[6][64]; /* se(v)  6 :a number 64 :a number */
#endif
    td_u32 qmatrix4x4[6][4];  /* 16 bytes within 4words */
    td_u32 qmatrix8x8[2][16]; /* 64 bytes within 16words  2 :a number 16 :a number */
} h264_pps;

typedef struct {
    td_u32 update_flag;
    td_u32 sei_len;
    td_u32 sei_code;
} h264_sei;

typedef struct {
    td_u8 field_pic_flag;
    td_u8 bottom_field_flag;
    td_u8 nal_unit_type;
    td_u32 seq_parameter_set_id;
    td_u32 pic_parameter_set_id;
    td_u32 nal_ref_idc;
    td_u32 frame_num;
    td_u32 idr_pic_id;
    td_u32 pic_oder_cnt_lsb;
    td_s32 delta_pic_oder_cnt_bottom;
    td_s32 delta_pic_order_cnt[2]; /* 2 :a number */
    td_u32 first_mb_in_slice;
} h264_prev_slice;

typedef struct {
    td_u8 slice_type;
    td_u8 field_pic_flag;
    td_u8 bottom_field_flag;
    td_u8 nal_unit_type;
    td_u8 mbaff_frame_flag;
    td_u8 new_pic_flag;
    td_u8 direct_spatial_mv_pred_flag;
    td_u8 apply_weights_flag;
    td_u8 pic_over_flag;
    td_u32 pic_parameter_set_id;
    td_u32 prev_frame_num;
    td_u32 frame_num;
    td_u32 len;
    td_u32 valid_bitlen;
    td_u32 nal_ref_idc;
    td_u32 bit_num;
    td_u32 idr_pic_id;
    td_u32 listx_size[2]; /* 2 :a number */
    td_u32 first_mb_in_slice;
    td_u32 num_ref_idx_l0_active_minus1;
    td_u32 num_ref_idx_l1_active_minus1;
    td_u32 luma_log2_weight_denom;
    td_u32 chroma_log2_weight_denom;
    td_s32 wp_weight_l0_y[32]; /* 32 :a number */
    td_s32 wp_weight_l0_u[32]; /* 32 :a number */
    td_s32 wp_weight_l0_v[32]; /* 32 :a number */
    td_s32 wp_weight_l1_y[32]; /* 32 :a number */
    td_s32 wp_weight_l1_u[32]; /* 32 :a number */
    td_s32 wp_weight_l1_v[32]; /* 32 :a number */
    td_s32 wp_offset_l0_y[32]; /* 32 :a number */
    td_s32 wp_offset_l0_u[32]; /* 32 :a number */
    td_s32 wp_offset_l0_v[32]; /* 32 :a number */
    td_s32 wp_offset_l1_y[32]; /* 32 :a number */
    td_s32 wp_offset_l1_u[32]; /* 32 :a number */
    td_s32 wp_offset_l1_v[32]; /* 32 :a number */
    td_u32 cabac_init_idc;
    td_s32 slice_qp_delta;
    td_u32 disable_deblocking_filter_idc;
    td_s32 slice_alpha_c0_offset_div2;
    td_s32 slice_beta_offset_div2;
    td_s32 picid_refidx0;
    td_s32 picid_nearpoc;
    td_u8 slice_output_flag;
} h264_slice;

typedef struct {
    /* for POC mode 0: */
    td_u32 pic_order_cnt_lsb;
    td_s32 delta_pic_order_cnt_bottom;
    td_s32 prev_pic_order_cnt_msb;
    td_u32 prev_pic_order_cnt_lsb;
    td_s32 pic_order_cnt_msb;
    td_s32 last_pic_order_cnt_lsb;
    td_s32 last_has_mmco_5;
    td_s32 last_pic_bottom_field;
    td_s32 toppoc;
    td_s32 bottompoc;
    td_s32 framepoc;
    td_s32 this_poc;
    /* for POC mode 1: */
    td_s32 delta_pic_order_cnt[2]; /* 2 :a number */
    td_u32 abs_frame_num;
    td_u32 prev_frame_num;
    td_u32 frame_num;
    td_u32 frame_num_offset;
    td_s32 prev_frame_num_offset;
    /* for POC mode 2: */
    td_s32 frame_num_in_poc_cycle;
    td_s32 poc_cycle_cnt;
    td_s32 expected_poc;
    td_s32 expected_delta_per_poc_cycle;
} h264_poc;

typedef struct {
    td_u8 ref_pic_list_reordering_flag_l0;
    td_u8 ref_pic_list_reordering_flag_l1;
    td_u32 reordering_of_pic_nums_idc_l0[33]; /* 33 :a number */
    td_u32 reordering_of_pic_nums_idc_l1[33]; /* 33 :a number */
    td_u32 abs_diff_pic_num_minus1_l0[33]; /* 33 :a number */
    td_u32 abs_diff_pic_num_minus1_l1[33]; /* 33 :a number */
    td_u32 long_term_pic_idx_l0[32]; /* 32 :a number */
    td_u32 long_term_pic_idx_l1[32]; /* 32 :a number */
} h264_reorder;

typedef struct {
    td_u32 memory_management_control_operation;
    td_u32 difference_of_pic_nums_minus1;
    td_u32 long_term_pic_num;
    td_u32 long_term_frame_idx;
    td_u32 max_long_term_frame_idx_plus1;
} h264_mmco;

typedef struct {
    td_u8 is_valid;
    td_u8 no_output_of_prior_pics_flag;
    td_u8 long_term_reference_flag;
    td_u8 adaptive_ref_pic_marking_mode_flag;
    h264_mmco mmco[MAX_MMCO_LEN];
} h264_mark;

typedef struct {
    td_s8 frame_packing_dec_ok;
    td_s8 frame_packing_arrangement_cancel_flag;
    td_s8 frame_packing_arrangement_extension_flag;
    td_s8 quincunx_sampling_flag;
    td_s8 spatial_flipping_flag;
    td_s8 frame0_flipped_flag;
    td_s8 field_views_flag;
    td_s8 current_frame_is_frame0_flag;
    td_s8 frame0_self_contained_flag;
    td_s8 frame1_self_contained_flag;
    td_s8 frame0_grid_position_x;
    td_s8 frame0_grid_position_y;
    td_s8 frame1_grid_position_x;
    td_s8 frame1_grid_position_y;
    td_s32 frame_packing_arrangement_type;
    td_s32 content_interpretation_type;
    td_s32 frame_packing_arrangement_id;
    td_s32 frame_packing_arrangement_reserved_byte;
    td_s32 frame_packing_arrangement_repetition_period;
} h264_frame_packing_arrangement;

typedef struct {
    td_u32 bp_num;
    td_u32 sei_timing_num;
    td_u32 sei_timing_num_sub;

    td_u32 cpb_removal_delay;
    td_u32 prev_au_cpb_removal_delay;
    td_u32 dpb_output_delay;
    td_u64 dpb_output_time;
    td_u64 frame_output_time;
    td_u64 au_removal_tm;
    td_u64 removal_tm_of_first_pic_in_bp;

    td_u32 vui_num_units_in_tick;
    td_u32 vui_time_scale;

    td_s64 last_idr_pts;
    td_u64 last_idr_frame_output_time;
    /* that of pic */
    td_u8 idr_pic;
    td_u8 new_bp_start;
} h264_sei_pts_parser;

typedef struct {
    td_u8 pic_struct;
    td_u32 cpb_removal_delay;
    td_u32 dpb_output_delay;
} h264_pic_timing_sei;

typedef struct {
    td_u8 structure;     /* 0: frame, 1: top_field, 2: bottom_field, 3: field pair */
    td_u8 is_long_term;  /* 0: not long term ref, 1: long term ref */
    td_u8 is_short_term; /* 0: not short term ref, 1: short term ref. BUT one pic cann't long and short. */

    td_s32 long_term_pic_num;
    td_s32 pic_num;
    td_s32 poc;

    td_u32 err_level;
    td_u32 pic_type;
    td_s32 max_qp;
    td_s32 min_qp;
    td_s32 avg_qp;
    struct h264_frame_store *frame_store;
} h264_store_pic;

typedef enum {
    FO_STATE_NA,
    FO_STATE_FAST_OUTPUT,
    FO_STATE_NORMAL_ORDER_OUTPUT,
    FO_STATE_MAX
} h264_fo_state;

typedef struct h264_frame_store {
    td_u8 non_existing;   /* 1: fs not exist, 0: exist */
    td_u8 is_used;        /* 0: empty, 1: top, 2: bottom, 3: both fields (or frame)--output */
    td_u8 is_reference;   /* 0: not used for ref, 1: top used, 2: bottom used, */
    td_u8 mode_skip_flag; /* 0: all dec, 1: top not dec, 2: bottom not dec, 3: both fields (or frame) not dec */
    td_u8 is_in_dpb;      /* 0: already out from dpb, 1: still in dpb */

    td_s32 logic_fs_id;

    td_u32 frame_num;
    td_s32 frame_num_wrap;
    td_u32 long_term_frame_idx;
    td_s32 poc;

    td_u32 is_displayed; /* 0: not display; 1: displayed */
    td_u32 mbaff_frame_flag;
    td_u32 pic_type;
    td_u32 err_level;
    td_u32 pmv_address_idc;
    td_u32 apc_idc;
    td_s32 avg_qp;

    h264_store_pic frame;
    h264_store_pic top_field;
    h264_store_pic bottom_field;
    h264_fo_state fo_state; /* for first frame fast output */
} h264_frame_store;

typedef struct {
    td_u8 state;
    td_u8 field_pair_flag;
    td_u8 non_existing;  /* 0: pic exist, 1: pic not exist. for frame_num gap. */
    td_u8 structure;     /* 0: frame, 1: top_field, 2: bottom_field, 3: reserved */
    td_u8 is_long_term;  /* 0: not long term ref,  1: long term ref */
    td_u8 is_short_term; /* 0: not short term ref, 1: short term ref. BUT one pic cann't long and short. */
    td_u8 nal_unit_type;
    td_u8 mode_skip_flag;
    td_u8 entropy_coding_mode_flag;
    td_u8 mbaff_frame_flag;
    vfmw_image fs_image;

    h264_frame_store *frame_store;
    h264_mark *mark_para;

    td_u32 frame_num;
    td_u32 long_term_frame_idx;
    td_u32 nal_ref_idc;
    td_s32 thispoc;
    td_s32 framepoc;
    td_s32 toppoc;
    td_s32 bottompoc;

    td_u32 pic_type;
    td_u32 is_idr_pic;
    td_u32 err_level;
    td_u32 pic_width_in_mb;
    td_u32 pic_height_in_mb;
    td_u32 frame_height_in_mb;
    td_u32 pic_mbs;
    UADDR stream_base_addr;
    td_u32 pmv_address_idc;

    td_u32 frame_stream_size;
    td_u32 cur_nal_size;

    td_u8 pic_output_flag;
} h264_cur_pic;

typedef struct {
    td_u8 structure; /* pic struct, 0: frame, 1: top field, 2:bottom field, 3: rsv */
    td_u8 frame_store_flag;
    td_u32 nal_ref_idc;
    td_u32 frame_num;
    td_u32 pic_type;
    td_u32 pic_counter;
    td_u32 image_id;
    td_u32 dpb_pos;
} h264_prev_pic;

typedef struct {
    td_u32 is_used[16]; /* 16 :a number */
    td_u32 idc[16]; /* 16 :a number */
    td_u32 logic_id[16]; /* 16 :a number */
    td_u32 poc[2][16]; /* 2 :a number 16 :a number */
    td_u32 size;
    td_u32 used_size;
    td_u32 ref_idx[32]; /* 32 :a number */
} h264_apc;

typedef struct {
    h264_frame_store *fs[16]; /* 16 :a number */
    h264_frame_store *fs_ref[16]; /* 16 :a number */
    h264_frame_store *fs_ltref[16]; /* 16 :a number */
    td_u32 size;
    td_u32 used_size;
    td_u32 ref_frames_in_buffer;
    td_u32 ltref_frames_in_buffer;
    td_u32 max_long_term_pic_idx;
} h264_dpb;

typedef struct {
    h264_nalu *slice_nal;
} h264_slc_info;

typedef struct {
    td_u8 last_pack_in_nal;
    td_u8 pack_is_pic_over_flag;
    td_u8 *stream;
    UADDR strm_phy_addr;
    td_s32 stream_len;     /* delete 0x03 */
    td_s32 raw_stream_len; /* not delete 0x03 */
    td_s32 stream_id;
    td_s32 seg_cut_state;
} h264_stream_packet;

typedef enum {
    FIRST_DISP_INIT = 0,
    FIRST_DISP_UNPAIRED,
    FIRST_DISP_OUTPUT
} h264_first_disp_state;

typedef struct {
    td_u8 idr_flag;
    td_u8 priority_id;
    td_u8 no_inter_layer_pred_flag;
    td_u8 dependency_id;
    td_u8 quality_id;
    td_u8 temporal_id;
    td_u8 use_ref_base_pic_flag;
    td_u8 discardable_flag;
    td_u8 output_flag;
    td_u8 reserved_three_2bits;
} h264_nalu_svc_ext;

#pragma pack(1)
typedef struct {
    td_u8 version;
    td_u32 format_type : 2;
    td_u32 packing_type : 2;
    td_u32 reserve : 4;
    td_u32 width : 12;
    td_u32 height : 12;
} h264_unregistered_splice_sei_v1;
#pragma pack()

typedef struct {
    syn_cfg_info *extra_data;
    syn_cfg_info extra_cfg;
    td_u32 start_mode; /* 0:Normal Mode, 1: Flush Mode */
    td_u32 profile;
    td_u32 level;

    h264_nalu *cur_nal;
    h264_nalu nal_array[1];
    bs_info bs;
    td_u8 is_stream_end_flag;
    td_u8 pic_is_decoded; /* pic is decoded with dsp_check_pic_over_flag */
    td_s32 cur_width_in_mb;
    td_s32 cur_height_in_mb;
    td_s32 max_slice_num;
    td_s32 max_sps_num;
    td_s32 max_pps_num;
    td_s32 true_dpb_size;
    h264_sps sps_tmp;
    h264_sps *sps[H264_MAX_SPS_NUM];
    h264_pps *pps[H264_MAX_PPS_NUM];
    h264_pps pps_tmp;
    h264_sps cur_sps;
    h264_pps cur_pps;
    h264_poc cur_poc;
    h264_sei private_sei;
    h264_reorder cur_reorder;
    h264_slice cur_slice;
    h264_prev_slice prev_slice;
    h264_mark mark[2]; /* 2 :a number */
    td_u32 cur_mark_idx;
    h264_frame_packing_arrangement frame_pack_sei;
    h264_pic_timing_sei pic_timing_sei;
    vfmw_splice_info splice_info;
    h264_sei_pts_parser pts_parser;
    h264_dpb dpb;
    h264_apc apc;
    h264_store_pic *listx[2][MAX_LIST_SIZE]; /* 2 :a number */
    h264_frame_store frame_store[MAX_FRAME_STORE];
    h264_cur_pic cur_pic;
    h264_cur_pic last_pic;
    h264_prev_pic prev_pic;
    h264_frame_store *direct_out_buf;
    h264_frame_store *last_field_buf;
    h264_nalu_svc_ext nalu_svc_ext;

    td_u8 pmv_store_used_flag[MAX_PMV_STORE]; /* pmv store memory allocate statistic */
    td_u32 total_fs_num;                      /* change while g_DPB.size change */
    td_u32 total_pmv_num;                     /* change while g_DPB.size change */

    td_u32 allow_start_dec; /* 0: find recover point, when recover point found and decode right this flag =1; */
    td_u32 have_nal_not_decoded;
    td_u32 is_insert_eopic_nal;
    td_u32 idr_found; /* 0: to find I/IDR, 1: I/IDR found */
    td_u32 ppic_found;
    td_u32 parse_frame_rate;

    td_u32 total_usrdat_num;
    td_u32 slice_para_num;
    td_u32 old_dec_mode;
    td_u32 imode_out_flag;
    td_u32 mutex_hold;

    td_u32 se_err_flg;
    td_u32 total_nal;
    td_u32 total_sps;
    td_u32 total_pps;
    td_u32 total_slice;
    td_u32 seq_cnt;
    td_u32 total_pic_num;
    td_u32 total_frame_num;
    td_u32 total_field_num;
    td_u32 idr_pic_of_cur_pic;
    td_u32 slice_num_in_pic;
    td_u32 max_bytes_received;

    td_u32 advanced_cfg;
    td_u32 ref_error_thr;
    td_u32 out_error_thr;
    td_s32 chan_id;
    td_u32 seg_cut_state;

#if defined(VFMW_H264_SLC_LDY_SUPPORT)
    td_u32 ldy_cfg;
    td_u32 ldy_mode;
    td_u32 half_nal_en;
    td_u32 half_nal_try;
    td_u32 half_nal_succ;
    td_u32 try_slice_num;
    td_u32 slc_ldy_cnt;
    td_u32 post_pic_count;
#endif

    h264_first_disp_state first_frame_displayed;

    td_u32 prev_store_pic_counter;
    td_u32 pic_counter;
    td_u32 new_pic_counter;
    td_u32 pic_out_counter;
    td_s32 num_total_frame;
    td_s32 num_output_frame;
    td_u32 scd_wrap_around;
    h264_nalu *last_nal;

    td_u32 last_seg_id;
    td_u32 err_nal_cnt;
    td_u32 need_clear_que;
    td_u32 fake_frm_height_in_mb;
    td_u32 fake_frm_width_in_mb;
    td_u32 is_resolution_changed;
    syn_mem_info mem_list[H264_MEM_NUM];
    td_u32 ctx_mem_size;
    dps_info *cur_dps;
    td_s32 sps_update_flag;

    vfmw_usrdat *usr_dat_array[MAX_USERDATA_NUM];
    afd_info afd_info_data;
    afd_info cur_afd_info;
    td_u32 is_sec;
    td_u32 first_i_flag;
    td_u32 ref_pic_single_size;
    td_s32 ref_pic_single_fsp_id[H264_APC_SIZE];
    spec_invalid_type spec_invalid;
    td_u32 spec_value;
} h264_ctx;

typedef struct {
    td_u32 constrained_set0_flag;
    td_u32 constrained_set1_flag;
    td_u32 constrained_set2_flag;
    td_u32 constrained_set3_flag;
    td_u32 profile_idc;
    td_u32 level_idc;
} h264_dec_sps_info;

/*************************** func declare *************************/
td_s32 h264_init(td_s32 chan_id, td_void **ctx, const syn_cfg_info *extra_data);
td_s32 h264_deinit(td_void *ctx, td_void *extra_data);
td_s32 h264_config(td_void *ctx, const syn_cfg_info *extra_data);
td_s32 h264_reset(td_void *ctx, td_void *args);
syn_parse_state h264_decode(stm_seg_info *seg, dps_info *dps, td_void *arg_ctx);
td_u32 h264_get_ctx_size(const td_void *arg_ctx);
td_s32 h264_get_max_size(td_void);
td_void h264_read_log(const td_void *args, td_void *buf, td_s32 len, td_s32 *used);
td_s32 h264_control(td_void *ctx, td_s32 cmd_id, const td_void *args);
td_s32 h264_get_parse_info(const td_void *ctx_ext, td_void *args_ext);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __H264_H__ */
