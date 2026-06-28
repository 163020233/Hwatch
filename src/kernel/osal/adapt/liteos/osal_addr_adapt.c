/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Mem adapt
 * Author:
 * Create: 2026-4-1
 */

#include "los_memory.h"
#include "soc_osal.h"
#include "osal_adapt.h"

unsigned int osal_mem_node_size_check(void *pool, void *ptr, unsigned int *totalSize, unsigned int *availSize)
{
    return LOS_MemNodeSizeCheck(pool, ptr, totalSize, availSize);
}
 
unsigned int osal_mem_info_get(void *pool, osal_mem_pool_status *poolStatus)
{
    unsigned int ret;
    LOS_MEM_POOL_STATUS status;
 
    if (poolStatus == NULL) {
        return OSAL_FAILURE;
    }
 
    ret = LOS_MemInfoGet(pool, &status);
    if (ret != LOS_OK) {
        return ret;
    }
 
    poolStatus->uwTotalFreeSize = status.uwTotalFreeSize;
    poolStatus->uwTotalUsedSize = status.uwTotalUsedSize;
    poolStatus->uwMaxFreeNodeSize = status.uwMaxFreeNodeSize;
    poolStatus->uwUsedNodeNum = status.uwUsedNodeNum;
    poolStatus->uwFreeNodeNum = status.uwFreeNodeNum;
#ifdef LOSCFG_MEM_TASK_STAT
    poolStatus->uwUsageWaterLine = status.uwUsageWaterLine;
#endif
 
    return OSAL_SUCCESS;
}
 
unsigned int osal_mem_pool_size_get(const void *pool)
{
    return LOS_MemPoolSizeGet(pool);
}