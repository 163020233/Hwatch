/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_thread.h"
#include "dec_base.h"
#include "dec_ctrl.h"
#include "dbg.h"
#include "vdec_chr.h"

/* lock and unlock */
STATIC td_void dec_thread_lock_thread_pool(dec_thread_pool *thread_pool)
{
    vfmw_unused(thread_pool);
    return;
}

STATIC td_void dec_thread_unlock_thread_pool(dec_thread_pool *thread_pool)
{
    vfmw_unused(thread_pool);
    return;
}

VFMW_ACC_BASE STATIC td_void dec_thread_lock_thread(dec_thread *thread)
{
    os_sema_down(thread->sema);

    return;
}

VFMW_ACC_BASE STATIC td_void dec_thread_unlock_thread(dec_thread *thread)
{
    os_sema_up(thread->sema);

    return;
}

/* init and deinit */
STATIC td_void dec_thread_init_thread(dec_thread *thread)
{
    vfmw_check_sec_func(memset_s(thread, sizeof(dec_thread), 0, sizeof(dec_thread)));

    os_sema_init(&thread->sema);

    return;
}

STATIC td_void dec_thread_deinit_thread(dec_thread *thread)
{
    os_sema_exit(thread->sema);

    vfmw_check_sec_func(memset_s(thread, sizeof(dec_thread), 0, sizeof(dec_thread)));

    return;
}

STATIC td_void dec_thread_clean_thread(dec_thread *thread)
{
    thread->unid = 0;
    thread->event = 0;
    thread->state = 0;
    thread->cmd = 0;
    thread->time_out = 0;
    thread->run_cnt = 0;
    thread->last_time = 0;
    thread->handle = TD_NULL;

    vfmw_check_sec_func(memset_s(thread->schduling_interval,
                                 DEC_DEBUG_CNT * sizeof(td_u32), 0, DEC_DEBUG_CNT * sizeof(td_u32)));
    vfmw_check_sec_func(memset_s(thread->cost_time,
                                 DEC_DEBUG_CNT * sizeof(td_u32), 0, DEC_DEBUG_CNT * sizeof(td_u32)));
    thread->cost_max_time = 0;

    return;
}

STATIC td_void dec_thread_init_thread_pool(dec_thread_pool *thread_pool)
{
    td_u32 id;

    thread_pool->thread_num = 0;

    for (id = 0; id < DEC_MAX_THREAD_NUM; id++) {
        dec_thread_init_thread(&thread_pool->thread_handle[id]);
    }

    return;
}

STATIC td_void dec_thread_deinit_thread_pool(dec_thread_pool *thread_pool)
{
    td_u32 id;

    thread_pool->thread_num = 0;

    for (id = 0; id < DEC_MAX_THREAD_NUM; id++) {
        dec_thread_deinit_thread(&thread_pool->thread_handle[id]);
    }

    return;
}

/* set state */
STATIC td_void dec_thread_set_state(dec_thread *thread, dec_thread_state state)
{
    dec_thread_lock_thread(thread);

    thread->state = state;

    dec_thread_unlock_thread(thread);

    return;
}

/* interface */
td_s32 dec_thread_get_thread(td_u32 thread_id, dec_thread **thread)
{
    dec_thread_pool *thread_pool = TD_NULL;

    vfmw_check_ret(thread != TD_NULL, DEC_ERR);

    if (thread_id >= DEC_MAX_THREAD_NUM) {
        dprint(PRN_ERROR, "DecThread thread_id error!\n");
        return DEC_ERR;
    }

    thread_pool = dec_ctrl_get_thread_pool();

    *thread = &(thread_pool->thread_handle[thread_id]);

    return DEC_OK;
}

td_s32 dec_thread_get_thread_num(td_void)
{
    td_s32 thread_num;
    dec_thread_pool *thread_pool = TD_NULL;

    thread_pool = dec_ctrl_get_thread_pool();

    dec_thread_lock_thread_pool(thread_pool);

    thread_num = (td_s32)thread_pool->thread_num;

    dec_thread_unlock_thread_pool(thread_pool);

    return thread_num;
}

td_void dec_thread_init(td_void)
{
    dec_thread_pool *thread_pool = TD_NULL;

    thread_pool = dec_ctrl_get_thread_pool();

    dec_thread_init_thread_pool(thread_pool);
}

td_void dec_thread_deinit(td_void)
{
    dec_thread_pool *thread_pool = TD_NULL;

    thread_pool = dec_ctrl_get_thread_pool();

    dec_thread_deinit_thread_pool(thread_pool);
}

STATIC VFMW_ACC_BASE td_s32 dec_thread_decide_state(dec_thread *thread, dec_thread_cmd cmd, td_u32 thread_pos)
{
    dec_thread_cmd cmd_tmp = cmd;

    vfmw_check_ret(thread != TD_NULL, DEC_ERR);
    dec_thread_lock_thread(thread);
    cmd_tmp = thread->cmd;
    thread->run_pos = thread_pos; /* 20 :a number */
    dec_thread_unlock_thread(thread);
    if (cmd_tmp == DEC_THREAD_CMD_STOP) {
        return DEC_STOP;
    } else if (cmd_tmp == DEC_THREAD_CMD_DESTROY) {
        dec_chan_try_refresh_bin();
        dec_chan_try_close_dev();
        return DEC_DESTROY;
    }

    return DEC_OK;
}

#ifdef VFMW_CHECK_MEM_SUPPORT
STATIC td_s32 dec_thread_check_mem(const dec_thread *thread)
{
    td_s32 ret;
    td_bool mem_err;
    STATIC td_u32 last_time = 0;
    td_u32 cur_time;

    cur_time = os_get_time_ms();
    if ((cur_time >= last_time) && (cur_time - last_time > thread->check_interval)) {
        vfmw_check_ret(os_check_mem != TD_NULL, DEC_ERR);
        ret = os_check_mem(&mem_err);
        vfmw_check_ret((ret == OSAL_OK) && (mem_err == 0), DEC_ERR);
        last_time = cur_time;
    } else if (cur_time < last_time) {
        last_time = cur_time;
    }

    return DEC_OK;
}
#endif

STATIC VFMW_ACC_BASE td_s32 dec_thread_start_routine(dec_thread *thread, td_u32 thread_unid, dec_thread_cmd cmd)
{
    td_u32 start_time, end_time, cost_time;
    td_s32 ret;

    vfmw_check_ret(thread != TD_NULL, DEC_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, DEC_ERR);

    if (thread_unid != thread->unid) {
        dprint(PRN_ERROR, "Dec thread unid changed! line:%d!\n", __LINE__);
        return DEC_ERR;
    }

    ret = dec_thread_decide_state(thread, cmd, 20); /* 20 is thread pos */
    if (ret != DEC_OK) {
        return ret;
    }

    start_time = os_get_time_us();
    cost_time = start_time - thread->last_time;
    thread->schduling_interval[thread->run_cnt % DEC_DEBUG_CNT] = cost_time;
    thread->last_time = start_time;

#ifdef VFMW_CHECK_MEM_SUPPORT
    ret = dec_thread_check_mem(thread);
    vfmw_check_ret(ret == DEC_OK, ret);
#endif

    thread->run_pos = 30; /* 30 :a number */
    dec_chan_routine();
    thread->run_pos = 40; /* 40 :a number */

    end_time = os_get_time_us();
    cost_time = (end_time > start_time) ? (end_time - start_time) : 0;
    thread->cost_time[thread->run_cnt % DEC_DEBUG_CNT] = cost_time;
    thread->cost_max_time = thread->cost_max_time > cost_time ? thread->cost_max_time : cost_time;
    thread->run_cnt++;

    ret = dec_thread_decide_state(thread, cmd, 40); /* 40 is thread pos */
    if (ret != DEC_OK) {
        return ret;
    }

    thread->run_pos = 50; /* 50 :a number */

    if (dec_thread_wait(thread) != DEC_OK) {
        dprint(PRN_ERROR, "dec_thread_wait failed!\n");
    }
    thread->run_pos = 60; /* 60 :a number */
    if (dec_thread_sleep(thread) != DEC_OK) {
        dprint(PRN_ERROR, "dec_thread_sleep failed!\n");
    }

    return DEC_OK;
}

STATIC VFMW_ACC_BASE td_s32 dec_thread_stop_routine(dec_thread *thread, td_u32 thread_unid, dec_thread_cmd cmd)
{
    dec_thread_cmd cmd_tmp = cmd;

    vfmw_check_ret(thread != TD_NULL, DEC_ERR);

    if (thread_unid != thread->unid) {
        dprint(PRN_ERROR, "Dec thread unid changed! line:%d!\n", __LINE__);
        return DEC_ERR;
    }

    dec_thread_lock_thread(thread);
    cmd_tmp = thread->cmd;
    thread->run_pos = 80; /* 80 :a number */
    dec_thread_unlock_thread(thread);

    if (cmd_tmp == DEC_THREAD_CMD_START) {
        return DEC_START;
    } else if (cmd_tmp == DEC_THREAD_CMD_DESTROY) {
        return DEC_DESTROY;
    }

    thread->run_pos = 90; /* 90 :a number */
    os_msleep(10); /* 10 :a number */

    return DEC_OK;
}

STATIC VFMW_ACC_BASE td_void dec_thread_destroy_routine(const dec_thread *thread, td_u32 thread_unid)
{
    vfmw_check(thread != TD_NULL);

    if (thread_unid == thread->unid) {
        if (dec_thread_clean(0) != DEC_OK) {
            dprint(PRN_ERROR, "dec thread clean Error\n");
        }
    } else {
        dprint(PRN_ERROR, "Dec thread unid changed! line:%d\n", __LINE__);
    }
}

VFMW_ACC_BASE td_s32 dec_thread_routine(td_void *data)
{
    td_s32 ret;
    dec_thread *thread = TD_NULL;
    dec_thread_cmd cmd = DEC_THREAD_CMD_NULL;
    td_u32 thread_unid;

    vfmw_unused(data);

    if (dec_thread_get_thread(0, &thread) != DEC_OK) {
        dprint(PRN_ERROR, "get_thread error!\n");
        return DEC_ERR;
    }

    thread_unid = thread->unid;

dec_start:

    thread->run_pos = 10; /* 10 :a number */
    dec_thread_set_state(thread, DEC_THREAD_STATE_RUN);

    while (1) {
        ret = dec_thread_start_routine(thread, thread_unid, cmd);
        if (ret == DEC_ERR) {
            return ret;
        } else if (ret == DEC_STOP) {
            goto dec_stop;
        } else if (ret == DEC_DESTROY) {
            goto dec_destroy;
        }
    }

dec_stop:

    thread->run_pos = 70; /* 70 :a number */
    dec_thread_set_state(thread, DEC_THREAD_STATE_SUSPEND);

    while (1) {
        ret = dec_thread_stop_routine(thread, thread_unid, cmd);
        if (ret == DEC_ERR) {
            return ret;
        } else if (ret == DEC_START) {
            goto dec_start;
        } else if (ret == DEC_DESTROY) {
            goto dec_destroy;
        }
    }

dec_destroy:

    thread->run_pos = 100; /* 100 :a number */

    dec_thread_destroy_routine(thread, thread_unid);

    return DEC_OK;
}

STATIC td_void dec_thread_set_default_info(dec_thread *thread)
{
    dec_entry *entry = dec_ctrl_get_entry();

    vfmw_check(thread != TD_NULL);

    thread->time_out = 5; /* 5 :a number */
    thread->run_pos = 0;
    thread->run_cnt = 0;
    thread->last_time = 0;
    thread->cost_max_time = 0;
    thread->id = 0;
    thread->state = DEC_THREAD_STATE_SUSPEND;
    thread->sleep_time = entry->sleep_time;
    thread->wait_time = entry->wait_time;
    thread->check_interval = entry->check_interval;
}

td_s32 dec_thread_create(td_void *func)
{
    dec_thread_pool *thread_pool = dec_ctrl_get_thread_pool();
    dec_thread *thread = TD_NULL;
    td_void *handle = TD_NULL;
    td_s32 ret = DEC_ERR;

    vfmw_check_ret((func != TD_NULL) && (os_event_init != TD_NULL) && (os_get_unid != TD_NULL) &&
        (os_create_thread != TD_NULL), DEC_ERR);

    if (dec_thread_get_thread(thread_pool->thread_num, &thread) != DEC_OK) {
        dprint(PRN_FATAL, "dec_thread_get_thread error!\n");
        return DEC_ERR;
    }

    dec_thread_lock_thread_pool(thread_pool);
    dec_thread_lock_thread(thread);

    vfmw_check_goto((thread_pool->thread_num < DEC_MAX_THREAD_NUM), exit);

    if (thread->handle != TD_NULL) {
        dprint(PRN_FATAL, "DecThread thread has created!\n");
        ret = DEC_OK;
        goto exit;
    }

    if (os_event_init(&thread->event, 0) != DEC_OK) {
        dprint(PRN_FATAL, "os_event_init err!\n");
        goto exit;
    }

    thread->unid = (td_u32)os_get_unid();
    handle = os_create_thread(func, TD_NULL, VFMW_DEC_THREAD_NAME);
    if (handle == TD_NULL) {
        dprint(PRN_FATAL, "DecThread create thread failed!\n");
        os_event_give(thread->event);
        ret = DEC_ERR;
        goto exit;
    }

    thread->handle = handle;

    dec_thread_set_default_info(thread);

    thread_pool->thread_num++;

    ret = DEC_OK;

exit:

    dec_thread_unlock_thread(thread);
    dec_thread_unlock_thread_pool(thread_pool);

    return ret;
}

td_s32 dec_thread_is_destroy(td_u32 thread_id)
{
    dec_thread *thread = TD_NULL;
    td_s32 ret = DEC_ERR;

    thread = dec_ctrl_get_thread(thread_id);
    if (thread == TD_NULL) {
        return DEC_ERR;
    }

    dec_thread_lock_thread(thread);
    if (thread->handle == TD_NULL) {
        ret = DEC_OK;
    }
    dec_thread_unlock_thread(thread);

    return ret;
}

td_s32 dec_thread_clean(td_u32 thread_id)
{
    dec_thread *thread = TD_NULL;

    thread = dec_ctrl_get_thread(thread_id);
    if (thread == TD_NULL) {
        dprint(PRN_ERROR, "%s thread is null\n", __func__);
        return DEC_ERR;
    }

    dec_thread_lock_thread(thread);
    if (thread->handle == TD_NULL) {
        dec_thread_unlock_thread(thread);
        return DEC_OK;
    }

    os_event_exit(thread->event);
    dec_thread_clean_thread(thread);
    dec_thread_unlock_thread(thread);

    return DEC_OK;
}

td_s32 dec_thread_destroy(td_u32 thread_id)
{
    dec_thread_pool *thread_pool = TD_NULL;
    dec_thread *thread = TD_NULL;
    td_s32 ret = DEC_ERR;
    td_void *handle = TD_NULL;

    thread_pool = dec_ctrl_get_thread_pool();
    thread = dec_ctrl_get_thread(thread_id);
    if (thread == TD_NULL) {
        dprint(PRN_ERROR, "%s thread is null\n", __func__);
        return DEC_ERR;
    }

    if (thread_pool->thread_num == 0) {
        dprint(PRN_ERROR, "dec thread_num is 0!\n");
        return DEC_ERR;
    }

    dec_thread_lock_thread_pool(thread_pool);
    dec_thread_lock_thread(thread);

    if (thread->handle == TD_NULL || thread->state == DEC_THREAD_STATE_NULL) {
        dprint(PRN_ERROR, "DEC_Thread NUll!\n");
        goto exit;
    }

    handle = thread->handle;
    thread->cmd = DEC_THREAD_CMD_DESTROY;
    os_event_give(thread->event);
    thread_pool->thread_num--;
    ret = DEC_OK;

exit:

    dec_thread_unlock_thread(thread);
    dec_thread_unlock_thread_pool(thread_pool);
    thread->run_pos = 200; /* 200 is num */
    while (dec_thread_is_destroy(thread_pool->thread_num) == DEC_ERR) {
        os_msleep(VFMW_THREAD_WAIT_EXIT_TIME);
    }
    thread->run_pos = 300; /* 300 is num */
    os_destroy_thread(handle, TD_FALSE);
    vdec_record_chr_event(-1, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_DEC_THREAD_DESTROY_OK);

    return ret;
}

td_void dec_thread_start(dec_thread *thread)
{
    vfmw_check(thread != TD_NULL);

    dec_thread_lock_thread(thread);

    thread->cmd = DEC_THREAD_CMD_START;

    dec_thread_unlock_thread(thread);

    return;
}

td_void dec_thread_stop(dec_thread *thread)
{
    vfmw_check(thread != TD_NULL);

    dec_thread_lock_thread(thread);
    thread->cmd = DEC_THREAD_CMD_STOP;
    dec_thread_unlock_thread(thread);

    return;
}

td_s32 dec_thread_wait(dec_thread *thread)
{
    td_u32 wtime;

    if (thread == TD_NULL) {
        return DEC_ERR;
    }

    if (thread->handle == TD_NULL) {
        return DEC_ERR;
    }

    if (thread->wait_time > 0) {
        wtime = thread->wait_time;
    } else {
        wtime = thread->time_out;
    }

    thread->wait_flag = 1;

    /* no lock for wakeup then destroy */
    os_event_wait(thread->event, (td_s32)wtime);

    thread->wait_flag = 0;

    return DEC_OK;
}

td_s32 dec_thread_sleep(const dec_thread *thread)
{
    if (thread == TD_NULL) {
        return DEC_ERR;
    }

    if (thread->sleep_time > 0) {
        os_msleep(thread->sleep_time);
    }

    return DEC_OK;
}

td_s32 dec_thread_wakeup(td_void)
{
    dec_thread_pool *thread_pool = TD_NULL;
    dec_thread *thread = TD_NULL;
    td_u32 i;

    thread_pool = dec_ctrl_get_thread_pool();

    for (i = 0; i < thread_pool->thread_num; i++) {
        if (dec_thread_get_thread(i, &thread) != DEC_OK) {
            continue;
        }

        if (thread != TD_NULL && thread->wait_flag == 1) {
            os_event_give(thread->event);
        }
    }

    return DEC_OK;
}

td_s32 dec_thread_start_all(td_void)
{
    dec_thread_pool *thread_pool = TD_NULL;
    dec_thread *thread = TD_NULL;
    td_u32 i;

    thread_pool = dec_ctrl_get_thread_pool();

    for (i = 0; i < thread_pool->thread_num; i++) {
        if (dec_thread_get_thread(i, &thread) != DEC_OK) {
            continue;
        }
        dec_thread_start(thread);
    }

    return DEC_OK;
}

td_s32 dec_thread_stop_all(td_void)
{
    dec_thread_pool *thread_pool = TD_NULL;
    dec_thread *thread = TD_NULL;
    td_u32 i;
    td_s32 time = 1000;

    thread_pool = dec_ctrl_get_thread_pool();

    for (i = 0; i < thread_pool->thread_num; i++) {
        if (dec_thread_get_thread(i, &thread) != DEC_OK) {
            continue;
        }
        dec_thread_stop(thread);

        for (; time > 0; time--) {
            if (thread->state == DEC_THREAD_STATE_NULL ||
                thread->state == DEC_THREAD_STATE_SUSPEND) {
                break;
            }
            os_msleep(1);
        }

        if (time <= 0) {
            dec_thread_set_state(thread, DEC_THREAD_STATE_SUSPEND);
        }
    }

    return DEC_OK;
}
