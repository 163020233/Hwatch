/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT SPP module.
 */

/**
 * @defgroup bluetooth_bts_spp SPP API
 * @ingroup  bluetooth
 * @{
 */

#ifndef BTS_SPP_H
#define BTS_SPP_H

#include <stdbool.h>
#include "bts_def.h"
#include "bts_br_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Invalid SPP server/client PORT.
 * @else
 * @brief  非法SPP Server 或 Client PORT。
 * @endif
 */
#define SPP_INVALID_PORT 0

/**
 * @if Eng
 * @brief  Max len of the svr name.
 * @else
 * @brief  svr name 长度最大值。
 * @endif
 */
#define SPP_SER_NAME_MAX 24

/**
 * @if Eng
 * @brief  Enum of SPP socket type.
 * @else
 * @brief  SPP socket类型定义枚举。
 * @endif
 */
typedef enum {
    SPP_SOCKET_RFCOMM = 0x0,    /*!< @if Eng RFCOMM
                                     @else   RFCOMM @endif */
} spp_socket_type_t;

/**
 * @if Eng
 * @brief  Parameters for create SPP socket.
 * @else
 * @brief  创建SPP socket的参数。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;                /*!< @if Eng UUID of service.
                                        @else   SPP socket对应的服务UUID。 @endif */
    spp_socket_type_t socket_type; /*!< @if Eng SPP socket type
                                        @else SPP socket类型。@endif */
    bool is_encrypt;               /*!< @if Eng is encrypted (not use now, encrypted default)
                                        @else 是否加密（目前此参数未使用，默认加密）。@endif */
} spp_create_socket_para_t;

/**
 * @if Eng
 * @brief  Creates an server listening socket based on the service record.
 * @attention NULL.
 * @param  [in]  socket_para The parameters to create a server socket.
 * @param  [in]  name       The service's name.
 * @param  [in]  len        The length of the service's name, len = strlen(name) + 1 max is SPP_SER_NAME_MAX.
 * @retval SPP_INVALID_ID create server fail.
 * @retval others create server success, return a port num.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_disconnect | spp_server_close
 * @else
 * @brief  创建SPP Server。
 * @attention NULL。
 * @param  [in]  socket_para 入参，SPP Server创建参数 。
 * @param  [in]  name 入参，SPP Server 名称。
 * @param  [in]  len 入参，SPP Server 名称长度，取值为strlen(name) + 1 最大值是 SPP_SER_NAME_MAX 。
 * @retval SPP_INVALID_ID   创建server失败。
 * @retval others 创建Server成功，返回port。
 * @par 依赖:
 * @li bts_spp.h
 * @see  spp_disconnect | spp_server_close
 * @endif
 */
uint16_t spp_server_create(spp_create_socket_para_t *socket_para, const char *name, unsigned int len);

/**
 * @if Eng
 * @brief  Disables an spp server socket and releases related resources.
 * @attention At the same time, the SDP database is deregistered.
              It is recommended that the server be disconnected and then closed.
              Otherwise, the disconnection status is not reported.
 * @param  [in]  port  The port number of the SPP server to close.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create
 * @else
 * @brief  关闭Server（同时释放相关资源）。
 * @attention 同时会去注册sdp数据库.对于server端，建议先断连后关闭，不然没有断连状态上报。
 * @param  [in]  port 入参，要关闭的SPP服务器的端口号。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create
 * @endif
 */
errcode_t spp_server_close(uint16_t port);

/**
 * @if Eng
 * @brief  Connects to a remote device over the socket.
 * @attention Check whether the connection is successful through callback. See @ref spp_conn_state_changed_callback.
 * @param  [in]  socket_para The params to create a client socket and connect to a remote device.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_disconnect | spp_write | spp_conn_state_changed_callback
 * @else
 * @brief  连接远端设备。
 * @attention 请通过回调确认是否连接成功 See @ref spp_conn_state_changed_callback。
 * @param  [in]  socket_para 入参，用于创建Client socket并连接远端设备的参数。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_disconnect | spp_write | spp_conn_state_changed_callback
 * @endif
 */
errcode_t spp_connect(spp_create_socket_para_t *socket_para, const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Disables a connection and releases related resources.
 * @attention NULL.
 * @param  [in]  port  The port number of the SPP connection to disconnect.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_conn_state_changed_callback
 * @else
 * @brief  断开连接（同时释放相关资源）。
 * @attention NULL。
 * @param  [in]  port 入参，要断开的SPP连接的端口号。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_conn_state_changed_callback
 * @endif
 */
errcode_t spp_disconnect(uint16_t port);

/**
 * @if Eng
 * @brief  Get the connection status of this socket.
 * @attention NULL.
 * @param  [in]  port The relative ID used to identify the current port socket.
 * @retval true connected.
 * @retval false disconnected.
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_disconnect
 * @else
 * @brief  获取指定SPP socket的连接状态。
 * @attention NULL.
 * @param  [in]  port 入参，用来指定当前操作port的ID。
 * @retval true 连接。
 * @retval false 未连接。
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_disconnect
 * @endif
 */
bool spp_is_connected(uint16_t port);

/**
 * @if Eng
 * @brief  Client write data to socket.
 * @attention NULL.
 * @param  [in]  port  The port number of the SPP connection.
 * @param  [in]  data  Indicate the data to be written.
 * @param  [in]  len   Indicates the length of the data to be written.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_disconnect | spp_conn_state_changed_callback
 * @else
 * @brief  向SPP socket写入数据
 * @attention NULL.
 * @param  [in]  port 入参，SPP连接的端口号。
 * @param  [in]  data 入参，存储待写入数据的buf。
 * @param  [in]  len 入参，待写入数据的长度。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create | spp_connect | spp_disconnect | spp_conn_state_changed_callback
 * @endif
 */
errcode_t spp_write(uint16_t port, const uint8_t *data, const uint32_t len);

/**
 * @if Eng
 * @brief  Send a connection response to a remote device, indicating whether to accept or reject the connection request.
 * @attention NULL.
 * @param  [in]  port         The port number of the SPP connection to response.
 * @param  [in]  is_accept    Indicates whether to accept the connection request (1 for accept, 0 for reject).
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see spp_server_create | spp_receive_connection_request_callback | spp_conn_state_changed_callback
 * @else
 * @brief  向远程设备发送连接响应，指示是否接受或拒绝连接请求
 * @attention NULL.
 * @param  [in]  port         要回复的SPP连接的端口号。
 * @param  [in]  is_accept    指示是否接受连接请求（1表示接受，0表示拒绝）。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see spp_server_create | spp_receive_connection_request_callback | spp_conn_state_changed_callback
 * @endif
 */
errcode_t spp_send_conn_rsp(uint16_t port, uint8_t is_accept);

/**
 * @if Eng
 * @brief Callback function for connection event.
 * @par Description: When registered, this callback reports connection event to upper layer application.
    If port is set to SPP_INVALID_CHANNEL, the connection fails to be initiated.
 * @param  [in]  conn_port The conn port used to identify the current socket port.
 * @param  [in]  state    Connect state of spp. See {@link profile_connect_state_t}
 * @retval void  No return value.
 * @par Dependency:
 *            @li bts_spp.h
 * @see spp_client_callbacks_t
 * @else
 * @brief 连接变化事件回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将连接事件上报给上层。当port为SPP_INVALID_CHANNEL代表发起连接失败。
 * @param  [in]  conn_port 入参，用来指定当前操作port。
 * @param  [in]  state    spp的连接状态。见 {@link profile_connect_state_t}
 * @retval void  无返回值。
 * @par 依赖:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @endif
 */
typedef void (*spp_conn_state_changed_callback)(bd_addr_t *bd_addr, uint16_t port, uint8_t state);

/**
 * @if Eng
 * @brief Callback function for data reception.
 * @par Description: When registered, this callback reports received data to upper layer application.
 * @param  [in]  port     The conn port used to identify the current socket port.
 * @param  [in]  data     Received data.
 * @param  [in]  data_len      Length of data.
 * @retval void  No return value.
 * @par Dependency:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @else
 * @brief 接收到数据回调函数。
 * @par 说明: 注册该回调函数之后，bluetooth调用该回调函数将接收到的数据上报给上层。
 * @param  [in]  port     入参，用来指定当前操作port。
 * @param  [in]  data     接收到的数据。
 * @param  [in]  data_len      数据长度。
 * @retval void  无返回值。
 * @par 依赖:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @endif
 */
typedef void (*spp_receive_data_callback)(bd_addr_t *bd_addr, uint16_t port, uint8_t *data, uint32_t data_len);

/**
 * @if Eng
 * @brief Callback function for receiving connection request.
 * @par Description: When registered, this callback notifies the upper-layer application
        about an incoming connection request.
 * @param  [in]  conn_port The conn port used to identify the current socket port.
 * @retval void  No return value.
 * @par Dependency:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @else
 * @brief 接收连接请求的回调函数。
 * @par 说明: 注册该回调函数后，当有连接请求到来时，通知上层应用。
 * @param  [in]  conn_port 入参，用来指定当前操作port。
 * @retval void  无返回值。
 * @par 依赖:
 *            @li bts_spp.h
 * @see spp_callbacks_t
 * @endif
 */
typedef void (*spp_receive_connection_request_callback)(bd_addr_t *bd_addr, uint16_t port);

/**
 * @if Eng
 * @brief Struct of SPP callback functions.
 * @else
 * @brief SPP回调函数接口定义。
 * @endif
 */
typedef struct {
    spp_receive_data_callback receive_data_cb;
    spp_conn_state_changed_callback conn_state_changed_cb;
    spp_receive_connection_request_callback receive_conn_req_cb;
} spp_callbacks_t;

/**
 * @if Eng
 * @brief Register callback functions for PBAP.
 * @par Description: Register callback functions for PBAP.
 * @attention  1. This function is called in bts context,should not be blocked or do long time waiting.
 * @attention  2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  func  A poniter of the Callback funcs.See @ref spp_client_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code. See @ref errcode_bt_t
 * @see spp_callbacks_t
 * @else
 * @brief 注册SPP回调函数。
 * @par 说明: 注册上层应用的回调。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  func  指向回调函数接口定义的指针。见 @ref spp_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码。参考 @ref errcode_bt_t
 * @see spp_callbacks_t
 * @endif
 */
errcode_t spp_register_callbacks(spp_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref spp_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref spp_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 */
errcode_t spp_unregister_callbacks(spp_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  app_id The app id of the callback function. See @ref bt_callback_app_id_t
 * @param  [in]  func   A poniter of the callback function. See @ref spp_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调（支持多应用回调注册）。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  app_id   注册回调的应用id. See @ref bt_callback_app_id_t
 * @param  [in]  func   回调函数指针。参考 @ref spp_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 */
errcode_t spp_register_callbacks_multi(uint8_t app_uid, spp_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  app_id   App id of the callback function. See @ref bt_callback_app_id_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调（支持多应用回调去注册）。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  app_id   注册回调的应用id. See @ref bt_callback_app_id_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 */
errcode_t spp_unregister_callbacks_multi(uint8_t app_uid);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
