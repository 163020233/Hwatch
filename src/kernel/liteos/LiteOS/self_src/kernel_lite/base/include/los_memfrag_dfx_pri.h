/*
 * Copyright: Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: memfrag dfx header
 * Author: HW
 * Create: 2025-11-07
 * History: NA
 */
#ifndef _LOS_MEMFRAG_DFX_H
#define _LOS_MEMFRAG_DFX_H

#include "los_multipledlinkhead_pri.h"
#include "los_memory_internal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_MEMFRAG_DFX_SHOW_INFO
typedef enum {
    NodeSize_0_64       = 0,
    NodeSize_64_128     = 1,
    NodeSize_128_256    = 2,
    NodeSize_256_512    = 3,
    NodeSize_512_1024   = 4,
    NodeSize_1024_2048  = 5,
    NodeSize_2048_4096  = 6,
    NodeSize_4096_MAX   = 7,
    NodeSize_END
} MemFragmentSizeRange; // 内存碎片大小区间分类

typedef struct {
    UINT32 totalUsedSize; // 正在使用的内存总大小
    UINT32 totalFreeSize; // 系统空闲的内存总大小
    UINT32 usedNodeNum;   // 使用的内存节点数量
    UINT32 freeNodeNum;   // 未使用的内存节点数量
    UINT32 maxUsedNodeSize; // 最大使用的内存节点的大小
    UINT32 maxFreeNodeSize; // 最大未使用的内存节点的大小
    UINT32 usedNode[NodeSize_END]; // 记录使用内存碎片不同区间数量
    UINT32 freeNode[NodeSize_END]; // 记录未使用内存碎片不同区间数量
#ifdef LOSCFG_KERNEL_MEM_SLAB_EXTENTION
    UINT32 usedSlabNum; // 使用的slab节点数量
    UINT32 freeSlabNum; // 未使用的slab节点数量
    UINT32 maxUsedSlabSize; // 最大使用的Slab节点的大小
    UINT32 maxFreeSlabSize; // 最大未使用的Slab节点的大小
    UINT32 usedSlab[NodeSize_END]; // 记录使用slab碎片不同区间数量
    UINT32 freeSlab[NodeSize_END]; // 记录未使用slab碎片不同区间数量
#endif
} LosMemFragInfo;

VOID LOS_MemFragInfo(void);
#endif /* LOSCFG_MEMFRAG_DFX_SHOW_INFO */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_MEMFRAG_DFX_H */
