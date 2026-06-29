/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Exception internal HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2020-06-24
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

#ifndef _LOS_HOOK_PRI_H
#define _LOS_HOOK_PRI_H

#include "los_typedef.h"

typedef enum {
    // system view hook
    LOS_HOOK_SYSVIEW_TSK_CREATE = 0,
    LOS_HOOK_SYSVIEW_MV_TSK_TO_READY,
    LOS_HOOK_SYSVIEW_READ_TSK_TO_READY,
    LOS_HOOK_SYSVIEW_MV_TSK_TO_DELAY,
    LOS_HOOK_SYSVIEW_MV_TSK_TO_OVERFLOW_DELAY,
    LOS_HOOK_SYSVIEW_MV_TSK_TO_SUSPEND,
    LOS_HOOK_SYSVIEW_ISR_EXIT_TO_SCHED,
    LOS_HOOK_SYSVIEW_ISR_EXIT,
    LOS_HOOK_SYSVIEW_ISR_ENTER,
    LOS_HOOK_SWTMR_DEBUG,
    LOS_HOOK_INT_CONSUME_TIME_ENTER,
    LOS_HOOK_INT_CONSUME_TIME_EXIT,
    LOS_HOOK_EXC_USER,
    LOS_HOOK_HWI_BOTTOM_HALF,
#ifdef LOSCFG_BACKTRACE_SP
    LOS_HOOK_BACKTRACE_SP,
#endif
    LOS_HOOK_TEST,
    LOS_HOOK_MAX
} LOS_HOOK_TYPE;

#define LOS_HOOK_PARAM0     0
#define LOS_HOOK_PARAM1     1
#define LOS_HOOK_PARAM2     2
#define LOS_HOOK_PARAM3     3
#define LOS_HOOK_PARAM4     4
#define LOS_HOOK_PARAM5     5

typedef void (*LOS_HookFunc)(uintptr_t args[], int argc);
INT32 LOS_HookExe(LOS_HOOK_TYPE hookId, int argc, ...);

#endif