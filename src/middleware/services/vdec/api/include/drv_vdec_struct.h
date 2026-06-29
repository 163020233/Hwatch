/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-30
 */

#ifndef __EXT_VDEC_TYPE_H__
#define __EXT_VDEC_TYPE_H__

#include "td_type.h"
#include "soc_module.h"
#include "soc_vdec_type.h"
#include "drv_video_ext.h"

/* CNcomment: 强制约束，严禁include其它外部头文件 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* --------------------------------------- EXTERNAL TYPE DEFINE --------------------------------------- */
/*
 * CNcomment: VDEC对外类型定义，内部类型不允许在本头文件中定义
 * 供用户态内核态共用，需考虑32&64 bit兼容
 */
#define EXT_VDEC_MAX_USD_NUM      4

#define VDEC_DFT_MAX_RAW_NUM 1024

#define VDEC_MODULE_NAME   "VDEC"

#undef  LOG_MODULE_ID
#define LOG_MODULE_ID     SOC_ID_VDEC

#define INTF

#define vdec_check(cond) \
do { \
    if (!(cond)) { \
        vdec_print_err("check '%s' error\n", #cond); \
        return; \
    } \
} while (0)

#define vdec_check_ret(cond) \
do { \
    if (!(cond)) { \
        vdec_print_err("check '%s' error\n", #cond); \
        return EXT_FAILURE; \
    } \
} while (0)

#define vdec_check_ret_val(cond, val) \
do { \
    if (!(cond)) { \
        vdec_print_err("check '%s' error\n", #cond); \
        return (val); \
    } \
} while (0)

#define vdec_check_func_ret(func) \
do { \
    td_s32 ret = (td_s32)(func); \
    if (ret != EXT_SUCCESS) { \
        vdec_print_err("check %s error\n", #func); \
        return EXT_FAILURE; \
    } \
} while (0)

#define vdec_check_chan_id(handle) \
do { \
    if ((handle) < 0 || (handle) >= VDEC_MAX_CHAN_NUM) { \
        vdec_print_err("handle %d invalid.\n", handle); \
        return EXT_FAILURE; \
    } \
} while (0)

#define vdec_check_goto(cond, label) \
    do { \
        if (!(cond)) { \
            vdec_print_err("check '%s' error\n", #cond); \
            goto label; \
        } \
    } while (0)

#define VDEC_INVALID_HANDLE   (EXT_VDEC_INVALID_HANDLE)

/*
 * define of TD_HANDLE :
 * bit31                                                                bit0
 *   |<----- 8bit ----->|<----- 8bit ----->|<---------- 16bit ---------->|
 *   |-------------------------------------------------------------------|
 *   |     soc_mod_id    | mod defined data |           chan_id           |
 *   |-------------------------------------------------------------------|
 * mod defined data: private data define by each module(for example: sub-mod id), usually, set to 0.
 */
#define vdec_handle_init(mod, private_data, chan_id) \
    (td_handle)((((mod) & 0xff) << 24) | ((((private_data) & 0xff) << 16)) | (((chan_id) & 0xffff)))

#define vdec_make_handle(chan_id) vdec_handle_init(SOC_ID_VDEC, 0, chan_id)
#define vdec_get_module_id_by_handle(handle) (((handle) >> 24) & 0xff)
#define vdec_get_private_data_by_handle(handle) (((handle) >> 16) & 0xff)
#define vdec_get_chan_id_by_handle(handle) (((handle)) & 0xffff)

#define vdec_pos() vdec_print_err("location\n")
#define vdec_uint64_ptr(ptr) ((td_u8 *)(td_uintptr_t)(ptr))
#define vdec_ptr_uint64(ptr) ((td_u64)(td_uintptr_t)(ptr))

#define VDEC_INVALID_PTS   ((td_s64)(-1))

typedef enum {
    EXT_VDEC_DPB_MODE_NORMAL,  /* Normal DPB procedure */
    EXT_VDEC_DPB_MODE_SIMPLE,  /* Simple DPB procedure */
    EXT_VDEC_DPB_MODE_SKYPLAY, /* Skyplay DPB procedure, allow dpb full */
    EXT_VDEC_DPB_MODE_MAX
} ext_vdec_dpb_mode;

typedef enum {
    EXT_VDEC_EVENT_MODE_ACTIVE,  /* Acitve report event */
    EXT_VDEC_EVENT_MODE_PASSIVE, /* Passive listen event */
    EXT_VDEC_EVENT_MODE_MAX
} ext_vdec_evt_mode;

typedef enum {
    EXT_VDEC_STM_INTERNAL, /* vdec input buffer allocated internally */
    EXT_VDEC_STM_MAX
} ext_vdec_stm_buf_mode;

typedef enum {
    EXT_VDEC_BUF_EMPTY,  /* Buffer is empty */
    EXT_VDEC_BUF_FULL,   /* Buffer is filled with data */
    EXT_VDEC_BUF_IN_USE, /* Buffer is in using */
    EXT_VDEC_BUF_MAX
} ext_vdec_buf_state;

typedef enum {
    EXT_VDEC_USD_TYPE_UNKNOWN = 0x0,  /* Unknown type */
    EXT_VDEC_USD_TYPE_DVB1_CC = 0x1,  /* Closed caption data */
    EXT_VDEC_USD_TYPE_DVB1_BAR = 0x2, /* Bar data */
    EXT_VDEC_USD_TYPE_AFD = 0x10000,  /* Active format description */
    EXT_VDEC_USD_TYPE_MAX
} ext_vdec_usd_type;

typedef enum {
    EXT_VDEC_USD_POS_UNKNOWN,    /* Unknown */
    EXT_VDEC_USD_POS_MPEG2_SEQ,  /* MPEG2 sequences */
    EXT_VDEC_USD_POS_MPEG2_GOP,  /* MPEG2 GOP */
    EXT_VDEC_USD_POS_MPEG2_FRM,  /* MPEG2 frames */
    EXT_VDEC_USD_POS_MPEG4_VSOS, /* MPEG4 sequences of visible objects */
    EXT_VDEC_USD_POS_MPEG4_VSO,  /* MPEG4 visible objects */
    EXT_VDEC_USD_POS_MPEG4_VOL,  /* MPEG4 video object layer */
    EXT_VDEC_USD_POS_MPEG4_GOP,  /* MPEG4 GOP */
    EXT_VDEC_USD_POS_H264_REG,   /* H264  regestered usd */
    EXT_VDEC_USD_POS_H264_UNREG, /* H264  unregestered usd */
    EXT_VDEC_USD_POS_AVS_SEQ,
    EXT_VDEC_USD_POS_AVS_FRM,
    EXT_VDEC_USD_POS_MAX
} ext_vdec_usd_pos;

typedef enum {
    EXT_VDEC_SPEED_INALID = 0,
    EXT_VDEC_SPEED_FORWARD,  /* Fast speed forward */
    EXT_VDEC_SPEED_BACKWARD, /* Fast speed backward */
    EXT_VDEC_SPEED_MAX
} ext_vdec_speed_direct;

typedef enum {
    EXT_VDEC_HDR_SS_DL_VES, /* Single Stream Dual Layer */   /* CNcomment: 单流双层格式 */
    EXT_VDEC_HDR_DS_DL_VES, /* Dual Stream Dual Layer */     /* CNcomment: 双流双层格式 */
    EXT_VDEC_HDR_SS_SL_VES, /* Single Stream Single Layer */ /* CNcomment: 单流单层格式 */
    EXT_VDEC_HDR_STM_TYPE_MAX
} ext_vdec_dv_hdr_stm_type;

typedef enum {
    EXT_VDEC_HDR_CHAN_NONE, /* CNcomment: 非DV HDR解码通道 */
    EXT_VDEC_HDR_CHAN_BL,   /* CNcomment: BL解码通道 */
    EXT_VDEC_HDR_CHAN_EL,   /* CNcomment: EL解码通道 */
    EXT_VDEC_HDR_CHAN_MAX
} ext_vdec_dv_hdr_chan_type;

typedef enum {
    /* Semi-Planner */
    EXT_VDEC_YUV_FMT_420SP, /* Spatial sampling format 4:2:0 */
    /* Semi-Planner Force Bit Depth */
    EXT_VDEC_YUV_FMT_420SP_FIX_8BIT,     /* Spatial sampling format 4:2:0, fix 8bit */
    EXT_VDEC_YUV_FMT_420SP_EXPAND_16BIT, /* Spatial sampling format 4:2:0, expand 16bit */
    /* Semi-Planner Compress */
    EXT_VDEC_YUV_FMT_420SP_CMP,  /* Spatial sampling format 4:2:0, private compress */
    EXT_VDEC_YUV_FMT_420SP_AFBC, /* Spatial sampling format 4:2:0, AFBC compress */
    /* Semi-Planner Tile */
    EXT_VDEC_YUV_FMT_420SP_TILE, /* Spatial sampling format TILE 4:2:0 */
    EXT_VDEC_YUV_FMT_MAX
} ext_vdec_yuv_format;

typedef enum {
    EXT_VDEC_FRM_INFO_DRV_VIDEO, /* MSP DRV define frame type */
    EXT_VDEC_FRM_INFO_UNF_VIDEO, /* MSP UNF define frame type */
    EXT_VDEC_FRM_INFO_MAX
} ext_vdec_frm_info_type;

typedef enum {
    EXT_VDEC_CMD_START_RESERVE = 0x00000000,

    EXT_VDEC_CMD_EXT_START = 0x20000000,
    EXT_VDEC_CMD_EXT_END = 0x30000000,

    EXT_VDEC_CMD_MAX
} ext_vdec_cmd_id;

typedef enum {
    EXT_VDEC_PARAM_RESERVE = 0x00000000,

    /* set low delay enable/disable. param: td_bool* */
    EXT_VDEC_PARAM_LOW_DELAY,
    /* set/get frame rate. param: ext_vdec_frm_rate* */
    EXT_VDEC_PARAM_FRAME_RATE,
    /* get channel feature. param: ext_vdec_feature* */
    EXT_VDEC_PARAM_FEATURE,
    /* get channel status. param: ext_vdec_status. */
    EXT_VDEC_PARAM_STATUS,

    EXT_VDEC_PARAM_MAX
} ext_vdec_param_id;

typedef enum {
    EXT_VDEC_CMP_ADJUST = 0,
    EXT_VDEC_CMP_OFF,
    EXT_VDEC_CMP_ON,
    EXT_VDEC_CMP_MAX
} ext_vdec_cmp_mode;

typedef enum {
    EXT_VDEC_NORM_PAL = 0,
    EXT_VDEC_NORM_NTSC,
    EXT_VDEC_NORM_MAX
} ext_vdec_norm_type;

typedef enum {
    EXT_VDEC_SUB_STD_UNKNOWN = 0,
    EXT_VDEC_SUB_STD_MPEG2_MPEG1,
    EXT_VDEC_SUB_STD_MPEG4_SHV,
    EXT_VDEC_SUB_STD_MPEG4_XVID,
    EXT_VDEC_SUB_STD_MPEG4_DIVX,
    EXT_VDEC_SUB_STD_MAX
} ext_vdec_sub_std;

typedef enum {
    EXT_VDEC_BROADCAST_DVB,
    EXT_VDEC_BROADCAST_DIRECTV,
    EXT_VDEC_BROADCAST_ATSC,
    EXT_VDEC_BROADCAST_DVD,
    EXT_VDEC_BROADCAST_ARIB,
    EXT_VDEC_BROADCAST_MAX
} ext_vdec_broadcast;

/*
 * CNcomment: 为兼容32&64 bit用户态和内核态
 * 指针类型统一使用ext_vdec_pxxx，保证占用长度一致
 */
typedef union {
    td_void *pointer; /* pointer type */
    td_u64 integer;   /* integer type */
} ext_vdec_pvoid;

typedef union {
    td_char *pointer;
    td_u64 integer;
} ext_vdec_pchar;

typedef union {
    td_u8 *pointer;
    td_u64 integer;
} ext_vdec_pu8;

typedef union {
    td_s8 *pointer;
    td_u64 integer;
} ext_vdec_ps8;

typedef union {
    td_u32 *pointer;
    td_u64 integer;
} ext_vdec_pu32;

typedef union {
    td_s32 *pointer;
    td_u64 integer;
} ext_vdec_ps32;

typedef union {
    td_u64 *pointer;
    td_u64 integer;
} ext_vdec_pu64;

typedef union {
    td_s64 *pointer;
    td_u64 integer;
} ext_vdec_ps64;

typedef struct {
    td_bool is_secure;
    td_u32 buffer_size;
} ext_vdec_init_stm;

typedef struct {
    ext_vdec_buf_state state;
    ext_vdec_frame_buf ext_buf;
} ext_vdec_unbind_frm_buf;

typedef struct {
    ext_vdec_broadcast broadcast;
    ext_vdec_usd_type type;
    ext_vdec_usd_pos position;
    td_s64 pts;
    td_u32 seq_cnt;
    td_u32 seq_frm_cnt;
    td_u32 user_data_len;
    td_handle fd;
    td_u64 usd_offset;
    td_bool is_over_flow;
    td_bool is_top_field_first;
} ext_vdec_user_data;

typedef struct {
    ext_vdec_pchar lib_name;
    td_u32 name_len;
} ext_vdec_ext_decoder;

typedef struct {
    td_u32 gap;
    td_s64 expect_pts;
    td_s64 actual_pts;
} ext_vdec_drop_stm;

typedef struct {
    td_bool is_adv_profile;
    td_u32 codec_version;
} ext_vdec_std_ext_vc1;

typedef struct {
    td_bool need_reverse;
    td_u32 disp_width;
    td_u32 disp_height;
} ext_vdec_std_ext_vp6;

typedef union {
    ext_vdec_std_ext_vc1 vc1;
    ext_vdec_std_ext_vp6 vp6;
} ext_vdec_std_ext;

typedef struct {
    ext_vdec_standard standard;
    ext_vdec_std_ext standard_ext;
    td_bool support;
} ext_vdec_check_random;

typedef struct {
    ext_vdec_speed_direct direct;
    td_u32 speed_integer;
    td_u32 speed_decimal;
} ext_vdec_speed;

typedef struct {
    td_bool is_idr_flag;          /* Whether is IDR frame */
    td_bool is_b_frm_ref;         /* Whether B frame is refer frame */
    td_bool is_continuous;        /* Whether frame is continuous */
    td_bool is_backward_optimize; /* Whether backward optimize enable */
    td_bool is_display_optimize;  /* Whether display optimize enable */
} ext_vdec_pvr_info;

typedef struct {
    td_bool is_dv_hdr_enable;           /* Is Dolby HDR enable or not */
    td_bool is_dv_frm_compatible;       /* Is Dolby frame compatible or not */
    ext_vdec_standard el_standard;            /* EL chan video type */
    ext_vdec_dv_hdr_chan_type chan_type; /* Chan type, BL or EL */
    ext_vdec_dv_hdr_stm_type stm_type;   /* VES format */
} ext_vdec_hdr_attr;

typedef struct {
    td_s64 pts;
} ext_vdec_start_pts;

typedef struct {
    td_s32 tag_value;
} ext_vdec_src_tag;

typedef struct {
    ext_vdec_yuv_format yuv_fmt;
    td_bool is_u_first;
} ext_vdec_out_format;

typedef struct {
    td_u32 reserve;
} ext_vdec_event_frame_param;

typedef struct {
    ext_vdec_norm_type norm_type;
    td_u32 width;
    td_u32 height;
    td_u32 frm_rate;
    td_bool progressive;
} ext_vdec_norm_param;

typedef struct {
    td_s64 timestamp;
    ext_vdec_error_type type;
} ext_vdec_dec_err_param;

typedef struct {
    ext_vdec_standard standard;
    td_bool user_dec;
    td_bool support;
} ext_vdec_opt_is_std_support;

typedef struct {
    td_u32 frame_stream_size;
    td_u32 avg_qp;
    td_u32 max_mv;
    td_u32 min_mv;
    td_u32 avg_mv;
    td_u32 skip_ratio;
    ext_vdec_frame_type frm_type;
} ext_vdec_uvmos;

typedef struct {
    ext_vdec_uvmos uvmos_info;
    td_u32 usd_num;
} ext_vdec_ext_frm_info;

typedef struct {
    td_mem_handle_t handle;
    td_u32 size;
} ext_vdec_bind_usd_param;

typedef enum {
    VDEC_YUV_NONE = 0,
    VDEC_YUV_SP400,
    VDEC_YUV_SP411,
    VDEC_YUV_SP420,
    VDEC_YUV_SP422_1X2,
    VDEC_YUV_SP422_2X1,
    VDEC_YUV_SP444,
    VDEC_YUV_P400,
    VDEC_YUV_P411,
    VDEC_YUV_P420,
    VDEC_YUV_P422_1X2,
    VDEC_YUV_P422_2X1,
    VDEC_YUV_P444,
    VDEC_YUV_P410,
    VDEC_YUV_MAX
} vdec_yuv_fmt;

typedef struct {
    td_bool is_frame_valid;
    td_u32 width;
    td_u32 height;
    td_u32 bit_depth;
    td_u32 luma_stride;
    td_u32 chrom_stride;
    td_u32 phy_addr;
    td_u32 line_num_size;
    td_u32 chrom_offset;
    td_u32 line_num_offset;
    td_s64 pts;
    td_u64 disp_time;
    td_mem_handle_t mem_handle;
    td_mem_size_t mem_size;
    vdec_yuv_fmt fmt;
} vdec_usrdec_frame;

typedef enum {
    VDEC_PRIVATE_CMD_START = EXT_VDEC_CMD_EXT_START,
    VDEC_PRIVATE_CMD_GET_STREAM,
    VDEC_PRIVATE_CMD_PUT_STREAM,
    VDEC_PRIVATE_CMD_GET_FRAME,
    VDEC_PRIVATE_CMD_PUT_FRAME,
    VDEC_PRIVATE_CMD_BIND_ES_USR_ADDR,
    VDEC_PRIVATE_CMD_CHK_FRM_STATE,
    VDEC_PRIVATE_CMD_END = EXT_VDEC_CMD_EXT_END,
} vdec_private_cmd;

typedef td_s32 (*fn_drv_vdec_callback)(td_handle handle, ext_vdec_event_id evt_id,
    const td_void *param, td_u32 size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
