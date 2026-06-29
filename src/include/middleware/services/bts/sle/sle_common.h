/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 *
 * Description: SLE PUBLIC API module.
 */

/**
 * @defgroup sle_common PUBLIC API
 * @ingroup  SLE
 * @{
 */

#ifndef SLE_COMMON_H
#define SLE_COMMON_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  SLE version 1.0.
 * @else
 * @brief  SLE版本1.0。
 * @endif
 */
#define SLE_VERSION         0x10

/**
 * @if Eng
 * @brief  SLE address length.
 * @else
 * @brief  SLE地址长度。
 * @endif
 */
#define SLE_ADDR_LEN        6

/**
 * @if Eng
 * @brief  SLE device name max length.
 * @else
 * @brief  SLE设备名称最大长度。
 * @endif
 */
#define SLE_NAME_MAX_LEN    31

/**
 * @if Eng
 * @brief  SLE link key length.
 * @else
 * @brief  SLE 链路密钥长度。
 * @endif
 */
#define SLE_LINK_KEY_LEN    16

/**
 * @if Eng
 * @brief  SLE channel map len.
 * @else
 * @brief  SLE 信道指示位图长度。
 * @endif
 */
#define SLE_CHANNEL_MAP_LEN 10

/**
 * @if Eng
 * @brief  SLE standard feature len.
 * @else
 * @brief  SLE 标准特性长度。
 * @endif
 */
#define SLE_STANDARD_FEATURES_LEN 10

/**
 * @if Eng
 * @brief  SLE private feature len.
 * @else
 * @brief  SLE 私有特性长度。
 * @endif
 */
#define SLE_PRIVATE_FEATURES_LEN 16

/**
 * @if Eng
 * @brief  SLE address type.
 * @else
 * @brief  SLE地址类型。
 * @endif
 */
typedef enum {
    SLE_ADDRESS_TYPE_PUBLIC = 0,      /*!< @if Eng public address
                                           @else   公有地址 @endif */
    SLE_ADDRESS_TYPE_RANDOM = 6,      /*!< @if Eng random address
                                           @else   随机地址 @endif */
} sle_addr_type_t;

/**
 * @if Eng
 * @brief  Struct of device address.
 * @else
 * @brief  设备地址。
 * @endif
 */
typedef struct {
    uint8_t type;                         /*!< @if Eng SLE device address type { @ref sle_addr_type_t }
                                               @else   SLE设备地址类型 { @ref sle_addr_type_t } @endif */
    unsigned char addr[SLE_ADDR_LEN];     /*!< @if Eng SLE device address
                                               @else   SLE设备地址 @endif */
} sle_addr_t;


/**
 * @if Eng
 * @brief  Link type.
 * @else
 * @brief 链路类型。
 * @endif
 */
typedef enum {
    SLE_ASYNCHRONOUS_CONNECTION = 0x00,     /*!< @if Eng not transparent
                                                 @else 异步链路 @endif */
    SLE_SYNCHRONIZATION_CONNECTION = 0x01,  /*!< @if Eng transparent
                                                 @else 同步链路 @endif */
} sle_link_type_t;

/**
 * @if Eng
 * @brief Link parameter information.
 * @else
 * @brief 链路参数信息
 * @endif
 */
typedef struct {
    uint8_t link_type;          /*!< @if Eng link type { @ref sle_link_type_t }.
                                     @else   链路类型 { @ref sle_link_type_t }。 @endif */
    union {
        uint16_t conn_id;       /*!< @if Eng  conn id.
                                      @else  连接 ID。 @endif */
        struct {
            uint8_t conn_type;  /*!< @if Eng ISO link type.
                                     @else   同步链路类型。 @endif */
            uint8_t group_id;   /*!< @if Eng ISO group id.
                                     @else   同步链路组ID。 @endif */
            uint16_t conn_id;   /*!< @if Eng ISO conn id.
                                     @else   连接 ID @endif */
            uint8_t member_id;  /*!< @if Eng ISO link member id.
                                    @else    同步链路的成员ID。 @endif */
        } iso_conn_info;
    };
} sle_conn_info_t;
#ifdef __cplusplus
}
#endif
#endif  /* SLE_COMMON_H */
/**
 * @}
 */

