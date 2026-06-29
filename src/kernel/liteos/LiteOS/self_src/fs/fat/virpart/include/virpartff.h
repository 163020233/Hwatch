/* -----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/
 * or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------- */

#ifndef _VIRPARTFF_H
#define _VIRPARTFF_H

#include "ff.h"

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION

#define FS_PARENT       0x00
#define FS_CHILD        0x01

#define FS_VIRENABLE    0x01
#define FS_VIRDISABLE   0x00

#define PARENTFS(fs)    ((FATFS*)((fs)->parent_fs))
#define CHILDFS(fs, i)  ((FATFS*)(*((fs)->child_fs + (i))))

#define _DEFAULT_VIRVOLUEMS         4
#define _MIN_CLST                   0x4000
#define _FLOAT_ACC                  0.00000001

/* Global Data Area @ 0x00 - 0x31 (32 Bytes) */
#define VR_ITEMSIZE                 0x20
#define VR_GLOBAL                   0x00
#define VR_PartitionCnt             0x00    /* Virtual Partition Amount */
#define VR_PartitionFSType          0x01    /* Filesystem Type (1 Byte) */
#define VR_PartitionStSec           0x02    /* Partition Start Sector (4 Bytes) */
#define VR_PartitionCtSec           0x06    /* Partition Sectors Count (4 Bytes) */
#define VR_PartitionClstSz          0x0A    /* Partition Clust Size (2 Bytes) */
#define VR_PartitionCtClst          0x0C    /* Partition Clust Amount (4 Bytes) */

/* Each Virtual Partition Area (32 Bytes) */
/* Start offset at 0x20, Each entry is at 32 Bytes */
#define VR_PARTITION                0x20
#define VR_Available                0x00    /* Virtual Partition Item Flag (1 Bytes) */
#define VR_StartClust               0x01    /* Virtual Partition Start Cluster (4 Bytes) */
#define VR_CountClust               0x05    /* Virtual Partition Cluster Count (4 Bytes) */
#define VR_Entry                    0x10    /* Virtual Partition Entry Label (16 Bytes) */

#define VR_VerifyString            0x1FC   /* Verify String (4 Bytes), Must be "LITE" */

#define ISVIRPART(fs)   ((fs)->vir_avail == FS_VIRENABLE)
#define ISNORMAL(fs)    ((fs)->vir_avail != FS_VIRENABLE)

#define ISPARENT(fs)    ((fs)->vir_flag == FS_PARENT)
#define ISCHILD(fs)     ((fs)->vir_flag != FS_PARENT)

#define VIRINFO_EXTERNAL    0
#define VIRINFO_BUILDIN     1

typedef struct FAT_VIRTUAL_PARTITION {
    virpartinfo virtualinfo;
    BOOL        isparamset;
} FAT_VIR_PART;

extern FAT_VIR_PART FatVirPart;

FRESULT follow_virentry (FFOBJID *obj, const TCHAR *path);
FATFS*  f_getfatfs (int vol);
FRESULT f_unregvirfs (FATFS *fs);
FRESULT f_checkname (const TCHAR *path);
FRESULT f_boundary (FATFS *fs, DWORD clust);
FRESULT f_disvirfs (FATFS *fs);
FRESULT f_unregvirfs (FATFS *fs);
FRESULT f_regvirfs (FATFS *fs);
FRESULT f_scanfat (FATFS *fs);
FRESULT f_checkvirpart (FATFS *fs, const TCHAR *path, INT vol);
FRESULT f_makevirpart (FATFS *fs, const TCHAR *path, INT vol);
FRESULT f_getvirfree (const TCHAR *path, DWORD *nclst, DWORD *cclst);

#endif
#endif /* _VIRPARTFF_H */
