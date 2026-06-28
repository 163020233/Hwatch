/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef SM_THREAD_H
#define SM_THREAD_H

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define STM_THREAD_NUM 1

typedef enum {
    THD_START_POS1 = 1,
    THD_START_POS2,
    THD_START_POS3,
    THD_START_POS4,
    THD_START_POS5,
    THD_STOP_POS1 = 10,
    THD_STOP_POS2,
    THD_DSTY_POS1 = 20,
    THD_DSTY_POS2,
    THD_DSTY_POS3,
} stm_thd_pos;

typedef enum {
    STM_THREAD_NULL = 0,
    STM_THREAD_CREATE,
    STM_THREAD_START,
    STM_THREAD_STOP,
    STM_THREAD_DESTROY,
    STM_THREAD_BUTT,
} stm_thread_status;

typedef struct {
    td_u32 id;
    td_u32 unid;
    td_void *handle;
    stm_thread_status cmd;
    stm_thread_status state;
    td_u32 pos;
    td_u32 run_cnt;
    td_s32 chan_id;
    td_u32 run_time[8]; /* 8 is time num */
    td_u32 max_time;
    td_u32 sleep_time;
    td_u32 wait_time;
    os_sema sema;
} stm_thread;

td_s32 stm_thread_create(td_void *func);
td_s32 stm_thread_start(td_void);
td_s32 stm_thread_stop(td_void);
td_s32 stm_thread_destroy(td_void);
td_s32 stm_thread_wait(td_void);
td_s32 stm_thread_wakeup(td_void);
td_s32 stm_thread_sleep(td_void);
td_s32 stm_thread_is_destroy(td_void);
td_s32 stm_thread_clean(td_void);
td_void stm_thread_lock(td_void);
td_void stm_thread_unlock(td_void);
td_void stm_thread_init(td_void);
td_void stm_thread_exit(td_void);
td_void stm_thread_set_wait(td_u32 time);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SM_THREAD_H */
