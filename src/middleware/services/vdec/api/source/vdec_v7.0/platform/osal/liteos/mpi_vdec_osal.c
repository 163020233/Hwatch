/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "mpi_vdec_osal.h"
#include <unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include "soc_errno.h"

td_u8* vdec_mmap(td_u32 phy_buf, td_u32 size)
{
    td_u8* temp = TD_NULL;

    vdec_uapi_unused(size);

    if (phy_buf == 0) {
        return TD_NULL;
    }

    temp = (td_u8 *)(uintptr_t)phy_buf;

    return temp;
}

td_s32 vdec_umap(td_void *addr, td_u32 size)
{
    vdec_uapi_unused(addr);
    vdec_uapi_unused(size);
    return EXT_SUCCESS;
}

td_void vdec_msleep(td_u32 ms)
{
    td_u32 us = ms;

    if (ms < 0x100000) {
        us = ms * 1000; /* 1000 is unit */
    }

    usleep(us);
}

