/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_EXT_H__
#define __VFMW_EXT_H__

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*  for adapt vfmw v6r2  */
#define UADDR         td_u32
#define MAX_VAL_UADDR (-1U)

#define VFMW_MAX_USD_NUM         4
#define VFMW_MAX_USD_SIZE        0x300
#define VFMW_CREATE_OPT_SIZE     128
#define VFMW_REPORT_MAX_NAME_LEN 32
#define VFMW_MAX_FRM_NUM         16
#define VFMW_OUTER_PARA_SIZE     12

/* omx buffer flags bits. */
#define VFMW_OMX_STM_FLAG_EOS         0x00000001
#define VFMW_OMX_STM_FLAG_STARTTIME   0x00000002
#define VFMW_OMX_STM_FLAG_DECODEONLY  0x00000004
#define VFMW_OMX_STM_FLAG_DATACORRUPT 0x00000008
#define VFMW_OMX_STM_FLAG_ENDOFFRAME  0x00000010
#define VFMW_OMX_STM_FLAG_SYNCFRAME   0x00000020
#define VFMW_OMX_STM_FLAG_EXTRADATA   0x00000040
#define VFMW_OMX_STM_FLAG_CODECCONFIG 0x00000080

typedef td_s32 (*fun_vfmw_mod_init)(td_void);
typedef td_void (*fun_vfmw_mod_exit)(td_void);
typedef td_s32 (*fun_vfmw_init)(td_void *args);
typedef td_s32 (*fun_vfmw_exit)(td_void *args);
typedef td_s32 (*fun_vfmw_suspend)(td_void *args);
typedef td_s32 (*fun_vfmw_resume)(td_void *args);
typedef td_s32 (*fun_vfmw_get_frame)(td_s32 chan_id, td_void *frame, td_void *ext_frm_info);
typedef td_s32 (*fun_vfmw_rels_frame)(td_s32 chan_id, const td_void *frame);
typedef td_s32 (*fun_vfmw_control)(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len);
typedef td_s32 (*fun_vfmw_event_report)(td_s32 chan_id, td_s32 type, const td_void *args, td_s32 size);

typedef enum {
    VFMW_STD_START = 0,
    VFMW_H264,
    VFMW_VC1,
    VFMW_MPEG4,
    VFMW_MPEG2,
    VFMW_H263,
    VFMW_DIVX3,
    VFMW_AVS,
    VFMW_JPEG,
    VFMW_REAL8,
    VFMW_REAL9,
    VFMW_VP6,
    VFMW_VP6F,
    VFMW_VP6A,
    VFMW_VP8,
    VFMW_SORENSON,
    VFMW_MVC,
    VFMW_HEVC,
    VFMW_VP9,
    VFMW_AVS2,
    VFMW_AV1,
    VFMW_AVS3,
    VFMW_MJPEG,
    VFMW_DBVS,
    VFMW_WMV1,
    VFMW_WMV2,
    VFMW_WMV3,
    VFMW_STD_MAX
} vfmw_vid_std;

typedef enum {
    VFMW_DEC_MODE_NORMAL = 0,
    VFMW_DEC_MODE_IP,
    VFMW_DEC_MODE_I,
    VFMW_DEC_MODE_FIRST_I,
    VFMW_DEC_MODE_DROP_INVALID_B,
    VFMW_DEC_WORK_MODE_KEEP_INVALID_B,
    VFMW_DEC_MODE_MAX
} vfmw_dec_mode;

typedef enum {
    VFMW_DISP_ORDER = 0,
    VFMW_DEC_ORDER,
    VFMW_DEC_ORDER_SIMPLE_DPB,
    VFMW_DEC_ORDER_SKYPLAY,
    VFMW_DISP_ORDER_BASE_LAYER,      /* output base layer only (shvc etc), same as VFMW_DISP_ORDER */
    VFMW_DISP_ORDER_ENHANCED_LAYER,  /* output enhanced layer only (shvc etc) */
    VFMW_DISP_ORDER_ALL_LAYERS,      /* output all layers (mv-hevc/shvc etc) */
    VFMW_OUTPUT_ORDER_MAX
} vfmw_out_order;

typedef enum {
    BASE_LAYER = 0,
    ENHANCED_LAYER,
    ALL_LAYERS,
    OUTPUT_LAYER_MAX
} vfmw_output_layer;

typedef enum {
    VFMW_CMP_ADJUST = 0,
    VFMW_CMP_ON,
    VFMW_CMP_OFF,
    VFMW_CMP_MAX
} vfmw_cmp;

typedef enum {
    VFMW_SOURCE_MEDIA = 0,
    VFMW_SOURCE_DTV,
    VFMW_SOURCE_NETWORK,
    VFMW_SOURCE_CAST,
    VFMW_SOURCE_MAX
} vfmw_source;

typedef enum {
    VFMW_BIT_DEPTH_NORMAL = 0,
    VFMW_BIT_DEPTH_8BIT_ONLY,
    VFMW_BIT_DEPTH_16BIT_EXPANDED,
    VFMW_BIT_DEPTH_MAX
} vfmw_bit_depth_attr;

typedef enum {
    VFMW_FLUSH_IN = 0,
    VFMW_FLUSH_OUT,
    VFMW_FLUSH_ALL,
    VFMW_FLUSH_MAX
} vfmw_flush_type;

typedef enum {
    VFMW_FRM_ALLOC_INNER = 0,
    VFMW_FRM_ALLOC_OUTER,
    VFMW_FRM_ALLOC_MAX
} vfmw_frm_alloc_type;

typedef enum {
    VFMW_CID_GET_CAPABILITY = 0,
    VFMW_CID_CREATE_CHAN,
    VFMW_CID_DESTROY_CHAN,
    VFMW_CID_GET_CHAN_CFG,
    VFMW_CID_CFG_CHAN,
    VFMW_CID_GET_CHAN_STATE,
    VFMW_CID_START_CHAN,
    VFMW_CID_STOP_CHAN,
    VFMW_CID_RESET_CHAN,
    VFMW_CID_SET_STREAM_INTF,
    VFMW_CID_GET_CHAN_MEMSIZE,
    VFMW_CID_RELEASE_STREAM,
    VFMW_CID_RESET_CHAN_WITH_OPTION,
    VFMW_CID_SET_FRAME_INTF,
    VFMW_CID_GET_USRDEC_STREAM,
    VFMW_CID_PUT_USRDEC_STREAM,
    VFMW_CID_GET_USRDEC_FRAME,
    VFMW_CID_PUT_USRDEC_FRAME,
    VFMW_CID_SET_DISCARDPICS_PARAM,
    VFMW_CID_EXTRA_WAKEUP_THREAD,
    VFMW_CID_SET_PTS_TO_SEEK,
    VFMW_CID_SET_TRICK_MODE,
    VFMW_CID_REPORT_DEMUX_RELEASE,
    VFMW_CID_SET_CTRL_INFO,
    VFMW_CID_SET_FRAME_RATE,
    VFMW_CID_START_LOWDLAY_CALC,
    VFMW_CID_STOP_LOWDLAY_CALC,
    VFMW_CID_ALLOC_MEM_TO_CHANNEL,
    VFMW_CID_ALLOC_MEM_TO_DECODER,
    VFMW_CID_SET_FRAME_BUFFER_NUM,
    VFMW_CID_SET_FRAME_MEM,
    VFMW_CID_SET_MORE_GAP_ENABLE,
    VFMW_CID_SET_FRAME_RATE_TYPE,
    VFMW_CID_GET_FRAME_RATE_TYPE,
    VFMW_CID_BIND_MEM,
    VFMW_CID_BIND_ATTACH,
    VFMW_CID_SET_FRAME_BUFFER_MODE,
    VFMW_CID_BIND_EXTERN_BUFFER,
    VFMW_CID_UNBIND_EXTERN_BUFFER,
    VFMW_CID_INQUIRE_ADPT_TYPE,
    VFMW_CID_ALLOC_SEC_BUF,
    VFMW_CID_FREE_SEC_BUF,
    VFMW_CID_INIT_ES_BUF,
    VFMW_CID_DEINIT_ES_BUF,
    VFMW_CID_BIND_ES_USR_ADDR,
    VFMW_CID_QUEUE_STREAM,
    VFMW_CID_DEQUEUE_STREAM,
    VFMW_CID_SEND_EOS,
    VFMW_CID_SET_EVENT_MAP,
    VFMW_CID_BIND_ES_BUF,
    VFMW_CID_UNBIND_ES_BUF,
    VFMW_CID_FLUSH_CHAN,
    VFMW_CID_SET_IDR_PTS,
    VFMW_CID_SET_3D_TYPE,
    VFMW_CID_GET_3D_TYPE,
    VFMW_CID_FLUSH_POST_PROCESS,
    VFMW_CID_CHECK_FRM_STATE,
    VFMW_CID_BIND_EXTERN_BUFFER_READY,
    VFMW_CID_UNBIND_EXTERN_BUFFER_READY,
    VFMW_CID_BIND_SEG_BUF,
    VFMW_CID_BEGIN_PIC,
    VFMW_CID_RENDER_PIC,
    VFMW_CID_END_PIC,
    VFMW_CID_MAX
} vfmw_cid;

typedef enum {
    MEM_CMA_CMA = 0,
    MEM_CMA_SEC,
    MEM_MMU_MMU,
    MEM_MMU_SEC,
} vfmw_mem_mode;

/* Dolby Vision elementary stream codec capacity */
typedef enum {
    DV_BL_DECODING_SUPPORT = 0, /* support decoding BL elementary stream only */
    DV_EL_DECODING_SUPPORT,     /* support decoding EL elementary stream only */
    DV_DECODING_MAX,
} vfmw_dv_codec_cap;

/* Dolby Vision VES implementation type */
typedef enum {
    DV_VES_IMP_START = 0,
    DV_SINGLE_VES_IMP,          /* single VES implementation */
    DV_DUAL_VES_IMP,            /* dual VES implementation */
    DV_VES_IMP_MAX
} vfmw_dv_ves_imp;

/* Dolby Vision layer implementation type */
typedef enum {
    DV_LAYER_START = 0,
    DV_SINGLE_LAYER_IMP,        /* single VES implementation */
    DV_DUAL_LAYER_IMP,          /* dual VES implementation */
    DV_LAYER_IMP_MAX
} vfmw_dv_layer_imp;

typedef enum {
    EXT_BUF_STATUS_INVALID = 0,
    EXT_BUF_STATUS_AVAILABLE,
    EXT_BUF_STATUS_UNAVAILABLE,
    EXT_BUF_STATUS_MAX
} vfmw_ext_buf_status;

typedef enum {
    ADPT_TYPE_VDEC = 0,
    ADPT_TYPE_MAX
} vfmw_adpt_type;

typedef enum {
    PTS_FRMRATE_TYPE_PTS = 0,     /* use frame rate from pts */
    PTS_FRMRATE_TYPE_STREAM,      /* use frame rate from stream */
    PTS_FRMRATE_TYPE_USER,        /* use frame rate set by user */
    PTS_FRMRATE_TYPE_USER_PTS,    /* use frame rate set by user until 2nd I frame, then use frame rate from pts */
    PTS_FRMRATE_TYPE_MAX
} vfmw_pts_framrate_type;

typedef enum {
    SYNTAX_ERR_NONE = 0,
    SYNTAX_ERR_VPS,
    SYNTAX_ERR_SPS,
    SYNTAX_ERR_PPS,
    SYNTAX_NO_REF,
    SYNTAX_ERR_SLICE_HDR,
    DEC_HW_ERR,
    SYNTAX_ERR_MAX
} vfmw_dec_err_type;

typedef enum {
    VFMW_USD_INVALID = 0,
    VFMW_USD_MP2SEQ,
    VFMW_USD_MP2GOP,
    VFMW_USD_MP2PIC,
    VFMW_USD_MP4VSOS,
    VFMW_USD_MP4VSO,
    VFMW_USD_MP4VOL,
    VFMW_USD_MP4GOP,
    VFMW_USD_H264_REG,
    VFMW_USD_H264_UNREG,
    VFMW_USD_AVSSEQ,
    VFMW_USD_AVSPIC,
    VFMW_USD_MAX
} vfmw_usd_src_type;

typedef enum {
    EVNT_START             = 0,
    EVNT_NEW_IMAGE         = 1 << 0,
    EVNT_USRDAT            = 1 << 1,
    EVNT_FIND_IFRAME       = 1 << 2,
    EVNT_VDM_ERR           = 1 << 3,
    EVNT_UNSUPPORT         = 1 << 4,
    EVNT_SE_ERR            = 1 << 5,
    EVNT_MEET_NEWFRM       = 1 << 6,
    EVNT_DECSYNTAX_ERR     = 1 << 7,
    EVNT_LAST_FRAME        = 1 << 8,
    EVNT_RESOLUTION_CHANGE = 1 << 9,
    EVNT_NEED_ARRANGE      = 1 << 10,
    EVNT_UNSUPPORT_SPEC    = 1 << 11,
    EVNT_FAKE_FRAME        = 1 << 12,
    EVNT_VIDSTD_ERROR      = 1 << 13,
    EVNT_ERR_FRAME         = 1 << 14,
    EVNT_ALLOC_MEM         = 1 << 15,
    EVNT_RELEASE_MEM       = 1 << 16,
    EVNT_RELEASE_BURST_MEM = 1 << 17,
    EVNT_CANCEL_ALLOC      = 1 << 18,
    EVNT_SET_ADDRESS_IN    = 1 << 19,
    EVNT_NEW_YUV           = 1 << 20,
    EVNT_RLS_STREAM        = 1 << 21,
    EVNT_RECEIVE_FAILED    = 1 << 22,
    EVNT_FIRST_PTS         = 1 << 23,
    EVNT_SECOND_PTS        = 1 << 24,
    EVNT_NORM_CHG          = 1 << 25,
    EVNT_FRM_PCK_CHG       = 1 << 26,
    EVNT_HDR_INFO          = 1 << 27,
    EVNT_REPORT_ERR        = 1 << 28,
    EVNT_OUT_OF_MEMORY_ERR = 1 << 29,

    EVNT_MAX               = 1 << 30
} vfmw_evnt_type;

typedef enum {
    VFMW_SPEC_BIT_DEPTH = 0,    /* unsupport bit depth */
    VFMW_SPEC_RESOLUTION,
    VFMW_SPEC_MAX
} vfmw_unsupport_spec;

typedef enum {
    SPY_CB_CR_420 = 0,
    SPY_CB_CR_422_1X2,
    SPY_CB_CR_422_2X1,
    SPY_CB_CR_444,
    SPY_CB_CR_400,
    SPY_CB_CR_411,
    PLNY_CB_CR_400,
    PLNY_CB_CR_411,
    PLNY_CB_CR_420,
    PLNY_CB_CR_422_1X2,
    PLNY_CB_CR_422_2X1,
    PLNY_CB_CR_444,
    PLNY_CB_CR_410,
    VFMW_YUV_FMT_MAX
} vfmw_yuv_fmt;

typedef enum {
    VFMW_FRM_LAYOUT_ADPT = 0,
    VFMW_FRM_LAYOUT_TILE,
    VFMW_FRM_LAYOUT_LINEAR,
    VFMW_FRM_LAYOUT_MAX
} vfmw_frm_layout;

typedef enum {
    VFMW_FRAME_TYPE_I,
    VFMW_FRAME_TYPE_P,
    VFMW_FRAME_TYPE_B,
    VFMW_FRAME_TYPE_MAX
} vfmw_frame_type;

typedef enum {
    VFMW_NORM_PAL = 0,
    VFMW_NORM_NTSC,
    VFMW_NORM_MAX
} vfmw_norm_type;

typedef enum {
    VFMW_FRM_LAYOUT_CAP_LINEAR = 1 << 0,
    VFMW_FRM_LAYOUT_CAP_TILE = 1 << 1,
    VFMW_FRM_LAYOUT_CAP_MAX
} vfmw_frm_layout_cap;

typedef enum {
    VFMW_BITDEPTH_CAP_DEFAULT = 1 << 0,
    VFMW_BITDEPTH_CAP_8_BIT_ONLY = 1 << 1,
    VFMW_BITDEPTH_CAP_16_BIT_EXPAND = 1 << 2,
    VFMW_BITDEPTH_CAP_MAX
} vfmw_bitdepth_cap;

typedef enum {
    VFMW_OUTER_FRM_NO_WAIT,  /* default mode */
    VFMW_OUTER_FRM_WAIT, /* wait for unbind ready */
    VFMW_OUTER_FRM_MAX
} vfmw_outer_frm_mode;

typedef union {
    struct {
        td_s32 is_adv_profile;
        td_s32 codec_version;
    } vc1_ext;

    struct {
        td_s32 reversed;
        td_u32 disp_width;
        td_u32 disp_height;
    } vp6_ext;

    struct {
        td_u32 need_film_grain;
    } av1_ext;
} vfmw_std_ext;

typedef struct {
    td_u8  is_seek_pending;
    td_u32 flags;
    td_u32 buf_len;
    td_u32 cfg_width;
    td_u32 cfg_height;
} vfmw_raw_ext;

typedef struct {
    td_u8  is_available;
    td_u8  full_rangeflag;
    td_u8  colour_primaries;
    td_u8  matrix_coeffs;
    td_u32 transfer_characteristics;
} vfmw_color_desc;

typedef enum {
    AIR_DEC_UNSUPPORT,
    AIR_DEC_FRAME,
    AIR_DEC_SLICE,
    AIR_DEC_MAX,
} vfmw_air_dec_version;

typedef struct {
    vfmw_air_dec_version air_version;
    td_u32 slice_num;
    td_u32 layer_num;
    td_u32 max_width;
    td_u32 max_height;
    td_u32 fixed_airdec;
} vfmw_air_dec;

typedef struct {
    td_s64  handle;
    td_u64  dma_buf;
    td_u64  offset;
    UADDR   phy_addr;
    td_u64  kern_vir_addr;
    td_u64  usr_vir_addr;
    td_u32  length;
    td_s64  pts;
    td_bool is_end_of_frm;
    td_bool is_dual_layer;
    td_u32  index;
    td_u32  marker;
    /* pvr info */
    td_u32  disp_enable_flag;
    td_u32  disp_frm_distance;
    td_u32  distance_before_first_frame;
    td_u32  gop_num;
    td_u64  disp_time;
} vfmw_stream_buf;

typedef struct {
    td_u8  valid;
    td_u8  is_not_last_packet_flag;
    td_u8  is_stream_end_flag;
    td_u8  is_extern_eop_flag;
    UADDR  phy_addr;
    td_u32 length;
    td_u32 index;
    td_u32 discontinue_count;
    td_u32 disp_enable_flag;
    td_u32 disp_frame_distance;
    td_u32 distance_before_first_frame;
    td_u32 gop_num;
    td_u64 vir_addr;
    td_u64 user_tag;
    td_u64 disp_time;
    td_s64 pts;
    td_u64 time_stamp;
    vfmw_raw_ext  raw_ext;
    td_u8  dv_dual_layer; /* 0: SL 1:DL */

    /* time point */
    td_u32 que_time;
    td_u32 deq_time;
} vfmw_stream_data;

typedef struct {
    td_s32 stream_provider_inst_id;
    td_s32 (*read_stream)(td_s32 inst_id, vfmw_stream_data *packet);
    td_s32 (*release_stream)(td_s32 inst_id, const vfmw_stream_data *packet);
} vfmw_stream_intf;

typedef struct {
    td_u32 idr_flag;               /* IDR frame Flag, 1 means IDR frame */
    td_u32 b_frm_ref_flag;         /* whether B frame is refer frame, 1 means B frame is refer frame */
    td_u32 continuous_flag;        /* whether send frame is continusous. 1 means continusous */
    td_u32 backward_optimize_flag; /* Backward optimize flag, 1 means optimize the backward fast play performance */
    td_u32 disp_pptimize_flag;     /* Display optimize flag, 1 means optimize the VO display performance */
} vfmw_control_info;

typedef struct {
    td_u8   sec_flag;
    td_char buf_name[VFMW_REPORT_MAX_NAME_LEN];
    td_u64  dma_buf;
    td_s64  mem_fd;
    td_u32  offset;
    UADDR   phy_addr;
    td_u64  vir_addr;
    td_bool is_map;
    td_bool is_cache;
    td_u32  size;
    td_s64  unique_id;
    td_u32  align_size;
    td_u32  union_num;
    td_u64  sec_info;
} vfmw_mem_report;

typedef struct {
    vfmw_mem_report ext_frm_buf;
    vfmw_mem_report ext_info_buf;
    vfmw_mem_report ext_hfbc_buf;
    vfmw_ext_buf_status status;
} vfmw_ext_buf;

typedef struct {
    vfmw_vid_std vid_std;
    vfmw_std_ext std_ext;
    td_s32 vdec_handle;
    td_u8 chan_priority;
    vfmw_dec_mode dec_mode;
    td_u8 single_i_frame;
    td_s32 dec_order_output;
    vfmw_bit_depth_attr en_bit_depth_cfg;
    td_s8 chan_err_thr;
    td_u8  sec_flag;
    td_s8  is_miracast_chan;
    td_s8  be_hdr_chan;
    td_s32 is_tunneled_path;
    td_u8  android_test;
    vfmw_cmp v_cmp_mode;
    td_u32 y_comp_ratio;
    td_u32 uv_comp_ratio;
    td_s32 support_all_p;
    td_u32 poc_threshold;
    td_u8 module_lowly_enable;
    td_s32 chan_lowly_enable;
    td_s32 dec_lowly_enable;
    td_s8  uvmos_enable;
    td_bool sample_frd_enable;
    td_bool frm_fence_enable;
    td_s32 crc_mode;
    vfmw_source source;
    td_u32 usd_pool_size;
    vfmw_frm_alloc_type alloc_type;
    td_u32 extra_frame_store_num;
    td_s32 linear_no_gap_en;

    td_s32 max_raw_packet_num;
    td_s32 max_raw_packet_size;
    td_s32 max_width;
    td_s32 max_height;
    td_u32 max_core_num;
    td_s32 max_slice_num;
    td_s32 max_vps_num;
    td_s32 max_sps_num;
    td_s32 max_pps_num;
    td_s32 max_ref_frame_num;

    td_bool is_max_buf_size_en;
    td_u32 max_dec_width;
    td_u32 max_dec_height;

    td_bool dv_compatible;
    vfmw_dv_codec_cap dv_capacity;
    vfmw_dv_ves_imp   ves_imp;
    vfmw_dv_layer_imp layer_imp;
    vfmw_frm_layout frame_layout;
    vfmw_output_layer frame_output_layer;
    vfmw_air_dec air_dec;
    td_u8 uv_order_enable;
    td_u64 private_data;
    vfmw_outer_frm_mode outer_frm_mode;
} vfmw_chan_cfg;

typedef struct {
    td_mem_handle_t fd;
    td_u64 dma_buf;
    UADDR  phy_addr;
    UADDR  mmu_addr;
    td_u64 vir_addr;
    td_u32 length;
    vfmw_mem_mode mode;
    td_u32 unid;
    td_handle ssm_handle;
    td_handle vdec_handle;
} vfmw_mem_desc;

typedef struct {
    td_s32 keep_bs;        /* keep bs in the scd buffer */
    td_s32 keep_sps_pps;   /* keep global info in ctx for seek reset, default 0 */
    td_s32 keep_fsp;       /* keep fsp info for seek reset, default 0 */
    td_s32 clear_report_info;    /* clear report info in ctx for seek reset, default 0 */
} vfmw_chan_reset_option;

typedef struct {
    td_u8  hal_disable;
    td_u8  seg_bypass_enbale;
    td_u32 print_word;
    UADDR  glb_shr_base_addr;
    td_u32 glb_shr_mem_length;
    UADDR  chan_option_addr;
    UADDR  proc_addr;
    UADDR  common_context;
} vfmw_share_data;

typedef struct {
    UADDR  img_phy_addr;
    td_u64 img_vir_addr;
    UADDR  evt_phy_addr;
    td_u64 evt_vir_addr;
    UADDR  stm_phy_addr;
    td_u64 stm_vir_addr;
    UADDR  sta_phy_addr;
    td_u64 sta_vir_addr;
    UADDR  cfg_phy_addr;
    td_u64 cfg_vir_addr;
    vfmw_mem_desc chan_shr_mem_desc;
} vfmw_shr_chn_mem;

typedef struct {
    td_s32 chan_id;
    td_handle vdec_handle;
    td_handle ssm_handle;
    vfmw_adpt_type adpt_type;
    td_s32 seg_buf_size;
    td_s32 max_slice_num;
    td_s32 max_vps_num;
    td_s32 max_sps_num;
    td_s32 max_pps_num;
    td_s32 max_ref_frame_num;
    td_s32 is_sec_mode;
    vfmw_vid_std  vid_std;
    vfmw_shr_chn_mem tmp_chan_shr_mem;
    vfmw_mem_desc chan_ctx_mem;
    vfmw_mem_desc dev_hal_mem;
} vfmw_chan_option;

typedef enum {
    VFMW_USD_TYPE_UNKNOWN,
    VFMW_USD_TYPE_DVB1_CC,
    VFMW_USD_TYPE_DVB1_BAR,
    VFMW_USD_TYPE_AFD,
    VFMW_USD_TYPE_MAX
} vfmw_usd_type;

typedef struct {
    vfmw_usd_type type;
    vfmw_usd_src_type position;
    td_s64 pts;
    td_u32 seq_cnt;
    td_u32 seq_frm_cnt;
    td_u32 user_data_len;
    td_handle fd;
    td_u64 usd_offset;
    td_bool is_over_flow;
    td_bool is_top_field_first;
} vfmw_usrdat_ext;

typedef struct {
    vfmw_norm_type norm_type;
    td_u32 width;
    td_u32 height;
    td_u32 frm_rate;
    td_bool progressive;
} vfmw_norm_chg_param;

typedef struct {
    td_bool is_frame_valid;
    td_u32 width;
    td_u32 height;
    td_u32 bit_depth;
    td_u32 luma_stride;
    td_u32 chrom_stride;
    UADDR  phy_addr;
    td_u32 line_num_size;
    td_u32 chrom_offset;
    td_u32 line_num_offset;
    td_s64 pts;
    td_u64 disp_time;
    vfmw_yuv_fmt fmt;
    td_mem_handle_t mem_handle;
    td_mem_size_t mem_size;
} vfmw_usrdec_frame_desc;

typedef struct {
    td_u32 fps_integer;        /* integral part of the frame rate (in frame/s) */
    td_u32 fps_decimal;        /* fractional part (calculated to three decimal places) of the frame rate (in frame/s) */
} vfmw_pts_frmrate_value;

typedef struct {
    vfmw_pts_framrate_type en_frm_rate_type;  /* source of frame rate */
    vfmw_pts_frmrate_value st_set_frm_rate;   /* setting frame rate */
} vfmw_pts_frmrate;

typedef struct {
    td_bool is_support_air_dec;
    td_u32 air_version;
} vfmw_air_cap;

typedef struct {
    td_s32 max_chan_num;
    td_s32 max_bit_rate;
    td_u32 frm_layout_cap;
    td_u32 bit_depth_cap;
    td_u16 max_width[VFMW_STD_MAX];
    td_u16 max_height[VFMW_STD_MAX];
    td_u16 max_frm_rate[VFMW_STD_MAX]; /* base fhd */
    vfmw_vid_std supported_std[VFMW_STD_MAX];
    vfmw_air_cap air_dec_cap;
} vfmw_cap;

typedef struct {
    td_s64 attached_handle;
    td_u32 attached_offset;
    UADDR  attached_phy_addr;
    td_u64 attached_vir_addr;
    td_u32 attached_size;

    td_s64 hfbc_handle;
    td_u64 hfbc_vir_addr;
} vfmw_image_attached;

typedef struct {
    vfmw_vid_std std;
    td_u32  image_width;
    td_u32  image_height;
    td_u32  disp_height;
    td_u32  disp_width;
    td_u32  disp_center_x;
    td_u32  disp_center_y;
    td_u32  aspect_width;
    td_u32  aspect_height;
    td_u32  scan_type;
    td_u32  video_format;
    td_u32  frame_rate;
    td_u32  bit_rate;
    td_u32  sub_video_standard;
    td_u32  profile;
    td_u32  level;
    td_u32  bit_depth;
    vfmw_color_desc color_info;
    td_s32  total_frame_num;
    td_s32  error_frame_num;
    td_s32  buffered_stream_size;   /* the un-decoded stream seg size produced by SCD */
    td_s32  buffered_stream_num;    /* the un-decoded stream seg num  produced by SCD */
    UADDR   chan_mem_address;
    td_s32  chan_mem_size;
    td_s32  total_fstore_num;
    td_s32  total_pmv_num;
    td_s32  voque_detail;
    td_u32  stream_not_enough;
    td_u32  wait_disp_frame_num;
    td_u32  mpeg4_shorthead;      /* mpeg4 short head info, 1: sh 2:not sh  0:not mpeg4 stream */
    td_u32  total_disp_frame_num; /* total display num ( plus extra_disp ) */
    td_u32  is_field_flg;
    td_bool is_end_of_stm;
    td_u32  total_stm_buf_size;
    td_u32  free_stm_buf_size;
    td_u32  used_stm_buf_size;
    td_u32  undecode_stm_size;
    td_u32  undecode_stm_num;
    td_u32  pvr_buf_num;
    td_u32  dec_frm_rate;
    td_u32  stream_frm_rate;
    td_s32  vpp_receive_try;
    td_s32  vpp_receive_ok;
    td_s32  vpp_release_try;
    td_s32  vpp_release_ok;

    td_u32  dec_task_state;
    td_bool dec_is_task_run;
    td_u32 dec_mode_attr;
    td_u32 dec_dps_num;
    td_u32 dec_load;

    td_bool fsp_has_free_dec;
    td_bool fsp_has_free_pmv;
    td_bool fsp_has_free_attach;

    td_bool dps_has_make_node;

    td_u32 syn_get_seg_try_cnt;
    td_u32 syn_get_seg_succ_cnt;
    td_u32 syn_spec_invalid;
    td_u32 syn_spec_value;

    td_bool stm_raw_new_flag;
    td_bool stm_seg_full;
    td_u32 raw_recv_try;
    td_u32 raw_recv_ok;
    td_u32 raw_fresh_num;
    td_u32 seg_recv_try;
    td_u32 seg_recv_ok;
    td_u32 seg_fresh_num;
    td_u32 cur_tick;
    td_u32 eos_flag;
} vfmw_chan_info;

typedef enum {
    CHIP_WUDANGSTICK = 1,
    CHIP_KONGTONGV100,
    CHIP_SHAOLINGUN,
    CHIP_HENGSHANV200,
    CHIP_MAX,
} chip_info;

typedef enum {
    VFMW_BOARD_ES = 1,
    VFMW_BOARD_CS,
    VFMW_BOARD_EC,
    VFMW_BOARD_MAX,
} board_info;

typedef struct {
    td_u8 is_secure;
    vfmw_adpt_type adpt_type;
    vfmw_mem_desc ext_hal_mem;
    fun_vfmw_event_report fn_callback;
    vfmw_share_data share_data;
} vfmw_init_param;

typedef struct {
    td_u32 image_id;
    td_u32 image_id_1;
    td_s32 gop_num;
    td_u32 frm_type;
    td_s32 top_field_frame_format;
    td_s32 bot_field_frame_format;
    td_s32 field_flag;
    td_s32 chan_id;
    td_s32 shr_index;
    td_u32 user_tag;
    td_u32 pix_sum_luma;
    td_u32 pix_sum_chroma;
    td_u32 cycle;
} vfmw_private_frm_info;

typedef struct {
    td_u32 avg_qp;
    td_u32 frame_stream_size;
    td_u32 max_mv;
    td_u32 min_mv;
    td_u32 avg_mv;
    td_u32 li_max_mv;
    td_u32 li_min_mv;
    td_u32 li_avg_mv;
    td_u32 skip_ratio;
    vfmw_frame_type frm_type;
    vfmw_vid_std vid_std;
} vfmw_image_uvmos;

typedef struct {
    vfmw_image_uvmos uvmos_info;
    td_u32 usd_num;
} vfmw_ext_frm_info;

typedef struct {
    td_u32 reserve;
} vfmw_begin_pic_param;

typedef struct {
    td_u32 id;
    td_u32 type;
    td_u32 len;
    td_u32 offset;
    td_s64 handle;
    td_u64 dma_buf;
    td_u64 phy_addr;
    td_u64 vir_addr;
} vfmw_va_buffer;

typedef struct {
    td_u32 buf_num;
    td_u32 ref_num;
    vfmw_va_buffer buf[1];
    vfmw_va_buffer cur_frm;
    vfmw_va_buffer ref_frm[VFMW_MAX_FRM_NUM];
} vfmw_render_pic_param;

typedef struct {
    td_u32 reserve;
} vfmw_end_pic_param;

/* vfmw external interface definition */
typedef struct {
    fun_vfmw_mod_init   fn_vfmw_mod_init;
    fun_vfmw_mod_exit   fn_vfmw_mod_exit;
    fun_vfmw_init       fn_vfmw_init;
    fun_vfmw_exit       fn_vfmw_exit;
    fun_vfmw_suspend    fn_vfmw_suspend;
    fun_vfmw_resume     fn_vfmw_resume;
    fun_vfmw_get_frame  fn_vfmw_get_frame;
    fun_vfmw_rels_frame fn_vfmw_rels_frame;
    fun_vfmw_control    fn_vfmw_control;
} vfmw_export_fun;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
