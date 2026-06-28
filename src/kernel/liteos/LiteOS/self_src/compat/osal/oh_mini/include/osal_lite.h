/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Osal HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _OSAL_LITE_H
#define _OSAL_LITE_H
#include "los_task_pri.h"
#include "los_swtmr_pri.h"
#include "los_sortlink_pri.h"
#include "los_queue_pri.h"
#include "los_mux_pri.h"
#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif
#include "los_memory_internal.h"
#include "los_sem_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef UNUSED
#define UNUSED(a) ((void)(1 || (a)))
#endif

typedef struct {
    LosTaskCB   *runTask;
    LosTaskCB   *newTask;
} LosTask;
extern LosTask              g_losTask;

STATIC INLINE VOID OsSetMutexCreateInfo(const LosMuxCB *mux, UINTPTR val)
{
    (void)mux;
    (void)val;
}

extern UINT8 g_excMsgArray[];
extern void FS_LockInit(void);
extern VOID ShbTskSwitchHook(UINT32 oldTaskId, UINT32 newTaskId);
extern UINT32 LOS_NextTaskIDGet(VOID);

#if !defined(LOSCFG_SAFETY_KERNEL)
extern VOID OsTaskWait(LOS_DL_LIST *list, UINT16 taskStatus, UINT32 timeout);
extern VOID OsTaskWake(LosTaskCB *resumedTask, UINT16 taskStatus);
#endif

#ifdef LOSCFG_MEM_MUL_POOL
extern VOID *LOS_MemPoolGet(VOID *pool);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OSAL_LITE_H */