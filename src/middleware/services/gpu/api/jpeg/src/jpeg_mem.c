/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg mem
 */

#include "jpeg_mem.h"
#include <stdlib.h>
#include <stdio.h>
#include "securec.h"
#include "soc_osal.h"
#include "gpu_mem_config.h"
#include "jpeg_struct.h"

/*-------------------------------------- macro define -------------------------------------*/

#define JPEG_MEM_SIZE_MIN    1
#define JPEG_MEM_SIZE_MAX    (5 * 1024 * 1024)

/*-------------------------------------- struct info --------------------------------------*/

/*-------------------------------------- func define --------------------------------------*/

/*-------------------------------------- func release -------------------------------------*/

td_s32 jpeg_mem_alloc_small(jpeg_mem_ptr mem)
{
    errno_t ret;

    if (mem->size == 0) {
        gpu_err("mem->size = 0\n");
        return EXT_FAILURE;
    }

    mem->vir_buf = (td_char *)malloc(mem->size);
    if (mem->vir_buf == TD_NULL) {
        gpu_err("mem alloc fail, size = %u\n", mem->size);
        return EXT_FAILURE;
    }

    ret = memset_s(mem->vir_buf, mem->size, 0x0, mem->size);
    if (ret != EOK) {
        gpu_err("memset_s fail, mem size = %u\n", mem->size);
        free(mem->vir_buf);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_void jpeg_mem_free_small(jpeg_mem_ptr mem)
{
    if (mem->vir_buf != TD_NULL) {
        free(mem->vir_buf);
        mem->vir_buf = TD_NULL;
    }
}

td_s32 jpeg_mem_alloc_large(jpeg_mem_ptr mem)
{
    errno_t ret;

    if ((mem->size < JPEG_MEM_SIZE_MIN) || (mem->size > JPEG_MEM_SIZE_MAX)) {
        gpu_err("mem size = %u is invalid, need in range[%u, %u]\n", mem->size, JPEG_MEM_SIZE_MIN, JPEG_MEM_SIZE_MAX);
        return EXT_FAILURE;
    }

    mem->phy_buf = GPU_PSRAM_ADDR;
    if (mem->phy_buf == 0) {
        gpu_err("Invalid value, mem->phy_buf = 0\n");
        return EXT_FAILURE;
    }

    mem->vir_buf = (td_char *)(uintptr_t)mem->phy_buf;
    if (mem->vir_buf == TD_NULL) {
        return EXT_FAILURE;
    }

    ret = memset_s(mem->vir_buf, mem->size, 0x0, mem->size);
    if (ret != EOK) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_void jpeg_mem_free_large(const jpeg_mem_ptr mem)
{
    uapi_unused(mem);

    return;
}
