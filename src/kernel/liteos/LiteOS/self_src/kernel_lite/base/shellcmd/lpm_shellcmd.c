/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Lowpower Shell Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-11-12
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
#include "shcmd.h"
#include "shell.h"
#include "los_lowpower.h"
#include "los_hwi_pri.h"
#include "los_swtmr_pri.h"
#include "los_task_pri.h"

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
#include "los_lowpower_debug_pri.h"
#endif

STATIC VOID OsPrintPowerMgrInfo(VOID)
{
    PRINTK("lowpower status: %s\n", LOS_GetLowPowerStatus() ? "enable" : "disable");
}

STATIC VOID OsPrintLowpowerHelpInfo(VOID)
{
    PRINTK("Usage:\n"
           "enable,               enable lowpower\n"
           "disable,              disable lowpower\n"
           "status,               print lowpower status\n"
           "sysinfo,              print information about hwi, swtmr and task\n"
           "timestamp cpuid       print recent sleep information about cpuid\n"
           "blockinfo cpuid       print deep block info about cpuid\n"
           "resetblockinfo cpuid  reset deep block info about cpuid\n");
}

STATIC VOID OsPrintSysInfo(VOID)
{
#ifdef LOSCFG_DEBUG_HWI
    PrintHwiInfo();
#endif

#ifdef LOSCFG_DEBUG_SWTMR
    OsDbgSwtmrInfoGet(OS_ALL_SWTMR_MASK);
#endif

    OsDbgTskInfoGet(OS_ALL_TASK_MASK);
}

#define LOWPOWER_CMD_LEN (sizeof(g_lowpowerSubCmd) / sizeof(LowpowerCmd))
typedef struct {
    const CHAR *subCmd;
    VOID (*subCmdExcuteFunc)(VOID);
} LowpowerCmd;

STATIC LowpowerCmd g_lowpowerSubCmd[] = {
    {"enable", LOS_EnableLowPower},
    {"disable", LOS_DisableLowPower},
    {"status", OsPrintPowerMgrInfo},
    {"sysinfo", OsPrintSysInfo},
    {"--help", OsPrintLowpowerHelpInfo},
};

LITE_OS_SEC_TEXT_MINOR UINT32 OsShellCmdlowpower(INT32 argc, const CHAR **argv)
{
    UINT32 cpuId;
    CHAR *endPtr = NULL;
    UINT32 i;
    /* 2 is input count */
    if ((argc < 1) || (argc > 2)) {
        PRINTK("\nUsage error\n"
            "Try 'lpm --help' for more information\n");
        return OS_ERROR;
    }
    for (i = 0; i < LOWPOWER_CMD_LEN; i++) {
        if (strcmp(g_lowpowerSubCmd[i].subCmd, argv[0]) == 0) {
            g_lowpowerSubCmd[i].subCmdExcuteFunc();
            return 0;
        }
    }
    /* 2 is input count */
    if ((strcmp("timestamp", argv[0]) == 0) && (argc == 2)) {
        cpuId = strtoul(argv[1], &endPtr, 0);
        if ((*endPtr != 0) || (cpuId >= LOSCFG_KERNEL_CORE_NUM)) {
            PRINTK("\ncpu ID can't access %s.\n", argv[1]);
            return OS_ERROR;
        }
        OsPowerMgrInfoTitle();
        OsPowerMgrInfoDbg(cpuId);
        return 0;
    }
    /* 2 is input count */
    if ((strcmp("blockinfo", argv[0]) == 0) && (argc == 2)) {
        cpuId = strtoul(argv[1], &endPtr, 0);
            if ((*endPtr != 0) || (cpuId >= LOSCFG_KERNEL_CORE_NUM)) {
                PRINTK("\ncpu ID can't access %s.\n", argv[1]);
                return OS_ERROR;
            }
        OsSleepBlockInfoPrint(cpuId);
        return 0;
    }
    /* 2 is input count */
    if ((strcmp("resetblockinfo", argv[0]) == 0) && (argc == 2)) {
        cpuId = strtoul(argv[1], &endPtr, 0);
            if ((*endPtr != 0) || (cpuId >= LOSCFG_KERNEL_CORE_NUM)) {
                PRINTK("\ncpu ID can't access %s.\n", argv[1]);
                return OS_ERROR;
            }
        OsResetSleepBlockCB(cpuId);
        return 0;
    }
    PRINTK("lpm: invalid option %s\n"
           "Try 'lpm --help' for more information.\n",
           argv[0]);
    return OS_ERROR;
}

SHELLCMD_ENTRY(lpm_shellcmd, CMD_TYPE_EX, "lpm", 1, (CmdCallBackFunc)OsShellCmdlowpower);
