/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Fatfs Config HeadFile
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

#ifndef _FATFS_CONFIG_H
#define _FATFS_CONFIG_H

#include "dirent.h"
#include "time.h"

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#include "fs/fs.h"
#endif

#include "los_config.h"

/* FatFs standard config */
#define FFCONF_DEF  5380

#define FF_FS_READONLY  0
#define FF_FS_MINIMIZE  0
#define FF_USE_STRFUNC  0
#define FF_USE_FIND     0
#define FF_USE_MKFS     1
#define FF_USE_FASTSEEK 0
#define FF_USE_EXPAND   1
#define FF_USE_CHMOD    1
#define FF_USE_LABEL    1
#define FF_USE_FORWARD  0
#ifdef LOSCFG_FS_FAT_CHINESE
#define FF_CODE_PAGE    936
#else
#define FF_CODE_PAGE    437
#endif
#ifdef LOSCFG_FS_FAT_STACK_LFN
#define FF_USE_LFN      2
#elif LOSCFG_FS_FAT_HEAP_LFN
#define FF_USE_LFN      3
#else
#define FF_USE_LFN      2
#endif
#define FF_MAX_LFN      255
#ifdef LOSCFG_FS_FAT_CHINESE_GB2312
#define FF_LFN_UNICODE  0
#elif LOSCFG_FS_FAT_CHINESE_UTF_8
#define FF_LFN_UNICODE  2
#else
#define FF_LFN_UNICODE  0
#endif
#define FF_LFN_BUF      255
#define FF_SFN_BUF      12
#define FF_STRF_ENCODE  3
#define FF_FS_RPATH     0
#define FF_VOLUMES      LOSCFG_FS_FAT_VOLUMES
#define FF_MIN_SS       512
#define FF_MAX_SS       4096
#define FF_LBA64        0
#define FF_MIN_GPT      0x10000000
#define FF_FS_TINY      0
#define FF_FS_NORTC     0
#define FF_NORTC_MON    1
#define FF_NORTC_MDAY   1
#define FF_NORTC_YEAR   2020
#define FF_FS_NOFSINFO  0
#ifndef FF_FS_LOCK
#ifdef LOSCFG_XTS_SUPPORT
#define FF_FS_LOCK      99
#else
#define FF_FS_LOCK      10
#endif /* LOSCFG_XTS_SUPPORT */
#endif /* FF_FS_LOCK */
#define FF_FS_REENTRANT 1
#define FF_FS_TIMEOUT   6000
#define FF_SYNC_t       UINT32

#define FF_STR_VOLUME_ID    0
#ifndef FF_VOLUME_STRS
#define FF_VOLUME_STRS      "RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
#endif
/* FatFs extend config */
#define FF_DIR_DEFINED  1
#define FF_FSIN_CHECK   1
#define FF_FAST_MKFS    1
#define FF_USE_FSCHECK  1
#define FF_DBR_VERIFY   1
#define FF_AUSIZE_ALIGN 1
#define FF_PRINT_MKFS   1
#define FF_USE_DYNAMIC_MEM 1
#define FF_MULTI_PARTITION_EXTEND   1
#define FF_USE_STANDARD_TRUNCATE    1
#define FF_USE_STANDARD_FALLOCATE   1

#if FF_MULTI_PARTITION_EXTEND
#define FF_MULTI_PARTITION   1
#else
#define FF_MULTI_PARTITION   0
#endif

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#define FF_USE_VIRTUAL_PARTITION    1
#else
#define FF_USE_VIRTUAL_PARTITION    0
#endif

#ifdef LOSCFG_FS_FAT_DCACHE
#define FF_USE_DENTRY 1
#else
#define FF_USE_DENTRY 0
#endif

#ifdef LOSCFG_FS_FAT_TRIM
#define FF_USE_TRIM   1
#else
#define FF_USE_TRIM   0
#endif

#ifdef LOSCFG_FS_FAT_UNI_TRIM
#define FF_USE_UNI_TRIM 1
#else
#define FF_USE_UNI_TRIM 0
#endif

#endif /* _FATFS_CONFIG_H */
