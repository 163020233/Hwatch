/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: mmc block Implementation
 * Author: Huawei LiteOS Team
 * Create: 2019-06-23
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

#include "kal_base.h"
#include "hci.h"

LITE_OS_SEC_TEXT uintptr_t LOS_Align(uintptr_t addr, uint32_t boundary)
{
    return (addr + boundary - 1) & ~((uintptr_t)(boundary - 1));
}

static void DCacheOperate(uintptr_t baseAddr, uint32_t size, uint32_t config)
{
    uint32_t flushNum;
    uint32_t count;
    uintptr_t startAddr = baseAddr & ~(CACHE_LINE_SIZE - 1);
    uintptr_t endAddr = baseAddr + size;

    endAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);

    flushNum = (endAddr - startAddr) / CACHE_LINE_SIZE;
    for (count = 0; count < flushNum; count++) {
        mb();
        WRITE_CUSTOM_CSR_VAL(DCINCVA, startAddr); /* write cmo's va */
        WRITE_CUSTOM_CSR_VAL(DCMAINT, config);
        mb();
        startAddr += CACHE_LINE_SIZE;
    }
}

LITE_OS_SEC_TEXT void ArchDCacheInvByVa(uintptr_t baseAddr, uint32_t size)
{
    uintptr_t opStartAddr = baseAddr & ~(CACHE_LINE_SIZE - 1);
    uintptr_t opEndAddr = (baseAddr + size) & ~(CACHE_LINE_SIZE - 1);

    if (opStartAddr != baseAddr) {
        DCacheOperate(opStartAddr, CACHE_LINE_SIZE, DCACHE_CLEAN_BY_VA);
    }

    if (opEndAddr != (baseAddr + size)) {
        DCacheOperate(opEndAddr, CACHE_LINE_SIZE, DCACHE_CLEAN_BY_VA);
    }
    DCacheOperate(baseAddr, size, DCACHE_INV_BY_VA);
}

LITE_OS_SEC_TEXT void ArchDCacheCleanByVa(uintptr_t baseAddr, uint32_t size)
{
    DCacheOperate(baseAddr, size, DCACHE_CLEAN_BY_VA);
}

LITE_OS_SEC_TEXT uint32_t ArchDCacheEnable(CacheSize dcclSize)
{
    /* linxCore m-profile cache size: a read-only field */
    (void)dcclSize;
    mb();
    WRITE_CUSTOM_CSR_VAL(DCCTL, DCCTL_ENABLE);
    mb();
    return LOS_OK;
}

LITE_OS_SEC_TEXT void ArchDCacheInvByAddr(uintptr_t startAddr, uintptr_t endAddr)
{
    uint32_t size;
    uintptr_t opStartAddr = startAddr & ~(CACHE_LINE_SIZE - 1);
    uintptr_t opEndAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (uint32_t)(opEndAddr - opStartAddr);

    ArchDCacheInvByVa(opStartAddr, size);
}

LITE_OS_SEC_TEXT void ArchDCacheCleanByAddr(uintptr_t startAddr, uintptr_t endAddr)
{
    uint32_t size;
    uintptr_t opStartAddr = startAddr & ~(CACHE_LINE_SIZE - 1);
    uintptr_t opEndAddr = LOS_Align(endAddr, CACHE_LINE_SIZE);
    size = (uint32_t)(opEndAddr - opStartAddr);

    ArchDCacheCleanByVa(opStartAddr, size);
}

void dma_cache_inv1(uintptr_t start, uintptr_t end)
{
    ArchDCacheInvByAddr(start, end);
}

void dma_cache_clean1(uintptr_t start, uintptr_t end)
{
    ArchDCacheCleanByAddr(start, end);
}

void hci_dma_clean_cache(void *addr, uint32_t size)
{
    LOS_ASSERT(size != 0);
    LOS_ASSERT((size & (CACHE_ALIGNED_SIZE - 1)) == 0);
    LOS_ASSERT(((uintptr_t)addr & (CACHE_ALIGNED_SIZE - 1)) == 0);

    uintptr_t start = (uintptr_t)addr & ~(CACHE_ALIGNED_SIZE - 1);
    uintptr_t end = (uintptr_t)addr + size;
    end = LOS_Align(end, CACHE_ALIGNED_SIZE);
    mmc_dcache_flush_range(start, end);
}

void hci_dma_inv_cache(void *addr, uint32_t size)
{
    LOS_ASSERT(size != 0);
    LOS_ASSERT((size & (CACHE_ALIGNED_SIZE - 1)) == 0);
    LOS_ASSERT(((uintptr_t)addr & (CACHE_ALIGNED_SIZE - 1)) == 0);

    uintptr_t start = (uintptr_t)addr & ~(CACHE_ALIGNED_SIZE - 1);
    uintptr_t end = (uintptr_t)addr + size;
    end = LOS_Align(end, CACHE_ALIGNED_SIZE);
    mmc_dcache_inv_range(start, end);
}