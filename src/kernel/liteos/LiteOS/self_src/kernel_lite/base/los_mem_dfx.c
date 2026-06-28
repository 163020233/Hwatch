/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#include "los_config.h"
#include "los_task_pri.h"
#include "los_memstat_pri.h"
#include "los_memory_pri.h"
#include "los_slab_pri.h"
#include "los_memory_internal.h"
#include "los_mem_dfx_pri.h"

#define HEAP_CAST(t, exp)       ((t)(exp))
#define HEAP_ALIGN              sizeof(UINTPTR)
#define MALLOC_MAXSIZE          (0xFFFFFFFF - HEAP_ALIGN + 1)
#define NODE_UNUSED             0
#define NODE_USED               1

#ifdef LOSCFG_MEM_TASK_STAT
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
typedef struct {
    UINTPTR callerRa;
    UINT32 totalSize;
    UINT32 blockCount;
} CallerRaMemStat;

#define RA_FLAG_MASK           ((UINTPTR)1)
#define RA_IS_FLAGGED(ra)      (((ra) & RA_FLAG_MASK) != 0)
#define RA_SET_FLAG(ra)        ((ra) | RA_FLAG_MASK)
#define RA_CLR_FLAG(ra)        ((ra) & (~RA_FLAG_MASK))
#define RA_BATCH_SIZE          ((IRQ_STACK_SIZE) / sizeof(CallerRaMemStat))
#define GET_IRQSTACK_RASTATS() ((CallerRaMemStat *)(IRQ_STACK_TOP - sizeof(CallerRaMemStat) * RA_BATCH_SIZE))

STATIC VOID printBatchRaStats(CallerRaMemStat *raStats, UINT32 raStatsCnt)
{
    for (UINT32 i = 0; i < raStatsCnt; i++) {
        PRINTK("callerRA=0x%08x : totalSize=%08d Bytes, blockCount=% 2d\r\n",
            (raStats[i].callerRa & (~RA_FLAG_MASK)),
            raStats[i].totalSize,
            raStats[i].blockCount);
    }
}

STATIC BOOL updateRaStats(CallerRaMemStat *raStats, UINT32 *raStatsCnt, UINTPTR callerRa, UINT32 size)
{
    UINT32 raCnt = *raStatsCnt;
    for (UINT32 i = 0; i < raCnt; i++) {
        if (raStats[i].callerRa == callerRa) {
            raStats[i].totalSize += size;
            raStats[i].blockCount++;
            return TRUE;
        }
    }
    if (raCnt < RA_BATCH_SIZE) {
        raStats[raCnt].callerRa = callerRa;
        raStats[raCnt].totalSize = size;
        raStats[raCnt].blockCount = 1;
        (raCnt)++;
        *raStatsCnt = raCnt;
        return TRUE;
    }
    return FALSE;
}

#endif /* LOSCFG_MEM_DFX_SHOW_CALLER_RA */

#ifdef LOSCFG_KERNEL_MEM_BESTFIT
STATIC BOOL OsMemNodeIsUsed(LosMemDynNode *node)
{
    if (node == NULL) {
        return FALSE;
    }
    return ((OS_MEM_NODE_GET_USED_FLAG(node->selfNode.sizeAndFlag) == OS_MEM_NODE_USED_FLAG) ? TRUE : FALSE);
}

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
STATIC BOOL checkUnflaggedNodes(LosMemDynNode *endNode, UINT8 taskId)
{
    LosMemDynNode *tmpNode;
    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        if ((!RA_IS_FLAGGED(tmpNode->selfNode.callerRa)) && (tmpNode->selfNode.taskId == taskId) &&
            OsMemNodeIsUsed(tmpNode)) {
            return FALSE;
        }
    }
    return TRUE;
}

STATIC UINT32 collectCallerRaStatsBatch(LosMemDynNode *endNode, UINT8 taskId,
    CallerRaMemStat *raStats, UINT32 *totalCostSize, UINT32 *totalPoolUsedSize)
{
    UINT32 raStatsCnt = 0;
    LosMemDynNode *tmpNode;
    UINTPTR callerRa;

    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        if (RA_IS_FLAGGED(tmpNode->selfNode.callerRa)) {
            continue;
        }
        if ((tmpNode->selfNode.taskId != taskId) || (!OsMemNodeIsUsed(tmpNode))) {
            continue;
        }
        UINT32 size = OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
        callerRa = tmpNode->selfNode.callerRa;

        BOOL ret = updateRaStats(raStats, &raStatsCnt, callerRa, size);
        if (ret) {
            *totalCostSize += OS_MEM_NODE_HEAD_SIZE;
            *totalPoolUsedSize += size;
            tmpNode->selfNode.callerRa = RA_SET_FLAG(tmpNode->selfNode.callerRa);
        }
    }
    return raStatsCnt;
}

/* traverse node and record RaStats each batch */
STATIC UINT32 traverseRaStats(VOID *pool, UINT8 taskId, LosMemDynNode *endNode, UINT32 *totalCostSize)
{
    UINT32 totalPoolUsedSize = 0;
    UINT32 raStatsCnt = 0;
    CallerRaMemStat *raStats = GET_IRQSTACK_RASTATS();
    BOOL allNodeFlagged = FALSE;

    while (!allNodeFlagged) {
        raStatsCnt = collectCallerRaStatsBatch(endNode, taskId, raStats, totalCostSize, &totalPoolUsedSize);
        if (raStatsCnt > 0) {
            printBatchRaStats(raStats, raStatsCnt);
        }
        allNodeFlagged = checkUnflaggedNodes(endNode, taskId);
    }
    return totalPoolUsedSize;
}
#endif /* LOSCFG_MEM_DFX_SHOW_CALLER_RA */

STATIC UINT32 OsHeapTaskHeapInfoGet(VOID *pool, UINT8 taskId)
{
    UINT32 totalPoolUsedSize = 0;
    UINT32 totalSlabUsedSize = 0;
    UINT32 totalCostSize = 0;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    LosMemDynNode *tmpNode;

    if (poolInfo == NULL || taskId > TASK_NUM - 1) {
        return LOS_NOK;
    }
    if (taskCB->taskName == NULL) {
        return LOS_NOK;
    }
    if (taskId == TASK_NUM - 1) {
        PRINTK("Other malloc details:\r\n");
    } else {
        PRINTK("Task:%s(taskId=%02d) malloc details:\r\n", taskCB->taskName, taskId);
    }
    // get total slab used mem
    totalSlabUsedSize = OsSlabMemInfoGet(pool, taskId);
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    totalPoolUsedSize = traverseRaStats(pool, taskId, endNode, &totalCostSize);

    // restore origin callerRa
    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        tmpNode->selfNode.callerRa = RA_CLR_FLAG(tmpNode->selfNode.callerRa);
    }
#else
    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        totalCostSize += OS_MEM_NODE_HEAD_SIZE;
        if ((tmpNode->selfNode.taskId == taskId) && OsMemNodeIsUsed(tmpNode)) {
            totalPoolUsedSize += OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
        }
    }
#endif
    PRINTK("Current total pool alloc size=%d Bytes\r\n", totalPoolUsedSize);
    PRINTK("Current total slab alloc size=%d Bytes\r\n", totalSlabUsedSize);
    PRINTK("Current total node cost size=%d Bytes\r\n\n", totalCostSize);
    return LOS_OK;
}

UINT32 OsSlabMemInfoGet(const VOID *pool, UINT32 taskId)
{
    UINT32 taskTotalUsedSlabMem = 0;
#if defined(LOSCFG_KERNEL_MEM_SLAB_EXTENTION) && (!defined(LOSCFG_KERNEL_MEM_SLAB_AUTO_EXPANSION_MODE))
    for (UINT32 clsidx = 0; clsidx < SLAB_MEM_COUNT; clsidx++) {
        struct LosSlabControlHeader *slabMem = OsSlabCtrlHdrGet(pool);
        OsSlabAllocator *allocator = slabMem->slabClass[clsidx].alloc;
        UINT32 numBits = allocator->bitset->numBits;
        for (UINT32 didx = 0; didx < numBits; didx++) {
            OsSlabBlockNode *blockNode = (OsSlabBlockNode *)(allocator->dataChunks + allocator->itemSz * (UINT32)didx);
            if (OS_SLAB_BLOCK_ID_GET(blockNode) == taskId) {
                taskTotalUsedSlabMem += OS_SLAB_BLOCK_SIZE_GET(blockNode);
            }
        }
    }
#endif
    return taskTotalUsedSlabMem;
}
#endif

#ifdef LOSCFG_KERNEL_MEM_BESTFIT_LITTLE
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
STATIC BOOL checkUnflaggedNodes(LosMemPoolInfo *ramHeap, UINT8 taskId)
{
    struct LosHeapNode *node = ramHeap->tail;
    for (node = ramHeap->tail; node != NULL; node = node->prev) {
        if ((!RA_IS_FLAGGED(node->callerRa)) && (node->taskId == taskId) && node->used) {
            return FALSE;
        }
    }
    return TRUE;
}

STATIC UINT32 collectCallerRaStatsBatch(
    LosMemPoolInfo *ramHeap, UINT8 taskId, CallerRaMemStat *raStats, UINT32 *totalUsedSize)
{
    UINT32 raStatsCnt = 0;
    UINTPTR callerRa;
    struct LosHeapNode *node = ramHeap->tail;
    for (node = ramHeap->tail; node != NULL; node = node->prev) {
        if (RA_IS_FLAGGED(node->callerRa)) {
            continue;
        }
        UINT32 size = node->size + OS_MEM_NODE_HEAD_SIZE;
        if ((node->taskId != taskId) || (!node->used)) {
            continue;
        }
        callerRa = node->callerRa;
        BOOL ret = updateRaStats(raStats, &raStatsCnt, callerRa, size);
        if (ret) {
            *totalUsedSize += size;
            node->callerRa = RA_SET_FLAG(node->callerRa);
        }
    }
    return raStatsCnt;
}

/* traverse node and record RaStats each batch */
STATIC VOID traverseRaStats(VOID *pool, UINT8 taskId, LosMemPoolInfo *ramHeap, UINT32 *totalUsedSize)
{
    UINT32 raStatsCnt = 0;
    CallerRaMemStat *raStats = GET_IRQSTACK_RASTATS();

    BOOL allNodeFlagged = FALSE;
    while (!allNodeFlagged) {
        raStatsCnt = collectCallerRaStatsBatch(ramHeap, taskId, raStats, totalUsedSize);
        if (raStatsCnt > 0) {
            printBatchRaStats(raStats, raStatsCnt);
        }
        allNodeFlagged = checkUnflaggedNodes(ramHeap, taskId);
    }
}
#endif /* LOSCFG_MEM_DFX_SHOW_CALLER_RA */

STATIC UINT32 OsHeapTaskHeapInfoGet(VOID *pool, UINT8 taskId)
{
    UINT32 totalUsedSize = 0;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);
    struct LosHeapNode *node = NULL;
    LosMemPoolInfo *ramHeap = HEAP_CAST(LosMemPoolInfo *, pool);

    if (ramHeap == NULL || taskId > TASK_NUM - 1) {
        return LOS_NOK;
    }
    if (taskCB->taskName == NULL) {
        return LOS_NOK;
    }
    if (taskId == TASK_NUM - 1) {
        PRINTK("Other malloc details:\r\n");
    } else {
        PRINTK("Task:%s(taskId=%02d) malloc details:\r\n", taskCB->taskName, taskId);
    }

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    traverseRaStats(pool, taskId, ramHeap, &totalUsedSize);
    // restore origin callerRa
    for (node = ramHeap->tail; node != NULL; node = node->prev) {
        node->callerRa = RA_CLR_FLAG(node->callerRa);
    }
#else
    for (node = ramHeap->tail; node != NULL; node = node->prev) {
        if ((node->taskId == taskId) && node->used) {
            totalUsedSize += node->size + OS_MEM_NODE_HEAD_SIZE;
        }
    }
#endif
    PRINTK("Current total alloc size=%d Bytes\r\n", totalUsedSize);
    return LOS_OK;
}

UINT32 OsHeapInfoDump(VOID *pool)
{
    LosHeapStatus heapStat;
    UINT32 err;

    err = OsHeapStatisticsGet(pool, &heapStat);
    if (err != LOS_OK) {
        return LOS_NOK;
    }
    PRINT_ERR("[Heap@0x%x stat] total:0x%x, used:0x%x, free:0x%x, peak usage:0x%x, peak free:0x%x\r\n", \
              (UINTPTR)pool, \
              (uint32_t)(heapStat.totalUsedSize + heapStat.totalFreeSize), \
              (uint32_t)(heapStat.totalUsedSize), (uint32_t)(heapStat.totalFreeSize), \
              (uint32_t)(heapStat.usageWaterLine), \
              (uint32_t)(heapStat.totalFreeSize + heapStat.totalUsedSize - heapStat.usageWaterLine));
    return LOS_OK;
}
#endif

VOID LOS_MemTaskHeapInfoGet(VOID *pool, UINT8 taskId)
{
    UINT32 intSave;

    if ((pool == NULL)) {
        return;
    }

    MEM_LOCK(intSave, pool);
    OsHeapTaskHeapInfoGet(pool, taskId);
    MEM_UNLOCK(intSave, pool);
    return;
}
#else
VOID LOS_MemTaskHeapInfoGet(VOID *pool, UINT8 taskId)
{
    return;
}
#endif

#ifdef LOSCFG_MEM_DEBUG
VOID OsExcMemInfo(VOID)
{
#ifdef LOSCFG_MEM_MUL_POOL
    LosMemPoolInfo *pool = OsMemMulPoolHeadGet();
    while (pool != NULL) {
        OsCheckPool(pool);
        pool = pool->nextPool;
    }
#endif
}
#endif
