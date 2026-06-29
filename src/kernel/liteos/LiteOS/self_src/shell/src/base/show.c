/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: LiteOS Shell Show Implementation File
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

#include "show.h"
#include "shmsg.h"
#ifdef LOSCFG_SHELL_CONSOLE
#include "console_pri.h"
#endif
#include "los_memory.h"
#include "los_mux.h"
#include "shell_pri.h"

#ifndef LOSCFG_SHELL_WITH_CB
STATIC BOOL g_shellSourceFlag = FALSE;
#ifndef LOSCFG_SHELL_CONSOLE
/* If not using console, then allocated shell struct will store in this global variable */
STATIC ShellCB *g_shellCB = NULL;
#endif

STATIC BOOL *OsShellSourceFlagGet(VOID)
{
    return &g_shellSourceFlag;
}

#else
STATIC BOOL *OsShellSourceFlagGet(VOID)
{
    ShellCB *shCB = OsShellBoxShCbGet();
    if (shCB == NULL) {
        return NULL;
    }
    return &shCB->shellSourceFlag;
}
#endif

STATIC UINT32 OsShellSourceInit(VOID)
{
    UINT32 ret;
    BOOL *flag = OsShellSourceFlagGet();
    if (flag == NULL) {
        return LOS_NOK;
    }

    if (*flag == TRUE) {
        return LOS_OK;
    }

    ret = OsCmdInit();
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsShellSysCmdRegister();
    if (ret != LOS_OK) {
        OsCmdDeInit();
        return ret;
    }

    *flag = TRUE;
    return LOS_OK;
}

STATIC VOID OsShellSourceDeInit(VOID)
{
    BOOL *flag = OsShellSourceFlagGet();
    if (flag == NULL) {
        return;
    }

    OsCmdDeInit();
    OsShellSysCmdUnregister();
    *flag = FALSE;
}

STATIC UINT32 OsShellCreateTask(ShellCB *shellCB)
{
    UINT32 ret = ShellTaskInit(shellCB);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = ShellEntryInit(shellCB);
    if (ret != LOS_OK) {
        (VOID)LOS_TaskDelete(shellCB->shellTaskHandle);
    }
    return ret;
}

#ifdef LOSCFG_SHELL_CONSOLE
STATIC INLINE UINT32 OsShellConsoleInit(INT32 consoleId, ShellCB *shellCB)
{
    CONSOLE_CB *consoleCB = OsGetConsoleByID(consoleId);
    if ((consoleCB == NULL) || (consoleCB->shellHandle != NULL)) {
        return LOS_NOK;
    }

    consoleCB->shellHandle = shellCB;
    shellCB->consoleID = (UINT32)consoleId;

    return LOS_OK;
}

STATIC INLINE VOID OsShellConsoleDeinit(INT32 consoleId)
{
    CONSOLE_CB *consoleCB = OsGetConsoleByID(consoleId);
    /* OsShellConsoleInit has checked the validation of consoleCB */
    consoleCB->shellHandle = NULL;
}
#else /* !LOSCFG_SHELL_CONSOLE */
STATIC INLINE UINT32 OsShellConsoleInit(INT32 consoleId, ShellCB *shellCB)
{
    (VOID)consoleId;
#ifndef LOSCFG_SHELL_WITH_CB
    g_shellCB = shellCB;
#else
    (VOID)shellCB;
#endif
    return LOS_OK;
}

STATIC INLINE VOID OsShellConsoleDeinit(INT32 consoleId)
{
    (VOID)consoleId;
#ifndef LOSCFG_SHELL_WITH_CB
    g_shellCB = NULL;
#endif
}
#endif /* LOSCFG_SHELL_CONSOLE */

#ifndef LOSCFG_SHELL_WITH_CB
STATIC UINT32 OsShellCBInit(ShellCB **shellCB)
{
    UINT32 ret = LOS_NOK;
    ShellCB *cb = NULL;

    if (OsShellSourceInit() != LOS_OK) {
        PRINT_ERR("shell source init error.\n");
        return LOS_NOK;
    }

    cb = LOS_MemAlloc(m_aucSysMem0, sizeof(ShellCB));
    if (cb == NULL) {
        PRINT_ERR("memory alloc error.\n");
        goto ERR_OUT_SOURCE_DEINIT;
    }
    (VOID)memset_s(cb, sizeof(ShellCB), 0, sizeof(ShellCB));

    *shellCB = cb;
    return LOS_OK;

ERR_OUT_SOURCE_DEINIT:
    OsShellSourceDeInit();
    return ret;
}

STATIC VOID OsShellCBDeinit(ShellCB * const *shellCB)
{
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, *shellCB);
    OsShellSourceDeInit();
}
#else
STATIC UINT32 OsShellCBInit(ShellCB **shellCB)
{
    UINT32 ret = LOS_NOK;
    ShellCB *cb = NULL;

    /* Can't create shell again. */
    if (OsShellBoxShCbGet() != NULL) {
        return LOS_NOK;
    }

    cb = LOS_MemAlloc(m_aucSysMem0, sizeof(ShellCB));
    if (cb == NULL) {
        return LOS_NOK;
    }
    (VOID)memset_s(cb, sizeof(ShellCB), 0, sizeof(ShellCB));
    cb->boxId = g_curBoxId;

    ret = OsShellBoxShCbSet(cb);
    if (ret != LOS_OK) {
        PRINT_ERR("shellCB set error.\n");
        goto ERR_OUT_FREE_MEM;
    }

    ret = OsShellSourceInit();
    if (ret != LOS_OK) {
        PRINT_ERR("shell source init error.\n");
        goto ERR_OUT_SET_CB;
    }
    *shellCB = cb;
    return LOS_OK;

ERR_OUT_SET_CB:
    (VOID)OsShellBoxShCbSet(NULL);
ERR_OUT_FREE_MEM:
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, cb);
    return ret;
}

STATIC VOID OsShellCBDeinit(ShellCB * const *shellCB)
{
    OsShellSourceDeInit();
    (VOID)OsShellBoxShCbSet(NULL);
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, *shellCB);
}
#endif

UINT32 OsShellInit(INT32 consoleId)
{
    ShellCB *shellCB = NULL;

    if (OsShellCBInit(&shellCB) != LOS_OK) {
        PRINT_ERR("shellCB init error.\n");
        return LOS_NOK;
    }

    if (OsShellConsoleInit(consoleId, shellCB) != LOS_OK) {
        goto ERR_OUT_SHELLCB_DEINIT;
    }

    if (LOS_MuxCreate(&shellCB->keyMutex) != LOS_OK) {
        PRINT_ERR("key mutex create error.\n");
        goto ERR_OUT_DEINIT_CONSOLE;
    }

    if (LOS_MuxCreate(&shellCB->historyMutex) != LOS_OK) {
        PRINT_ERR("history mutex create error.\n");
        goto ERR_OUT_DELETE_KEY_MUX;
    }

#ifndef LOSCFG_SHELL_WITH_CB
    if (strncpy_s(shellCB->shellWorkingDirectory, PATH_MAX, "/", 1) != EOK) {
        PRINT_ERR("strncpy_s error.\n");
        goto ERR_OUT_DELETE_HISTORY_MUX;
    }
#endif

    if (OsShellKeyInit(shellCB) != LOS_OK) {
        PRINT_ERR("shell key init error.\n");
        goto ERR_OUT_DELETE_HISTORY_MUX;
    }

    if (OsShellCreateTask(shellCB) != LOS_OK) {
        PRINT_ERR("shell task create error.\n");
        goto ERR_OUT_DEINIT_KEY;
    }

    return LOS_OK;

ERR_OUT_DEINIT_KEY:
    OsShellKeyDeInit(shellCB);
ERR_OUT_DELETE_HISTORY_MUX:
    (VOID)LOS_MuxDelete(shellCB->historyMutex);
ERR_OUT_DELETE_KEY_MUX:
    (VOID)LOS_MuxDelete(shellCB->keyMutex);
ERR_OUT_DEINIT_CONSOLE:
    OsShellConsoleDeinit(consoleId);
ERR_OUT_SHELLCB_DEINIT:
    OsShellCBDeinit(&shellCB);
    return LOS_NOK;
}

INT32 OsShellDeinit(INT32 consoleId)
{
    ShellCB *shellCB = NULL;

#ifndef LOSCFG_SHELL_WITH_CB
#ifdef LOSCFG_SHELL_CONSOLE
    CONSOLE_CB *consoleCB = OsGetConsoleByID(consoleId);
    if (consoleCB == NULL) {
        PRINT_ERR("shell deinit error.\n");
        return -1;
    }

    shellCB = (ShellCB *)consoleCB->shellHandle;
    consoleCB->shellHandle = NULL;
#else
    shellCB = g_shellCB;
#endif

#else
    shellCB = OsShellBoxShCbGet();
    if (shellCB == NULL) {
        PRINT_ERR("shell deinit error.\n");
        return -1;
    }

    OsShellSourceDeInit();
#endif
    (VOID)consoleId;
    ShellTaskDeinit(shellCB);

    return 0;
}

#ifdef LOSCFG_FS_VFS
CHAR *OsShellGetWorkingDirectory(VOID)
{
    ShellCB *shellCB = NULL;

#ifdef LOSCFG_SHELL_CONSOLE
    CONSOLE_CB *consoleCB = OsGetConsoleByTaskID(LOS_CurTaskIDGet());
    if (consoleCB == NULL) {
        return NULL;
    }

    shellCB = (ShellCB *)consoleCB->shellHandle;
    if (shellCB == NULL) {
        return NULL;
    }
    return shellCB->shellWorkingDirectory;
#else
    shellCB = g_shellCB;
    return shellCB->shellWorkingDirectory;
#endif
}
#endif
