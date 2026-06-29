/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_CHAN_H
#define SM_CHAN_H

#include "vfmw.h"
#include "stm_raw.h"
#include "stm_seg.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

#define STM_CHAN_NUM VFMW_CHAN_NUM
#define STM_EOS_SIZE 0x1000

#define stm_assert_chan_id_(id, ret)          \
    {                                        \
        if ((id) < 0 || (id) >= STM_CHAN_NUM) { \
            return (ret);                    \
        }                                    \
    }
#define STM_ASSERT_CHAN_ID stm_assert_chan_id_

typedef enum {
    STM_CHAN_STATE_NULL = 0,
    STM_CHAN_STATE_RUN,
    STM_CHAN_STATE_STOP,
    STM_CHAN_STATE_MAX
} stm_chan_state;

typedef enum {
    STM_EOS_WAIT = 0,
    STM_EOS_RECV,
    STM_EOS_INSERT,
    STM_EOS_PARSING,
    STM_EOS_DONE,
    STM_EOS_MAX
} stm_eos_state;

typedef enum {
    STM_RAW_NO_NEED_THRESHOLD = 0,
    STM_RAW_NEED_THRESHOLD,
} stm_raw_threshold;

typedef enum {
    STM_RAW_NOT_GET_LASTPACKET = 0,
    STM_RAW_GET_LASTPACKET,
} stm_raw_lastpacket;

typedef enum {
    STM_FIRST_SECOND_NOT_GET = 0,
    STM_FIRST_PTS_GET,
    STM_SECOND_PTS_GET,
} stm_pts_state;

typedef struct {
    td_s32 find_start_code_cnt;
    td_s32 last_one_start_code;
    td_s32 last_two_start_code;
    td_s32 current_start_code;
    td_s32 h264_score;
    td_s32 mpeg2_score;
} stm_chk_param;

typedef struct {
    td_s32 id;
    stm_chan_state state;
    stm_raw_threshold thres_state;      /* if need control the treshold */
    stm_raw_lastpacket get_last_packet; /* if get the lastpack of one frame */
    stm_eos_state eos;
    td_bool eop_flag;
    td_u32 unid;
    td_s32 dev_id;
    td_u32 block_time;
    td_void *handle;
    os_sema sema;
    os_lock lock;

    td_s32 is_sec;
    td_s32 seg_buf_size;
    stm_cfg_info cfg;
    vfmw_mem_desc seg_mem;
    vfmw_mem_desc eos_mem;
    vfmw_mem_desc eos_src;
    vfmw_mem_desc eop_mem;
    vfmw_stream_intf raw_intf;
    td_bool es_valid;
    td_bool std_err;
    td_u32  rls_raw_wait;
    td_u32  rls_raw_idx;

    raw_list_info *raw_list;
    seg_list *seg_list_info;

    stm_pts_state pts_state;

    stm_chk_param chk_param;

    td_s32 threshold;
    td_u32 tick;
} stm_chan;

typedef struct {
    td_s32 sec;
    td_s32 no_sec;
} stm_chan_type_num;

td_s32 stm_chan_create(td_s32 chan_id, const td_void *args);
td_s32 stm_chan_destroy(td_s32 chan_id);
td_s32 stm_chan_config(td_s32 chan_id, const td_void *args);
td_s32 stm_chan_cfg_ext_intf(td_s32 chan_id, const td_void *args);
td_s32 stm_chan_start(td_s32 chan_id);
td_s32 stm_chan_stop(td_s32 chan_id);
td_s32 stm_chan_reset(td_s32 chan_id);
td_s32 stm_chan_get_num(stm_chan_type_num *num);
td_s32 stm_chan_run_proc(td_void);
td_void stm_chan_print_block(td_s32 chan_id, const td_char *info);
td_void stm_chan_lock(td_s32 chan_id, td_ulong *lock_flag);
td_void stm_chan_unlock(td_s32 chan_id, td_ulong *lock_flag);
td_s32 stm_chan_set_es_invalid(td_s32 chan_id);
td_s32 stm_chan_need_lowldy(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_CHAN_H */
