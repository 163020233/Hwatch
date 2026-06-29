/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
 */

#ifndef _HOS_CMSIS_ADP_H
#define _HOS_CMSIS_ADP_H

#include "cmsis_os.h"
#include "vfs_files.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    osSemaphoreAttr_t attr;
    unsigned int maxCount;
    unsigned int initialCount;
} SemaphoreEx;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
