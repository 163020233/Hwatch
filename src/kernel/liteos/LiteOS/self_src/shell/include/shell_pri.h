/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: LiteOS Shell Implementation Private Headfile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#ifndef _HWLITEOS_SHELL_PRI_H
#define _HWLITEOS_SHELL_PRI_H

#include "los_config.h"
#include "shcmd.h"
#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define KEY_LOCK(shellCB) do {                                                          \
    UINT32 ret_;                                                                        \
    ret_ = LOS_MuxPend((shellCB)->keyMutex, LOS_WAIT_FOREVER);                          \
    if (ret_ != LOS_OK) {                                                               \
        PRINT_ERR("%s %d, mutex lock failed : 0x%x\n", __FUNCTION__, __LINE__, ret_);   \
    }                                                                                   \
} while (0)

#define KEY_UNLOCK(shellCB) do {                                                        \
    UINT32 ret_;                                                                        \
    ret_ = LOS_MuxPost((shellCB)->keyMutex);                                            \
    if (ret_ != LOS_OK) {                                                               \
        PRINT_ERR("%s %d, mutex unlock failed : 0x%x\n", __FUNCTION__, __LINE__, ret_); \
    }                                                                                   \
} while (0)

#define HISTORY_LOCK(shellCB) do {                                                      \
    UINT32 ret_;                                                                        \
    ret_ = LOS_MuxPend((shellCB)->historyMutex, LOS_WAIT_FOREVER);                      \
    if (ret_ != LOS_OK) {                                                               \
        PRINT_ERR("%s %d, mutex lock failed : 0x%x\n", __FUNCTION__, __LINE__, ret_);   \
    }                                                                                   \
} while (0)

#define HISTORY_UNLOCK(shellCB) do {                                                    \
    UINT32 ret_;                                                                        \
    ret_ = LOS_MuxPost((shellCB)->historyMutex);                                        \
    if (ret_ != LOS_OK) {                                                               \
        PRINT_ERR("%s %d, mutex unlock failed : 0x%x\n", __FUNCTION__, __LINE__, ret_); \
    }                                                                                   \
} while (0)

#ifdef LOSCFG_SHELL_USER_TEXT
#undef LITE_OS_SEC_TEXT_MINOR
#define LITE_OS_SEC_TEXT_MINOR
#endif

#ifdef LOSCFG_SHELL_LK
extern VOID OsLkLoggerInit(const CHAR *str);
#endif

extern CmdItem g_shellcmdBegin;
extern CmdItem g_shellcmdEnd;

#if defined(LOSCFG_OBSOLETE_API)
extern VOID *ShellEntry(UINTPTR param, UINTPTR rsv1, UINTPTR rsv2, UINTPTR rsv3);
extern VOID *ShellTask(UINTPTR param, UINTPTR rsv1, UINTPTR rsv2, UINTPTR rsv3);
#else
extern VOID *ShellTask(VOID *param);
extern VOID *ShellEntry(VOID *param);
#endif

extern VOID *ShellStdinLoop(ShellCB *shellCB);
#ifdef LOSCFG_EXC_INTERACTION
BOOL IsShellTask(UINT32 taskId);
#endif

#ifdef LOSCFG_BOX
#define SHELL_QUEUE_NAME_STR    "ShellTaskQue"
#define SHELL_QUEUE_NAME_LEN     16

/* Every box's shell has its local memory pool. */
#ifdef m_aucSysMem0
#undef m_aucSysMem0
#endif
#define m_aucSysMem0 ((VOID *)g_boxLocalPool[OS_MEM_POOL_INDEX])

extern UINT32 g_curShellBoxId;
extern UINT8 g_shboxStack[SH_BOX_STACK_SIZE];
extern const CHAR * const g_shboxTaskName;

extern VOID OsShBoxEntry(void);
extern VOID OsShellCmdBoxInfo(VOID);
extern UINT32 OsShellBoxPermCheck(VOID);
extern UINT32 OsShellBoxIdSet(UINT16 boxId);
extern UINT32 OsShellBoxShCbSet(ShellCB *shellCB);
extern ShellCB *OsShellBoxShCbGet(VOID);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HWLITEOS_SHELL_PRI_H */
