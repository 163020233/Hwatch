/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio debug
 * Author: audio
 * Create: 2019-12-19
 */

#include <xtensa/xtruntime.h>
#include <xtensa/hal.h>
#include "audio_type.h"
#include "audio_osal.h"

td_u32 audio_osal_irq_lock(td_void)
{
    return XTOS_SET_INTLEVEL(0xf); /* 0xf: int level mask */
}

td_void audio_osal_irq_restore(td_u32 level)
{
    XTOS_RESTORE_INTLEVEL(level);
}

td_s32 audio_osal_irq_request(td_u32 irq, audio_osal_irq_handler handler, audio_osal_irq_handler thread_fn,
    const td_char *name, td_void *dev)
{
    audio_unused(dev);
    audio_unused(name);
    audio_unused(thread_fn);
    _xtos_set_interrupt_handler(irq, (_xtos_handler)handler);

    return 0;
}

td_void audio_osal_irq_free(td_u32 irq, td_void *dev)
{
    audio_unused(dev);
    xthal_set_intclear(irq);
}

td_s32 audio_osal_irq_enable(td_u32 irq)
{
    _xtos_interrupt_enable(irq);

    return 0;
}

td_s32 audio_osal_irq_disable(td_u32 irq)
{
    _xtos_interrupt_disable(irq);

    return 0;
}
