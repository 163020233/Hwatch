/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */

/**
 * @defgroup bluetooth_bts_def PUBLIC API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_DEF_H
#define BTS_DEF_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Length of bluetooth device address.
 * @else
 * @brief  蓝牙设备的地址长度。
 * @endif
 */
#define BD_ADDR_LEN 6

/**
 * @if Eng
 * @brief  Max length of bluetooth uuid in octets.
 * @else
 * @brief  蓝牙UUID的最大长度，单位字节。
 * @endif
 */
#define BT_UUID_MAX_LEN 16

/**
 * @if Eng
 * @brief  Max length of bluetooth device name.
 * @else
 * @brief  蓝牙设备名称的最大长度，单位字节。
 * @endif
 */
#define BT_DEVICE_NAME_MAX_LEN 32

/**
 * @if Eng
 * @brief  Length of bluetooth link keyin octets.
 * @else
 * @brief  蓝牙配对密钥长度，单位字节。
 * @endif
 */
#define BT_LINK_KEY_LEN 16

/**
 * @if Eng
 * @brief  BLE channel map len.
 * @else
 * @brief  BLE 信道指示位图长度。
 * @endif
 */
#define BT_CHANNEL_MAP_LEN 5

/**
 * @if Eng
 * @brief  Min value of ble error code.
 * @else
 * @brief  蓝牙错误状态类型最小值。
 * @endif
 */
#define ERRCODE_BT_COMMON_BASE               0x80006000
/**
 * @if Eng
 * @brief  Min value of ble error code.
 * @else
 * @brief  蓝牙错误状态类型最大值。
 * @endif
 */
#define ERRCODE_BT_COMMON_END                 0x800067FF

/**
 * @if Eng
 * @brief  Enum of transport type.
 * @else
 * @brief  定义蓝牙传输的类型。
 * @endif
 */
typedef enum {
    BT_TRANSPORT_INVALID = 0x00, /*!< @if Eng invalid transport type. @else 未知传输类型。 @endif */
    BT_TRANSPORT_BR_EDR = 0x01,  /*!< @if Eng BR/EDR transport type. @else 传输类型为BR/EDR。 @endif */
    BT_TRANSPORT_LE = 0x02       /*!< @if Eng BLE transport type. @else 传输类型为BLE。 @endif */
} bt_transport_type_t;

/**
 * @if Eng
 * @brief  Enum of app id.
 * @else
 * @brief  定义注册BT回调的应用id。
 * @endif
 */
typedef enum {
    BT_CALLBACK_APP_ID_DEFAULT = 0x0,  /*!< @if Eng Default app id @else 默认的应用id。 @endif */
    BT_CALLBACK_APP_ID_MAX = 0xFF
} bt_callback_app_id_t;

/**
 * @if Eng
 * @brief  Enum of uuid type.
 * @else
 * @brief  定义UUID的类型。
 * @endif
 */
typedef enum {
    UUID_TYPE_NULL = 0x00, /*!< @if Eng null uuid. @else  空uuid。 @endif */
    UUID_TYPE_16_BIT,      /*!< @if Eng uuid type of 16 bit. @else  16bit长度的uuid。 @endif */
    UUID_TYPE_32_BIT,      /*!< @if Eng uuid type of 32 bit. @else  32bit长度的uuid。 @endif */
    UUID_TYPE_128_BIT,     /*!< @if Eng uuid type of 128 bit. @else  128bit长度的uuid。 @endif */
} uuid_type_t;

/**
 * @if Eng
 * @brief  Enum of bt status error code.
 * @else
 * @brief  定义蓝牙错误状态类型。
 * @endif
 */
typedef enum {
    ERRCODE_BT_SUCCESS = 0x00,                /*!< @if Eng error code of success
                                                   @else   执行成功错误码 @endif */
    ERRCODE_BT_FAIL = ERRCODE_BT_COMMON_BASE, /*!< @if Eng error code of failure
                                                   @else   执行失败错误码 @endif */
    ERRCODE_BT_NOT_READY,                     /*!< @if Eng error code of not ready
                                                   @else   执行状态未就绪错误码 @endif */
    ERRCODE_BT_MALLOC_FAIL,                   /*!< @if Eng error code of memmery not enough
                                                   @else   内存不足错误码 @endif */
    ERRCODE_BT_MEMCPY_FAIL,                   /*!< @if Eng error code of memcpy fail
                                                   @else   内存拷贝错误错误码 @endif */
    ERRCODE_BT_BUSY,                          /*!< @if Eng error code of system is busy
                                                   @else   繁忙无法响应错误码 @endif */
    ERRCODE_BT_DONE,                          /*!< @if Eng error code of complete
                                                   @else   执行完成错误码 @endif */
    ERRCODE_BT_UNSUPPORTED,                   /*!< @if Eng error code of not support
                                                   @else   不支持错误码 @endif */
    ERRCODE_BT_PARAM_ERR,                     /*!< @if Eng error code of invalid param
                                                   @else   无效参数错误码 @endif */
    ERRCODE_BT_STATE_ERR,                     /*!< @if Eng error code of state error
                                                   @else   状态错误 @endif */
    ERRCODE_BT_UNHANDLED,                     /*!< @if Eng error code of unhandle
                                                   @else   未处理错误码 @endif */
    ERRCODE_BT_AUTH_FAIL,                     /*!< @if Eng error code of auth failure
                                                   @else   鉴权失败错误码 @endif */
    ERRCODE_BT_RMT_DEV_DOWN,                  /*!< @if Eng error code of remote device down
                                                   @else   远端设备关闭错误码 @endif */
    ERRCODE_BT_AUTH_REJECTED,                 /*!< @if Eng error code of auth rejected
                                                   @else   鉴权被拒错误码 @endif */
    ERRCODE_BT_KEY_MISSING,                   /*!< @if Eng error code of key missing
                                                   @else   密钥丢失错误码 @endif */
    ERRCODE_BT_OUT_OF_RANGE,                  /*!< @if Eng error code of param out of range
                                                   @else   参数超出范围错误码 @endif */
    ERRCODE_BT_MAX = ERRCODE_BT_COMMON_END    /*!< @if Eng the max of bt error code
                                                   @else   蓝牙错误码最大值 @endif */
} errcode_bt_t;

/**
 * @if Eng
 * @brief Enum of disconnect reason.
 * @else
 * @brief 蓝牙断链原因。
 * @endif
 */
typedef enum {
    BT_DISCONNECT_UNKNOWN                       = 0x00,    /*!< @if Eng unknown disconnect reason
                                                                @else   未知原因断链 @endif */
    BT_DISCONNECT_PAGE_TIMEOUT                  = 0x04,    /*!< @if page timeout
                                                                @else   page超时 @endif */
    BT_DISCONNECT_BY_PIN_OR_KEY_MISSING         = 0x06,    /*!< @if Eng pin or key missing
                                                                @else   pin或key丢失 @endif */
    BT_DISCONNECT_BY_CONNECT_TIMEOUT            = 0x08,    /*!< @if Eng disconnect by timeout
                                                                @else   连接超时断链 @endif */
    BT_CONNECTION_ALREADY_EXISTS                = 0x0B,    /*!< @if Eng connection already exists
                                                                @else   连接已存在 @endif */
    BT_DISCONNECT_BY_REMOTE_USER                = 0x13,    /*!< @if Eng disconnect by remote
                                                                @else   远端用户断链 @endif */
    BT_DISCONNECT_BY_REMOTE_DUE_TO_LOW_RESOURCE = 0x14,    /*!< @if Eng disconnect by remote due to low resource
                                                                @else   资源不足远端用户断链 @endif */
    BT_DISCONNECT_BY_REMOTE_DUE_TO_POWER_OFF    = 0x15,    /*!< @if Eng disconnect by remote due to power off
                                                                @else   设备断电远端用户断链 @endif */
    BT_DISCONNECT_BY_LOCAL_HOST                 = 0x16,    /*!< @if Eng connection terminated by local
                                                                @else   本端HOST断链 @endif */
    BT_DISCONNECT_BY_MIC_ERROR                  = 0x3D,    /*!< @if Eng connection terminated due to mic failure
                                                                @else   mic error断连 @endif */
    BT_ESTABLISH_CONNECT_FAIL                   = 0x3E,    /*!< @if Eng connection failed to be established
                                                                @else   建连异常 @endif */
} bt_disconnect_reason_t;

/**
 * @if Eng
 * @brief  Enum of bluetooth device address type.
 * @else
 * @brief  定义设备的蓝牙地址类型。
 * @endif
 */
typedef enum {
    BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS          =  0x00, /*!< @if Eng bluetooth public address.
                                                                 @else   蓝牙公有地址。 @endif */
    BT_ADDRESS_TYPE_RANDOM_DEVICE_ADDRESS          =  0x01, /*!< @if Eng bluetooth random address.
                                                                 @else   蓝牙随机地址。 @endif */
} bt_addr_type;

/**
 * @if Eng
 * @brief  Enum of link key type.
 * @else
 * @brief  定义密钥类型。
 * @endif
 */
typedef enum {
    BT_LINK_KEY_TYPE_COMBINATION_KEY =
        0x00, /*!< @if Eng combination key.
                   @else   混合密钥。 @endif */
    BT_LINK_KEY_TYPE_LOCAL_UNIT_KEY =
        0x01, /*!< @if Eng local unit key.
                   @else   本地单元密钥。 @endif */
    BT_LINK_KEY_TYPE_REMOTE_UNIT_KEY =
        0x02, /*!< @if Eng remote unit key.
                   @else   远端单元密钥。 @endif */
    BT_LINK_KEY_TYPE_DEBUG_COMBINATION_KEY =
        0x03, /*!< @if Eng debug combination key.
                   @else   调试混合密钥。 @endif */
    BT_LINK_KEY_TYPE_UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192 =
        0x04, /*!< @if Eng unauthenticated combination key generated from P192.
                   @else   P192未授权混合密钥。 @endif */
    BT_LINK_KEY_TYPE_AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P192 =
        0x05, /*!< @if Eng authenticated combination key generated from P192.
                   @else   P192授权混合密钥。 @endif */
    BT_LINK_KEY_TYPE_CHANGED_COMBINATION_KEY =
        0x06, /*!< @if Eng changed combination key.
                   @else   变化的混合密钥。 @endif */
    BT_LINK_KEY_TYPE_UNAUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256 =
        0x07, /*!< @if Eng unauthenticated combination key generated from P256.
                   @else   P256未授权混合密钥。 @endif */
    BT_LINK_KEY_TYPE_AUTHENTICATED_COMBINATION_KEY_GENERATED_FROM_P256 =
        0x08, /*!< @if Eng authenticated combination key generated from P256.
                   @else   P256授权混合密钥。 @endif */
} bt_link_key_type_t;

/**
 * @if Eng
 * @brief  Struct of bluetooth device address.
 * @else
 * @brief  定义设备的蓝牙地址。
 * @endif
 */
typedef struct {
    uint8_t addr[BD_ADDR_LEN];     /*!< @if Eng bluetooth device address.
                                        @else   蓝牙地址。 @endif */
    uint8_t type;                  /*!< @if Eng bluetooth device address type, See { @ref bt_addr_type }.
                                        @else   蓝牙地址类型，参考 { @ref bt_addr_type }。 @endif */
} bd_addr_t;

/**
 * @if Eng
 * @brief  Struct of UUID.
 * @else
 * @brief  定义UUID结构。
 * @endif
 */
typedef struct {
    uint8_t uuid_len;               /*!< @if Eng length of uuid.
                                        @else    UUID长度。 @endif */
    uint8_t uuid[BT_UUID_MAX_LEN]; /*!< @if Eng uuid.
                                        @else    UUID字段。 @endif */
} bt_uuid_t;

/**
 * @if Eng
 * @brief Struct of string.
 * @else
 * @brief 定义字符串结构。
 * @endif
 */
typedef struct {
    uint16_t len;                   /*!< @if Eng size of the value, exclude the end '\0'.
                                        @else 字符个数，不包含字符串结束符 @endif */
    uint8_t *value;                 /*!< @if ptr to the value
                                        @else 字符串指针 @endif */
} bg_string_t;

/**
 * @if Eng
 * @brief  Struct of link key.
 * @else
 * @brief  定义配对密钥结构。
 * @endif
 */
typedef struct {
    uint8_t link_key[BT_LINK_KEY_LEN]; /*!< @if Eng link key.
                                            @else 配对密钥 @endif */
    uint8_t key_type;                  /*!< @if Eng key type, See { @ref bt_link_key_type_t }.
                                            @else 密钥类型，参考 { @ref bt_link_key_type_t }。 @endif */
} bt_link_key_t;
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif  /* BTS_DEF_H */

