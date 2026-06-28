/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BLE serivce api
 */

#ifndef BS_BLE_SERVICE_API_H
#define BS_BLE_SERVICE_API_H

#include "errcode.h"
#include "bs_device_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_SVC_SMP_LTK_LEN 16
#define BLE_SVC_SMP_IRK_LEN 16
#define BLE_SVC_ADV_DEV_NAME_LEN 32

/**
 * @if Eng
 * @brief Enum of BLE stack state.
 * @else
 * @brief BLE协议栈状态枚举.
 * @endif
 */
typedef enum {
    BLE_STATE_ENABLED,     /*!< @if Eng BLE stack is enabled.
                                @else   BLE协议栈已使能。 @endif */
    BLE_STATE_DISABLED,    /*!< @if Eng BLE stack is disabled.
                                @else   BLE协议栈已去使能。 @endif */
} bs_ble_state_t;

/**
 * @if Eng
 * @brief Enum of BLE connect state.
 * @else
 * @brief BLE连接状态枚举.
 * @endif
 */
typedef enum {
    BLE_CONNECT_CONNECTED,
    BLE_CONNECT_DISCONNECTED,
} bs_ble_conn_state_t;

/**
 * @if Eng
 * @brief Enum of BLE encrypt state.
 * @else
 * @brief BLE加密状态枚举.
 * @endif
 */
typedef enum {
    BLE_ENCRYPT_NONE,
    BLE_ENCRYPT_SUCCESS,
    BLE_ENCRYPT_FAILED,
} bs_ble_encrypt_state_t;

/**
 * @if Eng
 * @brief Enum of BLE encrypt state.
 * @else
 * @brief 本地角色枚举.
 * @endif
 */
typedef enum {
    BLE_SVC_LOCAL_CENTRAL,
    BLE_SVC_LOCAL_PERIPHERAL,
} bs_ble_local_role_t;

/**
 * @if Eng
 * @brief Enum of discovery type.
 * @else
 * @brief 发现模式枚举.
 * @endif
 */
typedef enum {
    BLE_DISCOVERY_CENTRAL,       /*!< @if Eng enter discovery mode as central device, aka start scan.
                                     @else   作为中央设备进入发现态，即扫描。 @endif */
    BLE_DISCOVERY_PERIPHERAL,    /*!< @if Eng enter discovery mode as peripheral device, aka start advertise.
                                     @else   作为外围设备进入发现态，即广播。 @endif */
} bs_ble_discovery_type_t;

/**
 * @if Eng
 * @brief Enum of BLE link switch state.
 * @else
 * @brief BLE链路切换状态枚举.
 * @endif
 */
typedef enum {
    BS_BLE_ACL_SWITCHING,
    BS_BLE_ACL_SWITCH_SUCC,
    BS_BLE_ACL_SWITCH_FAIL,
} bs_ble_acl_switch_state_t;

/**
 * @if Eng
 * @brief Enum of BLE advertising status.
 * @else
 * @brief BLE广播状态
 * @endif
 */
typedef enum {
    BS_BLE_ADV_STATUS_STOPPED = 0x00, /*!< @if Eng advertising stoped
                                    @else   广播停止 @endif */
    BS_BLE_ADV_STATUS_ADVERTISING,    /*!< @if Eng advertising
                                    @else   正在广播 @endif */
    BS_BLE_ADV_STATUS_UNKNOWN = 0xFF, /*!< @if Eng unknown
                                    @else   未知的 @endif */
} bs_ble_adv_status_t;

/**
 * @if Eng
 * @brief Enum of power on state.
 * @else
 * @brief 设备上电状态枚举.
 * @endif
 */
typedef enum {
    BS_BLE_POWER_ON_UNKNOWN    = 0x00, /*!< @if power on state is unknown
                                    @else   设备上电状态未知 @endif */
    BS_BLE_POWER_ON_SUCCESS    = 0x01, /*!< @if power on state is successful
                                    @else   设备上电状态成功 @endif */
    BS_BLE_POWER_ON_FAILED     = 0x02, /*!< @if power on state is fail
                                    @else   设备上电状态失败 @endif */
} bs_ble_power_on_status_t;

/**
 * @if Eng
 * @brief Enum of switch for frequency scanning function
 * @else
 * @brief 扫频功能开关参数值.
 * @endif
 */
typedef enum {
    BS_BLE_GAP_SET_CHANNEL_SCAN_SWITCH_OFF = 0x00,             /*!< @if Eng OFF
                                                    @else   扫频功能关闭 @endif */
    BS_BLE_GAP_SET_CHANNEL_SCAN_SWITCH_MASTER_ON = 0x01,       /*!< @if Eng
    Enabling the frequency scanning function on the master node of the link
                                                                @else   链路上主节点开启扫频功能 @endif */
    BS_BLE_GAP_SET_CHANNEL_SCAN_SWITCH_SLAVE_ON = 0x02,        /*!< @if Eng
    Enabling the frequency scanning function on the slave node of the link
                                                                @else   链路上从节点开启扫频功能 @endif */
    BS_BLE_GAP_SET_CHANNEL_SCAN_SWITCH_MASTER_SLAVE_ON = 0x03, /*!< @if Eng
    Enabling the frequency scanning function on the master and slave node of the link
                                                                @else   链路上主/从节点均开启扫频功能 @endif */
} bs_ble_gap_set_channel_scan_switch_value_t;

/**
 * @if Eng
 * @brief Structure of pair information.
 * @else
 * @brief 配对信息.
 * @endif
 */
typedef struct {
    uint8_t ltk[BLE_SVC_SMP_LTK_LEN];
    uint8_t rmt_irk[BLE_SVC_SMP_IRK_LEN];
} bs_ble_pair_info_report_t;

/**
 * @if Eng
 * @brief Structure of discovery result information.
 * @else
 * @brief 发现结果信息.
 * @endif
 */
typedef struct {
    bs_addr_t addr;                         /*!< @if Eng Address
                                                 @else   地址 @endif */
    uint8_t name[BLE_SVC_ADV_DEV_NAME_LEN]; /*!< @if Eng device name
                                                 @else   设备名称 @endif */
    int8_t tx_power;                       /*!< @if Eng Transmissive power
                                                @else   发送功率，范围: -127 ~ +20dBm @endif */
    int8_t rssi;                            /*!< @if Eng RSSI
                                                 @else   信号强度，范围: -127 ~ +20dBm @endif */
    uint8_t event_type;                     /*!< @if Eng Event type { @ref gap_ble_scan_result_evt_type_t }
                                                 @else   事件类型 { @ref gap_ble_scan_result_evt_type_t } @endif */
} bs_ble_discovery_result_t;

typedef struct {
    uint16_t dev_id;      /*!< @if Eng device ID
                                   @else   设备ID @endif */
    uint16_t maxtxoctets; /*!< @if Eng maxtxoctets
                          @else   最大字节数 @endif */
    uint16_t maxtxtime;   /*!< @if Eng maxtxtime
                           @else   最大发送时间 @endif */
} bs_ble_set_data_length_t;

/**
 * @if Eng
 * @brief connection result.
 * @else
 * @brief 连接结果.
 * @endif
 */
typedef struct {
    uint16_t dev_id;       /*!< @if Eng device ID.
                                @else   设备ID. */
    bs_addr_t addr;        /*!< @if Eng device address.
                                @else   设备地址. */
    uint8_t conn_state;    /*!< @if Eng ble connect state. {@ref bs_ble_conn_state_t}
                                @else   ble连接状态. {@ref bs_ble_conn_state_t} */
    uint8_t encrypt_state; /*!< @if ble encrypt state.
                                @else   ble加密状态. {@ref bs_ble_encrypt_state_t} */
    uint32_t reason;       /*!< @if ble disconnect reason.
                                @else   ble断连原因. */
    uint8_t local_role;    /*!< @if Eng local role.
                                @else   本地角色. {@ref bs_ble_local_role_t} */
} bs_ble_connection_result_t;

/**
 * @if Eng
 * @brief paired device infos.
 * @else
 * @brief 配对信息查询结果.
 * @endif
 */
typedef struct {
    uint16_t dev_id;       /*!< @if Eng device ID.
                                @else   设备ID. */
    bs_addr_t addr;       /*!< @if Eng address.
                                @else   地址. */
} bs_ble_paired_dev_info;


/**
 * @if Eng
 * @brief Struct of BLE link update event parameters.
 * @else
 * @brief ACL链路更新事件参数
 * @endif
 */
typedef struct {
    uint16_t interval;          /*!< @if Eng interval
                                     @else   链路调度间隔，单位slot @endif */
    uint16_t latency;           /*!< @if Eng latency
                                     @else   延迟周期，单位slot @endif */
    uint16_t timeout;           /*!< @if Eng timeout
                                     @else   超时时间，单位10ms @endif */
} bs_ble_conn_param_update_result_t;

/**
 * @if Eng
 * @brief Struct of setting BLE frequency scanning switch。
 * @else
 * @brief BLE扫频开关设置
 * @endif
 */
typedef struct {
    uint8_t switch_value; /*!< @if Eng switch for frequency scanning function,
                            see @ref bs_ble_gap_set_channel_scan_switch_value_t
                        @else   扫频功能开关  参考@ref bs_ble_gap_set_channel_scan_switch_value_t @endif */
    uint32_t period;      /*!< @if Eng Sweep period, in slots (0.625 ms)
                             @else   扫频周期，单位 slot(0.625ms) @endif */
} bs_ble_set_channel_scan_switch_t;

/**
 * @if Eng
 * @brief Use this function to enable ble.
 * @par Use this function to enable ble.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  使能ble。
 * @par    使能ble。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_enable(void);

/**
 * @if Eng
 * @brief Use this function to disable ble.
 * @par Use this function to disable ble.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  去使能ble。
 * @par    去使能ble。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_disable(void);

/**
 * @if Eng
 * @brief Use this function to discover peripheral device for central device based on ble.
 * @par Use this function for central device to start scan procedure.
 * @param [in] type  type of enter discovery.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  基于ble启动发现流程。
 * @par    中央设备使用该接口扫描外围设备。
 * @param [in] type  进入发现态的模式。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_start_discovery(uint8_t type);

/**
 * @if Eng
 * @brief Use this function to stop discover peripheral device for central device based on ble.
 * @par Use this function for central device to stop scan procedure.
 * @param [in] type  type of exit discovery.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  基于ble停止发现流程。
 * @par    中央设备使用该接口取消扫描外围设备。
 * @param [in] type  退出发现态的模式。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_stop_discovery(uint8_t type);

/**
 * @if Eng
 * @brief Use this function to enter pair procedure device based on ble.
 * @par Use this function to connect and pair device for central device.
 * @param [in] dev_id  remote device id.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  基于ble进入配对设备流程。
 * @par    设备进行连接并配对对端设备。
 * @param [in] dev_id  远端设备id.
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_start_pair(const bs_addr_t *addr);

/**
 * @if Eng
 * @brief Use this function to quit pair procedure device based on ble.
 * @par Use this function to cancel connect device for central device.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  基于ble退出配对设备流程。
 * @par    中央设备取消连接对端设备。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_stop_pair(void);

/**
 * @if Eng
 * @brief Use this function to reconnect device by ble.
 * @par Use this function to reconnect device by ble.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  设备回连
 * @par    设备回连
 * @retval 执行结果错误码。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @li
 * @endif
 */
errcode_t bs_ble_start_auto_conn(void);

/**
 * @if Eng
 * @brief Use this function to stop reconnect device by ble.
 * @par Use this function to stop reconnect device by ble.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  取消设备回连
 * @par    取消设备回连
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_stop_auto_conn(void);

/**
 * @if Eng
 * @brief Use this function to disconnect remote device.
 * @par Use this function to disconnect remote device.
 * @param  [in]  dev_id device ID.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  与对端设备断连。
 * @par    与对端设备断连。
 * @param  [in]  dev_id 设备ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_disconnect(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to disconnect all remote devices.
 * @par Use this function to disconnect all remote devices.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  与对端所有设备断连。
 * @par    与对端所有设备断连。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_disconnect_all(void);

/**
 * @if Eng
 * @brief Use this function to remove pair.
 * @par Use this function to remove pair.
 * @param  [in]  dev_id device ID.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  移除配对。
 * @par    移除配对。
 * @param  [in]  dev_id 设备ID。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_remove_pair(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to remove all paired devices.
 * @par Use this function to remove  all paired devices.
 * @param  [out] removed_num Number of devices removed.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  移除所有配对设备。
 * @par    移除所有配对设备。
 * @param  [out] removed_num 被移除设备的数量。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_remove_all_pair(uint16_t *removed_num);

/**
 * @if Eng
 * @brief  Callback invoked when stack enabled / disabled.
 * @par    When registered, this callback notifies the upper layer
 *         when ble stack is enabled or disabled.
 * @param  [in]  state ble stack state.
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  协议栈使能/去使能的回调函数。
 * @par    注册该回调函数之后，stack使能或者去使能后该回调函数会上报上层。
 * @param  [in]  state ble协议栈状态。
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_stack_state_cbk)(uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when ble connected / disconnected.
 * @par    When registered, this callback notifies the upper layer
 *         when ble is connected or disconnected.
 * @param  [in]  bs_ble_connection_result_t         ble connection result. {@ref bs_ble_connection_result_t}
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  ble连接/断连的回调函数。
 * @par    注册该回调函数之后，ble连接或者断连后该回调函数会上报上层。
 * @param  [in]  bs_ble_connection_result_t         ble连接状态结果. {@ref bs_ble_connection_result_t}
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_connect_state_cbk)(bs_ble_connection_result_t *result);

/**
 * @if Eng
 * @brief  Callback invoked when ble connected / disconnected.
 * @par    When registered, this callback notifies the upper layer
 *         when ble is connected or disconnected.
 * @param  [in]  dev_id device id.
 * @param  [in]  state  ble pair state.
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  ble连接/断连的回调函数。
 * @par    注册该回调函数之后，ble连接或者断连后该回调函数会上报上层。
 * @param  [in]  dev_id 设备id.
 * @param  [in]  state  ble配对状态.
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_pair_complete_cbk)(uint16_t dev_id, const bs_addr_t *addr,
    const bs_ble_pair_info_report_t *pair_info);

/**
 * @if Eng
 * @brief  Callback invoked when ble discover adv data.
 * @par    When registered, this callback notifies the upper layer
 *         when remote ble adv data is discovered.
 * @param  [in]  discovery_result  discovery result.
 * @param  [in]  data              adv data.
 * @param  [in]  len               length of adv data.
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  ble发现广播数据的回调函数。
 * @par    注册该回调函数之后，ble发现到远端设备的广播数据后该回调函数会上报上层。
 * @param  [in]  discovery_result  发现结果.
 * @param  [in]  data              广播数据.
 * @param  [in]  len               广播数据长度.
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_discover_result_cbk)(const bs_ble_discovery_result_t *discovery_result,
    const uint8_t *data, uint16_t len);

/**
 * @if Eng
 * @brief  Callback invoked when ble remote device pair is removed.
 * @par    When registered, this callback notifies the upper layer
 *         when remote ble device pair is removed.
 * @param  [in]  addr   ble address.
 * @param  [in]  status remove result status.
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  ble移除配对的回调函数。
 * @par    注册该回调函数之后，ble移除与远端设备的配对信息后该回调函数会上报上层。
 * @param  [in]  addr   ble地址.
 * @param  [in]  status 移除结果状态.
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_pair_remove_cbk)(const bs_addr_t *addr, errcode_t status);

/**
 * @if Eng
 * @brief  Callback invoked when read remote device RSSI.
 * @par    When registered, this callback notifies the upper layer
 *         when remote device RSSI is read.
 * @param  [in]  dev_id device ID.
 * @param  [in]  rssi RSSI value.
 * @param  [in]  status RSSI status.
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  读取RSSI的回调函数。
 * @par    注册该回调函数之后，读取到RSSI信息后该回调函数会上报上层。
 * @param  [in]  dev_id   设备ID.
 * @param  [in]  rssi 读取到的RSSI值.
 * @param  [in]  status 读取到的RSSI状态.
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_read_rssi_cbk)(uint16_t dev_id, int8_t rssi, uint8_t status);

/**
 * @if Eng
 * @brief  Callback invoked when ble link switch complete.
 * @par    When registered, this callback notifies the upper layer
 *         when ble link switch complete.
 * @param  [in]  state  ble link switch status. {@ref bs_ble_acl_switch_state_t}
 * @par Dependency:
 * @see bs_ble_callbacks_t
 * @else
 * @brief  ble链路切换状态回调函数。
 * @par    注册该回调函数之后，ble链路切换完成后该回调函数会上报上层。
 * @param  [in]  state  ble链路切换状态。{@ref bs_ble_acl_switch_state_t}
 * @par 依赖:
 * @see bs_ble_callbacks_t
 * @endif
 */
typedef void (*bs_ble_acl_switch_state_cbk)(uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when start or stop advertising
 * @par    When registered, this callback notifies the upper layer
 *         when start or stop advertising.
 * @param  [in]  adv_id advertising ID.
 * @param  [in]  status advertising status {@ref bs_ble_adv_status_t}.
 * @par Dependency:
 * @see
 * @else
 * @brief  广播状态回调函数。
 * @par    注册该回调函数之后，启动或停止广播的状态会通过该回调函数上报上层。
 * @param  [in]  adv_id 广播ID.
 * @param  [in]  status 广播状态 {@ref bs_ble_adv_status_t}.
 * @par 依赖:
 * @see
 * @endif
 */
typedef void (*bs_ble_advertising_state_cbk)(uint8_t adv_id, uint8_t status);

/**
 * @if Eng
 * @brief  Callback invoked when update connect parameters
 * @par    When registered, this callback notifies the upper layer
 *         when update connect parameters.
 * @param  [in]  dev_id device ID.
 * @param  [in]  status status {@ref errcode_t}.
 * @param  [in]  param  connect parameters {@ref bs_ble_conn_param_update_result_t}.
 * @par Dependency:
 * @see
 * @else
 * @brief  连接参数更新回调函数。
 * @par    注册该回调函数之后，连接参数更新会通过该回调函数上报上层。
 * @param  [in]  dev_id 设备ID.
 * @param  [in]  status 执行结果错误码. {@ref errcode_t}.
 * @param  [in]  param  连接参数 {@ref bs_ble_conn_param_update_result_t}.
 * @par 依赖:
 * @see
 * @endif
 */
typedef void (*bs_ble_connect_param_update_cbk)(uint16_t dev_id, errcode_t status,
    bs_ble_conn_param_update_result_t *param);

/**
 * @if Eng
 * @brief  Callback invoked when set data length
 * @par    When registered, this callback notifies the upper layer
 *         when set data length.
 * @param  [in]  dev_id device ID.
 * @param  [in]  status status {@ref errcode_t}.
 * @par Dependency:
 * @see
 * @else
 * @brief  设置数据长度回调函数。
 * @par    注册该回调函数之后，设置数据长度的结果会通过该回调函数上报上层。
 * @param  [in]  dev_id 设备ID.
 * @param  [in]  status 执行结果错误码 {@ref errcode_t}.
 * @par 依赖:
 * @see
 * @endif
 */
typedef void (*bs_ble_set_data_len_cbk)(uint16_t dev_id, errcode_t status);

/**
 * @if Eng
 * @brief Struct of BLE service callback function.
 * @else
 * @brief BLE service回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_ble_stack_state_cbk          stack_state_cbk;        /*!< @if Eng ble stack state callback
                                                                 @else   ble协议栈状态回调函数 @endif */
    bs_ble_connect_state_cbk        connect_state_cbk;      /*!< @if Eng ble connect state callback
                                                                 @else   ble连接状态回调函数 @endif */
    bs_ble_pair_complete_cbk        pair_complete_cbk;        /*!< @if Eng ble pair result callback
                                                                 @else   ble配对结果回调函数 @endif */
    bs_ble_discover_result_cbk      discover_result_cbk;    /*!< @if Eng discover result callback
                                                                 @else   发现结果上报回调函数 @endif */
    bs_ble_pair_remove_cbk          pair_remove_cbk;        /*!< @if Eng pair remove result callback
                                                                 @else   移除配对信息结果回调函数 @endif */
    bs_ble_read_rssi_cbk            read_rssi_cbk;          /*!< @if Eng read rssi result callback
                                                                 @else   读取到的RSSI结果回调函数 @endif */
    bs_ble_advertising_state_cbk    adv_state_cbk;          /*!< @if Eng start or stop advertising state callback
                                                                 @else   启动或停止广播状态回调函数 @endif */
    bs_ble_connect_param_update_cbk conn_param_update_cbk;  /*!< @if Eng update connect parameters callback
                                                                 @else   连接参数更新结果回调函数 @endif */
    bs_ble_set_data_len_cbk         set_data_length_cbk;    /*!< @if Eng set data length callback
                                                                 @else   设置数据长度结果回调函数 @endif */
} bs_ble_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register callbacks for ble service.
 * @par Use this function to register callbacks for ble service.
 * @param [in] app_id application id
 * @param [in] cbks callback functions {@ref bs_ble_callbacks_t}
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  ble service回调函数。
 * @par    ble service回调函数。
 * @param [in] app_id 上层应用ID
 * @param [in] cbks   回调函数 {@ref bs_ble_callbacks_t}
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_register_callbacks(uint8_t app_id, const bs_ble_callbacks_t *cbks);

/**
 * @if Eng
 * @brief Use this function to unregister callbacks for ble service.
 * @par Use this function to unregister callbacks for ble service.
 * @param [in] app_id application id
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  去注册ble service回调函数。
 * @par    去注册ble service回调函数。
 * @param [in] app_id 上层应用ID
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_unregister_callbacks(uint8_t app_id);

/**
 * @if Eng
 * @brief Use this function to register callbacks for ble acl switch.
 * @par Use this function to register callbacks for ble acl switch.
 * @param [in] app_id application id
 * @param [in] cbks callback functions {@ref bs_ble_acl_switch_state_cbk}
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:This interface can only be called after the BLE acl switch is enabled.
 * @li
 * @else
 * @brief  ble 主副切回调函数。
 * @par    ble 主副切回调函数。
 * @param [in] app_id 上层应用ID
 * @param [in] cbks   回调函数 {@ref bs_ble_acl_switch_state_cbk}
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖: 打开ble链路切换开关后调用
 * @li
 * @endif
 */
errcode_t bs_ble_acl_switch_register_callback(uint8_t app_id, const bs_ble_acl_switch_state_cbk cbks);

/**
 * @if Eng
 * @brief Use this function to set connection parameters for the specified device ID by
 *   the preset configuration information corresponding to the specified mode ID.
 * @par Use preset parameters to configure connection parameters.
 * @param [in] dev_id device ID
 * @param [in] mode_id mode ID
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:Before using this function, ensure that the preset connection parameters have been added.
 * @li
 * @else
 * @brief  使用指定的模式ID对应的配置对指定的设备ID的连接进行连接参数设置。
 * @par    BLE 使用预置参数对连接参数进行设置。
 * @param [in] dev_id 设备ID
 * @param [in] mode_id 模式ID
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖: 使用该函数前需已配置对应的预置连接参数。
 * @li
 * @endif
 */
errcode_t bs_ble_set_connect_param(uint16_t dev_id, uint16_t mode_id);

/**
 * @if Eng
 * @brief Use this function to set PHY parameters for the specified device ID by
 *   the preset configuration information corresponding to the specified mode ID.
 * @par Use preset parameters to configure PHY parameters.
 * @param [in] dev_id device ID
 * @param [in] mode_id mode ID
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:Before using this function, ensure that the preset PHY parameters have been added.
 * @li
 * @else
 * @brief  使用指定的模式ID对应的配置对指定的设备ID的PHY参数进行设置。
 * @par    BLE 使用预置参数对PHY参数进行设置。
 * @param [in] dev_id 设备ID
 * @param [in] mode_id 模式ID
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖: 使用该函数前需已配置对应的预置PHY参数。
 * @li
 * @endif
 */
errcode_t bs_ble_set_phy_param(uint16_t dev_id, uint16_t mode_id);

/**
 * @if Eng
 * @brief Use this funtion to set the BLE packet transmission parameters.
 * @par Use this funtion to set the BLE packet transmission parameters.
 * @param [in] param parameter
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  设置BLE发包参数。
 * @par    设置BLE发包参数。
 * @param [in] param 参数 {@ref bs_ble_set_data_length_t}
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_set_data_length(bs_ble_set_data_length_t *param);

/**
 * @if Eng
 * @brief Use this function to read Received Signal Strength Indicator.
 * @par Use this function to read Received Signal Strength Indicator.
 * @param [in] dev_id device ID
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:This interface can only be called after the callback function for reading RSSI has been registered.
 * @li
 * @else
 * @brief  读取对端设备的RSSI信息。
 * @par    读取对端设备的RSSI信息。
 * @param [in] dev_id 设备ID
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖: 该接口需要先注册读取RSSI的回调函数后才能调用
 * @li
 * @endif
 */
errcode_t bs_ble_read_remote_device_rssi(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this funtion to get information about all devices.
 * @par Use this funtion to get information about all devices.
 * @param [out] paired_infos an array indicated the queried paired device information {@ref bs_ble_paired_dev_info}
 * @param [in]  paired_infos_len indicates the length of the output array
 * @param [out] queried_num indicates the number of queried paired devices information
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  查询所有配对设备信息。
 * @par    查询所有配对设备信息。
 * @param [out] paired_infos 一个用于保存所有配对设备信息的数组 {@ref bs_ble_paired_dev_info}
 * @param [in]  paired_infos_len 数组长度
 * @param [out] queried_num 查询到的配对设备信息的数量
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_get_paired_dev_infos(bs_ble_paired_dev_info *paired_infos, const uint16_t paired_infos_len,
    uint16_t *queried_num);

/**
 * @if Eng
 * @brief Callback invoked when device power on.
 * @par When registered, this callback notifies the upper layer when device power on.
 * @param  [in] status device power on status. {@ref bs_ble_power_on_status_t}
 * @retval #void no return value.
 * @par Dependency:
 * @see bs_ble_dev_manager_callbacks_t
 * @else
 * @brief  设备上电回调函数。
 * @par    注册该回调函数之后，设备上电后上报启动结果给上层。
 * @param  [in] status 设备上电状态 {@ref bs_ble_power_on_status_t}
 * @retval 无返回值。
 * @par 依赖:
 * @see bs_ble_dev_manager_callbacks_t
 * @endif
 */
typedef void (*bs_ble_power_on_cbk)(bs_ble_power_on_status_t status);


/**
 * @if Eng
 * @brief Struct of bs ble device manager callback function.
 * @else
 * @brief BS BLE设备管理回调函数定义。
 * @endif
 */
typedef struct {
    bs_ble_power_on_cbk power_on_cbk;                        /*!< @if Eng Device power on callback
                                                                   @else   设备上电回调函数 @endif */
} bs_ble_dev_manager_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register callbacks for ble device management.
 * @par Use this function to register callbacks for ble device management.
 * @param [in] app_id application id
 * @param [in] cbks callback functions {@ref bs_ble_dev_manager_callbacks_t}
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency: This function should be called before the service thread starts.
 * @li
 * @else
 * @brief  ble 设备管理回调函数。
 * @par    ble 设备管理回调函数。
 * @param [in] app_id 上层应用ID
 * @param [in] cbks   回调函数 {@ref bs_ble_dev_manager_callbacks_t}
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖: 该函数应在服务线程启动前使用。
 * @li
 * @endif
 */
errcode_t bs_ble_dev_manager_register_callbacks(uint8_t app_id, const bs_ble_dev_manager_callbacks_t *cbks);

/**
 * @if Eng
 * @brief Use this function to unregister callbacks for ble device management.
 * @par Use this function to unregister callbacks for ble device management.
 * @param [in] app_id application id
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @li
 * @else
 * @brief  去注册ble设备管理回调函数。
 * @par    去注册ble设备管理回调函数。
 * @param [in] app_id 上层应用ID
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_ble_dev_manager_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif /* BS_BLE_SERVICE_API_H */
