/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_thread.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "stm_base.h"
#include "stm_ctrl.h"
#include "vdec_chr.h"

td_void stm_thread_lock(td_void)
{
    stm_thread *thread = stm_ctrl_get_entry()->thread;

    os_sema_down(thread->sema);
}

td_void stm_thread_unlock(td_void)
{
    stm_thread *thread = stm_ctrl_get_entry()->thread;

    os_sema_up(thread->sema);
}

td_void stm_thread_init(td_void)
{
    stm_thread *thread = stm_ctrl_get_entry()->thread;

    os_sema_init(&thread->sema);
}

td_void stm_thread_exit(td_void)
{
    stm_thread *thread = stm_ctrl_get_entry()->thread;

    os_sema_exit(thread->sema);
}

td_s32 stm_thread_create(td_void *func)
{
    td_void *handle = TD_NULL;
    stm_entry *entry = TD_NULL;
    stm_thread *thread = TD_NULL;

    vfmw_check_ret(func != TD_NULL, STM_ERR);
    entry = stm_ctrl_get_entry();
    thread = stm_ctrl_get_entry()->thread;

    stm_thread_lock();

    vfmw_check_goto(os_get_unid != TD_NULL, err);
    vfmw_check_goto(os_create_thread != TD_NULL, err);
    thread->unid = (td_u32)os_get_unid();
    thread->id = 0;
    thread->pos = 0;
    thread->run_cnt = 0;
    thread->handle = TD_NULL;
    thread->state = STM_THREAD_CREATE;
    thread->cmd = STM_THREAD_CREATE;

    handle = os_create_thread(func, TD_NULL, VFMW_STM_THREAD_NAME);
    if (handle == TD_NULL) {
        dprint(PRN_ERROR, "create stm_thread failed!\n");
        goto err;
    }

    thread->handle = handle;
    thread->max_time = 0;
    thread->sleep_time = entry->sleep_time;
    thread->wait_time = entry->wait_time;
    vfmw_check_sec_func(memset_s(thread->run_time, sizeof(thread->run_time), 0, sizeof(thread->run_time)));

    stm_thread_unlock();

    return STM_OK;
err:
    stm_thread_unlock();
    return STM_ERR;
}

td_s32 stm_thread_is_destroy(td_void)
{
    td_s32 ret = STM_ERR;
    stm_thread *thread = TD_NULL;

    stm_thread_lock();
    thread = stm_ctrl_get_entry()->thread;
    if (thread != TD_NULL && thread->handle == TD_NULL) {
        ret = STM_OK;
    }
    stm_thread_unlock();

    return ret;
}

td_s32 stm_thread_clean(td_void)
{
    stm_thread *thread = TD_NULL;

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        return STM_OK;
    }

    thread->pos = 0;
    thread->run_cnt = 0;
    thread->unid = 0;
    thread->state = STM_THREAD_NULL;
    thread->cmd = STM_THREAD_NULL;
    thread->handle = TD_NULL;

    return STM_OK;
}

td_s32 stm_thread_destroy(td_void)
{
    td_s32 ret;
    stm_thread *thread = TD_NULL;
    td_void *handle = TD_NULL;

    thread = stm_ctrl_get_entry()->thread;

    stm_thread_lock();

    if (thread->handle == TD_NULL) {
        stm_thread_unlock();
        return STM_OK;
    }

    if (thread->state == STM_THREAD_NULL) {
        thread->handle = TD_NULL;
        thread->pos = 0;
        thread->run_cnt = 0;
        thread->state = STM_THREAD_NULL;
        thread->cmd = STM_THREAD_NULL;
        stm_thread_unlock();
        return STM_OK;
    }
    handle = thread->handle;
    thread->cmd = STM_THREAD_DESTROY;
    stm_thread_unlock();

    ret = stm_thread_wakeup();
    if (ret != STM_OK) {
        /* thread func may exit after unlock */
        dprint(PRN_ERROR, "stm_thread wakeup error!\n");
    }
    thread->pos = THD_DSTY_POS2;
    while (stm_thread_is_destroy() == STM_ERR) {
        os_msleep(VFMW_THREAD_WAIT_EXIT_TIME);
    }
    thread->pos = THD_DSTY_POS3;
    os_destroy_thread(handle, TD_FALSE);
    vdec_record_chr_event(-1, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_STM_THREAD_DESTROY_OK);

    return STM_OK;
}

td_s32 stm_thread_start(td_void)
{
    stm_thread *thread = TD_NULL;

    stm_thread_lock();

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        dprint(PRN_ERROR, "stm_thread not exist !\n");
        stm_thread_unlock();
        return STM_ERR;
    }

    thread->cmd = STM_THREAD_START;
    stm_thread_unlock();

    return STM_OK;
}

td_s32 stm_thread_stop(td_void)
{
    stm_thread *thread = TD_NULL;

    stm_thread_lock();

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        stm_thread_unlock();
        return STM_ERR;
    }

    thread->cmd = STM_THREAD_STOP;
    stm_thread_unlock();

    return STM_OK;
}

td_s32 stm_thread_wait(td_void)
{
    stm_thread *thread = TD_NULL;
    td_u32 time;

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        return STM_ERR;
    }

    if (thread->wait_time > 0) {
        time = thread->wait_time;
    } else {
        time = 10; /* 10 is time ms */
    }
    os_event_wait(stm_ctrl_get_entry()->event, (td_s32)time);

    return STM_OK;
}

td_s32 stm_thread_wakeup(td_void)
{
    stm_thread *thread = TD_NULL;

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        return STM_ERR;
    }

    os_event_give(stm_ctrl_get_entry()->event);

    return STM_OK;
}

td_s32 stm_thread_sleep(td_void)
{
    stm_thread *thread = TD_NULL;

    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        return STM_ERR;
    }

    if (thread->sleep_time > 0) {
        os_msleep(thread->sleep_time);
    }

    return STM_OK;
}

td_void stm_thread_set_wait(td_u32 time)
{
    stm_thread *thread = TD_NULL;

    vfmw_check(time < 0x10);
    thread = stm_ctrl_get_entry()->thread;
    if (thread->handle == TD_NULL) {
        return;
    }

    thread->wait_time = time;
    return;
}

