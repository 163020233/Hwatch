/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description : LiteOS shared stack func.
 * Author : Huawei LiteOS Team
 * Create : 2025-11-03
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
 
#ifndef KEYSRC_SHARED_STACK_H
#define KEYSRC_SHARED_STACK_H
 
#include "los_task_pri.h"
#include "los_sem_pri.h"
#include "los_exc.h"
#ifdef LOSCFG_ARCH_VECTOR_ENABLE
#include "arch/vector.h"
#endif
 
extern LITE_OS_SEC_TEXT_INIT VOID ContextInitOnly(LosTaskCB *taskCB);
 
extern UINT32 OsSharedTaskCBInit(TSK_INIT_PARAM_S *initParam, LosTaskCB *taskCB, BOOL useUsrStack, VOID *topStack);
 
extern VOID LocalVarRefError(uintptr_t varPtr, char *varName);
extern void SharedTaskStatPrint(void);
 
extern LosTaskCB *TaskInfoTrans(LosTaskCB *newTask, LosTaskCB *runTask);
 
#endif