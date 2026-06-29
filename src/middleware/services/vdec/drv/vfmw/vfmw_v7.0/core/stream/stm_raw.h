/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_RAW_H
#define SM_RAW_H

#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_RAW_LEN    (16 * 1024 * 1024)
#define RAW_START_NUM  32
#define RAW_START_SIZE (1500 * 1024)
#define EXT_CC_SIZE    256

typedef vfmw_stream_data stm_raw_info;

typedef struct {
    td_s32 head;
    td_s32 tail;
    td_s32 history;
    td_s32 show_id;
    td_s32 push_num;
    td_s32 push_len;
    td_s32 fresh_size;
    td_s32 fresh_num;
    td_s32 is_cuting;
    td_s32 fill_new_flag;
    td_s32 last_cfg_all_flag;
    td_s32 last_start_time;
    td_s32 first_packet_offset;
    td_u32 delete_len;
    td_u32 need_release;

    td_u32 cfg_dev_cnt;
    td_u32 raw_num_cnt;
    td_u32 raw_size_cnt;
    td_u32 last_time_ms;
    td_u32 bitrate_kbps;
    td_u32 num_rate_ops;

    td_u32 raw_num_all;
    td_u32 raw_size_all;
    td_u32 delete_num;

    td_u32 recv_try_cnt;
    td_u32 recv_ok_cnt;
    td_u32 recv_fail_cnt;
    td_u32 rels_try_cnt;
    td_u32 rels_ok_cnt;

    td_u32 cc_len;
    td_u8 cc_data[EXT_CC_SIZE];

    stm_raw_info raw_tmp;
    stm_raw_info raw[MAX_RAW_NUM];
} raw_list_info;

td_s32 stm_raw_init(td_s32 chan_id);
td_s32 stm_raw_read_ext(td_s32 chan_id, stm_raw_info *raw);
td_s32 stm_raw_insert(td_s32 chan_id, const stm_raw_info *raw);
td_s32 stm_raw_release_ext(td_s32 chan_id, stm_raw_info *raw);
td_s32 stm_raw_delete(td_s32 chan_id);
td_s32 stm_raw_delete_head(td_s32 chan_id);
td_void stm_raw_del_cfg_raw(td_s32 chan_id);
td_s32 stm_raw_deleta_all(td_s32 chan_id);
td_s32 stm_raw_show_head(td_s32 chan_id, stm_raw_info *raw);
stm_raw_info *stm_raw_show_next(raw_list_info *raw_list);
td_s32 stm_raw_get_first_offset(td_s32 chan_id);
td_s32 stm_raw_is_full(const raw_list_info *raw_list);
td_s32 stm_raw_release_len(td_s32 chan_id, td_s32 del_len);
td_s32 stm_raw_get_fresh_num(td_s32 chan_id);
td_s32 stm_raw_get_fresh_size(td_s32 chan_id);
td_void stm_raw_get_pts(td_s32 chan_id, td_s32 len, td_s64 *last_pts, td_void *seg);
td_s32 stm_raw_gather_last_raw(td_s32);
td_bool stm_raw_is_raw_full(td_s32);
td_s32 stm_raw_get_raw_num_in_frm(td_s32 chan_id, td_s32 *raw_num, td_s32 *raw_len);
td_s32 stm_raw_adjust_vp6a(td_s32 chan_id, td_s32 *raw_num, td_u64 *src_vir_addr, td_s32 *src_len, td_s32 *idx);
td_u32 stm_raw_get_bit_rate(td_s32 chan_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_RAW_H */
