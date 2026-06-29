/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _LOS_EXC_INFO_H
#define _LOS_EXC_INFO_H

#include "los_config.h"
#include "los_memory.h"
#include "los_exc.h"
#include "arch/exception.h"
#include "osal_lite.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define INFO_TYPE_AND_SIZE      8

#define OS_TASK_SWITCH_INFO_COUNT   0xA

#define EXC_MSG_ARRAY_INIT_VALUE    0xff

#define EXC_QUEUE_TASK_INFO   0x20

#define MAX_SCENE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(ExcInfo) + sizeof(EXC_CONTEXT_S))
#define MAX_TSK_INFO_SIZE       (INFO_TYPE_AND_SIZE + sizeof(ExcTaskInfo) * LOSCFG_BASE_CORE_TSK_LIMIT)

#ifdef LOSCFG_BASE_IPC_QUEUE
#define MAX_QUEUE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(EXC_QUEUE_INFO_S) * LOSCFG_BASE_IPC_QUEUE_LIMIT)
#else
#define MAX_QUEUE_INFO_SIZE     (0)
#endif

#define MAX_SWITCH_INFO_SIZE    (INFO_TYPE_AND_SIZE + (sizeof(UINT32) + sizeof(CHAR) * LOS_TASK_NAMELEN) \
                                 * OS_TASK_SWITCH_INFO_COUNT)

typedef struct {
    CHAR                acName[LOS_TASK_NAMELEN];   /* Task entrance function */
    uint32_t            uwTaskID;                   /* Task ID */
    uint16_t            usTaskStatus;               /* Task status */
    uint16_t            usTaskPrio;                 /* Task priority */
    VOID                *pTaskSem;                  /* Semaphore pointer */
    VOID                *pTaskMux;                  /* Mutex pointer */
    uint32_t            uwSemID;                    /* Sem ID */
    uint32_t            uwMuxID;                    /* Mux ID */
    EVENT_CB_S          uwEvent;                    /* Event */
    uint32_t            uwEventMask;                /* Event mask */
    uint32_t            uwStackSize;                /* Task stack size */
    uint32_t            uwTopOfStack;               /* Task stack top */
    uint32_t            uwBottomOfStack;            /* Task stack bottom */
    uint32_t            uwSP;                       /* Task SP pointer */
    uint32_t            uwCurrUsed;                 /* Current task stack usage */
    uint32_t            uwPeakUsed;                 /* Task stack usage peak */
    BOOL                bOvf;                       /* Flag that indicates whether a task stack overflow occurs */
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

typedef ExcContext EXC_CONTEXT_S;

#define CORE_TSK_MAX_LIMIT                       64
#define OS_WAIT_TSK_ARRAY_LEN                    GET_TASKNUM_MASK_U32(LOSCFG_BASE_CORE_TSK_LIMIT)
typedef struct tagExcQueueInfo {
    UINT32 queueID;                               /* Queue ID */
    UINT16 queueLen;                              /* Queue length */
    UINT16 queueSize;                             /* Node size */
    UINT16 queueHead;                             /* Node head */
    UINT16 queueTail;                             /* Node tail */
    UINT16 writableCnt;                           /* Count of writable resources */
    UINT16 readableCnt;                           /* Count of readable resources */
    UINT32 waitReadTask[OS_WAIT_TSK_ARRAY_LEN];   /* Resource reading task */
    UINT32 waitWriteTask[OS_WAIT_TSK_ARRAY_LEN];  /* Resource writing task */
    UINT32 waitMemTask[OS_WAIT_TSK_ARRAY_LEN];    /* Memory task */
} EXC_QUEUE_INFO_S;

typedef struct {
    ExcInfoType  flag;
    UINT32       length;
    EXC_QUEUE_INFO_S queueInfo[LOSCFG_BASE_IPC_QUEUE_LIMIT];
} ExcQueueInfoArray;

typedef struct {
    ExcInfoType flag;
    UINT32      length;
    ExcTaskInfo taskInfo[LOSCFG_BASE_CORE_TSK_LIMIT];
} ExcTaskInfoArray;

typedef struct {
    ExcInfoType     flag;
    UINT32          length;
    ExcInfo         info;
    EXC_CONTEXT_S   context;
} ExcContextInfoArray;

typedef struct {
    UINT32              totalLen;
    ExcContextInfoArray excInfo;
    ExcTaskInfoArray    taskInfo;
    ExcQueueInfoArray   queueInfo;
} ExcMsgArray;

extern VOID OsExcMsgDump(VOID);
typedef UINT32 (*EXC_INFO_SAVE_CALLBACK)(UINT32, VOID *);
extern UINT8 g_excMsgArray[MAX_EXC_MEM_SIZE];
extern UINT32 ShbTaskSwitchInfoGet(UINT32 index, TaskIdAndName *taskSwitchInfo);
extern UINT8 ShbTaskSwitchInfoIsFull(VOID);
extern UINT8 ShbTaskSwitchInfoIdxGet(VOID);
extern ExcInfo g_excInfo;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif