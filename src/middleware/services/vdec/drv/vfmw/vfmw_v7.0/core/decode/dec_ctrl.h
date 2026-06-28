/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_CTRL_H__
#define __DEC_CTRL_H__

#include "vfmw.h"
#include "dec_thread.h"
#include "dec_chn.h"
#include "dec_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DEC_CID_CREATE_CHAN = 0,
    DEC_CID_CONFIG_CHAN,
    DEC_CID_START_CHAN,
    DEC_CID_STOP_CHAN,
    DEC_CID_RESET_CHAN,
    DEC_CID_DESTROY_CHAN,
    DEC_CID_GET_INFO,
    DEC_CID_MAX
} dec_cid;

typedef td_s32 (*dec_cmd_handler)(td_s32, const td_void *args);
typedef struct {
    dec_cid cmd_id;
    dec_cmd_handler handler;
} dec_cid_node;

typedef struct {
    td_u32 dec_check_0;

    dec_thread_pool thread_pool;
    dec_chan_pool chan_pool;
    dec_dev_pool dev_pool;

    os_sema sema;

    td_u32 dec_check_1;
    td_u32 sleep_time;
    td_u32 wait_time;
    td_u32 check_interval;
    td_u32 is_over_load;
    td_s32 bypass_chan;
} dec_entry;

td_s32 dec_ctrl_open(td_void);
td_s32 dec_ctrl_close(td_void);
td_s32 dec_ctrl_suspend(td_void);
td_s32 dec_ctrl_resume(td_void);
td_s32 dec_ctrl_wakeup(td_s32 chan_id);
td_s32 dec_ctrl_entry_init(td_void);
td_s32 dec_ctrl_entry_exit(td_void);

td_s32 dec_ctrl_create_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_config_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_start_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_stop_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_reset_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_destroy_chan(td_s32 chan_id, const td_void *args);
td_s32 dec_ctrl_get_state(td_void);
td_s32 dec_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 dec_ctrl_control(td_s32 chan_id, td_u32 cid, td_void *args);
td_s32 dec_ctrl_process(td_void);

dec_entry *dec_ctrl_get_entry(td_void);
dec_thread_pool *dec_ctrl_get_thread_pool(td_void);
dec_chan_pool *dec_ctrl_get_chan_pool(td_void);
dec_dev_pool *dec_ctrl_get_dev_pool(td_void);
dec_thread *dec_ctrl_get_thread(td_u32 thread_id);
dec_chan *dec_ctrl_get_chan(td_s32 chan_id);
dec_dev_info *dec_ctrl_get_dev(td_u32 dev_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
