/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LiteOS adapt module implementation.
 * Author: Huawei LiteOS Team
 * Create: 2025-01-15
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

#ifdef LOSCFG_DRIVERS_MMC
#include "soc/mmc.h"
#include "host.h"
#endif
#include "los_init.h"
#include "los_printf.h"
#include "los_task.h"
#include "los_task_pri.h"
#include "los_typedef.h"
#ifdef LOSCFG_FS_PROC
#include "proc_fs.h"
#endif
#define DELAY_MS 10000

typedef VOID (*LOWPOWERIDLEHOOK)(VOID);
static LOWPOWERIDLEHOOK g_low_power_hook = NULL;

__attribute__((weak)) VOID app_init(VOID)
{
    PRINTK("%s start ok\n", __FUNCTION__);

    while (1) {
        (VOID)LOS_TaskDelay(DELAY_MS);
        PRINTK("in app_init.\n");
    }
}

#ifndef LOSCFG_KERNEL_LOWPOWER
LITE_OS_SEC_TEXT_MINOR VOID LOS_LowpowerHookReg(LOWPOWERIDLEHOOK hook)
{
    g_low_power_hook = hook;
}
#endif

LITE_OS_SEC_TEXT VOID OsIdleTask(VOID)
{
    while (1) {
        LOS_TaskResRecycle();
        OsIdleHandler();

        if (g_low_power_hook != NULL) {
            g_low_power_hook();
        }
    }
}

#if !(defined(LOSCFG_DEBUG_TASK) || \
    defined(LOSCFG_SHELL) || \
    defined(LOSCFG_SHELL_ADAPT_AT) || \
    !defined(LOSCFG_EXC_SIMPLE_INFO))
typedef struct {
    UINT16 status;
    const CHAR *statusStr;
} TaskStatus;

#ifdef LOSCFG_TASK_JOINABLE
#define TASK_STATUS_TBL_LEN 8
#else
#define TASK_STATUS_TBL_LEN 7
#endif

STATIC TaskStatus g_taskStatusTbl[TASK_STATUS_TBL_LEN] = {
    {OS_TASK_STATUS_RUNNING, "Running"},
    {OS_TASK_STATUS_READY, "Ready"},
    {OS_TASK_STATUS_DELAY, "Delay"},
    {OS_TASK_STATUS_PEND_TIME | OS_TASK_STATUS_SUSPEND, "SuspendTime"},
    {OS_TASK_STATUS_PEND_TIME | OS_TASK_STATUS_PEND, "PendTime"},
    {OS_TASK_STATUS_PEND, "Pend"},
    {OS_TASK_STATUS_SUSPEND, "Suspend"},
#ifdef LOSCFG_TASK_JOINABLE
    {OS_TASK_STATUS_ZOMBIE, "Zombie"},
#endif
};

const CHAR *OsTskStatusConvertStr(UINT16 taskStatus)
{
    UINT32 i;
    for (i = 0; i < TASK_STATUS_TBL_LEN; ++i) {
        if ((taskStatus & g_taskStatusTbl[i].status) == g_taskStatusTbl[i].status) {
            return g_taskStatusTbl[i].statusStr;
        }
    }
    return "Invalid";
}
#endif
