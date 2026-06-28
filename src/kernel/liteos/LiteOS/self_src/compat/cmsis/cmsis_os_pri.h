/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: CMSIS Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-06-28
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
#ifndef _CMSIS_OS_PRI_H
#define _CMSIS_OS_PRI_H

#ifdef __LITEOS__
#include "wrap/wrap_free.h"

#if !defined(LOSCFG_LIB_CONFIGURABLE) && (LOSCFG_BASE_CORE_TICK_PER_SECOND == OS_SYS_MS_PER_SECOND)
STATIC_INLINE UINT32 Ms2Tick(UINT32 ms)
{
    return ms;
}
#else
STATIC_INLINE UINT32 Ms2Tick(UINT32 ms)
{
    return LOS_MS2Tick(ms);
}
#endif
#define OS_KERNEL_ACTIVE   (g_taskScheduled != 0)
#define OS_BUSY_DELAY(millisec)     LOS_Mdelay(millisec)
#else /* RTOS_Lite */
#include "os_buildef.h"
#ifdef OS_OPTION_COMPAT_CMSIS
#include "wrap/wrap_rtos.h"
#endif
STATIC_INLINE UINT32 Ms2Tick(UINT32 ms)
{
#if defined(OS_OPTION_TICK_DISABLE_USR_CONFIG) && (OS_KERNEL_TICK_PER_SECOND == OS_SYS_MS_PER_SECOND)
    return ms;
#else
    return LOS_MS2Tick(ms);
#endif
}
#define OS_KERNEL_ACTIVE   ((OS_FLG_BGD_ACTIVE & g_uniFlag) != 0)
#define OS_BUSY_DELAY(millisec)
#endif

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMSIS_LOG_ERROR
#define CMSIS_LOG_ERROR(fmt, ...)   PRINT_DEBUG(fmt, ##__VA_ARGS__)
#endif

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
extern osKernelState_t g_kernelState;
extern osMutexId_t OsMutexNewSub(UINTPTR creator);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CMSIS_OS_PRI_H */
