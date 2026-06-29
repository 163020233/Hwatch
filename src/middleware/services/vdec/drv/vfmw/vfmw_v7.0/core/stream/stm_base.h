/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_BASE_H
#define SM_BASE_H

#include "vfmw.h"
#include "dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define STM_OK  0
#define STM_ERR 1
#define STM_STOP    2
#define STM_DESTORY 3
#define STM_START   4
#define STM_BREAK   5

#define STM_SC_LEN 3

#define SEG_CUT_NORMAL 0
#define SEG_CUT_REPEAT 1
#define SEG_CUT_HALF   2

#define STM_DEBUG_CNT 8

typedef enum {
    STM_SEG_INVALID = 0,
    STM_SEG_SLICE = 1,
    STM_SEG_PPS = 2,
    STM_SEG_SPS = 3,
    STM_SEG_VPS = 4,
    STM_SEG_SEI = 5,
    STM_SEG_EOP = 6,
    STM_SEG_EOS = 7,
    STM_SEG_END,
} stm_seg_type;

typedef struct {
    td_u32 short_stream_flag : 1;
    td_u32 slice_flag : 1;
    td_u32 is_short_header : 1;
    td_u32 start_code_bit_offset : 4;
    td_u32 start_code_len : 4;
    td_u32 reserved : 21;
} mp4_seg_msg;

typedef struct {
    UADDR cur_seg_addr;
    UADDR next_seg_addr;
} stm_seg_addr;

typedef struct {
    td_s8 seg_state;
    td_s8 cut_result;
    td_u8 is_last_seg;
    td_u8 is_stream_end;
    td_u8 dv_dual_layer;
    td_u8 dsp_check_pic_over_flag;
    td_u8 ref_cnt;
    td_u8 rls_cnt;
    td_u8 *vir_addr;
    UADDR phy_addr;
    td_u32 length;
    td_u32 raw_idx;
    td_u32 raw_len;
    td_s32 stream_id;
    td_s64 pts;
    td_u64 timestamp;
    td_u64 user_tag;
    td_u64 disp_time;
    td_u32 disp_enable_flag;
    td_u32 disp_frame_distance;
    td_u32 distance_beforce_first_frame;
    td_u32 gop_num;
    mp4_seg_msg mp4_seg_msg_info;
    stm_seg_type seg_type;

    /* time point */
    td_u32 que_raw_time;
    td_u32 deq_raw_time;
    td_u32 que_seg_time;
    td_u32 deq_seg_time;
    /* for multi slice ldy */
    UADDR seg_base_phy_addr;
} stm_seg_info;

typedef struct {
    td_u8 priority;
    td_u8 is_tunnel_path;
    td_u8 is_android_test; /* android test flag, cts/vts/nts etc */
    td_u8 sec_flag;
    td_u8 half_seg_en;
    td_u8 fill_eop_en;
    td_u8 scd_bypass;
    td_u8 seg_bypass;
    td_u8 dn_msg_num;
    vfmw_vid_std vid_std;
    vfmw_std_ext std_ext;
    vfmw_adpt_type adpt_type;
    td_s32 max_raw_num;
    td_s32 max_raw_size;
} stm_cfg_info;

typedef struct {
    td_s32 is_sec;
    td_s32 seg_buf_size;
} stm_create_info;

typedef struct {
    td_u32 bit_rate;
    td_s32 fresh_seg_num;
    td_s32 fresh_seg_size;
    td_bool raw_new_flag;
    td_bool stm_seg_full;
    td_u32 raw_recv_try;
    td_u32 raw_recv_ok;
    td_u32 raw_rels_try;
    td_u32 raw_rels_ok;
    td_u32 raw_fresh_num;
    td_u32 seg_recv_try;
    td_u32 seg_recv_ok;
    td_u32 seg_fresh_num;
    td_u32 eos_flag;
} stm_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_BASE_H */
