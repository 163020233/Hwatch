/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NPU_DRV_FILE_IF_H
#define NPU_DRV_FILE_IF_H

#include <stdint.h>

/**
 * @ingroup TSDRV
 * @brief tsdrv inner file system initialize
 *
 * @param file_sys_id [IN]      ID of the inner file system
 * @param file_sys_addr [IN]    start address of the inner file system
 * @param file_sys_len [IN]     length of the inner file system
 *
 * @retval 0 The function is successfully executed.
 * @retval OtherValues Failure.
 */
int32_t uapi_npu_file_sys_init(uint32_t file_sys_id, uint32_t file_sys_addr, uint32_t file_sys_len);

/**
 * @ingroup TSDRV
 * @brief tsdrv inner file system deinitialize
 */
void uapi_npu_file_sys_deinit(void);

#endif
