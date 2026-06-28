/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi3z cache
 * Author: audio
 */

#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include "audio_type.h"

td_void cache_init(td_void)
{
    const struct {
        td_u32 addr;
        td_u32 cattr;
    } cache_cfg[] = {
        {0x00000000, XCHAL_CA_BYPASS},
        {0x20000000, XCHAL_CA_BYPASS},
        {0x40000000, XCHAL_CA_BYPASS},
        {0x60000000, XCHAL_CA_BYPASS},
        {0x80000000, XCHAL_CA_BYPASS},
        {0xa0000000, XCHAL_CA_WRITEBACK}, /* for common mem */
        {0xc0000000, XCHAL_CA_WRITEBACK}, /* for ocram */
        {0xe0000000, XCHAL_CA_BYPASS},
    };

    td_u32 i;

    for (i = 0; i < sizeof(cache_cfg) / sizeof(cache_cfg[0]); i++) {
        td_void *p = (td_void *)(uintptr_t)cache_cfg[i].addr;
        xthal_set_region_attribute(p, 0x20000000, cache_cfg[i].cattr, XTHAL_CAFLAG_EXACT);
    }
}
