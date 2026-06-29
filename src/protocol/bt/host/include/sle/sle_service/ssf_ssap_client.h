
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) at media table file.
 */
#ifndef SSF_SSAP_CLIENT_H
#define SSF_SSAP_CLIENT_H

#include <stdbool.h>
#include "securec.h"
#include "errcode.h"
#include "bts_def.h"
#include "ssf_ssap_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS_SLE_DESCRIPTOR_COUNT_MAX 5

/**
 * @if Eng
 * @brief Struct of SSAP write commands and requests.
 * @else
 * @brief SSAP 写入命令与请求。
 * @endif
 */
typedef struct {
    uint16_t handle;    /*!< @if Eng property handle.
                             @else   属性句柄。 @endif */
    uint8_t  type;      /*!< @if Eng property type.
                             @else   属性类型。 @endif */
    bool is_need_confirm; /*!< @if WWhether to require peer confirmation: false: no peer response needed, true: the
                           result is returned in the bs_gattc_write_cfm_callback callback after peer confirmation.
                           @else   是否需要对端确认, false：无需对端回复,
                           true:对端确认后结果在bs_gattc_write_cfm_callback回调中返回。@endif */
    uint16_t data_len;  /*!< @if Eng Data Length.
                             @else   数据长度。 @endif */
    uint8_t  *data;     /*!< @if Eng Data.
                             @else   数据内容。 @endif */
} bs_sle_ssapc_write_param_t;

/**
 * @if Eng
 * @brief Struct of SSAP method Commands and Requests.
 * @else
 * @brief SSAP 方法命令与请求。
 * @endif
 */
typedef struct {
    uint16_t handle;    /*!< @if Eng property handle.
                             @else   属性句柄。 @endif */
    uint16_t data_len;  /*!< @if Eng Data Length.
                             @else   数据长度。 @endif */
    uint8_t  *data;     /*!< @if Eng Data.
                             @else   数据内容。 @endif */
    bool is_need_confirm; /*!< @if WWhether to require peer confirmation: false: no peer response needed, true: the
                        result is returned in the bs_gattc_write_cfm_callback callback after peer confirmation.
                        @else   是否需要对端确认, false：无需对端回复,
                        true:对端确认后结果在bs_gattc_write_cfm_callback回调中返回。@endif */
} bs_sle_ssapc_method_param_t;

/**
 * @if Eng
 * @brief Service discovery result.
 * @else
 * @brief 服务发现结果。
 * @endif
 */
typedef struct {
    uint16_t   start_hdl;          /*!< @if Eng service start handle.
                                        @else   服务起始句柄。 @endif */
    uint16_t   end_hdl;            /*!< @if Eng service end handle.
                                        @else   服务结束句柄。 @endif */
    bs_sle_uuid_t uuid;            /*!< @if Eng service end handle.
                                        @else   服务UUID。 @endif */
} bs_sle_ssapc_find_service_result_t;

 /**
 * @if Eng
 * @brief Attribute discovery result.
 * @else
 * @brief 属性发现结果。
 * @endif
 */
typedef struct {
    uint16_t   handle;                                     /*!< @if Eng property handle.
                                                                @else   属性句柄。 @endif */
    uint32_t   operate_indication;                         /*!< @if Eng Operation instructions.
                                                                @else   操作指示。 @endif */
    bs_sle_uuid_t uuid;                                    /*!< @if Eng UUID.
                                                                @else   UUID标识。 @endif */
    uint8_t    descriptors_count;                          /*!< @if Eng Attribute Descriptor Type List Length.
                                                                @else   属性描述符类型列表长度。 @endif */
    uint8_t    descriptors_type[BS_SLE_DESCRIPTOR_COUNT_MAX]; /*!< @if Eng Attribute descriptor type list.
                                                                        { @ref ssap_property_descriptor_type_t }.
                                                                @else   属性描述符类型列表，
                                                                        { @ref ssap_property_descriptor_type_t }类型定义
                                                                @endif */
} bs_sle_ssapc_find_property_result_node_t;


/**
 * @if Eng
 * @brief Method/Event discovery result.
 * @else
 * @brief 方法、事件发现结果。
 * @endif
 */
typedef struct {
    uint16_t   handle;     /*!< @if Eng method handle.
                                @else   句柄。 @endif */
    bs_sle_uuid_t uuid;    /*!< @if Eng UUID.
                                @else   UUID。 @endif */
} bs_sle_ssapc_find_method_event_result_t;

/**
 * @if Eng
 * @brief Results for SSAP handle and value.
 * @else
 * @brief SSAP 句柄值。
 * @endif
 */
typedef struct {
    uint16_t handle;    /*!< @if Eng property handle.
                             @else   属性句柄。 @endif */
    uint8_t  type;      /*!< @if Eng property type.
                             @else   属性类型。 @endif */
    uint16_t data_len;  /*!< @if Eng Data Length.
                             @else   数据长度。 @endif */
    uint8_t  *data;     /*!< @if Eng Data.
                             @else   数据内容。 @endif */
} bs_sle_ssapc_handle_value_t;

/**
 * @if Eng
 * @brief Results for SSAP method.
 * @else
 * @brief SSAP 方法。
 * @endif
 */
typedef struct {
    uint16_t handle;    /*!< @if Eng method handle.
                             @else   方法句柄。 @endif */
    uint16_t data_len;  /*!< @if Eng Data Length.
                             @else   数据长度。 @endif */
    uint8_t *data;      /*!< @if Eng Data.
                             @else   数据内容。 @endif */
} bs_sle_ssapc_method_result_t;

/**
 * @if Eng
 * @brief  Struct of ssap info exchange
 * @else
 * @brief  ssap 信息交换结构体。
 * @endif
 */
typedef struct {
    uint32_t mtu_size; /*!< @if Eng mtu size
                            @else   mtu大小 @endif */
    uint16_t version;  /*!< @if Eng version
                            @else   版本 @endif */
} bs_sle_ssap_exchange_info_t;

/**
 * @if Eng
 * @brief A service information about a remote object.
 * @else
 * @brief 远端对象的服务信息发现结果。
 * @endif
 */
typedef struct {
    bs_sle_ssapc_find_service_result_t        service;        /*!< @if Eng service information.
                                                  @else   服务信息 { @ref bs_sle_ssapc_find_service_result_t }。
                                                  @endif */
    uint8_t                                   property_count; /*!< @if Eng Number of service property.
                                                                   @else 服务的属性个数。 @endif */
    bs_sle_ssapc_find_property_result_node_t *property_list;  /*!< @if Eng property items array.
                                                  @else   属性元素数组，{ @ref bs_sle_ssapc_find_property_result_node_t }
                                                  @endif */
    uint8_t                                   method_count;   /*!< @if Eng Number of service method.
                                                                   @else 服务的方法个数。 @endif */
    bs_sle_ssapc_find_method_event_result_t  *method_list;    /*!< @if Eng method items array.
                                                  @else   方法元素数组，{ @ref bs_sle_ssapc_find_method_event_result_t }
                                                  @endif */
    uint8_t                                   event_count;    /*!< @if Eng Number of service event.
                                                                   @else   服务的事件个数。 @endif */
    bs_sle_ssapc_find_method_event_result_t  *event_list;     /*!< @if Eng event items array.
                                                  @else   事件元素数组，{ @ref bs_sle_ssapc_find_method_event_result_t}
                                                  @endif */
} bs_sle_ssapc_find_service_result_node_t;

/**
 * @if Eng
 * @brief Callback function for the client to receive the read response from the server.
 * @par Description: This is a callback function for the client to receive the read response from the server.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] read_data Indicates the read data.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other A failure occurred. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务端读响应的回调。
 * @par Description: 客户端接收服务端读响应的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  read_data 读数据
 * @param  [in]  status    错误码
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_read_cfm_callback)(uint16_t dev_id, uint8_t client_id,
    bs_sle_ssapc_handle_value_t *read_data, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for the client to receive the write response from the server.
 * @par Description: This is a callback function for the client to receive the write response from the server.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] write_result Indicates the written data.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务端写响应的回调。
 * @par Description: 客户端接收服务端写响应的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  write_result 写数据
 * @param  [in]  status    错误码
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_write_cfm_callback)(uint16_t dev_id, uint8_t client_id,
    bs_sle_ssapc_handle_value_t *write_result, errcode_t status);

/**
 * @if Eng
 * @brief Defines the callback for the client to receive notification requests from the server.
 * @par Description This is a callback for the client to receive notification requests from the server.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] data Indicates the notification data.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务端通知请求的回调。
 * @par Description: 客户端接收服务端通知请求的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  data 通知数据
 * @param  [in]  status    错误码
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_notification_callback)(uint16_t dev_id, uint8_t client_id,
    bs_sle_ssapc_handle_value_t *data, errcode_t status);

/**
 * @if Eng
 * @brief Indicates the callback for the client to receive requests from the server.
 * @par Description: Indicates the callback for the client to receive requests from the server.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] data Indicates the data.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务端指示请求的回调。
 * @par Description: 客户端接收服务端指示请求的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  data 指示数据
 * @param  [in]  status    错误码
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_indication_callback)(uint16_t dev_id, uint8_t client_id, bs_sle_ssapc_handle_value_t *data,
    errcode_t status);

/**
 * @if Eng
 * @brief Indicates the callback for the client to receive requests from the server.
 * @par Description: Indicates the callback for the client to receive requests from the server.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] data Indicates the data.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务端调用方法响应的回调。
 * @par Description: 客户端接收服务端调用方法响应的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  call_method_result 调用方法结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_call_method_cfm_callback)(uint16_t dev_id, uint8_t client_id,
    bs_sle_ssapc_method_result_t *call_method_result);

/**
 * @brief Defines the callback for the client to receive service discovery results.
 * @par Description This is a callback for the client to receive service discovery results.
 * @param [in] dev_id Indicates the device ID.
 * @param [in] client_id Indicates the client ID.
 * @param [in] service_count Indicates the number of discovered services.
 * @param [in] service Indicates the detailed service discovery result.
 * @param [in] status Indicates the error code.
 * @retval ERRCODE_SUCC Success.
 * @retval Other Failure. For details, see @ref errcode_t.
 * @par Dependency:
 * @else
 * @brief  客户端接收服务发现结果的回调。
 * @par Description: 客户端接收服务发现结果的回调。
 * @param  [in]  dev_id    设备ID
 * @param  [in]  client_id 客户端ID
 * @param  [in]  service_count 发现服务的个数
 * @param  [in]  service 发现服务具体结果
 * @param  [in]  status    错误码
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssapc_find_service_callback)(uint16_t dev_id, uint8_t client_id,
    uint8_t service_count, bs_sle_ssapc_find_service_result_node_t *service, errcode_t status);

/**
 * @if Eng
 * @brief Struct of ssap client read and write callback function.
 * @else
 * @brief ssap client读写回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_ssapc_read_cfm_callback              read_cfm_cb;         /*!< @if Eng Receive write response callback.
                                                                   @else   收到写响应回调函数。 @endif */
    bs_sle_ssapc_write_cfm_callback              write_cfm_cb;      /*!< @if Eng Receive write response callback.
                                                                  @else   收到写响应回调函数。 @endif */
} bs_sle_ssapc_read_write_callbacks_t;

/**
 * @if Eng
 * @brief Struct of ssap client notification and indication callback function.
 * @else
 * @brief ssap client 通知和指示回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_ssapc_notification_callback           notification_cb; /*!< @if Eng Callback hook for receive notification.
                                                                 @else   通知事件上报钩子。 @endif */
    bs_sle_ssapc_indication_callback             indication_cb;   /*!<  @if Eng Callback hook for receive indication.
                                                                 @else   指示事件上报钩子。 @endif */
} bs_sle_ssapc_notication_indication_callbacks_t;

/**
 * @if Eng
 * @brief Struct of ssap client method callback function.
 * @else
 * @brief ssap client 方法回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_ssapc_call_method_cfm_callback   call_method_cfm_cb;   /*!< @if Eng Receive call method response callback.
                                                                @else   收到方法调用响应回调函数。 @endif */
} bs_sle_ssapc_method_callbacks_t;

/**
 * @if Eng
 * @brief Struct of ssap client operation callback function.
 * @else
 * @brief ssap client 操作类接口回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_ssapc_read_write_callbacks_t  read_write_cbk; /*!< @if Eng Struct of ssap client read and write callback function.
                                                       @else ssap client读写回调函数接口定义。
                                                       { @ref ssapc_read_write_callbacks_t } @endif */
    bs_sle_ssapc_notication_indication_callbacks_t notication_indication_cbk;
                                    /*!< @if Eng Struct of ssap client notification and indication callback function.
                                         @else ssap client notification和indication回调函数接口定义。
                                         { @ref ssapc_notication_indication_callbacks_t } @endif */
    bs_sle_ssapc_method_callbacks_t method_cbk;  /*!< @if Eng Struct of ssap client method callback function.
                                               @else ssap client notification和indication回调函数接口定义。
                                               { @ref ssapc_method_callbacks_t }  @endif */
} bs_sle_ssapc_operation_cbk_t;

/**
 * @if Eng
 * @brief  Register ssap client.
 * @par Description: Register ssap client.
 * @param  [in]  app_uuid  App uuid
 * @param  [out] client_id Client ID
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册ssap客户端。
 * @par Description: 注册ssap客户端。
 * @param  [in]  app_uuid  上层应用uuid
 * @param  [out] client_id 客户端ID
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t bs_sle_ssapc_register(bs_sle_uuid_t *app_uuid, uint8_t *client_id);

/**
 * @if Eng
 * @brief  Unregister ssap client.
 * @par Description: Unregister ssap client.
 * @param  [in] client_id Client ID
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注销ssap客户端。
 * @par Description: 注销ssap服务端。
 * @param  [in] client_id 客户端ID
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t bs_sle_ssapc_unregister(uint8_t client_id);

/**
 * @if Eng
 * @brief  Find all service.
 * @par Description: Find all service.
 * @param [in] client_id Client ID.
 * @param [in] dev_id   Device ID.
 * @param [in] find_structure_cbk  callback when discovery service complete.
 * @param [in] is_forced  true：force discovery service from remote dev，false：return find service from cache .
 * @retval error code, discovery all service result will be returned at { @ref bs_sle_ssapc_find_service_callback }
 * @else
 * @brief  查找服务、属性、方法、事件。
 * @par Description: 查找服务、属性、方法、事件。
 * @param [in] client_id 客户端 ID。
 * @param [in] dev_id   设备 ID。
 * @param [in] find_structure_cbk  查找服务完成会回调。
 * @param [in] is_forced true:强制从远端设备获取服务,  false: 如果本地有缓存则从缓存中获取所有服务.
 * @retval 执行结果错误码, 服务发现结果将在 { @ref bs_sle_ssapc_find_service_callback }中返回。
 * @endif
 */
errcode_t bs_sle_ssapc_find_all_service(
    uint16_t dev_id, uint8_t client_id, bs_sle_ssapc_find_service_callback find_structure_cbk, bool is_remote);

/**
 * @if Eng
 * @brief  Find service by uuid or uuid group.
 * @par Description: Find service by uuid or uuid group.
 * @param [in] client_id Client ID.
 * @param [in] dev_id   Device ID.
 * @param [in] uuid_num  Number of UUIDs.
 * @param [in] uuid  uuid items array.
 * @param [in] find_structure_cbk  callback when discovery service complete.
 * @retval error code, discovery all service result will be returned at { @ref bs_sle_ssapc_find_service_callback }
 * @else
 * @brief   基于服务uuid或者uuid组发现指定服务。
 * @par Description: 查找服务、属性、方法、事件。
 * @param [in] client_id 客户端 ID。
 * @param [in] dev_id   设备 ID。
 * @param [in] uuid_num  UUID个数.
 * @param [in] uuid  uuid数组.
 * @param [in] find_structure_cbk  查找服务完成会回调。
 * @retval 执行结果错误码, 服务发现结果将在 { @ref bs_sle_ssapc_find_service_callback }中返回。
 * @endif
 */
errcode_t bs_sle_ssapc_find_service_by_uuid(uint16_t dev_id, uint8_t client_id,
    uint8_t uuid_num, bs_sle_uuid_t *uuid_list, bs_sle_ssapc_find_service_callback find_structure_cbk);

/**
 * @if Eng
 * @brief  Register SSAP client multi-service callbacks.
 * @par Description: Register SSAP client multi-service callbacks.
 * @param  [in] client_id    client ID.
 * @param  [in] dev_id   Device ID.
 * @param  [in] service_uuid service UUID, Enter the UUID in little-endian order.
 * @param  [in] func         callback function.
 * @retval error code.
 * @par Notes
 *     1. The current interface needs to be invoked before start service.
 * @else
 * @brief  注册 SSAP client 多服务回调函数。
 * @par Description: 注册多服务回调函数。
 * @param  [in] client_id    客户端ID。
 * @param  [in] dev_id   Device ID.
 * @param  [in] service_uuid 服务唯一标识，UUID以小端序输入。
 * @param  [in] func         回调函数。
 * @retval 执行结果错误码。
 * @par 注意事项
 *     1、当前接口需要在服务发现完成之后调用；
 * @endif
 */
errcode_t bs_sle_ssapc_register_callbacks(uint16_t dev_id, uint8_t client_id, bs_sle_uuid_t *service_uuid,
    bs_sle_ssapc_operation_cbk_t *func);

/**
 * @if Eng
 * @brief  Unregister SSAP client multi-service callbacks.
 * @par Description: Unregister SSAP client multi-service callbacks.
 * @param  [in] client_id    client ID.
 * @param  [in] dev_id   Device ID.
 * @param  [in] service_uuid service UUID, Enter the UUID in little-endian order.
 * @retval error code.
 * @par Notes
 *     1. The current interface needs to be invoked before start service.
 * @else
 * @brief  去注册 SSAP client 多服务回调函数。
 * @par Description: 去注册多服务回调函数。
 * @param  [in] client_id    客户端ID。
 * @param  [in] dev_id   Device ID.
 * @param  [in] service_uuid 服务唯一标识, 以小端序输入。
 * @retval 执行结果错误码。
 * @par 注意事项
 *     1、当前接口需要在服务发现完成之后调用；
 * @endif
 */
errcode_t bs_sle_ssapc_unregister_callbacks(uint16_t dev_id, uint8_t client_id, bs_sle_uuid_t *service_uuid);

 /**
 * @if Eng
 * @brief  Send read by handle request.
 * @par Description: Send read by handle request.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id   Device ID.
 * @param  [in]  handle    handle.
 * @param  [in]  type      property type.
 * @retval error code, read result will be returned at { @ref bs_sle_ssapc_read_cfm_callback }.
 * @else
 * @brief  发起按照句柄读取请求。
 * @par Description: 发起按照句柄读取请求。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id   设备 ID。
 * @param  [in]  handle    句柄。
 * @param  [in]  type      特征类型。
 * @retval 执行结果错误码，读取结果将在 { @ref bs_sle_ssapc_read_cfm_callback }中返回。
 * @endif
 */
errcode_t bs_sle_ssapc_read(uint16_t dev_id, uint8_t client_id, uint16_t handle, uint8_t type);

/**
 * @if Eng
 * @brief  Send write request or command.
 * @par Description: Send write request or command.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id   Device ID.
 * @param  [in]  param     Parameter for write request or command.
 * @retval error code, write result will be returned at { @ref bs_sle_ssapc_write_cfm_callback }.
 * @else
 * @brief  发起写请求或命令。
 * @par Description: 发起写请求或命令。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id   设备 ID。
 * @param  [in]  param     写请求或命令参数。
 * @retval 执行结果错误码，写结果将在 { @ref bs_sle_ssapc_write_cfm_callback }中返回。
 * @endif
 */
errcode_t bs_sle_ssapc_write(uint16_t dev_id, uint8_t client_id, bs_sle_ssapc_write_param_t *param);

/**
 * @if Eng
 * @brief  Call method request or command.
 * @par Description: Call method request or command.
 * @param [in] client_id Client ID.
 * @param [in] dev_id   Device ID.
 * @param [in] param     Parameter for call method.
 * @retval error code.
 * @else
 * @brief  方法调用请求或命令。
 * @par Description: 方法调用请求或命令。
 * @param [in] client_id 客户端 ID。
 * @param [in] dev_id   设备 ID。
 * @param [in] param     方法调用参数。
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_ssapc_call_method(uint16_t dev_id, uint8_t client_id, bs_sle_ssapc_method_param_t *param);

/**
 * @if Eng
 * @brief  Send exchange info request.
 * @par Description: Send exchange info request.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id   Device ID.
 * @param  [in]  param     Client info.
 * @retval error code
 * @else
 * @brief  发送交换info请求。
 * @par Description: 发送交换info请求。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id   设备 ID。
 * @param  [in]  param     客户端info。
 * @retval 执行结果错误码
 * @endif
 */
errcode_t bs_sle_ssapc_exchange_info_req(uint16_t dev_id, uint8_t client_id, bs_sle_ssap_exchange_info_t *param);
#ifdef __cplusplus
}
#endif

#endif /* SSF_SSAP_CLIENT_H */
