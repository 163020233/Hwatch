/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR serivce port profile api
 */

#ifndef BS_BR_SERVICE_PORT_API_H
#define BS_BR_SERVICE_PORT_API_H

#include <stdint.h>
#include "errcode.h"
#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  BR Data Transmission Maximum Data Length
 * @else
 * @brief  BR 数传发送数据长度最大值
 * @endif
 */
#define BR_PORT_SERVICE_MAX_DATA_LEN 990

/**
 * @if Eng
 * @brief Connection change event callback function.
 * @par Description: After registering this callback function, bluetooth calls this callback function to report the
    connection event to the upper layer. When port is SPP_INVALID_CHANNEL, it means that the connection failed.
 * @param  [in]  dev_id     device id.
 * @param  [in]  port       Current connection port
 * @param  [in]  state      Connect state of bs_br_port. See {@link profile_connect_state_t}
 * @retval #void no return value.
 * @par Dependency:
 * @li  bs_br_service_port_api.h
 * @else
 * @brief  连接变化事件回调函数。
 * @par    说明: 注册该回调函数之后，bluetooth调用该回调函数将连接事件上报给上层。当port为SPP_INVALID_CHANNEL代表发起连接失败。
 * @param  [in]  dev_id     设备ID.
 * @param  [in]  port       当前连接port。
 * @param  [in]  state      bs_br_port的连接状态。见 {@link profile_connect_state_t}
 * @retval 无返回值。
 * @par 依赖
 * @li  bs_br_service_port_api.h
 * @endif
 */
typedef void (*bs_br_port_conn_state_changed_callback)(uint16_t dev_id, uint16_t port, uint8_t state);

/**
 * @if Eng
 * @brief Callback function for data reception.
 * @par Description: When registered, this callback reports received data to upper layer application.
 * @param  [in]  dev_id     device id.
 * @param  [in]  port       The conn port used to identify the current socket port.
 * @param  [in]  data       Received data.
 * @param  [in]  data_len   Length of data.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bs_br_service_port_api.h
 * @else
 * @brief  接收到数据回调函数。
 * @par    说明: 注册该回调函数之后，bluetooth调用该回调函数将接收到的数据上报给上层。
 * @param  [in]  dev_id         设备ID。
 * @param  [in]  port           用来指定当前操作port。
 * @param  [in]  data           接收到的数据。
 * @param  [in]  data_len       数据长度。
 * @retval 无返回值。
 * @par 依赖:
 * @li  bs_br_service_port_api.h
 * @endif
 */
typedef void (*bs_br_port_receive_data_callback)(uint16_t dev_id, uint16_t port, uint8_t *data, uint16_t data_len);

/**
 * @if Eng
 * @brief Callback function for receiving connection request.
 * @par Description: When registered, this callback notifies the upper-layer application
        about an incoming connection request.
 * @param  [in]  dev_id Input parameter, identifying the link.
 * @param  [in]  port The conn port used to identify the current socket port.
 * @retval #void no return value.
 * @par Dependency:
 * @li  bs_br_service_port_api.h
 * @else
 * @brief 接收连接请求的回调函数。
 * @par 说明: 注册该回调函数后，当有连接请求到来时，通知上层应用。
 * @param  [in]  dev_id     设备ID。
 * @param  [in]  port       入参，用来指定当前操作port。
 * @retval #void  无返回值。
 * @par 依赖:
 * @li  bs_br_service_port_api.h
 * @endif
 */
typedef void (*bs_br_port_connection_request_callback)(uint16_t dev_id, uint16_t port);

typedef struct {
    bs_br_port_conn_state_changed_callback conn_state_cbk;         /*!< @if Eng standard info callback.
                                                                        @else bs port 连接状态回调函数。@endif */
    bs_br_port_receive_data_callback receive_data_cbk;             /*!< @if Eng standard info callback.
                                                                        @else bs port 数据接收回调函数。@endif */
    bs_br_port_connection_request_callback conn_req_cbk;           /*!< @if Eng Connection request callback function.
                                                                        @else bs port 连接请求回调函数。@endif */
} bs_br_port_callbacks_t;

/**
 * @if Eng
 * @brief  The server creates port parameter information.
 * @else
 * @brief  服务端创建port参数信息。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;         /*!< @if Eng Application UUID. Supports 2 or 16 bits.
                                    @else 应用uuid。支持2位或16位 @endif */
    char *name;             /*!< @if Eng port server name.
                                    @else  port Server名称。 @endif */
    uint8_t name_len;      /*!< @if Eng Source port server name length.
                                    @else  port Server名称长度。 @endif */
} bs_br_port_create_info_t;

/**
 * @if Eng
 * @brief Create a server port.
 * @par Create a server port.
 * @param [in] port_create_info     Create server-side port related parameters: uuid, name, name_len.
 * @param [in] channel              Fixed channel flag, used in conjunction with auto_generate_port_flag.
 * @param [in] auto_generate_port_flag Whether the port is created by the profile. If it is 0, it is allocated by
 the profile. If it is 1, the port is specified.
 * @param [in/out] port Assigned port number.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  创建服务端port。
 * @par    创建服务端port。
 * @param [in] port_create_info 创建server端port相关参数：uuid，name，name_len。
 * @param [in] channel 固定通道标识，和auto_generate_port_flag配合使用。
 * @param [in] auto_generate_port_flag 是否由profile创建port，为0时由profile分配，为1时为指定port。
 * @param [in/out] port 分配的port号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_create_server_port(bs_br_port_create_info_t *port_create_info, uint8_t channel,
    uint8_t auto_generate_port_flag, uint16_t *port);

/**
 * @if Eng
 * @brief Set the server fixed port.
 * @par Set the server fixed port.
 * @param [in] uuid Application UUID.
 * @param [in] channel Fixed channel identifier.
 * @param [in] port Fixed port number of the peer end.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  设置服务端固定port。
 * @par    设置服务端固定port。
 * @param [in] uuid 应用uuid。
 * @param [in] channel 固定通道标识。
 * @param [in] port 对端固定的port号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_create_remote_port(bt_uuid_t *uuid, uint8_t channel, uint16_t port);

/**
 * @if Eng
 * @brief Server deletes port.
 * @par Server deletes port.
 * @param [in] port The port to be deleted.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  服务端删除port。
 * @par    服务端删除port。
 * @param [in] port 待删除的的port。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_destroy(uint16_t port);

/**
 * @if Eng
 * @brief Port connection.
 * @par The service discovers and creates the client port, and returns the local port and connection status in
bs_br_port_conn_state_changed_callback after receiving the response.
 * @param [in] uuid     Application UUID.
 * @param [in] dev_id   device id.
 * @param [in] find_remote_flag find_remote_flag Whether service discovery is required for the peer port. If it is a
fixed port, service discovery is not required.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  port连接。
 * @par    port连接，服务发现并创建客户端port，收到响应后在bs_br_port_conn_state_changed_callback中返回本端port和连接状态。
 * @param [in] uuid     应用uuid。
 * @param [in] dev_id   设备id。
 * @param [in] find_remote_flag 是否需要服务发现对端port，若为固定port则不需要做服务发现。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_connect(uint16_t dev_id, bt_uuid_t *uuid, uint8_t find_remote_flag);

/**
 * @if Eng
 * @brief Port connection.
 * @par The service discovers and creates the client port, and returns the local port and connection status in
bs_br_port_conn_state_changed_callback after receiving the response.
 * @param [in] uuid     Application UUID.
 * @param [in] addr     device addr.
 * @param [in] find_remote_flag find_remote_flag Whether service discovery is required for the peer port. If it is a
fixed port, service discovery is not required.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  port连接。
 * @par    port连接，服务发现并创建客户端port，收到响应后在bs_br_port_conn_state_changed_callback中返回本端port和连接状态。
 * @param [in] uuid     应用uuid。
 * @param [in] addr     设备地址。
 * @param [in] find_remote_flag 是否需要服务发现对端port，若为固定port则不需要做服务发现。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_connect_by_addr(bd_addr_t addr, bt_uuid_t *uuid, uint8_t find_remote_flag);

/**
 * @if Eng
 * @brief  Send a connection response to a remote device, indicating whether to accept or reject the connection request.
 * @par  Send a connection response to a remote device, indicating whether to accept or reject the connection request.
 * @param  [in]  port       The port number of the PORT connection to response.
 * @param  [in]  is_accept  Indicates whether to accept the connection request (1 for accept, 0 for reject).
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li bts_spp.h
 * @see bs_br_port_create_server_port | bs_br_port_connection_request_callback | bs_br_port_conn_state_changed_callback
 * @else
 * @brief  向远程设备发送连接响应，指示是否接受或拒绝连接请求。
 * @par    向远程设备发送连接响应，指示是否接受或拒绝连接请求。
 * @param  [in]  port       要回复的 port 连接的端口号。
 * @param  [in]  is_accept  指示是否接受连接请求（1表示接受，0表示拒绝）。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li bts_spp.h
 * @see bs_br_port_create_server_port | bs_br_port_connection_request_callback | bs_br_port_conn_state_changed_callback
 * @endif
 */
errcode_t bs_br_port_send_conn_rsp(uint16_t port, uint8_t is_accept);

/**
 * @if Eng
 * @brief Client disconnect the connection.
 * @par Client disconnect the connection.
 * @param [in] dev_id   Device address id。
 * @param [in] port     client port
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  客户端去连接。
 * @par    客户端去连接。
 * @param [in] dev_id   设备ID。
 * @param [in] port     客户端port。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_disconnect(uint16_t dev_id, uint16_t port);

/**
 * @if Eng
 * @brief Port send data.
 * @par Port send dat.
 * @param [in] dev_id   device id.
 * @param [in] port     local port。
 * @param [in] data     Business data to be sent, the max data length is 990,
    If it exceeds the max data length, the sending will fail. See @ref BR_PORT_SERVICE_MAX_DATA_LEN
 * @param [in] len      Length of the sent service data.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  发送数据。
 * @par    发送数据。
 * @param [in] dev_id   设备id。
 * @param [in] port     本端port。
 * @param [in] data     要发送的业务数据。
 * @param [in] len      发送的业务数据长度, 数据长度最大为990，超过最大长度会发送失败 See @ref BR_PORT_SERVICE_MAX_DATA_LEN
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_write_data(uint16_t dev_id, uint16_t port, const uint8_t *data, const uint16_t len);

/**
 * @if Eng
 * @brief Register local port related callbacks.
 * @par Register local port related callbacks.
 * @param [in] uuid Application UUID.
 * @param [in] func Callback function that needs to be registered。
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  注册本端port相关回调。
 * @par    注册本端port相关回调。
 * @param [in] uuid 应用的uuid。
 * @param [in] func 需要注册的回调函数。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_register_callbacks(bt_uuid_t *uuid, bs_br_port_callbacks_t *func);

/**
 * @if Eng
 * @brief Unregister the local port related callback.
 * @par Unregister the local port related callback.
 * @param [in] port Application UUID.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  本端port相关回调去注册。
 * @par    本端port相关回调去注册。
 * @param [in] port 应用的uuid。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_port_unregister_callbacks(bt_uuid_t *uuid);

/**
 * @if Eng
 * @brief bs port enable.
 * @par Description: bs port enable.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Depends:
 * @li
 * @else
 * @brief  bs port 使能。
 * @par Description: bs port 使能。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li  无
 * @endif
 */
errcode_t bs_br_port_enable(void);

/**
 * @if Eng
 * @brief   bs port disable.
 * @par     bs port disable.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Depends:
 * @li
 * @else
 * @brief  bs port 去使能。
 * @par Description: bs port 去使能。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li  无
 * @endif
 */
errcode_t bs_br_port_disable(void);

#ifdef __cplusplus
}
#endif
#endif /* BS_BR_SERVICE_PORT_API_H */