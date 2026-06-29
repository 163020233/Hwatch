/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 * Create : 2024-04-14
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
 *---------------------------------------------------------------------------*/

#ifndef _LOS_EXC_INFO_H
#define _LOS_EXC_INFO_H

#include "los_config.h"
#include "los_memory.h"
#include "los_exc.h"
#include "arch/exception.h"
#include "osal_lite.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define INFO_TYPE_AND_SIZE      8

#define OS_TASK_SWITCH_INFO_COUNT   0xA

#define EXC_MSG_ARRAY_INIT_VALUE    0xff

#define MAX_SCENE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(ExcInfo) + sizeof(EXC_CONTEXT_S))
#define MAX_TSK_INFO_SIZE       (INFO_TYPE_AND_SIZE + sizeof(ExcTaskInfo) * LOSCFG_BASE_CORE_TSK_LIMIT)

#ifdef LOSCFG_BASE_IPC_QUEUE
#define MAX_QUEUE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(QUEUE_INFO_S) * LOSCFG_BASE_IPC_QUEUE_LIMIT)
#else
#define MAX_QUEUE_INFO_SIZE     (0)
#endif

#define MAX_SWITCH_INFO_SIZE    (INFO_TYPE_AND_SIZE + (sizeof(UINT32) + sizeof(CHAR) * LOS_TASK_NAMELEN) \
                                 * OS_TASK_SWITCH_INFO_COUNT)

typedef struct {
    CHAR                acName[LOS_TASK_NAMELEN];   /**< Task entrance function         */
    uint32_t            uwTaskID;                   /**< Task ID                        */
    uint16_t            usTaskStatus;               /**< Task status                    */
    uint16_t            usTaskPrio;                 /**< Task priority                  */
    VOID                *pTaskSem;                  /**< Semaphore pointer              */
    VOID                *pTaskMux;                  /**< Mutex pointer                  */
    uint32_t            uwSemID;                    /**< Sem ID                         */
    uint32_t            uwMuxID;                    /**< Mux ID                         */
    EVENT_CB_S          uwEvent;                    /**< Event                          */
    uint32_t            uwEventMask;                /**< Event mask                     */
    uint32_t            uwStackSize;                /**< Task stack size                */
    uint32_t            uwTopOfStack;               /**< Task stack top                 */
    uint32_t            uwBottomOfStack;            /**< Task stack bottom              */
    uint32_t            uwSP;                       /**< Task SP pointer                */
    uint32_t            uwCurrUsed;                 /**< Current task stack usage       */
    uint32_t            uwPeakUsed;                 /**< Task stack usage peak          */
    BOOL                bOvf;                       /**< Flag that indicates whether a task stack overflow occurs */
} ExcTaskInfo;

typedef struct {
    UINT32 taskId;
    CHAR   taskName[LOS_TASK_NAMELEN];
} TaskIdAndName;

typedef struct {
    UINT8 idx;
    UINT8 isFull;
    UINT16 pid[OS_TASK_SWITCH_INFO_COUNT];
    CHAR   name[OS_TASK_SWITCH_INFO_COUNT][LOS_TASK_NAMELEN];
} TaskSwitchInfo;

enum MemMangType {
    MEM_MANG_MEMBOX,
    MEM_MANG_MEMORY,
    MEM_MANG_EMPTY
};

typedef struct {
    UINT32 type;
    UINT32 startAddr;
    UINT32 size;
    VOID *blkAddrArray;
} MemInfo;

typedef struct {
    enum MemMangType type;
    UINT32 startAddr;
    UINT32 size;
    UINT32 free;
    UINT32 blockSize;
    UINT32 errorAddr;
    UINT32 errorLen;
    UINT32 errorOwner;
} MemInfoCB;

#define OS_SYS_MEM_NUM                                      20

#define MAX_EXC_MEM_SIZE        (INFO_TYPE_AND_SIZE + MAX_SCENE_INFO_SIZE + MAX_TSK_INFO_SIZE + MAX_QUEUE_INFO_SIZE \
                                 + MAX_SWITCH_INFO_SIZE)

typedef struct TagExcContext {
#ifdef LOSCFG_ARCH_FPU_ENABLE
    UINT32 S16;
    UINT32 S17;
    UINT32 S18;
    UINT32 S19;
    UINT32 S20;
    UINT32 S21;
    UINT32 S22;
    UINT32 S23;
    UINT32 S24;
    UINT32 S25;
    UINT32 S26;
    UINT32 S27;
    UINT32 S28;
    UINT32 S29;
    UINT32 S30;
    UINT32 S31;
#endif
    UINT32 uwR4;
    UINT32 uwR5;
    UINT32 uwR6;
    UINT32 uwR7;
    UINT32 uwR8;
    UINT32 uwR9;
    UINT32 uwR10;
    UINT32 uwR11;
    UINT32 uwPriMask;
    /* auto save */
    UINT32 uwSP;
    UINT32 uwR0;
    UINT32 uwR1;
    UINT32 uwR2;
    UINT32 uwR3;
    UINT32 uwR12;
    UINT32 uwLR;
    UINT32 uwPC;
    UINT32 uwxPSR;
#ifdef LOSCFG_ARCH_FPU_ENABLE
    UINT32 S0;
    UINT32 S1;
    UINT32 S2;
    UINT32 S3;
    UINT32 S4;
    UINT32 S5;
    UINT32 S6;
    UINT32 S7;
    UINT32 S8;
    UINT32 S9;
    UINT32 S10;
    UINT32 S11;
    UINT32 S12;
    UINT32 S13;
    UINT32 S14;
    UINT32 S15;
    UINT32 FPSCR;
    UINT32 NO_NAME;
#endif
}EXC_CONTEXT_S;

typedef struct {
    ExcInfoType     flag;
    UINT32          length;
    ExcInfo         info;
    EXC_CONTEXT_S   context;
} ExcContextInfoArray;

typedef struct {
    ExcInfoType flag;
    UINT32      length;
    ExcTaskInfo  taskInfo[LOSCFG_BASE_CORE_TSK_LIMIT];
} ExcTaskInfoArray;

typedef struct {
    ExcInfoType  flag;
    UINT32       length;
    QUEUE_INFO_S queueInfo[LOSCFG_BASE_IPC_QUEUE_LIMIT];
} ExcQueueInfoArray;

typedef struct {
    UINT32              totalLen;
    ExcContextInfoArray excInfo;
    ExcTaskInfoArray    taskInfo;
    ExcQueueInfoArray   queueInfo;
} ExcMsgArray;

typedef UINT32 (*EXC_INFO_SAVE_CALLBACK)(UINT32, VOID *);
extern VOID OsExcMsgDump(VOID);
extern UINT8 g_excMsgArray[MAX_EXC_MEM_SIZE];
extern UINT32 ShbTaskSwitchInfoGet(UINT32 index, TaskIdAndName *taskSwitchInfo);
extern UINT8 ShbTaskSwitchInfoIsFull(VOID);
extern UINT8 ShbTaskSwitchInfoIdxGet(VOID);
extern ExcInfo g_excInfo;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif