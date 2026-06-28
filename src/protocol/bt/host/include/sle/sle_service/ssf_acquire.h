
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Acquire API File.
 */

#ifndef SSF_ACQUIRE_H
#define SSF_ACQUIRE_H

#include <stdint.h>
#include <stddef.h>
#include "errcode.h"
#include "sle_common.h"
#include "bs_device_mgr.h"
#include "ssf_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLE_ACQUIRE_COMMON = SLE_CONFIG_COMMON,
} bs_sle_acquire_group_t;

typedef enum {
    SLE_ACQUIRE_COMMON_LOCAL_ADDR = SLE_ACQUIRE_COMMON | 0x01,

    SLE_ACQUIRE_TYPE_MAX,
} bs_sle_acquire_type_t;

typedef union {
    /**
     * @brief SLE_ACQUIRE_COMMON_LOCAL_ADDR
     */
    sle_addr_t local_addr;
} bs_sle_acquire_t;

/**
 * @if Eng
 * @brief SLE information acquisition function
 * @par This function is used to acquire status information from SLE by specifying the operation type and sub-item.
 * @attention None
 * @param [in]  type  Operation type, {ref bs_sle_acquire_type_t}, the first 3 bits represent the protocol type, the following 5 bits
 *                    represent the module type, and the last 8 bits represent the specific acquisition item.
 * @param [out] param Acquired parameters, the specific type depends on the operation type {ref bs_sle_acquire_t}, must not be NULL.
 * @param [out] len   Length of the acquired parameters, only applies when the acquired parameter is an array, can be NULL.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief 星闪获取信息函数
 * @par  该函数用于获取星闪的状态信息，通过指定操作类型和子项来获取信息。
 * @attention 无
 * @param  [in]  type  操作类型，{ref bs_sle_acquire_type_t}，前3bit代表协议类型，其后5bit表示模块类型，最后8bit代表具体获取项。
 * @param  [out] param 获取参数，具体类型根据操作类型而定{ref bs_sle_acquire_t}，不得为NULL。
 * @param  [out] len   获取参数的长度，仅在获取参数为数组时生效，可以为NULL。
 * @retval 执行结果错误码，表示操作是否成功。
 * @endif
 */
errcode_t bs_sle_acquire(bs_sle_acquire_type_t type, bs_sle_acquire_t *param, uint16_t *len);

#ifdef __cplusplus
}
#endif
#endif /* SSF_ACQUIRE_H */