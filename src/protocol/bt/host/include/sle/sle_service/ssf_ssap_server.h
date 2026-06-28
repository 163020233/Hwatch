
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: ssf SLE SSAP Server Service.
 */

#ifndef SSF_SSAP_SERVER_H
#define SSF_SSAP_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"
#include "bts_def.h"
#include "ssf_ssap_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of ssap indication cfm result.
 * @else
 * @brief 指示确认结果。
 * @endif
 */
typedef enum {
    BS_SSAP_INDICATION_CFM_FAIL   = 0x00,  /*!< @if Eng Indication cfm fail
                                                @else   指示失败 @endif */
    BS_SSAP_INDICATION_CFM_SUCESS = 0x01,  /*!< @if Eng Indication cfm sucess
                                                @else   指示成功 @endif */
} bs_sle_ssaps_indication_cfm_result_t;

/**
 * @if Eng
 * @brief Struct of read request information.
 * @else
 * @brief 读请求信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;        /*!< @if Eng Service Instance id.
                                 @else   服务实例 ID。 @endif */
    uint8_t item_id;        /*!< @if Eng item id, Refer to the order when adding item.
                                 @else   条目标识，参考添加条目时的顺序。 @endif */
    uint16_t request_id;    /*!< @if Eng Request id.
                                 @else   请求id。 @endif */
    uint8_t type;           /*!< @if Eng property、descriptor type { @ref bs_sle_ssap_property_type_t }.
                                 @else   属性、描述符类型。{ @ref bs_sle_ssap_property_type_t } @endif */
    bool need_rsp;          /*!< @if Eng Whether response is needed.
                                 @else   是否需要发送响应，需要回复响应的场景，需要立即回复响应，否则会阻塞后续ssap信令交互
                                         如果需要回复响应，则需调用接口 { @ref bs_sle_ssaps_send_response }。 @endif */
} bs_sle_ssaps_req_read_cb_t;

/**
 * @if Eng
 * @brief Struct of write request information.
 * @else
 * @brief 写请求信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;        /*!< @if Eng Service Instance id.
                                 @else   服务实例 ID。 @endif */
    uint8_t item_id;        /*!< @if Eng item id, Refer to the order when adding item.
                                     @else   条目标识，参考添加条目时的顺序。@endif */
    uint16_t request_id;    /*!< @if Eng Request id.
                                 @else   请求id。 @endif */
    uint8_t type;           /*!< @if Eng property、descriptor type { @ref bs_sle_ssap_property_type_t }.
                                 @else   属性、描述符类型 { @ref bs_sle_ssap_property_type_t }。 @endif */
    bool need_rsp;          /*!< @if Eng Whether response is needed.
                                 @else   是否需要发送响应，需要回复响应的场景，需要立即回复响应，否则会阻塞后续ssap信令交互
                                         如果需要回复响应，则需调用接口 { @ref bs_sle_ssaps_send_response }。 @endif */
    uint16_t length;        /*!< @if Eng Length of write request data.
                                 @else   请求写的数据长度。 @endif */
    uint8_t *value;         /*!< @if Eng Write request data.
                                 @else   请求写的数据。 @endif */
} bs_sle_ssaps_req_write_cb_t;

/**
 * @if Eng
 * @brief Struct of call method request information.
 * @else
 * @brief 方法调用请求信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;        /*!< @if Eng Service Instance id.
                                 @else   服务实例 ID。 @endif */
    uint8_t item_id;        /*!< @if Eng item id, Refer to the order when adding item.
                                 @else   条目标识，参考添加条目时的顺序。 @endif */
    uint16_t request_id;    /*!< @if Eng Request id.
                                 @else   请求id。 @endif */
    bool need_rsp;          /*!< @if Eng Whether response is needed.
                                 @else   是否需要发送响应，需要回复响应的场景，需要立即回复响应，否则会阻塞后续ssap信令交互
                                         如果需要回复响应，则需调用接口 { @ref bs_sle_ssaps_send_response }。 @endif */
    uint16_t length;        /*!< @if Eng Length of write request data.
                                 @else   请求传入的数据长度。 @endif */
    uint8_t *value;         /*!< @if Eng Write request data.
                                 @else   请求传入的数据。 @endif */
} bs_sle_ssaps_req_call_method_cb_t;

/**
 * @if Eng
 * @brief Struct of send response information.
 * @else
 * @brief 发送响应信息。
 * @endif
 */
typedef struct {
    uint16_t request_id;  /*!< @if Eng Request ID.
                               @else   请求 ID。 @endif */
    uint16_t value_len;   /*!< @if Eng Length of reponse data.
                               @else   响应的数据长度。 @endif */
    uint8_t *value;       /*!< @if Eng Reponse data.
                               @else   响应的数据。 @endif */
} bs_sle_ssaps_send_rsp_t;

/**
 * @if Eng
 * @brief Struct of send notification/indication information.
 * @else
 * @brief 发送通知/指示信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;        /*!< @if Eng Service Instance id.
                                 @else   服务实例 ID。 @endif */
    uint8_t item_id;        /*!< @if Eng item id, Refer to the order when adding item.
                                 @else   条目标识，参考添加条目时的顺序。 @endif */
    uint8_t type;           /*!< @if Eng property、descriptor type { @ref bs_sle_ssap_property_type_t }.
                                 @else   属性类型 { @ref bs_sle_ssap_property_type_t }。 @endif */
    uint16_t value_len;     /*!< @if Eng Length of notification/indication data.
                                 @else   通知/指示数据长度。 @endif */
    uint8_t *value;         /*!< @if Eng Notification/indication data.
                                 @else   发送的通知/指示数据。 @endif */
} bs_sle_ssaps_ntf_ind_t;

/**
 * @if Eng
 * @brief Struct of send event notification/indication information.
 * @else
 * @brief 发送事件通知/指示信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;        /*!< @if Eng Service Instance id.
                                 @else   服务实例 ID。 @endif */
    uint8_t item_id;        /*!< @if Eng item id, Refer to the order when adding item.
                                 @else   条目标识，参考添加条目时的顺序。 @endif */
    bool need_rsp;          /*!< @if Eng The receiver needs to respond.
                                 @else   需要接收端回复响应，有回复响应：通知，无回复响应：指示。 @endif */
    uint16_t value_len;     /*!< @if Eng Length of event notification/indication data.
                                 @else   事件通知/指示数据长度。 @endif */
    uint8_t *value;         /*!< @if Eng Event notification/indication data.
                                 @else   发送事件的通知/指示数据。 @endif */
} bs_sle_ssaps_event_ntf_ind_t;

/**
 * @if Eng
 * @brief Struct of add SSAP descriptor information.
 * @else
 * @brief SSAP 添加描述符信息。
 * @endif
 */
typedef struct {
    bs_sle_uuid_t uuid;           /*!< @if Eng uuid of this descriptor.
                                       @else   该描述符uuid。 @endif */
    uint16_t permissions;         /*!< @if Eng descriptor permissions. { @ref bs_sle_ssap_permission_t }.
                                       @else   特征权限。 { @ref bs_sle_ssap_permission_t } @endif */
    uint32_t operate_indication;  /*!< @if Eng operate Indication. { @ref bs_sle_ssap_operate_indication_t }
                                       @else   操作指示 { @ref bs_sle_ssap_operate_indication_t } @endif */
    uint8_t type;                 /*!< @if Eng descriptor type. { @ref bs_sle_ssap_property_type_t }.
                                       @else   描述符类型。 { @ref bs_sle_ssap_property_type_t } @endif */
    uint16_t value_len;           /*!< @if Eng data length.
                                       @else   数据长度。 @endif */
    uint8_t *value;               /*!< @if Eng data.
                                       @else   数据。 @endif */
} bs_sle_ssaps_descriptor_info_t;

/**
 * @if Eng
 * @brief Struct of add SSAP property information.
 * @else
 * @brief SSAP 添加属性信息。
 * @endif
 */
typedef struct {
    bs_sle_uuid_t uuid;           /*!< @if Eng uuid of this property.
                                      @else    该属性的uuid。 @endif */
    uint16_t permissions;         /*!< @if Eng Properity permissions. { @ref bs_sle_ssap_permission_t }
                                       @else   特征权限。{ @ref bs_sle_ssap_permission_t }。 @endif */
    uint32_t operate_indication;  /*!< @if Eng Operate Indication. { @ref bs_sle_ssap_operate_indication_t }
                                       @else   操作指示 { @ref bs_sle_ssap_operate_indication_t } @endif */
    uint8_t desc_count;           /*!< @if Eng Number of peoperty descriptor arrays: desc_array.
                                       @else   属性描述符数组：desc_array的数量 @endif */
    bs_sle_ssaps_descriptor_info_t *desc_array;  /*!< @if Eng peoperty descriptor arrays
                                                  @else   属性描述符数组 @endif */
    uint16_t value_len;                      /*!< @if Eng data length.
                                                  @else   数据长度。 @endif */
    uint8_t *value;                          /*!< @if Eng data.
                                                  @else   数据。 @endif */
} bs_sle_ssaps_property_info_t;

/**
 * @if Eng
 * @brief Struct of add SSAP method and event information.
 * @else
 * @brief SSAP 添加方法与事件信息。
 * @endif
 */
typedef struct {
    bs_sle_uuid_t uuid;           /*!< @if Eng uuid of this method、event.
                                      @else    该方法、事件的uuid。 @endif */
    uint16_t permissions;         /*!< @if Eng Properity permissions. { @ref bs_sle_ssap_permission_t }
                                       @else   特征权限。{ @ref bs_sle_ssap_permission_t }。 @endif */
    uint32_t operate_indication;  /*!< @if Eng Operate Indication. { @ref bs_sle_ssap_operate_indication_t }
                                       @else   操作指示 { @ref bs_sle_ssap_operate_indication_t } @endif */
    uint16_t value_len;           /*!< @if Eng data length.
                                       @else   数据长度。 @endif */
    uint8_t *value;               /*!< @if Eng data.
                                       @else   数据。 @endif */
} bs_sle_ssaps_method_and_event_info_t;

/**
 * @if Eng
 * @brief Struct of add SSAP service item information.
 * @else
 * @brief SSAP 添加服务的条目信息。
 * @endif
 */
typedef struct {
    uint8_t type;  /*!< @if Eng type of property、method、event.{ @ref bs_sle_ssapsitem_type_t }
                        @else   添加的条目类型，不包含服务类型，
                                仅包含：属性、方法、事件 { @ref bs_sle_ssapsitem_type_t } @endif */
    union {
        bs_sle_ssaps_property_info_t property;                  /*!< @if Eng add property information.
                                                                 @else   添加属性信息， 当条目类型为属性时使用。 @endif */
        bs_sle_ssaps_method_and_event_info_t method_and_event;  /*!< @if Eng add method and event information.
                                                                 @else   添加方法、事件信息，
                                                                         当条目类型为方法、事件时使用。 @endif */
    };
} bs_sle_ssaps_item_info_t;

/**
 * @if Eng
 * @brief Callback function for starting the service after adding an instance.
 * @par Callback function for starting the service after adding an instance.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @param [in] server_id Server ID.
 * @param [in] inst_id Instance ID of the configured service.
 * @param [in] status Execution result error code.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  添加实例后，开始服务的回调函数。
 * @par    添加实例后，开始服务的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param [in] server_id 服务端 ID。
 * @param [in] inst_id   配置服务的实例ID。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_start_service_callback)(uint8_t server_id, uint8_t inst_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for deleting all instances on the server.
 * @par Callback function for deleting all instances on the server.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @param [in] server_id Server ID.
 * @param [in] inst_id Instance ID of the configured service.
 * @param [in] status Execution result error code.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  删除服务端的所有实例的回调函数。
 * @par    删除服务端的所有实例的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param [in] server_id 服务端 ID。
 * @param [in] inst_id   配置服务的实例ID。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_delete_service_callback)(uint8_t server_id, uint8_t inst_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for receiving read requests.
 * @par Callback function for receiving read requests.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @attention 2. If the request requires a response, the response must be returned immediately;
                 otherwise, subsequent SSAP signaling interaction will be blocked.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] read_cb_para Read request parameters.
 * @param [in] status Execution result error code.
 * @retval #void no return value.
 * @par Dependency:
 * @li  sle_ssap_stru.h
 * @see sle_ssaps_callbacks_t
 * @else
 * @brief  收到读请求的回调函数。
 * @par    收到读请求的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 如果该请求需要回复响应，则需立即回复响应，否则会阻塞后续ssap信令交互。
 * @param [in] device_id    设备id。
 * @param [in] server_id    服务端 ID。
 * @param [in] read_cb_para 读请求参数。
 * @param [in] status       执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @li  sle_ssap_stru.h
 * @see sle_ssaps_callbacks_t
 * @endif
 */
typedef void (*bs_sle_ssaps_read_request_callback)(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_req_read_cb_t *read_cb_para, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive write request.
 * @par Callback invoked when  receive write request.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @attention 2. If the request requires a response, the response must be returned immediately;
                 otherwise, subsequent SSAP signaling interaction will be blocked.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] write_cb_para Write request parameters.
 * @param [in] status Execution result error code.
 * @param [in] status        error code.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  收到写请求的回调函数。
 * @par    收到写请求的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 如果该请求需要回复响应，则需立即回复响应，否则会阻塞后续ssap信令交互。
 * @param [in] device_id     设备id。
 * @param [in] server_id     服务端 ID。
 * @param [in] write_cb_para 写请求参数。
 * @param [in] status        执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_write_request_callback)(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_req_write_cb_t *write_cb_para, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive call method request.
 * @par Callback invoked when receive call method request.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @attention 2. If the request requires a response, the response must be returned immediately;
                 otherwise, subsequent SSAP signaling interaction will be blocked.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] method_cb_para call method request parameter.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  收到方法调用请求的回调函数。
 * @par    收到方法调用请求的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. 如果该请求需要回复响应，则需立即回复响应，否则会阻塞后续ssap信令交互。
 * @param [in] device_id     设备id。
 * @param [in] server_id     服务端 ID。
 * @param [in] method_cb_para 方法调用请求参数。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_call_method_request_callback)(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_req_call_method_cb_t *call_method_cb_para);

/**
 * @if Eng
 * @brief Callback invoked when receive indicate cfm.
 * @par Callback invoked when receive indicate cfm.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] cfm_result cfm result.
 * @param [in] status    error code.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  收到指示确认的回调函数。
 * @par    收到指示确认的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param [in] device_id     设备id。
 * @param [in] server_id     服务端 ID。
 * @param [in] cfm_result 确认结果。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_indicate_cfm_callback)(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_indication_cfm_result_t cfm_result);

/**
 * @if Eng
 * @brief Callback invoked when mtu size changed.
 * @par Callback invoked when mtu size changed.
 * @attention 1. This callback function runs on the BTS thread and must not block or wait for a long time.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] mtu_size  mtu size.
 * @param [in] status    error code.
 * @retval #void no return value.
 * @par Dependency:
 * @else
 * @brief  mtu大小改变的回调函数。
 * @par    mtu大小改变的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param [in] device_id 设备id。
 * @param [in] server_id 服务端 ID。
 * @param [in] mtu_size  mtu 大小。
 * @param [in] status    执行结果错误码。
 * @retval 无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_ssaps_mtu_changed_callback)(
    uint16_t device_id, uint8_t server_id, uint16_t mtu_size, errcode_t status);

/**
 * @if Eng
 * @brief SSAP standard service callback function.
 * @else
 * @brief SSAP 服务端回调函数。
 * @endif
 */
typedef struct {
    bs_sle_ssaps_start_service_callback start_service_cb;      /*!< @if Eng Service started callback.
                                                                @else   添加实例后，开启服务回调函数。 @endif */
    bs_sle_ssaps_delete_service_callback delete_service_cb;    /*!< @if Eng Service delete all server callback.
                                                                @else   删除服务端的所有实例回调函数。 @endif */
    bs_sle_ssaps_read_request_callback read_request_cb;        /*!< @if Eng Service read request callback.
                                                                @else   收到读请求的回调函数。 @endif */
    bs_sle_ssaps_write_request_callback write_request_cb;      /*!< @if Eng Service write request callback.
                                                                @else   收到写请求的回调函数。 @endif */
    bs_sle_ssaps_call_method_request_callback call_method_cb;  /*!< @if Eng Service write request callback.
                                                                @else   收到写请求的回调函数。 @endif */
    bs_sle_ssaps_indicate_cfm_callback indicate_cfm_cb;        /*!< @if Eng Service indicate cfm callback.
                                                                @else   指示确认回调函数。 @endif */
    bs_sle_ssaps_mtu_changed_callback mtu_changed_cb;          /*!< @if Eng Service mtu changed callback.
                                                                @else   mtu 大小更新回调函数。 @endif */
} bs_sle_ssaps_callbacks_t;

/**
 * @if Eng
 * @brief SSAP standard service config structure extend param.
 * @else
 * @brief SSAP 服务配置结构扩展参数。
 * @endif
 */
typedef struct {
    uint8_t resv; /*!< @if Eng Reserved
                       @else 预留 @endif */
} bs_sle_ssaps_extend_param_t;

/**
 * @if Eng
 * @brief SSAP standard service config structure.
 * @else
 * @brief SSAP 服务配置结构。
 * @endif
 */
typedef struct {
    bs_sle_uuid_t uuid;                            /*!< @if Eng uuid of this service.
                                                        @else   该服务uuid。 @endif */
    uint8_t is_primary;                            /*!< @if Eng is primary service tag
                                                        @else   是否是主服务标记 @endif */
    uint8_t item_count;                            /*!< @if Eng size of item_array.
                                                        @else   item_array 中的条目个数 @endif */
    bs_sle_ssaps_callbacks_t cbks;                 /*!< @if Eng server callback function.
                                                        @else   服务端回调函数 @endif */
    bs_sle_ssaps_extend_param_t extend_param;      /*!< @if Eng server structure extend param.
                                                        @else   服务端扩展参数 @endif */
    bs_sle_ssaps_item_info_t item_array[0];       /*!< @if Eng point array of item descr.
                                                        @else   服务中条目数组。 @endif */
} bs_sle_ssaps_service_template_t;

/**
 * @if Eng
 * @brief  Register SSAP server.
 * @par Description: Register SSAP server.
 * @param  [in]  app_uuid  App uuid.
 * @param  [out] server_id Server ID.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li bs_sle_ssap_server.h
 * @else
 * @brief  注册SSAP服务端。
 * @par Description: 注册SSAP服务端。
 * @param  [in]  app_uuid  上层应用uuid。
 * @param  [out] server_id 服务端ID。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li bs_sle_ssap_server.h
 * @endif
 */
errcode_t bs_sle_ssaps_register_server(bs_sle_uuid_t *app_uuid, uint8_t *server_id);

/**
 * @if Eng
 * @brief  Unregister SSAP server.
 * @par Description: Unregister SSAP server.
 * @param  [in] server_id Server ID.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li bs_sle_ssap_server.h
 * @else
 * @brief  注销SSAP服务端。
 * @par Description: 注销SSAP服务端。
 * @param  [in] server_id 服务端ID。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li bs_sle_ssap_server.h
 * @endif
 */
errcode_t bs_sle_ssaps_unregister_server(uint8_t server_id);

/**
 * @if Eng
 * @brief Add a service instance on the server.
 * @par Description: Adds a service instance on the server.
 * @attention 1. The index of the item_array array in the template structure of the configuration service
                 will be used as the identifier of an entry in subsequent operations.
                 Therefore, users need to store the index relationship.
 * @param [in] server_id Server ID.
 * @param [in] service Template structure of the configured service.
 * @param [out] inst_id Instance ID of the configured service.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li bs_sle_ssap_server.h
 * @else
 * @brief  服务端添加服务实例。
 * @par Description: 服务端添加服务实例。
 * @attention 1. 配置服务的模板结构中的item_array数组的下标，会作为后续操作时条目的标识符，需要用户存储索引关系。
 * @param  [in] server_id 服务端ID。
 * @param  [in] service 配置服务的模板结构。
 * @param  [out] inst_id 配置服务的实例ID。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li bs_sle_ssap_server.h
 * @endif
 */
errcode_t bs_sle_ssaps_service_instance_init(
    uint8_t server_id, bs_sle_ssaps_service_template_t *service, uint8_t *inst_id);

/**
 * @if Eng
 * @brief Sends a response when receiving a read\write request that requires user reply.
 * @par Description: Sends a response when receiving a read\write request that requires user reply.
                     { @ref bs_sle_ssaps_read_request_callback } and { @ref bs_sle_ssaps_write_request_callback }
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] param     Response parameters.
 * @param [in] status    Status code of read/write. Success: ERRCODE_SLE_SUCCESS.
                         For details about the exception { @ref bs_sle_ssapserrcode_t }
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li bs_sle_ssap_server.h
 * @else
 * @brief  当收到需要用户回复响应的读、写请求时，发送响应。
 * @par Description: 当收到需要用户回复响应的读、写请求时，发送响应。
                     { @ref bs_sle_ssaps_read_request_callback } 与 { @ref bs_sle_ssaps_write_request_callback }
 * @param  [in] device_id 设备id。
 * @param  [in] server_id 服务端ID。
 * @param  [in] param     响应参数。
 * @param  [in] status    读写结果的状态, 成功：ERRCODE_SLE_SUCCESS  异常参考{ @ref bs_sle_ssapserrcode_t }
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li bs_sle_ssap_server.h
 * @endif
 */
errcode_t bs_sle_ssaps_send_response(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_send_rsp_t *param, uint8_t status);

/**
 * @if Eng
 * @brief Sends a response when receiving a method request that requires user reply.
 * @par Description: Sends a response when receiving a method request that requires user reply.
                     { @ref bs_sle_ssaps_call_method_request_callback }
 * @attention 1. The error response of the method should be reflected in the user-defined method response data.
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] param Response parameters.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li bs_sle_ssap_server.h
 * @else
 * @brief  当收到需要用户回复响应的方法请求时，发送响应。
 * @par Description: 当收到需要用户回复响应的方法请求时，发送响应。{ @ref bs_sle_ssaps_call_method_request_callback }
 * @attention 1. 方法的错误响应，应在用户定义的方法的响应数据中体现。
 * @param  [in] device_id 设备id。
 * @param  [in] server_id 服务端ID。
 * @param  [in] param     响应参数。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li bs_sle_ssap_server.h
 * @endif
 */
errcode_t bs_sle_ssaps_send_method_response(uint16_t device_id, uint8_t server_id, bs_sle_ssaps_send_rsp_t *param);

/**
 * @if Eng
 * @brief  Send indication or notification to remote device.
 * @par Description: Send indication or notification to remote device,
                     send status depend on item descriptor: client property configuration.
                     value = 0x0000: notification and indication not allowed
                     value = 0x0001: notification allowed
                     value = 0x0002: indication allowed
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] param     notify/indicate parameter.
 * @retval error code.
 * @par Depends:
 * @else
 * @brief  向对端发送通知或指示。
 * @par Description: 向对端发送通知或指示，具体发送状态取决于特征描述符：客户端特征配置
                     value = 0x0000：不允许通知和指示
                     value = 0x0001：允许通知
                     value = 0x0002：允许指示
 * @param  [in] device_id 设备id。
 * @param  [in] server_id 服务端ID。
 * @param  [in] param     通知或指示参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @endif
 */
errcode_t bs_sle_ssaps_notify_indicate(uint16_t device_id, uint8_t server_id, bs_sle_ssaps_ntf_ind_t *param);

/**
 * @if Eng
 * @brief  Send event indication or notification to remote device.
 * @par Description: Send event indication or notification to remote device
 * @param [in] device_id Device ID.
 * @param [in] server_id Server ID.
 * @param [in] param     event notify/indicate parameter.
 * @retval error code.
 * @par Depends:
 * @else
 * @brief  向对端发送事件通知或指示。
 * @par Description: 向对端发送事件通知或指示。
 * @param  [in] device_id 设备id。
 * @param  [in] server_id 服务端ID。
 * @param  [in] param     事件 通知或指示参数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @endif
 */
errcode_t bs_sle_ssaps_event_notify_indicate(
    uint16_t device_id, uint8_t server_id, bs_sle_ssaps_event_ntf_ind_t *param);

/**
 * @if Eng
 * @brief  Set mtu size before connected.
 * @par Description: Set mtu size before connected.
 * @param  [in] server_id server ID.
 * @param  [in] mtu_size  mtu size.
 * @retval error code.
 * @par Depends:
 * @else
 * @brief  在连接之前设置服务端最大mtu。
 * @par Description: 在连接之前设置服务端最大mtu。
 * @param  [in] server_id 服务端ID。
 * @param  [in] mtu_size  mtu 大小。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @endif
 */
errcode_t bs_sle_ssaps_set_mtu_size(uint8_t server_id, uint16_t mtu_size);

#ifdef __cplusplus
}
#endif
#endif /* SSF_SSAP_SERVER_H */