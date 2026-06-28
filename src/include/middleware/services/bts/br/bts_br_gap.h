/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT GAP module.
 */

/**
 * @defgroup bluetooth_bts_gap BR GAP API
 * @ingroup  bluetooth
 * @{
 */


#ifndef BTS_BR_GAP_H
#define BTS_BR_GAP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Limited Discoverability.
 * @else
 * @brief 蓝牙受限可发现设备类型。
 * @endif
 */
#define SERVICE_LIMITED_DISCOVERABILITY 0x002000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Positioning.
 * @else
 * @brief 蓝牙定位设备。
 * @endif
 */
#define SERVICE_POSITIONING 0x010000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Networking.
 * @else
 * @brief 蓝牙网络设备。
 * @endif
 */
#define SERVICE_NETWORKING 0x020000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Render.
 * @else
 * @brief 蓝牙渲染类型设备。
 * @endif
 */
#define SERVICE_RENDER 0x040000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Capture.
 * @else
 * @brief 蓝牙捕获类型设备。
 * @endif
 */
#define SERVICE_CAPTURE 0x080000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that object transfer.
 * @else
 * @brief 蓝牙对象传输类型设备。
 * @endif
 */
#define SERVICE_OBJECT_TRANSFER 0x100000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Audio.
 * @else
 * @brief 蓝牙音频类型设备。
 * @endif
 */
#define SERVICE_AUDIO 0x200000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Telephony.
 * @else
 * @brief 蓝牙电话类型设备。
 * @endif
 */
#define SERVICE_TELEPHONY 0x400000

/**
 * @if Eng
 * @brief The Class of Bluetooth Device/Service that Information.
 * @else
 * @brief 蓝牙信息类型设备。
 * @endif
 */
#define SERVICE_INFORMATION 0x800000

/**
 * @if Eng
 * @brief  Enum of hfp sco connection status.
 * @else
 * @brief  定义蓝牙hfp的连接状态。
 * @endif
 */
typedef enum {
    HFP_SCO_STATE_CONNECTING = 0x01,    /*!< @if Eng status of sco is connecting. @else sco正在连接。 @endif */
    HFP_SCO_STATE_CONNECTED = 0x02,     /*!< @if Eng status of sco is connected. @else sco已连接。 @endif */
    HFP_SCO_STATE_DISCONNECTING = 0x03, /*!< @if Eng status of sco is disconnecting. @else sco正在断连。 @endif */
    HFP_SCO_STATE_DISCONNECTED = 0x04,   /*!< @if Eng status of sco is disconnected. @else sco已断连。 @endif */
    HFP_SCO_STATE_CONNECT_FAILURE = 0x05 /*!< @if Eng status of sco connect is failure. @else sco连接失败。 @endif */
} hfp_sco_connect_state_t;

/**
 * @if Eng
 * @brief  Enum of hfp volume type.
 * @else
 * @brief  定义蓝牙hfp的音量调整类型。
 * @endif
 */
typedef enum {
    HFP_VOLUME_MIC = 0x00, /*!< @if Eng volume type of microphone. @else 调整microphone增益。 @endif */
    HFP_VOLUME_SPEAKER     /*!< @if Eng volume type of speaker. @else 调整speaker增益。 @endif */
} hfp_volume_type_t;

/**
 * @if Eng
 * @brief  Enum of bt profile connection status.
 * @else
 * @brief  定义蓝牙协议的连接状态。
 * @endif
 */
typedef enum {
    PROFILE_STATE_CONNECTING = 0x01,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    PROFILE_STATE_CONNECTED = 0x02,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    PROFILE_STATE_DISCONNECTING = 0x03, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    PROFILE_STATE_DISCONNECTED = 0x04   /*!< @if Eng status of disconnected. @else 已断连。 @endif */
} profile_connect_state_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth ACL connection state.
 * @else
 * @brief 蓝牙协议栈状态。
 * @endif
 */
typedef enum {
    BT_STACK_STATE_TURNING_ON = 0x0, /*!< @if Eng Bluetooth stack state of turning on.
                                          @else   蓝牙协议栈正在初始化中。 @endif */
    BT_STACK_STATE_TURN_ON,          /*!< @if Eng Bluetooth stack state of turn on.
                                          @else   蓝牙协议栈正在运行中。 @endif */
    BT_STACK_STATE_TURNING_OFF,      /*!< @if Eng Bluetooth stack state of turning off.
                                          @else   蓝牙协议栈正在去初始化中。 @endif */
    BT_STACK_STATE_TURN_OFF          /*!< @if Eng Bluetooth stack state of turn off.
                                          @else   蓝牙协议栈已关闭。 @endif */
} bt_stack_state_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth scan mode.
 * @else
 * @brief 蓝牙扫描模式。
 * @endif
 */
typedef enum {
    GAP_SCAN_MODE_NONE = 0x00,                      /*!< @if Eng Scan type of none.
                                                         @else 未知扫描类型。 @endif */
    GAP_SCAN_MODE_CONNECTABLE,                      /*!< @if Eng Scan type of connection.
                                                         @else 可连接扫描类型。 @endif */
    GAP_SCAN_MODE_GENERAL_DISCOVERABLE,             /*!< @if Eng Scan type of general discovery.
                                                         @else 通用广播扫描类型。 @endif */
    GAP_SCAN_MODE_LIMITED_DISCOVERABLE,             /*!< @if Eng Scan type of limited discovery.
                                                         @else 受限广播扫描类型。 @endif */
    GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, /*!< @if Eng Scan type of connection and general discovery.
                                                         @else 可连接的通用广播扫描类型。 @endif */
    GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE  /*!< @if Eng Scan type of connection and limited discovery.
                                                         @else 可连接的受限广播扫描类型。 @endif */
} gap_scan_mode_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth discovery state.
 * @else
 * @brief 蓝牙扫描状态。
 * @endif
 */
typedef enum {
    GAP_DISCOVERY_STARTED = 0x01, /*!< @if Eng Bluetooth discovery state of started.
                                       @else 已开始扫描。 @endif */
    GAP_DISCOVERYING = 0x02,      /*!< @if Eng Bluetooth discovery state of discovering.
                                       @else 正在扫描。 @endif */
    GAP_DISCOVERY_STOPED = 0x03,  /*!< @if Eng Bluetooth discovery state of stoped.
                                       @else 扫描已停止。 @endif */
} gap_discovery_state_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth ACL connection state.
 * @else
 * @brief 蓝牙ACL连接状态。
 * @endif
 */
typedef enum {
    GAP_ACL_STATE_CONNECTED,      /*!< @if Eng GAP ACL state of connected.
                                       @else GAP ACL 已连接。 @endif */
    GAP_ACL_STATE_DISCONNECTED,   /*!< @if Eng GAP ACL state of disconnected.
                                       @else GAP ACL 已断接。 @endif */
    GAP_ACL_STATE_LE_CONNECTED,   /*!< @if Eng BLE GAP ACL state of connected.
                                       @else BLE GAP ACL 已连接。 @endif */
    GAP_ACL_STATE_LE_DISCONNECTED /*!< @if Eng BLE GAP ACL state of disconnected.
                                       @else BLE GAP ACL 已断接。 @endif */
} gap_acl_state_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth pairing state.
 * @else
 * @brief 蓝牙配对状态。
 * @endif
 */
typedef enum {
    GAP_PAIR_NONE = 0x01,    /*!< @if Eng Pair state of none.
                                  @else 未配对状态。 @endif */
    GAP_PAIR_PAIRING = 0x02, /*!< @if Eng Pair state of pairing.
                                  @else 正在配对。 @endif */
    GAP_PAIR_PAIRED = 0x03   /*!< @if Eng Pair state of paired.
                                  @else 已完成配对。 @endif */
} gap_pair_state_t;

/**
 * @if Eng
 * @brief Length of bluetooth device name.
 * @else
 * @brief 蓝牙设备名称长度。
 * @endif
 */
#define BD_NAME_LEN 32

/**
 * @if Eng
 * @brief Struct of device's pairing information.
 * @else
 * @brief 配对设备信息数据。
 * @endif
 */
typedef struct {
    bd_addr_t addr;                         /*!< @if Eng Bluetooth device address.
                                                 @else   蓝牙地址。 @endif */
    unsigned char device_name[BD_NAME_LEN]; /*!< @if Eng Name of the device record, must be in UTF-8.
                                                 @else   蓝牙设备名称。 @endif */
} gap_paired_device_info_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth pairing type.
 * @else
 * @brief 蓝牙配对类型。
 * @endif
 */
typedef enum {
    GAP_PAIR_CONFIRM_TYPE_PIN_CODE = 0x01,        /*!< @if Eng Pair type of pin code.
                                                       @else 通过pin code 配对。 @endif */
    GAP_PAIR_CONFIRM_TYPE_PASSKEY_DISPLAY = 0x02, /*!< @if Eng Pair type of dispaly passkey.
                                                       @else 展示PASSKEY。 @endif */
    GAP_PAIR_CONFIRM_TYPE_PASSKEY_INPUT = 0x03,   /*!< @if Eng Pair type of input passkey.
                                                       @else 输入PASSKEY。 @endif */
    GAP_PAIR_CONFIRM_TYPE_NUMERIC = 0x04,         /*!< @if Eng Pair type of numeric.
                                                       @else 配对数值。 @endif */
    GAP_PAIR_CONFIRM_TYPE_CONSENT = 0x05,         /*!< @if Eng Pair type of consent.
                                                       @else 请求用户同意。 @endif */
} gap_pair_confirm_type_t;

/**
 * @if Eng
 * @brief Enum of Bluetooth link role.
 * @else
 * @brief 蓝牙链路角色。
 * @endif
 */
typedef enum {
    GAP_LINK_ROLE_MASTER = 0x00, /*!< @if Eng Link role of master.
                                      @else 主端链路角色。 @endif */
    GAP_LINK_ROLE_SLAVE = 0x01,  /*!< @if Eng Link role of master.
                                      @else 从端链路角色。 @endif */
} gap_link_role_t;

/**
 * @if Eng
 * @brief Enum of rejecting connect reason.
 * @else
 * @brief 拒绝连接原因。
 * @endif
 */
typedef enum {
    GAP_REJECT_CONNECT_REASON_LIMITED_RESOURCES =  0x00, /*!< @if Eng Rejected due to limited resources.
                                                              @else 资源有限。 @endif */
    GAP_REJECT_CONNECT_REASON_SECURITY_REASONS,          /*!< @if Eng Security requirements not being fulfilled.
                                                              @else 未满足安全要求。 @endif */
    GAP_REJECT_CONNECT_REASON_UNACCEPTABLE_BD_ADDR,      /*!< @if Eng This device does not accept the BD_ADDR.
                                                                      This may be because the device will only
                                                                      accept connections from specific BD_ADDRs.
                                                              @else 不接受该地址，只接受特定地址连接。 @endif */
} gap_reject_connect_reason_t;

/**
 * @if Eng
 * @brief Struct of Bluetooth discovery device.
 * @else
 * @brief 蓝牙发现设备结构体。
 * @endif
 */
typedef struct {
    bd_addr_t addr;                       /*!< @if Eng address of device.
                                               @else 设备地址。 @endif */
    uint8_t name[BT_DEVICE_NAME_MAX_LEN]; /*!< @if Eng name of device, include the end '\0'.
                                               @else 设备名称, 包含字符串结束符。 @endif */
    uint32_t device_class;                /*!< @if Eng class of device.
                                               @else 设备类型。 @endif */
    int8_t rssi;                          /*!< @if Eng received signal strength indicator.
                                               @else 信号强度 @endif */
} gap_discovery_device_t;

/**
 * @if Eng
 * @brief Struct of local device information.
 * @else
 * @brief 本地设备信息结构体。
 * @endif
 */
typedef struct {
    uint8_t scan_mode;          /*!< @if Eng if scan mode, refer to gap_scan_mode_t.
                                     @else 扫描模式，@ref gap_scan_mode_t。  @endif */
    bool is_discovering;        /*!< @if Eng if is discovering.
                                     @else 是否设备发现中。 @endif */
    uint8_t paired_devices_num; /*!< @if Eng number of paired devices.
                                     @else 已配对设备数量。 @endif */
} gap_local_device_info_t;

/**
 * @if Eng
 * @brief Struct of remote device information.
 * @else
 * @brief 远端设备信息结构体。
 * @endif
 */
typedef struct {
    bd_addr_t addr;                       /*!< @if Eng address of device.
                                               @else 设备地址。 @endif */
    uint8_t name[BT_DEVICE_NAME_MAX_LEN]; /*!< @if Eng name of device, include the end '\0'.
                                               @else 设备名称, 包含字符串结束符。 @endif */
    uint32_t device_class;                /*!< @if Eng class of device.
                                               @else 设备类型。 @endif */
    bool is_connected;                    /*!< @if Eng if is connected.
                                               @else 是否已连接。 @endif */
    bool is_paired;                       /*!< @if Eng if is paired.
                                               @else 是否已配对。 @endif */
} gap_remote_device_info_t;

/**
 * @if Eng
 * @brief Use this funtion to enable BT stack.
 * @par  Use this funtion to enable BT stack.
 * @attention NULL.
 * @param  [in] transport uint8_t, Transport type bitmap, use BT_TRANSPORT_TYPE_BR_EDR | BT_TRANSPORT_TYPE_LE to
                                    enable both BR_EDR and LE @ref bt_transport_type_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  使能BT协议栈。
 * @par    使能BT协议栈。
 * @attention 无
 * @param  [in] transport uint8_t, 蓝牙传输类型位图，使用BT_TRANSPORT_TYPE_BR_EDR | BT_TRANSPORT_TYPE_LE使能
                                    BR_EDR和LE传输 @ref bt_transport_type_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t enable_bt_stack(uint8_t transport);

/**
 * @if Eng
 * @brief Use this funtion to disable BT stack.
 * @par  Use this funtion to disable BT stack.
 * @attention NULL.
 * @param  [in] transport uint8_t, Transport type bitmap, use BT_TRANSPORT_TYPE_BR_EDR | BT_TRANSPORT_TYPE_LE to
                                    disable both BR_EDR and LE @ref bt_transport_type_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  去使能BT协议栈。
 * @par    去使能BT协议栈。
 * @attention 无
 * @param  [in] transport uint8_t, 蓝牙传输类型位图，使用BT_TRANSPORT_TYPE_BR_EDR | BT_TRANSPORT_TYPE_LE去使能
                                    BR_EDR和LE传输 @ref bt_transport_type_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t disable_bt_stack(uint8_t transport);

/**
 * @if Eng
 * @brief Use this funtion to set local host bluetooth address.
 * @par   Use this funtion to set local host bluetooth address.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, local host bluetooth address.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  设置本地设备地址。
 * @par    设置本地设备地址。
 * @attention 无
 * @param  [in] addr const bd_addr_t *, 蓝牙设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t bluetooth_set_local_addr(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get local host bluetooth address.
 * @par   Use this funtion to get local host bluetooth address.
 * @attention NULL
 * @param  [out] addr bd_addr_t *, local host bluetooth address.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取本地设备地址。
 * @par    获取本地设备地址。
 * @attention 无
 * @param  [out] addr bd_addr_t *, 蓝牙设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t bluetooth_get_local_addr(bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get local host bluetooth name.
 * @par   Use this funtion to get local host bluetooth name.
 * @attention NULL
 * @param   [out] local_name uint8_t *, the name of local host bluetooth device.
 * @param   [inout] length uint8_t *, as input parameter, the buffer size of user,
                                       as output parameter, the length of local device name.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取本地设备名称。
 * @par    获取本地设备名称。
 * @attention 无
 * @param   [out] local_name uint8_t *, 蓝牙设备名称。
 * @param   [inout] length uint8_t *, 作为入参时为用户分配的内存大小，作为出参时为本地设备名称长度。。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t bluetooth_get_local_name(uint8_t *local_name, uint8_t *length);

/**
 * @if Eng
 * @brief Use this funtion to set local host bluetooth name.
 * @par   Use this funtion to set local host bluetooth name.
 * @attention NULL
 * @param  [in] local_name const uint8_t *, the name of local host bluetooth device.
 * @param  [in] length const uint8_t, the length of device name, including the endding 0.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  设置本地设备名称。
 * @par    设置本地设备名称。
 * @attention 无
 * @param  [in] 蓝牙设备名称。
 * @param  [in] 名称长度，包括结束符\0。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t bluetooth_set_local_name(const uint8_t *local_name, const uint8_t length);

/**
 * @if Eng
 * @brief Use this funtion to set device scan mode.
 * @par   Use this funtion to set device scan mode.
 * @attention NULL
 * @param  [in] mode uint8_t, scan mode,refer to gap_scan_mode_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  设置蓝牙广播模式。
 * @par    设置蓝牙广播模式。
 * @attention 无
 * @param   [in]  广播模式，参考枚举类型 @ref gap_scan_mode_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_set_bt_scan_mode(uint8_t mode);

/**
 * @if Eng
 * @brief Use this funtion to get the paired devices information.
 * @par   Use this funtion to get the paired devices information.
 * @attention NULL.
 * @param  [out] dev_infos gap_remote_device_info_t *, the paired devices information.
 * @param   [inout] number as input parameter, the buffer size of user,
                           as output parameter, the number of paired devices.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取配对设备列表。
 * @par    获取配对设备列表。
 * @attention 无。
 * @param  [out] dev_infos 配对设备信息列表。
 * @param   [inout] number 作为入参时为用户分配的配对信息列表数量，作为出参时为实际获取到的配对设备数量。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_get_paired_devices_list(gap_remote_device_info_t *dev_infos, uint8_t *number);

/**
 * @if Eng
 * @brief Use this funtion to pair a remote device.
 * @par   Use this funtion to pair a remote device.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  配对设备。
 * @par    配对设备。
 * @attention 无
 * @param  [in] addr 设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_pair_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to reply the pair request when pair_request_callback is invoked.
 * @par   Use this funtion to reply the pair request when pair_request_callback is invoked.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @param  [in] accept bool, wheather to accept the pair request.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  回复配对请求。
 * @par    回复配对请求。
 * @attention 无
 * @param  [in] addr 设备地址。
 * @param  [in] accept 是否接受配对。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_reply_pair_request(const bd_addr_t *addr, bool accept);

/**
 * @if Eng
 * @brief Use this funtion to remove a remote device from the paired list.
 * @par   Use this funtion to remove a remote device from the paired list.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  删除配对设备。
 * @par    删除配对设备。
 * @attention 无
 * @param  [in] 待删除的设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_remove_pair(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to set link key to flash.
 * @par   Use this funtion to set link key to flash.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @param  [in] link_key const bt_link_key_t *, refer bt_link_key_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  设置链路密钥到flash中。
 * @par    设置链路密钥到flash中。
 * @attention 无
 * @param  [in] addr 设备地址。
 * @param  [in] link_key 应用层保存的设备密钥。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_set_link_key(const bd_addr_t *addr, const bt_link_key_t *link_key);

/**
 * @if Eng
 * @brief Use this funtion to to authenticate and encrypt the ACL link.
 * @par   Use this funtion to encrypt link.
 * @attention The ACL link shall be created before this function is called.
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  加密链路。
 * @par    加密链路。
 * @attention 链路建立之后再调用。
 * @param  [in] addr 设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_encrypt_link(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to to perform the role switch.
 * @par   Use this funtion to to perform the role switch.
 * @attention The ACL link shall be created before this function is called.
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @param  [in] link_role uint8_t, refer gap_link_role_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  设置链路角色。
 * @par    设置链路角色。
 * @attention 链路建立之后再调用。
 * @param  [in] addr 设备地址。
 * @param  [in] link_role 链路角色，参考枚举类型 @ref gap_link_role_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_set_link_role(const bd_addr_t *addr, uint8_t link_role);

/**
 * @if Eng
 * @brief Use this funtion to disconnect from a remote device.
 * @par   Use this funtion to disconnect all profiles from a remote device.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  断开设备连接。
 * @par    断开设备连接，包括所有profile连接。
 * @attention 无
 * @param  [in] 待检查的设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_disconnect_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to connect to a remote device.
 * @par   Use this funtion to establish an ACL connection with a remote device.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  与设备建立连接。
 * @par    与设备建立ACL连接。
 * @attention 无
 * @param  [in] 待连接的设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_connect_remote_device(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to reply connect request when connect_request_callback is invoked.
 * @par   Use this funtion to reply connect request when connect_request_callback is invoked.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @param  [in] accept bool, wheather to accept the connect request.
 * @param  [in] reason uint8_t, reason for reject when accept is set to false. refer gap_reject_connect_reason_t.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  回复连接请求。
 * @par    回复连接请求。
 * @attention 无
 * @param  [in] addr 设备地址。
 * @param  [in] accept 是否接受连接。
 * @param  [in] reason 拒绝连接原因，参考枚举类型 @ref gap_reject_connect_reason_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_reply_acl_connect_request(const bd_addr_t *addr, bool accept, uint8_t reason);

/**
 * @if Eng
 * @brief Use this funtion to start BT discovery.
 * @par  Use this funtion to start BT discovery.
 * @attention NULL
 * @param  NULL
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  启动扫描。
 * @par    启动扫描。
 * @attention 无
 * @param  无
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_start_discovery(void);

/**
 * @if Eng
 * @brief Use this funtion to stop BT discovery.
 * @par  Use this funtion to stop BT discovery.
 * @attention NULL
 * @param  NULL
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  停止扫描。
 * @par    停止扫描。
 * @attention 无
 * @param  无
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_cancel_discovery(void);

/**
 * @if Eng
 * @brief Use this funtion to get RSSI of a remote device.
 * @par   Use this funtion to get RSSI of a remote device.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取远端设备RSSI信号强度。
 * @par    获取远端设备RSSI信号强度。
 * @attention 无
 * @param  [in] 待查询的设备地址，@ref bd_addr_t。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_read_remote_rssi_value(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to set the inquiry parameters.
 * @par  Use this funtion to set the inquiry parameters.
 * @attention NULL
 * @param  [in] device_class uint32_t, class of device to report, specified device is filtered if not 0.
 * @param  [in] max_num uint8_t, max number of response devices, range 0~20, 0 means unlimited.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置扫描参数。
 * @par    设置扫描参数。
 * @attention 无
 * @param  [in] device_class 设备类型,如果不为0会过滤指定设备类型。
 * @param  [in] max_num 最大上报数量,范围0~20，0表示不限制。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_br_set_inquiry_parameters(uint32_t device_class, uint8_t max_num);

/**
 * @if Eng
 * @brief Use this funtion to set the device class.
 * @par  Use this funtion to set the device class.
 * @attention NULL
 * @param  [in] device_class uint32_t, class of device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置设备类型。
 * @par    设置设备类型。
 * @attention 无
 * @param  [in] device_class 设备类型。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_br_set_local_device_class(uint32_t device_class);

/**
 * @if Eng
 * @brief  Use this funtion to set the inquiry scan interval and window.
 * @par    Use this funtion to set the inquiry scan interval and window.
 * @attention NULL
 * @param  [in] interval uint16_t, inquiry scan interval,
 *              unit is slot(0.625ms), range 0x0012 ~ 0x1000, only even values are valid.
 * @param  [in] window uint16_t, inquiry scan window, unit is slot(0.625ms), range 0x0011 ~ 0x1000.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置inquiry scan参数。
 * @par    设置inquiry scan参数。
 * @attention 无
 * @param  [in] interval inquiry scan interval，单位为slot（0.625ms），范围0x0012 ~ 0x1000，只有偶数为有效值。
 * @param  [in] window inquiry scan window，单位为slot（0.625ms），范围0x0011 ~ 0x1000。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_br_set_inquiry_scan_paramters(uint16_t interval, uint16_t window);

/**
 * @if Eng
 * @brief  Use this funtion to set the page scan interval and window.
 * @par    Use this funtion to set the page scan interval and window.
 * @attention NULL
 * @param  [in] interval uint16_t, page scan interval,
 *              unit is slot(0.625ms), range 0x0012 ~ 0x1000, only even values are valid.
 * @param  [in] window uint16_t, page scan window, unit is slot(0.625ms), range 0x0011 ~ 0x1000.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_def.h
 * @else
 * @brief  设置page scan参数。
 * @par    设置page scan参数。
 * @attention 无
 * @param  [in] interval page scan interval，单位为slot（0.625ms），范围0x0012 ~ 0x1000，只有偶数为有效值。
 * @param  [in] window page scan window，单位为slot（0.625ms），范围0x0011 ~ 0x1000。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_def.h
 * @endif
 */
errcode_t gap_br_set_page_scan_paramters(uint16_t interval, uint16_t window);

/**
 * @if Eng
 * @brief Use this funtion to request the remote name.
 * @par   Use this funtion to request the remote name.
 * @attention NULL
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  请求远端设备名称。
 * @par    请求远端设备名称。
 * @attention 无
 * @param  [in] addr 设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_request_remote_name(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to read the remote version infomation.
 * @par   Use this funtion to read the remote version infomation.
 * @attention The ACL link shall be created before this function is called.
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  读取远端版本信息。
 * @par    读取远端版本信息。
 * @attention 链路建立之后再调用。
 * @param  [in] addr 设备地址。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_read_remote_version_info(const bd_addr_t *addr);

/**
 * @if Eng
 * @brief Use this funtion to get the local device information.
 * @par   Use this funtion to get the local device information.
 * @attention NULL.
 * @param  [out] dev_info gap_local_device_info_t *, the local device information.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取本地设备信息。
 * @par    获取本地设备信息。
 * @attention 无。
 * @param  [out] dev_info 本地设备信息。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_get_local_device_info(gap_local_device_info_t *dev_info);

/**
 * @if Eng
 * @brief Use this funtion to get the remote device information.
 * @par   Use this funtion to get the remote device information.
 * @attention NULL.
 * @param  [in] addr const bd_addr_t *, address of the device.
 * @param  [out] dev_info gap_remote_device_info_t *, the remote device information.
 * @retval error code, ERRCODE_BT_SUCCESS execute successfully.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @else
 * @brief  获取远端设备信息。
 * @par    获取远端设备信息。
 * @attention 无。
 * @param  [in] addr 设备地址。
 * @param  [out] dev_info 远端设备信息。
 * @retval 执行结果错误码, ERRCODE_BT_SUCCESS 执行成功。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @endif
 */
errcode_t gap_br_get_remote_device_info(const bd_addr_t *addr, gap_remote_device_info_t *dev_info);

/**
 * @if Eng
 * @brief Callback function for bluetooth state change.
 * @par   When registered, this callback reports any change of GAP state to upper layer application.
 * @attention NULL
 * @param  [in] transport const uint8_t, transport type, refer to bt_transport_type_t.
 * @param  [in] state  const uint8_t, GAP state, refer to bt_stack_state_t.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
* @see  asi_gap_callbacks_t
 * @else
 * @brief  蓝牙开关回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将蓝牙开关状态传递给上层应用。
 * @attention 无
 * @param  [in] 数据传输类型， @ref bt_transport_type_t。
 * @param  [in] GAP状态，  @ref bt_stack_state_t。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  asi_gap_callbacks_t
 * @endif
 */
typedef void (*gap_state_changed_callback)(const uint8_t transport, const uint8_t state);

/**
 * @if Eng
 * @brief Callback function for bluetooth local device address change.
 * @par   When registered, this callback reports local device address change to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the local device.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙本地设备地址变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将本地设备地址变化传递给上层应用。
 * @attention 无
 * @param  [in] 本地设备地址。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_local_device_addr_changed_callback)(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Callback function for bluetooth local device name change.
 * @par   When registered, this callback reports local device name change to upper layer application.
 * @attention NULL
 * @param  [in] device_name const uint8_t *, local device name.
 * @param  [in] length uint8_t, the length of device name.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙本地设备名称变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将本地设备名称变化传递给上层应用。
 * @attention 无
 * @param  [in] 本地设备名称。
 * @param  [in] 名称长度。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_local_device_name_changed_callback)(const uint8_t *device_name, uint8_t length);

/**
 * @if Eng
 * @brief Callback function for bluetooth scan mode change.
 * @par   When registered, this callback reports scan mode change to upper layer application.
 * @attention NULL
 * @param  [in] mode int, scan mode. @ref gap_scan_mode_t
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙广播模式变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将广播模式变化传递给上层应用。
 * @attention 无
 * @param  [in]  广播模式。@ref gap_scan_mode_t
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_scan_mode_changed_callback)(uint8_t mode);

/**
 * @if Eng
 * @brief Callback function for bluetooth discovery state change.
 * @par   When registered, this callback reports any change of discovery state to upper layer application.
 * @attention NULL
 * @param  [in] status uint8_t, discovery state, refer to gap_discovery_state_t.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙发现状态变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将发现状态传递给上层应用。
 * @attention 无
 * @param  [in] 发现状态，@ref gap_discovery_state_t。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_discovery_state_changed_callback)(uint8_t status);

/**
 * @if Eng
 * @brief Callback function for bluetooth discovery result.
 * @par   When registered, this callback reports discovery result to upper layer application.
 * @attention NULL
 * @param  [in] device const gap_discovery_device_t *, the device info of the remote device.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙发现结果回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将发现结果传递给上层应用。
 * @attention 无
 * @param  [in] 所发现的远端设备信息。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_discovery_result_callback)(const gap_discovery_device_t *device);

/**
 * @if Eng
 * @brief Callback function for report connection request.
 * @par   When registered, this callback report connection request to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] device_class uint32_t, Class of Device.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  收到ACL连接请求回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数通知上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 设备类型。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_acl_connect_request_callback)(const bd_addr_t *bd_addr, uint32_t device_class);

/**
 * @if Eng
 * @brief Callback function for bluetooth ACL connecction state change.
 * @par   When registered, this callback reports any change of ACL connection state to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] state gap_acl_state_t, ACL connection state.
 * @param  [in] reason uint32_t, reason code for state change, refer to HCI error code.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙ACL连接状态变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将ACL连接状态传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] ACL连接状态。
 * @param  [in] 状态变化原因，参考标准HCI错误码。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_acl_state_changed_callback)(const bd_addr_t *bd_addr, gap_acl_state_t state, uint32_t reason);

/**
 * @if Eng
 * @brief Callback function for bluetooth pair request.
 * @par   When registered, this callback reports pair requests to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] bondable bool, local is bondable and remote annouced bondable.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙配对请求回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将配对请求传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] bondable bool, 本地和远端都支持绑定.
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_pair_request_callback)(const bd_addr_t *bd_addr, bool bondable);

/**
 * @if Eng
 * @brief Callback function for bluetooth pair confirmation.
 * @par   When registered, this callback reports pair confirmations to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] req_type uint8_t, the pair request type, refer to gap_pair_confirm_type_t.
 * @param  [in] number const uint8_t *, paired passkey, 6 byte length, each byte value equals ascii value of digit 0-9.
 * @param  [in] len const uint8_t，length of passkey, fixed value 6 if passkey exist.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  蓝牙配对确认请求回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将配对确认请求传递给上层应用。
 * @attention 无
 * @param  [in]  远端设备地址。
 * @param  [in]  配对类型，@ref gap_pair_confirm_type_t。
 * @param  [in]  配对密码，长度为6字节，每个字节的值为数字0-9的ascii值。
 * @param  [in]  配对密码长度，如果配对密码不为空固定值为6。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_pair_confirm_request_callback)(
    const bd_addr_t *bd_addr, uint8_t req_type, const uint8_t *number, const uint8_t len);

/**
 * @if Eng
 * @brief Callback function for authentication complete.
 * @par   When registered, this callback reports authentication complete to upper layer application.
 * @attention NULL
 * @param  [in]  bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in]  status errcode_t, error code.
 * @param  [in]  link_key const bt_link_key_t *, bt link key.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @else
 * @brief  认证完成的回调函数。
 * @par    认证完成的回调函数。
 * @attention  无
 * @param  [in]  bd_addr 远端设备地址。
 * @param  [in]  status 执行结果错误码。
 * @param  [in]  link_key 设备密钥。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_auth_complete_callback)(
    const bd_addr_t *bd_addr, errcode_t status, const bt_link_key_t *link_key);

/**
 * @if Eng
 * @brief Callback function for pairing remove.
 * @par When registered, this callback reports pairing remove to upper layer application.
 * @attention NULL
 * @param  [in]  bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in]  status errcode_t, error code.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @else
 * @brief  取消设备配对完成的回调函数。
 * @par    取消设备配对完成的回调函数。
 * @attention  无
 * @param  [in]  bd_addr 远端设备地址。
 * @param  [in]  status 执行结果错误码。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_pair_remove_callback)(const bd_addr_t *bd_addr, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for encrypt complete.
 * @par When registered, this callback reports encrypt complete to upper layer application.
* @attention NULL
 * @param  [in]  bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in]  status errcode_t, error code.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @else
 * @brief  加密完成回调函数。
 * @par    加密完成回调函数。
 * @attention  无
 * @param  [in]  bd_addr 远端设备地址。
 * @param  [in]  status 执行结果错误码。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_encrypt_complete_callback)(const bd_addr_t *bd_addr, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for uuid change of remote devices.
 * @par   When registered, this callback reports uuid change of remote devices to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the local device.
 * @param  [in] uuid int, uuid of remote device.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  远端设备UUID变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将远端设备UUID变化传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 远端设备UUID。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_remote_uuid_changed_callback)(const bd_addr_t *bd_addr, bt_uuid_t uuid);

/**
 * @if Eng
 * @brief Callback function for name change of remote devices.
 * @par   When registered, this callback reports name change of remote devices to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] device_name const uint8_t *, remote device name, include the end '\0'..
 * @param  [in] length uint8_t, the length of device name.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  远端设备名称变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将远端设备名称变化传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 远端设备名称, 包含字符串结束符。
 * @param  [in] 名称长度。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_remote_name_changed_callback)(
    const bd_addr_t *bd_addr, const uint8_t *device_name, uint8_t length);

/**
 * @if Eng
 * @brief Callback function for alias change of remote devices.
 * @par   When registered, this callback reports alias change of remote devices to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] alias const uint8_t *, remote device alias.
 * @param  [in] length uint8_t, the length of device alias.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  远端设备昵称变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将远端设备昵称变化传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 远端设备昵称。
 * @param  [in] 昵称长度。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_remote_alias_changed_callback)(const bd_addr_t *bd_addr, const uint8_t *alias, uint8_t length);

/**
 * @if Eng
 * @brief Callback function for Class of Device(CoD) change of remote devices.
 * @par   When registered, this callback reports CoD change of remote devices to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] device_class uint32_t, Device Class.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  远端设备类型变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将远端设备类型变化传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 设备类型。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_remote_cod_changed_callback)(const bd_addr_t *bd_addr, uint32_t device_class);

/**
 * @if Eng
 * @brief Callback function for link role changed.
 * @par   When registered, this callback report link role changed to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] link_role uint8_t, refer gap_link_role_t.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  链路角色变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数通知上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 链路角色，参考枚举类型 @ref gap_link_role_t。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_link_role_changed_callback)(const bd_addr_t *bd_addr, uint8_t link_role);

/**
 * @if Eng
 * @brief Callback function for RSSI change of remote devices.
 * @par   When registered, this callback reports RSSI change of remote device to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] rssi int8_t, rssi value.
 * @param  [in] status errcode_t, error code.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  远端设备信号强度变化回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数将远端设备信号强度变化传递给上层应用。
 * @attention 无
 * @param  [in] 远端设备地址。
 * @param  [in] 信号强度。
 * @param  [in] 执行结果错误码 @ref errcode_bt_t。
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_read_remote_rssi_event_callback)(const bd_addr_t *bd_addr, int8_t rssi, errcode_t status);

/**
 * @if Eng
 * @brief Callback function for read remote device version.
 * @par   When registered, this callback report read remote device version result to upper layer application.
 * @attention NULL
 * @param  [in] bd_addr const bd_addr_t *, the address of the remote device.
 * @param  [in] manu_id uint16_t, manufacturer id.
 * @param  [in] version uint8_t, version.
 * @param  [in] sub_version uint16_t, sub version.
 * @retval void no return value.
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  读取远端版本信息回调函数。
 * @par    注册该回调函数之后，bluetooth调用该回调函数通知上层应用。
 * @attention 无
 * @param  [in] bd_addr 设备地址.
 * @param  [in] manu_id 制造商id.
 * @param  [in] version 版本号.
 * @param  [in] sub_version 子版本号.
 * @retval void  无返回值。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
typedef void (*gap_br_read_remote_version_info_callback)(
    const bd_addr_t *bd_addr, uint16_t manu_id, uint8_t version, uint16_t sub_version);

/**
 * @if Eng
 * @brief Struct of GAP callback functions.
 * @else
 * @brief APP回调函数接口定义。
 * @endif
 */
typedef struct {
    gap_state_changed_callback state_changed_callback;
    gap_local_device_addr_changed_callback device_addr_changed_callback;
    gap_local_device_name_changed_callback device_name_changed_callback;
    gap_scan_mode_changed_callback scan_mode_changed_callback;
    gap_discovery_state_changed_callback discovery_state_changed_callback;
    gap_discovery_result_callback discovery_result_callback;
    gap_br_acl_connect_request_callback connect_request_callback;
    gap_acl_state_changed_callback acl_state_changed_callbak;
    gap_br_pair_request_callback pair_request_callback;
    gap_br_pair_confirm_request_callback pair_confirm_request_callback;
    gap_br_auth_complete_callback auth_complete_callback;
    gap_br_pair_remove_callback pair_remove_callback;
    gap_br_encrypt_complete_callback encrypt_complete_callback;
    gap_remote_uuid_changed_callback remote_uuid_changed_callback;
    gap_remote_name_changed_callback remote_name_changed_callback;
    gap_remote_alias_changed_callback remote_alias_changed_callback;
    gap_remote_cod_changed_callback remote_cod_changed_callback;
    gap_br_link_role_changed_callback link_role_changed_callback;
    gap_read_remote_rssi_event_callback read_remote_rssi_event_callback;
    gap_br_read_remote_version_info_callback read_remote_version_info_callback;
} gap_callbacks_t;

/**
 * @if Eng
 * @brief Register callback functions for GAP.
 * @par   Register callback functions for GAP.
 * @attention NULL
 * @param  [in] func gap_callbacks_t *, a pointer to the callback funcs.
 * @retval ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval Other               Error code, @ref errcode_bt_t
 * @par Dependency:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @else
 * @brief  注册GAP回调函数。
 * @par    注册上层应用的回调。
 * @attention 无
 * @param  [in] 指向回调函数的指针。
 * @retval ERRCODE_BT_SUCCESS   执行成功。
 * @retval Other               错误码, @ref errcode_bt_t。
 * @par 依赖:
 * @li  bts_br_gap.h
 * @see  gap_callbacks_t
 * @endif
 */
errcode_t gap_register_callbacks(gap_callbacks_t *func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif
