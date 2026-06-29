/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: dsp timer
 * Author: Audio
 */

#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/xtruntime.h>
#include <xtensa/hal.h>
#include "audio_debug.h"

#include "adsp_interrupt.h"
#include "dsp_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static dsp_timer_source g_dsp_timer;

static td_void dsp_timer_mod(dsp_timer_id id, td_u32 interval)
{
    td_ulong old_cmp;

    old_cmp = xthal_get_ccompare(id);
    xthal_set_ccompare(id, old_cmp + interval);
}

static td_void dsp_timer_disable(dsp_timer_id id)
{
    if (id == DSP_TIMER0) {
        dsp_interrupt_off(XCHAL_TIMER0_INTERRUPT);
    } else if (id == DSP_TIMER1) {
        dsp_interrupt_off(XCHAL_TIMER1_INTERRUPT);
    } else {
    }
}

static td_void dsp_timer_enable(dsp_timer_id id)
{
    if (id == DSP_TIMER0) {
        xthal_set_ccompare(0, xthal_get_ccount() + g_dsp_timer.timer[DSP_TIMER0].timer_interval);
        dsp_interrupt_on(XCHAL_TIMER0_INTERRUPT);
    } else if (id == DSP_TIMER1) {
        xthal_set_ccompare(1, xthal_get_ccount() + g_dsp_timer.timer[DSP_TIMER1].timer_interval);
        dsp_interrupt_on(XCHAL_TIMER1_INTERRUPT);
    } else {
    }
}

static td_void dsp_timer_callback(const dsp_timer_instance *timer)
{
    if (timer->isr_func != TD_NULL) {
        timer->isr_func(timer->context);
    }

    dsp_timer_mod(timer->timer_id, timer->timer_interval);
}

static td_void dsp_timer0_func(td_void)
{
    dsp_timer_callback(&g_dsp_timer.timer[DSP_TIMER0]);
}

static td_void dsp_timer1_func(td_void)
{
    dsp_timer_callback(&g_dsp_timer.timer[DSP_TIMER1]);
}

td_void timer_reg_isr_func(dsp_timer_id id, td_void (*isr_func)(td_void *), td_void *context)
{
    if (id >= DSP_TIMER_MAX) {
        return;
    }

    g_dsp_timer.timer[id].isr_func = isr_func;
    g_dsp_timer.timer[id].context = context;
}

td_void timer_del_isr_func(dsp_timer_id id)
{
    if (id >= DSP_TIMER_MAX) {
        return;
    }

    g_dsp_timer.timer[id].isr_func = TD_NULL;
}

td_void timer_set_enable(dsp_timer_id id)
{
    dsp_timer_enable(id);
}

td_void timer_set_disable(dsp_timer_id id)
{
    dsp_timer_disable(id);
}

td_void timer_init(dsp_timer_id id, td_u32 interval)
{
    if (id >= DSP_TIMER_MAX) {
        return;
    }

    g_dsp_timer.timer[id].timer_id = id;
    g_dsp_timer.timer[id].timer_interval = interval;

    switch (id) {
        case DSP_TIMER0:
            dsp_interrupt_setup(XCHAL_TIMER0_INTERRUPT, dsp_timer0_func);
            xthal_set_ccompare(0, xthal_get_ccount() + interval);
            break;

        case DSP_TIMER1:
            dsp_interrupt_setup(XCHAL_TIMER1_INTERRUPT, dsp_timer1_func);
            xthal_set_ccompare(1, xthal_get_ccount() + interval);
            break;

        default:
            break;
    }
}

td_void timer_deinit(dsp_timer_id id)
{
    dsp_timer_disable(id);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
