/* ----------------------------------------------------------------------------
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Memory related implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#if defined(__LITEOS__)
#include "stdlib.h"
#include "los_memory.h"
#include "stdlib_mem_alloc_pri.h"
#ifdef LOSCFG_BOX
#include "los_task_pri.h"
#endif
#else
#include "../include/posix_adapt.h"
#include "drb_mem.h"
#endif

#if defined(__LITEOS__)
#define OS_SEC_L4_TEXT
#define DEFAULT_VISIBILITY
#define OS_WRAP_MEM_ALLOC(size)        LOS_MemAlloc(STD_LIBC_MEM_POOL, (size))
#define OS_WRAP_MEM_FREE(ptr)          LOS_MemFree(STD_LIBC_MEM_POOL, (ptr))
#define OS_WRAP_MEM_REALLOC(ptr, size) LOS_MemRealloc(STD_LIBC_MEM_POOL, (ptr), (unsigned int)(size))
#else
OS_SEC_L4_BSS U8 g_posixMemPtNo;
OS_SEC_L4_TEXT U32 OsPosixCstdlibInit(U8 ptNo)
{
    if ((ptNo >= g_maxPtNum) || OS_MEM_PT_IS_UNINIT(OS_MEM_PT_INFO(ptNo))) {
        return OS_ERRNO_MEM_FREE_PTNO_INVALID;
    }

    g_posixMemPtNo = ptNo;
    return 0;
}

static OS_SEC_L4_TEXT void *WrapMemAlloc(size_t size)
{
    void *ptr = DRB_MemAlloc(OS_MID_POSIX, g_posixMemPtNo, size);
    if (ptr == NULL) {
        errno = ENOMEM;
    }
    return ptr;
}

static OS_SEC_L4_TEXT size_t OsMemGetSize(void *p)
{
    struct TagFscMemCtrl *currBlk; /* 当前内存块指针 */
    uintptr_t blkTailMagic;

    currBlk = (struct TagFscMemCtrl *)OsMemGetHeadAddr((uintptr_t)p);
    if (currBlk->fscHead.magicWord != OS_MEM_HEAD_MAGICWORD) {
        return 0;
    }

    /* 找到内存越界检查魔术字的地址 */
    blkTailMagic = (uintptr_t)currBlk + (uintptr_t)(currBlk->size) - (uintptr_t)OS_FSC_MEM_TAIL_SIZE;
    return (unsigned char*)blkTailMagic - (unsigned char*)p;
}

static OS_SEC_L4_TEXT void *WrapMemRealloc(void *ptr, size_t size)
{
    void *newPtr;
    size_t oldSize;
    size_t newSize;

    oldSize = OsMemGetSize(ptr);
    if (oldSize == 0) {
        return NULL;
    } else if (oldSize == size) {
        return ptr;
    }

    newPtr = malloc(size);
    if (newPtr == NULL) {
        return newPtr;
    }

    newSize = (size < oldSize) ? size : oldSize;
    if (memcpy_s(newPtr, size, ptr, newSize) != EOK) {
        OS_GOTO_SYS_ERROR();
    }

    free(ptr);

    return newPtr;
}

#define OS_WRAP_MEM_ALLOC(size)        WrapMemAlloc((size))
#define OS_WRAP_MEM_FREE(ptr)          DRB_MemFree(OS_MID_POSIX, (ptr))
#define OS_WRAP_MEM_REALLOC(ptr, size) WrapMemRealloc((ptr), (size))

static bool OsIsPtrNull(void **ptr)
{
    return (ptr == NULL);
}

INLINE bool OsIsAlignInvalid(size_t align)
{
    return ((align == 0) || ((align & (align - 1)) != 0) ||
        ((align & (sizeof(void *) - 1)) != 0) || (align >= (1U << MEM_ADDR_BUTT)));
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT __attribute__((__nonnull__ (1))) int posix_memalign(void **memptr,
    size_t alignment, size_t size)
{
    enum MemAlign alignPow;

    if (OsIsPtrNull(memptr)) {
        return EINVAL;
    }

    if (OsIsAlignInvalid(alignment) || (size == 0)) {
        *memptr = NULL;
        return EINVAL;
    }

    alignPow = OsGetRMB((unsigned int)alignment);
    *memptr = DRB_MemAllocAlign(OS_MID_POSIX, g_posixMemPtNo, size, alignPow);
    if (*memptr == NULL) {
        return ENOMEM;
    }

    return ENOERR;
}
#endif

/*
 * Allocates the requested memory and returns a pointer to it. The requested
 * size is nitems each size bytes long (total memory requested is nitems*size).
 * The space is initialized to all zero bits.
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT void *calloc(size_t nitems, size_t size)
{
    size_t real_size;
    void *ptr = NULL;

    if (nitems == 0 || size == 0 || nitems > (UINT32_MAX / size)) {
        return NULL;
    }

    real_size = (size_t)(nitems * size);
#if defined LOSCFG_BOX && defined LOSCFG_KERNEL_MEM_ALLOC
#ifdef LOSCFG_FS_EXC_OPERATION
    if (!OS_IN_KER_STATE && !OS_EXC_ACTIVE_USER) {
#else
    if (!OS_IN_KER_STATE) {
#endif
        ptr = LOS_MemAlloc(STD_LIBC_MEM_POOL, (UINT32)real_size);
    } else {
        ptr = LOS_MemAlloc(STD_LIBC_MEM_POOL_KERNEL, (UINT32)real_size);
    }
#else
    ptr = OS_WRAP_MEM_ALLOC((unsigned int)real_size);
#endif
    if (ptr != NULL) {
        (void)memset((void *)ptr, 0, real_size);
    }
    return ptr;
}

/*
 * Deallocates the memory previously allocated by a call to calloc, malloc, or
 * realloc. The argument ptr points to the space that was previously allocated.
 * If ptr points to a memory block that was not allocated with calloc, malloc,
 * or realloc, or is a space that has been deallocated, then the result is undefined.
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT void free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

#if defined LOSCFG_BOX && defined LOSCFG_KERNEL_MEM_ALLOC
#ifdef LOSCFG_FS_EXC_OPERATION
    if (!OS_IN_KER_STATE && !OS_EXC_ACTIVE_USER) {
#else
    if (!OS_IN_KER_STATE) {
#endif
        (void)LOS_MemFree(STD_LIBC_MEM_POOL, ptr);
    } else {
        (void)LOS_MemFree(STD_LIBC_MEM_POOL_KERNEL, ptr);
    }
#else
    (void)OS_WRAP_MEM_FREE(ptr);
#endif
}

/*
 * Allocates the requested memory and returns a pointer to it. The requested
 * size is size bytes. The value of the space is indeterminate.
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT void *malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

#if defined LOSCFG_BOX && defined LOSCFG_KERNEL_MEM_ALLOC
#ifdef LOSCFG_FS_EXC_OPERATION
    if (!OS_IN_KER_STATE && !OS_EXC_ACTIVE_USER) {
#else
    if (!OS_IN_KER_STATE) {
#endif
        return (void *)LOS_MemAlloc(STD_LIBC_MEM_POOL, (UINT32)size);
    } else {
        return (void *)LOS_MemAlloc(STD_LIBC_MEM_POOL_KERNEL, (UINT32)size);
    }
#else
    return (void *)OS_WRAP_MEM_ALLOC((unsigned int)size);
#endif
}

/*
 * Attempts to resize the memory block pointed to by ptr that was previously
 * allocated with a call to malloc or calloc. The contents pointed to by ptr are
 * unchanged. If the value of size is greater than the previous size of the
 * block, then the additional bytes have an undeterminate value. If the value
 * of size is less than the previous size of the block, then the difference of
 * bytes at the end of the block are freed. If ptr is null, then it behaves like
 * malloc. If ptr points to a memory block that was not allocated with calloc
 * or malloc, or is a space that has been deallocated, then the result is
 * undefined. If the new space cannot be allocated, then the contents pointed
 * to by ptr are unchanged. If size is zero, then the memory block is completely
 * freed.
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        ptr = malloc(size);
        return ptr;
    }

    if (size == 0) {
        free(ptr);
        return NULL;
    }

#if defined LOSCFG_BOX && defined LOSCFG_KERNEL_MEM_ALLOC
#ifdef LOSCFG_FS_EXC_OPERATION
    if (!OS_IN_KER_STATE && !OS_EXC_ACTIVE_USER) {
#else
    if (!OS_IN_KER_STATE) {
#endif
        return (void *)LOS_MemRealloc(STD_LIBC_MEM_POOL, ptr, (UINT32)size);
    } else {
        return (void *)LOS_MemRealloc(STD_LIBC_MEM_POOL_KERNEL, ptr, (UINT32)size);
    }
#else
    return (void *)OS_WRAP_MEM_REALLOC(ptr, size);
#endif
}