/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BLE GATT Server Service
 */

#ifndef BS_BLE_GATT_SERVER_SERVICE_H
#define BS_BLE_GATT_SERVER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"
#include "bts_def.h"
#include "bs_ble_gatt_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Struct of add characteristic information.
 * @else
 * @brief 添加特征信息。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;   /*!< @if Eng UUID of GATT characteristic.
                                 @else   GATT 特征 UUID。 @endif */
    uint8_t permissions;    /*!< @if Eng Characteristic permissions, { @ref bs_gatt_attribute_permission_t }.
                                 @else   特征权限， { @ref bs_gatt_attribute_permission_t }。 @endif */
    uint8_t properties;    /*!< @if Eng Characteristic properties, { @ref bs_gatt_characteristic_property_t }.
                                 @else   特征特性， { @ref bs_gatt_characteristic_property_t } 。 @endif */
    uint16_t value_len;     /*!< @if Eng Length of characteristic data.
                                 @else   特征数据长度。 @endif */
    uint8_t *value;         /*!< @if Eng characteristic data.
                                 @else   特征数据。 @endif */
} bs_gatts_add_chara_info_t;

/**
 * @if Eng
 * @brief Struct of add characteristic descriptor information.
 * @else
 * @brief 添加特征描述符信息。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;   /*!< @if Eng UUID of GATT descriptor.
                                @else   GATT 描述符 UUID。 @endif */
    uint8_t permissions;   /*!< @if Eng Descriptor permissions, { @ref bs_gatt_attribute_permission_t }.
                                @else   描述符权限， { @ref bs_gatt_attribute_permission_t }。 @endif */
    uint16_t value_len;    /*!< @if Eng Length of descriptor data.
                                @else   描述符数据长度。 @endif */
    uint8_t *value;        /*!< @if Eng descriptor data.
                                @else   描述符数据。 @endif */
} bs_gatts_add_desc_info_t;

/* 模板添加特征/描述符信息 */
typedef struct {
    uint8_t type; /*!< @if Eng 0:character, 1：descriptor.
                       @else 添加特征信息。 @endif */
    union {
        bs_gatts_add_chara_info_t character; /*!< @if Eng Struct of add characteristic information.
                                                      @else 添加特征信息。 @endif */
        bs_gatts_add_desc_info_t descriptor; /*!< @if Eng Struct of add characteristic descriptor information.
                                                  @else 添加特征描述符信息。 @endif */
    };
} bs_gatts_attribute_info;

/**
 * @if Eng
 * @brief Struct of read request information.
 * @else
 * @brief 读请求信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;      /*!< @if Eng Service Instance id.
                               @else   服务实例 ID。 @endif */
    uint8_t attribute_id; /*!< @if Eng attribute ID of the current service instance.
                               @else   当前服务实例下的属性ID。 @endif */
    uint16_t request_id;  /*!< @if Eng Request id.
                               @else   请求id。 @endif */
    bool need_rsp;        /*!< @if Eng Whether response is needed.
                               @else   是否需要发送响应。 @endif */
    bool need_authorize;  /*!< @if Eng Whether authorization is needed.
                               @else   是否需要授权。 @endif */
} bs_gatts_req_read_cb_t;

/**
 * @if Eng
 * @brief Struct of write request information.
 * @else
 * @brief 写请求信息。
 * @endif
 */
typedef struct {
    uint8_t inst_id;      /*!< @if Eng Service Instance id.
                               @else   服务实例 ID。 @endif */
    uint8_t attribute_id; /*!< @if Eng attribute ID of the current service instance.
                               @else   当前服务实例下的属性ID。 @endif */
    uint16_t request_id;  /*!< @if Eng Request id.
                               @else   请求id。 @endif */
    bool need_rsp;        /*!< @if Eng Whether response is needed.
                               @else   是否需要发送响应。 @endif */
    bool need_authorize;  /*!< @if Eng Whether authorization is needed.
                               @else   是否需要授权。 @endif */
    uint16_t value_len;      /*!< @if Eng Length of write request data.
                               @else   请求写的数据长度。 @endif */
    uint8_t *value;       /*!< @if Eng Write request data.
                               @else   请求写的数据。 @endif */
} bs_gatts_req_write_cb_t;

/**
 * @if Eng
 * @brief Struct of send response information.
 * @else
 * @brief 发送响应信息。
 * @endif
 */
typedef struct {
    uint16_t request_id; /*!< @if Eng Request ID.
                              @else   请求 ID。 @endif */
    uint8_t status;      /*!< @if Eng Status code of read/write, { @ref bs_gatt_status_t }.
                              @else   读写结果的状态， { @ref bs_gatt_status_t }。 @endif */
    uint16_t value_len;  /*!< @if Eng Length of reponse data.
                              @else   响应的数据长度。 @endif */
    uint8_t *value;      /*!< @if Eng Reponse data.
                              @else   响应的数据。 @endif */
} bs_gatts_send_rsp_t;

/**
 * @if Eng
 * @brief Struct of send notification/indication information.
 * @else
 * @brief 发送通知/指示信息。
 * @endif
 */
typedef struct {
    uint8_t instance_id;             /*!< @if Eng Service Instance id.
                                           @else  服务实例ID。 @endif */
    uint8_t attribute_id;            /*!< @if Eng attribute ID.
                                          @else   属性ID。 @endif */
    uint16_t value_len;              /*!< @if Eng Length of notification/indication data.
                                           @else   通知/指示数据长度。 @endif */
    uint8_t *value;                  /*!< @if Eng Notification/indication data.
                                           @else   发送的通知/指示数据。 @endif */
} bs_gatts_ntf_ind_t;

/**
 * @if Eng
 * @brief Callback invoked when service started.
 * @par Callback invoked when service started.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  server_id Server ID.
 * @param  [in]  inst_id   Service instance ID.
 * @param  [in]  status    error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  开始服务的回调函数。
 * @par    开始服务的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  server_id 服务端 ID。
 * @param  [in]  inst_id   服务实例 ID。
 * @param  [in]  status    执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gatts_start_service_callback)(uint8_t server_id, uint8_t inst_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when service stopped.
 * @par Callback invoked when service stopped.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  server_id Server ID.
 * @param  [in]  inst_id   Service instance ID.
 * @param  [in]  status    error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  停止服务的回调函数。
 * @par    停止服务的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  server_id 服务端 ID。
 * @param  [in]  inst_id   服务实例 ID。
 * @param  [in]  status    执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gatts_stop_service_callback)(uint8_t server_id, uint8_t inst_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when service deleted.
 * @par Callback invoked when service deleted.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  server_id server ID.
 * @param  [in]  status    error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  删除服务的回调函数。
 * @par    删除服务的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  server_id 服务端 ID。
 * @param  [in]  status    执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gatts_delete_service_callback)(uint8_t server_id, uint8_t inst_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive read request.
 * @par Callback invoked when  receive read request.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  dev_id       Device ID.
 * @param  [in]  server_id    server ID.
 * @param  [in]  inst_id      service instance ID.
 * @param  [in]  read_cb_para read request parameter.
 * @param  [in]  status       error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  收到读请求的回调函数。
 * @par    收到读请求的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  dev_id       设备 ID。
 * @param  [in]  server_id    服务端 ID。
 * @param  [in]  inst_id      服务实例 ID。
 * @param  [in]  read_cb_para 读请求参数。
 * @param  [in]  status       执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gatts_read_request_callback)(uint16_t dev_id, uint8_t server_id,
    bs_gatts_req_read_cb_t *read_cb_para, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive write request.
 * @par Callback invoked when  receive write request.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id       Device ID.
 * @param  [in]  server_id    server ID.
 * @param  [in]  inst_id      service instance ID.
 * @param  [in]  write_cb_para write request parameter.
 * @param  [in]  status        error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  收到写请求的回调函数。
 * @par    收到写请求的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id        设备 ID。
 * @param  [in]  server_id     服务端 ID。
 * @param  [in]  inst_id       服务实例 ID。
 * @param  [in]  write_cb_para 写请求参数。
 * @param  [in]  status        执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*bs_gatts_write_request_callback)(uint16_t dev_id, uint8_t server_id,
    bs_gatts_req_write_cb_t *write_cb_para, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive remote indication confirm.
 * @par Callback invoked when receive remote indication confirm.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  dev_id       Device ID.
 * @param  [in]  server_id    server ID.
 * @param  [in]  inst_id      service instance ID.
 * @param  [in]  status       error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  收到对端indication confirm的回调函数。
 * @par    收到对端indication confirm的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  dev_id        设备 ID。
 * @param  [in]  server_id     服务端 ID。
 * @param  [in]  inst_id       服务实例 ID。
 * @param  [in]  status        执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*bs_gatts_indication_confirm_callback)(uint16_t dev_id, uint8_t server_id, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when mtu size changed.
 * @par Callback invoked when mtu size changed.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @attention 2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  server_id server ID.
 * @param  [in]  dev_id    device ID.
 * @param  [in]  mtu_size  mtu size.
 * @param  [in]  status    error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @else
 * @brief  mtu大小改变的回调函数。
 * @par    mtu大小改变的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  server_id 服务端 ID。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  mtu_size  mtu 大小。
 * @param  [in]  status    执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @see gap_ble_callbacks_t
 * @endif
 */
typedef void (*bs_gatts_mtu_changed_callback)(uint16_t dev_id, uint8_t server_id, uint16_t mtu_size, errcode_t status);

typedef struct {
    bs_gatts_start_service_callback start_service_cb;     /*!< @if Eng Service started callback.
                                                               @else   启动服务回调函数。 @endif */
    bs_gatts_stop_service_callback stop_service_cb;       /*!< @if Eng Service stopped callback.
                                                               @else   停止服务回调函数。 @endif */
    bs_gatts_delete_service_callback delete_service_cb;   /*!< @if Eng Service deleted callback.
                                                                @else   删除服务回调函数。 @endif */
    bs_gatts_read_request_callback read_request_cb;       /*!< @if Eng Read request received callback.
                                                               @else   收到远端读请求回调函数。 @endif */
    bs_gatts_write_request_callback write_request_cb;     /*!< @if Eng Write request received callback.
                                                               @else   收到远端写请求回调函数。 @endif */
    bs_gatts_indication_confirm_callback indicate_cfm_cb; /*!< @if Eng Indicate cfm callback.
                                                               @else   指示确认回调函数。 @endif */
    bs_gatts_mtu_changed_callback mtu_changed_cb;         /*!< @if Eng Mtu changed callback.
                                                               @else   mtu 大小更新回调函数。 @endif */
} bs_gatt_service_callbacks_t;

typedef struct {
    uint8_t resverved; /*!<@if Eng Reserved
                           @else 预留 @endif */
} bs_gatt_service_extend_param_t;

/**
 * @if Eng
 * @brief GATT standard service config structure.
 * @else
 * @brief GATT 服务配置结构。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;                                        /*!< @if Eng uuid of this service.
                                                                @else   该服务uuid。 @endif */
    uint8_t is_primary;                                    /*!< @if Eng is primary service tag
                                                                @else   是否是主服务标记 @endif */
    uint8_t attribute_count;                               /*!< @if Eng size of attribute_array.
                                                                @else   attribute_array中存储的属性（包括特征和描述符）个数 @endif */
    bs_gatt_service_callbacks_t cbks;                      /*!< @if Eng server callback function.
                                                                @else   服务端回调函数 @endif */
    bs_gatt_service_extend_param_t extend_param;           /*!< @if Eng server structure extend param.
                                                                @else   服务端扩展参数 @endif */
    bs_gatts_attribute_info attribute_array[0];            /*!< @if Eng point array of attribute config.
                                                                @else 服务中的属性（包括特征和描述符）数组。@endif */
} bs_gatt_service_template_t;

/**
 * @if Eng
 * @brief  Register gatt server.
 * @par Description: Register gatt server.
 * @param  [in]  app_uuid  App uuid.
 * @param  [out] server_id Server ID.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  注册gatt服务端。
 * @par Description: 注册gatt服务端。
 * @param  [in]  app_uuid  上层应用uuid。
 * @param  [out] server_id 服务端ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_register(bt_uuid_t *app_uuid, uint8_t *server_id);

/**
 * @if Eng
 * @brief  Unregister gatt server.
 * @par Description: Unregister gatt server.
 * @param  [in] server_id Server ID.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  注销gatt服务端。
 * @par Description: 注销gatt服务端。
 * @param  [in] server_id 服务端ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_unregister(uint8_t server_id);

/**
 * @if Eng
 * @brief  Add a gatt service.
 * @par Description: Add a gatt service.
 * @param  [in] server_id Server ID.
 * @param  [in] service Service Template.
 * @param  [out] inst_id ID of the service instance.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  添加gatt服务实例。
 * @par Description: 添加一个gatt服务。
 * @param  [in] server_id 服务端 ID。
 * @param  [in] service   服务模板。
 * @param  [out] inst_id  服务实例ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_service_instance_init(uint8_t server_id, bs_gatt_service_template_t *service, uint8_t *inst_id);

/**
 * @if Eng
 * @brief  Delete a gatt service.
 * @par Description: Delete a gatt service.
 * @param  [in] server_id Server ID.
 * @param  [in] inst_id   ID of the service instance.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  删除一个gatt服务。
 * @par Description: 删除一个gatt服务。
 * @param  [in] server_id 服务端 ID。
 * @param  [in] inst_id   服务实例ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_service_instance_deinit(uint8_t server_id, uint8_t inst_id);

/**
 * @if Eng
 * @brief  Send response when receive the request need to response by user.
 * @par Description: Send response when receive the request need to response by user
                     { @ref bs_gatts_read_request_callback } { @ref bs_gatts_write_request_callback }.
 * @param  [in] server_id server ID.
 * @param  [in] dev_id    device ID.
 * @param  [in] param     response parameter.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  当收到需要用户回复响应的请求时发送响应。
 * @par Description: 当收到需要用户回复响应的请求时发送响应
                     { @ref bs_gatts_read_request_callback } { @ref bs_gatts_write_request_callback }。
 * @param  [in] server_id 服务端 ID.
 * @param  [in] dev_id    设备ID。
 * @param  [in] param     响应参数。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_send_rsp(uint16_t dev_id, uint8_t server_id, bs_gatts_send_rsp_t *param);

/**
 * @if Eng
 * @brief  Send indication or notification to remote device.
 * @par Description: Send indication or notification to remote device,
                     send status depend on character descriptor: client characteristic configuration.
 * @param  [in] server_id server ID.
 * @param  [in] dev_id    device ID.
 * @param  [in] param     notify/indicate parameter.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  向对端发送通知或指示。
 * @par Description: 向对端发送通知或指示，具体发送状态取决于特征描述符：客户端特征配置。
 * @param  [in] server_id 服务端 ID。
 * @param  [in] dev_id    设备ID。
 * @param  [in] param     通知或指示参数。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_notify_indicate(uint16_t dev_id, uint8_t server_id, bs_gatts_ntf_ind_t *param);

/**
 * @if Eng
 * @brief  Set server rx mtu before connected.
 * @par Description: Set server rx mtu before connected.
 * @param  [in] server_id server ID.
 * @param  [in] mtu_size  server rx mtu size.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  在连接之前设置服务端接收mtu。
 * @par Description: 在连接之前设置服务端接收mtu。
 * @param  [in] server_id 服务端ID。
 * @param  [in] mtu_size 服务端接收mtu。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gatts_set_mtu_size(uint8_t server_id, uint16_t mtu_size);

#ifdef __cplusplus
}
#endif
#endif /* BS_BLE_GATT_SERVER_SERVICE_H */