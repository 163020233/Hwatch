/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _LOS_DEBUG_H
#define _LOS_DEBUG_H
#include "los_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    EXC_REBOOT,
    EXC_ASSERT,
    EXC_PANIC,
    EXC_STACKOVERFLOW,
    EXC_INTERRUPT,
    EXC_TYPE_END
} EXC_TYPE;

VOID OsExcHookRegister(ExcHookFn excHookFn);
typedef VOID (*ExcHookFn)(EXC_TYPE excType);
VOID OsDoExcHook(EXC_TYPE excType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_DEBUG_H */
