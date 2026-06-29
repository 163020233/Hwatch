/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Fatfs Config HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-8-20
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

#include <stddef.h>

#define FFCONF_DEF 5380

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY 0
#define FF_FS_MINIMIZE 0

#define FF_USE_STRFUNC 0

#define FF_USE_FIND    0

#define FF_USE_MKFS    1

#define FF_USE_FASTSEEK 0

#define FF_USE_EXPAND   0

#define FF_USE_CHMOD    0

#define FF_USE_LABEL    0

#define FF_USE_FORWARD  0
#define FF_CODE_PAGE    932

#define FF_USE_LFN      1
#define FF_MAX_LFN      255

#define FF_LFN_UNICODE  0

#define FF_LFN_BUF      255
#define FF_SFN_BUF      12

#define FF_STRF_ENCODE  3

#define FF_FS_RPATH     0

#define FF_VOLUMES      1

#define FF_STR_VOLUME_ID    0
#define FF_VOLUME_STRS      "RAM","NAND","CF","SD","SD2","USB","USB2","USB3"

#define FF_MULTI_PARTITION 0

#define FF_MIN_SS          512
#define FF_MAX_SS          4096

#define FF_LBA64        0

#define FF_MIN_GPT      0x10000000

#define FF_USE_TRIM     0

#define FF_FS_TINY      0

#define FF_FS_EXFAT     0

#define FF_FS_NORTC     1
#define FF_NORTC_MON    1
#define FF_NORTC_MDAY   1
#define FF_NORTC_YEAR   2020

#define FF_FS_NOFSINFO  1

#define FF_FS_LOCK      0

#define FF_FS_REENTRANT 0
#define FF_FS_TIMEOUT   1000
#define FF_SYNC_t       HANDLE

#endif /* _FATFS_CONFIG_H */
