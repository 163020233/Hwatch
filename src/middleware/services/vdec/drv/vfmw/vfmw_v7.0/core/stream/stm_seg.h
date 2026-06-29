/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef STM_SEG_H
#define STM_SEG_H

#include "vfmw.h"
#include "stm_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_SEG_ALLOW 100000

#define SEG_TOP_BLANK_SIZE (4 * 1024)
#define SEG_BTM_BLANK_SIZE (1 * 1024)
#define SEG_BUF_MIN_SIZE   (1 * 1024)

#define SEG_STATE_IDLE 0
#define SEG_STATE_NEW  1
#define SEG_STATE_USE  2

#define SEG_START_NUM     100
#define SEG_FRESH_NUM     1000
#define SEG_FRESH_NUM_OMX 16

typedef struct {
    td_s32 head;
    td_s32 tail;
    td_s32 history;
    td_s32 seg_current;
    td_u64 seg_dma_buf;
    UADDR seg_buf_phy_addr;
    td_u8 *seg_buf_vir_addr;
    td_s32 seg_buf_size;
    UADDR seg_buf_read_addr;
    UADDR seg_buf_write_addr;
    td_u8 *seg_buf_write_vir_addr;

    td_u32 pre_lsb;
    td_u32 pre_msb;
    td_s32 first_start_flag;
    UADDR seg_next_addr;

    td_s8 full_flag;
    td_s8 roll_flag;
    UADDR roll_addr;
    UADDR last_seg_addr;
    td_u32 last_seg_size;
    td_u32 insert_seg_num;

    td_s32 seg_total_size;
    td_s32 seg_fresh_size;
    td_s32 seg_fresh_num;

    td_u32 seg_num_cnt;
    td_u32 seg_size_cnt;
    td_u32 recv_try_cnt;
    td_u32 recv_ok_cnt;
    td_u32 recv_fail_cnt;
    td_u32 rels_try_cnt;
    td_u32 rels_ok_cnt;

    td_u32 mpeg4_mode_flag;
    td_u32 last_h263_num;
    td_u32 is_counting;
    td_u32 block_seg_num;
    td_u32 block_time_in_ms;
    td_u32 last_block_time;
    td_s64 last_pts;
    stm_seg_info last_seg;
    stm_seg_info seg[MAX_SEG_NUM];
    td_s32 seg_max_num;
} seg_list;

td_s32 stm_seg_init(td_s32 chan_id, td_s32 opt);
td_s32 stm_seg_insert(td_s32 chan_id, const stm_seg_info *seg);
td_s32 stm_seg_receive(td_s32 chan_id, stm_seg_info *seg);
td_s32 stm_seg_release(td_s32 chan_id, td_s32 stream_id);
td_s32 stm_seg_release_first(td_s32 chan_id);
td_s32 stm_seg_config_buf(td_s32 chan_id, const vfmw_mem_desc *mem);
td_s32 stm_seg_get_free_num(td_s32 chan_id);
td_s32 stm_seg_get_free_size(td_s32 chan_id);
td_s32 stm_seg_get_fresh_num(td_s32 chan_id);
td_s32 stm_seg_get_fresh_size(td_s32 chan_id);
td_u32 stm_seg_get_min_buf_size(td_void);
td_s32 stm_seg_raw_to_seg(td_s32 chan_id, stm_seg_info *seg);
td_s32 stm_seg_check(td_s32 chan_id);
td_bool stm_seg_has_fresh(const seg_list *seg_list_info);
td_s32 stm_seg_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt);
td_bool stm_seg_is_full(const seg_list *seg_list_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* STM_SEG_H */
