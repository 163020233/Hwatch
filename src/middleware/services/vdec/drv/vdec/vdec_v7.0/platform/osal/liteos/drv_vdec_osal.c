/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "drv_vdec_osal.h"
#include "soc_osal.h"

td_s32 vdec_osal_atomic_init(vdec_osal_atomic *var)
{
    vdec_unused(var);
    return EXT_SUCCESS;
}

td_void vdec_osal_atomic_exit(vdec_osal_atomic *var)
{
    vdec_unused(var);
    return;
}

td_s32 vdec_osal_atomic_inc_return(vdec_osal_atomic *var)
{
    return osal_atomic_inc_return(var);
}

td_s32 vdec_osal_atomic_dec_return(vdec_osal_atomic *var)
{
    return osal_atomic_dec_return(var);
}

td_void vdec_osal_atomic_inc(vdec_osal_atomic *var)
{
    osal_atomic_inc(var);
}

td_void vdec_osal_atomic_dec(vdec_osal_atomic *var)
{
    osal_atomic_dec(var);
}

td_s32 vdec_osal_atomic_read(vdec_osal_atomic *var)
{
    return osal_atomic_read(var);
}

td_void vdec_osal_atomic_set(vdec_osal_atomic *var, td_s32 value)
{
    osal_atomic_set(var, value);
}

td_s32 vdec_osal_event_init(vdec_osal_event *s)
{
    if (s == TD_NULL) {
        return EXT_FAILURE;
    }
    s->flag = 0;
    return osal_wait_init(&s->wait);
}

td_s32 vdec_osal_event_init_cond(vdec_osal_event *s)
{
    if (s == TD_NULL) {
        return EXT_FAILURE;
    }
    s->flag = 0;
    return EXT_SUCCESS;
}

static td_s32 vdec_osal_wait_call_back(const td_void *args)
{
    vdec_osal_event *event = TD_NULL;

    event = (vdec_osal_event *)args;

    return event->flag == 0 ? 0 : 1;
}

td_s32 vdec_osal_event_wait(vdec_osal_event *s, td_u32 ms_wait_time)
{
    td_s32 ret;

    if (s == TD_NULL) {
        return EXT_FAILURE;
    }
    ret = osal_wait_timeout_uninterruptible(&s->wait,
        vdec_osal_wait_call_back, s, ms_wait_time);
    s->flag = 0;

    return ret;
}

td_s32 vdec_osal_event_wait_interruptible(vdec_osal_event *s, td_u32 ms_wait_time)
{
    td_s32 ret;

    if (s == TD_NULL) {
        return EXT_FAILURE;
    }
    ret = osal_wait_timeout_interruptible(&s->wait,
        vdec_osal_wait_call_back, s, ms_wait_time);
    s->flag = 0;

    return ret;
}

td_void vdec_osal_event_give(vdec_osal_event *s)
{
    if (s == TD_NULL) {
        return;
    }
    s->flag = 1;
    osal_wait_wakeup(&(s->wait));
}

td_void vdec_osal_event_exit(vdec_osal_event *s)
{
    if (s == TD_NULL) {
        return;
    }
    s->flag = 0;
    osal_wait_destroy(&(s->wait));
}

td_void vdec_osal_sema_init(vdec_osal_sema *s)
{
    osal_sem_init(s, 1);
}

td_void vdec_osal_sema_exit(vdec_osal_sema *s)
{
    osal_sem_destroy(s);
}

td_void vdec_osal_sema_down(vdec_osal_sema *s)
{
    osal_sem_down(s);
}

td_void vdec_osal_sema_up(vdec_osal_sema *s)
{
    osal_sem_up(s);
}

td_u32 vdec_osal_get_time(td_void)
{
    td_u32 curr_us;
    osal_timeval cur_time = { 0 };

    osal_gettimeofday(&cur_time);
    curr_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000 index */

    return curr_us;
}

td_void vdec_osal_msleep(td_u32 time)
{
    osal_msleep(time);
}
