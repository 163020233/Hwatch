/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Connection API File.
 */
#ifndef SSF_CONNECTION_H
#define SSF_CONNECTION_H
#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"
#include "ssf_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS_SLE_LINK_KEY_LEN 16

/**
 * @if Eng
 * @brief Enum of discovery type.
 * @else
 * @brief 发现模式类型枚举。
 * @endif
 */
typedef enum {
    SLE_DISCOVERY_CENTRAL,    /*!< @if Eng enter discovery mode as central device, start inquiry.
                                       @else   作为中央设备进入发现态，即查询外围设备。 @endif */
    SLE_DISCOVERY_PERIPHERAL, /*!< @if Eng enter discovery mode as peripheral device, start scan.
                                       @else   作为外围设备进入发现态，即进入scan状态。 @endif */
    SLE_DISCOVERY_PERIPHERAL_FORCE_SCAN,  /*!< @if Eng force peripheral device to start scan.
                                                   @else   强制外围设备进入扫描状态。 @endif */
} bs_sle_discovery_type_t;

/**
 * @if Eng
 * @brief Enum of SLE connect state.
 * @else
 * @brief SLE连接状态枚举。
 * @endif
 */
typedef enum {
    SLE_CONNECT_NONE,         /*!< @if Eng state none, the default state.
                                        @else   默认连接状态。 @endif */
    SLE_CONNECT_CONNECTED,    /*!< @if Eng connected state.
                                        @else   已连接状态。 @endif */
    SLE_CONNECT_DISCONNECTED, /*!< @if Eng disconnected state.
                                        @else   断连状态。 @endif */
} bs_sle_connect_state_t;

/**
 * @if Eng
 * @brief Enum of SLE acb state.
 * @else
 * @brief SLE异步链路状态枚举。
 * @endif
 */
typedef enum {
    SLE_ACB_DISCONNECTED = 0, /*!< @if Eng disconnected state.
                                        @else   断连状态。 @endif */
    SLE_ACB_CONNECTED = 1,    /*!< @if Eng connected state.
                                        @else   已连接状态。 @endif */
} bs_sle_acb_connect_state_t;

/**
 * @if Eng
 * @brief Enum of SLE encrypt state.
 * @else
 * @brief SLE加密状态枚举。
 * @endif
 */
typedef enum {
    SLE_ENCRYPT_NONE,    /*!< @if Eng state none, the default state.
                                   @else   默认加密状态。 @endif */
    SLE_ENCRYPT_SUCCESS, /*!< @if Eng encrypt success.
                                   @else   加密成功。 @endif */
    SLE_ENCRYPT_FAILED,  /*!< @if Eng encrypt failed.
                                   @else   加密失败。 @endif */
} bs_sle_encrypt_state_t;

/**
 * @if Eng
 * @brief Enum of SLE pair state.
 * @else
 * @brief SLE配对状态枚举。
 * @endif
 */
typedef enum {
    SLE_PAIR_REMOVE = 0,     /*!< @if Eng connected state.
                                    @else   取消配对状态。 @endif */
    SLE_PAIR_COMPLETE = 1, /*!< @if Eng disconnected state.
                                    @else   已配对状态。 @endif */
} bs_sle_pair_state_t;

/**
 * @if Eng
 * @brief  Enum of common state.
 * @else
 * @brief  通用状态。
 * @endif
 */
typedef enum {
    BS_SLE_COMMON_STATE_DISABLE = 0x0,          /*!< @if Eng disable @else 未使能 @endif */
    BS_SLE_COMMON_STATE_ENABLE = 0x1,           /*!< @if Eng enable @else 已使能 @endif */
    BS_SLE_COMMON_STATE_ENABLING = 0x2,         /*!< @if Eng enabling @else 使能中 @endif */
    BS_SLE_COMMON_STATE_DISABLING = 0x3,        /*!< @if Eng disabling @else 去使能中 @endif */
} bs_sle_common_state_t;


/**
 * @if Eng
 * @brief  Enum of ota state.
 * @else
 * @brief  SLE OTA状态。
 * @endif
 */
typedef enum {
    BS_SLE_OTA_STATE_LEAVE,
    BS_SLE_OTA_STATE_ENTER,
    BS_SLE_OTA_STATE_END,
} bs_sle_ota_state_t;

/**
 * @if Eng
 * @brief Enum of sle link quality report parameters.
 * @else
 * @brief 星闪链路质量上报参数，bitmap表示
 * @endif
 */
typedef enum {
    BS_SLE_LINK_QUALITY_PER = 0x40,              /*!< @if Eng Error frequency.
                                                  @else   报错频率 @endif */
} bs_sle_link_quality_t;

/**
 * @if Eng
 * @brief Structure of pair information.
 * @else
 * @brief 配对信息。
 * @endif
 */
typedef struct {
    uint8_t link_key[BS_SLE_LINK_KEY_LEN];   /*!< @if Eng link key
                                                  @else   链路密钥 @endif */
    uint8_t crypto_algo;                     /*!< @if Eng encryption algorithm type { @ref sle_crypto_algo_t }
                                                  @else   加密算法类型 { @ref sle_crypto_algo_t } @endif */
    uint8_t key_deriv_algo;                  /*!< @if Eng key distribution algorithm type { @ref sle_key_deriv_algo_t }
                                                  @else   秘钥分发算法类型 { @ref sle_key_deriv_algo_t } @endif */
    uint8_t integr_chk_ind;                  /*!< @if Eng integrity check indication { @ref sle_integr_chk_ind_t }
                                                  @else   完整性校验指示 { @ref sle_integr_chk_ind_t } @endif */
    uint8_t is_bond;                         /*!< @if Eng pair bonding indication { @ref sle_bond_ind_t }
                                                  @else   配对绑定指示 { @ref sle_bond_ind_t } @endif */
} bs_sle_pair_info_t;

/**
 * @if Eng
 * @brief  struct of device name.
 * @else
 * @brief  设备名称。
 * @endif
 */
typedef struct {
    uint8_t len;   /*!< @if Eng device name length.
                        @else  名称长度，包括结束符\0。 @endif */
    uint8_t *name; /*!< @if Eng device name.
                        @else  设备名称。 @endif */
} bs_sle_device_name_t;

/**
 * @if Eng
 * @brief  struct of manufacturer specific data.
 * @else
 * @brief  厂商自定义信息。
 * @endif
 */
typedef struct {
    uint16_t company_id; /*!< @if Eng company ID.
                             @else  厂商标识。 @endif */
    uint8_t data_len;    /*!< @if Eng manufacturer specific data lenth.
                              @else  厂商自定义数据长度。 @endif */
    uint8_t *data;       /*!< @if Eng manufacturer specific data.
                           @else  厂商自定义数据。 @endif */
} bs_sle_manufacturer_specific_data_t;

/**
 * @brief  发现对端广播数据的标准解析结果
 */
typedef struct {
    uint8_t event;                                          /*!< @if Eng device discovery event type .
                                                                 @else 设备发现事件类型。 @endif */
    int8_t rssi;                                            /*!< @if Eng received signal strength indicator.
                                                                 @else 信号强度。 @endif */
    bs_sle_device_name_t full_name;                         /*!< @if Eng full device name.
                                                              @else  设备完整名称。 @endif */
    bs_sle_device_name_t short_name;                        /*!< @if Eng short device name.
                                                              @else  设备缩写名称。 @endif */
    bs_sle_manufacturer_specific_data_t manu_specific_data; /*!< @if Eng vendor custom info
                                                            @else  厂商自定义信息。 @endif */
} bs_sle_discovery_result_t;

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
    sle_addr_t addr;       /*!< @if Eng device addres.
                                @else   设备地址. */
    uint8_t conn_state;    /*!< @if Eng sle connect state. {@ref bs_sle_connect_state_t}
                                @else   sle连接状态. {@ref bs_sle_connect_state_t} */
    uint8_t encrypt_state; /*!< @if Eng sle encrypt state.
                                @else   sle加密状态. {@ref bs_sle_encrypt_state_t} */
    uint32_t reason;       /*!< @if Eng sle disconnect reason.
                                @else   sle断连原因. */
    uint8_t local_role;         /*!< @if Eng local role.
                                @else   本地角色. */
} bs_sle_connection_result_t;

/**
 * @if Eng
 * @brief Enum of sle ACB logical link update event parameters.
 * @else
 * @brief 星闪ACB逻辑链路更新事件参数
 * @endif
 */
typedef struct {
    uint16_t interval;              /*!< @if Eng interval
                                         @else   链路调度间隔，单位slot @endif */
    uint16_t latency;               /*!< @if Eng latency
                                         @else   延迟周期，单位slot @endif */
    uint16_t supervision;           /*!< @if Eng timeout
                                         @else   超时时间，单位10ms @endif */
} bs_sle_conn_param_update_result_t;


/**
 * @if Eng
 * @brief Struct of sle phy parameter.
 * @else
 * @brief 星闪phy参数
 * @endif
 */
typedef struct {
    uint8_t tx_format;          /*!< @if Eng Transmitted radio frame type, @ref bs_sle_radio_frame_t
                                     @else 发送无线帧类型，参考 { @ref bs_sle_radio_frame_t }。 @endif */
    uint8_t rx_format;          /*!< @if Eng Received radio frame type, @ref bs_sle_radio_frame_t
                                     @else 接收无线帧类型，参考 { @ref bs_sle_radio_frame_t }。 @endif */
    uint8_t tx_phy;             /*!< @if Eng Transmitted PHY, @ref bs_sle_phy_tx_rx_t
                                     @else 发送PHY，参考 { @ref bs_sle_phy_tx_rx_t }。 @endif */
    uint8_t rx_phy;             /*!< @if Eng Received PHY, @ref bs_sle_phy_tx_rx_t
                                     @else 接收PHY，参考 { @ref bs_sle_phy_tx_rx_t }。 @endif */
    uint8_t tx_pilot_density;   /*!< @if Eng Transmitted pilot density indicator, @ref bs_sle_phy_tx_rx_pilot_density_t
                                     @else 发送导频密度指示，参考 { @ref bs_sle_phy_tx_rx_pilot_density_t }。 @endif */
    uint8_t rx_pilot_density;   /*!< @if Eng Received pilot density indicator, @ref bs_sle_phy_tx_rx_pilot_density_t
                                     @else 接收导频密度指示，参考 { @ref bs_sle_phy_tx_rx_pilot_density_t }。 @endif */
    uint8_t g_feedback;         /*!< @if Eng Indicates the feedback type of the pre-transmitted link.
                                             The value range is 0 to 63.
                                     @else 先发链路反馈类型指示，取值范围0-63。 @endif */
    uint8_t t_feedback;         /*!< @if Eng Indicates the feedback type of the post-transmit link.
                                             The value range is 0-7.
                                     @else 后发链路反馈类型指示，取值范围0-7。 @endif */
} bs_sle_set_phy_t;

/**
 * @if Eng
 * @brief Struct of sle data len change parameter.
 * @else
 * @brief 星闪数据长度变化
 * @endif
 */
typedef struct {
    uint16_t max_rx_octets;          /*!< @if Eng the max octet number of receive payload
                                     @else 最大接收载荷字节数 @endif */
    uint16_t max_tx_octets;          /*!< @if Eng the max octet number of send payload
                                     @else 最大发送载荷字节数 @endif */
} bs_sle_data_len_change_result_t;

/**
 * @if Eng
 * @brief Struct of report for link quality.
 * @else
 * @brief 链路质量上报
 * @endif
 */
typedef struct {
    uint16_t    bitmap;      /*!< @if Eng Bitmap{ @ref bs_sle_link_quality_t }.
                                  @else   位图，每一bit指示不同参数使能 { @ref bs_sle_link_quality_t }@endif */
    int8_t      per;         /*!< @if Eng Error frequency.
                                  @else   报错频率 @endif */
} bs_sle_link_quality_report_t;

/**
 * @if Eng
 * @brief  Callback invoked when the device requesting connect.
 * @par    When registered, this callback notifies the upper layer
 *         when the device requesting connect.
 * @param  [in]  device_id         device id.
 * @param  [in]  addr       device address.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  配对请求的回调函数。
 * @par    注册该回调函数之后，设备请求配对时该回调函数会上报上层。
 * @param  [in]  device_id     设备id。
 * @param  [in]  addr   设备地址。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_pair_request_cbk)(uint16_t device_id, const sle_addr_t *addr);

/**
 * @if Eng
 * @brief  Callback invoked when sle connected / disconnected.
 * @par    When registered, this callback notifies the upper layer
 *         when sle is connected or disconnected.
 * @param  [in]  result     sle connection result. {@ref bs_sle_connection_result_t}
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  sle连接/断连的回调函数。
 * @par    注册该回调函数之后，sle连接或者断连后该回调函数会上报上层。
 * @param  [in]  result     sle连接状态结果。 {@ref bs_sle_connection_result_t}
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_connect_state_cbk)(bs_sle_connection_result_t *result);

/**
 * @if Eng
 * @brief  Callback invoked when the device pair complete.
 * @par    When registered, this callback notifies the upper layer
 *         when the device pair complete.
 * @param  [in]  device_id         device id.
 * @param  [in]  pair_info      pair information {@ref bs_sle_pair_info_t}.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  配对完成的回调函数。
 * @par    注册该回调函数之后，设备配对完成后该回调函数会上报上层。
 * @param  [in]  device_id     设备id。
 * @param  [in]  pair_info  配对信息{@ref bs_sle_pair_info_t}。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_pair_complete_cbk)(
    uint16_t device_id, const sle_addr_t *addr, const bs_sle_pair_info_t *pair_info);

/**
 * @if Eng
 * @brief  Callback invoked when the device pair removed.
 * @par    When registered, this callback notifies the upper layer
 *         when the device pair removed.
 * @param  [in]  device_id         device id.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  配对移除的回调函数。
 * @par    注册该回调函数之后，设备配对移除后该回调函数会上报上层。
 * @param  [in]  device_id     设备id。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_pair_remove_cbk)(uint16_t device_id);

/**
 * @if Eng
 * @brief  Callback invoked when new device is discovered.
 * @par    When registered, this callback notifies the upper layer
 *         when new device the device is discovered.
 * @param  [in]  discovery_result         discovery result.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  设备发现结果的回调函数。
 * @par    注册该回调函数之后，发现新设备后该回调函数会上报上层。
 * @param  [in]  discovery_result     设备发现结果。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_discovery_result_cbk)(
    const sle_addr_t *addr, bs_sle_discovery_result_t *discovery_result, uint8_t *data, uint16_t data_len);

/**
 * @if Eng
 * @brief  Callback invoked when discovery state changed.
 * @par    When registered, this callback notifies the upper layer
 *         when discovery state changed.
 * @param  [in]  type The connection type, which specifies the local node's role (central or peripheral) during the
 * discovery process {@ref bs_sle_discovery_type_t}.
 * @param  [in]  state current discover state {@ref bs_sle_common_state_t}.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  设备发现状态改变的回调函数。
 * @par    注册该回调函数之后，设备发现状态改变后该回调函数会上报上层。
 * @param  [in]  type 连接类型，指定在发现过程中本端的角色（中心节点或外围节点）{@ref bs_sle_discovery_type_t}。
 * @param  [in]  state 当前的设备发现状态 {@ref bs_sle_common_state_t}。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_discovery_state_cbk)(uint8_t type, uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when auto connect state changed.
 * @par    When registered, this callback notifies the upper layer
 *         when auto connect state changed.
 * @param  [in]  state current auto connect state {@ref bs_sle_common_state_t}.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  回连状态改变的回调函数。
 * @par    注册该回调函数之后，回连状态改变后该回调函数会上报上层。
 * @param  [in]  state 当前的回连状态 {@ref bs_sle_common_state_t}。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_auto_conn_state_cbk)(uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked when read remote device RSSI.
 * @par    When registered, this callback notifies the upper layer
 *         when remote device RSSI is read.
 * @param  [in]  dev_id device ID.
 * @param  [in]  rssi RSSI value.
 * @param  [in]  status RSSI status.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  读取RSSI的回调函数。
 * @par    注册该回调函数之后，读取到RSSI信息后该回调函数会上报上层。
 * @param  [in]  dev_id   设备ID.
 * @param  [in]  rssi 读取到的RSSI值.
 * @param  [in]  status 读取到的RSSI状态.
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_read_rssi_cbk)(uint16_t dev_id, int8_t rssi, uint8_t status);

/**
 * @if Eng
 * @brief  Callback invoked when connection parameter updated.
 * @par    When registered, this callback notifies the upper layer
 *         when connection parameter updated.
 * @param  [in]  dev_id device ID.
 * @param  [in]  status status of connect parameter update.
 * @param  [in]  param  parameter of connection.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  连接参数更新的回调函数。
 * @par    注册该回调函数之后，连接参数更新后该回调函数会上报上层。
 * @param  [in]  dev_id 设备ID.
 * @param  [in]  status 连接参数更新的状态.
 * @param  [in]  param  更新后的连接参数.
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_conn_param_update_cbk)(uint16_t dev_id, errcode_t status,
    const bs_sle_conn_param_update_result_t *param);

 /**
 * @if Eng
 * @brief Callback invoked when set PHY complete.
 * @par Callback invoked when set PHY complete.
 * @param  [in]  dev_id device ID.
 * @param  [in]  status  result of setting the PHY.
 * @param  [in]  param   current PHY parameters { @ref bs_sle_set_phy_t }.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  设置PHY的回调函数。
 * @par    设置PHY的回调函数。
 * @param  [in]  dev_id  设备ID。
 * @param  [in]  status  设置PHY结果。
 * @param  [in]  param   当前PHY参数 { @ref bs_sle_set_phy_t }。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_set_phy_cbk)(uint16_t dev_id, errcode_t status, const bs_sle_set_phy_t *param);

 /**
 * @if Eng
 * @brief Callback invoked when set data length complete.
 * @par Callback invoked when set data length complete.
 * @param  [in]  dev_id device ID.
 * @param  [in]  status  result of setting the data length.
 * @param  [in]  param   current data length parameters { @ref bs_sle_data_len_change_result_t }.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  设置数据长度的回调函数。
 * @par    设置数据长度的回调函数。
 * @param  [in]  dev_id  设备ID。
 * @param  [in]  status  设置数据长度结果。
 * @param  [in]  param   当前数据长度参数 { @ref bs_sle_data_len_change_result_t }。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_set_data_len_cbk)(
    uint16_t dev_id, errcode_t status, const bs_sle_data_len_change_result_t *param);

 /**
 * @if Eng
 * @brief Callback invoked when link quality report.
 * @par Callback invoked when link quality report
 * @param  [in]  dev_id   device ID.
 * @param  [in]  status   status error code
 * @param  [in]  report   link quality report { @ref bs_sle_link_quality_report_t }.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  连接质量上报回调。
 * @par    连接质量上报回调。
 * @param  [in]  dev_id  设备ID。
 * @param  [in]  status  执行结果错误码。
 * @param  [in]  report  质量参数 { @ref bs_sle_link_quality_report_t }。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_quality_report_cbk)(
    uint16_t dev_id, errcode_t status, const bs_sle_link_quality_report_t *report);

/**
 * @if Eng
 * @brief Struct of sle service callback function.
 * @else
 * @brief sle service回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_pair_request_cbk pair_request_cb;            /*!< @if Eng SLE pair request callback. If the function for reporting
                                                                     pairing requests is enabled, when a pairing request from
                                                                     the peer is received, this callback is reported.
                                                             @else   星闪配对请求的回调函数。打开配对请求上报时，收到对端配对
                                                                     请求会上报该回调。@endif */
    bs_sle_connect_state_cbk connect_state_cb;          /*!< @if Eng SLE device connect state callback. This callback
                                                                     is reported when the connection status and encryption
                                                                     status of devices changed.
                                                             @else   星闪（非合作设备集内设备）连接状态的回调函数。
                                                                     当连接状态以及加密状态发生改变时上报该回调。 @endif */
    bs_sle_connect_state_cbk member_connect_state_cb;   /*!< @if Eng SLE device in cooperation device set connect state
                                                                     callback. This callback is reported when the connection
                                                                     status and encryption status of devices within the SLE
                                                                     cooperation device set change.
                                                             @else   星闪合作设备集内的设备间连接状态的回调函数。
                                                                     当星闪合作设备集内设备连接状态以及加密状态发生改变时
                                                                     上报该回调。 @endif */
    bs_sle_pair_complete_cbk pair_complete_cb;          /*!< @if Eng SLE pair complete callback. When the pairing
                                                                     process is finished, this callback will report
                                                                     and notify the pairing information.
                                                             @else   星闪配对完成的回调函数。当配对流程成功后，
                                                                     该回调会上报并通知配对信息。 @endif */
    bs_sle_pair_remove_cbk pair_remove_cb;              /*!< @if Eng SLE remove pair callback. When the device pairing
                                                                     information is removed, this callback will report
                                                                     and notify the result.
                                                             @else   星闪移除配对完成的回调函数。当移除设备配对信息后，
                                                                     该回调会上报并通知结果。 @endif */
    bs_sle_discovery_result_cbk discovery_result_cb;    /*!< @if Eng SLE remove pair callback. When the device pairing
                                                                     information is removed, this callback will report
                                                                     and notify the result.
                                                             @else   星闪发现结果上报的回调函数。当移除设备配对信息后，
                                                                     该回调会上报并通知结果。 @endif */
    bs_sle_discovery_state_cbk discovery_state_cb;      /*!< @if Eng SLE discovery state report callback. This callback
                                                                     is reported when entering or exiting the discovery
                                                                     state.
                                                             @else   星闪发现状态上报的回调函数。当进入或退出发现状态后上报
                                                                     该回调。@endif */
    bs_sle_auto_conn_state_cbk auto_conn_state_cb;      /*!< @if Eng SLE automatic connect state report callback. This
                                                                     callback is reported when entering or exiting the
                                                                     automatic reconnection state.
                                                             @else   星闪自动回连状态上报的回调函数。当进入或退出自动回连状态
                                                                     后上报该回调。@endif */
    bs_sle_read_rssi_cbk read_rssi_cbk;                 /*!< @if Eng read rssi result callback
                                                             @else   读取到的RSSI结果回调函数 @endif */
    bs_sle_conn_param_update_cbk conn_param_update_cbk; /*!< @if Eng connect parameters update callback
                                                             @else   连接参数更新结果回调函数 @endif */
    bs_sle_set_phy_cbk set_phy_cb;                     /*!< @if Eng set PHY parameters callback
                                                             @else   设置PHY参数结果回调函数 @endif */
    bs_sle_set_data_len_cbk set_data_length_cb;         /*!< @if Eng set data length callback
                                                             @else   设置数据长度结果回调函数 @endif */
    bs_sle_quality_report_cbk link_quality_report_event_cb; /*!< @if Eng link quality report callback
                                                             @else   链路质量上报回调函数 @endif */
} bs_sle_callbacks_t;

/**
 * @if Eng
 * @brief Registers the callback function for reporting the pairing status.
 * @par Description: This function is used to register a callback function that reports changes in the device pairing
 * status.
 * @param[in] id The identifier used to distinguish different applications.
 * @param[in] func The callback function to be registered.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 注册配对状态上报回调函数。
 * @par Description: 该函数用于注册一个回调函数，用于在设备配对状态发生变化后上报。
 * @param[in] id 用于区分不同应用的标识。
 * @param[in] func 要注册的回调函数。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_register_callbacks(uint32_t app_id, bs_sle_callbacks_t *cbks);

/**
 * @if Eng
 * @brief Unregister the callback function for reporting the pairing status.
 * @par Description: This function is used to deregister the callback function that was previously registered for
 * reporting changes in pairing status.
 * @param[in] id The identifier used to distinguish different applications.
 * @retval void
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 去注册配对状态上报回调函数。
 * @par Description: 该函数用于注销之前注册的配对状态发生变化后上报的回调函数。
 * @param[in] id 用于区分不同应用的标识。
 * @retval void
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
void bs_sle_unregister_callbacks(uint32_t app_id);

/**
 * @if Eng
 * @brief Start device discovery process.
 * @par Description: This function starts the device discovery process. Depending on the specified type, it will
 * initiate either broadcasting or scanning on the local node. After the discovery process completes, the results will
 * be notified through a callback function.
 * @param[in] type The connection type, which specifies the local node's role (central or peripheral) during the
 * discovery process.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note If the discovery process is interrupted or terminated before completion, the callback function will not be
 * triggered.
 * @else
 * @brief 启动设备发现过程
 * @par Description:
 * 该函数用于启动设备发现过程，根据传入的type，会开启本节点的广播或者扫描逻辑。发现过程完成后，会通过回调函数通知结果。
 * @param[in] type 连接类型，指定在发现过程中本端的角色（中心节点或外围节点）。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 如果发现过程在完成前被中断或终止，回调函数将不会被触发。
 * @endif
 */
errcode_t bs_sle_start_discovery(bs_sle_discovery_type_t type);

/**
 * @if Eng
 * @brief Stop device discovery process.
 * @par Description: This function stops the currently running device discovery process.
 * @param[in] type The connection type, which specifies the local node's role (central or peripheral) during the
 * discovery process.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note If the discovery process is not running, calling this function will have no effect.
 * @else
 * @brief 停止设备发现过程
 * @par Description: 该函数用于停止当前正在运行的设备发现过程。
 * @param[in] type 连接类型，指定在发现过程中本端的角色（中心节点或外围节点）。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 如果发现过程未在运行中，调用此函数将不会产生任何效果。
 * @endif
 */
errcode_t bs_sle_stop_discovery(bs_sle_discovery_type_t type);

/**
 * @if Eng
 * @brief Start pairing function.
 * @par Description: This function starts the pairing process for a discovered device.
 * @param[in] addr The address information of the device to be paired.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 启动配对函数。
 * @par Description: 该函数用于启动已发现设备的配对逻辑。
 * @param[in] addr 要配对的设备地址信息。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_start_pair(const sle_addr_t *addr);

/**
 * @if Eng
 * @brief Disconnect from a paired device function.
 * @par Description: This function disconnects from a specific paired device.
 * @param[in] sle_obj The object representing the paired device to disconnect from.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note After calling this function, the connection to the device will be terminated, and the connection state will be
 * cleared. You will need to reconnect to communicate with the device again.
 * @else
 * @brief 断开与配对设备的连接函数。
 * @par Description: 该函数用于断开与某个配对设备的连接。
 * @param[in] sle_obj 要断开连接的配对设备对象。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 调用此函数后，与该设备的连接将被终止，连接状态将被清除。若要再次与该设备通信，需要重新建立连接。
 * @endif
 */
errcode_t bs_sle_disconnect(sle_obj_t obj);

/**
 * @if Eng
 * @brief Disconnect from all paired devices function.
 * @par Description: This function disconnects from all currently paired devices.
 * @param[in] None.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note After calling this function, the connections to all devices will be terminated, and the connection states will
 * be cleared. You will need to reconnect to communicate with the devices again.
 * @else
 * @brief 断开与配对设备的连接函数。
 * @par Description: 该函数用于断开与某个配对设备的连接。
 * @param[in] 无。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 调用此函数后，与所有设备的连接将被终止，连接状态将被清除。若要再次与设备通信，需要重新建立连接。
 * @endif
 */
errcode_t bs_sle_disconnect_all(void);

/**
 * @if Eng
 * @brief Remove paired device function.
 * @par Description: This function removes the pairing data of a specific device.
 * @param[in] sle_obj The object representing the paired device to be removed.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note After calling this function, the pairing data of the device will be deleted, and it will no longer
 * automatically reconnect in the background.
 * @else
 * @brief 删除配对设备函数。
 * @par Description: 该函数用于删除某个设备的配对数据。
 * @param[in] sle_obj 要删除的配对设备对象。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 调用此函数后，该设备的配对数据将被删除，后续将无法在背景回连中自动连接。
 * @endif
 */
errcode_t bs_sle_remove_pair(sle_obj_t obj);

/**
 * @if Eng
 * @brief Remove all paired devices function.
 * @par Description: This function removes the pairing data of all devices.
 * @param[in] None.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_connection.h
 * @note After calling this function, the pairing data of all devices will be deleted, and it will no longer
 * automatically reconnect in the background.
 * @else
 * @brief 删除所有配对设备函数。
 * @par Description: 该函数用于删除所有设备的配对数据。
 * @param[in] 无。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @note 调用此函数后，所有设备的配对数据将被删除，后续将无法在背景回连中自动连接。
 * @endif
 */
errcode_t bs_sle_remove_all_pairs(void);

/**
 * @if Eng
 * @brief start background reconnection.
 * @par Description: start background reconnection function.
 * @param[in] None
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 开启背景回连功能
 * @par Description: 开启背景回连功能。
 * @param[in] 无
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_start_auto_conn(void);

/**
 * @if Eng
 * @brief stop background reconnection.
 * @par Description: stop background reconnection function.
 * @param[in] None
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 停止背景回连功能
 * @par Description: 停止背景回连功能。
 * @param[in] 无
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_stop_auto_conn(void);

/**
 * @if Eng
 * @brief Use this function to set connection parameters for the specified device ID by
 *   the preset configuration information corresponding to the specified mode ID.
 * @par Use preset parameters to configure connection parameters.
 * @param [in] dev_id device ID
 * @param [in] mode_id mode ID
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_t
 * @par Dependency:Before using this function, ensure that the preset connection parameters have been added.
 * @li
 * @else
 * @brief  使用指定的模式ID对应的配置对指定的设备ID的连接进行连接参数设置。
 * @par    BLE 使用预置参数对连接参数进行设置。
 * @param [in] dev_id 设备ID
 * @param [in] mode_id 模式ID
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_t
 * @par 依赖: 使用该函数前需已配置对应的预置连接参数。
 * @li
 * @endif
 */
errcode_t bs_sle_set_connect_param(uint16_t dev_id, uint16_t mode_id);

/**
 * @if Eng
 * @brief Use this function to read Received Signal Strength Indicator.
 * @par Use this function to read Received Signal Strength Indicator.
 * @param [in] dev_id device ID
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_t
 * @par Dependency:This interface can only be called after the callback function for reading RSSI has been registered.
 * @li
 * @else
 * @brief  读取对端设备的RSSI信息。
 * @par    读取对端设备的RSSI信息。
 * @param [in] dev_id 设备ID
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_t
 * @par 依赖: 该接口需要先注册读取RSSI的回调函数后才能调用
 * @li
 * @endif
 */
errcode_t bs_sle_read_remote_device_rssi(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to set PHY parameters for the specified device ID by
 *   the preset configuration information corresponding to the specified mode ID.
 * @par Use preset parameters to configure PHY parameters.
 * @param [in] dev_id device ID
 * @param [in] mode_id mode ID
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_sle_t
 * @par Dependency:Before using this function, ensure that the preset PHY parameters have been added.
 * @li
 * @else
 * @brief  使用指定的模式ID对应的配置对指定的设备ID的PHY参数进行设置。
 * @par    SLE 使用预置参数对PHY参数进行设置。
 * @param [in] dev_id 设备ID
 * @param [in] mode_id 模式ID
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_sle_t
 * @par 依赖: 使用该函数前需已配置对应的预置PHY参数。
 * @li
 * @endif
 */
errcode_t bs_sle_set_phy_param(uint16_t dev_id, uint16_t mode_id);

/**
 * @if Eng
 * @brief Use this funtion to set the SLE packet transmission parameters.
 * @par Use this funtion to set the SLE packet transmission parameters.
 * @param [in] dev_id  device ID
 * @param [in] tx_octets Maximum number of payload bytes that can be transmitted on the link.
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_sle_t
 * @par Dependency:
 * @li
 * @else
 * @brief  设置SLE发包参数。
 * @par    设置SLE发包参数。
 * @param [in] dev_id 设备ID
 * @param [in] tx_octets 连接链路上所偏好的最大传输payload字节数.
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_sle_t
 * @par 依赖:
 * @li
 * @endif
 */
errcode_t bs_sle_set_data_length(uint16_t dev_id, uint16_t tx_octets);

/**
 * @if Eng
 * @brief start directed reconnection.
 * @par Description: start directed reconnection function.
 * @param[in] None
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 开启定向回连功能
 * @par Description: 开启定向回连功能。
 * @param[in] 无
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_start_directed_reconnect(sle_addr_t *dest_addr);

/**
 * @if Eng
 * @brief stop directed reconnection.
 * @par Description: stop directed reconnection function.
 * @param[in] None
 * @retval errcode_t ERRCODE_SUCC on success, error code on failure.
 * @par Depends:
 * @li ssf_connection.h
 * @else
 * @brief 停止定向回连功能
 * @par Description: 停止定向回连功能。
 * @param[in] 无
 * @retval errcode_t 成功时返回ERRCODE_SUCC，失败时返回错误码。
 * @par 依赖：
 * @li ssf_connection.h
 * @endif
 */
errcode_t bs_sle_stop_directed_reconnect(void);

/**
 * @if Eng
 * @brief Use this funtion to set the SLE ota state.
 * @par Use this funtion to set the SLE ota state.
 * @param [in] obj  sle obj
 * @param [in] ota_state state of ota. @ref bs_sle_ota_state_t
 * @retval #ERRCODE_SLE_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_sle_t
 * @li
 * @else
 * @brief  设置星闪OTA状态。
 * @par    设置星闪OTA状态。
 * @param [in] obj 星闪对象
 * @param [in] ota_state OTA状态. @ref bs_sle_ota_state_t
 * @retval #ERRCODE_SLE_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_sle_t
 * @li
 * @endif
 */
errcode_t bs_sle_set_ota_state(sle_obj_t obj, uint8_t ota_state);
#ifdef __cplusplus
}
#endif
#endif /* SSF_CONNECTION_H */
