/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description : LiteOS adapt rtthread interrupt.
 * Author : Huawei LiteOS Team
 * Create : 2025-8-28
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
#ifndef _LOS_CFSQUEUE_PRI_H_
#define _LOS_CFSQUEUE_PRI_H_

#ifdef LOSCFG_WCFS_SCHEDULER

#include "los_task_pri.h"

#define MIDDLE_WEIGHT       2501
#ifdef LOSCFG_WCFS_MIN_RUNTIME
#define CFS_MINTIME_EXEC LOSCFG_WCFS_MIN_RUNTIME
#else
#define CFS_MINTIME_EXEC    10
#endif

#define OS_CFS_PRIO_IS_VALID(prio)         (0 <= prio && prio < OS_PRIORITY_QUEUE_NUM + 4)

typedef struct {
    LOS_DL_LIST         queues;
    SPIN_LOCK_S         lock;                           /* private spinlock */
} CfsQueue;

/* CFS Starvation prevention */
// 宏含义为CFS任务运行时间的百分占比  取值[0~100]  默认值为0   非常！不建议将宏值设定到5%以上
#ifdef LOSCFG_CFS_TIME_RATIO
#define CFS_TASK_TIME_RATIO LOSCFG_CFS_TIME_RATIO
#else
#define CFS_TASK_TIME_RATIO 0
#endif

#define TASK_TICKS_BASE 100 // 100个tick为基数
#define TASK_TICKS_RT_HOLD 0xFFFFFFFF // RT全部占用cpu场景

typedef enum {
    PRIORITY_RT,
    PRIORITY_CFS,
    PREVIOUS_RT,
    PREVIOUS_CFS
} CpuTaskType;

extern CpuTaskType g_priorityTaskType;
/* CFS Starvation prevention */

extern LITE_OS_SEC_BSS CfsQueue g_cfsQueue;

extern VOID OsCfsQueueEnqueue(LOS_DL_LIST *cfsqueueItem);

extern LITE_OS_SEC_TEXT_MINOR LosTaskCB *OsGetCfsTask(VOID);

extern UINT64 OsGetCfsWeight(UINT64 deltaExec, UINT16 prio);

extern VOID OsUpgradeVruntime(VOID);

extern bool OsCfsNeedSchedule(VOID);

extern VOID LOS_PrtCfsTaskInfo(VOID);

#endif
#endif