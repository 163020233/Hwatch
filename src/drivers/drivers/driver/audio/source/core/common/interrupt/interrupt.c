/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: Utility set.
 * Author: audio
 */

#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "audio_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void dsp_int_en(td_u32 int_id)
{
    _xtos_interrupt_enable(int_id);
}

td_void dsp_int_disable(td_u32 int_id)
{
    _xtos_interrupt_disable(int_id);
}

td_void dsp_interrupt_on(td_u32 int_id)
{
    _xtos_ints_on(1 << int_id);
}

td_void dsp_interrupt_off(td_u32 int_id)
{
    _xtos_ints_off(1 << int_id);
}

td_void dsp_int_off_all(td_void)
{
    _xtos_ints_off(0xffffffff);
}

td_void dsp_interrupt_disable_all(td_void)
{
    _xtos_ints_off(0xffffffff);
}

td_void dsp_int_clc_all(td_void)
{
    xthal_set_intclear(0xffffffff);
}

td_void dsp_interrupt_clear(td_u32 int_id)
{
    xthal_set_intclear(int_id);
}

td_void dsp_interrupt_trigger(td_u32 int_id)
{
    xthal_set_intset(1 << int_id);
}

td_void dsp_interrupt_setup(td_u32 int_id, td_void (*handler_interrupt)(td_void))
{
    _xtos_set_interrupt_handler(int_id, handler_interrupt);
}

td_void dsp_set_exception_handler(td_s32 exccause, td_void (*handler_exception)(td_void))
{
    _xtos_set_exception_handler(exccause, handler_exception);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
