/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Fatfs Extend Interface HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-8-20
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
 
#ifndef _FATFS_EXTEND_H
#define _FATFS_EXTEND_H

#include "ff.h"
#include "dirent.h"
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#include "virpartff.h"
#endif
#include "fatfs_dentry.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* System Time Flag */
#define SYSTEM_TIME_ENABLE  0x01
#define SYSTEM_TIME_DISABLE 0x00

extern UINT gTimeStatus;
extern time_t Fat2Time(DWORD time);
extern VOID *fat_memalloc(UINT32 size);
extern VOID *fat_memzalloc(UINT32 size);
extern VOID *fat_memalloc_align(UINT32 size, UINT32 align);
extern VOID fat_memfree(VOID *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FATFS_EXTEND_H */ 
