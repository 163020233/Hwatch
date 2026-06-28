/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: bs device manager definitions
 */

#ifndef BS_DEVICE_MGR_H
#define BS_DEVICE_MGR_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS_PROTOCOL_CONFIG_SHIFT_BITS 13
#define BS_ADDR_LEN 6
#define BS_CONNECTION_UPPER_LIMIT 4

/**
 * @if Eng
 * @brief Enum of protocols.
 * @else
 * @brief 协议类型枚举.
 * @endif
 */
typedef enum {
    BS_PROTOCOL_BREDR,
    BS_PROTOCOL_BLE,
    BS_PROTOCOL_SLE,
    BS_MODULE_SDM,
} bs_protocol_type_t;

/**
 * @if Eng
 * @brief device address.
 * @else
 * @brief 设备地址。
 * @endif
 */
typedef struct {
    uint8_t addr[BS_ADDR_LEN];
    uint8_t type;
} bs_addr_t;

/**
 * @if Eng
 * @brief Use this function to config parameters.
 * @par Use this function config parameters.
 * @param [in] type  types of parameters {@ref bs_ble_config_type_t}
 * @param [in] param parameter
 * @param [in] len   length of parameter
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  配置参数.
 * @par    配置参数.
 * @param [in] type  配置参数类型 {@ref bs_ble_config_type_t}
 * @param [in] param 参数
 * @param [in] len   参数长度
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_config(uint16_t type, void *param, uint16_t len);

/**
 * @if Eng
 * @brief Use this function to acquire parameters.
 * @par Use this function acquire parameters.
 * @param [in] type  types of parameters
 * @param [out] param parameter
 * @param [out] len   length of parameter
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  获取参数.
 * @par    获取参数.
 * @param [in] type  获取参数类型
 * @param [out] param 参数
 * @param [out] len   参数长度
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_acquire(uint16_t type, void *param, uint16_t *len);

/**
 * @if Eng
 * @brief Use this function to allocate device ID by address.
 * @par Use this function to allocate device ID by address. If can not allocate device ID,
        return BS_CONNECTION_UPPER_LIMIT.
 * @param [in] protocol protocol type. {@ref bs_protocol_type_t}
 * @param [in] type     address type.
 * @param [in] addr     device address.
 * @retval device ID.
 * @par Dependency:
 * @li
 * @else
 * @brief  通过地址分配device ID。
 * @par    通过地址分配device ID。如果无法分配device ID，则返回 BS_CONNECTION_UPPER_LIMIT。
 * @param [in] protocol 协议类型。 {@ref bs_protocol_type_t}
 * @param [in] type     地址类型。
 * @param [in] addr     设备地址。
 * @retval device ID.
 * @par 依赖:
 * @li
 * @endif
 */
uint16_t bs_alloc_device_id_by_addr(bs_protocol_type_t protocol, uint8_t type, const uint8_t *addr);

/**
 * @if Eng
 * @brief Use this function to find device ID by address.
 * @par Use this function to find device ID by address. If can not find device ID,
        return BS_CONNECTION_UPPER_LIMIT.
 * @param [in] protocol protocol type. {@ref bs_protocol_type_t}
 * @param [in] type     address type.
 * @param [in] addr     device address.
 * @retval device ID.
 * @par Dependency:
 * @li
 * @else
 * @brief  通过地址查找device ID。
 * @par    通过地址查找device ID。如果无法分配device ID，则返回 BS_CONNECTION_UPPER_LIMIT。
 * @param [in] protocol 协议类型。 {@ref bs_protocol_type_t}
 * @param [in] type     地址类型。
 * @param [in] addr     设备地址。
 * @retval device ID.
 * @par 依赖:
 * @li
 * @endif
 */
uint16_t bs_find_device_id_by_addr(bs_protocol_type_t protocol, uint8_t type, const uint8_t *addr);

/**
 * @if Eng
 * @brief Use this function to get address by device ID.
 * @par   Use this function to get address by device ID. If device ID is invalid, return NULL.
 * @param [in] protocol protocol type. {@ref bs_protocol_type_t}
 * @retval device address.
 * @par Dependency:
 * @li
 * @else
 * @brief 通过device ID获取地址。
 * @par   通过device ID获取地址。如果device ID无效，则返回NULL。
 * @param [in] protocol 协议类型。 {@ref bs_protocol_type_t}
 * @retval 设备地址。
 * @par 依赖:
 * @li
 * @endif
 */
bs_addr_t *bs_get_addr_by_device_id(uint16_t dev_id);

#ifdef __cplusplus
}
#endif
#endif /* BS_DEVICE_MGR_H */