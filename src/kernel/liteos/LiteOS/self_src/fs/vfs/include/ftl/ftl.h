/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Flash Transfer Layer Module Headfile
 * Author: Huawei LiteOS Team
 * Create: 2023-07-06
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
#ifndef _FTL_H
#define _FTL_H

#include "ftl_node.h"
#include "ftl_compress.h"
#include "los_mux.h"
#include "los_list.h"
#include "inode/inode.h"
#include "mtd_partition.h"
#include "linux/mtd/mtd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_FS_FTL_DEBUG
    #define FTL_DEBUG(fmt, args...) PRINTK(fmt, ##args)
#else
    #define FTL_DEBUG(...)
#endif

#define FTL_INVALID_THREAD_ID 0xFFFFFFFF

typedef enum {
    BLOCK_CLEAN = 0, /* block can be used for alloc */
    BLOCK_ALLOC,     /* block is locked by allocator */
    BLOCK_DIRTY,     /* block is writeen all region */
    BLOCK_ERASE,     /* block head is all 0xFF, need be erased */
    BLOCK_BAD,
    BLOCK_UNKNOWN
} BlockStatus;

typedef struct {
    FtlMapInode inode;
} FtlInodeCache;

typedef struct {
    UINT32 blockNum;
    UINT32 blockSeq;
    BlockStatus blockStatus;
} FtlBlockSeq;

typedef struct {
    LOS_DL_LIST listNode;

    UINT16 num;             /* Block num */
    UINT16 eraseTimes;      /* The more erase times, the block is more hard to be chosen for gc */

    UINT32 seq;             /* Seq, more bigger the data more new */
    UINT32 curAllocPos;     /* Alloc seg position int the block */
    UINT32 usedSize;        /* Used size */
    UINT32 obsoleteSegSize; /* The more obsolete pages, the block is more easy to be chosen for gc */
} FtlBlockInfo;

typedef struct {
    VOID *priv; /* Mtd info */
    UINT32 refCnt;
    UINT32 ftlRefCntLock;
    UINT32 ftlLock;
    UINT32 gcThreadId;
    EVENT_CB_S gcEvent; /* event for GC */

    LOS_DL_LIST blockList[BLOCK_UNKNOWN + 1];

    FtlBlockInfo *allocBlock;
    FtlBlockInfo *blockArray;
    UINT32 curSeqNum; /* Block alloc seq nums, the same data in block(seq n) is more new than block(seq n-1) */

    UINT32 phyOffset;
    UINT32 blockPages;
    UINT32 blockSize;
    UINT32 pageSize;
    UINT32 oobSize; /* In nor flash, size is FtlInodeTag */

    UINT32 totalBlocks; /* Total blocks of the partition */
    UINT32 userBlocks;  /* Total blocks can be used by application user */
    UINT32 userPages;   /* Total pages can be used by application user */

    UINT32 cleanBlocks; /* Total clean blocks that can be allocated */
    UINT32 obsoleteSegSize; /* Total obsolete segment size of all blocks */

    BOOL tagOnBand; /* No oob region, store tag in data region */
    UINT8 *buffer;
    UINT8 *dataBuffer;
    UINT8 *tagBuffer;
    UINT8 *blockScanBuffer;

    FtlInodeCache *inodeCache;
} FtlManage;

extern INT32 FtlLogicalPageTrim(FtlManage *ftl, UINT32 lPage);
extern INT32 FtlLogicalPageWrite(FtlManage *ftl, const UINT8 *buf, UINT32 lPage);
extern INT32 FtlLogicalPageRead(FtlManage *ftl, UINT8 *buf, UINT32 lPage);
extern FtlManage *FtlDevInit(VOID *priv);
extern VOID FtlDevDeinit(FtlManage *ftl);
extern INT32 FtlBlockDriverRegister(const CHAR *mtdDev);
extern INT32 FtlBlockDriverUnregister(const CHAR *mtdDev);
extern INT32 FtlBlockTrim(FAR struct inode *inode, UINT64 startPage, UINT32 nPages);
extern INT32 FtlDevOpen(FtlManage *ftl);
extern INT32 FtlDevCLose(FtlManage *ftl);
extern INT32 FtlErase(FAR struct inode *inode, UINT32 start_sector, UINT32 nsectors);
extern INT32 DevBlockErase(FtlManage *ftl, UINT32 blockNum);
extern INT32 DevUpdateBlockInfo(FtlManage *ftl);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* _FTL_H */
