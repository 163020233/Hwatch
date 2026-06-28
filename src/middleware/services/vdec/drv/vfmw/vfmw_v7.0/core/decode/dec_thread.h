/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_THREAD_H__
#define __DEC_THREAD_H__

#include "vfmw.h"
#include "dec_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEC_MAX_THREAD_NUM 1

typedef enum {
    DEC_THREAD_STATE_NULL = 0,
    DEC_THREAD_STATE_RUN,
    DEC_THREAD_STATE_WAIT,
    DEC_THREAD_STATE_SUSPEND,
    DEC_THREAD_STATE_BUTT,
} dec_thread_state;

typedef enum {
    DEC_THREAD_CMD_NULL = 0,
    DEC_THREAD_CMD_START,
    DEC_THREAD_CMD_STOP,
    DEC_THREAD_CMD_DESTROY,
    DEC_THREAD_CMD_BUTT,
} dec_thread_cmd;

typedef struct {
    td_u32 id;
    td_u32 unid;
    td_void *handle;
    dec_thread_state state;
    dec_thread_cmd cmd;
    os_event event;
    td_u16 time_out;
    td_u32 run_pos;
    td_u32 run_cnt;
    td_u32 last_time;
    td_u32 schduling_interval[DEC_DEBUG_CNT];
    td_u32 cost_time[DEC_DEBUG_CNT];
    td_u32 cost_max_time;
    td_u32 sleep_time;
    td_u32 wait_time;
    td_u32 wait_flag;
    td_u32 check_interval;
    os_sema sema;
} dec_thread;

typedef struct tag_dec_thread_pool {
    dec_thread thread_handle[DEC_MAX_THREAD_NUM]; /* handle of thread */
    td_u32 thread_num;                            /* current number of thread */
    /* mutex_t      ThreadPoolMutex;  mutex of thread pool */
} dec_thread_pool;

td_s32 dec_thread_get_thread(td_u32 thread_id, dec_thread **thread);
td_s32 dec_thread_get_thread_num(td_void);

td_void dec_thread_init(td_void);
td_void dec_thread_deinit(td_void);

td_s32 dec_thread_routine(td_void *data);
td_s32 dec_thread_create(td_void *func);
td_s32 dec_thread_destroy(td_u32 thread_id);
td_void dec_thread_start(dec_thread *thread);
td_void dec_thread_stop(dec_thread *thread);
td_s32 dec_thread_wait(dec_thread *thread);
td_s32 dec_thread_sleep(const dec_thread *thread);
td_s32 dec_thread_wakeup(td_void);
td_s32 dec_thread_start_all(td_void);
td_s32 dec_thread_stop_all(td_void);
td_s32 dec_thread_is_destroy(td_u32 thread_id);
td_s32 dec_thread_clean(td_u32 thread_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
