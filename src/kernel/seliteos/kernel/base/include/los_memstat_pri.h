/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Memory Module Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-11-06
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

#ifndef LOS_MEMSTAT_PRI_H
#define LOS_MEMSTAT_PRI_H

#include "los_typedef.h"
#include "los_memory.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_MEM_TASK_STAT
typedef struct {
    UINT32 memUsed;
    UINT32 memPeak;
} TaskMemUsedInfo;

typedef struct {
    UINT32 memTotalUsed;
    UINT32 memTotalPeak;
    UINT32 memTaskCnt;
    TaskMemUsedInfo taskMemstats[0]; /* Task number is variable in kernel and box */
} Memstat;

extern VOID OsMemstatInit(Memstat *stat, UINT32 usedSize);
extern VOID OsMemstatTaskUsedInc(Memstat *stat, UINT32 usedSize, UINT32 taskId);
extern VOID OsMemstatTaskUsedDec(Memstat *stat, UINT32 usedSize, UINT32 taskId);
extern VOID OsMemTaskClear(UINT32 taskId);
extern UINT32 OsMemTaskUsage(UINT32 taskId);
#define OS_MEM_STAT_INIT(stat, usedSize)                OsMemstatInit(stat, usedSize)
#define OS_MEM_ADD_USED(stat, usedSize, taskId)         OsMemstatTaskUsedInc(stat, usedSize, taskId)
#define OS_MEM_REDUCE_USED(stat, usedSize, taskId)      OsMemstatTaskUsedDec(stat, usedSize, taskId)
#define OS_MEM_CLEAR(taskId)                            OsMemTaskClear(taskId)
#else
#define OS_MEM_STAT_INIT(stat, usedSize)
#define OS_MEM_ADD_USED(stat, usedSize, taskId)
#define OS_MEM_REDUCE_USED(stat, usedSize, taskId)
#define OS_MEM_CLEAR(taskId)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_MEMSTAT_PRI_H */
