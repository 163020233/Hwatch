/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_BASE_H__
#define __SYN_BASE_H__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "dps.h"
#include "fsp.h"
#include "stm_intf.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

#define SYN_OK  0
#define SYN_ERR 1

#define SYN_DEBUG_CNT 20

#define VFMW_STREAM_END_CODE1 0x48535049
#define VFMW_STREAM_END_CODE2 0x43454E44

#define LAST_ALREADY_OUT 1
#define LAST_OUTPUT_OK   0
#define LAST_OUTPUT_FAIL (-1)

#define FRAME 0

#define reverse_endian32(x)       \
    (((x) << 24) |  \
    (((x)&0x0000ff00) << 8) |  \
    (((x)&0x00ff0000) >> 8) |  \
    (((x) >> 24) & 0x000000ff))

typedef enum {
    SYN_CHAN_NONE,
    SYN_CHAN_CREATE_DONE,
    SYN_CHAN_CONFIG_DONE,
    SYN_CHAN_START_DONE,
    SYN_CHAN_STOP_DONE,
    SYN_CHAN_RESET_DONE,
    SYN_CHAN_BUTT,
} syn_chan_state;

typedef enum {
    SYN_PARSE_BEGIN = 100,
    SYN_PARSE_CURR_FRMDONE,
    SYN_PARSE_PRE_FRMDONE,
    SYN_PARSE_PRE_FRMERR,
    SYN_PARSE_NOT_FRMDONE,
    SYN_PARSE_RESOURCE_INVALID,
    SYN_PARSE_ERR,
    SYN_PARSE_END
} syn_parse_state;

typedef enum {
    TRY_IDLE = 0,
    TRY_HALF,
    TRY_NORM,
    TRY_SLICE,
} sed_try_state;

typedef struct {
    td_u8 stop_record;
    td_u8 allow_b_before_p;
    td_u8 sec_flag;
    td_u8 dv_dual_layers;
    vfmw_adpt_type adpt_type;
    td_u8 priority;
    td_s32 vid_std;
    td_s32 ref_err_thr;
    td_s32 out_err_thr;
    td_s32 dec_mode; /* 0 IPB, 1 IP, 2 I */
    td_s32 crc_mode;
    td_s32 output_order; /* 0:display order, 1:decode order */
    UADDR syn_mem_addr;
    td_s32 syn_mem_size;
    td_u32 sed_only_en;
    td_u32 slc_ldy_en;
    td_s32 lowdly_en;
    td_u32 is_adaptive;
    td_s32 max_vps_num;
    td_s32 max_sps_num;
    td_s32 max_pps_num;
    td_s32 max_ref_num;
    td_s32 max_slice_num;
    td_s32 max_width;
    td_s32 max_height;
    td_bool is_max_buf_size_en;
    td_u32 max_dec_width;
    td_u32 max_dec_height;
    td_u32 dynamic_fs_en;
    td_s32 support_b_frame; /* if support B frame. 1: yes, 0: no */
    td_s32 support_all_p_frame;
    td_u32 new_pic_flag;
    td_s64 pts;
    td_u64 user_tag;
    td_u64 disp_time;
    td_u32 disp_enable_flag;
    td_u32 disp_frame_distance;
    td_u32 distance_before_1st_frame;
    td_u32 gop_num;
    td_s64 next_pts;
    td_u64 next_user_tag;
    td_u64 next_disp_time;
    td_u32 next_disp_enable_flag;
    td_u32 next_disp_frame_distance;
    td_u32 next_distance_before_1st_frame;
    td_u32 next_gop_num;
    td_s32 err_frm_num;
    td_s32 fast_forward_backward_speed;
    td_s32 more_gap_en;
    td_s32 wait_fs_flag;
    td_s32 bit_depth;
    td_s32 bit_depth_changed;
    td_s8  is_hdr_chan;
    td_u32 calc_perf_slc_num;

    vfmw_control_info pvr_info;
    vfmw_output_layer frame_output_layer;
    vfmw_air_dec_version air_version;
    vfmw_air_dec air_dec_attr;
} syn_cfg_info;

typedef struct {
    td_s64 pts;
    td_u64 user_tag;
    td_u64 disp_time;
    td_u32 disp_enable_flag;
    td_u32 disp_frame_distance;
    td_u32 distance_before_1st_frame;
    td_u32 gop_num;
    td_s64 next_pts;
    td_u64 next_user_tag;
    td_u64 next_disp_time;
    td_u32 next_disp_enable_flag;
    td_u32 next_disp_frame_distance;
    td_u32 next_distance_before_1st_frame;
    td_u32 next_gop_num;
    td_u8 dv_dual_layers;

    /* time point */
    td_u32 que_raw_time;
    td_u32 deq_raw_time;
    td_u32 que_seg_time;
    td_u32 deq_seg_time;
} syn_stm_info;

typedef struct {
    td_void *mem_addr;
    td_u32 mem_size;
} syn_mem_info;

typedef struct {
    td_s32 (*read_seg)(td_s32 chan_id, stm_seg_info *seg);
} syn_ext_intf;

typedef struct {
    vfmw_vid_std std;
    td_u32 profile;
    td_u32 level;
    td_u32 get_seg_try_cnt;
    td_u32 get_seg_succ_cnt;
    spec_invalid_type spec_invalid;
    td_u32 spec_value;
} syn_syntax_info;

td_void syn_qsort(td_void *base, td_u32 num, td_u32 width, td_s32(*comp)(const td_void *, const td_void *));
td_void syn_set_aspect_ratio(vfmw_image *img, vfmw_vdec_dar code);
td_s32 syn_is_pts_rewind(td_s32 chan_id, td_s64 current_pts, td_s64 min_pts);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
