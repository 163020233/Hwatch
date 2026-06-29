/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Dynload Share Memory Framework.
 * Author: Huawei LiteOS Team
 * Create: 2022-07-01
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

#include "los_dynshm.h"
#include "los_memory.h"
#include "los_mux.h"
#include "los_list.h"
#include "los_spinlock.h"
#include "hal_iommu.h"

#define PAGE_SIZE_4K         0x1000
#define PAGE_SIZE_16K        0x4000
#define PAGE_SIZE_64K        0x10000

#define SHM_PRINT_DEBUG      PRINT_DEBUG
#define SHM_PRINT_INFO       PRINT_INFO
#define SHM_PRINT_ERR        PRINT_ERR

#define REGION_IDX_MAP_INIT_VAL      0xff
#define DYN_SHM_STATUS_READY         0x2
#define DYN_SHM_STATUS_INITIALIZING  0x1
#define DYN_SHM_STATUS_UNINIT        0x0
#define DYNSHM_LOCK(state)           LOS_SpinLockSave(&g_dynShmSpin, &(state))
#define DYNSHM_UNLOCK(state)         LOS_SpinUnlockRestore(&g_dynShmSpin, (state))

#define CHECK_INT_ACTIVE_RETURN()             \
    if (OS_INT_ACTIVE) {                      \
        return LOS_ERRNO_DYN_SHM_IN_ISR;      \
    }

typedef struct {
    UINTPTR virAddr;
    UADDR phyAddr;
    UINT32 size;
    UINT32 type;
    LOS_DL_LIST listNode;
} DynShmArea;

typedef struct {
    UINT32 totalFreeSize;
    DynShmArea usedBlkListHead;
    DynShmArea usedBlkListEnd;
} RegionManager;

typedef struct {
    DynShmConfig userCfg;
    UINT8 rgnIdxMap[LOS_VA_SPACE_TYPE_MAX];
    DynShmArea blocks[LOSCFG_KERNEL_DYNSHM_VM_BLOCK_NUM];
    RegionManager regionCB[DYN_SHM_RGN_MAX];
    FreePageTableInfo unusedPtInfo;
} DynShmManager;

STATIC LOS_DL_LIST g_losFreeBlock;

STATIC DynShmManager g_shmCB = { 0 };
STATIC UINT32 g_shmMutex = 0xFFFFFFFF;
STATIC UINT32 g_blockMutex = 0xFFFFFFFF;

STATIC UINT32 g_dynShmInitFlag = DYN_SHM_STATUS_UNINIT;
STATIC SPIN_LOCK_INIT(g_dynShmSpin);
typedef struct {
    UINT32 startAddr;
    UINT32 endAddr;
} UsedRegionConfig;

typedef struct {
    UsedRegionConfig nonsec;
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    UsedRegionConfig sec;
#endif
} UsedRegionRange;

#ifdef LOSCFG_KERNEL_DYNSHM_1_LEVEL_PAGETABLE_REUSE
STATIC INLINE VOID ShmCheckAndSetRegionRange(UsedRegionConfig *cfg, UINT32 regionStart, UINT32 regionEnd)
{
    if (regionStart < cfg->startAddr) {
        cfg->startAddr = regionStart;
    }
    if (regionEnd > cfg->endAddr) {
        cfg->endAddr = regionEnd;
    }
}

STATIC INLINE VOID ShmSetExtremeAddr(UsedRegionRange *region, UINT32 regionStart, UINT32 regionEnd, UINT32 type)
{
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    if (LOS_SHM_IS_SEC(type)) {
        ShmCheckAndSetRegionRange(&region->sec, regionStart, regionEnd);
        return;
    }
#else
    (VOID)type;
#endif
    ShmCheckAndSetRegionRange(&region->nonsec, regionStart, regionEnd);
}

STATIC VOID ShmUnusedPtInfoSet(FreePageTableInfo *info, UsedRegionRange *region)
{
    DynShmConfig *cfg = &g_shmCB.userCfg;
    SysPageTable *pt = &cfg->innerPageTable;
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    info->sec.lowerStartAddr = pt->sec.baseAddr;
    info->sec.lowerFreeSize = IommuGetPteOffset(region->sec.startAddr, cfg->pageSize);
    info->sec.higherStartAddr = pt->sec.baseAddr + IommuGetPteOffset(region->sec.endAddr, cfg->pageSize);
    info->sec.higherFreeSize = pt->sec.tableSize - IommuGetPteOffset(region->sec.endAddr, cfg->pageSize);
#else
    (VOID)memset((VOID *)(UINTPTR)&info->sec, 0, sizeof(info->sec));
#endif
    info->nonsec.lowerStartAddr = pt->nonsec.baseAddr;
    info->nonsec.lowerFreeSize = IommuGetPteOffset(region->nonsec.startAddr, cfg->pageSize);
    info->nonsec.higherStartAddr = pt->nonsec.baseAddr + IommuGetPteOffset(region->nonsec.endAddr, cfg->pageSize);
    info->nonsec.higherFreeSize = pt->nonsec.tableSize - IommuGetPteOffset(region->nonsec.endAddr, cfg->pageSize);
}

STATIC UINT32 ShmUnusedPtPoolInit(VOID)
{
    UINT32 ret;
    FreePageTableConfig *cfg = &g_shmCB.unusedPtInfo.nonsec;
    ret = LOS_MemInit((VOID *)(UINTPTR)cfg->lowerStartAddr, cfg->lowerFreeSize);
    if (ret != LOS_OK) {
        PRINT_ERR("lower pt init failed, ret:%#x.\n", ret);
        return LOS_NOK;
    }
    ret = LOS_MemInit((VOID *)(UINTPTR)cfg->higherStartAddr, cfg->higherFreeSize);
    if (ret != LOS_OK) {
        PRINT_ERR("higher pt init failed, ret:%#x.\n", ret);
        goto DEINIT_POOL;
    }
    return LOS_OK;
DEINIT_POOL:
    (VOID)LOS_MemDeInit((VOID *)(UINTPTR)cfg->lowerStartAddr);
    return LOS_NOK;
}
#else
STATIC INLINE VOID ShmSetExtremeAddr(UsedRegionRange *region, UINT32 regionStart, UINT32 regionEnd, UINT32 type)
{
    (VOID)region;
    (VOID)regionStart;
    (VOID)regionEnd;
    (VOID)type;
}

STATIC VOID ShmUnusedPtInfoSet(FreePageTableInfo *info, UsedRegionRange *region)
{
    (VOID)info;
    (VOID)region;
}

STATIC UINT32 ShmUnusedPtPoolInit(VOID)
{
    return LOS_OK;
}
#endif

STATIC INLINE UINT32 ShmCheckStatusReady(VOID)
{
    UINT32 intSave, ret;

    DYNSHM_LOCK(intSave);
    ret = (g_dynShmInitFlag != DYN_SHM_STATUS_READY) ? LOS_NOK : LOS_OK;
    DYNSHM_UNLOCK(intSave);
    return ret;
}

STATIC INLINE UINT32 ShmCheckPagetableInput(SysPageTable *pt)
{
    if ((pt->nonsec.baseAddr == 0) || (pt->nonsec.tableSize == 0) ||
        (pt->nonsec.baseAddr > (UINT32_MAX - pt->nonsec.tableSize))) {
        return LOS_NOK;
    }
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    if ((pt->sec.baseAddr == 0) || (pt->sec.tableSize == 0) ||
        (pt->sec.baseAddr > (UINT32_MAX - pt->sec.tableSize))) {
        return LOS_NOK;
    }
#endif
    return LOS_OK;
}

STATIC INLINE UINT32 ShmCheckRegionInput(DynShmConfig *config)
{
    UINT8 idx;
    DynShmRegion *region = NULL;
    UINT32 pageAlign, type;
    UINT32 regionStart;
    UINT32 regionEnd;

    pageAlign = config->pageSize;

    for (idx = 0; idx < config->regionCnt; ++idx) {
        region = &config->regions[idx];
        type = region->type;
        if ((type >= LOS_SHM_TYPE_MAX) || (region->size == 0)) {
            return LOS_NOK;
        }
        if ((region->startAddr == 0) || (region->startAddr > (UINT32_MAX - region->size))) {
            return LOS_NOK;
        }
#ifndef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
        if (LOS_SHM_IS_SEC(type)) {
            return LOS_NOK;
        }
#endif
        regionStart = SHM_ALIGN_UP(region->startAddr, pageAlign);
        regionEnd = SHM_ALIGN_DN(region->startAddr + region->size, pageAlign);
        if (regionStart >= regionEnd) {
            return LOS_NOK;
        }
    }

    return LOS_OK;
}

STATIC UINT32 ShmCheckInput(DynShmConfig *config)
{
    UINT32 pageSize;

    if ((config == NULL) || (config->regionCnt > DYN_SHM_RGN_MAX) || (config->regionCnt == 0) ||
        (config->flushTlbHandler == NULL)) {
        return LOS_NOK;
    }

    pageSize = config->pageSize;
    if ((pageSize != PAGE_SIZE_4K) && (pageSize != PAGE_SIZE_16K) && (pageSize != PAGE_SIZE_64K)) {
        SHM_PRINT_ERR("[%s %d] pageSize:%#x \n", __FUNCTION__, __LINE__, pageSize);
        return LOS_NOK;
    }

    if ((ShmCheckPagetableInput(&config->outerPageTable) != LOS_OK) ||
        (ShmCheckPagetableInput(&config->innerPageTable) != LOS_OK)) {
        return LOS_NOK;
    }

    return ShmCheckRegionInput(config);
}

STATIC VOID ShmRegionCBInit(RegionManager *handle, UINT32 regionStart, UINT32 regionEnd)
{
    LOS_ListInit(&handle->usedBlkListHead.listNode);
    handle->totalFreeSize = regionEnd - regionStart;
    handle->usedBlkListHead.virAddr = regionStart;
    handle->usedBlkListHead.size = 0;

    handle->usedBlkListEnd.virAddr = regionEnd;
    handle->usedBlkListEnd.size = 0;
    LOS_ListTailInsert(&handle->usedBlkListHead.listNode, &handle->usedBlkListEnd.listNode);
}

STATIC UINT32 ShmManagerInit(VOID)
{
    UINT32 pageAlign = g_shmCB.userCfg.pageSize;
    UINT8 idx;
    UINT32 regionStart, regionEnd, type, ret;
    DynShmRegion *region = NULL;
    UsedRegionRange regionRange = {
        {(UINT32)-1, 0},
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
        {(UINT32)-1, 0}
#endif
    };

    for (idx = 0; idx < g_shmCB.userCfg.regionCnt; ++idx) {
        region = &g_shmCB.userCfg.regions[idx];
        type = region->type;

        regionStart = SHM_ALIGN_UP(region->startAddr, pageAlign);
        regionEnd = SHM_ALIGN_DN(region->startAddr + region->size, pageAlign);

        ShmRegionCBInit(&g_shmCB.regionCB[idx], regionStart, regionEnd);

        SHM_PRINT_INFO("[%s %d] rgn%u type:%u [%#x, %#x], aligned [%#x, %#x], pageAlign: %#x, size: %#x\n",
            __FUNCTION__, __LINE__, idx, type, region->startAddr, region->startAddr + region->size,
            regionStart, regionEnd, pageAlign, regionEnd - regionStart);
        if ((idx == 0) && (IommuSetPmpRgnStart(regionStart) != LOS_OK)) {
            return LOS_NOK;
        }
        if (IommuSetPmpRgnEnd(type, regionEnd) != LOS_OK) {
            return LOS_NOK;
        }

        if (!LOS_SHM_IS_MMZ(type)) {
            ShmSetExtremeAddr(&regionRange, regionStart, regionEnd, type);
            region->nextRgnIdx = g_shmCB.rgnIdxMap[type];
            g_shmCB.rgnIdxMap[type] = idx;
        }
    }
    ShmUnusedPtInfoSet(&g_shmCB.unusedPtInfo, &regionRange);
    ret = ShmUnusedPtPoolInit();

    return ret;
}

STATIC UINT32 ShmCheckAndSetModuleStatus(UINT32 status)
{
    UINT32 intSave;
    UINT32 ret = LOS_OK;

    DYNSHM_LOCK(intSave);
    if (g_dynShmInitFlag != DYN_SHM_STATUS_UNINIT) {
        ret = LOS_NOK;
        goto OUT;
    }
    g_dynShmInitFlag = status;

OUT:
    DYNSHM_UNLOCK(intSave);
    return ret;
}

STATIC INLINE VOID ShmSetModuleStatus(UINT32 status)
{
    UINT32 intSave;

    DYNSHM_LOCK(intSave);
    g_dynShmInitFlag = status;
    DYNSHM_UNLOCK(intSave);
}

UINT32 LOS_DynShmInit(DynShmConfig *config)
{
    UINT32 index;
    UINT32 ret = LOS_OK;

    if (ShmCheckAndSetModuleStatus(DYN_SHM_STATUS_INITIALIZING) != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_INIT_REPEATEDLY;
    }

    if (ShmCheckInput(config) != LOS_OK) {
        ret = LOS_ERRNO_DYN_SHM_INPUT_INVALID;
        goto RECOVER_MODULE_STATUS;
    }

    if (LOS_MuxCreate(&g_shmMutex) != LOS_OK) {
        ret = LOS_ERRNO_DYN_SHM_INIT_FAILED;
        goto RECOVER_MODULE_STATUS;
    }

    if (LOS_MuxCreate(&g_blockMutex) != LOS_OK) {
        ret = LOS_ERRNO_DYN_SHM_INIT_FAILED;
        goto DELETE_SHM_MUX;
    }

    (VOID)memcpy(&g_shmCB.userCfg, config, sizeof(DynShmConfig));
    (VOID)memset((VOID *)(UINTPTR)g_shmCB.rgnIdxMap, REGION_IDX_MAP_INIT_VAL, sizeof(g_shmCB.rgnIdxMap));
    if (IommuSetPtPmpRegion(&config->outerPageTable) != LOS_OK) {
        ret = LOS_ERRNO_DYN_SHM_INPUT_INVALID;
        goto DELETE_ALL_MUX;
    }
    (VOID)memset((VOID *)(UINTPTR)config->innerPageTable.nonsec.baseAddr, 0, config->innerPageTable.nonsec.tableSize);
#ifdef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    (VOID)memset((VOID *)(UINTPTR)config->innerPageTable.sec.baseAddr, 0, config->innerPageTable.sec.tableSize);
#endif

    LOS_ListInit(&g_losFreeBlock);
    for (index = 0; index < LOSCFG_KERNEL_DYNSHM_VM_BLOCK_NUM; index++) {
        LOS_ListTailInsert(&g_losFreeBlock, &g_shmCB.blocks[index].listNode);
    }

    if (ShmManagerInit() != LOS_OK) {
        ret = LOS_ERRNO_DYN_SHM_INIT_FAILED;
        goto DELETE_ALL_MUX;
    }

    ShmSetModuleStatus(DYN_SHM_STATUS_READY);

    return LOS_OK;

DELETE_ALL_MUX:
    (VOID)LOS_MuxDelete(g_blockMutex);
DELETE_SHM_MUX:
    (VOID)LOS_MuxDelete(g_shmMutex);
RECOVER_MODULE_STATUS:
    ShmSetModuleStatus(DYN_SHM_STATUS_UNINIT);

    return ret;
}

STATIC DynShmArea *ShmGetBlockFromFreeList(VOID)
{
    DynShmArea *block = NULL;
    LOS_DL_LIST *firstNode = NULL;

    (VOID)LOS_MuxPend(g_blockMutex, LOS_WAIT_FOREVER);
    if (LOS_ListEmpty(&g_losFreeBlock)) {
        goto OUT;
    }
    firstNode = LOS_DL_LIST_FIRST(&g_losFreeBlock);
    block = LOS_DL_LIST_ENTRY(firstNode, DynShmArea, listNode);
    LOS_ListDelete(firstNode);

OUT:
    (VOID)LOS_MuxPost(g_blockMutex);
    return block;
}

STATIC VOID ShmAddBlock2Freelist(DynShmArea *block)
{
    (VOID)LOS_MuxPend(g_blockMutex, LOS_WAIT_FOREVER);
    LOS_ListHeadInsert(&g_losFreeBlock, &block->listNode);
    (VOID)LOS_MuxPost(g_blockMutex);
}

STATIC UINT32 ShmAllocFromRegion(UINT8 regionId, DynShmArea *freeBlk)
{
    RegionManager *handle = &g_shmCB.regionCB[regionId];

    LOS_DL_LIST *head = &handle->usedBlkListHead.listNode;
    LOS_DL_LIST *tail = &handle->usedBlkListEnd.listNode;
    LOS_DL_LIST *item = head;

    DynShmArea *curBlock = NULL;
    DynShmArea *nextBlock = NULL;

    UINT32 memSize = freeBlk->size;
    UINT32 ret = LOS_OK;

    (VOID)LOS_MuxPend(g_shmMutex, LOS_WAIT_FOREVER);
    while (item != tail) {
        curBlock = LOS_DL_LIST_ENTRY(item, DynShmArea, listNode);
        nextBlock = LOS_DL_LIST_ENTRY(item->pstNext, DynShmArea, listNode);
        if ((nextBlock->virAddr - (curBlock->virAddr + curBlock->size)) > memSize) {
            break;
        }
        item = item->pstNext;
    }
    if (item == tail) {
        ret = LOS_NOK;
        goto OUT;
    }

    freeBlk->virAddr = curBlock->virAddr + curBlock->size;
    ret = IommuCopyPageTable(&g_shmCB.userCfg, freeBlk->phyAddr, freeBlk->virAddr, memSize,
        LOS_SHM_IS_SEC(freeBlk->type));
    if (ret != LOS_OK) {
        goto OUT;
    }
    SHM_PRINT_DEBUG("[%s %d] mapped block phyAddr:%#llx -> virAddr:%#lx size:%#x\n", __FUNCTION__, __LINE__,
        freeBlk->phyAddr, freeBlk->virAddr, memSize);
    LOS_ListAdd(&curBlock->listNode, &freeBlk->listNode);
    handle->totalFreeSize -= memSize;
    SHM_PRINT_INFO("[%s %d] region%u left free size:%#x\n", __FUNCTION__, __LINE__, regionId, handle->totalFreeSize);

OUT:
    (VOID)LOS_MuxPost(g_shmMutex);
    return ret;
}

STATIC UINT32 ShmAllocVirAddr(DynShmMemType memType, DynShmArea *block)
{
    UINT8 idx;
    UINT32 ret;
    DynShmRegion *region = NULL;

    for (idx = g_shmCB.rgnIdxMap[memType]; idx != REGION_IDX_MAP_INIT_VAL;) {
        SHM_PRINT_DEBUG("[%s %d] current region[%u]\n", __FUNCTION__, __LINE__, idx);
        region = &g_shmCB.userCfg.regions[idx];
        ret = ShmAllocFromRegion(idx, block);
        if (ret == LOS_OK) {
            SHM_PRINT_INFO("[%s %d] virAddr %#lx alloc from region[%u]\n", __FUNCTION__, __LINE__,
                block->virAddr, idx);
            return LOS_OK;
        }

        idx = (UINT8)region->nextRgnIdx;
    }
    return LOS_NOK;
}

STATIC DynShmArea *ShmFreeFromRegion(UINT8 regionId, UINT32 virAddr)
{
    RegionManager *handle = &g_shmCB.regionCB[regionId];
    LOS_DL_LIST *head = &handle->usedBlkListHead.listNode;
    LOS_DL_LIST *tail = &handle->usedBlkListEnd.listNode;
    LOS_DL_LIST *item = head->pstNext;
    DynShmArea *curBlock = NULL;
    UINT32 ret;

    if ((virAddr >= handle->usedBlkListEnd.virAddr) || (virAddr < handle->usedBlkListHead.virAddr)) {
        return NULL;
    }

    (VOID)LOS_MuxPend(g_shmMutex, LOS_WAIT_FOREVER);
    while (item != tail) {
        curBlock = LOS_DL_LIST_ENTRY(item, DynShmArea, listNode);
        if ((virAddr < (curBlock->virAddr + curBlock->size)) && (virAddr >= curBlock->virAddr)) {
            break;
        }
        item = item->pstNext;
    }
    if (item == tail) {
        curBlock = NULL;
        goto OUT;
    }

    ret = IommuCleanPageTable(&g_shmCB.userCfg, curBlock->virAddr, curBlock->size, LOS_SHM_IS_SEC(curBlock->type));
    if (ret != LOS_OK) {
        curBlock = NULL;
        goto OUT;
    }

    LOS_ListDelete(&curBlock->listNode);
    handle->totalFreeSize += curBlock->size;
    SHM_PRINT_DEBUG("[%s %d] unmapped block phyAddr:%#llx -> virAddr:%#lx size:%#x\n", __FUNCTION__, __LINE__,
        curBlock->phyAddr, curBlock->virAddr, curBlock->size);
    SHM_PRINT_INFO("[%s %d] region%u left free size:%#x\n", __FUNCTION__, __LINE__, regionId, handle->totalFreeSize);

OUT:
    (VOID)LOS_MuxPost(g_shmMutex);
    return curBlock;
}

STATIC DynShmArea *ShmFreeVirAddr(UINT32 virAddr)
{
    UINT8 idx;
    DynShmArea *block = NULL;

    for (idx = 0; idx < g_shmCB.userCfg.regionCnt; ++idx) {
        block = ShmFreeFromRegion(idx, virAddr);
        if (block != NULL) {
            return block;
        }
    }

    return NULL;
}

#define SHM_BLOCK_NOT_MAPPED       0x0
#define SHM_BLOCK_IS_SUITABLE      0x1
#define SHM_BLOCK_ALREADY_MAPPED   0x2

STATIC UINT32 ShmBlockIsAlreadyMapped(UINT8 regionId, UADDR inAddr, UINT32 size, UINTPTR *outAddr)
{
    RegionManager *handle = &g_shmCB.regionCB[regionId];

    LOS_DL_LIST *head = &handle->usedBlkListHead.listNode;
    LOS_DL_LIST *tail = &handle->usedBlkListEnd.listNode;
    LOS_DL_LIST *item = head->pstNext;

    DynShmArea *curBlock = NULL;

    UINT32 ret = SHM_BLOCK_NOT_MAPPED;
    *outAddr = 0;

    (VOID)LOS_MuxPend(g_shmMutex, LOS_WAIT_FOREVER);
    while (item != tail) {
        curBlock = LOS_DL_LIST_ENTRY(item, DynShmArea, listNode);
        if ((inAddr >= curBlock->phyAddr) && ((inAddr + size) <= (curBlock->phyAddr + curBlock->size))) {
            *outAddr = curBlock->virAddr + (UINTPTR)(inAddr - curBlock->phyAddr);
            ret = SHM_BLOCK_IS_SUITABLE;
            break;
        }
        if ((inAddr >= curBlock->phyAddr) && (inAddr < (curBlock->phyAddr + curBlock->size))) {
            ret = SHM_BLOCK_ALREADY_MAPPED;
            break;
        }
        item = item->pstNext;
    }

    (VOID)LOS_MuxPost(g_shmMutex);
    return ret;
}

STATIC UINT32 ShmFindMappedBlock(UADDR inAddr, UINT32 size, DynShmMemType memType, UINTPTR *outAddr)
{
    UINT8 idx;
    UINT32 ret;
    DynShmRegion *region = NULL;

    for (idx = g_shmCB.rgnIdxMap[memType]; idx != REGION_IDX_MAP_INIT_VAL;) {
        SHM_PRINT_DEBUG("[%s %d] current region[%u]\n", __FUNCTION__, __LINE__, idx);
        region = &g_shmCB.userCfg.regions[idx];
        ret = ShmBlockIsAlreadyMapped(idx, inAddr, size, outAddr);
        if (ret == SHM_BLOCK_ALREADY_MAPPED) {
            SHM_PRINT_ERR("[%s %d] inAddr is already mapped\n", __FUNCTION__, __LINE__);
            return ret;
        } else if (ret == SHM_BLOCK_IS_SUITABLE) {
            SHM_PRINT_INFO("[%s %d] inAddr %#llx, size:%#x is just matched\n", __FUNCTION__, __LINE__, inAddr, size);
            return ret;
        }

        idx = (UINT8)region->nextRgnIdx;
    }
    return SHM_BLOCK_NOT_MAPPED;
}

UINT32 LOS_DynShmMap(UADDR inAddr, UINT32 size, DynShmMemType memType, UINTPTR *outAddr)
{
    UINT32 ret;
    DynShmArea *block = NULL;
    UINTPTR tempAddr;
    UADDR alignedInAddr;
    UINT32 alignedSize;
    UADDR alignedSize64;

    SHM_PRINT_INFO("[%s %d] inAddr:%#llx, memType:%d, size:%#x\n", __FUNCTION__, __LINE__, inAddr, memType, size);
    if ((outAddr == NULL) || (((UINTPTR)outAddr & (sizeof(UINTPTR) - 1)) != 0) || (memType >= LOS_SHM_TYPE_MAX) ||
        (size == 0) || (inAddr > ((1ULL << LOSCFG_DRIVERS_IOMMU_VA_BITS) - size))) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }

#ifndef LOSCFG_KERNEL_DYNSHM_SECURE_ENBALE
    if (LOS_SHM_IS_SEC(memType)) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
#endif

    if (ShmCheckStatusReady() != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_NOT_READY;
    }

    if (LOS_SHM_IS_MMZ(memType)) {
        *outAddr = (UINTPTR)inAddr;
        return LOS_OK;
    }

    CHECK_INT_ACTIVE_RETURN();

    /* Check if inAddr is mapped before */
    alignedInAddr = SHM_ALIGN_DN(inAddr, g_shmCB.userCfg.pageSize);
    alignedSize64 = SHM_ALIGN_UP(inAddr + size, g_shmCB.userCfg.pageSize) - alignedInAddr;
    if (alignedSize64 > 0xFFFFFFFFULL) { /* 0xFFFFFFFFULL: no more than UINT32_MAX */
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    alignedSize = (UINT32)alignedSize64;
    ret = ShmFindMappedBlock(alignedInAddr, alignedSize, memType, &tempAddr);
    if (ret == SHM_BLOCK_IS_SUITABLE) {
        *outAddr = tempAddr + (inAddr - alignedInAddr);
        return LOS_OK;
    } else if (ret == SHM_BLOCK_ALREADY_MAPPED) {
        *outAddr = 0;
        return LOS_ERRNO_DYN_SHM_ALREADY_MAPPED;
    }

    block = ShmGetBlockFromFreeList();
    if (block == NULL) {
        return LOS_ERRNO_DYN_SHM_NO_MEMORY;
    }
    block->type = memType;
    block->phyAddr = alignedInAddr;
    block->size = alignedSize;

    ret = ShmAllocVirAddr(memType, block);
    if (ret != LOS_OK) {
        ShmAddBlock2Freelist(block);
        SHM_PRINT_ERR("[%s %d] alloc virtual space failed\n", __FUNCTION__, __LINE__);
        return LOS_ERRNO_DYN_SHM_NO_MEMORY;
    }

    *outAddr = block->virAddr + (inAddr - block->phyAddr);
    SHM_PRINT_INFO("[%s %d] outAddr:%#lx\n", __FUNCTION__, __LINE__, *outAddr);

    return LOS_OK;
}

UINT32 LOS_DynShmUnmap(UINTPTR virAddr, UINT32 size)
{
    DynShmArea *block = NULL;

    if ((size == 0) || (virAddr > ((UINTPTR)(-1) - size))) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }

    if (ShmCheckStatusReady() != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_NOT_READY;
    }

    CHECK_INT_ACTIVE_RETURN();
    SHM_PRINT_INFO("[%s %d] virAddr:%#lx, size:%#x\n", __FUNCTION__, __LINE__, virAddr, size);

    block = ShmFreeVirAddr(virAddr);
    if (block == NULL) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }

    ShmAddBlock2Freelist(block);
    return LOS_OK;
}

UINT32 ShmGetBlockId(DynShmArea *block)
{
    return ((UINTPTR)block - (UINTPTR)g_shmCB.blocks) / sizeof(g_shmCB.blocks[0]);
}

typedef struct {
    UADDR addr;
    UINT32 type;
} BlockAttr;

STATIC UINT32 ShmGetAddrAndType(UADDR inAddr, BlockAttr *attr, BOOL inAddrIsInner)
{
    UADDR inAddrStart, inAddrEnd, outAddrStart;
    UINT8 idx;
    RegionManager *handle = NULL;
    LOS_DL_LIST *head = NULL;
    LOS_DL_LIST *tail = NULL;
    LOS_DL_LIST *item = NULL;
    DynShmArea *block = NULL;

    SHM_PRINT_DEBUG("[%s %d] inAddr:%#llx\n", __FUNCTION__, __LINE__, inAddr);

    (VOID)LOS_MuxPend(g_shmMutex, LOS_WAIT_FOREVER);
    for (idx = 0; idx < g_shmCB.userCfg.regionCnt; ++idx) {
        handle = &g_shmCB.regionCB[idx];
        head = &handle->usedBlkListHead.listNode;
        tail = &handle->usedBlkListEnd.listNode;
        item = head->pstNext;

        while (item != tail) {
            block = LOS_DL_LIST_ENTRY(item, DynShmArea, listNode);
            if (inAddrIsInner == TRUE) {
                inAddrStart = block->virAddr;
                outAddrStart = block->phyAddr;
            } else {
                inAddrStart = block->phyAddr;
                outAddrStart = block->virAddr;
            }
            inAddrEnd = inAddrStart + block->size;
            SHM_PRINT_INFO("[%s %d] block%u inAddrStart:%#llx, outAddrStart:%#llx, size = %#x, type = %u\n",
                __FUNCTION__, __LINE__, ShmGetBlockId(block), inAddrStart, outAddrStart, block->size, block->type);

            if ((inAddr >= inAddrStart) && (inAddr < inAddrEnd)) {
                attr->addr = outAddrStart + (inAddr - inAddrStart);
                attr->type = block->type;
                SHM_PRINT_DEBUG("[%s %d] outAddr:%#llx, type:%u\n", __FUNCTION__, __LINE__, attr->addr, attr->type);
                (VOID)LOS_MuxPost(g_shmMutex);
                return LOS_OK;
            }
            item = item->pstNext;
        }
    }

    (VOID)LOS_MuxPost(g_shmMutex);
    return LOS_NOK;
}

STATIC UINT32 ShmInner2OuterVirAddr(UADDR inAddr, UADDR *outAddr)
{
    BlockAttr attr;
    if (ShmGetAddrAndType(inAddr, &attr, TRUE) != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    *outAddr = attr.addr;
    return LOS_OK;
}

STATIC UINT32 ShmOuter2InnerVirAddr(UADDR inAddr, UADDR *outAddr)
{
    BlockAttr attr;
    if (ShmGetAddrAndType(inAddr, &attr, FALSE) != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    *outAddr = attr.addr;
    return LOS_OK;
}

STATIC UINT32 ShmInnerVir2PhyAddr(UADDR inAddr, UADDR *outAddr)
{
    BlockAttr attr;
    PageTableConfig *pt = NULL;
    if (ShmGetAddrAndType(inAddr, &attr, TRUE) != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    if (LOS_SHM_IS_SEC(attr.type)) {
        pt = &g_shmCB.userCfg.innerPageTable.sec;
    } else {
        pt = &g_shmCB.userCfg.innerPageTable.nonsec;
    }
    return IommuVir2PhyAddr(pt, inAddr, outAddr, g_shmCB.userCfg.pageSize);
}

STATIC UINT32 ShmOuterVir2PhyAddr(UADDR inAddr, UADDR *outAddr)
{
    BlockAttr attr;
    PageTableConfig *pt = NULL;
    if (ShmGetAddrAndType(inAddr, &attr, FALSE) != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }
    if (LOS_SHM_IS_SEC(attr.type)) {
        pt = &g_shmCB.userCfg.outerPageTable.sec;
    } else {
        pt = &g_shmCB.userCfg.outerPageTable.nonsec;
    }
    return IommuVir2PhyAddr(pt, inAddr, outAddr, g_shmCB.userCfg.pageSize);
}

UINT32 LOS_DynShmAddrTranslate(UADDR inAddr, UADDR *outAddr, DynShmTransType optType)
{
    UINT32 ret;

    if ((optType >= SHM_ADDR_TRANS_TYPE_MAX) || (outAddr == NULL) ||
        (((UINTPTR)outAddr & (sizeof(UADDR) - 1)) != 0)) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }

    if (ShmCheckStatusReady() != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_NOT_READY;
    }

    CHECK_INT_ACTIVE_RETURN();

    switch (optType) {
        case INNER_VA_TO_OUTER_VA:
            ret = ShmInner2OuterVirAddr(inAddr, outAddr);
            break;
        case OUTER_VA_TO_INNER_VA:
            ret = ShmOuter2InnerVirAddr(inAddr, outAddr);
            break;
        case INNER_VA_TO_MEM_PA:
            ret = ShmInnerVir2PhyAddr(inAddr, outAddr);
            break;
        case OUTER_VA_TO_MEM_PA:
            ret = ShmOuterVir2PhyAddr(inAddr, outAddr);
            break;
        default:
            ret = LOS_ERRNO_DYN_SHM_INPUT_INVALID;
            break;
    }
    return ret;
}

VOID *LOS_DynShmBulkMalloc(DynShmBulkMem *mem)
{
    UINTPTR virAddr;
    UINT32 ret;

    if ((mem == NULL) || (g_shmCB.userCfg.bulkAllocHandler == NULL) || (g_shmCB.userCfg.bulkFreeHandler == NULL)) {
        return NULL;
    }
    SHM_PRINT_DEBUG("[%s %d] type = %u, size=%#x, phy: %#llx\n",
        __FUNCTION__, __LINE__, mem->type, mem->size, mem->phyAddr);

    mem->virAddr = 0;
    /* phyAddr will be adjusted in bulkAllocHandler */
    ret = g_shmCB.userCfg.bulkAllocHandler(mem);
    if (ret != LOS_OK) {
        return NULL;
    }

    if (mem->noNeedMap == TRUE) {
        virAddr = (UINTPTR)(-1);
        goto OUT;
    }

    ret = LOS_DynShmMap(mem->phyAddr, mem->size, mem->type, &virAddr);
    if (ret != LOS_OK) {
        g_shmCB.userCfg.bulkFreeHandler(mem);
        return NULL;
    }

OUT:
    mem->virAddr = virAddr;
    return (VOID *)(UINTPTR)virAddr;
}

UINT32 LOS_DynShmBulkFree(DynShmBulkMem *mem)
{
    UINT32 ret;
    if ((mem == NULL) || (g_shmCB.userCfg.bulkFreeHandler == NULL)) {
        return LOS_ERRNO_DYN_SHM_INPUT_INVALID;
    }

    if (LOS_SHM_IS_MMZ(mem->type) || (mem->noNeedMap == TRUE)) {
        goto BULK_FREE;
    }

    ret = LOS_DynShmUnmap(mem->virAddr, mem->size);
    if (ret != LOS_OK) {
        return ret;
    }

BULK_FREE:
    return g_shmCB.userCfg.bulkFreeHandler(mem);
}

#ifdef LOSCFG_KERNEL_DYNSHM_1_LEVEL_PAGETABLE_REUSE
VOID *LOS_DynShmUnusedPtMemAlloc(UINT32 size)
{
    FreePageTableConfig *cfg = &g_shmCB.unusedPtInfo.nonsec;
    VOID *poolStartAddr = (VOID *)(UINTPTR)cfg->lowerStartAddr;
    VOID *addr = NULL;
    if (ShmCheckStatusReady() != LOS_OK) {
        return NULL;
    }
    addr = LOS_MemAlloc(poolStartAddr, size);
    if (addr != NULL) {
        return addr;
    }
    poolStartAddr = (VOID *)(UINTPTR)cfg->higherStartAddr;
    return LOS_MemAlloc(poolStartAddr, size);
}

UINT32 LOS_DynShmUnusedPtMemFree(VOID *addr)
{
    FreePageTableConfig *cfg = &g_shmCB.unusedPtInfo.nonsec;
    VOID *poolStartAddr = (VOID *)(UINTPTR)cfg->lowerStartAddr;
    UINT32 ret;
    if (ShmCheckStatusReady() != LOS_OK) {
        return LOS_ERRNO_DYN_SHM_NOT_READY;
    }
    ret = LOS_MemFree(poolStartAddr, addr);
    if (ret == LOS_OK) {
        return LOS_OK;
    }
    poolStartAddr = (VOID *)(UINTPTR)cfg->higherStartAddr;
    return LOS_MemFree(poolStartAddr, addr);
}
#endif
