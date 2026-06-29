/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: os platform header.
 * Author: Huawei LiteOS Team
 * Create: 2025-01-24
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
#ifndef ASM_PLATFORM_H
#define ASM_PLATFORM_H

#include "los_typedef.h"
#include "register_config.h"
#include "soc/timer.h"
#include "asm/interrupt_config.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    LOS_EMG_LEVEL = 0,
    LOS_COMMOM_LEVEL,
    LOS_ERR_LEVEL,
    LOS_WARN_LEVEL,
    LOS_INFO_LEVEL,
    LOS_DEBUG_LEVEL,
} LOS_LEVEL;

#define PRINT_LEVEL LOS_ERR_LEVEL

#define OS_SYS_CLOCK    (32000000ULL)

#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE        32
#endif

#define FF_FS_LOCK              32

#ifdef LOSCFG_PLATFORM_OSAPPINIT
extern VOID app_init(VOID);
#endif

#ifdef BUFSIZ
#undef BUFSIZ
#define BUFSIZ 1024
#endif

extern UINT32 oal_get_sleep_ticks(VOID);
extern VOID oal_ticks_restore(UINT32 ticks);
extern VOID *OsGetMainTask(VOID);
extern VOID OsSetMainTask(VOID);
extern VOID BoardConfig(VOID);

#ifdef __cplusplus
}
#endif

#endif /* ASM_PLATFORM_H */
