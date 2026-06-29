/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_H__
#define __VFMW_H__

#include "vfmw_ext.h"
#include "vfmw_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VFMW_VERSION  2026041700

#if defined(SOC_DEBUG) || defined(VT_SUPPORT)
#define VFMW_DEBUG 1
#else
#define VFMW_DEBUG 0
#endif

#ifndef INLINE
#ifdef VFMW_DPRINT_SUPPORT
#define INLINE
#else
#define INLINE inline
#endif
#endif

#ifndef STATIC
#define STATIC static
#endif

#define soc_sys_get_stride(w) (((w) + 63) / 64 * 64)

#define VFMW_CHAN_NUM VFMW_MAX_CHAN_NUM
#define VFMW_MAX_FRAME_NUM VFMW_FSP_MAX_FRAME_NUM

#define VFMW_INVALID_PTS ((td_s64)(-1))
#define VFMW_MIN_PTS     ((td_s64)(-2))
#define VFMW_MAX_PTS     ((td_s64)(0x1638E38B88))        /*  us  */
#define VFMW_MAX_SINT8    ((td_s8)0x7F)
#define VFMW_MAX_UINT16   ((td_u16)0xFFFF)
#define VFMW_MAX_SINT16   ((td_s16)0x7FFF)
#define VFMW_MAX_UINT32   ((td_u32)0xFFFFFFFF)
#define VFMW_MAX_SINT32   ((td_s32)0x7FFFFFFF)
#define VFMW_MIN_SINT32   ((td_s32)0x80000000)
#define VFMW_MAX_UINT64   ((td_u64)0xFFFFFFFFFFFFFFFF)
#define VFMW_MAX_HEIGHT   8192
#define VFMW_MAX_WIDTH    8192

#undef  LOG_MODULE_ID
#define LOG_MODULE_ID     SOC_ID_VFMW

#define VFMW_CTX_ALIGN 0x10
#define VFMW_DEV_ALIGN 0x10

/* Mem unique_id Format:
 <--   63 ~ 32 (32bit)  --  31 ~ 24 (8bit)  --  23 ~ 16 (8bit)  --  15 ~ 0 (16bit)  -->
 <--   Chan create time --   Mem Group ID   --      Mem ID      --     Chan ID      -->
*/
#define set_chan_uid(unid, chan_id)        (((td_u64)(unid) << 32) + ((chan_id) & 0xffff))
#define set_mem_uid(chan_uid, group, type) (((td_u64)(chan_uid)) | ((((td_u32)(group)) & 0xff) << 24) | \
                                            ((((td_u32)(type)) & 0xff) << 16))
#define get_chan_uid(uid)                  (((td_u64)(uid)) & 0xffffffff0000ffff)
#define get_chan_id_by_uid(uid)            (((td_u16)(uid)) & 0xffff)
#define get_mem_id_by_uid(uid)             ((((td_u32)(uid)) & 0x00ff0000) >> 16)

#define AFD_INFO_MIN_LENGTH_IN_BYTES       5
#define AFD_IDENTIFIER_LENGTH_IN_BYTES     4
#define AFD_IDENTIFIER                     0x31475444
#define AFD_ACTIVE_FORMAT_FLAG_BYTE_VALUE  0x41

#define UNREG_PLAYLOAD_LEN                 9
#define UUID_ISO_IEC_11578_LEN             16

#define VFMW_META_USERDATA_SIZE 0x2000 /* 4k metadata +4k userdata */
#define VFMW_MAX_META_USERDATA_SIZE (VFMW_META_USERDATA_SIZE * VFMW_MAX_FRAME_NUM)

#define uint64_ptr(ptr) ((td_void *)(td_uintptr_t)(ptr))
#define ptr_uint64(ptr) ((td_u64)(td_uintptr_t)(ptr))

#define vfmw_unused(x)  ((x) = (x))

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define CUVA_MAX_PARANUM_TM          2
#define CUVA_MAX_PARANUM_3SPLINE     2
#define CUVA_MAX_PARANUM_SM          8
#define SYSTEM_START_CODE_BIT        8
#define MINIMUM_MAXRGB_BIT           12
#define AVERAGE_MAXRGB_BIT           12
#define VARIANCE_MAXRGB_BIT          12
#define MAXIMUM_MAXRGB_BIT           12
#define TONE_MAPPING_MODE_BIT        1
#define TONE_MAPPING_PARAM_NUM_BIT   1
#define TARGETED_SYSTEM_DISPLAY_BIT  12
#define BASE_FLAG_BIT                1
#define BASE_PARAM_M_P_BIT           14
#define BASE_PARAM_M_M_BIT           6
#define BASE_PARAM_M_A_BIT           10
#define BASE_PARAM_M_B_BIT           10
#define BASE_PARAM_M_N_BIT           6
#define BASE_PARAM_K1_BIT            2
#define BASE_PARAM_K2_BIT            2
#define BASE_PARAM_K3_BIT            4
#define BASE_PARAM_DELTA_MODE_BIT    3
#define BASE_PARAM_DELTA_BIT         7
#define P3SPLINE_FLAG_BIT            1
#define P3SPLINE_NUM_BIT             1
#define P3SPLINE_TH_MODE_BIT         2
#define P3SPLINE_TH_MB_BIT           8
#define P3SPLINE_TH1_BIT             12
#define P3SPLINE_TH2_BIT             10
#define P3SPLINE_TH3_BIT             10
#define P3SPLINE_STRENGTH_BIT        8
#define COLOR_SATURATION_BIT         1
#define COLOR_SATURATION_NUM_BIT     3
#define COLOR_SATURATION_GAIN_BIT    8

#define vfmw_check_sec_func(ret) ((td_void)(ret))

typedef enum {
    INVALID_MODE = 0,
    QUERY_MODE = 1,
    REPORT_MODE,
    WORKMODE_BUTT
} vfmw_vpp_work_mode;

/* DAR(Display Aspect Ratio) */
typedef enum {
    VFMW_DAR_UNKNOWN = 0,
    VFMW_DAR_4_3,
    VFMW_DAR_16_9,
    VFMW_DAR_221_100,
    VFMW_DAR_235_100,
    VFMW_DAR_IMG_SIZE,
    VFMW_DAR_END_RESERVED
} vfmw_vdec_dar;

typedef enum {
    VFMW_MEM_GROUP_FSP = 0,
    VFMW_MEM_GROUP_USER = 1,
    VFMW_MEM_GROUP_BUTT
} mem_group;

typedef enum {
    NULL_MEM_TYPE,
    GLB_MEM_TYPE,
    DEV_HAL_MEM_TYPE,
    DEV_UMAP_MEM_TYPE,
    CHAN_MEM_TYPE,
    CACHE_MEM_TYPE = CHAN_MEM_TYPE,
    NO_CACHE_MEM_TYPE = CHAN_MEM_TYPE,
    CHAN_SHR_MEM_TYPE,
    RAW_MEM_TYPE,
    SEG_MEM_TYPE,
    NOR_MEM_TYPE,
    MAX_MEM_TYPE
} vfmw_mem_type;

/* / for no_spec type in fault_check */
typedef enum {
    SPEC_VALID = 0,          /* /< normal state */
    SPEC_WIDTH_INVALID = 1,  /* /< exceed width  */
    SPEC_HEIGHT_INVALID,     /* /< exceed height */
    SPEC_BIT_INVALID,        /* /< exceed bit */
    SPEC_PIX_FMT_INVALID,    /* /< invalid YUV collection specifications */
    SPEC_LOGIC_UNSUPPORT,    /* /< h265 */
    SPEC_REDUNDANT_PIC,      /* /< h264 */
} spec_invalid_type;

typedef enum {
    VFMW_MEM_MAX
} vfmw_mem_src;

typedef struct {
    td_u64 dma_buf;
    td_u8 is_sec;
    td_u8 is_cached;
    td_char *mem_name;
    td_u8 *vir_addr;
    UADDR phy_addr;
    UADDR mmu_addr;
    td_u32 length;
    td_u32 unid;
    vfmw_mem_src source;
    vfmw_mem_mode mode;
    vfmw_mem_type type;
    td_handle vdec_handle;
    td_handle ssm_handle;
} mem_record; /* no use btw arm64/mcu32/tee */

typedef struct {
    td_mem_handle_t fd;
    td_u64 fd_offset;
    td_u64 dma_buf;
    td_u64 vir_addr;
    td_u64 client_vir_addr;
    UADDR  phy_addr;
    td_s32 chan_id;
    vfmw_mem_type type;
} vfmw_mem_attach;

typedef struct {
    td_u32 is_mem_start;
    td_u8  data[VFMW_MAX_USD_SIZE]; /* usrdat data entity */
    td_u8  pic_coding_type;
    td_u8  top_field_first;
    td_s8  is_registered;           /* for CC, valid when IsRegistered=1 */
    td_u8  itu_t_t35_country_code;
    td_u8  itu_t_t35_country_code_extension_byte;
    td_u16 itu_t_t35_provider_code;
    td_u32 pic_num_count;
    td_u32 dnr_used_flag;          /* internal used only, ignore */
    vfmw_usd_src_type  from;           /* usrdat source */
    td_u32 seq_cnt;                /* to be removed later */
    td_u32 seq_img_cnt;
    td_s32 data_size;              /* usrdat size, in byte */
    td_s64 pts;                    /* pts of the frame containning the userdata */
    td_u64 dma_buf;
    td_u64 offset;
} vfmw_usrdat;

typedef struct {
    td_u8    *vir_addr;
    UADDR     phy_addr;
    td_s32    length;
} vfmw_stream_param;

typedef struct {
    td_s32   mode;
    td_s32   dis_nums;
} vfmw_dispic_param;

typedef struct {
    td_s32 chan_no_cache_size;
    td_u32 chan_cache_size;
    td_u32 hal_size;
    td_s32 hal_umap_size;
} vfmw_svr_mem_need;

typedef struct {
    UADDR  frm_phy_addr;
    td_u64 frm_vir_addr;
    td_u32 frm_size;

    UADDR  pmv_phy_addr;
    td_u64 pmv_vir_addr;
    td_u32 pmv_size;

    UADDR  meta_data_phy_addr;
    td_u64 meta_data_addr;
    td_u32 meta_data_size;
} frame_buf;

typedef struct {
    td_u8  linear_en;
    td_u8  compress_en;
    td_u32 y_stride;
    td_u32 uv_stride;
    td_u32 head_stride;
    td_u32 head_info_size;
    td_u32 stride_2bit;
    td_u32 bit_depth_luma;
    td_u32 bit_depth_chroma;
    td_u32 dec_width;
    td_u32 dec_height;

    td_u64 frm_dma_buf;
    UADDR  luma_phy_addr;
    UADDR  back_tunnel_line_num_phy_addr;
    UADDR  chrom_phy_addr;
    UADDR  luma_phy_addr_2bit;
    UADDR  chrom_phy_addr_2bit;
} image_dec;

typedef struct {
    td_u16 display_primaries_x[3]; /* 3: array size */
    td_u16 display_primaries_y[3]; /* 3: array size */
    td_u16 white_point_x;
    td_u16 white_point_y;
    td_u32 max_disp_mastering_luminance;
    td_u32 min_disp_mastering_luminance;
} mastering_display_colour_volums;

typedef struct {
    td_u32 max_content_light_level;
    td_u32 max_pic_average_light_level;
} content_light_level_info;

typedef struct {
    td_u8 hdr_transfer_characteristic_idc;
} hlg_compatible_info;

typedef struct {
    td_u8 tm_input_signal_black_level_offset;
    td_u8 tm_input_signal_white_level_offset;
    td_u8 shadow_gain;
    td_u8 highlight_gain;
    td_u8 mid_tone_width_adj_factor;
    td_u8 tm_output_fine_tuning_num_val; /* 0~10 */
    td_u8 tm_output_fine_tuning_x[11]; /* 11: array size */
    td_u8 tm_output_fine_tuning_y[11]; /* 11: array size */
    td_u8 saturation_gain_num_val;   /* 0~6 */
    td_u8 saturation_gain_x[7]; /* 7: array size */
    td_u8 saturation_gain_y[7]; /* 7: array size */
} sl_hdr1_metadata_variables;

typedef struct {
    td_u8  luminance_mapping_num_val; /* 0~33 */
    td_u16 luminance_mapping_x[34]; /* 34: array size */
    td_u16 luminance_mapping_y[34]; /* 34: array size */
    td_u8  colour_correction_num_val;  /* 0~33 */
    td_u16 colour_correction_x[34]; /* 34: array size */
    td_u16 colour_correction_y[34]; /* 34: array size */
    td_u16 chroma_to_luma_injection_mu_a;
    td_u16 chroma_to_luma_injection_mu_b;
} sl_hdr1_metadata_tables;

typedef struct {
    td_u8  spec_version;
    td_u8  payload_mode;
    td_u8  hdr_pic_colour_space;
    td_u8  hdr_master_display_colour_space;
    td_u16 hdr_master_display_max_luminance;
    td_u16 hdr_master_display_min_luminance;
    td_u8  sdr_pic_colour_space;
    td_u8  sdr_master_display_colour_space;
    union {
        sl_hdr1_metadata_variables variables;
        sl_hdr1_metadata_tables    tables;
    } metadata;
} sl_hdr1_matadata;

/* avs2 hdr */
typedef struct {
    td_u32  tm_input_signal_black_level_offset;
    td_u32  tm_input_signal_white_level_offset;
    td_u32  shadow_gain;
    td_u32  highlight_gain;
    td_u32  mid_tone_width_adj_factor;
    td_u32  tm_output_fine_tuning_num_val;
    td_u32  tm_output_fine_tuning_x[10]; /* 10: array size */
    td_u32  tm_output_fine_tuning_y[10]; /* 10: array size */
    td_u32  saturation_gain_num_val;
    td_u32  saturation_gain_x[6]; /* 6: array size */
    td_u32  saturation_gain_y[6]; /* 6: array size */
} sl_hdr_metadata_var;

typedef struct {
    td_u32  luminance_mapping_num_val;
    td_u32  luminance_mapping_x[65]; /* 65: array size */
    td_u32  luminance_mapping_y[65]; /* 65: array size */
    td_u32  colour_correction_num_val;
    td_u32  colour_correction_x[65]; /* 65: array size */
    td_u32  colour_correction_y[65]; /* 65: array size */
} sl_hdr_metadata_tab;

typedef struct {
    td_u32  part_id;
    td_u32  major_spec_version_id;
    td_u32  minor_spec_version_id;
    td_u32  payload_mode;
    td_u32  hdr_pic_colour_space;
    td_u32  hdr_display_colour_space;
    td_u32  hdr_display_max_luminance;
    td_u32  hdr_display_min_luminance;
    td_u32  sdr_pic_colour_space;
    td_u32  sdr_display_colour_space;
    td_u32  sdr_display_max_luminance;
    td_u32  sdr_display_min_luminance;
    td_u32  matrix_coefficient[4]; /* 4: array size */
    td_u32  chroma_to_luma_injection[2]; /* 2: array size */
    td_u32  k_coefficient[3]; /* 3: array size */
    union {
        sl_hdr_metadata_var variables;
    } u;
} sl_hdr_metadata;

typedef struct {
    td_u8 sub_sampling_y;
    td_u8 sub_sampling_x;
    td_u8 monochrome;
    td_s32 num_y_points;
    td_s32 num_cb_points;
    td_s32 num_cr_points;
    td_s32 cb_mult;
    td_s32 cb_luma_mult;
    td_s32 cb_offset;
    td_s32 cr_mult;
    td_s32 cr_luma_mult;
    td_s32 cr_offset;
    td_s32 overlap_flag;
    td_s32 clip_to_restricted_range;
    td_s32 bit_depth;
    td_s32 chroma_scaling_from_luma;
    td_s32 grain_scale_shift;
    td_s32 matrix_coefficients;
    td_u16 random_seed;
} film_grain_param;

typedef struct {
    td_u16 allow_width;
    td_u16 allow_height;
} image_allow_info;

typedef struct {
    td_u32 session_id;
    image_allow_info hdcp_allow_info[4]; /* 4: max size */
} img_hdcp_info;

typedef struct {
    td_u8  is_valid;
    td_u8  linear_en;
    td_u8  tile_format;
    td_u8  compress_en;
    td_u32 y_stride;
    td_u32 uv_stride;
    td_u32 head_stride;
    td_u32 head_info_size;
    td_u32 stride_2bit;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 disp_center_x;
    td_u32 disp_center_y;

    td_s64 frm_mem_fd;
    td_u64 frm_dma_buf;
    td_s64 frm_mem_fd_1;
    td_u64 frm_dma_buf_1;
    UADDR  luma_phy_addr;
    td_u64 luma_vir_addr;
    UADDR  back_tunnel_line_num_phy_addr;
    UADDR  chrom_phy_addr;
    UADDR  luma_phy_addr_1;
    UADDR  chrom_phy_addr_1;
    UADDR  luma_phy_addr_2bit;
    UADDR  chrom_phy_addr_2bit;

    td_u64 sec_info;
    td_u32 av1_film_tab_offset;
    td_bool apply_grain_flag;
    film_grain_param fg_param;
    img_hdcp_info hdcp_info;
} image_disp;

typedef struct {
    td_u32   actual_crc_8bit[2]; /* 2: array size */
    td_u32   actual_crc_2bit[2]; /* 2: array size */
    td_u32   actual_crc_8bit_1[2]; /* 2: array size */
} image_crc;

/*********************************************************************************************************************/
/* image_format bits introduce:                                                                                      */
/* [1:0] frame_type: 00(I), 01(P), 10(B), 11(reserved)                                                               */
/* [4:2] sample_type: 000(YUV:4:2:0), 001(YUV:4:0:0), 010~111(reserved)                                              */
/* [7:5] norm_type: 000(component), 001(PLA), 010(NTSC), 011(SECAM), 100(MAC),                                       */
/*                  101(Unspecified Video Format), 110~111(reserved)                                                 */
/* [9:8] source_format: 00(progressive), 01(interlaced), 10(infered_progressive), 11(infered_interlaced)             */
/* [11:10] field_valid_flag: 00(top_field invalid, bottom_field invalid), 01(top_field valid, bottom_field invalid), */
/*                           10(top_field invalid, bottom_field valid), 11(top_field valid, bottom_field valid)      */
/* [13:12]top_field_first: 00(bottom field first), 01(top field first), 10(un-know), 11(reserved)                    */
/* [16:14] aspect_ratio: 000(unspecified), 001(4:3), 010(16:9), 011(2.21:1),100(2.35:1),                             */
/*                       101(origin width and height), 111(reserved)                                                 */
/* [17]    vdh_2d_en: 0: 1d , 1: 2d                                                                                  */
/* [18]    pts_unit : 0: ms , 1: us                                                                                  */
/* [31:19] reserved                                                                                                  */
/*********************************************************************************************************************/
typedef struct {
    td_u8 frame_type;           /* [1:0] */
    td_u8 sample_type;          /* [4:2] */
    td_u8 video_format;         /* [7:5] */
    td_u8 source_format;        /* [9:8] */
    td_u8 field_valid_flag;     /* [11:10] */
    td_u8 top_field_first;      /* [13:12] */
    td_u8 aspect_ratio;         /* [16:14] */
    td_u8 frame_rate_valid;     /* [20] */
    td_u8 repeat_first_field;
    td_u8 reserved;
} image_format;

typedef union {
    td_u32  data;
    struct {
        td_u32 top_over : 8; /* 0 not dec  1 dec success 2 dec error  3 needed dec but in VOQ */
        td_u32 bot_over : 8; /* 0 not dec  1 dec success 2 dec error  3 needed dec but in VOQ */
        td_u32 reserved : 16;
    } bits;
} image_dec_over;

typedef struct {
    td_bool active_format_flag;
    td_u32 active_format;
} afd_info;

typedef struct {
    td_u8  mastering_available;
    td_u8  content_available;
    td_u8  s1_hdr1_metadata_available;
    td_u8  is_backwards_compatible;
    td_bool dynamic_metadata_valid;
    td_bool cuva_dynamic_metadata_valid;

    UADDR  hdr_metadata_phy_addr;
    td_u32 hdr_metadata_size;
    td_u64 hdr_metadata_vir_addr;
    td_u64 hdr_dma_buf;
    td_mem_handle_t hdr_metadata_fd;
    td_u64 hdr_addr_offset;

    vfmw_color_desc colour_info;
    mastering_display_colour_volums mastering_display_colour_volume;
    content_light_level_info content_light_level;
    hlg_compatible_info comapitibility_info;

    td_u8         be_dv_hdr_chan;
    td_u8         dv_compatible;
    vfmw_dv_ves_imp ves_imp;
    vfmw_dv_layer_imp layer_imp;
    vfmw_dv_codec_cap dv_capacity;
} vfmw_hdr_info;

typedef enum {
    FRAME_PACKING_TYPE_NONE,             /* normal frame, not a 3D frame */
    FRAME_PACKING_TYPE_SIDE_BY_SIDE,     /* side by side */
    FRAME_PACKING_TYPE_TOP_BOTTOM,       /* top bottom */
    FRAME_PACKING_TYPE_TIME_INTERLACED,  /* time interlaced: one frame for left eye, the next frame for right eye */
    FRAME_PACKING_TYPE_BUTT
} vfmw_frm_pack_type;

typedef struct {
    td_u8  pic_structure; /* 0: frame, 1: top, 2: bottom, 3: mbaff, 4: field pair */
    td_u8  pic_qp_enable;
    td_u8  chroma_format_idc; /* 0: yuv400 , 1: yuv420 */
    td_u8  vc1_profile;

    td_s32 qp_y;
    td_s32 qp_u;
    td_s32 qp_v;
    td_s32 ranged_frm;

    td_u8  range_mapy_flag;
    td_u8  range_mapy;
    td_u8  range_mapuv_flag;
    td_u8  range_mapuv;

    td_u8  bottom_range_mapy_flag;
    td_u8  bottom_range_mapy;
    td_u8  bottom_range_mapuv_flag;
    td_u8  bottom_range_mapuv;
} vfmw_vc1_range;

typedef struct {
    td_u8  mjpeg;
    td_s32 is_progressive_seq;
    td_s32 is_progressive_frm;
    td_s32 real_frm_rate;
    td_s32 matrix_coef;
    td_u32 rwzb;
} optmalg_info;

typedef struct {
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 bit_depth;

    td_u32 luma_stride;

    td_u32 luma_header_offset;
    td_u32 luma_data_offset;

    td_u32 chroma_header_offset;
    td_u32 chroma_data_offset;

    td_u32 luma_data2_offset;     /* for 10bit or wider bits use */
    td_u32 chroma_data2_offset;   /* for 10bit or wider bits use */
} vfmw_attr_hfbc;

typedef struct {
    td_u32 pix_sum_luma;
    td_u32 pix_sum_chroma;
} image_pix;

typedef enum {
    VFMW_SPLICE_NONE = 0,
    VFMW_SPLICE_AYUV_TAB,
    VFMW_SPLICE_MAX
} vfmw_splice_type;

typedef enum {
    VFMW_SPLICE_FORMAT_AYUV = 0,
    VFMW_SPLICE_FORMAT_MAX
} vfmw_splice_format_type;

typedef enum {
    VFMW_SPLICE_PACKING_TAB = 0,
    VFMW_SPLICE_PACKING_MAX
} vfmw_splice_packing_type;

typedef enum {
    VFMW_SPLICE_VERSION_V1 = 0,
    VFMW_SPLICE_VERSION_MAX
} vfmw_splice_version_type;

typedef struct {
    td_u8 version;
    td_u32 ori_width;
    td_u32 ori_height;
    vfmw_splice_type type;
    vfmw_splice_format_type format_type;
    vfmw_splice_packing_type packing_type;
} vfmw_splice_info;

typedef struct {
    td_u32      version;
    td_u32      aspect_width;
    td_u32      aspect_height;
    td_u32      disp_enable_flag;
    td_u32      disp_frame_distance;
    td_u32      distance_before_first_frame;
    td_u32      gop_num;

    td_bool     is_user_dec;
    td_u32      is_fld_flag;  /* 0:frm, 1:fld */
    td_u32      top_fld_type;
    td_u32      bottom_fld_type;
    td_u32      image_width;
    td_u32      image_height;
    td_u32      frame_rate;   /* frame rate, in Q10 */
    td_u32      image_id;
    td_u32      error_level;
    td_u32      seq_img_cnt;

    td_u32      bit_depth;
    td_s32      last_frame;
    td_s32      view_id;      /* h264 mvc */
    td_s32      image_id_1;
    td_u32      is_3d;
    UADDR       line_num_phy_addr;
    td_u32      rwzb;
    td_u64      frame_ptr;
    td_u32      out_put_flag;
    td_u32      left_offset;
    td_u32      right_offset;
    td_u32      top_offset;
    td_u32      bottom_offset;
    td_u32      is_sec_frm;
    td_s32      inter_pts_delta;
    td_u64      usrdat[VFMW_MAX_USD_NUM];
    td_s64      src_pts;
    td_s64      pts;
    td_s32      poc;
    td_s32      pic_qp;
    td_u64      user_tag;
    td_u32      fence_flag;
    td_u64      disp_time;
    td_u8       dv_dual_layer;
    td_s32      shr_index;
    td_u16      disp_ctrl_width;
    td_u16      disp_ctrl_height;
    td_u32      image_width_ref;
    td_u32      image_height_ref;
    td_u32      buf_size;
    td_u32      tick;
    td_bool     is_spliced;
    td_u32      cycle;

    vfmw_splice_info    splice_info;
    image_dec           dec_info;
    image_disp          disp_info;
    image_crc           crc_info;
    image_pix           pix_info;
    image_format        format;
    image_dec_over      dec_over;

    afd_info            afd;
    vfmw_yuv_fmt        yuv_format;
    vfmw_hdr_info       hdr_input;
    vfmw_vid_std        vid_std;
    vfmw_frm_pack_type  frm_pack_type;
    vfmw_image_attached attach_buf;
} vfmw_image;

typedef struct {
    td_u32      image_id;
    td_s32      image_id_1;
    td_u32      is_3d;
} vfmw_image_rls;

typedef struct {
    td_s32 (*mark_image_state)(td_s32 chan_id, vfmw_image *image, td_s32 structure, td_u32 state);
    td_s32 (*mark_field_flag)(td_s32 chan_id, const vfmw_image *image);
    td_s32 (*update_image_info)(td_s32 chan_id, const vfmw_image *image);
    td_void (*try_rels_image)(td_s32 chan_id);
} vfmw_fsp_frame_intf;

typedef struct {
    td_u16 system_start_code;
    td_u16 minimum_maxrgb_pq;
    td_u16 average_maxrgb_pq;
    td_u16 variance_maxrgb_pq;
    td_u16 maximum_maxrgb_pq;
    td_u16 tone_mapping_enable_mode_flag;
    td_u16 tone_mapping_param_enable_num;
    td_u16 targeted_system_display_maximum_luminance_pq[CUVA_MAX_PARANUM_TM];
    td_u16 base_enable_flag[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_m_p[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_m_m[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_m_a[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_m_b[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_m_n[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_k1[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_k2[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_k3[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_delta_enable_mode[CUVA_MAX_PARANUM_TM];
    td_u16 base_param_enable_delta[CUVA_MAX_PARANUM_TM];
    td_u16 p_3spline_enable_flag[CUVA_MAX_PARANUM_TM];
    td_u16 p_3spline_enable_num[CUVA_MAX_PARANUM_TM];
    td_u16 p_3spline_th_enable_mode[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_u16 p_3spline_th_enable_mb[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_u16 p_3spline_th_enable[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_s16 p_3spline_th_enable_delta1[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_s16 p_3spline_th_enable_delta2[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_s16 p_3spline_enable_strength[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];
    td_u16 color_saturation_mapping_enable_flag;
    td_u16 color_saturation_enable_num;
    td_u16 color_saturation_enable_gain[CUVA_MAX_PARANUM_SM];
} vfmw_cuva_metadata;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* *< __VFMW_H__  */
