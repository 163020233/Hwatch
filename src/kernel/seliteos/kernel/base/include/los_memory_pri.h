/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS Memory Module Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-06-01
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

#ifndef LOS_MEMORY_PRI_H
#define LOS_MEMORY_PRI_H

#include "los_memory.h"
#include "los_memstat_pri.h"
#include "los_mux.h"
#include "los_hwi.h"
#ifdef LOSCFG_MEM_TASK_STAT
#include "los_task_pri.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Memory pool information structure */
#if defined(LOSCFG_MEM_BESTFIT)

typedef struct LosHeapManager {
    VOID        *pool;      /* Starting address of a memory pool */
    UINT32      poolSize;   /* Memory pool size */
    UINT32      memMux;     /* Memory pool mutex lock */
#ifdef LOSCFG_MEM_SLAB_EXTENTION
    struct LosSlabControlHeader slabCtrlHdr;
#endif
#ifdef LOSCFG_MEM_TASK_STAT
    Memstat     stat;
#endif
} LosMemPoolInfo;

#elif defined(LOSCFG_MEM_BESTFIT_LITTLE)

typedef struct LosHeapManager {
    struct LosHeapNode *head;
    struct LosHeapNode *tail;
    UINT32      size;
    UINT32      memMux;     /* Memory pool mutex lock */
#ifdef LOSCFG_MEM_SLAB_EXTENTION
    struct LosSlabControlHeader slabCtrlHdr;
#endif
#ifdef LOSCFG_MEM_TASK_STAT
    Memstat     stat;
#endif
} LosMemPoolInfo;

#endif

#ifdef LOSCFG_MEM_TASK_STAT
STATIC INLINE UINT32 OsMemStatCntGet(VOID)
{
    OsKernelInfo info = { 0U };
    (VOID)OsResourceInfoGet(RESORURCE_TYPE_KERNEL, 0, &info, (UINT32)sizeof(info));
    return info.memTaskCount;
}

#define MEM_POOL_INFO_SIZE  (UINT32)(sizeof(LosMemPoolInfo) + (sizeof(TaskMemUsedInfo) * OsMemStatCntGet()))
#else
#define MEM_POOL_INFO_SIZE  (UINT32)(sizeof(LosMemPoolInfo))
#endif

#define OS_MEM_LOCK_TYPE_MUX 0U
#define OS_MEM_LOCK_TYPE_INT LOS_MEM_INIT_ATTR_LOCK_MSK

#define IS_ALIGNED(value, alignSize) ((((UINTPTR)(value)) & ((UINTPTR)((alignSize) - 1U))) == 0)

LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 OsMemLockModeGet(UINT32 attr, BOOL *intLock)
{
    if ((attr & LOS_MEM_INIT_ATTR_LOCK_MSK) == OS_MEM_LOCK_TYPE_MUX) {
        *intLock = FALSE;
    } else if ((attr & LOS_MEM_INIT_ATTR_LOCK_MSK) == OS_MEM_LOCK_TYPE_INT) {
        *intLock = TRUE;
    } else {
        return LOS_NOK;
    }
    return LOS_OK;
}

#define MEMLOCKCREATE_CALL(type, pool, intLock)     MemLockCreate##type(pool, intLock)
#define MEMLOCKCREATE_FUNCTION(type)                                                            \
    LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 MemLockCreate##type(VOID *pool, BOOL intLock)   \
    {                                                                                           \
        if (intLock) {                                                                          \
            ((type *)pool)->memMux = OS_INVALID;                                                \
        } else {                                                                                \
            ((type *)pool)->memMux = 0;                                                         \
            if (LOS_MuxCreate(&((type *)pool)->memMux) != LOS_OK) {                             \
                return LOS_NOK;                                                                 \
            }                                                                                   \
        }                                                                                       \
        return LOS_OK;                                                                          \
    }

#define MEMLOCKDELETE_CALL(type, pool)              MemLockDelete##type(pool)
#define MEMLOCKDELETE_FUNCTION(type)                                                            \
    LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 MemLockDelete##type(const VOID *pool)           \
    {                                                                                           \
        if (((type *)pool)->memMux != OS_INVALID) {                                             \
            if (LOS_MuxDelete(((const type *)pool)->memMux) != LOS_OK) {                        \
                return LOS_NOK;                                                                 \
            }                                                                                   \
        }                                                                                       \
        return LOS_OK;                                                                          \
    }

#define MEMLOCK_CALL(type, pool, intSave)           MemLock##type(pool, intSave)
#define MEMLOCK_FUNCTION(type)                                                                  \
    LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 MemLock##type(                                  \
        const VOID *pool, UINT32 *intSave)                                                      \
    {                                                        \
        UINT32 ret;                                          \
        if (((type *)pool)->memMux != OS_INVALID) {            \
            ret = LOS_MuxPend(((const type *)pool)->memMux, LOS_WAIT_FOREVER);       \
            if (ret != LOS_OK) {                              \
                PRINT_ERR("memlock failed ret:%x\n", ret);                       \
                return LOS_NOK;                                                                 \
            }                                                                                   \
        } else {                                                                                \
            *intSave = LOS_IntLock();                                                           \
        }                                                                                       \
        return LOS_OK;                                                                          \
    }

#define MEMUNLOCK_CALL(type, pool, intSave)         MemUnlock##type(pool, intSave)
#define MEMUNLOCK_FUNCTION(type)                                                                \
    LITE_OS_SEC_ALW_INLINE STATIC_INLINE VOID MemUnlock##type(                                  \
        const VOID *pool, const UINT32 intSave)                                                 \
    {                                                                                           \
        if (((type *)pool)->memMux != OS_INVALID) {                                             \
            (VOID)LOS_MuxPost(((const type *)pool)->memMux);                                    \
        } else {                                                                                \
            LOS_IntRestore(intSave);                                                            \
        }                                                                                       \
    }

#ifdef LOSCFG_KERNEL_MEM_ALLOC
extern UINT32 OsMemSystemInit(UINTPTR memStart);
#endif
/* SLAB extension needs memory algorithms provide following internal apis */
#ifdef LOSCFG_MEM_SLAB_EXTENTION
extern VOID* OsMemAlloc(VOID *pool, UINT32 size);
#endif /* LOSCFG_MEM_SLAB_EXTENTION */
extern UINT32 OsMemFree(const VOID *pool, const VOID *ptr);
extern VOID *OsMemAllocNoLock(VOID *pool, UINT32 size);
extern VOID *OsMemAllocAlignNoLock(VOID *pool, UINT32 size, UINT32 boundary);
extern UINT32 OsMemFreeNoLock(const VOID *pool, const VOID *ptr);

STATIC INLINE UINT32 OsMemMulPoolInit(const VOID *pool, UINT32 size)
{
    (VOID)pool;
    (VOID)size;
    return LOS_OK;
}

STATIC INLINE UINT32 OsMemMulPoolDeinit(const VOID *pool)
{
    (VOID)pool;
    return LOS_OK;
}

STATIC INLINE BOOL OsMemIntLockCheck(const VOID *pool, UINT32 size)
{
    BOOL intLock = FALSE;
    (VOID)pool;
    (VOID)size;

#ifdef LOSCFG_USR_MEM_ALLOC
#ifdef LOSCFG_KERNEL_MEM_ALLOC
    if ((((UINTPTR)pool >= (UINTPTR)m_aucSysMem1) && \
        (((UINTPTR)pool + size) <= ((UINTPTR)m_aucSysMem1 + OS_SYS_MEM_SIZE))) || (LOS_IN_KERNEL)) {
        intLock = TRUE;
    }
#endif /* LOSCFG_KERNEL_MEM_ALLOC */
#else
    intLock = TRUE;
#endif /* LOSCFG_USR_MEM_ALLOC */
    return intLock;
}

STATIC INLINE UINT32 OsMemAttrGet(const VOID *pool, UINT32 size)
{
    return OsMemIntLockCheck(pool, size) ? (OS_MEM_LOCK_TYPE_INT | LOS_MEM_INIT_ATTR_SLAB_MSK) : \
                                           (OS_MEM_LOCK_TYPE_MUX | LOS_MEM_INIT_ATTR_SLAB_MSK);
}

extern VOID OsMemInfoPrint(const VOID *pool);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_MEMORY_PRI_H */
