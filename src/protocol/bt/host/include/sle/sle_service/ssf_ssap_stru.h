
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: ssf SLE SSAP STRU module.
 */

#ifndef SSF_SSAP_STRU_H
#define SSF_SSAP_STRU_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Length of sle UUID.
 * @else
 * @brief  星闪UUID长度。
 * @endif
 */
#define BS_SLE_UUID_LEN 16

/**
 * @if Eng
 * @brief Enum of ssap item type service、property、method or event.
 * @else
 * @brief SSAP 条目类型，服务、属性、方法、事件。
 * @endif
 */
typedef enum {
    BS_SSAP_ITEM_TYPE_SERVICE  = 0x00,       /*!< @if Eng SSAPS item type: service
                                                  @else   SSAP 条目类型：服务 @endif */
    BS_SSAP_ITEM_TYPE_PROPERTY = 0x01,       /*!< @if Eng SSAPS item type: property
                                                  @else   SSAP 条目类型：属性 @endif */
    BS_SSAP_ITEM_TYPE_METHOD   = 0x02,       /*!< @if Eng SSAPS item type: method
                                                  @else   SSAP 条目类型：方法 @endif */
    BS_SSAP_ITEM_TYPE_EVENT    = 0x03,       /*!< @if Eng SSAPS item type: event
                                                  @else   SSAP 条目类型：事件 @endif */
} bs_sle_ssap_item_type_t;

/**
 * @if Eng
 * @brief  SSAP property type.
 * @else
 * @brief  SSAP 特征类型。
 * @endif
 */
typedef enum {
    BS_SSAP_PROPERTY_TYPE_VALUE             = 0x00,  /*!< @if Eng property value
                                                          @else   特征值 @endif */
    BS_SSAP_DESCRIPTOR_USER_DESCRIPTION     = 0x01,  /*!< @if Eng user description descriptor
                                                          @else   属性说明描述符 @endif */
    BS_SSAP_DESCRIPTOR_CLIENT_CONFIGURATION = 0x02,  /*!< @if Eng client configuration descriptor
                                                          @else   客户端配置描述符 @endif */
    BS_SSAP_DESCRIPTOR_SERVER_CONFIGURATION = 0x03,  /*!< @if Eng server configuration descriptor
                                                          @else   服务端配置描述符 @endif */
    BS_SSAP_DESCRIPTOR_PRESENTATION_FORMAT  = 0x04,  /*!< @if Eng presentation format descriptor
                                                          @else   格式描述符 @endif */
    BS_SSAP_DESCRIPTOR_RFU                  = 0x05,  /*!< @if Eng RFU descriptor, 0x05 – 0x1F
                                                          @else   服务管理保留描述符，0x05 – 0x1F @endif */
    BS_SSAP_DESCRIPTOR_CUSTOM               = 0xFF,  /*!< @if Eng custom descriptor
                                                          @else   厂商自定义描述符 @endif */
} bs_sle_ssap_property_type_t;

/**
 * @if Eng
 * @brief  SSAP attribute permission.
 * @else
 * @brief  SSAP attribute操作权限。
 * @endif
 */
typedef enum {
    BS_SSAP_PERMISSION_READ                = 0x01,  /*!< @if Eng readable
                                                         @else   可读 @endif */
    BS_SSAP_PERMISSION_WRITE               = 0x02,  /*!< @if Eng writable
                                                         @else   可写 @endif */
    BS_SSAP_PERMISSION_ENCRYPTION_NEED     = 0x04,  /*!< @if Eng need encryption
                                                         @else   需要加密 @endif */
    BS_SSAP_PERMISSION_AUTHENTICATION_NEED = 0x08,  /*!< @if Eng need authentication
                                                         @else   需要认证 @endif */
    BS_SSAP_PERMISSION_AUTHORIZATION_NEED  = 0x10,  /*!< @if Eng need authorization
                                                         @else   需要授权 @endif */
} bs_sle_ssap_permission_t;

/**
 * @if Eng
 * @brief  SSAP operation indication.
 * @else
 * @brief  SSAP 操作指示。
 * @endif
 */
typedef enum {
    BS_SSAP_OPERATE_INDICATION_BITMAP_READ            = 0x01, /*!< @if Eng data values can be read
                                                                   @else   数据值可被读取 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_WRITE_NO_RSP    = 0x02, /*!< @if Eng data values can be written without feedback.
                                                                   @else   数据值可被写入，写入后无反馈 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_WRITE           = 0x04, /*!< @if Eng data values can be written. After being
                                                                        written, a feedback is generated to the client.
                                                                   @else   数据值可被写入，写入后产生反馈给客户端 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_NOTIFY          = 0x08, /*!< @if Eng data value send to client by notification.
                                                                   @else   数据值通过通知方式传递给客户端 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_INDICATE        = 0x10, /*!< @if Eng data value send to client by indication.
                                                                   @else   数据值通过指示方式传递给客户端 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_BROADCAST       = 0x20, /*!< @if Eng data value can be carried in broadcast
                                                                   @else   数据值可携带在广播中 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_DESCRITOR_WRITE = 0x100,/*!< @if Eng data value description descriptor can be
                                                                        written
                                                                   @else   数据值说明描述符可被写入 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_DESCRIPTOR_CLIENT_CONFIGURATION_WRITE = 0x200,
                                                                /*!< @if Eng client descriptor value can be written
                                                                     @else 客户端描述符可被写入 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_DESCRIPTOR_SERVER_CONFIGURATION_WRITE = 0x400,
                                                                /*!< @if Eng server description value can be written
                                                                     @else 服务端述符可被写入 @endif */
    BS_SSAP_OPERATE_INDICATION_BITMAP_MAX,                      /*!< @if Eng maximum of operation indication
                                                                     @else   操作指示最大值 @endif */
} bs_sle_ssap_operate_indication_t;

/**
 * @if Eng
 * @brief  SSAP errcode.
 * @else
 * @brief  SSAP 错误码。
 * @endif
 */
typedef enum {
    BS_SSAP_ERRCODE_PDU_INVALID                           = 0x01,  /*!< @if Eng invalid PDU
                                                                        @else   服务端接收的 PDU 无效 @endif */
    BS_SSAP_ERRCODE_PDU_NOT_SUPPORTED                     = 0x02,  /*!< @if Eng PDU not support
                                                                        @else   服务端不支持处理接收的 @endif */
    BS_SSAP_ERRCODE_UNKNOW                                = 0x03,  /*!< @if Eng PDU unknow
                                                                        @else   服务端执行请求时发生未知错误 @endif */
    BS_SSAP_ERRCODE_INVALID_HANDLE                        = 0x04,  /*!< @if Eng invalid handle
                                                                        @else   请求中的句柄无效 @endif */
    BS_SSAP_ERRCODE_INSUFFICIENT_RESOURCES                = 0x05,  /*!< @if Eng insufficient resources
                                                                        @else   服务端没有足够资源完成请求 @endif */
    BS_SSAP_ERRCODE_READ_NOT_PERMITTED                    = 0x06,  /*!< @if Eng prohibit reading
                                                                        @else   服务端禁止客户端读取值 @endif */
    BS_SSAP_ERRCODE_WRITE_NOT_PERMITTED                   = 0x07,  /*!< @if Eng prohibit writing
                                                                        @else   服务端禁止客户端写入值 @endif */
    BS_SSAP_ERRCODE_INSUFFICIENT_AUTHENTICATION           = 0x08,  /*!< @if Eng client no authentication
                                                                        @else   客户端未经过认证 @endif */
    BS_SSAP_ERRCODE_INSUFFICIENT_AUTHORIZATION            = 0x09,  /*!< @if Eng client no authorizaion
                                                                        @else   客户端未被授权 @endif */
    BS_SSAP_ERRCODE_INSUFFICIENT_ENCRYPTION               = 0x0A,  /*!< @if Eng bearer not encrypted
                                                                        @else   传输 PDU 的承载未加密 @endif */
    BS_SSAP_ERRCODE_ATTRIBUTE_NOT_FOUND                   = 0x0B,  /*!< @if Eng entries not found
                                                                        @else   服务端未找到对应条目 @endif */
    BS_SSAP_ERRCODE_INVALID_METHOD_CALL                   = 0x0C,  /*!< @if Eng method access error
                                                                        @else   方法访问错误 @endif */
    BS_SSAP_ERRCODE_INVALID_ATTRIBUTE_VALUE_TYPE          = 0x0D,  /*!< @if Eng data type incorrected
                                                                        @else   客户端发送写入数据类型
                                                                                不符的错误 @endif */
    BS_SSAP_ERRCODE_INVALID_ATTRIBUTE_VALUE_LENGTH        = 0x0E,  /*!< @if Eng data value length incorrected
                                                                        @else   客户端发送写入值长度不符的错误 @endif */
    BS_SSAP_ERRCODE_OUT_OF_RANGE                          = 0x0F,  /*!< @if Eng data value incorrected
                                                                        @else   客户端写入的值超出范围 @endif */
    BS_SSAP_ERRCODE_SERVER_SUBCONTRACT_NOT_SUPPORTED      = 0x10,  /*!< @if Eng subcontract not support
                                                                        @else   服务端不支持信令的分包发送 @endif */
    BS_SSAP_ERRCODE_ATTRIBUTE_NUM_OVER_LIMIT              = 0x11,  /*!< @if Eng attribute number over limit
                                                                        @else   请求数量超出服务端的支持能力 @endif */
    BS_SSAP_ERRCODE_SERVER_FRAGMENT_NUMBER_NOT_SUPPORTED  = 0x12,  /*!< @if Eng fragment number not support
                                                                        @else   服务端不支持信令分片序号功能 @endif */
    BS_SSAP_ERRCODE_SERVER_SUBCONTRACT_NOT_FULLY_RECEIVED = 0x13,  /*!< @if Eng subcontract not fully receive
                                                                        @else   服务端未接收所有分包 @endif */
    BS_SSAP_ERRCODE_SERVER_TRANSACTION_NUM_NOT_SUPPORTED  = 0x14,  /*!< @if Eng transaction number not support
                                                                        @else   服务端不支持消息事务号 @endif */
    BS_SSAP_ERRCODE_SERVER_RELIABLE_MODE_NOT_SUPPORTED    = 0x15,  /*!< @if Eng reliable mode not support
                                                                        @else   服务端不支持可靠模式 @endif */
} bs_sle_ssap_errcode_t;

/**
 * @if Eng
 * @brief  sle UUID.
 * @else
 * @brief  星闪 通用唯一识别码。
 * @endif
 */
typedef struct {
    uint8_t len;                    /*!< @if Eng uuid len
                                         @else   UUID 长度 @endif */
    uint8_t uuid[BS_SLE_UUID_LEN];  /*!< @if Eng uuid
                                         @else   UUID字段 @endif */
} bs_sle_uuid_t;

#ifdef __cplusplus
}
#endif
#endif /* SSF_SSAP_STRU_H */