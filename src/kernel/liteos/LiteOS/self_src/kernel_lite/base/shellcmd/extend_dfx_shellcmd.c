/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: XBUG
 * Author: Huawei LiteOS Team
 * Create: 2021-11-08
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

#ifdef LOSCFG_SHELL
#include "shcmd.h"
#include "shell.h"
#endif
#include "los_task_pri.h"
#include "los_memory_pri.h"

#ifdef LOSCFG_TRIGGER_ENABLE
#include "arch/trigger.h"
#endif

UINT32 OsDbgTskHeapInfoPrint(UINT32 argc, const CHAR **argv)
{
    UINT32 taskId = OS_ALL_TASK_MASK;
    UINTPTR mem_pool = (UINTPTR)OS_SYS_MEM_ADDR; // heap_start

    if (argc == 1) {
        taskId = strtoul(argv[0], NULL, 0);
    } else if (argc == 2) { // 2
        taskId = strtoul(argv[0], NULL, 0);
        mem_pool = strtoul(argv[1], NULL, 0);
    }

    PRINTK("========== pool info ==========\n");
    OsMemInfoPrint((void *)mem_pool);
    if (taskId == OS_ALL_TASK_MASK) {
        PRINTK("========== [all task heap] pool start: 0x%x ==========\n", mem_pool);
        for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
            LOS_MemTaskHeapInfoGet((void *)mem_pool, idx);
        }
    } else {
        PRINTK("========== [task_id: %d heap] pool start: 0x%x ==========\n", taskId, mem_pool);
        LOS_MemTaskHeapInfoGet((void *)mem_pool, taskId);
    }
    return LOS_OK;
}

#ifdef LOSCFG_TRIGGER_ENABLE
UINT32 OsTriggerProc(INT32 argc, const CHAR *argv[])
{
    if (argc == 1) {
        if (strcmp(argv[0], "unset") == 0) {    // unset trigger
            ArchProtectionUnSetAddrRo(0xffffffff);
        } else {
            goto ERROR;
        }
    } else if (argc == 3) { // set trigger, need 3 parameter
        if (strcmp(argv[0], "set") != 0) {
            goto ERROR;
        }

        // input
        UINT32 addr = (UINT32)strtoul(argv[1], NULL, 0);
        UINT32 size = (UINT32)strtoul(argv[2], NULL, 0);
        if (addr == 0 || size == 0) {
            goto PARA_ERROR;
        }

        ArchProtectionSetAddrRo(addr, size);
        GetTriggerInfo(0xffffffff);
    } else {
        goto ERROR;
    }
    return LOS_OK;
ERROR:
    PRINTK("\nUsage: trigger or trigger [unset] or trigger [set] [addr] [size]\n");
    return LOS_NOK;
PARA_ERROR:
    PRINTK("\nParameter invalid. addr in range [0, 0xffffffff] and size in range [1, 2, 4, 8, 16]\n");
    return LOS_NOK;
}
#endif

#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(task_mem_shellcmd, CMD_TYPE_EX, "task_mem", 0, (CmdCallBackFunc)OsDbgTskHeapInfoPrint);
#ifdef LOSCFG_TRIGGER_ENABLE
SHELLCMD_ENTRY(trigger_shellcmd, CMD_TYPE_EX, "trigger", XARGS, (CmdCallBackFunc)OsTriggerProc);
#endif
#endif /* LOSCFG_SHELL */
