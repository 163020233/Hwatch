/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: LiteOS Shell Message Implementation File
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

#include "shmsg.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "securec.h"
#include "los_task_pri.h"
#include "los_event.h"
#include "los_list.h"
#include "los_printf.h"
#include "los_memory.h"
#include "shell_pri.h"
#include "los_mux.h"

#ifdef LOSCFG_FS_VFS
#include "console_pri.h"
#endif

#ifdef LOSCFG_BOX
#include "los_queue_pri.h"
#include "los_perm.h"
#include "los_shell.h"
#endif

#define SHELL_CMD_PARSE_EVENT   0x111
#define SHELL_CMD_EXIT_EVENT    0x112
#define SHELL_CMD_EVENT_MASK    0xFFF

#ifdef LOSCFG_SHELL_TASK_STACK_SIZE
#define SHELL_TASK_STACK_SIZE   LOSCFG_SHELL_TASK_STACK_SIZE
#else
#define SHELL_TASK_STACK_SIZE   0x3000
#endif

#ifdef LOSCFG_SHELL_ENTRY_STACK_SIZE
#define SHELL_ENTRY_STACK_SIZE  LOSCFG_SHELL_ENTRY_STACK_SIZE
#else
#define SHELL_ENTRY_STACK_SIZE  0x1000
#endif

#define SHELL_TASK_PRIORITY     9
#define SHELL_ENTRY_PRIORITY    9

#if defined(LOSCFG_OBSOLETE_API)
#define SHELL_TASK_ENTRY_DEFINE(funcName)                                                        \
LITE_OS_SEC_TEXT_MINOR VOID *(funcName)(UINTPTR param, UINTPTR rsv1 __attribute__((__unused__)),   \
    UINTPTR rsv2  __attribute__((__unused__)), UINTPTR rsv3  __attribute__((__unused__)))
#else
#define SHELL_TASK_ENTRY_DEFINE(funcName)  LITE_OS_SEC_TEXT_MINOR VOID *(funcName)(VOID *param)
#endif

CHAR *ShellGetInputBuf(ShellCB *shellCB)
{
    CmdKeyLink *cmdKey = shellCB->cmdKeyLink;
    CmdKeyLink *cmdNode = NULL;

    KEY_LOCK(shellCB);
    if ((cmdKey == NULL) || LOS_ListEmpty(&cmdKey->list)) {
        KEY_UNLOCK(shellCB);
        return NULL;
    }

    cmdNode = LOS_DL_LIST_ENTRY(cmdKey->list.pstNext, CmdKeyLink, list);
    LOS_ListDelete(&(cmdNode->list));
    KEY_UNLOCK(shellCB);

    return cmdNode->cmdString;
}

STATIC VOID ShellSaveHistoryCmd(CHAR *string, ShellCB *shellCB)
{
    CmdKeyLink *cmdHistory = shellCB->cmdHistoryKeyLink;
    CmdKeyLink *cmdKey = LOS_DL_LIST_ENTRY(string, CmdKeyLink, cmdString);
    CmdKeyLink *cmdNxt = NULL;

    if ((string == NULL) || (strlen(string) == 0)) {
        return;
    }

    HISTORY_LOCK(shellCB);

    if (cmdHistory->count != 0) {
        cmdNxt = LOS_DL_LIST_ENTRY(cmdHistory->list.pstPrev, CmdKeyLink, list);
        if (strcmp(string, cmdNxt->cmdString) == 0) {
            (VOID)LOS_MemFree(m_aucSysMem0, (VOID *)cmdKey);
            goto END;
        }
    }

    if (cmdHistory->count == CMD_HISTORY_LEN) {
        cmdNxt = LOS_DL_LIST_ENTRY(cmdHistory->list.pstNext, CmdKeyLink, list);
        LOS_ListDelete(&cmdNxt->list);
        LOS_ListTailInsert(&cmdHistory->list, &cmdKey->list);
        (VOID)LOS_MemFree(m_aucSysMem0, (VOID *)cmdNxt);
        goto END;
    }

    LOS_ListTailInsert(&cmdHistory->list, &cmdKey->list);
    cmdHistory->count++;

END:
    HISTORY_UNLOCK(shellCB);
    return;
}

STATIC VOID ShellNotify(ShellCB *shellCB)
{
#ifdef LOSCFG_BOX
    (VOID)LOS_EventWriteWithSId(shellCB->shellEvent, SHELL_CMD_PARSE_EVENT);
#else
    (VOID)LOS_EventWrite(&shellCB->shellEvent, SHELL_CMD_PARSE_EVENT);
#endif
}

enum {
    STAT_NOMAL_KEY,
    STAT_ESC_KEY,
    STAT_MULTI_KEY
};

enum {
    CMD_KEY_ESC_VALUE   = 0x1b,
    CMD_KEY_UP_VALUE    = 0x41,
    CMD_KEY_DOWN_VALUE  = 0x42,
    CMD_KEY_RIGHT_VALUE = 0x43,
    CMD_KEY_LEFT_VALUE  = 0x44,
    CMD_KEY_COMBINATION_VALUE = 0x5b
};

STATIC INT32 ShellCmdLineCheckUDRL(const CHAR ch, ShellCB *shellCB)
{
    INT32 ret = LOS_NOK;

    switch (ch) {
        case CMD_KEY_ESC_VALUE:
            shellCB->shellKeyType = STAT_ESC_KEY;
            ret = LOS_OK;
            break;
        case CMD_KEY_COMBINATION_VALUE:
            if (shellCB->shellKeyType == STAT_ESC_KEY) {
                shellCB->shellKeyType = STAT_MULTI_KEY;
                ret = LOS_OK;
            }
            break;
        case CMD_KEY_UP_VALUE:
            if (shellCB->shellKeyType == STAT_MULTI_KEY) {
                OsShellHistoryShow(CMD_KEY_UP, shellCB);
                shellCB->shellKeyType = STAT_NOMAL_KEY;
                ret = LOS_OK;
            }
            break;
        case CMD_KEY_DOWN_VALUE:
            if (shellCB->shellKeyType == STAT_MULTI_KEY) {
                shellCB->shellKeyType = STAT_NOMAL_KEY;
                OsShellHistoryShow(CMD_KEY_DOWN, shellCB);
                ret = LOS_OK;
            }
            break;
        case CMD_KEY_RIGHT_VALUE:
            /* fall-through */
        case CMD_KEY_LEFT_VALUE:
            if (shellCB->shellKeyType == STAT_MULTI_KEY) {
                shellCB->shellKeyType = STAT_NOMAL_KEY;
                ret = LOS_OK;
            }
            break;
        default:
            break;
    }
    return ret;
}

#ifdef LOSCFG_BOX
STATIC VOID ShellBoxNotify(ShellCB *shellCB)
{
    KEY_LOCK(shellCB);
    OsShellCmdPush(shellCB->shellBuf, shellCB->cmdKeyLink);
    KEY_UNLOCK(shellCB);
    ShellNotify(shellCB);
}

STATIC VOID ShellCmdDispatch(ShellCB *shellCB)
{
    UINT32 id;
    CHAR   queName[SHELL_QUEUE_NAME_LEN];

    if (g_curShellBoxId == LOS_SHELL_BOX_ID) {
        ShellBoxNotify(shellCB);
    } else {
        if (snprintf_s(queName, SHELL_QUEUE_NAME_LEN, SHELL_QUEUE_NAME_LEN - 1,
            "%s%u", SHELL_QUEUE_NAME_STR, g_curShellBoxId) < 0) {
            return;
        }
        if (LOS_SIdGetByName(queName, LOS_SID_TYPE_QUE, &id) == LOS_OK) {
            (VOID)LOS_QueueWriteCopy(id, (VOID *)shellCB->shellBuf, strlen(shellCB->shellBuf) + 1, LOS_NO_WAIT);
        } else {
            /* If exception occurs in user box, box id resets to shell box. */
            (VOID)OsShellBoxIdSet(LOS_SHELL_BOX_ID);
            ShellBoxNotify(shellCB);
        }
    }
}

#else
STATIC VOID ShellCmdDispatch(ShellCB *shellCB)
{
    KEY_LOCK(shellCB);
    OsShellCmdPush(shellCB->shellBuf, shellCB->cmdKeyLink);
    KEY_UNLOCK(shellCB);
    ShellNotify(shellCB);
}
#endif

STATIC VOID ShellCmdTab(ShellCB *shellCB, pf_OUTPUT outputFunc)
{
    INT32 ret;

#ifdef LOSCFG_BOX
    if (g_curShellBoxId != LOS_SHELL_BOX_ID) {
        return;
    }
#endif

    if ((shellCB->shellBufOffset > 0) && (shellCB->shellBufOffset < (SHOW_MAX_LEN - 1))) {
        ret = OsTabCompletion(shellCB->shellBuf, &shellCB->shellBufOffset);
        if (ret > 1) {
            outputFunc("Huawei LiteOS # %s", shellCB->shellBuf);
        }
    }
}

LITE_OS_SEC_TEXT_MINOR VOID ShellCmdLineParse(CHAR c, pf_OUTPUT outputFunc, ShellCB *shellCB)
{
    const CHAR ch = c;
    INT32 ret;

    if ((shellCB->shellBufOffset == 0) && (ch != '\n') && (ch != '\0')) {
        (VOID)memset_s(shellCB->shellBuf, SHOW_MAX_LEN, 0, SHOW_MAX_LEN);
    }

    if (ch == '\r') {
        if (shellCB->shellBufOffset < (SHOW_MAX_LEN - 1)) {
            shellCB->shellBuf[shellCB->shellBufOffset] = '\0';
        }
        shellCB->shellBufOffset = 0;
        ShellCmdDispatch(shellCB);
        return;
    } else if ((ch == '\b') || (ch == 0x7F)) { /* backspace or delete(0x7F) */
        if ((shellCB->shellBufOffset > 0) && (shellCB->shellBufOffset < (SHOW_MAX_LEN - 1))) {
            shellCB->shellBuf[shellCB->shellBufOffset - 1] = '\0';
            shellCB->shellBufOffset--;
            outputFunc("\b \b");
        }
        return;
    } else if (ch == 0x09) { /* 0x09: tab */
        ShellCmdTab(shellCB, outputFunc);
        return;
    }
    /* parse the up/down/right/left key */
    ret = ShellCmdLineCheckUDRL(ch, shellCB);
    if (ret == LOS_OK) {
        return;
    }

    if ((ch != '\n') && (ch != '\0')) {
        if (shellCB->shellBufOffset < (SHOW_MAX_LEN - 1)) {
            shellCB->shellBuf[shellCB->shellBufOffset] = ch;
        } else {
            shellCB->shellBuf[SHOW_MAX_LEN - 1] = '\0';
        }
        shellCB->shellBufOffset++;
        outputFunc("%c", ch);
    }

    shellCB->shellKeyType = STAT_NOMAL_KEY;
}

STATIC LITE_OS_SEC_TEXT_MINOR UINT32 ShellMsgTypeGet(CmdParsed *cmdParsed, const CHAR *cmdType)
{
    UINT32 minLen;
    CmdItemNode *curCmdItem = (CmdItemNode *)NULL;
    UINT32 len = (UINT32)strlen(cmdType);
    CmdModInfo *cmdInfo = OsCmdInfoGet();
    if (cmdInfo == NULL) {
        return OS_INVALID;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(curCmdItem, &cmdInfo->cmdList.list, CmdItemNode, list) {
        if ((len == strlen(curCmdItem->cmd->cmdKey)) &&
            (strncmp((CHAR *)(curCmdItem->cmd->cmdKey), cmdType, len) == 0)) {
            minLen = (len < CMD_KEY_LEN) ? len : (CMD_KEY_LEN - 1);
            if (strncpy_s((CHAR *)(cmdParsed->cmdKeyword), CMD_KEY_LEN, cmdType, minLen) != EOK) {
                PRINTK("strncpy_s failure in %s\n", __FUNCTION__);
            }
            cmdParsed->cmdType = curCmdItem->cmd->cmdType;
            return LOS_OK;
        }
    }

    return OS_INVALID;
}

STATIC UINT32 ShellMsgNameGetAndExec(CmdParsed *cmdParsed, const CHAR *output, UINT32 len)
{
    UINT32 loop;
    UINT32 ret;
    const CHAR *tmpStr = NULL;
    BOOL quotes = FALSE;
    CHAR *msgName = (CHAR *)LOS_MemAlloc(m_aucSysMem0, len + 1);
    if (msgName == NULL) {
        PRINTK("malloc failure in %s[%d]\n", __FUNCTION__, __LINE__);
        return OS_INVALID;
    }
    /* Scan the 'output' string for command */
    /* Notice: Command string must not have any special name */
    for (tmpStr = output, loop = 0; (*tmpStr != '\0') && (loop < len);) {
        /* If reach a double quotes, switch the quotes matching status */
        if (*tmpStr == '\"') {
            SWITCH_QUOTES_STATUS(quotes);
            /* Ignore the double quote character itself */
            tmpStr++;
            continue;
        }
        /* If detected a space which the quotes matching status is false */
        /* which said has detected the first space for separator, finish this scan operation */
        if ((*tmpStr == ' ') && (QUOTES_STATUS_CLOSE(quotes))) {
            break;
        }
        msgName[loop] = *tmpStr++;
        loop++;
    }
    msgName[loop] = '\0';
    /* Scan the command list to check whether the command can be found */
    ret = ShellMsgTypeGet(cmdParsed, msgName);
    PRINTK("\n");
    if (ret != LOS_OK) {
        PRINTK("%s:command not found", msgName);
    } else {
        (VOID)OsCmdExec(cmdParsed, (CHAR *)output);
    }
    (VOID)LOS_MemFree(m_aucSysMem0, msgName);
    return ret;
}

LITE_OS_SEC_TEXT_MINOR UINT32 ShellMsgParse(const CHAR *msg)
{
    CHAR *output = NULL;
    UINT32 len;
    CmdParsed cmdParsed;
    UINT32 ret = OS_INVALID;

    if (msg == NULL) {
        goto END;
    }

    len = (UINT32)strlen(msg);
    output = (CHAR *)LOS_MemAlloc(m_aucSysMem0, len + 1);
    if (output == NULL) {
        PRINTK("malloc failure in %s[%d]\n", __FUNCTION__, __LINE__);
        goto END;
    }
    /* Call function 'OsCmdKeyShift' to squeeze and clear useless or overmuch space if string buffer */
    ret = OsCmdKeyShift(msg, output, len + 1);
    if ((ret != LOS_OK) || (strlen(output) == 0)) {
        ret = OS_INVALID;
        goto END_FREE_OUTPUT;
    }

    (VOID)memset_s(&cmdParsed, sizeof(CmdParsed), 0, sizeof(CmdParsed));

    ret = ShellMsgNameGetAndExec(&cmdParsed, output, len);

END_FREE_OUTPUT:
    (VOID)LOS_MemFree(m_aucSysMem0, output);
END:
    return ret;
}

SHELL_TASK_ENTRY_DEFINE(ShellEntry)
{
    ShellCB *shellCB = (ShellCB *)param;

    (VOID)memset_s(shellCB->shellBuf, SHOW_MAX_LEN, 0, SHOW_MAX_LEN);
    return ShellStdinLoop(shellCB);
}

STATIC VOID ShellCmdProcess(ShellCB *shellCB)
{
    CHAR *buf = NULL;
    while (1) {
        buf = ShellGetInputBuf(shellCB);
        if (buf == NULL) {
            break;
        }
        (VOID)ShellMsgParse(buf);
        ShellSaveHistoryCmd(buf, shellCB);
        shellCB->cmdMaskKeyLink = shellCB->cmdHistoryKeyLink;
    }
}

STATIC VOID ShellTaskIpcDeinit(ShellCB *shellCB);

STATIC VOID ShellExitProcess(ShellCB *shellCB)
{
    OsShellKeyDeInit(shellCB);
    ShellTaskIpcDeinit(shellCB);
    (VOID)LOS_MuxDelete(shellCB->historyMutex);
    (VOID)LOS_MuxDelete(shellCB->keyMutex);
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB);
#ifdef LOSCFG_BOX
    (VOID)OsShellBoxShCbSet(NULL);
#endif
}

#ifdef LOSCFG_BOX
SHELL_TASK_ENTRY_DEFINE(ShellTask)
{
    UINT32 ret;
    ShellCB *shellCB = (ShellCB *)param;

    while (1) {
        if (g_curShellBoxId == (UINT32)LOS_SHELL_BOX_ID) {
            PRINTK("\nHuawei LiteOS # ");
        } else {
            PRINTK("\nbox_%u@Huawei LiteOS # ", g_curShellBoxId);
        }
        ret = LOS_EventReadWithSId(shellCB->shellEvent, SHELL_CMD_EVENT_MASK,
                                   LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        if (ret == SHELL_CMD_PARSE_EVENT) {
            ShellCmdProcess(shellCB);
        } else if (ret == SHELL_CMD_EXIT_EVENT) {
            break;
        }
    }
    ShellExitProcess(shellCB);
    return 0;
}

SHELL_TASK_ENTRY_DEFINE(ShellUserTask)
{
    UINT32 ret;
    UINT32 size = SHOW_MAX_LEN;
    ShellCB *shellCB = (ShellCB *)param;

    while (1) {
        ret = LOS_QueueReadCopy(shellCB->shellQueue, shellCB->shellBuf, &size, LOS_WAIT_FOREVER);
        if (ret != LOS_OK) {
            continue;
        }

        KEY_LOCK(shellCB);
        OsShellCmdPush(shellCB->shellBuf, shellCB->cmdKeyLink);
        KEY_UNLOCK(shellCB);

        ShellCmdProcess(shellCB);
        size = SHOW_MAX_LEN;

        if (g_curShellBoxId == (UINT32)LOS_SHELL_BOX_ID) {
            PRINTK("\nHuawei LiteOS # ");
        } else {
            PRINTK("\nbox_%u@Huawei LiteOS # ", g_curShellBoxId);
        }
    }
    return 0;
}

#else
SHELL_TASK_ENTRY_DEFINE(ShellTask)
{
    UINT32 ret;
    ShellCB *shellCB = (ShellCB *)param;

    while (1) {
        PRINTK("\nHuawei LiteOS # ");
        ret = LOS_EventRead(&shellCB->shellEvent, SHELL_CMD_EVENT_MASK,
                            LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        if (ret == SHELL_CMD_PARSE_EVENT) {
            ShellCmdProcess(shellCB);
        } else if (ret == SHELL_CMD_EXIT_EVENT) {
            break;
        }
    }
    ShellExitProcess(shellCB);
    return 0;
}
#endif

#define SERIAL_SHELL_TASK_NAME "SerialShellTask"
#define SERIAL_ENTRY_TASK_NAME "SerialEntryTask"
#define TELNET_SHELL_TASK_NAME "TelnetShellTask"
#define TELNET_ENTRY_TASK_NAME "TelnetEntryTask"

#ifdef LOSCFG_BOX
#define SHELL_USER_TASK_NAME   "ShellUserTask"

#define LOS_SHL_NODE_LIMIT      8
#define LOS_SHL_MSG_LEN_MAX     SHOW_MAX_LEN
#define LOS_SHL_QUEUE_BUFF_SIZE \
    (UINT16)OS_QUEUE_MSG_SIZE(LOS_SHL_NODE_LIMIT, LOS_SHL_MSG_LEN_MAX)

STATIC UINT32 ShellTaskIpcInit(ShellCB *shellCB)
{
    UINT32 ret;
    CHAR   queName[SHELL_QUEUE_NAME_LEN];

    if (shellCB->boxId == LOS_SHELL_BOX_ID) {
        return LOS_EventCreate(&shellCB->shellEvent);
    }

    shellCB->queueMem = LOS_MemAlloc((VOID *)m_aucSysMem0, LOS_SHL_QUEUE_BUFF_SIZE);
    if (shellCB->queueMem == NULL) {
        return LOS_NOK;
    }

    ret = LOS_QueueCreateStatic(NULL, LOS_SHL_NODE_LIMIT, &shellCB->shellQueue, 0,
                                LOS_SHL_MSG_LEN_MAX, shellCB->queueMem, LOS_SHL_QUEUE_BUFF_SIZE);
    if (ret != LOS_OK) {
        goto EXIT_MEM_FREE;
    }

#ifdef LOSCFG_BASE_IPC_PERM
    ret = LOS_SIdGrant(shellCB->shellQueue, LOS_SHELL_BOX_ID);
    if (ret != LOS_OK) {
        goto EXIT_QUE_DEL;
    }
#endif
    if (snprintf_s(queName, SHELL_QUEUE_NAME_LEN, SHELL_QUEUE_NAME_LEN - 1,
        "%s%u", SHELL_QUEUE_NAME_STR, shellCB->boxId) < 0) {
        goto EXIT_SID_DGT;
    }
    ret = LOS_SIdRegName(shellCB->shellQueue, queName);
    if (ret != LOS_OK) {
        goto EXIT_SID_DGT;
    }
    return LOS_OK;

EXIT_SID_DGT:
#ifdef LOSCFG_BASE_IPC_PERM
    (VOID)LOS_SIdDeGrant(shellCB->shellQueue, LOS_SHELL_BOX_ID);
EXIT_QUE_DEL:
#endif
    (VOID)LOS_QueueDelete(shellCB->shellQueue);
EXIT_MEM_FREE:
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->queueMem);
    return ret;
}

STATIC VOID ShellTaskIpcDeinit(ShellCB *shellCB)
{
    if (shellCB->boxId == LOS_SHELL_BOX_ID) {
        (VOID)LOS_EventDelete(shellCB->shellEvent);
    } else {
#ifdef LOSCFG_BASE_IPC_PERM
        (VOID)LOS_SIdDeGrant(shellCB->shellQueue, LOS_SHELL_BOX_ID);
#endif
        (VOID)LOS_QueueDelete(shellCB->shellQueue);
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->queueMem);
    }
}

STATIC UINT32 ShellTaskInfoGet(ShellCB *shellCB, CHAR **name, TSK_ENTRY_FUNC *entry)
{
    if (shellCB->boxId == LOS_SHELL_BOX_ID) {
        *name = SERIAL_SHELL_TASK_NAME;
        *entry = (TSK_ENTRY_FUNC)ShellTask;
    } else {
        if (snprintf_s(shellCB->shellTaskName, SHELL_TASK_NAME_LEN, SHELL_TASK_NAME_LEN - 1,
            "%s%u", SHELL_USER_TASK_NAME, shellCB->boxId) < 0) {
            return LOS_NOK;
        }
        *name = shellCB->shellTaskName;
        *entry = (TSK_ENTRY_FUNC)ShellUserTask;
    }

    return LOS_OK;
}

#else
STATIC UINT32 ShellTaskIpcInit(ShellCB *shellCB)
{
    return LOS_EventInit(&shellCB->shellEvent);
}

STATIC VOID ShellTaskIpcDeinit(ShellCB *shellCB)
{
    (VOID)LOS_EventDestroy(&shellCB->shellEvent);
}

STATIC UINT32 ShellTaskInfoGet(ShellCB *shellCB, CHAR **name, TSK_ENTRY_FUNC *entry)
{
#ifdef LOSCFG_SHELL_CONSOLE
    if (shellCB->consoleID == CONSOLE_SERIAL) {
        *name = SERIAL_SHELL_TASK_NAME;
    } else if (shellCB->consoleID == CONSOLE_TELNET) {
        *name = TELNET_SHELL_TASK_NAME;
    } else {
        return LOS_NOK;
    }
#else
    (VOID)shellCB;
    /* in non-console mode, shell only support one input */
    *name = SERIAL_SHELL_TASK_NAME;
#endif
    *entry = ShellTask;
    return LOS_OK;
}
#endif

LITE_OS_SEC_TEXT_MINOR UINT32 ShellTaskInit(ShellCB *shellCB)
{
    CHAR             *name = NULL;
    TSK_ENTRY_FUNC   taskEntry = NULL;
    TSK_INIT_PARAM_S initParam = {0};

    if (shellCB == NULL) {
        return LOS_NOK;
    }

    if (ShellTaskInfoGet(shellCB, &name, &taskEntry) != LOS_OK) {
        return LOS_NOK;
    }

    initParam.pfnTaskEntry = taskEntry;
    initParam.usTaskPrio   = SHELL_TASK_PRIORITY;
    initParam.uwStackSize  = SHELL_TASK_STACK_SIZE;
    initParam.pcName       = name;
    initParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    LOS_TASK_PARAM_INIT_ARG(initParam, shellCB);

    if (ShellTaskIpcInit(shellCB) != LOS_OK) {
        return LOS_NOK;
    }

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    return LOS_TaskCreate(&shellCB->shellTaskHandle, &initParam);
#else
    shellCB->shellTaskStack = LOS_MemAllocAlign((VOID *)m_aucSysMem0, SHELL_TASK_STACK_SIZE,
        LOSCFG_STACK_POINT_ALIGN_SIZE);
    if (shellCB->shellTaskStack == NULL) {
        ShellTaskIpcDeinit(shellCB);
        return LOS_NOK;
    }
    UINT32 ret = LOS_TaskCreateStatic(&shellCB->shellTaskHandle, &initParam, shellCB->shellTaskStack);
    if (ret != LOS_OK) {
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->shellTaskStack);
        ShellTaskIpcDeinit(shellCB);
    }
    return ret;
#endif
}

LITE_OS_SEC_TEXT_MINOR VOID ShellTaskDeinit(ShellCB *shellCB)
{
    (VOID)LOS_TaskDelete(shellCB->shellEntryHandle);
#ifndef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->shellEntryStack);
    (VOID)LOS_TaskDelete(shellCB->shellTaskHandle);
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->shellTaskStack);
    ShellExitProcess(shellCB);
#else
    (VOID)LOS_EventWrite(&shellCB->shellEvent, SHELL_CMD_EXIT_EVENT);
#endif
}

#ifdef LOSCFG_EXC_INTERACTION
BOOL IsShellTask(UINT32 taskId)
{
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);

    if ((taskCB->taskEntry == (TSK_ENTRY_FUNC)ShellTask) ||
        (taskCB->taskEntry == (TSK_ENTRY_FUNC)ShellEntry)) {
        return TRUE;
    }
    return FALSE;
}
#endif

LITE_OS_SEC_TEXT_MINOR UINT32 ShellEntryInit(ShellCB *shellCB)
{
    UINT32 ret;
    CHAR *name = NULL;
    TSK_INIT_PARAM_S initParam = {0};

    if (shellCB == NULL) {
        return LOS_NOK;
    }

#ifdef LOSCFG_BOX
    if (shellCB->boxId != LOS_SHELL_BOX_ID) {
        return LOS_OK;
    }
#endif

#ifdef LOSCFG_SHELL_CONSOLE
    if (shellCB->consoleID == CONSOLE_SERIAL) {
        name = SERIAL_ENTRY_TASK_NAME;
    } else if (shellCB->consoleID == CONSOLE_TELNET) {
        name = TELNET_ENTRY_TASK_NAME;
    } else {
        return LOS_NOK;
    }
#else
    /* in non-console mode, shell only support one input */
    name = SERIAL_ENTRY_TASK_NAME;
#endif

    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ShellEntry;
    initParam.usTaskPrio   = SHELL_ENTRY_PRIORITY;
    initParam.uwStackSize  = SHELL_ENTRY_STACK_SIZE;
    initParam.pcName       = name;
    initParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    LOS_TASK_PARAM_INIT_ARG(initParam, shellCB);

#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    ret = LOS_TaskCreate(&shellCB->shellEntryHandle, &initParam);
#else
    shellCB->shellEntryStack = LOS_MemAllocAlign((VOID *)m_aucSysMem0, SHELL_ENTRY_STACK_SIZE,
        LOSCFG_STACK_POINT_ALIGN_SIZE);
    if (shellCB->shellEntryStack == NULL) {
        return LOS_NOK;
    }
    ret = LOS_TaskCreateStatic(&shellCB->shellEntryHandle, &initParam, shellCB->shellEntryStack);
    if (ret != LOS_OK) {
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, shellCB->shellEntryStack);
    }
#endif

#ifdef LOSCFG_SHELL_CONSOLE
    ret = ConsoleTaskReg((INT32)shellCB->consoleID, shellCB->shellEntryHandle);
#endif

    return ret;
}
