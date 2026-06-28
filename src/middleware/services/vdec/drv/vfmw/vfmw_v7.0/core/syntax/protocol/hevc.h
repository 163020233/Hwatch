/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef _SYN_HEVC_HEADER_
#define _SYN_HEVC_HEADER_

#include "syn_base.h"
#include "syn_ctrl.h"
#include "stm_intf.h"
#include "syn_bs.h"
#include "msg_h265.h"
#include "stm_base.h"
#include "dps.h"
#include "syn_parse.h"
#include "usrdata.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HEVC_SYNTAX_OPT

#define HEVC_MAX_SUB_LAYER_NUM 6

#define HEVC_MAX_DPB_NUM       17
#define HEVC_STREAM_BASE_ALIGN 15
#define HEVC_GOP_LEN           100 /* the vfmw_max length of gop for wrapping around */

/* define the profiles */
#define H265_MAIN_PROFILE
#define HM11_SYNTAX    1
#define HM9_NALU_TYPES 1
#define HEVC_NOTDEC    (-2)

#define HEVC_STREAM_END_CODE1 0x48535049
#define HEVC_STREAM_END_CODE2 0x43454E44

/* profile related constant */
#if defined(H265_MAIN_PROFILE)
#define HEVC_MAX_VPS_NUM        16
#define HEVC_MAX_SPS_NUM        16
#define HEVC_MAX_PPS_NUM        64
#define HEVC_MAX_LONG_TERM_PICS 5
#define HEVC_MAX_DELTA_POC      5
#endif

#define HEVC_MEM_NUM (HEVC_MAX_VPS_NUM + HEVC_MAX_SPS_NUM + HEVC_MAX_PPS_NUM + 3)
#define BASE_PROFILE_MAX_BITSTREAM_SIZE 4096

#define HEVC_MAX_DPB_PIC_BUF 6
#define HEVC_MAX_ENTRY_POINT 100

#define HEVC_MAX_LSB_NUM 33
#if !defined(CFG_HEVC_MAX_TEMPLAYER)
#define HEVC_MAX_TEMPLAYER 8 /* vfmw_max number of temporal layer */
#else
#define HEVC_MAX_TEMPLAYER (CFG_HEVC_MAX_TEMPLAYER)
#endif
#define HEVC_MAX_CPB_CNT           32 /* Upper bound of (cpb_cnt_minus1 + 1) */
#define HEVC_MAX_NUM_PICS_IN_SOP   1024
#define HEVC_MAX_NESTING_NUM_LAYER 64
#define HEVC_MAX_CHROMA_FORMAT_IDC 3
#define HEVC_MAX_INT               2147483647  /* vfmw_max value of signed 32-bit integer */
#define HEVC_MAX_UINT              0xFFFFFFFFU /* vfmw_max. value of unsigned 32-bit integer */

#define HEVC_FALSE 0
#define HEVC_TRUE  1

#define HEVC_STREAM_END_PPS_ID 191

#ifdef VFMW_MV_HEVC_SUPPORT
#define HEVC_SCALE_TYPE_10X                    0
#define HEVC_SCALE_TYPE_15X                    1
#define HEVC_SCALE_TYPE_20X                    2
#define HEVC_SCALE_TYPE_30X                    3

#define HEVC_MAX_NUM_LAYERS                    2
#define HEVC_MAX_VPS_OUTPUTLAYER_SETS          (HEVC_MAX_VPS_OP_SETS_PLUS1 + 1)
#define HEVC_MAX_NUM_SCALED_REF_LAYERS         (HEVC_MAX_NUM_LAYERS - 1)
#define HEVC_EXT_MAX_REF_LOC_OFFSET            64
#define HEVC_MAX_NUM_SCALABILITY_TYPES         16
#define HEVC_MAX_PARTITION_NUM                 16
#define HEVC_MAX_SIZE_VIEW_QUEUE               16
#define HEVC_VPS_MAX_MAX_REP_FORMAT_MINUS_ONE  256
#else
#define HEVC_MAX_NUM_LAYERS                    1
#endif

#if HM11_SYNTAX
#if !defined(CFG_HEVC_MAX_VPS_OP_SETS_PLUS1)
#define HEVC_MAX_VPS_OP_SETS_PLUS1 1024
#else
#define HEVC_MAX_VPS_OP_SETS_PLUS1 (CFG_HEVC_MAX_VPS_OP_SETS_PLUS1)
#endif
#define HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1 2
#endif

#if !defined(CFG_HEVC_MAX_STRPS_NUM)
#define HEVC_MAX_STRPS_NUM 64
#else
#define HEVC_MAX_STRPS_NUM (CFG_HEVC_MAX_STRPS_NUM)
#endif
/* macros */
#if !defined(CFG_HEVC_MAX_CU_DEPTH)
#define HEVC_MAX_CU_DEPTH 7       /* log2(LCUSize) */
#else
#define HEVC_MAX_CU_DEPTH (CFG_HEVC_MAX_CU_DEPTH)
#endif
#define HEVC_MAX_CU_SIZE (1 << (HEVC_MAX_CU_DEPTH)) /* maximum allowable size of CU */
#define HEVC_MIN_PU_SIZE 4

#define HEVC_SCALING_LIST_NUM         6  /* < list number for quantization matrix */
#define HEVC_SCALING_LIST_DC          16 /* < default DC value */
#define HEVC_SCALING_LIST_START_VALUE 8  /* < start value for dpcm mode */
#define HEVC_MAX_MATRIX_COEF_NUM      64 /* < vfmw_max coefficient number for quantization matrix */
#define HEVC_MAX_LIST_SIZE            (16 + 1)
#define HEVC_MAX_FRAME_STORE          20 /* Maximun Frame Store Num */
#define HEVC_MAX_PMV_STORE            20 /* Maximun PMV   Store Num */

#define HEVC_SINT8_MAX_VAL  127
#define HEVC_SINT8_MIN_VAL  (-128)
#define HEVC_SINT13_MAX_VAL 4095
#define HEVC_SINT13_MIN_VAL (-4096)

#define HEVC_I_FRAME 0
#define HEVC_P_FRAME 1
#define HEVC_B_FRAME 2

/********************* error information define *******************/
#define HEVC_ERR_FS_OUT_AGAIN (-3)
#define HEVC_INIT_DECODER_ERR (-1)
#define HEVC_DEC_ERR          (-1)
#define HEVC_DEC_NORMAL       0
#define HEVC_OK               0
#define HEVC_PROFILE_ERR      (-1)
#define HEVC_LEVEL_ERR        (-1)
#define HEVC_VHB_MEM_ERR      (-5)
#define HEVC_DPB_ERR          (-3)
#define HEVC_FS_ALLOC_ERR     (-4)
#define HEVC_VHB_MEM_ERR      (-5)
#define HEVC_OUT_FLAG         (-6)
#define HEVC_APC_ERR          (-7)
#define HEVC_BS_OVERFLOW_ERR  (-8)
#define HEVC_DEC_OCCUPY       99
#define HEVC_VPS_ERR          (-1)
#define HEVC_SPS_ERR          (-1)
#define HEVC_SE_ERR           (-1)
#define HEVC_PPS_ERR          (-1)
#define HEVC_VLC_CODE_ERR     (-1)
#define HEVC_FLC_ERR          (-1)
#define HEVC_IDR_ERR          (-1)
#define HEVC_SLICE_HEADER_ERR (-1)
#define HEVC_MMCO_NOT_EQUAL   (-1)
#define HEVC_RECOVERPOINT_ERR (-1)
#define HEVC_REDUNDANTPIC_ERR (-1)
#define HEVC_PIC_INITIAL_ERR  (-1)
#define HEVC_INITDPB_ERR      (-1)
#define HEVC_OUTPUT_ERR       (-1)

#define HEVC_MEET_EOS     2
#define HEVC_PIC_DECODING 1
#define HEVC_PIC_EMPTY    0

#define MRG_MAX_NUM_CANDS 5

/* for image format clean bits */
#define HEVC_CLEAN_FRAME_TYPE      (~3)
#define HEVC_CLEAN_VIDEO_FORMAT    (~(7 << 5))
#define HEVC_CLEAN_SOURCE_FORMAT   (~(3 << 8))
#define HEVC_CLEAN_FIELD_VALID     (~(3 << 10))
#define HEVC_CLEAN_TOP_FILED_FIRST (~(3 << 12))
#define HEVC_CLEAN_ASPECT_RATIO    (~(7 << 14))

#define HEVC_GET_ONE_NALU_SIZE (4 * 1024)
#define HEVC_START_FRAME       0

#define HEVC_MAX_BYTES_START (512 * 1024) /* 512*1024 bytes received, start VDM to dec */

#define HEVC_MAX_SLOT_NUM         (HEVC_MAX_SLICE_NUM + 5)
#define HEVC_SLICE_SLOT_START_NUM 3

#pragma pack(1)
typedef struct {
    td_u8 version;
    td_u32 format_type : 2;
    td_u32 packing_type : 2;
    td_u32 reserve : 4;
    td_u32 width : 12;
    td_u32 height : 12;
} h265_unregistered_splice_sei_v1;
#pragma pack()

typedef struct {
    td_u8 general_tier_flag;
    td_u8 general_profile_compatibility_flag[32]; /* 32 :a number */
    td_u8 general_progressive_source_flag;
    td_u8 general_interlaced_source_flag;
    td_u8 general_non_packed_constraint_flag;
    td_u8 general_frame_only_constraint_flag;
    td_u8 sub_layer_profile_present_flag[6]; /* 6 :a number */
    td_u8 sub_layer_level_present_flag[HEVC_MAX_SUB_LAYER_NUM];
    td_u8 sub_layer_profile_compatibility_flag[6][32]; /* 6 :a number 32 :a number */
    td_u8 sub_layer_tier_flag[6]; /* 6 :a number */
    td_u8 sub_layer_progressive_source_flag[HEVC_MAX_SUB_LAYER_NUM];
    td_u8 sub_layer_interlaced_source_flag[6]; /* 6 :a number */
    td_u8 sub_layer_non_packed_constraint_flag[6]; /* 6 :a number */
    td_u8 sub_layer_frame_only_constraint_flag[6]; /* 6 :a number */
    td_s32 general_profile_idc;
    td_s32 general_profile_space;
    td_s32 general_reserved_zero_44bits;
    td_s32 general_level_idc;
    td_s32 sub_layer_profile_space[HEVC_MAX_SUB_LAYER_NUM];
    td_s32 sub_layer_profile_idc[6]; /* 6 :a number */
    td_s32 sub_layer_reserved_zero_44bits[6]; /* 6 :a number */
    td_s32 sub_layer_level_idc[6]; /* 6 :a number */
} hevc_profile_tier_level;

/* / coefficient scanning type used in ACS */
typedef enum {
    SCAN_ZIGZAG = 0, /* /< typical zigzag scan */
    SCAN_HOR,        /* /< horizontal first scan */
    SCAN_VER,        /* /< vertical first scan */
    SCAN_DIAG        /* /< up-right diagonal scan */
} hevc_coeff_scan_type;

/* / reference list index */
typedef enum {
    REF_PIC_LIST_0 = 0,  /* /< reference list 0 */
    REF_PIC_LIST_1 = 1,  /* /< reference list 1 */
    REF_PIC_LIST_C = 2,  /* /< combined reference list for uni-prediction in B-Slices */
    REF_PIC_LIST_X = 100 /* /< special mark */
} hevc_ref_pic_list;

typedef enum {
    SCALING_LIST_4X4 = 0,
    SCALING_LIST_8X8,
    SCALING_LIST_16X16,
    SCALING_LIST_32X32,
    SCALING_LIST_SIZE_NUM
} hevc_scaling_list_size;

typedef struct {
    td_u8 use_transform_skip; /* !< transform skipping flag for setting default scaling matrix for 4x4 */
    /* !< UseDefaultScalingMatrixFlag */
    td_u8 use_default_scaling_matrix_flag[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    /* !< flag for using default matrix */
    td_u8 scaling_list_pred_mode_flag[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    td_u8 scaling_list_coef[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM][HEVC_MAX_MATRIX_COEF_NUM];
    td_u8 scaling_list_pred_matrix_id_delta[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
    td_u8 ref_matrix_id[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];  /* !< ref_matrix_id */
    td_u8 pred_matrix_id[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM]; /* !< reference list index */
    td_s32 scaling_list_dc_coef_minus8;
    td_s32 scaling_list_delta_coef;
    /* !< the DC value of the matrix coefficient for 16x16 */
    td_s32 scaling_list_dc[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];
} hevc_scaling_list;

typedef struct {
    td_u32 inter_ref_pic_set_prediction_flag;
    td_u32 delta_idx;
    td_u32 delta_rps_sign;
    td_u32 abs_delta_rps;
    td_u32 num_negative_pics;
    td_u32 num_positive_pics;
    td_u32 num_of_pics;
    td_u32 num_ref_idc;
    td_u32 num_of_longterm_pics;
    td_u8 ref_idc[HEVC_MAX_NUM_REF_PICS + 1];
    td_u8 used_flag[HEVC_MAX_NUM_REF_PICS + 1];
    td_s32 delta_poc[HEVC_MAX_NUM_REF_PICS + 1];
    td_s32 poc[HEVC_MAX_NUM_REF_PICS];
} hevc_short_term_rpset;

typedef struct {
    td_u8 ref_pic_list_modification_flag_l0;
    td_u8 ref_pic_list_modification_flag_l1;
    td_u32 list_entry_l0[32]; /* 32 :a number */
    td_u32 list_entry_l1[32]; /* 32 :a number */
} hevc_ref_pic_lists_moddification;

typedef struct {
    td_u32 luma_log2_weight_denom;
    td_s32 delta_chroma_log2_weight_denom;

    td_s32 luma_weight_l0_flag[HEVC_MAX_NUM_REF_PICS];
    td_s32 chroma_weight_l0_flag[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_luma_weight_l0[HEVC_MAX_NUM_REF_PICS];
    td_s32 luma_offset_l0[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_chroma_weight_l0[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
    td_s32 delta_chroma_offset_l0[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */

    /* HEVC_B_SLICE */
    td_s32 luma_weight_l1_flag[HEVC_MAX_NUM_REF_PICS];
    td_s32 chroma_weight_l1_flag[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_luma_weight_l1[HEVC_MAX_NUM_REF_PICS];
    td_s32 luma_offset_l1[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_chroma_weight_l1[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
    td_s32 delta_chroma_offset_l1[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */

    /* inferred value */
    td_u32 chroma_log2_weight_denom;
    td_s32 luma_weight_l0[HEVC_MAX_NUM_REF_PICS];
    td_s32 chroma_weight_l0[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
    td_s32 chroma_offset_l0[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
    td_s32 luma_weight_l1[HEVC_MAX_NUM_REF_PICS];
    td_s32 chroma_weight_l1[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
    td_s32 chroma_offset_l1[HEVC_MAX_NUM_REF_PICS][2]; /* 2 :a number */
} hevc_pred_weight_table;

typedef struct {
    /* Explicit weighted prediction parameters parsed in slice header, */
    /* or Implicit weighted prediction parameters (8 bits depth values). */
    td_u8 present_flag;
    td_u32 log2_weight_denom;
    td_s32 implicit_weight;
    td_s32 implicit_offset;

    /* Weighted prediction scaling values built from above parameters (bitdepth scaled): */
    td_s32 w, o, offset, shift, round;
} hevc_wp_scaling_param;

typedef struct {
    td_u8 fixed_pic_rate_general_flag;
    td_u8 fixed_pic_rate_within_cvs_flag;
    td_u8 low_delay_hrd_flag;
    td_u32 elemental_duration_in_tc_minus1;
    td_u32 cpb_cnt_minus1;
    td_u32 bit_rate_value_minus1[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
    td_u32 cpb_size_value_minus1[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
    td_u32 cpb_size_du_value_minus1[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
    td_u32 bit_rate_du_value_minus1[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
    td_u32 cbr_flag[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
} hevc_hrd_sublayer_set;

typedef struct {
    td_u8 nal_hrd_parameters_present_flag;
    td_u8 vcl_hrd_parameters_present_flag;
    td_u8 sub_pic_hrd_params_present_flag;
    td_u8 sub_pic_cpb_params_in_pic_timing_sei_flag;
    td_u32 tick_divisor_minus2;
    td_u32 du_cpb_removal_delay_increment_length_minus1;
    td_u32 dpb_output_delay_du_length_minus1;
    td_u32 bit_rate_scale;
    td_u32 cpb_size_scale;
    td_u32 cpb_size_du_scale;
    td_u32 initial_cpb_removal_delay_length_minus1;
    td_u32 au_cpb_removal_delay_length_minus1;
    td_u32 dpb_output_delay_length_minus1;
    hevc_hrd_sublayer_set hrd_sublayer[HEVC_MAX_TEMPLAYER];
} hevc_hrd_param_set;

#ifdef VFMW_MV_HEVC_SUPPORT
typedef struct {
    td_u8 separate_colour_plane_vps_flag;
    td_u32 pic_width_vps_in_luma_samples;
    td_u32 pic_height_vps_in_luma_samples;
    td_u32 chroma_format_vps_idc;
    td_u32 bit_depth_vps_luma_minus8;
    td_u32 bit_depth_vps_chroma_minus8;
    td_u32 conf_win_vps_left_offset;
    td_u32 conf_win_vps_right_offset;
    td_u32 conf_win_vps_top_offset;
    td_u32 conf_win_vps_bottom_offset;
} hevc_rep_format_set;

typedef struct {
} hevc_vps_vui_bsp_hrdpara_set;

typedef struct {
    hevc_vps_vui_bsp_hrdpara_set vps_vui_bsp_hrdpara;
} hevc_vps_vui_set;

typedef struct {
    td_u8 head_reserved;
    td_u8 rep_format_equal_flag;
    td_u8 max_one_active_ref_layer_flag;
    td_u8 all_ref_layers_active_flag;
    td_u8 poc_lsb_not_present_flag[HEVC_MAX_NUM_LAYERS];
    td_u8 direct_dependency_flag[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_LAYERS];
    td_u8 output_layer_flag[HEVC_MAX_VPS_OUTPUTLAYER_SETS][HEVC_MAX_NUM_LAYERS];
    td_u8 necessary_layer_flag[HEVC_MAX_VPS_OUTPUTLAYER_SETS][HEVC_MAX_NUM_LAYERS];
    td_s32 num_layer_sets;
    td_s32 max_layers_minus1;
    td_s32 num_output_layer_sets;
    td_u32 vps_num_rep_formats_minus1;
    td_u32 view_id_val[HEVC_MAX_NUM_LAYERS];
    td_s32 layer_id_in_nuh[HEVC_MAX_NUM_LAYERS];
    td_s32 layer_idx_in_vps[HEVC_MAX_NUM_LAYERS];
    td_u32 vps_rep_format_idx[HEVC_MAX_NUM_LAYERS];
    td_s32 num_direct_ref_layers[HEVC_MAX_NUM_LAYERS];
    td_s32 sub_layers_vps_max_minus1[HEVC_MAX_NUM_LAYERS];
    td_s32 ref_layer_id[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_LAYERS];
    td_s32 layer_set_idx_for_ols_minus1[HEVC_MAX_VPS_OUTPUTLAYER_SETS];
    td_s32 max_sub_layers_in_layer_set_minus1[HEVC_MAX_VPS_OUTPUTLAYER_SETS];
    td_s32 max_tid_il_ref_pics_plus1[HEVC_MAX_NUM_LAYERS][HEVC_MAX_NUM_LAYERS];

    hevc_vps_vui_set vui;
    hevc_rep_format_set rep_format[HEVC_MAX_NUM_LAYERS];

    td_u8 tail_reserved;
} hevc_vps_extension_set;

typedef struct {
    td_u8 sps_infer_scaling_list_flag;
    td_u32 sps_scaling_list_ref_layer_id;
} hevc_sps_extension_set;


typedef struct {
    td_u8 head_reserved;

    td_u8 pps_infer_scaling_list_flag;
    td_u8 resample_phase_set_present_flag[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u32 pps_scaling_list_ref_layer_id;
    td_u8 scaled_ref_layer_offset_present_flag[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 scaled_ref_layer_left_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 scaled_ref_layer_top_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 scaled_ref_layer_right_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 scaled_ref_layer_bottom_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u8 ref_region_offset_present_flag[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 ref_region_left_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 ref_region_top_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 ref_region_right_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_s32 ref_region_bottom_offset[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u32 phase_hor_luma[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u32 phase_ver_luma[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u32 phase_hor_chroma_plus8[HEVC_EXT_MAX_REF_LOC_OFFSET];
    td_u32 phase_ver_chroma_plus8[HEVC_EXT_MAX_REF_LOC_OFFSET];

    td_u8 tail_reserved;
}hevc_pps_extension_set;

typedef struct {
    td_s32 poc;
    td_s32 logic_fs_id;
    vfmw_image *image;
} hevc_view_frame;

typedef struct {
    td_u16 head;
    td_u16 tail;
    hevc_view_frame frame[HEVC_MAX_SIZE_VIEW_QUEUE];
} hevc_view_queue;

typedef struct {
    td_u8 head_reserved;
    td_s32 num_active_ref_layer_pics;
    td_u32 inter_layer_pred_layer_idc[HEVC_MAX_NUM_LAYERS];
    td_u8 tail_reserved;
}hevc_slice_extension_set;
#endif

typedef struct {
    td_u8 valid;
    td_u8 vps_temporal_id_nesting_flag;
    td_u8 is_refresh;
    td_u8 vps_extension_flag;
    td_u8 vps_extension_data_flag;
    td_u8 vps_poc_proportional_to_timing_flag;
    td_u8 vps_timing_info_present_flag;
    td_u8 vps_sub_layer_ordering_info_present_flag;
    td_s32 video_parameter_set_id;
    td_s32 vps_reserved_three_2bits;
    td_s32 vps_max_layers_minus1;
    td_s32 vps_max_sub_layers;
    td_s32 vps_max_sub_layers_minus1;
    td_s32 vps_reserved_0xffff_16bits;
    td_s32 vps_max_layer_id;
    td_s32 vps_num_layer_sets_minus1;
    td_s32 vps_num_units_in_tick;
    td_s32 vps_time_scale;
    td_s32 vps_num_ticks_poc_diff_one_minus1;
    td_s32 vps_num_hrd_parameters;
    td_s32 vps_max_dec_pic_buffering[HEVC_MAX_TEMPLAYER];
    td_s32 vps_num_reorder_pics[HEVC_MAX_TEMPLAYER];
    td_s32 vps_max_latency_increase[HEVC_MAX_TEMPLAYER];
    td_s32 layer_id_included_flag[HEVC_MAX_VPS_OP_SETS_PLUS1][HEVC_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1];
    td_s32 hrd_layer_set_idx[HEVC_MAX_VPS_OP_SETS_PLUS1];
    td_s32 cprms_present_flag[HEVC_MAX_VPS_OP_SETS_PLUS1];

    hevc_profile_tier_level profile_tier_level;
    hevc_hrd_param_set hrd_parameters;
#ifdef VFMW_MV_HEVC_SUPPORT
    td_u8 vps_base_layer_internal_flag;
    td_u32 layer_set_layer_id_list[HEVC_MAX_VPS_OUTPUTLAYER_SETS][HEVC_MAX_NUM_LAYERS];
    td_u32 num_layers_in_id_list[HEVC_MAX_VPS_OUTPUTLAYER_SETS];
    hevc_vps_extension_set vps_extension;
#endif
} hevc_video_param_set;

typedef struct {
    td_u8 aspect_ratio_info_present_flag;
    td_u8 overscan_info_present_flag;
    td_u8 overscan_appropriate_flag;
    td_u8 video_signal_type_present_flag;
    td_u8 video_full_range_flag;
    td_u8 colour_description_present_flag;
    td_u8 chroma_loc_info_present_flag;
    td_u8 neutral_chroma_indication_flag;
    td_u8 field_seq_flag;
    td_u8 frame_field_info_present_flag;
    td_u8 default_display_window_flag;
    td_u8 vui_timing_info_present_flag;
    td_u8 vui_poc_proportional_to_timing_flag;
    td_u8 vui_hrd_parameters_present_flag;
    td_u8 bitstream_restriction_flag;
    td_u8 tiles_fixed_structure_flag;
    td_u8 motion_vectors_over_pic_boundaries_flag;
    td_u8 restricted_ref_pic_lists_flag;
    td_u32 aspect_ratio;
    td_u32 aspect_ratio_idc;
    td_u32 sar_width;
    td_u32 sar_height;
    td_u32 video_format;
    td_u32 colour_primaries;
    td_u32 transfer_characteristics;
    td_u32 matrix_coeffs;
    td_u32 chroma_sample_loc_type_top_field;
    td_u32 chroma_sample_loc_type_bottom_field;
    td_u32 def_disp_win_left_offset;
    td_u32 def_disp_win_right_offset;
    td_u32 def_disp_win_top_offset;
    td_u32 def_disp_win_bottom_offset;
    td_u32 vui_num_units_in_tick;
    td_u32 vui_time_scale;
    td_u32 vui_num_ticks_poc_diff_one_minus1;
    td_u32 min_spatial_segmentation_idc;
    td_u32 max_bytes_per_pic_denom;
    td_u32 max_bits_per_min_cu_denom;
    td_u32 log2_max_mv_length_horizontal;
    td_u32 log2_max_mv_length_vertical;

    hevc_hrd_param_set hrd_parameters;
} hevc_vui_param_set;

typedef struct {
    td_u32 air_version;
    td_u32 air_slice_num;
    td_u32 air_layer_num;
    td_u32 air_big_width;
    td_u32 air_big_height;
    td_u32 air_integrality;
} hevc_air_sps_param_set;

typedef struct {
    td_u8 is_refresh;
    td_u8 valid;
    td_u8 sps_temporal_id_nesting_flag;
    td_u8 sps_sub_layer_ordering_info_present_flag;
    td_u8 restricted_ref_pic_lists_flag;
    td_u8 sps_scaling_list_data_present_flag;
    td_u8 long_term_ref_pics_present_flag;
    td_u8 sps_temporal_mvp_enable_flag;
    td_u8 vui_parameters_present_flag;
    td_u8 sps_extension_flag;
    td_u8 sps_extension_data_flag;
    td_u8 used_by_curr_pic_lt_sps_flag[HEVC_MAX_LSB_NUM];

    td_u32 video_parameter_set_id;
    td_u32 sps_max_sub_layers_minus1;
    td_u32 seq_parameter_set_id;
    td_u32 reorder_num;
    /* Start copy for dps, do not change order */
    td_u32 scaling_list_enabled_flag;
    td_u32 amp_enabled_flag;
    td_u32 sample_adaptive_offset_enabled_flag;
    td_u32 pcm_loop_filter_disable_flag;
    td_u32 sps_strong_intra_smoothing_enable_flag;
    td_u32 chroma_format_idc;
    td_u32 pic_width_in_luma_samples;
    td_u32 pic_height_in_luma_samples;
    td_u32 bit_depth_luma;
    td_u32 qp_bdoffset_y;
    td_u32 bit_depth_chroma;
    td_u32 qp_bdoffset_c;
    td_u32 pcm_enabled_flag;
    td_u32 pcm_bit_depth_luma;
    td_u32 pcm_bit_depth_chroma;
    td_u32 log2_min_cb_sizey;
    td_u32 log2_ctb_sizey;
    td_u32 quadtree_tu_log2_min_size;
    td_u32 quadtree_tu_log2_max_size;
    td_u32 pcm_log2_min_size;
    td_u32 pcm_log2_max_size;
    td_u32 max_transform_hierarchy_depth_inter;
    td_u32 max_transform_hierarchy_depth_intra;
    td_u32 max_cu_depth;
    td_u32 ctb_num_width;  /* PicWidthInCtbsY */
    td_u32 ctb_num_height; /* PicHeightInCtbsY */
    td_u32 for_copy_end;
    /* End copy for dps */
    td_u32 separate_colour_plane_flag;
    td_u32 conformance_window_flag;
    td_u32 conf_win_left_offset;
    td_u32 conf_win_right_offset;
    td_u32 conf_win_top_offset;
    td_u32 conf_win_bottom_offset;

    td_u32 max_pic_order_cnt_lsb;
    td_u32 bits_for_poc;
    td_u32 log2_min_luma_coding_block_size_minus3;
    td_u32 log2_diff_max_min_luma_coding_block_size;
    td_u32 min_cb_sizey;
    td_u32 ctb_sizey;
    td_u32 max_cu_width;
    td_u32 max_cu_height;
    td_u32 log2_min_transform_block_size_minus2;
    td_u32 log2_diff_max_min_transform_block_size;
    td_u32 log2_min_pcm_coding_block_size_minus3;
    td_u32 log2_diff_max_min_pcm_coding_block_size;
    td_u32 quadtree_tu_max_depth_inter;
    td_u32 quadtree_tu_max_depth_intra;
    td_u32 min_tr_depth;
    td_u32 max_tr_depth;
    td_u32 num_short_term_ref_pic_sets;
    td_u32 num_long_term_ref_pic_sps;
    td_u32 dpb_size;
    td_u32 disp_width;
    td_u32 disp_height;

    td_u32 lt_ref_pic_poc_lsb_sps[HEVC_MAX_LSB_NUM];
    td_u32 max_dec_pic_buffering[HEVC_MAX_TEMPLAYER];
    td_u32 num_reorder_pics[HEVC_MAX_TEMPLAYER];
    td_u32 max_latency_increase[HEVC_MAX_TEMPLAYER];

    hevc_profile_tier_level profile_tier_level;
    hevc_scaling_list scaling_list;
    hevc_vui_param_set vui_parameters;
    hevc_short_term_rpset short_term_ref_pic_set[HEVC_MAX_STRPS_NUM];
#ifdef VFMW_MV_HEVC_SUPPORT
    td_u8 sps_ext_or_max_sub_layers_minus1;
    hevc_sps_extension_set sps_extension;
#endif
    hevc_air_sps_param_set air_parameters;
} hevc_seq_param_set;

typedef struct {
    td_u32 bp_num;
    td_u32 sei_timing_num;
    td_u32 sei_timing_num_sub;
    td_u32 initial_cpb_removal_delay_1st;
    td_u32 initial_cpb_removal_delay_last;
    td_u32 last_gop_size;
    td_u32 cpb_removal_delay;
    td_u32 dpb_output_delay;
    td_u64 dpb_output_time;
    td_u64 frame_output_time;
    td_u32 vui_num_units_in_tick;
    td_u32 vui_time_scale;
    td_s64 last_idr_pts;
    td_u64 last_idr_frame_output_time;
    td_u64 last_frame_output_time;
    /* that of pic */
    td_u8 idr_pic_found;
    td_u8 new_bp_start;
} hevc_sei_pts_parser;

typedef enum {
    BUFFERING_PERIOD = 0,
    PICTURE_TIMING = 1,
    PAN_SCAN_RECT = 2,
    FILLER_PAYLOAD = 3,
    USER_DATA_REGISTERED_ITU_T_T35 = 4,
    USER_DATA_UNREGISTERED = 5,
    RECOVERY_POINT = 6,
    SCENE_INFO = 9,
    FULL_FRAME_SNAPSHOT = 15,
    PROGRESSIVE_REFINEMENT_SEGMENT_START = 16,
    PROGRESSIVE_REFINEMENT_SEGMENT_END = 17,
    FILM_GRAIN_CHARACTERISTICS = 19,
    POST_FILTER_HINT = 22,
    TONE_MAPPING_INFO = 23,
    FRAME_PACKING = 45,
    DISPLAY_ORIENTATION = 47,
    SOP_DESCRIPTION = 128,
    ACTIVE_PARAMETER_SETS = 129,
    DECODING_UNIT_INFO = 130,
    TEMPORAL_LEVEL0_INDEX = 131,
    DECODED_PICTURE_HASH = 132,
    SCALABLE_NESTING = 133,
    REGION_REFRESH_INFO = 134,
} hevc_sei_payload_type;

typedef struct {
    td_u32 initial_cpb_removal_delay;
    td_u32 initial_cpb_removal_offset;
    td_u32 initial_alt_cpb_removal_delay;
    td_u32 initial_alt_cpb_removal_offset;
} hevc_sei_buf_period_initial;

typedef struct {
    td_u8 irap_cpb_params_present_flag;
    td_u8 concatenation_flag;
    td_u32 bp_seq_parameter_set_id;
    td_u32 cpb_delay_offset;
    td_u32 dpb_delay_offset;
    td_u32 au_cpb_removal_delay_delta_minus1;
    hevc_sei_buf_period_initial nal_vcl_inf[HEVC_MAX_CPB_CNT][2]; /* 2 :a number */
    /* derived values */
    td_s32 init_cpb_removal_delay;
    td_s32 init_cpb_removal_delay_offset;
    td_s32 derived_cpb_delay_offset;
    td_s32 derived_dpb_delay_offset;
} hevc_sei_buf_period;

typedef struct {
    td_u8 duplicate_flag;
    td_u8 du_common_cpb_removal_delay_flag;
    td_u32 pic_struct;
    td_u32 source_scan_type;
    td_u32 au_cpb_removal_delay_minus1;
    td_u32 pic_dpb_output_delay;
    td_u32 pic_dpb_output_du_delay;
    td_u32 num_decoding_units_minus1;
    td_u32 du_common_cpb_removal_delay_increment_minus1;
    td_u32 num_nalus_in_du_minus1[256]; /* 256 :a number */
    td_u32 du_cpb_removal_delay_increment_minus1[256]; /* 256 :a number */

    td_s64 removal_tm_of_first_pic_in_bp;
    td_s64 removal_tm_of_prev_non_discardable_pic;
    td_s64 removal_tm_of_prev_picture;
    td_s64 au_removal_tm;
} hevc_sei_pic_timming;

typedef struct {
    td_u8 itu_t_t35_country_code;
    td_u16 terminal_provider_code;
    td_u8 terminal_provider_oriented_code_message_idc;
} hevc_sei_user_data_reg_itu_t_t35;

typedef struct {
    td_u8 is_avalible;
    td_u8 ts_103_433_spec_version;
    td_u8 ts_103_433_payload_mode;
} hevc_sei_tsi;

typedef struct {
    td_u8 is_avalible;
    td_u32 cv_rec_id;
    td_u8 cv_rec_cancel_flag;
    td_u8 cv_rec_persistence_flag;
    td_u8 cv_rec_target_info_flag;
    td_u8 cv_rec_primaries;
    td_u8 cv_rec_matrix_coefficients;
    td_u8 tone_mapping_input_signal_black_level_offset;
    td_u8 tone_mapping_input_signal_white_level_offset;
    td_u8 shadow_gain_control;
    td_u8 highlight_gain_control;
    td_u8 mid_tone_width_adjustment_factor;
    td_u8 tone_mapping_output_fine_tuning_num_val; /* shall be in the range of 0 to 10 */
    td_u8 tone_mapping_output_fine_tuning_x[15]; /* 15 :a number */
    td_u8 tone_mapping_output_fine_tuning_y[15]; /* 15 :a number */
    td_u8 saturation_gain_num_val; /* shall be in the range of 0 to 6 */
    td_u8 saturation_gain_x[15]; /* 15 :a number */
    td_u8 saturation_gain_y[15]; /* 15 :a number */
} hevc_sei_color_volume_reconstruction_info;

typedef struct {
    td_u8 is_avalible;
    td_u32 colour_remap_id;
    td_u8 colour_remap_cancel_flag;
    td_u8 colour_remap_persistence_flag;
    td_u8 colour_remap_video_signal_info_present_flag;
    td_u8 colour_remap_full_range_flag;
    td_u8 colour_remap_primaries;
    td_u8 colour_remap_transfer_function;
    td_u8 colour_remap_matrix_coefficients;
    td_u8 colour_remap_input_bit_depth;
    td_u8 colour_remap_bit_depth;
    td_u8 pre_lut_num_val_minus1[3]; /* 3 :a number */
    td_u16 pre_lut_coded_value[3][33]; /* 3 :a number 33 :a number */
    td_u16 pre_lut_target_value[3][33]; /* 3 :a number 33 :a number */
    td_u8 colour_remap_matrix_present_flag;
    td_u8 log2_matrix_denom;
    td_s32 colour_remap_coeffs[3][3]; /* 3 :a number 3 :a number */
    td_u8 post_lut_num_val_minus1[3]; /* 3 :a number */
    td_u16 post_lut_coded_value[3][33]; /* 3 :a number 33 :a number */
    td_u16 post_lut_target_value[3][33]; /* 3 :a number 33 :a number */
} hevc_sei_color_remapping_info;

typedef struct {
    td_u8 *user_data_payload_byte;
    td_u8 uuid_iso_iec_11578[16]; /* 16 :a number */
    td_u32 user_data_length;
} hevc_sei_user_data_unreg;

typedef struct {
    td_u8 self_contained_cvs_flag;
    td_u8 no_parameter_set_update_flag;
    td_s32 active_video_parameter_set_id;
    td_s32 num_sps_ids_minus1;
    td_s32 active_seq_parameter_set_id[16]; /* 16 :a number */
} hevc_sei_active_parameter_set;

typedef struct {
    td_u8 dpb_output_du_delay_present_flag;
    td_s32 decoding_unit_idx;
    td_s32 du_spt_cpb_removal_delay_increment;
    td_s32 pic_spt_dpb_output_du_delay;
} hevc_sei_decoding_unit_info;

typedef struct {
    td_u8 exact_match_flag;
    td_u8 broken_link_flag;
    td_s32 recovery_poc_cnt;
} hevc_sei_recovery_point;

typedef struct {
    td_u8 frame_packing_arrangement_cancel_flag;
    td_u8 quincunx_sampling_flag;
    td_u8 spatial_flipping_flag;
    td_u8 frame0_flipped_flag;
    td_u8 field_views_flag;
    td_u8 current_frame_is_frame0_flag;
    td_u8 frame0_self_contained_flag;
    td_u8 frame1_self_contained_flag;
    td_u8 frame_packing_arrangement_persistence_flag;
    td_u8 upsampled_aspect_ratio_flag;
    td_u32 frame_packing_arrangement_id;
    td_u32 frame_packing_arrangement_type;
    td_u32 content_interpretation_type;
    td_u32 frame0_grid_position_x;
    td_u32 frame0_grid_position_y;
    td_u32 frame1_grid_position_x;
    td_u32 frame1_grid_position_y;
    td_u32 frame_packing_arrangement_reserved_byte;
} hevc_sei_frame_packing_arrangement;

typedef struct {
    td_u8 hor_flip;
    td_u8 ver_flip;
    td_u8 display_orientation_cancel_flag;
    td_u8 display_orientation_persistence_flag;
    td_u32 anticlockwise_rotation;
} hevc_sei_display_orientation;

typedef struct {
    td_u32 temporal_sub_layer_zero_idx;
    td_u32 irap_pic_id;
} hevc_sei_temporal_sub_layer;

typedef struct {
    td_u8 refreshed_region_flag;
} hevc_sei_region_refresh_info;

typedef struct {
    td_u8 is_avalible;
    td_u32 display_primaries_x[3]; /* 3 :a number */
    td_u32 display_primaries_y[3]; /* 3 :a number */
    td_u32 white_point_x;
    td_u32 white_point_y;
    td_u32 max_display_mastering_luminance;
    td_u32 min_display_mastering_luminance;
} hevc_sei_mastering_display_colour_volume;

typedef struct {
    td_u8 is_avalible;
    td_u16 max_content_light_level;
    td_u16 max_pic_average_light_level;
} hevc_sei_content_light_level_info;

typedef struct {
    td_u8 hdr_transfer_characteristic_idc;
} hevc_sei_hlg_compatible_info;

typedef struct {
    td_u8 tone_map_cancel_flag;
    td_u8 tone_map_persistence_flag;
    td_u8 exposure_compensation_value_sign_flag;
    td_u32 tone_map_id;
    td_u32 coded_data_bit_depth;
    td_u32 target_bit_depth;
    td_u32 tone_map_model_id;
    td_u32 min_value;
    td_u32 max_value;
    td_u32 sigmoid_midpoint;
    td_u32 sigmoid_width;
    td_u32 camera_iso_speed_idc;
    td_u32 camera_iso_speed_value;
    td_u32 exposure_index_idc;
    td_u32 exposure_index_value;
    td_u32 exposure_compensation_value_numerator;
    td_u32 exposure_compensation_value_denom_idc;
    td_u32 ref_screen_luminance_white;
    td_u32 extended_range_white_level;
    td_u32 nominal_black_level_code_value;
    td_u32 nominal_white_level_code_value;
    td_u32 extended_white_level_code_value;
    td_u32 num_pivots;
    td_u32 coded_pivot_value[256]; /* 256 :a number */
    td_u32 target_pivot_value[256]; /* 256 :a number */
    td_u32 start_of_coded_interval[257]; /* 257 :a number */
} hevc_sei_tone_mapping_info;

typedef struct {
    td_u32 sop_seq_parameter_set_id;
    td_u32 num_entries_in_sop_minus1;
    td_u32 sop_vcl_nut[HEVC_MAX_NUM_PICS_IN_SOP];
    td_u32 sop_temporal_id[HEVC_MAX_NUM_PICS_IN_SOP];
    td_u32 sop_short_term_rps_idx[HEVC_MAX_NUM_PICS_IN_SOP];
    td_s32 sop_poc_delta[HEVC_MAX_NUM_PICS_IN_SOP];
} hevc_sei_sop_info;

typedef enum {
    MD5,
    CRC,
    CHECKSUM,
    RESERVED,
} hevc_sei_method;

typedef struct {
    td_u8 digest[3][16]; /* 3 :a number 16 :a number */
    td_u32 hash_type;
    hevc_sei_method method;
} hevc_sei_decoded_picture_hash;

typedef struct {
    td_u8 bitstream_subset_flag;
    td_u8 nesting_op_flag;
    td_u8 default_op_flag;
    td_u8 all_layers_flag;
    td_u8 caller_owns_seis;
    td_u32 nesting_num_ops_minus1;
    td_u32 nesting_max_temporal_id_plus1[HEVC_MAX_NESTING_NUM_LAYER]; /* 20141028 */
    td_u32 nesting_op_idx[HEVC_MAX_NESTING_NUM_LAYER];
    td_u32 nesting_no_op_max_temporal_id_plus1;
    td_u32 nesting_num_layers_minus1;
    td_u32 nesting_layer_id[HEVC_MAX_NESTING_NUM_LAYER];
} hevc_sei_scalable_nesting;

typedef struct {
    td_bool is_cuva;
    vfmw_cuva_metadata cuva_metadata;
} hevc_sei_cuva_hdr_info;

typedef struct {
    td_u32 last_payload_type_byte;
    td_u32 last_payload_size_byte;
    td_u32 reserved_payload_extension_data;
    td_u32 payload_bit_equal_to_one;
    td_u32 payload_bit_equal_to_zero;
    hevc_sei_payload_type payloadtype;
    hevc_sei_buf_period buf_period;
    hevc_sei_pic_timming pic_timming;
    hevc_sei_user_data_reg_itu_t_t35 itu_t35;
    hevc_sei_tsi tsi;
    hevc_sei_color_volume_reconstruction_info color_volume_rec_info;
    hevc_sei_color_remapping_info color_remapping_info;
    hevc_sei_user_data_unreg user_data_unreg;
    hevc_sei_active_parameter_set active_param_sets;
    hevc_sei_decoding_unit_info decoding_unit_info;
    hevc_sei_recovery_point recovery_point;
    hevc_sei_frame_packing_arrangement frame_packing;
    hevc_sei_display_orientation display_orientation;
    hevc_sei_temporal_sub_layer temporal_sub_layer;
    hevc_sei_region_refresh_info region_refresh_info;
    hevc_sei_mastering_display_colour_volume mastering_display_colour_volume;
    hevc_sei_content_light_level_info content_light_level;
    hevc_sei_hlg_compatible_info hdr_compatible_info;
    hevc_sei_tone_mapping_info tone_mapping_info;
    hevc_sei_sop_info sop_info;
    hevc_sei_scalable_nesting scalable_nesting;
    hevc_sei_decoded_picture_hash decoded_picture_hash;
    hevc_sei_cuva_hdr_info cuva_info;
} hevc_sei_param_set;

typedef struct {
    td_u32 air_slice_id;
    td_u32 air_layer_id;
} hevc_air_slice_param_set;

typedef struct {
    td_u8 valid;
    td_u8 is_refresh;
    td_u8 dependent_slice_segments_enabled_flag;
    td_u8 output_flag_present_flag;
    td_u8 cabac_init_present_flag;
    td_u8 pic_slice_chroma_qp_offsets_present_flag;
    td_u8 loop_filter_across_slices_enabled_flag;
    td_u8 deblocking_filter_control_present_flag;
    td_u8 deblocking_filter_override_enabled_flag;
    td_u8 pic_disable_deblocking_filter_flag;
    td_u8 pic_scaling_list_data_present_flag;
    td_u8 lists_modification_present_flag;
    td_u8 slice_segment_header_extension_present_flag;
    td_u8 pps_extension_flag;
    td_u8 pps_extension_data_flag;
    /* Start copy for dps, do not change order */
    td_u32 sign_data_hiding_flag;
    td_u32 entropy_coding_sync_enabled_flag;
    td_u32 uniform_spacing_flag;
    td_u32 constrained_intra_pred_flag;
    td_u32 transform_skip_enabled_flag;
    td_u32 cu_qp_delta_enabled_flag;
    td_u32 weighted_pred_flag;
    td_u32 weighted_bipred_flag;
    td_u32 transquant_bypass_enable_flag;
    td_u32 tiles_enabled_flag;
    td_u32 loop_filter_across_tiles_enabled_flag;
    td_s32 diff_cu_qp_delta_depth;
    td_s32 pic_cb_qp_offset;
    td_s32 pic_cr_qp_offset;
    td_s32 num_tile_columns;
    td_s32 num_tile_rows;
    td_u32 log2_parallel_merge_level;
    td_s16 column_bd[HEVC_MAX_TILE_COLUMNS];
    td_s16 row_bd[HEVC_MAX_TILE_ROWS];
    td_u32 for_copy_end;
    /* End copy */
    td_s32 pic_parameter_set_id;
    td_s32 seq_parameter_set_id;
    td_u32 num_extra_slice_header_bits;
    td_s32 num_ref_idx_l0_default_active;
    td_s32 num_ref_idx_l1_default_active;
    td_s32 pic_init_qp;
    td_s32 max_cu_qp_delta_depth;
    td_s32 pps_beta_offset_div2;
    td_s32 pps_tc_offset_div2;
    td_s16 column_width[HEVC_MAX_TILE_COLUMNS];
    td_s16 row_height[HEVC_MAX_TILE_ROWS];

    hevc_scaling_list scaling_list;
#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_pps_extension_set pps_extension;
#endif
} hevc_pic_param_set;

typedef enum {
    NOT_NEW_PIC = 0,
    IS_NEW_PIC,
    IS_SKIP_PIC,
} hevc_pic_type;

typedef struct {
    td_u8 pic_output_flag;
    td_u8 first_slice_segment_in_pic_flag;
    td_u8 no_output_of_prior_pics_flag;
    td_u8 dependent_slice_segment_flag;
    td_u8 short_term_ref_pic_set_sps_flag;
    td_u8 num_ref_idx_active_override_flag;
    td_u8 deblocking_filter_override_flag;
    td_u8 used_by_curr_pic_lt_flag[HEVC_MAX_NUM_REF_PICS];
    td_u8 delta_poc_msb_present_flag[HEVC_MAX_NUM_REF_PICS];
    /* Start copy for dps */
    td_u32 slice_sao_luma_flag;
    td_u32 slice_sao_chroma_flag;
    td_u32 slice_temporal_mvp_enable_flag;
    td_u32 mvd_l1_zero_flag;
    td_u32 cabac_init_flag;
    td_u32 collocated_from_l0_flag;
    td_u32 slice_disable_deblocking_filter_flag;
    td_u32 slice_loop_filter_across_slices_enabled_flag;
    td_s32 slice_qp;
    td_s32 slice_type;
    td_s32 slice_segment_address;
    td_s32 max_num_merge_cand;
    td_s32 slice_cb_qp_offset;
    td_s32 slice_cr_qp_offset;
    td_s32 slice_beta_offset_div2;
    td_s32 slice_tc_offset_div2;
    td_u32 list_x_size[2]; /* 2 :a number */
    td_u32 for_copy_end;
    /* End copy for dps */
    td_s32 poc;
    td_s32 prev_poc;
    td_u32 view_id;
    td_u32 num_ref_idx[3]; /* 3 :a number */
    td_s32 offset_len;
    td_u32 nuh_temporal_id;
    td_u32 nal_unit_type;
    td_s32 pic_parameter_set_id;
    td_s32 colour_plane_id;
    td_s32 pic_order_cnt_lsb;
    td_s32 short_term_ref_pic_set_idx;
    td_u32 num_long_term_sps;
    td_u32 num_long_term_pics;
    td_u32 num_ref_idx_l0_active;
    td_u32 num_ref_idx_l1_active;
    td_s32 collocated_ref_idx;
    td_s32 slice_qp_delta;
    td_u32 num_entry_point_offsets;
    td_u32 slice_segment_header_extension_length;
    td_u32 slice_segment_header_extension_data_byte;
    td_u32 dependent_slice_curstart_cuaddr;
    td_u32 dependent_slice_curend_cuaddr;
    td_s32 entry_point_offset[1];
    td_s32 lt_idx_sps[HEVC_MAX_NUM_REF_PICS];
    td_s32 poc_lsb_lt[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_poc_msb_cycle_lt[HEVC_MAX_NUM_REF_PICS];
    td_s32 poclsb_lt[HEVC_MAX_NUM_REF_PICS];
    td_s32 delta_poc_msbc_ycle_lt[HEVC_MAX_NUM_REF_PICS];
    td_s32 check_lt_msb[HEVC_MAX_NUM_REF_PICS];

    hevc_pic_type new_pic_type;
    hevc_pred_weight_table pred_weight_table;
    hevc_short_term_rpset short_term_ref_pic_set;
    hevc_ref_pic_lists_moddification ref_pic_lists_modification;
#ifdef VFMW_MV_HEVC_SUPPORT
    hevc_slice_extension_set slice_extension;
#endif
    hevc_air_slice_param_set air_slice_param;
} hevc_slice_segment_header;

typedef struct {
    td_s32 poc;
    td_u32 err_level;
    td_u32 pic_type;
    td_u32 structure;     /* 0: frame, 1: top_field, 2: bottom_field, 3: field pair */
    td_u32 is_long_term;  /* 0: not long term ref,  1: long term ref */
    td_u32 is_short_term; /* 0: not short term ref, 1: short term ref. BUT one pic cann't long and short. */
    td_s32 long_term_pic_num;
    td_s32 pic_num;
    td_s32 avg_qp;
#ifdef VFMW_MV_HEVC_SUPPORT
    td_s32 is_inter_layer;
    hevc_slice_extension_set slice_extension;
#endif
    struct hevc_frame_store *frame_store;
} hevc_storable_pic;

typedef enum {
    FIRST_FRAME_OUT_INIT = 0,
    FIRST_FRAME_WAIT_OUT,
    FIRST_FRAME_ALREADY_OUT,
} hevc_fo_state;

typedef enum {
    FS_NOT_USED = 0,
    FS_IN_DPB,
    FS_DISPLAY_ONLY,
} hevc_fs_state;

typedef struct hevc_frame_store {
    td_u8 non_existing;              /* 0: real frame, 1: fake frame from creat lost ref pic */
    td_u8 is_reference;              /* 0: not ref,    1: for ref */
    td_u8 is_displayed;              /* 0: not insert, 1: already insert in vo queue */
    hevc_fs_state frame_store_state; /* frame store state */
    hevc_fo_state first_frame_state; /* first frame fast out state */
    td_s32 poc;
    td_u32 pic_type;
    td_u32 err_level;
    td_u32 pmv_address_idc;
    td_u32 apc_idc;
    td_s32 logic_fs_id;
    td_s32 view_id;
    hevc_storable_pic frame;
#ifdef VFMW_MV_HEVC_SUPPORT
    td_s8 inter_layer_ref[HEVC_APC_SIZE];
    vfmw_image fs_img;
#endif
    td_u32 sei_field_flag;
    td_u32 sei_source_scan_type;
} hevc_frame_store;

typedef struct {
    td_u8 pic_output_flag;
    td_u8 create_lost_pic;
    td_u8 is_ref_idc;
    td_u8 state;
    td_u8 is_in_dpb;    // 默认为false，表示没有插入到dpb，用于多slice低延迟模式下的第一组slice输出
    td_u32 nal_unit_type;
    td_s32 thispoc;
    td_u32 pic_type;
    td_u32 pic_width;
    td_u32 pic_height;
    td_u32 pic_ctbs;
    td_u32 start_count;
    UADDR stream_base_addr;
    td_u32 pmv_address_idc;
    td_u32 err_level;
    td_u32 sei_field_flag;
    td_u32 sei_source_scan_type;
    td_s32 pic_qp;
    td_u32 qp_slice_num; /* slice num with qp in pic */

    td_u32 frame_stream_size;
    td_u32 current_nal_size;
    td_u32 total_valid_bit_len;

    td_u32 view_id;
    hevc_frame_store *frame_store;
    vfmw_image fs_image;
    td_u32 air_slice_id;
    td_u32 air_layer_id;
    td_u32 big_frame_eop;
} hevc_curr_pic;

typedef struct {
    td_u32 size;
    td_u32 used_size;
    td_u32 is_used[HEVC_APC_SIZE];
    td_u32 idc[HEVC_APC_SIZE];
    td_s32 poc[HEVC_APC_SIZE];
#ifdef VFMW_MV_HEVC_SUPPORT
    td_s32 view_id[HEVC_APC_SIZE];
    td_s8 inter_layer_ref[HEVC_APC_SIZE];
#endif
} hevc_apc;

typedef struct {
    td_u32 size;
    td_u32 used_size;
    td_u32 max_long_term_pic_idx;
    td_u32 ltref_frames_in_buffer;
    td_u32 negative_ref_frames_in_buffer;
    td_u32 positive_ref_frames_in_buffer;
#ifdef VFMW_MV_HEVC_SUPPORT
    td_u32 inter_size;
    td_s32 view_id;
    td_u32 negative_inter_layer_ref_frames;
    td_u32 positive_inter_layer_ref_frames;
    hevc_frame_store *fs_negative_inter_layer_ref[HEVC_MAX_DPB_NUM];
    hevc_frame_store *fs_positive_inter_layer_ref[HEVC_MAX_DPB_NUM];
#endif
    hevc_frame_store *fs[HEVC_MAX_DPB_NUM];
    hevc_frame_store *fs_negative_ref[HEVC_MAX_DPB_NUM];
    hevc_frame_store *fs_positive_ref[HEVC_MAX_DPB_NUM];
    hevc_frame_store *fs_ltref[HEVC_MAX_DPB_NUM];
} hevc_dpb;

typedef struct {
    td_u8 *stream_buffer;    /* [MAX_CODED_FRAME_SIZE], actual codebuffer for read bytes */
    td_u32 bitstream_length; /* over codebuffer length, byte oriented, UVLC only */
    UADDR stream_phy_addr;   /* physic addr of stream    unit: byte */
    td_u32 valid_bitlen;     /* valid pBs when dec slice data    unit: bit */
    td_u32 bits_offset;      /* offset in 128 pBs when this segment stream start    unit: bit */
    UADDR bytes_pos;         /* stream VDH will used start bytes ph addr, 4words/16bytes addr aligned     unit: byte */
    td_s32 stream_id;        /* stream seg id for release used */
} hevc_bit_stream;

typedef struct {
    td_u8 nal_used_segment; /* which segment using decode */
    td_u32 nal_bitoffset;
    td_u32 nal_segment; /* how many stream segment received in this nal */
    td_u32 nal_trail_zero_bit_num;
    td_u32 nal_unit_type;
    td_u32 nal_ref_idc;
    td_u32 forbidden_zero_bit;
    td_u32 nuh_reserved_zero_6bits;
    td_u32 nuh_temporal_id;
    td_u32 is_valid;
    td_u32 nal_integ;
    td_u32 nal_layer_id;

    hevc_bit_stream stream[2]; /* 2 :a number */
    td_s32 raw_stream_len;
} hevc_nalu;

typedef struct {
    hevc_nalu *slice_nal;
} hevc_msg_slot;

enum nal_unit_type {
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0, /* 0 */
    NAL_UNIT_CODED_SLICE_TRAIL_R,     /* 1 */

    NAL_UNIT_CODED_SLICE_TSA_N, /* 2 */
    NAL_UNIT_CODED_SLICE_TLA_R, /* 3     Current name in the spec: TSA_R */

    NAL_UNIT_CODED_SLICE_STSA_N, /* 4 */
    NAL_UNIT_CODED_SLICE_STSA_R, /* 5 */

    NAL_UNIT_CODED_SLICE_RADL_N, /* 6 */
    NAL_UNIT_CODED_SLICE_RADL_R, /* 7     Current name in the spec: RADL_R */

    NAL_UNIT_CODED_SLICE_RASL_N, /* 8 */
    NAL_UNIT_CODED_SLICE_RASL_R, /* 9     Current name in the spec: RASL_R */

    NAL_UNIT_RESERVED_VCL_N10,
    NAL_UNIT_RESERVED_VCL_R11,
    NAL_UNIT_RESERVED_VCL_N12,
    NAL_UNIT_RESERVED_VCL_R13,
    NAL_UNIT_RESERVED_VCL_N14,
    NAL_UNIT_RESERVED_VCL_R15,

    NAL_UNIT_CODED_SLICE_BLA_W_LP,   /* 16    Current name in the spec: BLA_W_LP */
    NAL_UNIT_CODED_SLICE_BLA_W_RADL, /* 17    Current name in the spec: BLA_W_DLP */
    NAL_UNIT_CODED_SLICE_BLA_N_LP,   /* 18 */
    NAL_UNIT_CODED_SLICE_IDR_W_RADL, /* 19    Current name in the spec: IDR_W_DLP */
    NAL_UNIT_CODED_SLICE_IDR_N_LP,   /* 20 */
    NAL_UNIT_CODED_SLICE_CRA,        /* 21 */
    NAL_UNIT_RESERVED_IRAP_VCL22,
    NAL_UNIT_RESERVED_IRAP_VCL23,

    NAL_UNIT_RESERVED_VCL24,
    NAL_UNIT_RESERVED_VCL25,
    NAL_UNIT_RESERVED_VCL26,
    NAL_UNIT_RESERVED_VCL27,
    NAL_UNIT_RESERVED_VCL28,
    NAL_UNIT_RESERVED_VCL29,
    NAL_UNIT_RESERVED_VCL30,
    NAL_UNIT_RESERVED_VCL31,

    NAL_UNIT_VPS,                   /* 32 */
    NAL_UNIT_SPS,                   /* 33 */
    NAL_UNIT_PPS,                   /* 34 */
    NAL_UNIT_ACCESS_UNIT_DELIMITER, /* 35 */
    NAL_UNIT_EOS,                   /* 36 */
    NAL_UNIT_EOB,                   /* 37 */
    NAL_UNIT_FILLER_DATA,           /* 38 */
    NAL_UNIT_PREFIX_SEI,            /* 39 Prefix SEI */
    NAL_UNIT_SUFFIX_SEI,            /* 40 Suffix SEI */
    NAL_UNIT_RESERVED_NVCL41,
    NAL_UNIT_RESERVED_NVCL42,
    NAL_UNIT_RESERVED_NVCL43,
    NAL_UNIT_RESERVED_NVCL44,
    NAL_UNIT_RESERVED_NVCL45,
    NAL_UNIT_RESERVED_NVCL46,
    NAL_UNIT_RESERVED_NVCL47,

    NAL_UNIT_EOPIC,    /* NAL_UNIT_UNSPECIFIED_48 */
    NAL_UNIT_EOSTREAM, /* NAL_UNIT_UNSPECIFIED_49 */
    NAL_UNIT_END_OF_BIG_FRAME, /* NAL_UNIT_UNSPECIFIED_50, end of big frame for air stream */
    NAL_UNIT_UNSPECIFIED_51,
    NAL_UNIT_UNSPECIFIED_52,
    NAL_UNIT_UNSPECIFIED_53,
    NAL_UNIT_UNSPECIFIED_54,
    NAL_UNIT_UNSPECIFIED_55,
    NAL_UNIT_UNSPECIFIED_56,
    NAL_UNIT_UNSPECIFIED_57,
    NAL_UNIT_UNSPECIFIED_58,
    NAL_UNIT_UNSPECIFIED_59,
    NAL_UNIT_UNSPECIFIED_60,
    NAL_UNIT_UNSPECIFIED_61,
    NAL_UNIT_UNSPECIFIED_62,
    NAL_UNIT_UNSPECIFIED_63,
    NAL_UNIT_INVALID,
};

typedef struct {
    td_u8 uniform_spacing_flag;
    td_s32 num_tile_rows_minus1;
    td_s32 num_tile_columns_minus1;
    td_s32 row_height_minus1[HEVC_MAX_TILE_ROWS];
    td_s32 column_width_minus1[HEVC_MAX_TILE_COLUMNS];
} hevc_tile_info;

typedef struct {
    td_s32 pic_width_in_luma_sample;
    td_s32 pic_height_in_luma_sample;
    td_s32 ctb_size_y_log2;
    td_s32 min_tb_size_log2;
    td_s32 pic_width_in_ctb;
    td_s32 pic_height_in_ctb;
} hevc_pic_size_info;

typedef struct {
    td_u8 last_pack_in_nal;
    td_u8 *p_stream;
    UADDR strm_phy_addr;
    td_s32 stream_len;
    td_s32 raw_stream_len;
    td_s32 stream_id;
    td_s32 seg_cut_state;
} hevc_stream_packet;

typedef enum {
    FIRST_FRAME_INIT = 0,
    FIRST_FRAME_PROCESSED,
} hevc_first_frame_state;

typedef struct {
    td_u32 update_flag;
    td_u32 sei_len;
    td_u32 sei_code;
} hevc_sei;

typedef struct {
    td_u8 valid;
    td_u8 new_sequence;
    td_u8 prev_rap_is_bla;
    td_u8 no_rasl_out_flag;
    td_s32 poc_cra;
    td_s32 poc_random;
} hevc_ldy_info;

typedef struct {
    td_u32 independent_cost;
    td_u32 independent_num;
    td_u32 dependent_cost;
    td_u32 dependent_num;
    td_u32 independent_max_cost;
    td_u32 dependent_max_cost;
    td_u32 independent_min_cost;
    td_u32 dependent_min_cost;
    td_u32 calc_slc_num;
} hevc_slice_cost;

typedef struct {
    td_u8 is_init;
    td_u8 new_sequence;
    td_u8 no_output_of_prior_pics_flag[HEVC_MAX_NUM_LAYERS];
    td_u8 no_rasl_output_flag[HEVC_MAX_NUM_LAYERS];
    td_u8 p_pic_found;
    td_u8 have_nal_not_decoded;
    td_u8 is_insert_eopic_nal;
    td_u8 is_stream_end_flag;
    td_u8 prev_rap_is_bla;
    td_u8 is_multi_layers;
    td_s8 have_independent_slice;
    td_s8 auc_convert_to_bit[HEVC_MAX_CU_SIZE + 1];
    td_u8 pmv_store_used_flag[HEVC_MAX_PMV_STORE];
    td_s32 max_slice_num;
    td_s32 max_vps_num;
    td_s32 max_sps_num;
    td_s32 max_pps_num;
    td_s32 max_slot_num;
    td_s32 true_dpb_size;
    td_u32 allow_start_dec;
    td_u32 curr_msg_slot_num;
    td_u32 total_fs_num;  /* change while g_DPB.size change */
    td_u32 total_pmv_num; /* change while g_DPB.size change */
    td_s32 prev_slice_poc;
    td_s32 last_slice_poc;
    td_u32 bit_depth_y;
    td_u32 bit_depth_c;
    td_s32 poc_random_access[HEVC_MAX_NUM_LAYERS];
    td_s32 poc_cra;
    td_u32 num_slice_para;
    td_u32 seg_cut_state;
    UADDR seg_base_phy_addr;

    td_u32 ldy_cfg;
    td_u32 ldy_mode;
#if defined(VFMW_HEVC_SLC_LDY_SUPPORT)
    td_u32 half_nal_en;
    td_u32 half_nal_try;
    td_u32 half_nal_succ;
    td_u32 no_next_slice;
    td_u32 try_slice_num;
    td_u32 post_pic_count;
    td_u32 first_slc_ctb;
    hevc_ldy_info ldy_info;
#endif

    td_s32 set_dps_xps;
    td_s32 last_display_poc[HEVC_MAX_NUM_LAYERS];
    td_u32 total_pic_num;
    td_u32 max_bytes_received;
    td_u32 add_cu_depth;
    td_u32 advanced_cfg;
    td_u32 ref_error_thr;
    td_u32 out_error_thr;
    td_s32 chan_id;
    td_u32 old_dec_mode;
    td_u32 prev_poc;
    td_s32 prev_vps_id;
    td_s32 prev_sps_id;
    td_s32 prev_pps_id;
    td_s32 prev_slc_pps_id;
    td_s32 prev_dps_size;
    td_u32 prev_pic_width;
    td_u32 prev_pic_height;
    td_u32 prev_l2_ctb_size_y;
    td_u32 prev_bit_depth_l;
    td_u32 prev_bit_depth_c;
    td_u32 prev_reorder_num;
    td_u32 new_pic_counter;
    td_u32 pic_out_counter;
    td_s32 total_frame_num;
    td_s32 output_frame_num;
    td_u32 scd_wrap_around;
    td_u32 need_drop_b_frame;
    td_s32 last_slice_pps_id;

    td_u32 scaling_list_size[4]; /* 4 :a number */
    td_u32 scaling_list_size_x[4]; /* 4 :a number */
    td_u32 sig_last_scan_cg_32x32[64]; /* 64 :a number */
    td_u32 qmatrix[4][6][16]; /* 4 types:4x4 8x8 16x16 32x32 */
    td_u32 scaling_list_num[SCALING_LIST_SIZE_NUM];
    td_u32 scaling_list_dc[SCALING_LIST_SIZE_NUM][HEVC_SCALING_LIST_NUM];

    bs_info *bs_ptr;
    hevc_video_param_set *vps[HEVC_MAX_VPS_NUM];
    hevc_seq_param_set *sps[HEVC_MAX_SPS_NUM];
    hevc_pic_param_set *pps[HEVC_MAX_PPS_NUM];
    syn_cfg_info *extra_data_ptr;
    syn_cfg_info extra_data;
    syn_stm_info stm_info;
    hevc_nalu *curr_nal;
    hevc_storable_pic *list_x[2][HEVC_MAX_LIST_SIZE]; /* 2 :a number */

#ifdef VFMW_MV_HEVC_SUPPORT
    td_u32 layer_num;
    td_u8 is_shvc_type;
    td_u8 is_mem_arrange_done;
    td_s32 curr_vps_id[HEVC_MAX_NUM_LAYERS];
    td_s32 curr_sps_id[HEVC_MAX_NUM_LAYERS];
    td_s32 curr_pps_id[HEVC_MAX_NUM_LAYERS];
    td_u32 layer_width[HEVC_MAX_NUM_LAYERS];
    td_u32 layer_height[HEVC_MAX_NUM_LAYERS];
    td_u32 max_width;
    td_u32 max_height;
    hevc_hrd_param_set hrd_param;
    hevc_view_queue viewque[HEVC_MAX_NUM_LAYERS];
    hevc_apc apc[HEVC_MAX_NUM_LAYERS];
#endif

    bs_info bs;
    hevc_dpb dpb;
    hevc_sei_param_set sei;
    hevc_sei private_sei;
    hevc_sei_pts_parser pts_parser;
    hevc_video_param_set *tmp_vps;
    hevc_seq_param_set *tmp_sps;
    hevc_pic_param_set *tmp_pps;
    hevc_video_param_set *curr_vps;
    hevc_seq_param_set *curr_sps;
    hevc_pic_param_set *curr_pps;
    hevc_seq_param_set *sei_sps;
    hevc_curr_pic curr_pic;
    hevc_slice_segment_header curr_slice;
    hevc_slice_segment_header independent_slice;
    hevc_first_frame_state first_frame_state;
    hevc_nalu nal_array[1];
    hevc_frame_store frame_store[HEVC_MAX_FRAME_STORE];

    hevc_nalu *last_nal;
    td_u32 general_profile_idc;
    td_u32 general_profile_compatibility_flag_1;
    td_u32 general_profile_compatibility_flag_2;
    td_u32 general_level_idc;

    td_u32 last_seg_id;
    td_u32 err_nal_cnt;
    td_u32 need_clear_que;
    td_u32 fake_frm_height_in_mb;
    td_u32 fake_frm_width_in_mb;
    td_u32 is_resolution_changed;
    td_u32 frame_rate;
    syn_mem_info mem_list[HEVC_MEM_NUM];
    td_u32 hevc_ctx_size;
    dps_info *curr_dps;
    td_s32 vps_update_flag;
    td_u32 total_usr_dat_num;
    vfmw_usrdat *usr_dat_array[MAX_USERDATA_NUM];
    afd_info afd_info_data;
    afd_info curr_afd_info;
    stm_seg_type seg_type;
    hevc_slice_cost slc_cost;
    td_u32 is_sec;
    td_u32 first_i_flag;
    td_s32 air_logic_id;
    air_state air_sta;
    vfmw_air_dec attr_air_dec;
    td_u32 curr_air_slice_id;
    td_u32 curr_air_layer_id;
    spec_invalid_type spec_invalid;
    td_u32 spec_value;
    vfmw_splice_info splice_info;
} hevc_ctx;

// hevc_init_: avoid compilation errors caused by VFMW and DEMUX redefinition.
td_s32 hevc_init_(td_s32 chan_id, td_void **ctx, const syn_cfg_info *extra_data_ptr);
td_s32 hevc_deinit(td_void *ctx, td_void *extra_data_ptr);
td_s32 hevc_config(td_void *ctx, const syn_cfg_info *extra_data_ptr);
td_s32 hevc_reset(td_void *ctx, td_void *args);
syn_parse_state hevc_decode(stm_seg_info *seg, dps_info *dps, td_void *ctx);
td_u32 hevc_get_ctx_size(const td_void *ctx);
td_s32 hevc_get_max_size(td_void);
td_void hevc_read_log(const td_void *args, td_void *buf, td_s32 len, td_s32 *used);
td_s32 hevc_control(td_void *ctx, td_s32 cmd_id, const td_void *args);
td_s32 hevc_get_parse_info(const td_void *ctx_ext, td_void *args_ext);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
