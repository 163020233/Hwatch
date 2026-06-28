/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi system
 * Author: audio
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <xtensa/hal.h>

#include "audio_type.h"
#include "audio_struct.h"

td_void system_init(td_void);
td_void system_deinit(td_void);

static inline td_void system_idle(td_void)
{
#ifndef SAP_FPGA
    __asm("waiti 0");
#endif
}

static inline audio_core_id system_get_core_id(td_void)
{
    if (xthal_get_prid() == 0) {
        return AUDIO_CORE_DSP0;
    } else {
        return AUDIO_CORE_DSP1;
    }
}

#endif
