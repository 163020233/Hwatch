/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BLE GATT Client Service
 */

#ifndef BS_BLE_GATT_CLIENT_SERVICE_H
#define BS_BLE_GATT_CLIENT_SERVICE_H

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
 * @brief Struct of handle value information.
 * @else
 * @brief 句柄值信息。
 * @endif
 */
typedef struct {
    uint16_t handle;   /*!< @if Eng attribute handle.
                            @else   属性句柄。@endif */
    uint16_t data_len; /*!< @if Eng data length.
                            @else   数据长度。@endif */
    uint8_t *data;     /*!< @if Eng data.
                            @else   数据。@endif */
} bs_gattc_handle_value_t;

/**
 * @if Eng
 * @brief Struct of handle write information.
 * @else
 * @brief 客户端写信息。
 * @endif
 */
typedef struct {
    uint16_t handle;   /*!< @if Eng attribute handle.
                            @else   属性句柄。@endif */
    bool is_need_confirm; /*!< @if WWhether to require peer confirmation: false: no peer response needed, true: the
                           result is returned in the bs_gattc_write_cfm_callback callback after peer confirmation.
                           @else   是否需要对端确认, false：无需对端回复,
                           true:对端确认后结果在bs_gattc_write_cfm_callback回调中返回。@endif */
    uint16_t data_len; /*!< @if Eng data length.
                            @else   数据长度。@endif */
    uint8_t *data;     /*!< @if Eng data.
                            @else   数据。@endif */
} bs_gattc_write_value_t;

/**
 * @if Eng
 * @brief Struct of discovery service result.
 * @else
 * @brief 发现服务结果。
 * @endif
 */
typedef struct {
    uint16_t start_hdl; /*!< @if Eng service start handle.
                             @else   服务起始句柄。@endif */
    uint16_t end_hdl;   /*!< @if Eng service stop handle.
                             @else   服务结束句柄。@endif */
    bt_uuid_t uuid;     /*!< @if Eng service uuid.
                             @else   服务uuid。@endif */
} bs_gattc_discovery_service_result_t;

/**
 * @if Eng
 * @brief Struct of discovery character result.
 * @else
 * @brief 发现特征结果。
 * @endif
 */
typedef struct {
    bt_uuid_t uuid;          /*!< @if Eng character uuid.
                                  @else   特征uuid。@endif */
    uint16_t declare_handle; /*!< @if Eng character declare handle.
                                  @else   特征声明句柄。@endif */
    uint16_t value_handle;   /*!< @if Eng character value handle.
                                  @else   特征值句柄。@endif */
    uint8_t properties;      /*!< @if Eng character properties { @ref bs_gatt_characteristic_property_t }.
                                  @else   特征特性 { @ref bs_gatt_characteristic_property_t }。@endif */
} bs_gattc_discovery_character_result_t;

/**
 * @if Eng
 * @brief Struct of discovery character descriptor result.
 * @else
 * @brief 发现特征描述符结果。
 * @endif
 */
typedef struct {
    uint16_t descriptor_hdl; /*!< @if Eng character descriptor handle.
                                  @else   特征描述符句柄。@endif */
    bt_uuid_t uuid;          /*!< @if Eng character descriptor uuid.
                                  @else   特征描述符uuid。@endif */
} bs_gattc_discovery_descriptor_result_t;

/**
 * @if Eng
 * @brief A service information about a remote object.
 * @else
 * @brief 远端对象的服务信息发现结果。
 * @endif
 */
typedef struct {
    bs_gattc_discovery_service_result_t service;          /*!< @if Eng service information
                                                               @else 服务信息。
                                                               @endif */
    uint8_t character_count;                              /*!< @if Eng Number of service character.
                                                              @else 服务的特征个数。 @endif */
    bs_gattc_discovery_character_result_t *character_list;   /*!< @if Eng character items array.
                                                               @else 特征元素数组。
                                                               @endif */
    uint8_t descriptor_count;                             /*!< @if Eng Number of service descriptor.
                                                               @else 服务的描述符个数。 @endif */
    bs_gattc_discovery_descriptor_result_t *descriptor_list; /*!< @if Eng descriptor items array.
                                                               @else 描述符元素数组。
                                                               @endif */
} bs_gattc_find_service_result_node_t;

/**
 * @if Eng
 * @brief Callback invoked when service discovery completed.
 * @par Callback invoked when service discovery completed.
 * @attention 1.This function is called in BLE service context,should not be blocked or do long time waiting.
 * @attention 2.The memories of pointer are requested and freed by the BLE service automatically.
 * @param [out] dev_id    Device ID.
 * @param [out] client_id Client ID.
 * @param [out] service_count Number of discovered services.
 * @param [out] service   service items array.
 * @param [out] status    error code. @ref errcode_bt_t
 * @retval #void no return value.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  服务发现的回调函数。
 * @par    服务发现的回调函数。
 * @attention  1. 该回调函数运行于BLE service线程，不能阻塞或长时间等待。
 * @attention  2. pointer由BLE service申请内存，也由BLE service释放，回调中不应释放。
 * @param [out] dev_id    设备 ID。
 * @param [out] client_id 客户端 ID。
 * @param [out] service_count 发现到的服务个数
 * @param [out] service   服务条目数组。
 * @param [out] status    执行结果错误码。 @ref errcode_bt_t
 * @retval 无返回值。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_find_service_callback)(uint16_t dev_id, uint8_t client_id,
    uint8_t service_count, bs_gattc_find_service_result_node_t *service, errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive read response.
 * @par Callback invoked when receive read response.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id      Device ID.
 * @param  [in]  client_id   client ID.
 * @param  [in]  read_result read result.
 * @param  [in]  status      GATT error code. { @ref bs_gatt_status_t }
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  收到读响应的回调函数。
 * @par    收到读响应的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id      设备 ID。
 * @param  [in]  client_id   客户端 ID。
 * @param  [in]  read_result 读取结果。
 * @param  [in]  status      GATT错误码。{ @ref bs_gatt_status_t }
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_read_cfm_callback)(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *read_result,
    uint16_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive write response.
 * @par Callback invoked when receive write response.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  client_id client ID.
 * @param  [in]  handle    write attribute handle.
 * @param  [in]  status    GATT error code. { @ref bs_gatt_status_t }
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  收到写响应的回调函数。
 * @par    收到写响应的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  handle    请求写的句柄。
 * @param  [in]  status    GATT错误码。{ @ref bs_gatt_status_t }
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_write_cfm_callback)(uint16_t dev_id, uint8_t client_id, uint16_t handle, uint16_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive notification.
 * @par Callback invoked when receive notification.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  client_id Client ID.
 * @param  [in]  data      data.
 * @param  [in]  status    error code. @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  收到通知的回调函数。
 * @par    收到通知的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  data      数据。
 * @param  [in]  status    执行结果错误码。@ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_notification_callback)(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *data,
    errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when receive indication.
 * @par Callback invoked when receive indication.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  client_id Client ID.
 * @param  [in]  data      data.
 * @param  [in]  status    error code. @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  收到指示的回调函数。
 * @par    收到指示的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  data      数据。
 * @param  [in]  status    执行结果错误码。@ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_indication_callback)(uint16_t dev_id, uint8_t client_id, bs_gattc_handle_value_t *data,
    errcode_t status);

/**
 * @if Eng
 * @brief Callback invoked when mtu changed.
 * @par Callback invoked when mtu changed.
 * @attention 1.This function is called in bts context,should not be blocked or do long time waiting.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  client_id Client ID.
 * @param  [in]  mtu_size  mtu size.
 * @param  [in]  status    error code. @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  mtu改变的回调函数。
 * @par    mtu改变的回调函数。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  mtu_size  mtu大小。
 * @param  [in]  status    执行结果错误码。@ref errcode_bt_t
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
typedef void (*bs_gattc_mtu_changed_callback)(uint16_t dev_id, uint8_t client_id, uint16_t mtu_size, errcode_t status);

/**
 * @if Eng
 * @brief Struct of gatt client read and write callback function.
 * @else
 * @brief gatt client读写回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_gattc_read_cfm_callback read_cb;                  /*!< @if Eng Receive read response callback.
                                                            @else   收到读响应回调函数。 @endif */
    bs_gattc_write_cfm_callback write_cb;                /*!< @if Eng Receive write response callback.
                                                           @else   收到写响应回调函数。 @endif */
} bs_gattc_read_write_callbacks_t;

/**
 * @if Eng
 * @brief Struct of gatt client notification and indication callback function.
 * @else
 * @brief gatt client notification和indication回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_gattc_notification_callback notification_cb; /*!< @if Eng Receive notification callback.
                                              @else   收到通知回调函数。 @endif */
    bs_gattc_indication_callback indication_cb;     /*!< @if Eng Receive indication callback.
                                          @else   收到指示回调函数。 @endif */
} bs_gattc_notication_indication_callbacks_t;

/**
 * @if Eng
 * @brief Struct of gatt client operation callback function.
 * @else
 * @brief gatt client 操作类接口回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_gattc_read_write_callbacks_t read_write_cbk; /*!< @if Eng Struct of gatt client read and write callback function.
                                                      @else gatt client读写回调函数接口定义。
                                                      @endif */
    bs_gattc_notication_indication_callbacks_t notication_indication_cbk;
    /*!< @if Eng Struct of gatt client notification and indication callback function.
         @else gatt client notification和indication回调函数接口定义。
         @endif */
    bs_gattc_mtu_changed_callback mtu_changed_cb; /*!< @if Eng Mtu changed callback,
                                                    @else mtu改变回调函数 @endif */
} bs_gattc_operation_cbk_t;

/**
 * @if Eng
 * @brief  Register gatt client.
 * @par Description: Register gatt client.
 * @param  [in]  app_uuid  App uuid.
 * @param  [out] client_id Client ID.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  注册gatt客户端。
 * @par Description: 注册gatt客户端。
 * @param  [in]  app_uuid  上层应用uuid。
 * @param  [out] client_id 客户端ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_register(bt_uuid_t *app_uuid, uint8_t *client_id);

/**
 * @if Eng
 * @brief  Unregister gatt client.
 * @par Description: Unregister gatt client.
 * @param  [in] client_id Client ID.
 * @retval ERRCODE_BT_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  注销gatt客户端。
 * @par Description: 注销gatt客户端。
 * @param  [in] client_id 客户端ID。
 * @retval ERRCODE_BT_SUCCESS 成功。
 * @retval Other        失败。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_unregister(uint8_t client_id);

/**
 * @if Eng
 * @brief  Find all structure.
 * @par Description: Find all structure.
 * @param [in] client_id Client ID.
 * @param [in] dev_id    Device ID.
 * @param [in] find_structure_cbk  callback when discovery structure complete.
 * @param [in] is_remote  true：force discovery structure from remote dev，false：return find structure from cache.
 * @retval error code. See @ref errcode_bt_t, discovery all structure result
                       will be returned at { @ref gattc_find_structure_all_callback }
 * @par Depends:
 * @li  bts_def.h
 * @else
 * @brief  发现服务结构。
 * @par Description: 查找服务、特征、描述符。
 * @param [in] client_id 客户端 ID。
 * @param [in] dev_id    设备 ID。
 * @param [in] find_structure_cbk  查找服务完成回调。
 * @param [in] is_remote true:强制从远端设备获取服务,  false: 如果本地有缓存则从缓存中获取所有服务.
 * @retval 执行结果错误码,参考 @ref errcode_bt_t, 服务发现结果将在 { @ref bs_gattc_find_service_callback }中返回。
 * @par 依赖：
 * @li  bts_def.h
 * @endif
 */
errcode_t bs_gattc_find_all_service(
    uint16_t dev_id, uint8_t client_id, bs_gattc_find_service_callback find_structure_cbk, bool is_remote);

/**
 * @if Eng
 * @brief  Find service group by uuid.
 * @par Description: Find all structure.
 * @param [in] client_id Client ID.
 * @param [in] dev_id    Device ID.
 * @param [in] uuid_num  Number of UUIDs to be discovered.
 * @param [in] uuid      UUIDs to be discovered.
 * @param [in] find_structure_cbk  callback when discovery structure complete.
 * @param [in] is_remote  true：force discovery structure from remote dev，false：return find structure from cache .
 * @retval error code. See @ref errcode_bt_t, discovery all structure result
                       will be returned at { @ref gattc_find_structure_all_callback }
 * @par Depends:
 * @li  bts_def.h
 * @else
 * @brief  通过uuid发现服务结构。
 * @par Description: 查找服务、特征、描述符。
 * @param [in] client_id 客户端 ID。
 * @param [in] dev_id    设备 ID。
 * @param [in] uuid_num  待发现的uuid数量
 * @param [in] uuid      待发现的uuid
 * @param [in] find_structure_cbk  查找服务完成回调。
 * @retval 执行结果错误码, 参考 @ref errcode_bt_t, 服务发现结果将在 { @ref bs_gattc_find_service_callback }中返回。
 * @par 依赖：
 * @li  bts_def.h
 * @endif
 */
errcode_t bs_gattc_find_service_by_uuid(uint16_t dev_id, uint8_t client_id,
    uint8_t uuid_num, bt_uuid_t *uuid_list, bs_gattc_find_service_callback find_structure_cbk);
/**
 * @if Eng
 * @brief  Register GATT client multi-service callbacks.
 * @par Description: Register GATT client multi-service callbacks.
 * @param  [in] client_id    client ID.
 * @param  [in] dev_id       Device ID.
 * @param  [in] service_uuid service UUID.
 * @param  [in] func         callback function.
 * @retval error code. See @ref errcode_bt_t
 * @par Notes
 *     1. This interface is mutually exclusive with the gattc_register_callbacks interface.
 *        Only one interface can be used.
 *     2. The current interface needs to be invoked before start service.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  注册 GATT client 多服务回调函数。
 * @par Description: 注册多服务回调函数。
 * @param  [in] client_id    客户端ID。
 * @param  [in] dev_id       设备 ID.
 * @param  [in] service_uuid 服务唯一标识。
 * @param  [in] func         回调函数。
 * @retval 执行结果错误码。参考 @ref errcode_bt_t
 * @par 注意事项
 *     1、与gattc_register_callbacks接口互斥，只能使用一个；
 *     2、当前接口需要在服务发现完成之后调用；
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_register_callbacks(uint16_t dev_id, uint8_t client_id, bt_uuid_t *service_uuid,
    bs_gattc_operation_cbk_t *func);

/**
 * @if Eng
 * @brief  Unregister GATT client multi-service callbacks.
 * @par Description: Unregister GATT client multi-service callbacks.
 * @param  [in] client_id    client ID.
 * @param  [in] dev_id       Device ID.
 * @param  [in] service_uuid service UUID.
 * @retval error code. See @ref errcode_bt_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  去注册 GATT client 多服务回调函数。
 * @par Description: 去注册多服务回调函数。
 * @param  [in] client_id    客户端ID。
 * @param  [in] dev_id       设备 ID.
 * @param  [in] service_uuid 服务唯一标识。
 * @retval 执行结果错误码。参考 @ref errcode_bt_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_unregister_callbacks(uint16_t dev_id, uint8_t client_id, bt_uuid_t *service_uuid);

/**
 * @if Eng
 * @brief  Send read by handle request.
 * @par Description: Send read by handle request.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  handle    handle.
 * @retval error code, read result will be returned at { @ref bs_gattc_read_cfm_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  发起按照句柄读取请求。
 * @par Description: 发起按照句柄读取请求。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  handle    句柄。
 * @retval 执行结果错误码，读取结果将在 { @ref bs_gattc_read_cfm_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_read(uint16_t dev_id, uint8_t client_id, uint16_t handle);

/**
 * @if Eng
 * @brief  Send write request.
 * @par Description: Send write request.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  param     Parameter for write request.
 * @retval error code, write result will be returned at { @ref bs_gattc_write_cfm_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  发起写请求。
 * @par Description: 发起写请求。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  param     写信息参数。
 * @retval 执行结果错误码，写结果将在 { @ref bs_gattc_write_cfm_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_write(uint16_t dev_id, uint8_t client_id, bs_gattc_write_value_t *param);

/**
 * @if Eng
 * @brief  Send exchange mtu request.
 * @par Description: Send exchange mtu request.
 * @param  [in]  client_id Client ID.
 * @param  [in]  dev_id    Device ID.
 * @param  [in]  mtu_size  Client rx mtu.
 * @retval error code, mtu change result will be returned at { @ref bs_gattc_mtu_changed_callback }.
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  发送交换mtu请求。
 * @par Description: 发送交换mtu请求。
 * @param  [in]  client_id 客户端 ID。
 * @param  [in]  dev_id    设备 ID。
 * @param  [in]  mtu_size  客户端接收mtu。
 * @retval 执行结果错误码， mtu改变结果将在 { @ref bs_gattc_mtu_changed_callback }中返回。
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
errcode_t bs_gattc_exchange_mtu_req(uint16_t dev_id, uint8_t client_id, uint16_t mtu_size);

#ifdef __cplusplus
}
#endif
#endif /* BS_BLE_GATT_CLIENT_SERVICE_H */