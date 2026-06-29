/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Ftl image build Module Header files
 * Author: Huawei LiteOS Team
 * Create: 2023-5-6
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

#ifndef _FTL_IMAGE_H
#define _FTL_IMAGE_H

#include "ff.h"
#include "los_typedef.h"
#include "ftl_node.h"
#include "linux/crc32.h"
#include "securec.h"

#define KB2BYTE              1024
#define FTL_IMAGE_NAME       "ftl.bin"
#define SECTOR_SIZE          FF_MAX_SS
#define IMAGE_INODE_MAP_NUMS (1024 * 1024)
#define IMAGE_MAX_SIZE       (1024 * 1024 * 64)

typedef struct {
    FtlMapInode inode;
    UINT8 *buffer;
} FtlImageInode;

typedef struct {
    BOOL compressed; /* Image data compress(zlib) enable  */

    UINT32 partitionSize;
    UINT32 blockSize;
    UINT32 pageSize;
    UINT32 imageSize;

    UINT32 curBlockSeq;
    UINT32 curWritePos;

    UINT8 *summaryBuffer;
    UINT8 *virtualBuffer;
    FtlImageInode *inodeMap;
} FtlImageContext;

extern INT32 FtlImageCreatFs(VOID);
extern INT32 FtlImageMountFs(VOID);
extern INT32 FtlImageCreatDir(const CHAR *dir);
extern VOID *FtlImageCreatFile(const CHAR *file);
extern INT32 FtlImageWriteFile(VOID *fp, UINT8 *buffer, UINT32 bufLen);
extern INT32 FtlImageCloseFile(VOID *fp);

extern INT32 ParseDir(const CHAR *path);
extern INT32 FtlImageBuild(FtlImageContext *image, const char *fileName);

extern FtlImageContext *FtlImageContextGet(VOID);
extern INT32 FtlDataCompress(const UINT8 *src, UINT32 srcLen, UINT8 *dst, UINT32 *dstLen);
extern INT32 FtlDataDecompress(UINT8 *src, UINT32 srcLen, UINT8 *dst, UINT32 dstLen);

#endif