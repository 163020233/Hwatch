/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description : LiteOS shell commond adapt product AT commond.
 * Author : Huawei LiteOS Team
 * Create : 2025-9-11
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

#ifndef _LOS_AT_H
#define _LOS_AT_H

#include "los_printf.h"
#include "los_task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern INT32 OsShellCmdSystemInfo(INT32 argc, const CHAR **argv);
extern UINT32 OsDbgTskHeapInfoPrint(UINT32 argc, const CHAR **argv);
extern UINT32 OsShellCmdDumpTask(INT32 argc, const CHAR **argv);

#ifdef LOSCFG_DEBUG_SWTMR
extern UINT32 OsShellCmdSwtmrInfoGet(INT32 argc, const UINT8 **argv);
#endif

#ifdef LOSCFG_DEBUG_SEMAPHORE
extern UINT32 OsShellCmdSemInfoGet(UINT32 argc, const CHAR **argv);
#endif

#ifdef LOSCFG_DEBUG_MUTEX
extern UINT32 OsShellCmdMuxInfoGet(UINT32 argc, const CHAR **argv);
#endif
#ifdef LOSCFG_DEBUG_QUEUE
extern UINT32 OsShellCmdQueueInfoGet(UINT32 argc, const CHAR **argv);
#endif

#ifdef LOSCFG_DEBUG_MEM_MWMD
extern UINT32 SystemMw(INT32 argc, const CHAR **argv);
extern UINT32 SystemMd(INT32 argc, const CHAR **argv);
#endif

#ifdef LOSCFG_MEM_LEAKCHECK
extern UINT32 OsShellCmdMemUsed(INT32 argc, const CHAR *argv[]);
#endif
extern UINT32 OsShellCmdFree(INT32 argc, const CHAR *argv[]);
extern UINT32 OsShellCmdMemCheck(INT32 argc, const CHAR *argv[]);
#if (defined LOSCFG_ARCH_INTERRUPT_TAKEOVER) && (defined LOSCFG_DEBUG_HWI)
extern UINT32 OsShellCmdHwi(INT32 argc, const CHAR **argv);
#endif
#ifdef LOSCFG_TRIGGER_ENABLE
extern UINT32 OsTriggerProc(INT32 argc, const CHAR *argv[]);
#endif
#ifdef LOSCFG_KERNEL_CPUP
extern UINT32 OsShellCmdCpup(INT32 argc, const CHAR **argv);
#endif

#ifdef LOSCFG_KERNEL_PERF
extern VOID OsPerfHwPmuStart(VOID);
extern VOID LOS_PerfStop(VOID);
extern VOID OsPerfDataDump(VOID);
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
extern VOID OsSchedStatStart(VOID);
extern VOID OsSchedStatStop(VOID);
extern VOID OsSchedStatInfo(VOID);
#endif

#ifdef LOSCFG_MEMFRAG_DFX_SHOW_INFO
extern VOID LOS_MemFragInfo(VOID);
#endif
/**
 * @ingroup liteos dfx
 * @brief Execute user command function.
 *
 * @par Description:
 * This function takes a command string as input, parses it, and executes the corresponding command.
 * @attention
 * <ul>
 * <li>The input command string must be a valid command supported by the system.</li>
 * <li>The function will return an error code if the command is invalid or cannot be executed.</li>
 * </ul>
 * @param input [IN] Type #CHAR*. It is the command string to be executed.
 *
 * @retval #LOS_OK (0) Success.
 * @retval #LOS_NOK (non-zero) Failure.
 */
INT32 OSExecUserCmd(const CHAR *input);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_AT_H */