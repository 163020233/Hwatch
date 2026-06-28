/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved.
 */

#ifndef NPU_DRV_FREQ_IF_H
#define NPU_DRV_FREQ_IF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup TSDRV
 * @brief tsdrv frequency change
 *
 * @param freq [IN]      npu frequency, param must be 64 or 133 or 200
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int32_t uapi_npu_freq_change(uint32_t freq);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
