/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2013. All rights reserved.
 * Description: Add callerRa use wrap build option
 * Author: Huawei LiteOS Team
 * Create: 2013-07-11
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

#include "los_task_pri.h"
#include "los_memory_internal.h"
#ifdef LOSCFG_COMPAT_CMSIS_VER_1
#include "cmsis_os1.h"
#endif

#define NODE_USED 1
STATIC UINT32 OsDfxGetCurTaskId(VOID)
{
    UINT32 taskId;

    if ((OsCurrTaskGet() != NULL) && OS_INT_INACTIVE) {
        taskId = LOS_CurTaskIDGet();
    } else {
        taskId = TASK_NUM - 1;
    }
    return taskId;
}

UINTPTR *g_osCallerRA = NULL;

STATIC VOID OsSaveCallerRa(UINTPTR callerRa)
{
    UINT32 taskId =  OsDfxGetCurTaskId();
    g_osCallerRA[taskId] = callerRa;
}

STATIC BOOL OsIsSetCallerRa(VOID)
{
    UINT32 taskId =  OsDfxGetCurTaskId();
    return (g_osCallerRA[taskId] != (UINTPTR)0);
}

VOID LOS_SaveCallerRa(UINTPTR callerRa)
{
    if (OsIsSetCallerRa() != TRUE) {
        OsSaveCallerRa(callerRa);
    }
}

VOID LOS_ResetCallerRa(VOID)
{
    OsSaveCallerRa(0);
}

BOOL LOS_IsSetCallerRa(VOID)
{
    return OsIsSetCallerRa();
}

static const OsMemDfxOps *g_MemDfxOps = NULL;
VOID LOS_MemDfxOpsReg(const OsMemDfxOps *memDfxOps)
{
    g_MemDfxOps = memDfxOps;
}

VOID LOS_MemAllocDfxHook(VOID *addr, UINT32 size, UINTPTR callerRa)
{
    if (addr == NULL) {
        OsSaveCallerRa((UINTPTR)0);
    }

    UINT32 taskID = OsDfxGetCurTaskId();
    if (g_MemDfxOps != NULL && g_MemDfxOps->memAllocDfxProc != NULL) {
        g_MemDfxOps->memAllocDfxProc(addr, size, callerRa, taskID);
    }
    return;
}

VOID LOS_MemFreeDfxHook(VOID *addr)
{
    if (g_MemDfxOps != NULL && g_MemDfxOps->memFreeDfxProc != NULL) {
        g_MemDfxOps->memFreeDfxProc(addr);
    }
    return;
}

VOID *__real_LOS_MemAlloc(VOID *pool, UINT32 size);
VOID *__wrap_LOS_MemAlloc(VOID *pool, UINT32 size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemAlloc(pool, size);
    LOS_ResetCallerRa();
    return ret;
}

VOID *__real_LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary);
VOID *__wrap_LOS_MemAllocAlign(VOID *pool, UINT32 size, UINT32 boundary)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemAllocAlign(pool, size, boundary);
    LOS_ResetCallerRa();

    return ret;
}

VOID *__real_LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size);
VOID *__wrap_LOS_MemRealloc(VOID *pool, VOID *ptr, UINT32 size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemRealloc(pool, ptr, size);
    LOS_ResetCallerRa();

    return ret;
}

#ifdef LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK
#define CALLERRA_GET_ADDR(node) ((UINTPTR *)((UINT8 *)(node) + OS_MEM_NODE_HEAD_SIZE))
#define RECORD_CALLER_RA(node, ra) do { *CALLERRA_GET_ADDR((node)) = (UINTPTR)(ra); } while (0)
#define GET_CALLER_RA(node) (*CALLERRA_GET_ADDR((MEM_NODE_STRUCT)(node)))

#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
STATIC BOOL OsSlabMemIsHeld(VOID *pool, VOID *ptr)
{
    struct LosSlabControlHeader *slabMem = OsSlabCtrlHdrGet(pool);
    OsSlabBlockNode *slabNode = OS_SLAB_BLOCK_HEAD_GET(ptr);
    UINT32 idx;

    if ((slabMem->enabled == FALSE) || (!OS_ALLOC_FROM_SLAB_CHECK(slabNode))) {
        return FALSE;
    }

    for (idx = 0; idx < SLAB_MEM_COUNT; idx++) {
        if (slabMem->slabClass[idx].blkSz < OS_SLAB_BLOCK_SIZE_GET(slabNode)) {
            continue;
        }
        return TRUE;
    }
    return FALSE;
}
#endif /* LOSCFG_KERNEL_MEM_SLAB_EXTENTION */

UINT32 __real_LOS_MemFree(VOID *pool, VOID *ptr);
UINT32 __wrap_LOS_MemFree(VOID *pool, VOID *ptr)
{
    if ((pool == NULL) || (ptr == NULL)) {
        return LOS_NOK;
    }
#ifdef LOSCFG_KERNEL_MEM_BESTFIT
    if (!IS_ALIGNED(pool, sizeof(VOID *)) || !IS_ALIGNED(ptr, sizeof(VOID *))) {
        return LOS_NOK;
    }
    if ((ptr < pool) || ((UINT8 *)ptr >= (UINT8 *)pool + ((LosMemPoolInfo *)pool)->poolSize)) {
        return LOS_NOK;
    }
#else
    if ((ptr < pool) || ((UINT8 *)ptr > (UINT8 *)pool + (((LosMemPoolInfo *)pool)->size) + sizeof(LosMemPoolInfo))) {
        return LOS_NOK;
    }
#endif
    UINT32 ret;
#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
    /* slab memfree doesn't support memory leak and memory double-free detect */
    if (OsSlabMemIsHeld(pool, ptr)) {
        ret = __real_LOS_MemFree(pool, ptr);
        return ret;
    }
#endif

    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
#ifdef LOSCFG_KERNEL_MEM_BESTFIT
    LosMemDynNode *node = OsGetRealPtr(pool, ptr);
    BOOL nodeIsUsed = OS_MEM_NODE_GET_USED_FLAG(node->selfNode.sizeAndFlag);
#else
    UINT32 gapSize = *((UINT32 *)((UINTPTR)ptr - sizeof(gapSize)));
    if (OS_MEM_GET_ALIGN_FLAG(gapSize)) {
        gapSize = OS_MEM_GET_ALIGN_GAPSIZE(gapSize);
        ptr = (VOID *)((UINTPTR)ptr - gapSize);
    }
    struct LosHeapNode *node = ((struct LosHeapNode *)ptr) - 1;
    BOOL nodeIsUsed = (node->used == NODE_USED);
#endif
    if (!nodeIsUsed) {
        UINTPTR firstFreeCallerRa = GET_CALLER_RA(node);
        UINTPTR doubleFreeCallerRa = g_osCallerRA[OsCurrTaskGet()->taskId];
        PRINTK("Memory double free detected!\n");
        PRINTK("Current running taskId = %d, taskName = %s.\n", OsCurrTaskGet()->taskId, OsCurrTaskGet()->taskName);
        PRINTK("Memory first free function addr = 0x%p.\nMemory double free function addr = 0x%p.\n",
            firstFreeCallerRa, doubleFreeCallerRa);
    }
    ret = __real_LOS_MemFree(pool, ptr);
    RECORD_CALLER_RA(node, g_osCallerRA[OsCurrTaskGet()->taskId]);
    LOS_ResetCallerRa();
    return ret;
}
#endif

#ifdef LOSCFG_KERNEL_MEM_BESTFIT
#ifdef LOSCFG_MEM_MUL_MODULE
VOID *__real_LOS_MemMalloc(VOID *pool, UINT32 size, UINT32 moduleId);
VOID *__wrap_LOS_MemMalloc(VOID *pool, UINT32 size, UINT32 moduleId)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemMalloc(pool, size, moduleId);
    LOS_ResetCallerRa();

    return ret;
}

VOID *__real_LOS_MemMallocAlign(VOID *pool, UINT32 size, UINT32 boundary, UINT32 moduleId);
VOID *__wrap_LOS_MemMallocAlign(VOID *pool, UINT32 size, UINT32 boundary, UINT32 moduleId)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemMallocAlign(pool, size, boundary, moduleId);
    LOS_ResetCallerRa();

    return ret;
}

VOID *__real_LOS_MemMrealloc(VOID *pool, VOID *ptr, UINT32 size, UINT32 moduleId);
VOID *__wrap_LOS_MemMrealloc(VOID *pool, VOID *ptr, UINT32 size, UINT32 moduleId)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemMrealloc(pool, ptr, size, moduleId);
    LOS_ResetCallerRa();
    return ret;
}
#endif /* LOSCFG_MEM_MUL_MODULE */
#endif /* LOSCFG_KERNEL_MEM_BESTFIT */

#if (defined(LOSCFG_KERNEL_MEMBOX_STATIC) || defined(LOSCFG_KERNEL_MEMBOX_DYNAMIC))
VOID *__real_LOS_MemboxAlloc(VOID *pool);
VOID *__wrap_LOS_MemboxAlloc(VOID *pool)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_LOS_MemboxAlloc(pool);
    LOS_ResetCallerRa();
    return ret;
}
#endif

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
VOID *__real_osPoolAlloc(osPoolId pool_id);
VOID *__wrap_osPoolAlloc(osPoolId pool_id)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_osPoolAlloc(pool_id);
    LOS_ResetCallerRa();
    return ret;
}

VOID *__real_osPoolCAlloc(osPoolId pool_id);
VOID *__wrap_osPoolCAlloc(osPoolId pool_id)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    VOID *ret = __real_osPoolAlloc(pool_id);
    LOS_ResetCallerRa();
    return ret;
}
#endif

#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
OsSlabAllocator *__real_OsSlabAllocatorNew(VOID *pool, UINT32 itemSz, UINT32 itemAlign, UINT32 numItems);
OsSlabAllocator *__wrap_OsSlabAllocatorNew(VOID *pool, UINT32 itemSz, UINT32 itemAlign, UINT32 numItems)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    OsSlabAllocator *allocator = __real_OsSlabAllocatorNew(pool, itemSz, itemAlign, numItems);
    LOS_ResetCallerRa();
    return allocator;
}
#endif /* LOSCFG_KERNEL_MEM_SLAB_EXTENTION */

#if (defined(LOSCFG_KERNEL_CPPSUPPORT) || defined(LOSCFG_COMPAT_POSIX))
void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    void *ret = __real_malloc(size);
    LOS_ResetCallerRa();
    return ret;
}

void *__real_realloc(void *ptr, size_t size);
void *__wrap_realloc(void *ptr, size_t size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    void *ret = __real_realloc(ptr, size);
    LOS_ResetCallerRa();
    return ret;
}

void *__real_calloc(size_t nitems, size_t size);
void *__wrap_calloc(size_t nitems, size_t size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    void *ret = __real_calloc(nitems, size);
    LOS_ResetCallerRa();
    return ret;
}

void *__real_zalloc(size_t size);
void *__wrap_zalloc(size_t size)
{
    LOS_SaveCallerRa((UINTPTR)__builtin_return_address(0));
    void *ret = __real_zalloc(size);
    LOS_ResetCallerRa();
    return ret;
}
#endif /* LOSCFG_KERNEL_CPPSUPPORT || LOSCFG_COMPAT_POSIX */
