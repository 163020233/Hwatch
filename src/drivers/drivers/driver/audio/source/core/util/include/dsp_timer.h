/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2022. All rights reserved.
 * Description: dsp timer
 * Author: Audio
 */

#ifndef __DSP_TIMER_H__
#define __DSP_TIMER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    DSP_TIMER0,
    DSP_TIMER1,

    DSP_TIMER_MAX,
} dsp_timer_id;

typedef struct {
    dsp_timer_id timer_id;
    td_u32 timer_interval;
    td_void (*isr_func)(td_void *);
    td_void *context;
} dsp_timer_instance;

typedef struct {
    dsp_timer_instance timer[DSP_TIMER_MAX];
} dsp_timer_source;

td_void timer_init(dsp_timer_id id, td_u32 interval);
td_void timer_deinit(dsp_timer_id id);
td_void timer_set_enable(dsp_timer_id id);
td_void timer_set_disable(dsp_timer_id id);
td_void timer_reg_isr_func(dsp_timer_id id, td_void (*isr_func)(td_void *), td_void *context);
td_void timer_del_isr_func(dsp_timer_id id);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
