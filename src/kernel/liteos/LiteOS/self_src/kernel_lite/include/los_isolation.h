/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description : LiteOS Kernel Isolation Module Implementation..
 * Author : Huawei LiteOS Team
 * Create : 2026-06-09
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

/**
 * @defgroup los_isolation
 * @ingroup kernel
 */

#ifndef _LOS_ISOLATION_H
#define _LOS_ISOLATION_H

#include "los_base.h"
#include "los_task.h"
#include "los_task_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ISOLATION_TASK_MAX_NUM 1
#define KERNEL_COMMON_STACK_SIZE 3072
#define ISOLATION_STACK_SIZE (KERNEL_COMMON_STACK_SIZE - OS_STACK_MAGIC_SIZE)

typedef struct {
    INT32 (*enterCallback)(VOID);
    INT32 (*exitCallback)(VOID);
    UINTPTR oldSp;
    UINTPTR topOfStack;
    UINTPTR stackSize;
} OsIsolationContext;

typedef struct {
    CHAR *taskName;
    INT32 (*enterCallback)(VOID);
    INT32 (*exitCallback)(VOID);
    UINT32 baseAddress;
    UINT32 regionSize;
} IsolationTaskInfo;

extern UINT32 LOS_IsolationRegionRegister(IsolationTaskInfo *config);
extern UINT32 LOS_IsolationRegionUnregister(const CHAR *taskName);
extern VOID OsIsolationContextSave(OsIsolationContext *ctx);
extern VOID OsIsolationContextRestore(OsIsolationContext *ctx);
extern IsolationTaskInfo *OsIsolationTaskCheck(const CHAR *taskName);
extern VOID OsIsolationTaskSwitch(LosTaskCB *taskCB);
extern IsolationTaskInfo *g_currentIsolationTask;

#define LOS_ISOLATION_ACCESS_CHECK_PTR(ptr)                                                  \
    ({                                                                                       \
        UINT32 _ret = LOS_OK;                                                                \
        IsolationTaskInfo *_task_ = g_currentIsolationTask;                                  \
        if (_task_ != NULL && _task_->regionSize != 0) {                                     \
            if ((ptr) < _task_->baseAddress || (ptr)-_task_->baseAddress > _task_->regionSize) { \
                _ret = LOS_NOK;                                                              \
            }                                                                                \
        }                                                                                    \
        _ret;                                                                                \
    })

extern UINTPTR SYS_ISO_CALL(UINTPTR func, ...);
#define va_num_args_impl(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define va_num_args(...) va_num_args_impl(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define all_unused_impl_(nargs) ISO_CALL##nargs
#define all_unused_impl(nargs) all_unused_impl_(nargs)

#define ISO_CALL1(a) SYS_ISO_CALL((UINTPTR)(a))
#define ISO_CALL2(a, b) SYS_ISO_CALL((UINTPTR)(a), b)
#define ISO_CALL3(a, b, c) SYS_ISO_CALL((UINTPTR)(a), b, c)
#define ISO_CALL4(a, b, c, d) SYS_ISO_CALL((UINTPTR)(a), b, c, d)
#define ISO_CALL5(a, b, c, d, e) SYS_ISO_CALL((UINTPTR)(a), b, c, d, e)
#define ISO_CALL6(a, b, c, d, e, f) SYS_ISO_CALL((UINTPTR)(a), b, c, d, e, f)
#define ISO_CALL7(a, b, c, d, e, f, g) SYS_ISO_CALL((UINTPTR)(a), b, c, d, e, f, g)
#define ISO_CALL8(a, b, c, d, e, f, g, h) SYS_ISO_CALL((UINTPTR)(a), b, c, d, e, f, g, h)

#define ISO_CALL(...) all_unused_impl(va_num_args(__VA_ARGS__))(__VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_ISOLATION_H */
