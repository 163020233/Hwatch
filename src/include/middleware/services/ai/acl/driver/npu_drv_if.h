/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 */

#ifndef NPU_DRV_IF_H
#define NPU_DRV_IF_H

#include <stdint.h>
#include "npu_drv_acl_if.h"
#include "npu_drv_file_if.h"
#include "npu_drv_work_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup TSDRV
 * @brief tsdrv debug function initialize
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int32_t uapi_npu_debug_init(void);

/**
 * @ingroup TSDRV
 * @brief set module loglevel and enableEvent
 *
 * @param moduleId [IN]         moudule id(see slog.h, eg: CCE), -1: all modules, others: invalid
 * @param level [IN]            log level(0: debug, 1: info, 2: warning, 3: error, 4: null output)
 * @param enableEvent [IN]      1: enable; 0: disable, others:invalid
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int32_t uapi_npu_dlog_setlevel(int moduleId, int level, int enableEvent);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
