/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NPU_DRV_WORK_IF_H
#define NPU_DRV_WORK_IF_H

#include <stdint.h>

typedef void (* npu_drv_msg_handle)(void *npu_drv_data, uint16_t size);

/**
 * @ingroup TSDRV
 * @brief call the npu main function for processing
 *
 * @param data [IN]                 npu main function addr
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int uapi_npu_work_handle(void *data);

/**
 * @ingroup TSDRV
 * @brief register msg process handle for call npu main function
 *
 * @param handle [IN]               msg process handle func ptr
 */
void uapi_npu_register_msg_callback(npu_drv_msg_handle handle);

#endif
