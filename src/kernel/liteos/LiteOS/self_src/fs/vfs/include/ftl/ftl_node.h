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
#ifndef _FTL_NODE_H
#define _FTL_NODE_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SEGMENT_USED_MAGIC 0x0A050A05
#define BLOCK_FF_MAGIC     0xFFFFFFFF
#define BLOCK_CLEAN_MAGIC  0x434C4541

#define FTL_CRC_SIZE        8
#define FTL_ALIGN_SIZE      4
#define FTL_SEARCH_SIZE     4
#define FTL_SECTOR_SIZE     512
#define FTL_RESERVED_BLOCKS 2
#define BLOCK_ERROR_TIMES   3
#define FTL_CLEANMARK_SIZE  ((UINT32)sizeof(FtlInodeTag))

#define FTL_ALIGN(x)       (((x) + (FTL_ALIGN_SIZE - 1)) & ~(FTL_ALIGN_SIZE - 1))

#ifdef LOSCFG_FS_FAT_FTL_COMPRESS
#define FTL_BLOCK_MAX_BIT        10
#define FTL_BLOCK_PAGE_MAX_BIT   7
#define FTL_LOGICAL_PAGE_MAX_BIT 21
typedef struct {
    UINT32 used : 1;                                    /* node status */
    UINT32 lPage : FTL_LOGICAL_PAGE_MAX_BIT;            /* logical page */
    UINT32 blockNum : FTL_BLOCK_MAX_BIT;
    UINT16 pageOffsetInBlock;                           /* page address in block */
    UINT16 segSize;                                     /* page size */
} __attribute__ ((__packed__)) FtlMapInode;

#else

#define FTL_BLOCK_MAX_BIT        9
#define FTL_BLOCK_PAGE_MAX_BIT   7
#define FTL_LOGICAL_PAGE_MAX_BIT 15
typedef struct {
    UINT32 used : 1;                                    /* node status */
    UINT32 lPage : FTL_LOGICAL_PAGE_MAX_BIT;            /* logical page */
    UINT32 blockNum : FTL_BLOCK_MAX_BIT;
    UINT32 pageOffsetInBlock : FTL_BLOCK_PAGE_MAX_BIT;  /* page index in block */
} __attribute__ ((__packed__)) FtlMapInode;
#endif

typedef struct {
    UINT32 tagFlag;
    UINT32 seqNum;
    FtlMapInode inode;
    UINT32 tagCrc;
    UINT32 dataCrc;
} __attribute__ ((__packed__)) FtlInodeTag;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* _FTL_NODE_H */
