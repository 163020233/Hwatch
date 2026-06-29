/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Expand Api
 * Author: Huawei LiteOS Team
 * Create: 2025-03-01
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

#include "los_task.h"
#include "los_exc_info.h"

#if defined(LOSCFG_BASE_CORE_TSK_MONITOR) && defined(CONFIG_OH_MINI)
STATIC UINT32 g_newTaskId = 0;
STATIC TSKSWITCHHOOK g_pfnTraceTskSwitchHook = NULL;
TaskSwitchInfo g_taskSwitchInfo = {0};

STATIC VOID ShbExcTskSwitch(LosTaskCB * newTaskCB)
{
    if (newTaskCB == NULL) {
        return;
    }
    g_taskSwitchInfo.pid[g_taskSwitchInfo.idx] = (UINT16)newTaskCB->taskId;

    errno_t ret = memcpy_s(g_taskSwitchInfo.name[g_taskSwitchInfo.idx], LOS_TASK_NAMELEN,
                           newTaskCB->taskName, LOS_TASK_NAMELEN);
    if (ret != EOK) {
        PRINT_ERR("exc task switch copy file name failed!\n");
    }
    g_taskSwitchInfo.name[g_taskSwitchInfo.idx][LOS_TASK_NAMELEN - 1] = '\0';

    if (++g_taskSwitchInfo.idx == OS_TASK_SWITCH_INFO_COUNT) {
        g_taskSwitchInfo.idx = 0;
        g_taskSwitchInfo.isFull = TRUE;
    }
}

UINT8 ShbTaskSwitchInfoIsFull(VOID)
{
    return g_taskSwitchInfo.isFull;
}

UINT8 ShbTaskSwitchInfoIdxGet(VOID)
{
    return g_taskSwitchInfo.idx;
}

UINT32 ShbTaskSwitchInfoGet(UINT32 index, TaskIdAndName *taskSwitchInfo)
{
    UINT32 intSave;
    UINT32 curIndex;

    if (taskSwitchInfo == NULL) {
        return LOS_ERRNO_TSK_PTR_NULL;
    }

    curIndex = index;
    if (curIndex >= OS_TASK_SWITCH_INFO_COUNT) {
        curIndex %= OS_TASK_SWITCH_INFO_COUNT;
    }

    intSave = LOS_IntLock();

    taskSwitchInfo->taskId = (UINT32)g_taskSwitchInfo.pid[curIndex];

    if (memcpy_s(taskSwitchInfo->taskName, LOS_TASK_NAMELEN,
                 g_taskSwitchInfo.name[curIndex], LOS_TASK_NAMELEN) != EOK) {
        PRINT_ERR("ShbTaskSwitchInfoGet copy task name failed\n");
    }

    LOS_IntRestore(intSave);
    return LOS_OK;
}

VOID ShbTraceTskSwitchHookReg(TSKSWITCHHOOK hook)
{
    g_pfnTraceTskSwitchHook = hook;
}

VOID ShbTskSwitchHook(UINT32 oldTaskId, UINT32 newTaskId)
{
    g_newTaskId = newTaskId;
    ShbExcTskSwitch(OS_TCB_FROM_TID(newTaskId));
    if (g_pfnTraceTskSwitchHook != NULL) {
        g_pfnTraceTskSwitchHook(oldTaskId, newTaskId);
    }
}

UINT32 LOS_NextTaskIDGet(VOID)
{
    return g_newTaskId;
}
#endif