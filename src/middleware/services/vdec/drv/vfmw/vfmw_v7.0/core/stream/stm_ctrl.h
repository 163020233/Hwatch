/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_CTRL_H
#define SM_CTRL_H

#include "vfmw.h"
#include "vfmw_osal.h"
#include "stm_intf.h"
#include "stm_chan.h"
#include "stm_thread.h"
#include "stm_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_s32 (*open)(td_void);
    td_s32 (*close)(td_void);
    td_s32 (*suspend)(td_void);
    td_s32 (*resume)(td_void);
    td_s32 (*wakeup)(td_void);
    td_s32 (*receive_seg)(td_s32, stm_seg_info *);
    td_s32 (*release_seg)(td_s32, td_s32);
    td_s32 (*set_ref_cnt)(td_s32, td_s32, td_s32);
    td_s32 (*control)(td_s32, stm_cmd, td_void *);
    td_s32 (*get_mem_need_size)(vfmw_svr_mem_need *);
    td_s32 (*get_chan_mem_size)(td_void);
} stm_ops;

typedef enum {
    STM_STATE_NULL = 0,
    STM_STATE_RUN,
    STM_STATE_MAX,
} stm_state;

typedef struct {
    td_u32 reserve1;

    os_sema sema;
    os_sema raw_sema;
    os_sema seg_sema;
    os_event event;
    os_lock lock;
    os_atomic ref_cnt;
    stm_state state;
    stm_ops *ops;

    td_u32 ctx_size;
    td_u32 hal_size;
    td_u32 sleep_time;
    td_u32 wait_time;
    td_s32 this_chan;
    td_s32 start_chan;
    os_file raw_file[STM_CHAN_NUM];
    os_file seg_file[STM_CHAN_NUM];
    stm_dev dev[STM_DEV_NUM];
    stm_chan chan[STM_CHAN_NUM];
    stm_thread thread[STM_THREAD_NUM];
    td_s8 save_seek_raw;
    td_u32 stm_no_raw;
    td_u32 stm_raw_full;
    td_u32 stm_no_seg;
    td_u32 stm_seg_full;
    td_s32 bypass_chan;

    td_u32 reserve2;
} stm_entry;

td_s32 stm_ctrl_open(td_void);
td_s32 stm_ctrl_close(td_void);
td_s32 stm_ctrl_suspend(td_void);
td_s32 stm_ctrl_resume(td_void);
td_s32 stm_ctrl_wake_up(td_void);
td_s32 stm_ctrl_receive_seg(td_s32 chan_id, stm_seg_info *seg);
td_s32 stm_ctrl_release_seg(td_s32 chan_id, td_s32 seg_id);
td_s32 stm_ctrl_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt);
td_s32 stm_ctrl_control(td_s32 chan_id, stm_cmd cmd, td_void *arg);
td_s32 stm_ctrl_delete_raw(td_s32 chan_id);
td_s32 stm_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem);

td_void stm_ctrl_lock(td_ulong *lock_flag);
td_void stm_ctrl_unlock(td_ulong *lock_flag);
td_void stm_ctrl_sema_up(td_void);
td_void stm_ctrl_sema_down(td_void);

stm_ops *stm_ctrl_get_ops(td_void);
stm_entry *stm_ctrl_get_entry(td_void);
stm_thread *stm_ctrl_get_thread(td_void);
stm_dev *stm_ctrl_get_dev(td_s32 dev_id);
td_s32 stm_ctrl_process(td_void);
td_bool stm_ctrl_seg_has_fresh(td_s32 chan_id);

#define raw_file_(id)               stm_ctrl_get_entry()->raw_file[id]
#define RAW_FILE raw_file_
#define seg_file_(id)               stm_ctrl_get_entry()->seg_file[id]
#define SEG_FILE seg_file_
#define stm_ctrl_get_chan_(chan_id) (&(stm_ctrl_get_entry()->chan[chan_id]))
#define STM_CTRL_GET_CHAN stm_ctrl_get_chan_

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_CTRL_H */
