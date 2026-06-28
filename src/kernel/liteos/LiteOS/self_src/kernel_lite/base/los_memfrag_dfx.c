/*
 * @Copyright: Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * @Description:  memfrag dfx
 * @Author: HW
 * @Create: 2025-11-7
 * @History: NA
 */

#include "los_memfrag_dfx_pri.h"

#ifdef LOSCFG_MEMFRAG_DFX_SHOW_INFO
STATIC VOID CountNodeNum(UINT32 *NodeSize, UINT32 size, UINT32 blkCnt)
{
    // 根据Nodesize大小统计到不同区间
    if (size < 64) { // 64 Byte
        NodeSize[NodeSize_0_64] += blkCnt;
    } else if (size < 128) { // 128 Byte
        NodeSize[NodeSize_64_128] += blkCnt;
    } else if (size < 256) { // 256 Byte
        NodeSize[NodeSize_128_256] += blkCnt;
    } else if (size < 512) { // 512 Byte
        NodeSize[NodeSize_256_512] += blkCnt;
    } else if (size < 1024) { // 1024 Byte
        NodeSize[NodeSize_512_1024] += blkCnt;
    } else if (size < 2048) { // 2048 Byte
        NodeSize[NodeSize_1024_2048] += blkCnt;
    } else if (size < 4096) { // 4096 Byte
        NodeSize[NodeSize_2048_4096] += blkCnt;
    } else {
        NodeSize[NodeSize_4096_MAX] += blkCnt;
    }
    return;
}

#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
STATIC VOID MemSlabFragInfoPrint(LosMemFragInfo *MemFragInfo)
{
    PRINTK("total_SlabNum:                                %-8u\n", MemFragInfo->usedSlabNum +
        MemFragInfo->freeSlabNum);
    PRINTK("total_usedSlabNum/total_freeSlabNum:          %-8u %-8u\n", MemFragInfo->usedSlabNum,
        MemFragInfo->freeSlabNum);
    PRINTK("MaxUsedSlabSize/MaxFreeSlabSize:              %-8u %-8u\n", MemFragInfo->maxUsedSlabSize,
        MemFragInfo->maxFreeSlabSize);
    PRINTK("usedSlabNum/freeSlabNum [0~64 Byte]:          %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_0_64],
        MemFragInfo->freeSlab[NodeSize_0_64]);
    PRINTK("usedSlabNum/freeSlabNum [64~128 Byte]:        %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_64_128],
        MemFragInfo->freeSlab[NodeSize_64_128]);
    PRINTK("usedSlabNum/freeSlabNum [128~256 Byte]:       %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_128_256],
        MemFragInfo->freeSlab[NodeSize_128_256]);
    PRINTK("usedSlabNum/freeSlabNum [256~512 Byte]:       %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_256_512],
        MemFragInfo->freeSlab[NodeSize_256_512]);
    PRINTK("usedSlabNum/freeSlabNum [512~1024 Byte]:      %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_512_1024],
        MemFragInfo->freeSlab[NodeSize_512_1024]);
    PRINTK("usedSlabNum/freeSlabNum [1024~2048 Byte]:     %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_1024_2048],
        MemFragInfo->freeSlab[NodeSize_1024_2048]);
    PRINTK("usedSlabNum/freeSlabNum [2048~4096 Byte]:     %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_2048_4096],
        MemFragInfo->freeSlab[NodeSize_2048_4096]);
    PRINTK("usedSlabNum/freeSlabNum [4096~Max Byte]:      %-8u %-8u\n", MemFragInfo->usedSlab[NodeSize_4096_MAX],
        MemFragInfo->freeSlab[NodeSize_4096_MAX]);
    return;
}
#endif

STATIC VOID MemFragInfoPrint(LosMemFragInfo *MemFragInfo)
{
    // 打印MemFragInfo
    PRINTK("total_Size:                                   %-8u\n", MemFragInfo->totalUsedSize +
        MemFragInfo->totalFreeSize);
    PRINTK("total_usedSize/total_FreeSize:                %-8u %-8u\n", MemFragInfo->totalUsedSize,
        MemFragInfo->totalFreeSize);
    PRINTK("total_NodeNum:                                %-8u\n", MemFragInfo->usedNodeNum +
        MemFragInfo->freeNodeNum);
    PRINTK("total_usedNodeNum/total_freeNodeNum:          %-8u %-8u\n", MemFragInfo->usedNodeNum,
        MemFragInfo->freeNodeNum);
    PRINTK("MaxUsedNodeSize/MaxFreeNodeSize:              %-8u %-8u\n", MemFragInfo->maxUsedNodeSize,
        MemFragInfo->maxFreeNodeSize);
    PRINTK("usedNodeNum/freeNodeNum [0~64 Byte]:          %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_0_64],
        MemFragInfo->freeNode[NodeSize_0_64]);
    PRINTK("usedNodeNum/freeNodeNum [64~128 Byte]:        %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_64_128],
        MemFragInfo->freeNode[NodeSize_64_128]);
    PRINTK("usedNodeNum/freeNodeNum [128~256 Byte]:       %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_128_256],
        MemFragInfo->freeNode[NodeSize_128_256]);
    PRINTK("usedNodeNum/freeNodeNum [256~512 Byte]:       %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_256_512],
        MemFragInfo->freeNode[NodeSize_256_512]);
    PRINTK("usedNodeNum/freeNodeNum [512~1024 Byte]:      %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_512_1024],
        MemFragInfo->freeNode[NodeSize_512_1024]);
    PRINTK("usedNodeNum/freeNodeNum [1024~2048 Byte]:     %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_1024_2048],
        MemFragInfo->freeNode[NodeSize_1024_2048]);
    PRINTK("usedNodeNum/freeNodeNum [2048~4096 Byte]:     %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_2048_4096],
        MemFragInfo->freeNode[NodeSize_2048_4096]);
    PRINTK("usedNodeNum/freeNodeNum [4096~Max Byte]:      %-8u %-8u\n", MemFragInfo->usedNode[NodeSize_4096_MAX],
        MemFragInfo->freeNode[NodeSize_4096_MAX]);
#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
    MemSlabFragInfoPrint(MemFragInfo);
#endif
    return;
}

#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
STATIC VOID MemSlabinfo(const VOID *pool, LosMemFragInfo *MemFragInfo)
{
    UINT32 i;
    UINT32 usedCnt = 0;
    UINT32 freeCnt = 0;
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    struct LosSlabControlHeader slabInfo = poolInfo->slabCtrlHdr;
    for (i = 0; i < SLAB_MEM_COUNT; i++) {
        if (slabInfo.slabClass[i].blkSz == 0) {
            continue; // 跳过无效slab
        }
        usedCnt = slabInfo.slabClass[i].blkUsedCnt;
        freeCnt = slabInfo.slabClass[i].blkCnt - slabInfo.slabClass[i].blkUsedCnt;
        // 更新内存节点总数量
        MemFragInfo->freeSlabNum += freeCnt;
        MemFragInfo->usedSlabNum += usedCnt;
        // 更新内存总大小
        MemFragInfo->totalFreeSize += slabInfo.slabClass[i].blkSz * freeCnt;
        MemFragInfo->totalUsedSize += slabInfo.slabClass[i].blkSz * usedCnt;
        // 统计不同区间的slab数量
        CountNodeNum(MemFragInfo->freeSlab, slabInfo.slabClass[i].blkSz, freeCnt);
        CountNodeNum(MemFragInfo->usedSlab, slabInfo.slabClass[i].blkSz, usedCnt);
        // 更新maxFreeSlabSize
        if (freeCnt > 0 && slabInfo.slabClass[i].blkSz > MemFragInfo->maxFreeSlabSize) {
            MemFragInfo->maxFreeSlabSize = slabInfo.slabClass[i].blkSz;
        }
        // 更新maxUsedSlabSize
        if (usedCnt > 0 && slabInfo.slabClass[i].blkSz > MemFragInfo->maxUsedSlabSize) {
            MemFragInfo->maxUsedSlabSize = slabInfo.slabClass[i].blkSz;
        }
    }
    return;
}
#endif

#ifdef LOSCFG_KERNEL_MEM_BESTFIT
STATIC VOID MemNodeinfo(const VOID *pool, LosMemFragInfo *MemFragInfo)
{
    UINT32 size = 0;
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *tmpNode = NULL;
    LosMemDynNode *endNode = NULL;

    if (pool == NULL) {
        return;
    }
    endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    for (tmpNode = OS_MEM_FIRST_NODE(poolInfo); tmpNode < endNode; tmpNode = OS_MEM_NEXT_NODE(tmpNode)) {
        size = OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
        if (size == 0) {
            continue; // 跳过无效节点
        }
        if (!OS_MEM_NODE_GET_USED_FLAG(tmpNode->selfNode.sizeAndFlag)) {
            MemFragInfo->freeNodeNum++;
            MemFragInfo->totalFreeSize += size;
            // 统计不同区间未使用的内存节点数量
            CountNodeNum(MemFragInfo->freeNode, size, 1);
            if (size > MemFragInfo->maxFreeNodeSize) {
                MemFragInfo->maxFreeNodeSize = size;
            }
        } else {
            MemFragInfo->usedNodeNum++;
            MemFragInfo->totalUsedSize += size;
            // 统计不同区间使用的内存节点数量
            CountNodeNum(MemFragInfo->usedNode, size, 1);
            if (size > MemFragInfo->maxUsedNodeSize) {
                MemFragInfo->maxUsedNodeSize = size;
            }
        }
    }
    return;
}
#endif

#ifdef LOSCFG_KERNEL_MEM_BESTFIT_LITTLE

STATIC VOID MemNodeinfo(const VOID *pool, LosMemFragInfo *MemFragInfo)
{
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    struct LosHeapNode *node = (struct LosHeapNode *)(poolInfo + 1);
    UINTPTR heapStart = (UINTPTR)poolInfo;
    UINTPTR heapEnd = (UINTPTR)node + poolInfo->size;

    if (pool == NULL) {
        return;
    }
    while (node != NULL) {
        if ((UINTPTR)node < heapStart || (UINTPTR)node > heapEnd) {
            return;
        }
        if (node->size == 0) {
            node = OsHeapPrvGetNext(poolInfo, node);
            continue; // 跳过无效节点
        }
        if (!node->used) {
            MemFragInfo->freeNodeNum++;
            MemFragInfo->totalFreeSize += node->size;
            // 统计不同区间未使用的内存节点数量
            CountNodeNum(MemFragInfo->freeNode, node->size, 1);
            if (node->size > MemFragInfo->maxFreeNodeSize) {
                MemFragInfo->maxFreeNodeSize = node->size;
            }
        } else {
            MemFragInfo->usedNodeNum++;
            MemFragInfo->totalUsedSize += node->size;
            // 统计不同区间使用的内存节点数量
            CountNodeNum(MemFragInfo->usedNode, node->size, 1);
            if (node->size > MemFragInfo->maxUsedNodeSize) {
                MemFragInfo->maxUsedNodeSize = node->size;
            }
        }
        node = OsHeapPrvGetNext(poolInfo, node);
    }
    return;
}
#endif

STATIC VOID MemFragInfoShow(const VOID *pool)
{
    LosMemFragInfo MemFragInfo = {0};

    MemNodeinfo(pool, &MemFragInfo); // 统计内存node信息
#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
    MemSlabinfo(pool, &MemFragInfo); // 统计内存slab信息
#endif
    MemFragInfoPrint(&MemFragInfo); // 打印最终结果
    return;
}

VOID LOS_MemFragInfo(void)
{
    UINT32 index = 0;
    UINT32 intSave;
    PRINTK("====== LOS_MemFragInfo ======\n");
#ifdef LOSCFG_MEM_MUL_POOL
    // addr is OS_SYS_MEM_ADDR
    VOID *nextPool = OsMemMulPoolHeadGet();
    while (nextPool != NULL) {
        PRINTK("========= pool%uInfo =========\n", index);
        PRINTK("pool addr:                                    0x%-8x\n", nextPool);
        index++;
        MEM_LOCK(intSave, nextPool);
        MemFragInfoShow(nextPool);
        MEM_UNLOCK(intSave, nextPool);
        nextPool = ((LosMemPoolInfo *)nextPool)->nextPool;
    }
#else
    VOID *nextPool = (VOID *)OS_SYS_MEM_ADDR;
    MEM_LOCK(intSave, nextPool);
    MemFragInfoShow(nextPool);
    MEM_UNLOCK(intSave, nextPool);
    index++;
#endif
    PRINTK("==============================\n");
    PRINTK("LOSCFG_MEM_MUL_POOL num is %u\n", index);
    return;
}
#endif /* LOSCFG_MEMFRAG_DFX_SHOW_INFO */