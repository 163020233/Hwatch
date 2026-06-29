
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Init API File.
 */

#ifndef SSF_PORT_H
#define SSF_PORT_H

#include <stdint.h>
#include "sle_ssap_stru.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Sle port Channel return status.
 * @else
 * @brief  星闪 port 通道返回状态。
 * @endif
 */
typedef enum {
    BS_SLE_PORT_CONN_SUCC = 0,         /*!< @if Eng transmission channel connection is established successfully.
                                                @else  传输通道连接建立成功。 @endif */
    BS_SLE_PORT_CONN_FAIL,             /*!< @if Eng transmission channel connection is established failed.
                                                @else  传输通道连接建立失败。 @endif */
    BS_SLE_PORT_DISCONN_SUCC,          /*!< @if Eng transmission channel connection was released successfully.
                                                @else  传输通道连接释放成功。 @endif */
    BS_SLE_PORT_DISCONN_FAIL,          /*!< @if Eng transmission channel connection was released failed.
                                                @else  传输通道连接释放失败。 @endif */
} bs_sle_port_channel_status_t;

/**
 * @if Eng
 * @brief  Sle port Transport connection basic information.
 * @else
 * @brief  星闪 port 端口发现模式。
 * @endif
 */
typedef enum {
    BS_SLE_PORT_STATIC_GENERATE = 0,        /*!< @if Eng transmission channel connection is established successfully.
                                                @else  静态指定端口号。 @endif */
    BS_SLE_PORT_AUTO_GENERATE,              /*!< @if Eng transmission channel connection is established failed.
                                                @else  动态协商生成端口号。 @endif */
} bs_sle_port_auto_generate_port_flag;

/**
 * @if Eng
 * @brief  Sle port Transport connection basic information.
 * @else
 * @brief  星闪 port 端口分配模式。
 * @endif
 */
typedef enum {
    BS_SLE_PORT_STATIC_REMOTE = 0,           /*!< @if Eng transmission channel connection is established successfully.
                                                @else  对端端口号已固定分配 @endif */
    BS_SLE_PORT_AUTO_REMOTE,                /*!< @if Eng transmission channel connection is established failed.
                                                @else  通过服务发现获取对端打开端口号。 @endif */
} bs_sle_port_find_remote_flag;

typedef struct {
    uint16_t uuid_num;
    sle_uuid_t *uuid_info_list;
} bs_sle_port_remote_uuid_info_t;

/**
 * @if Eng
 * @brief  Sle port Transport connection basic information.
 * @else
 * @brief  星闪 port 链路基础信息。
 * @endif
 */
typedef struct {
    uint16_t conn_id;            /*!< @if Eng Connection ID.
                                      @else   链路ID。 @endif */
    uint16_t src_port;           /*!< @if Eng Source port.
                                      @else   源端口。 @endif */
    uint16_t dst_port;           /*!< @if Eng Destination port.
                                      @else   目的端口。 @endif */
} bs_sle_port_conn_param_t;

/**
 * @if Eng
 * @brief  Callback invoked when channel status change.
 * @par Description:
 *           Callback invoked when channel status change.
 * @param  [in]  dev_id device id
 * @param  [in]  src_port the src port of local
 * @param  [in]  dst_port  the dst port of remote
 * @param  [in]  state the channel status
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @else
 * @brief  通道状态变化通知。
 * @par 说明:通道状态变化通知。
 * @param  [in]  dev_id 设备id
 * @param  [in]  src_port 本端端口号
 * @param  [in]  dst_port  对端端口号
 * @param  [in]  state 通道状态
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @endif
 */
typedef void (*bs_sle_port_conn_state_changed_callback)(
    uint16_t dev_id, uint16_t src_port, uint16_t dst_port, uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when channel receive data.
 * @par Description:
 *           Callback invoked when channel receive data.
 * @param  [in]  dev_id device id
 * @param  [in]  src_port source port number
 * @param  [in]  dst_port destination port number
 * @param  [in]  data  the data buf
 * @param  [in]  data_len the data length
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @else
 * @brief  通道接收报文通知函数。
 * @par 说明:通道接收报文通知函数。
 * @param  [in]  dev_id 设备id
 * @param  [in]  src_port 发送端端口号
 * @param  [in]  dst_port 接收端端口号
 * @param  [in]  data  数据缓冲区
 * @param  [in]  data_len 数据长度
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @endif
 */
typedef void (*bs_sle_port_receive_data_callback)(
    uint16_t dev_id, uint16_t src_port, int16_t dst_port, const uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief  Callback invoked when dynamic transmission status changes.
 * @par Description:
 *           Callback invoked when dynamic transmission status changes.
 * @param  [in]  tc_handle port handle
 * @param  [in]  data_header  The connection parameters of the transmission channel
 * @param  [in]  data  The data buffer
 * @param  [in]  data_len  The length of the data buffer
 * @retval #void  No return value.
 * @par Dependency:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @else
 * @brief  动态传输状态变化回调函数。
 * @par 说明: 动态传输状态变化回调函数。
 * @param  [in]  tc_handle 通道handle
 * @param  [in]  data_header  传输通道的连接参数
 * @param  [in]  data  数据缓冲区
 * @param  [in]  data_len  数据缓冲区长度
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_sle_port_event_cbk_t
 * @endif
 */
typedef void (*bs_sle_port_dynamic_trans_status_callback)(uint8_t tc_handle,
    const bs_sle_port_conn_param_t *data_header, const uint8_t *data, uint16_t data_len);

typedef struct {
    bs_sle_port_receive_data_callback receive_data_cbk;               // 接收数据回调
    bs_sle_port_conn_state_changed_callback channel_status_cbk;       // 连接状态变化回调
    bs_sle_port_dynamic_trans_status_callback trans_data_status_cbk;  // 有连接发送结果通知回调
} bs_sle_port_event_cbk_t;

/**
 * @if Eng
 * @brief Callback for UUID list retrieval.
 * @par Description:
 *           This callback is invoked when the UUID list retrieval is completed.
 *           It provides the result of the operation and the UUID information.
 * @param  [in]  reason The result of the UUID retrieval operation.
 * @param  [in]  uuid_info The UUID information, including the UUID list.
 * @retval #void  No return value.
 * @par Dependency:
 * @li None
 * @else
 * @brief UUID列表获取回调函数。
 * @par 说明:当UUID列表获取完成时，调用此回调函数。它提供操作结果和UUID信息。
 * @param  [in]  reason UUID获取操作的结果。
 * @param  [in]  uuid_info UUID信息，包括UUID列表。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li 无
 * @endif
 */
typedef void (*bs_sle_port_get_uuid_info_callback)(uint16_t dev_id,
    errcode_t reason, const bs_sle_port_remote_uuid_info_t* uuid_info);

typedef struct {
    bs_sle_port_get_uuid_info_callback get_uuid_info_cbk;   // 获取uuidlist回调
} ssf_port_uuid_info_cbk_t;

/**
 * @if Eng
 * @brief  Callback invoked when port enable.
 * @par Description:
 *           Callback invoked when port enable.
 * @param  [in]  status enable result
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @else
 * @brief  端口服务打开结果回调通知。
 * @par 说明:端口服务打开结果回调通知。
 * @param  [in] status 使能结果
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @endif
 */
typedef void (*bs_sle_port_enable_callback)(errcode_t status);

/**
 * @if Eng
 * @brief  Enable port profile.
 * @par Description: Initialize port profile.
 * @retval error code.
 * @par Depends:
 * @attention server/Client must act this interface
 * @li None
 * @else
 * @brief  使能port能力
 * @par Description: 使能port能力
 * @retval 执行结果错误码。
 * @par 依赖：
 * @attention 服务端客户端都必须要调用
 * @li 无
 * @endif
 */
errcode_t bs_sle_port_enable(bs_sle_port_enable_callback cbk);

/**
 * @if Eng
 * @brief  Create local port.
 * @par Description: Create local port.
 * @param  [in]  uuid          the default application uuid.
 * @param  [in]  manufacturer_id       Alloc for port manufacturer_id
 * @param  [in]  auto_generate_port_flag       auto or static discover port
 * @param  [out] port            Dynamically assigned port.
 * @retval error code.
 * @par Depends:
 * @attention server/Client must act this interface
 * @li None
 * @else
 * @brief  分配port。
 * @par Description: 分配port。
 * @param  [in]  uuid           默认传入应用uuid。
 * @param  [in]  manufacturer_id   厂商标识
 * @param  [in]  auto_generate_port_flag       1:自动分配端口或者 0:指定端口
 * @param  [out] port          动态分配端口号。
 * @retval 执行结果错误码。
 * @par 依赖：
 *  @attention 服务端客户端使用端口传输必须调用
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_create_local_port(
    sle_uuid_t uuid, uint16_t manufacturer_id, uint8_t auto_generate_port_flag, uint16_t *port);

/**
 * @if Eng
 * @brief  Register port manager callbacks.
 * @par Description: Register port manager callbacks.
 * @param [in] port The port bound to the port management callback.
 * @param [in] func Callback function.
 * @retval error code.
 * @par Depends:
 * @attention server/Client must act this interface
 * @li  sle_common.h
 * @else
 * @brief  注册port 管理回调函数。
 * @par Description: 注册port 管理回调函数。
 * @param [in] port 跟port管理回调绑定的端口。
 * @param [in] func port管理回调函数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @attention 客户端和服务端必须注册
 * @li  sle_common.h
 * @endif
 */
errcode_t bs_sle_port_register_callbacks(uint16_t port, bs_sle_port_event_cbk_t cbk);

/**
 * @if Eng
 * @brief  create port based on uuid.
 * @par Description: create port based on uuid.
 * @param  [in]  uuid        search UUID, standard or private.
 * @param  [in]  port        remote port.
 * @retval error code.
 * @par Depends:
 * @attention must use bs_sle_port_create_local_port before this
 * @li none
 * @else
 * @brief  根据uuid创建对端port。
 * @par Description: 根据uuid创建对端port。
 * @param  [in]  uuid        查询输入的uuid。
 * @param  [in]  port      远端端口.
 * @retval 执行结果错误码。
 * @par 依赖：
 * @attention 调用此接口前必须使用bs_sle_port_create_local_port 通过静态模式创建本端端口
 *            服务端客户端都可使用
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_create_remote_port(sle_uuid_t uuid, uint16_t port);

// 连理连接，建立传输通道
typedef enum {
    SLE_CHANNEL_BASIC_MODE = 0,    // 基础模式
    SLE_CHANNEL_STREAM_MODE,       // 流模式，暂时不支持
    SLE_CHANNEL_TRANSPARENT_MODE,  // 透传模式，暂时不支持
    SLE_CHANNEL_RELIABLE_MODE,     // 可靠模式
} bs_sle_channel_trans_mode_t;

/**
 * @if Eng
 * @brief  connect server.
 * @par Description: Discover port services and read properties and connect remote.
 * @param  [in]  dev_id    dev_id
 * @param  [in]  uuid  search UUID, standard or private.
 * @param  [in]  find_remote_flag      auto discover remote port.
 * @param  [in]  trans_mode      trans_mode.
 * @retval error code, The service discovery result will be returned in { @ref sle_port_property_callback }.
 * @par Depends:
 * @attention if find_remote_flag = 0, this means must config remote port by bs_sle_port_create_remote_port
 * @li none
 * @else
 * @brief 连接对端。
 * @par Description: 连接对端。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  uuid    应用uuid。
 * @param  [in]  find_remote_flag = 1表示可以自动发现port服务, 在连接接口里面自动创建通道;
 * false表示本对端端口号是静态获取的，需要UX配置下来
 * @param  [in]  trans_mode   传输模式，只支持基础和可靠模式
 * @retval 执行结果错误码，服务发现结果将在 { @ref sle_port_property_callback }中返回。
 * @par 依赖：
 * @attention find_remote_flag = 0属于静态分配模式，这种情况下需要先通过bs_sle_port_create_remote_port完成端口号配置
 *               客户端使用接口
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_connect(
    uint16_t dev_id, sle_uuid_t uuid, uint8_t find_remote_flag, bs_sle_channel_trans_mode_t trans_mode);

/**
 * @if Eng
 * @brief  Send data.
 * @par Description: Send data.
 * @param  [in]  dev_id     dev_id
 * @param  [in]  uuid   uuid
 * @param  [in]  data   data sent.
 * @param  [in] data_len data length sent.
 * @retval error code.
 * @par Depends:
 * @attention  None
 * @li none
 * @else
 * @brief  发送数据。
 * @par Description: 发送数据。
 * @param  [in]  dev_id    dev_id
 * @param  [in]  uuid     应用标识
 * @param  [in]  data       发送的数据。
 * @param  [in]  data_len   数据长度。
 * @retval 执行结果错误码。z
 * @par 依赖：
 * @attention 无
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_write_data_by_uuid(uint16_t dev_id, sle_uuid_t *uuid, uint16_t data_len, uint8_t *data);

/**
 * @if Eng
 * @brief  Send data.
 * @par Description: Send data.
 * @param  [in]  dev_id     dev_id
 * @param  [in]  src_port   source port
 * @param  [in]  dst_port   dst port.
 * @param  [in]  data   data sent.
 * @param  [in] data_len data length sent.
 * @retval error code.
 * @par Depends:
 * @attention  None
 * @li none
 * @else
 * @brief  发送数据。
 * @par Description: 发送数据。
 * @param  [in]  dev_id    dev_id
 * @param  [in]  src_port  本端端口
 * @param  [in]  dst_port   对端端口
 * @param  [in]  data       发送的数据。
 * @param  [in]  data_len   数据长度。
 * @retval 执行结果错误码。z
 * @par 依赖：
 * @attention 无
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_write_data_by_port(
    uint16_t dev_id, uint16_t src_port, uint16_t dst_port, uint16_t data_len, uint8_t *data);

/**
 * @if Eng
 * @brief  disconnect port.
 * @par Description: disconnect port.
 * @param  [in]  dev_id     dev_id
 * @param  [in]  uuid   uuid.
 * @retval error code.
 * @par Depends:
 * @attention when need to trans data after act this interface, need connect remote by bs_sle_port_connect
 * @li none
 * @else
 * @brief  根据UUID断开通道。
 * @par Description: 根据UUID断开通道。
 * @param  [in]  dev_id    dev_id
 * @param  [in]  uuid  服务ID
 * @retval 执行结果错误码。
 * @par 依赖：
 * @attention 调用完本接口后如果再想传输数据，不需要再创建本端端口号但是需要再次connect对端
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_disconnect_by_uuid(uint16_t dev_id, sle_uuid_t *uuid);

/**
 * @if Eng
 * @brief  disconnect port.
 * @par Description: disconnect port.
 * @param  [in]  dev_id     dev_id
 * @param  [in]  src_port   source port
 * @param  [in]  dst_port   dst port.
 * @retval error code.
 * @par Depends:
 * @attention when need to trans data after act this interface, need connect remote by bs_sle_port_connect
 * @li none
 * @else
 * @brief  根据本端端口号断开通道
 * @par Description: 根据本端端口号断开通道
 * @param  [in]  dev_id    dev_id
 * @param  [in]  src_port  本端端口
 * @param  [in]  dst_port   对端端口
 * @retval 执行结果错误码。
 * @par 依赖：
 * @attention 调用完本接口后如果再想传输数据，不需要再创建本端端口号但是需要再次connect对端
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_disconnect_by_port(uint16_t dev_id, uint16_t src_port, uint16_t dst_port);

/**
 * @if Eng
 * @brief  destroy port by port.
 * @par Description: destroy port by port.
 * @param  [in]  port     local port
 * @retval error code.
 * @par Depends:
 * @attention when need to trans data after act this interface, need realloc local poert and connect remote
 * @li none
 * @else
 * @brief  根据端口号销毁通道
 * @par Description: 根据端口号销毁通道
 * @param  [in]  port  本端端口
 * @retval 执行结果错误码。如果指定的端口不存在，也会返回成功。
 * @par 依赖：
 * @attention 使用本接口后需要重新申请本对端端口并建立连接
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_destroy_by_port(uint16_t port);

/**
 * @if Eng
 * @brief  destroy port by uuid.
 * @par Description: destroy port by uuid.
 * @param  [in]  uuid     uuid
 * @retval error code.
 * @par Depends:
 * @attention when need to trans data after act this interface, need realloc local poert and connect remote
 * @li none
 * @else
 * @brief  根据UUID销毁通道
 * @par Description: 根据UUID销毁通道
 * @param  [in]  uuid  search UUID, standard or private.
 * @retval 执行结果错误码。如果指定的uuid不存在，也会返回成功。
 * @par 依赖：
 * @attention 使用本接口后需要重新申请本对端端口并建立连接
 * @li  无
 * @endif
 */
errcode_t bs_sle_port_destroy_by_uuid(sle_uuid_t *uuid);

/**
 * @if Eng
 * @brief Retrieves the UUID list of the remote device.
 * @par Description:
 *           This function is used to asynchronously retrieve the UUID list of a remote device
 *           based on the provided device ID. The result is returned through the provided callback function.
 * @param  [in]  dev_id The device ID of the remote device.
 * @param  [in]  uuid_cbk The callback function for handling the UUID list retrieval result.
 * @retval #ERRCODE_SLE_SUCCESS  The UUID retrieval request was successful.
 * @retval #ERRCODE_SLE_POINTER_NULL  The callback function is null.
 * @retval #ERRCODE_SLE_FAIL  The UUID retrieval request failed.
 * @par Dependency:
 * @li The callback function must be valid and non-null.
 * @else
 * @brief 获取远程设备的UUID列表。
 * @par 说明:此函数用于异步获取指定设备ID的远程设备的UUID列表。结果通过提供的回调函数返回。
 * @param  [in]  dev_id 远程设备的设备ID。
 * @param  [in]  uuid_cbk 处理UUID列表获取结果的回调函数。
 * @retval #ERRCODE_SLE_SUCCESS  UUID获取请求成功。
 * @retval #ERRCODE_SLE_POINTER_NULL  回调函数为空。
 * @retval #ERRCODE_SLE_FAIL  UUID获取请求失败。
 * @par 依赖:
 * @li 回调函数必须有效且非空。
 * @endif
 */
errcode_t bs_sle_port_get_remote_uuid_info_by_conn_id(uint16_t dev_id, ssf_port_uuid_info_cbk_t *uuid_cbk);

#ifdef __cplusplus
}
#endif
#endif /* SSF_PORT_H */