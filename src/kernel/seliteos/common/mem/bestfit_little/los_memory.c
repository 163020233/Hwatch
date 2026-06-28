/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2023. All rights reserved.
 * Description: LiteOS Mem Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-05-12
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

#include "los_memory_pri.h"
#include "los_memory_internal.h"
#include "los_printf.h"

#define MemLockCreate(pool, intLock)    MEMLOCKCREATE_CALL(LosMemPoolInfo, (pool), (intLock))
#define MemLockDelete(pool)             MEMLOCKDELETE_CALL(LosMemPoolInfo, (pool))
#define MemLock(pool, intSave)          MEMLOCK_CALL(LosMemPoolInfo, (pool), (intSave))
#define MemUnlock(pool, intSave)        MEMUNLOCK_CALL(LosMemPoolInfo, (pool), (intSave))

#define LOS_TRACE(TYPE, ...)

#ifdef LOSCFG_KERNEL_MEM_ALLOC
LITE_OS_SEC_RODATA_USRSPACE UINT8 *m_aucSysMem1 = NULL;
#endif

MEMLOCKCREATE_FUNCTION(LosMemPoolInfo)
MEMLOCKDELETE_FUNCTION(LosMemPoolInfo)
MEMLOCK_FUNCTION(LosMemPoolInfo)
MEMUNLOCK_FUNCTION(LosMemPoolInfo)

UINT32 LOS_MemPoolInit(VOID *pool, UINT32 size, UINT32 attr)
{
    UINT32 intSave = 0;
    BOOL intLock;

    if ((pool == NULL) || (size < OS_MEM_MIN_POOL_SIZE) || (size > OS_MEM_MAX_POOL_SIZE) ||
        (((UINTPTR)pool + (UINTPTR)size) < (UINTPTR)pool)) {
        return LOS_NOK;
    }
    if (OsMemLockModeGet(attr, &intLock) != LOS_OK) {
        return LOS_NOK;
    }
    if (!IS_ALIGNED(size, OS_MEM_ALIGN_SIZE) || !IS_ALIGNED(pool, OS_MEM_ALIGN_SIZE)) {
        PRINT_WARN("pool [%p, %p) size 0x%x should be aligned with OS_MEM_ALIGN_SIZE\n",
                   pool, (UINTPTR)pool + size, size);
        size = OS_MEM_ALIGN(size, OS_MEM_ALIGN_SIZE) - OS_MEM_ALIGN_SIZE;
    }
    if (MemLockCreate(pool, intLock) != LOS_OK) {
        return LOS_NOK;
    }
    if (MemLock(pool, &intSave) != LOS_OK) {
        (VOID)MemLockDelete(pool);
        return LOS_NOK;
    }
    if (OsMemMulPoolInit(pool, size) != LOS_OK) {
        MemUnlock(pool, intSave);
        (VOID)MemLockDelete(pool);
        return LOS_NOK;
    }
    if (OsHeapInit(pool, size) == FALSE) {
        (VOID)OsMemMulPoolDeinit(pool);
        MemUnlock(pool, intSave);
        (VOID)MemLockDelete(pool);
        return LOS_NOK;
    }
    MemUnlock(pool, intSave);
    LOS_TRACE(MEM_INFO_REQ, pool);
    return LOS_OK;
}

UINT32 LOS_MemInit(VOID *pool, UINT32 size)
{
    return LOS_MemPoolInit(pool, size, OsMemAttrGet(pool, size));
}

UINT32 LOS_MemDeInit(const VOID *pool)
{
    if (pool == NULL) {
        return LOS_NOK;
    }
    return MemLockDelete(pool);
}

#ifdef LOSCFG_KERNEL_MEM_ALLOC
UINT32 OsMemSystemInit(UINTPTR memStart)
{
    UINT32 ret;

    m_aucSysMem1 = (UINT8 *)memStart;
    ret = LOS_MemInit(m_aucSysMem1, (UINT32)OS_SYS_MEM_SIZE);
    PRINT_INFO("LiteOS system heap memory address: %p,size: 0x%x\n", m_aucSysMem1, (UINT32)OS_SYS_MEM_SIZE);
    return ret;
}
#endif

VOID *OsMemAllocNoLock(VOID *pool, UINT32 size)
{
    VOID *ptr = NULL;

    if (size == 0) {
        return NULL;
    }

    ptr = OsHeapAlloc(pool, size);
    return ptr;
}

VOID *OsMemAllocAlignNoLock(VOID *pool, UINT32 size, UINT32 boundary)
{
    return OsHeapAllocAlign(pool, size, boundary);
}

UINT32 OsMemFreeNoLock(const VOID *pool, const VOID *ptr)
{
    BOOL ret = FALSE;

    ret = OsHeapFree(pool, ptr);

    return ((ret == TRUE) ? LOS_OK : LOS_NOK);
}

VOID *LOS_MemAlloc(VOID *pool, UINT32 size)
{
    VOID *ptr = NULL;
    UINT32 intSave = 0;

    if (pool == NULL) {
        return ptr;
    }

    if (MemLock(pool, &intSave) != LOS_OK) {
        return ptr;
    }

    ptr = OsMemAllocNoLock(pool, size);

    MemUnlock(pool, intSave);

    LOS_TRACE(MEM_ALLOC, pool, (UINTPTR)ptr, size);
    return ptr;
}

VOID *LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary)
{
    VOID *ptr = NULL;
    UINT32 intSave = 0;

    if (pool == NULL) {
        return NULL;
    }

    if (MemLock(pool, &intSave) != LOS_OK) {
        return ptr;
    }

    ptr = OsMemAllocAlignNoLock(pool, size, boundary);
    MemUnlock(pool, intSave);

    LOS_TRACE(MEM_ALLOC_ALIGN, pool, (UINTPTR)ptr, size, boundary);
    return ptr;
}

VOID *LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size)
{
    VOID *retPtr = NULL;
    VOID *freePtr = NULL;
    UINT32 intSave = 0;
    struct LosHeapNode *node = NULL;
    UINT32 cpySize;
    UINT32 gapSize;
    errno_t rc;

    if (pool == NULL) {
        return NULL;
    }

    /* Zero-size requests are treated as free. */
    if ((ptr != NULL) && (size == 0)) {
        if (LOS_MemFree(pool, ptr) != LOS_OK) {
            PRINT_ERR("LOS_MemFree error!\n");
        }
    } else if (ptr == NULL) { // Requests with NULL pointers are treated as malloc.
        retPtr = LOS_MemAlloc(pool, size);
    } else {
        if (MemLock(pool, &intSave) != LOS_OK) {
            return NULL;
        }

        /* find the real ptr through gap size */
        gapSize = *((UINTPTR *)((UINTPTR)ptr - sizeof(UINTPTR)));
        if (OS_MEM_GET_ALIGN_FLAG(gapSize) != 0) {
            MemUnlock(pool, intSave);
            return NULL;
        }
        node = ((struct LosHeapNode *)ptr) - 1;
        cpySize = (size > (node->size)) ? (node->size) : size;

        MemUnlock(pool, intSave);

        retPtr = LOS_MemAlloc(pool, size);
        if (retPtr != NULL) {
            rc = memcpy_s(retPtr, size, ptr, cpySize);
            if (rc == EOK) {
                freePtr = ptr;
            } else {
                freePtr = retPtr;
                retPtr = NULL;
            }

            if (LOS_MemFree(pool, freePtr) != LOS_OK) {
                PRINT_ERR("LOS_MemFree error!\n");
            }
        }
    }

    return retPtr;
}

UINT32 LOS_MemFree(const VOID *pool, const VOID *ptr)
{
    UINT32 ret;
    UINT32 intSave = 0;

    if ((pool == NULL) || (ptr == NULL)) {
        return LOS_NOK;
    }

    if (MemLock(pool, &intSave) != LOS_OK) {
        return LOS_NOK;
    }

    ret = OsMemFreeNoLock(pool, ptr);

    MemUnlock(pool, intSave);

    LOS_TRACE(MEM_FREE, pool, (UINTPTR)ptr);
    return ret;
}

UINT32 LOS_MemInfoGet(const VOID *pool, LOS_MEM_POOL_STATUS *poolStatus)
{
    LosHeapStatus heapStatus;
    UINT32 err;
    UINT32 intSave = 0;

    if ((pool == NULL) || (poolStatus == NULL)) {
        return LOS_NOK;
    }

    if (MemLock(pool, &intSave) != LOS_OK) {
        return LOS_NOK;
    }

    err = OsHeapStatisticsGet(pool, &heapStatus);
    if (err != LOS_OK) {
        MemUnlock(pool, intSave);
        return LOS_NOK;
    }

    poolStatus->totalUsedSize   = heapStatus.totalUsedSize;
    poolStatus->totalFreeSize   = heapStatus.totalFreeSize;
    poolStatus->maxFreeNodeSize = heapStatus.maxFreeNodeSize;
    poolStatus->usedNodeNum     = heapStatus.usedNodeNum;
    poolStatus->freeNodeNum     = heapStatus.freeNodeNum;

#ifdef LOSCFG_MEM_TASK_STAT
    poolStatus->usageWaterLine = heapStatus.usageWaterLine;
#endif
    MemUnlock(pool, intSave);
    return LOS_OK;
}

UINT32 LOS_MemTotalUsedGet(const VOID *pool)
{
    LosHeapStatus heapStatus;
    UINT32 err;
    UINT32 intSave = 0;

    if (pool == NULL) {
        return LOS_NOK;
    }

    if (MemLock(pool, &intSave) != LOS_OK) {
        return LOS_NOK;
    }
    err = OsHeapStatisticsGet(pool, &heapStatus);
    MemUnlock(pool, intSave);
    if (err != LOS_OK) {
        return LOS_NOK;
    }

    return heapStatus.totalUsedSize;
}

UINT32 LOS_MemPoolSizeGet(const VOID *pool)
{
    const struct LosHeapManager *heapManager = NULL;

    if (pool == NULL) {
        return LOS_NOK;
    }

    heapManager = (const struct LosHeapManager *)pool;
    return heapManager->size;
}

UINTPTR LOS_MemLastUsedGet(const VOID *pool)
{
    if (pool == NULL) {
        return LOS_NOK;
    }
    return OsHeapLastUsedGet(pool);
}

/* bestfit_little do not support get used block count */
UINT32 LOS_MemUsedBlksGet(const VOID *pool)
{
    (VOID)pool;
    return OS_INVALID;
}

/* bestfit_little do not support get owner's task id */
UINT32 LOS_MemTaskIdGet(const VOID *ptr)
{
    (VOID)ptr;
    return OS_INVALID;
}

/*
 * Description : print heap information
 * Input       : pool --- Pointer to the manager, to distinguish heap
 */
VOID OsMemInfoPrint(const VOID *pool)
{
    struct LosHeapManager *heapMan = (struct LosHeapManager *)pool;
    LosHeapStatus status = {0};

    if (OsHeapStatisticsGet(heapMan, &status) == LOS_NOK) {
        return;
    }

    PRINT_INFO("pool addr    pool size    used size    free size    max free    alloc Count    free Count\n");
    PRINT_INFO("0x%-8x   0x%-8x   0x%-8x    0x%-8x   0x%-8x   0x%-8x     0x%-8x\n",
               pool, heapMan->size, status.totalUsedSize, status.totalFreeSize, status.maxFreeNodeSize,
               status.usedNodeNum, status.freeNodeNum);
}
