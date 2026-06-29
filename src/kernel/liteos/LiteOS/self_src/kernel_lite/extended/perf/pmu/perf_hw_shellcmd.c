/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: ShellCmd Perf
 * Author: Huawei LiteOS Team
 * Create: 2025-08-20
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
 */

#include "los_task_pri.h"
#include "los_perf.h"
#ifdef LOSCFG_SHELL
#include "shcmd.h"
#endif

#define PERF_BUFFER_SIZE 0x1000

STATIC VOID printPerfData(const CHAR *data, UINT32 len)
{
    UINT32 intSave;
    SCHEDULER_LOCK(intSave);
    PRINTK("\n--- PERF DATA START ---\n");
    for (UINT32 i = 0; i < len; i++) {
        PRINTK("%02x", data[i]);
    }
    PRINTK("\n--- PERF DATA END ---\n");
    SCHEDULER_UNLOCK(intSave);
}

VOID OsPerfHwPmuStart(VOID)
{
    UINT32 ret;
    PerfConfigAttr config = {
        .eventsCfg =
            {
                .type = PERF_EVENT_TYPE_HW,
                .events =
                    {
                        {PERF_COUNT_HW_CPU_CYCLES, 0x2FFFF},
                        {PERF_COUNT_HW_DCACHE_REFERENCES, 0xFFFF},
                        {PERF_COUNT_HW_DCACHE_MISSES, 0xFFFF},
                    },
                .eventsNr = 3,
                .predivided = FALSE,
            },
        .sampleType = PERF_RECORD_IP | PERF_RECORD_CALLCHAIN | PERF_RECORD_CPU,
        .needSample = TRUE,
    };

    CHAR *buf = (CHAR *)LOS_MemAlloc(m_aucSysMem1, PERF_BUFFER_SIZE);
    LOS_PerfInit(buf, PERF_BUFFER_SIZE);
    ret = LOS_PerfConfig(&config);
    if (ret != LOS_OK) {
        PRINTK("perf config error!\r\n");
    }
    LOS_PerfStart(0);
    LOS_MemFree(m_aucSysMem1, buf);
}

VOID OsPerfDataDump(VOID)
{
    CHAR *buf = (CHAR *)LOS_MemAlloc(m_aucSysMem1, PERF_BUFFER_SIZE);
    UINT32 realLen = LOS_PerfDataRead(buf, PERF_BUFFER_SIZE);
    printPerfData(buf, realLen);
    LOS_PerfDeinit();
    LOS_MemFree(m_aucSysMem1, buf);
}
#ifdef LOSCFG_SHELL
SHELLCMD_ENTRY(perfstart_shellcmd, CMD_TYPE_EX, "perfstart", 0, (CmdCallBackFunc)OsPerfHwPmuStart);
SHELLCMD_ENTRY(perfstop_shellcmd, CMD_TYPE_EX, "perfstop", 0, (CmdCallBackFunc)LOS_PerfStop);
SHELLCMD_ENTRY(perfdump_shellcmd, CMD_TYPE_EX, "perfdump", 0, (CmdCallBackFunc)OsPerfDataDump);
#endif