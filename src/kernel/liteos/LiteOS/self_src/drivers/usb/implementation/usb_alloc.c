/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Usb Mem alloc File
 * Author: Huawei LiteOS Team
 * Create: 2025-07-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


#include <los_memory.h>

#include "usb_dma_cache.h"
#include "usb_alloc.h"

#ifndef unused
#define unused(...)
#endif

#ifdef LOSCFG_DRIVERS_USB_HOST_STANDALONE_MEM_POOL

void usb_host_mem_pool_get(uintptr_t *base_addr, unsigned long *size)
{
    if (base_addr != NULL)
        *base_addr = g_usb_mem_addr_start;

    if (size != NULL)
        *size = g_usb_mem_size;
}

void *usb_malloc_align(size_t size, unsigned int boundary)
{
    void *ptr = LOS_MemAllocAlign((void*)g_usb_mem_addr_start, size, boundary);
    if (ptr == NULL) {
        dprintf("usb_malloc_align failed, size = %d, boundary = %u\n", size, boundary);
        return NULL;
    }

    memset_s(ptr, size, 0, size);

    return ptr;
}

void *usb_malloc(size_t size, int x, int flag)
{
    unused(x);

    void *ptr = LOS_MemAlloc((void*)g_usb_mem_addr_start, size);
    if (ptr == NULL) {
        dprintf("usb_malloc:usb_malloc failed, size = %d\n", size);
        return NULL;
    }

    memset_s(ptr, size, 0, size);

    return ptr;
}

void usb_free(void *ptr, int x)
{
    unused(x);
    (void)LOS_MemFree((void*)g_usb_mem_addr_start, ptr);
}

#else

void *usb_malloc_align(size_t size, unsigned int boundary)
{
    return memalign(boundary, size);
}

void *usb_malloc(size_t size, int x, int flag)
{
    unused(flag);
    return zalloc(size);
}

void usb_free(void *ptr, int x)
{
    unused(x);
    free(ptr);
    return;
}
#endif