/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: SLE video trans. \n
 * Author: @CompanyNameTag \n
 * History: add new file \n
 * 2025-10-25, Create file. \n
 */

#ifndef SLE_DATA_TRANS_H
#define SLE_DATA_TRANS_H

#include "sle_common.h"
#include "bts_def.h"
#include "sle_errcode.h"

#ifndef unused
#define unused(P)          \
    {                           \
        (void)(P);              \
    }
#endif

#define INDEX_0 0
#define INDEX_4 4
#define INDEX_5 5

/**
 * @if Eng
 * @brief Definitaion of SLE announce general data structure.
 * @else
 * @brief SLE 广播通用数据结构。
 * @endif
 */
struct sle_adv_common_value_t {
    uint8_t length;
    uint8_t type;
    uint8_t value;
};

/**
 * @if Eng
 * @brief Definitaion of BLE ADV Channel mapping.
 * @else
 * @brief SLE 广播信道映射。
 * @endif
 */
typedef enum {
    SLE_ADV_CHANNEL_MAP_77                 = 0x01,
    SLE_ADV_CHANNEL_MAP_78                 = 0x02,
    SLE_ADV_CHANNEL_MAP_79                 = 0x04,
    SLE_ADV_CHANNEL_MAP_DEFAULT            = 0x07
} sle_adv_channel_map_t;

/**
 * @if Eng
 * @brief Definitaion of SLE ADV Data Type.
 * @else
 * @brief SLE 广播数据类型
 * @endif
 */
typedef enum {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL                              = 0x01,   /*!< 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE                                  = 0x02,   /*!< 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID                      = 0x03,   /*!< 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID                     = 0x04,   /*!< 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS         = 0x05,   /*!< 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS        = 0x06,   /*!< 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS       = 0x07,   /*!< 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS      = 0x08,   /*!< 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE                 = 0x09,   /*!< 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME                         = 0x0A,   /*!< 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME                          = 0x0B,   /*!< 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL                               = 0x0C,   /*!< 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN                     = 0x0D,   /*!< SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID                    = 0x0E,   /*!< SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED                                     = 0xFE,   /*!< 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA                   = 0xFF    /*!< 厂商自定义信息 */
} sle_adv_data_type_t;

/**
 * @if Eng
 * @brief Definitaion of SLE Transmission link status Type.
 * @else
 * @brief SLE 传输链路状态类型
 * @endif
 */
typedef enum {
    SLE_TRANSMISSION_LINK_READY = 0,        /*!< @if Eng Transmission link is ready.
                                                 @else   传输链路已就绪。 @endif */
    SLE_TRANSMISSION_LINK_IDLE  = 1,        /*!< @if Eng Transmission link is not ready.
                                                 @else   传输链路未就绪。 @endif */
    SLE_TRANSMISSION_LINK_BUSY  = 2,        /*!< @if Eng Transmission link is busy.
                                                 @else   传输链路繁忙。 @endif */
} sle_transmission_link_status_t;

/**
 * @if Eng
 * @brief Callback invoked when Transmission link status change.
 * @par Callback invoked when Transmission link status change.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param  [in]  status Transmission link status code.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_receiver_callback_t
 * @else
 * @brief  传输链路状态回调函数。
 * @par    传输链路状态回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param  [in]  status 传输链路状态码。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_receiver_callback_t
 * @endif
 */
typedef void (*sle_link_status_callback)(sle_transmission_link_status_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive data.
 * @par Callback invoked when receive data.
 * @attention 1.This function is called in SLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the SLE service automatically.
 * @param  [in]  data Received data.
 * @param  [in]  len  Length of the received data.
 * @par Dependency:
 * @li  sle_common.h
 * @see sle_receiver_callback_t
 * @else
 * @brief  接收数据回调函数。
 * @par    接收数据回调函数。
 * @attention  1. 该回调函数运行于SLE service线程，不能阻塞或长时间等待。
 * @attention  2. 指针由SLE service申请内存，也由SLE service释放，回调中不应释放。
 * @param  [in]  data 收到的数据。
 * @param  [in]  len  收到数据的长度。
 * @par 依赖:
 * @li  sle_common.h
 * @see sle_receiver_callback_t
 * @endif
 */
typedef void (*sle_port_receive_data_cbk)(uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief Struct of SLE data receiver callback function.
 * @else
 * @brief SLE 数据接收端回调函数接口定义。
 * @endif
 */
typedef struct {
    sle_link_status_callback link_status_cb;            /*!< @if Eng Transmission link status callback.
                                                             @else   传输链路状态回调函数。 @endif */
    sle_port_receive_data_cbk receive_data_cb;          /*!< @if Eng Receive data callback.
                                                             @else   接收数据回调函数。 @endif */
} sle_receiver_callback_t;

/**
 * @if Eng
 * @brief  Register SLE data receiver callbacks.
 * @par Description: Register SLE data receiver callbacks.
 * @param  [in]  func Callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册SLE数据接收端回调。
 * @par Description: 注册SLE数据接收端回调。
 * @param  [in]  func 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_receiver_reg_callback(sle_receiver_callback_t *func);

/**
 * @if Eng
 * @brief  Initialize receiver.
 * @par Description: Initialize receiver.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  初始化接收端。
 * @par Description: 初始化接收端。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_receiver_init(void);

/**
 * @if Eng
 * @brief  Deinitializes receiver.
 * @par Description: Deinitializes receiver.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  去初始化接收端。
 * @par Description: 去初始化接收端。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_receiver_deinit(void);

/**
 * @if Eng
 * @brief  Register SLE data transmitter callbacks.
 * @par Description: Register SLE data transmitter callbacks.
 * @param  [in]  func Callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册SLE数据发送端回调。
 * @par Description: 注册SLE数据发送端回调。
 * @param  [in]  func 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t sle_transmitter_reg_callback(sle_link_status_callback func);

/**
 * @if Eng
 * @brief  Initialize transmitter.
 * @par Description: Initialize transmitter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  初始化发送端。
 * @par Description: 初始化发送端。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_transmitter_init(void);

/**
 * @if Eng
 * @brief  Deinitializes transmitter.
 * @par Description: Deinitializes transmitter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  去初始化发送端。
 * @par Description: 去初始化发送端。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_transmitter_deinit(void);

/**
 * @if Eng
 * @brief  SLE transmitter send data function.
 * @par Description: SLE transmitter send data function.
 * @param [in] data Configuration parameters, corresponding configuration types
 * @param [in] type Config type { @ref sle_config_type_t }.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  SLE设备管理设置配置函数。
 * @par Description: SLE设备管理设置配置函数。
 * @param [in] data 配置参数，对应配置类型：
 * @param [in] type 配置类型 { @ref sle_config_type_t }。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
errcode_t sle_transmitter_send_data(uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Register SLE data Transmission AT command.
 * @par Description: Register SLE data Transmission AT command.
 * @par Depends:
 * @li sle_common.h
 * @else
 * @brief  注册SLE数据传输AT命令。
 * @par Description: 注册SLE数据传输AT命令。
 * @par 依赖：
 * @li sle_common.h
 * @endif
 */
void at_sle_data_trans_cmd_register(void);
#endif