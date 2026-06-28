/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR serivce api
 */

#ifndef BS_BR_SERVICE_API_H
#define BS_BR_SERVICE_API_H

#include <stdbool.h>
#include "errcode.h"
#include "bs_device_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS_BR_DEVICE_NAME_MAX_LEN 32
#define BS_BR_LINK_KEY_LEN 16

/**
 * @if Eng
 * @brief Enum of BR/EDR stack state.
 * @else
 * @brief BR/EDR协议栈状态枚举。
 * @endif
 */
typedef enum {
    BR_STATE_ENABLED,  /*!< @if Eng stack is enabled.
                             @else   协议栈开启。 @endif */
    BR_STATE_DISABLED, /*!< @if Eng stack is disabled.
                             @else   协议栈关闭。 @endif */
} bs_br_state_t;

/**
 * @if Eng
 * @brief Enum of discovery type.
 * @else
 * @brief 发现模式类型枚举。
 * @endif
 */
typedef enum {
    BR_DISCOVERY_CENTRAL,    /*!< @if Eng enter discovery mode as central device, start inquiry.
                                       @else   作为中央设备进入发现态，即查询外围设备。 @endif */
    BR_DISCOVERY_PERIPHERAL, /*!< @if Eng enter discovery mode as peripheral device, start scan.
                                       @else   作为外围设备进入发现态，即进入scan状态。 @endif */
} bs_br_discovery_type_t;

/**
 * @if Eng
 * @brief Enum of BR/EDR connect state.
 * @else
 * @brief BR/EDR连接状态枚举。
 * @endif
 */
typedef enum {
    BR_CONNECT_NONE,         /*!< @if Eng state none, the default state.
                                        @else   默认连接状态。 @endif */
    BR_CONNECT_CONNECTED,    /*!< @if Eng connected state.
                                        @else   已连接状态。 @endif */
    BR_CONNECT_DISCONNECTED, /*!< @if Eng disconnected state.
                                        @else   断连状态。 @endif */
} bs_br_connect_state_t;

/**
 * @if Eng
 * @brief  Enum of bt profile connection status.
 * @else
 * @brief  蓝牙服务连接状态。
 * @endif
 */
typedef enum {
    BR_PROFILE_STATE_CONNECTING = 0x01,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    BR_PROFILE_STATE_CONNECTED = 0x02,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    BR_PROFILE_STATE_DISCONNECTING = 0x03, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    BR_PROFILE_STATE_DISCONNECTED = 0x04   /*!< @if Eng status of disconnected. @else 已断连。 @endif */
} bs_br_profile_connect_state_t;

/**
 * @if Eng
 * @brief Enum of BR/EDR encrypt state.
 * @else
 * @brief BR/EDR加密状态枚举。
 * @endif
 */
typedef enum {
    BR_ENCRYPT_NONE,    /*!< @if Eng state none, the default state.
                                   @else   默认加密状态。 @endif */
    BR_ENCRYPT_SUCCESS, /*!< @if Eng encrypt success.
                                   @else   加密成功。 @endif */
    BR_ENCRYPT_FAILED,  /*!< @if Eng encrypt failed.
                                   @else   加密失败。 @endif */
} bs_br_encrypt_state_t;

/**
 * @if Eng
 * @brief Enum of rejecting connect reason.
 * @else
 * @brief 拒绝连接原因。
 * @endif
 */
typedef enum {
    BR_CONNECT_REJECT_REASON_LIMITED_RESOURCES ,   /*!< @if Eng Rejected due to limited resources.
                                                        @else 资源有限。 @endif */
    BR_CONNECT_REJECT_REASON_SECURITY_REASONS,     /*!< @if Eng Security requirements not being fulfilled.
                                                        @else 未满足安全要求。 @endif */
    BR_CONNECT_REJECT_REASON_UNACCEPTABLE_BD_ADDR, /*!< @if Eng This device does not accept the BD_ADDR.
                                                                This may be because the device will only
                                                                accept connections from specific BD_ADDRs.
                                                        @else 不接受该地址，只接受特定地址连接。 @endif */
} bs_br_connect_reject_reason_t;

/**
 * @if Eng
 * @brief Structure of pair information.
 * @else
 * @brief 配对信息。
 * @endif
 */
typedef struct {
    uint8_t link_key[BS_BR_LINK_KEY_LEN]; /*!< @if Eng link key.
                                               @else 配对密钥。 @endif */
    uint8_t key_type;                     /*!< @if Eng key type.
                                               @else 密钥类型。 @endif */
} bs_br_pair_info_t;

/**
 * @if Eng
 * @brief Structure of discovery result.
 * @else
 * @brief 设备发现结果。
 * @endif
 */
typedef struct {
    bs_addr_t addr;
    uint8_t name[BS_BR_DEVICE_NAME_MAX_LEN];
    uint32_t device_class;
    int8_t rssi;
} bs_br_discovery_result_t;

/**
 * @if Eng
 * @brief Structure of remote device information.
 * @else
 * @brief 远端设备信息。
 * @endif
 */
typedef struct {
    uint8_t  addr[BS_ADDR_LEN];           /*!< @if Eng address of device.
                                               @else 设备地址。 @endif */
    uint32_t class_of_device;             /*!< @if Eng class of device.
                                               @else 设备类型。 @endif */
    uint8_t  name[BS_BR_DEVICE_NAME_MAX_LEN];    /*!< @if Eng name of device, include the end '\0'.
                                               @else 设备名称, 包含字符串结束符。 @endif */
} bs_br_remote_device_info_t;

/**
 * @if Eng
 * @brief  Callback invoked when stack enabled / disabled.
 * @par    When registered, this callback notifies the upper layer
 *         when br stack is enabled or disabled.
 * @param  [in]  state br stack state.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  协议栈使能/去使能的回调函数。
 * @par    注册该回调函数之后，stack使能或者去使能后该回调函数会上报上层。
 * @param  [in]  state br协议栈状态。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_stack_state_cbk)(uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when the device requesting connect.
 * @par    When registered, this callback notifies the upper layer
 *         when the device requesting connect.
 * @param  [in]  dev_id         device id.
 * @param  [in]  dev_addr       device address.
 * @param  [in]  dev_class      class of device.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  连接请求的回调函数。
 * @par    注册该回调函数之后，设备请求连接时该回调函数会上报上层。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  dev_addr   设备地址。
 * @param  [in]  dev_class  设备类型。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_connect_request_cbk)(uint16_t dev_id, const bs_addr_t *dev_addr, uint32_t dev_class);

/**
 * @if Eng
 * @brief  Callback invoked when br connected / disconnected.
 * @par    When registered, this callback notifies the upper layer
 *         when br is connected or disconnected.
 * @param  [in]  dev_id         device id.
 * @param  [in]  conn_state     br connect state {@ref bs_br_connect_state_t}.
 * @param  [in]  encrypt_state  br encrypt state {@ref bs_br_encrypt_state_t}.
 * @param  [in]  reason         br disconnect reason or encrypt fail reason.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  br连接/断连的回调函数。
 * @par    注册该回调函数之后，br连接或者断连后该回调函数会上报上层。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  conn_state br连接状态{@ref bs_br_connect_state_t}。
 * @param  [in]  enc_state  br加密状态{@ref bs_br_encrypt_state_t}。
 * @param  [in]  reason     br断连原因或加密失败原因。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_connect_state_cbk)(uint16_t dev_id, uint8_t conn_state, uint8_t encrypt_state, uint32_t reason);

/**
 * @if Eng
 * @brief  Callback invoked when the device pair complete.
 * @par    When registered, this callback notifies the upper layer
 *         when the device pair complete.
 * @param  [in]  dev_id         device id.
 * @param  [in]  pair_info      pair information {@ref bs_br_pair_info_t}.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  配对完成的回调函数。
 * @par    注册该回调函数之后，设备配对完成后该回调函数会上报上层。
 * @param  [in]  dev_id     设备id。
 * @param  [in]  pair_info  配对信息{@ref bs_br_pair_info_t}。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_pair_complete_cbk)(uint16_t dev_id, const bs_br_pair_info_t *pair_info);

/**
 * @if Eng
 * @brief  Callback invoked when the device pair removed.
 * @par    When registered, this callback notifies the upper layer
 *         when the device pair removed.
 * @param  [in]  dev_id         device id.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  配对移除的回调函数。
 * @par    注册该回调函数之后，设备配对移除后该回调函数会上报上层。
 * @param  [in]  dev_id     设备id。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_pair_remove_cbk)(uint16_t dev_id);

/**
 * @if Eng
 * @brief  Callback invoked when new device is discovered.
 * @par    When registered, this callback notifies the upper layer
 *         when new device the device is discovered.
 * @param  [in]  discovery_result         discovery result.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  设备发现结果的回调函数。
 * @par    注册该回调函数之后，发现新设备后该回调函数会上报上层。
 * @param  [in]  discovery_result     设备发现结果。
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_discovery_result_cbk)(const bs_br_discovery_result_t *discovery_result);

/**
 * @if Eng
 * @brief  Callback invoked when remote name is reported.
 * @par    When registered, this callback notifies the upper layer
 *         when remote device name is reported.
 * @param  [in]  dev_id         device id.
 * @param  [in]  name           device name.
 * @param  [in]  length         length of name.
 * @par Dependency:
 * @see bs_br_callbacks_t
 * @else
 * @brief  设备发现结果的回调函数。
 * @par    注册该回调函数之后，发现新设备后该回调函数会上报上层。
 * @param  [in]  dev_id         设备ID.
 * @param  [in]  name           设备名.
 * @param  [in]  length         设备名长度.
 * @par 依赖:
 * @see bs_br_callbacks_t
 * @endif
 */
typedef void (*bs_br_remote_name_changed_cbk)(uint16_t dev_id, const uint8_t *name, uint8_t length);

/**
 * @if Eng
 * @brief Struct of BR service callback function.
 * @else
 * @brief BR service回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_br_stack_state_cbk stack_state_cb;
    bs_br_connect_request_cbk connect_request_cb;
    bs_br_connect_state_cbk connect_state_cb;
    bs_br_pair_complete_cbk pair_complete_cb;
    bs_br_pair_remove_cbk pair_remove_cb;
    bs_br_discovery_result_cbk discovery_result_cb;
    bs_br_remote_name_changed_cbk remote_name_changed_cb;
} bs_br_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register the callback functions for the br/edr.
 * @par Use this function to register the callback functions for the br/edr.
 * @param [in] app_id application id {@ref bs_ble_callbacks_t}.
 * @param [in] cbks callback functions {@ref bs_br_callbacks_t}.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  注册br/edr回调。
 * @par    注册br/edr回调。
 * @param [in] app_id 上层应用ID {@ref bs_ble_callbacks_t}。
 * @param [in] cbks 回调结构体 {@ref bs_br_callbacks_t}。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_register_callbacks(uint8_t app_id, const bs_br_callbacks_t *cbks);

/**
 * @if Eng
 * @brief Use this function to enable br stack.
 * @par Use this function to enable br stack.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  使能br协议栈。
 * @par    使能br协议栈。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_enable(void);

/**
 * @if Eng
 * @brief Use this function to disable br stack.
 * @par Use this function to disable br stack.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  去使能br协议栈。
 * @par    去使能br协议栈。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_disable(void);

/**
 * @if Eng
 * @brief Use this function to enter discovery mode.
 * @par Use this function to enter discovery mode.
 * @param [in] type type of discovery mode {@ref bs_br_discovery_type_t}.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  基于br/edr进入设备发现模式。
 * @par    基于br/edr进入设备发现模式。
 * @param [in] type 设备发现模式类型 {@ref bs_br_discovery_type_t}。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_start_discovery(bs_br_discovery_type_t type);

/**
 * @if Eng
 * @brief Use this function to stop discovery mode.
 * @par Use this function to stop discovery mode.
 * @param [in] type type of discovery mode {@ref bs_br_discovery_type_t}.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  退出设备发现模式。
 * @par    退出设备发现模式。
 * @param [in] type 设备发现模式类型 {@ref bs_br_discovery_type_t}。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_stop_discovery(bs_br_discovery_type_t type);

/**
 * @if Eng
 * @brief Use this function to start pair procedure device based on br/edr.
 * @par Use this function to start pair procedure device based on br/edr.
 * @param [in] dev_id remote device id.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  基于br/edr进行配对设备流程。
 * @par    基于br/edr进行配对设备流程。
 * @param [in] dev_id 远端设备ID。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_start_pair(const bs_addr_t *addr);

/**
 * @if Eng
 * @brief Use this function to remove pair info and disconnect.
 * @par Use this function to remove pair info and disconnect.
 * @param [in] dev_id remote device id.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  删除配对并断连。
 * @par    删除配对并断连。
 * @param [in] dev_id 远端设备ID。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_remove_pair(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to reconnect device by br/edr.
 * @par Use this function to reconnect device by br/edr.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  设备回连。
 * @par    设备回连。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_start_auto_conn(void);

/**
 * @if Eng
 * @brief Use this function to stop reconnect device by br/edr.
 * @par Use this function to stop reconnect device by br/edr.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  停止设备回连。
 * @par    停止设备回连。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_stop_auto_conn(void);

/**
 * @if Eng
 * @brief Use this function to accept connect.
 * @par Use this function to accept connect.
 * @param [in] dev_id remote device id.
 * @param [in] accept whether to accept.
 * @param [in] reason reject reject reason {@ref bs_br_connect_reject_reason_t}.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  接受连接。
 * @par    接受连接。
 * @param [in] dev_id 远端设备ID。
 * @param [in] accept 是否接受。
 * @param [in] reason 拒绝原因 {@ref bs_br_connect_reject_reason_t}。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_accept_connect(uint16_t dev_id, bool accept, uint8_t reason);

/**
 * @if Eng
 * @brief Use this function to disconnect the device.
 * @par Use this function to disconnect the device.
 * @param [in] dev_id remote device id.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  断开连接。
 * @par    断开连接。
 * @param [in] dev_id 远端设备ID。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_disconnect(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to get remote device information by device ID.
 * @par Use this function to get remote device information by device ID.
 * @param [in]  dev_id      remote device id.
 * @param [out] device_info remote device information.
 * @retval error code
 * @par Dependency:
 * @li
 * @else
 * @brief  基于设备ID获取对端设备信息。
 * @par    基于设备ID获取对端设备信息。
 * @param [in]  dev_id      远端设备ID。
 * @param [out] device_info 远端设备信息。
 * @retval 执行结果错误码。
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_br_get_remote_device_info(uint16_t dev_id, bs_br_remote_device_info_t *device_info);

#ifdef __cplusplus
}
#endif
#endif /* BS_BR_SERVICE_API_H */