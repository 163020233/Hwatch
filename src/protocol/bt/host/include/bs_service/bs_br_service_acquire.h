/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR serivce config enums
 */

#ifndef BS_BR_SERVICE_ACQUIRE_H
#define BS_BR_SERVICE_ACQUIRE_H

#include <stdbool.h>
#include "bs_device_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BR_DEVICE_NAME_LEN 32

typedef enum {
    BR_ACQUIRE_LOCAL    = 0x0000 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BR_ACQUIRE_REMOTE   = 0x0100 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
} bs_br_acquire_group_t;

typedef union {
    /**
     * @brief BR_ACQUIRE_REMOTE_BOND_LIST
     */
    struct bs_br_bond_device_info {
        uint8_t     addr[BS_ADDR_LEN];
        uint16_t    dev_id;
        uint32_t    class_of_device;
        uint8_t     name[BR_DEVICE_NAME_LEN];
    } bond_device_info;
} bs_br_acquire_t;

/**
 * @if Eng
 * @brief Enum of config parameters type.
 * @else
 * @brief 配置参数类型枚举.
 * @endif
 */
typedef enum {
    BR_ACQUIRE_REMOTE_BOND_LIST           = BR_ACQUIRE_REMOTE | 0x0,
} bs_br_acquire_type_t;

/**
 * @if Eng
 * @brief Use this function to acquire the device.
 * @par Use this function to acquire the device.
 * @param [in] type type Type of acquisition.
 * @param [out] param Obtained Data Content.
 * @param [out] len Length of the obtained data.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  用于获取设备信息
 * @par    用于获取设备信息
 * @param [in] type 获取类型。
 * @param [out] param 获取到的数据内容。
 * @param [out] len 获取到的数据长度。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_acquire(bs_br_acquire_type_t type, bs_br_acquire_t *param, uint16_t *len);

#ifdef __cplusplus
}
#endif
#endif /* BS_BR_SERVICE_ACQUIRE_H */