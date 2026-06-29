/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BLE serivce config enums
 */

#ifndef BS_BLE_SERVICE_CONFIG_H
#define BS_BLE_SERVICE_CONFIG_H

#include "bs_device_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t dev_id;            /*!< @if Eng conn handle
                                    @else   连接句柄 @endif */
    uint8_t all_phys;           /*!< @if Eng all phys
                                    @else   所有PHY @endif */
    uint8_t tx_phys;            /*!< @if Eng tx phys
                                    @else   发送PHY @endif */
    uint8_t rx_phys;            /*!< @if Eng rx phys
                                    @else   接收PHY @endif */
    uint16_t phy_options;       /*!< @if Eng phy options
                                    @else   PHY选项 @endif */
} bs_ble_set_phy_t;

typedef struct {
    uint16_t dev_id;             /*!< @if Eng connection id
                                      @else   连接 ID @endif */
    uint16_t interval_min;       /*!< @if Eng min interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最小间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t interval_max;       /*!< @if Eng max interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最大间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t slave_latency;      /*!< @if Eng slave reply min latency,the value range is 0-499
                                      @else   从设备回复最小间隔，值范围0-499 @endif */
    uint16_t timeout_multiplier; /*!< @if Eng interval for disconnection due to timeout,
                                              the value range is 10-3200,the unit is 10ms
                                      @else   超时断连间隔，值范围10-3200，单位为10ms @endif */
} bs_ble_conn_param_update_t;

typedef enum {
    BLE_CONFIG_LOCAL     = 0x0000 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BLE_CONFIG_ADV       = 0x0100 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BLE_CONFIG_SCAN      = 0x0200 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BLE_CONFIG_CONNECT   = 0x0300 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BLE_CONFIG_PAIR      = 0x0400 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BLE_CONFIG_RECONNECT = 0x0500 | (BS_PROTOCOL_BLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
} bs_ble_config_group_t;

/**
 * @if Eng
 * @brief Enum of config parameters type.
 * @else
 * @brief 配置参数类型枚举.
 * @endif
 */
typedef enum {
    BLE_CONFIG_LOCAL_ADDR            = BLE_CONFIG_LOCAL | 0x0,
    BLE_CONFIG_LOCAL_NAME            = BLE_CONFIG_LOCAL | 0x1,
    BLE_CONFIG_LOCAL_APPEARANCE      = BLE_CONFIG_LOCAL | 0x2, /* gap_ble_appearance_type_t */
    BLE_CONFIG_PAIR_INFO_AVAILABLE   = BLE_CONFIG_LOCAL | 0x3, /* gap_ble_set_pair_info_available */
    BLE_CONFIG_ACL_SWITCH_USE        = BLE_CONFIG_LOCAL | 0x4, /*!< @if Eng config acl switch.
                                                                {@ref bs_ble_config_enable_t}
                                                                @else 配置BLE链路无感切换开关
                                                                {@ref bs_ble_config_enable_t}
                                                                @endif */
    BLE_CONFIG_PEER_ADDR             = BLE_CONFIG_LOCAL | 0x5, /*!< @if Eng config tws peer le addr. {@ref char *}
                                                                @else 配置tws对耳BLE地址 {@ref char *} @endif */

    BLE_CONFIG_ADV_UNDIR_SLOW        = BLE_CONFIG_ADV | 0x1, /* !< @if Eng Use the slow gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_SLOW, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of slow gear.
                 @else
                使用低速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_SLOW 的宏值相同，则对应的用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统低速挡默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_UNDIR_MIDDLE      = BLE_CONFIG_ADV | 0x2, /* !< @if Eng Use the middle gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_MIDDLE, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of middle gear.
                 @else
                使用中速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_MIDDLE 的宏值相同，则对应用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统中速档默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_UNDIR_FAST        = BLE_CONFIG_ADV | 0x3, /* !< @if Eng Use the fast gear to set the parameters.
                If preset advertising parameters have been added by using the BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset advertising parameters is the same as the macro
                value of BLE_CONFIG_ADV_UNDIR_FAST, the preset advertising parameters take effect first. If the preset
                advertising parameters are not added, the parameters take effect by default of fast gear.
                 @else
                使用快速档位对广播参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的
                广播参数，且添加的预置广播参数的模式ID与 BLE_CONFIG_ADV_UNDIR_FAST 的宏值相同，则对应的用户预置广播参数优先生效；
                若未添加对应的用户预置广播参数，则系统高速档默认参数生效。
                 @endif */
    BLE_CONFIG_ADV_DIR_SLOW          = BLE_CONFIG_ADV | 0x4,
    BLE_CONFIG_ADV_DIR_FAST          = BLE_CONFIG_ADV | 0x5,
    BLE_CONFIG_ADV_TXPOWER           = BLE_CONFIG_ADV | 0x6,
    BLE_CONFIG_ADV_WHITE_LIST        = BLE_CONFIG_ADV | 0x7,
    BLE_CONFIG_ADV_DATA              = BLE_CONFIG_ADV | 0x8,
    BLE_CONFIG_ADV_SCAN_RSP          = BLE_CONFIG_ADV | 0x9,
    BLE_CONFIG_ADV_PEER_ADDR         = BLE_CONFIG_ADV | 0xA,
    BLE_CONFIG_ADV_DURATION          = BLE_CONFIG_ADV | 0xB, /* 只可用于配置回连广播的持续时间 */
    BLE_CONFIG_ADV_OPR_CUSTOM_PARAM  = BLE_CONFIG_ADV | 0xC, /* !< @if Eng Add preset advertising parameters
                                                                {@ref bs_ble_opr_pre_set_adv_param_t *}
                                                            @else 添加用户预制广播参数
                                                                {@ref bs_ble_opr_pre_set_adv_param_t *}
                                                            @endif */

    BLE_CONFIG_SCAN_MIDDLE           = BLE_CONFIG_SCAN | 0x0,/* !< @if Eng Use the middle gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_MIDDLE, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of middle gear.
                 @else
                使用中速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_MIDDLE 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统中速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_SLOW             = BLE_CONFIG_SCAN | 0x1,/* !< @if Eng Use the slow gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_SLOW, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of slow gear.
                 @else
                使用低速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_SLOW 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统低速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_FAST             = BLE_CONFIG_SCAN | 0x2,/* !< @if Eng Use the fast gear to set the parameters.
                If preset scanning parameters have been added by using the BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before the
                level configuration, and the mode ID of the added preset scanning parameters is the same as the macro
                value of BLE_CONFIG_SCAN_FAST, the preset scanning parameters take effect first. If the preset
                scanning parameters are not added, the parameters take effect by default of fast gear.
                 @else
                使用高速档位对扫描参数进行设置。如果在进行档位配置前已使用 BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的
                扫描参数，且添加的预置扫描参数的模式ID与 BLE_CONFIG_SCAN_FAST 的宏值相同，则对应的用户预置扫描参数优先生效；
                若未添加对应的用户预置扫描参数，则系统高速档默认参数生效。
                 @endif */
    BLE_CONFIG_SCAN_TYPE             = BLE_CONFIG_SCAN | 0x3,
    BLE_CONFIG_SCAN_WHITE_LIST       = BLE_CONFIG_SCAN | 0x4,
    BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM  = BLE_CONFIG_SCAN | 0x5, /* !< @if Eng Add preset scan parameters
                                                                {@ref bs_ble_opr_pre_set_scan_param_t *}
                                                            @else 添加用户预制扫描参数
                                                                {@ref bs_ble_opr_pre_set_scan_param_t *}
                                                            @endif */

    BLE_CONFIG_CONNECT_SLOW          = BLE_CONFIG_CONNECT | 0x0,
    BLE_CONFIG_CONNECT_HIGH          = BLE_CONFIG_CONNECT | 0x1,
    BLE_CONFIG_CONNECT_UPDATE        = BLE_CONFIG_CONNECT | 0x2, /* !< @if Eng config update.
                                                                      {@ref bs_ble_conn_param_update_t *}
                                                                 @else 更新连接参数
                                                                      {@ref bs_ble_conn_param_update_t *}
                                                                 @endif */
    BLE_CONFIG_CONNECT_PHY           = BLE_CONFIG_CONNECT | 0x3, /* !< @if Eng config PHY. {@ref bs_ble_set_phy_t *}
                                                                    @else 更新物理层参数 {@ref bs_ble_set_phy_t *} @endif */

    BLE_CONFIG_CONNECT_PRESET_CONN   = BLE_CONFIG_CONNECT | 0x4, /* !< @if Eng config pre-set connection params. {@ref bs_ble_opr_pre_set_conn_param_t *}
                                                                    @else 配置预定义的连接参数 {@ref bs_ble_opr_pre_set_conn_param_t *} @endif */

    BLE_CONFIG_CONNECT_PRESET_PHY    = BLE_CONFIG_CONNECT | 0x5, /* !< @if Eng config pre-set PHY. {@ref bs_ble_opr_pre_set_phy_param_t *}
                                                                    @else 配置预定义的连接参数 {@ref bs_ble_opr_pre_set_phy_param_t *} @endif */

    BLE_CONFIG_PAIR_USE_CTDK          = BLE_CONFIG_PAIR | 0x0,      /*!< @if Eng config ctdk.
                                                                            {@ref bs_ble_config_enable_t}
                                                                         @else 配置ctdk
                                                                            {@ref bs_ble_config_enable_t}
                                                                         @endif */
    BLE_CONFIG_PAIR_NOT_AUTO_PAIR     = BLE_CONFIG_PAIR | 0x1,  /*!< @if Eng This parameter is used to configure
                                                                    whether to disable automatic pair after the
                                                                    connection is complete.{@ref bs_ble_config_enable_t}
                                                                @else 用于配置在完成连接后是否不自动进行配对。
                                                                    {@ref bs_ble_config_enable_t}
                                                                @endif */

    BLE_CONFIG_RECONNECT_CENTRAL      = BLE_CONFIG_RECONNECT | 0x0,
    BLE_CONFIG_RECONNECT_PERIPHERAL   = BLE_CONFIG_RECONNECT | 0x1,
    BLE_CONFIG_RECONNECT_USE_WL       = BLE_CONFIG_RECONNECT | 0x2,
    BLE_CONFIG_RECONNECT_SCAN_RECONN  = BLE_CONFIG_RECONNECT | 0x3,

    BLE_CONFIG_RECONNECT_ADD_WL       = BLE_CONFIG_RECONNECT | 0x10,
    BLE_CONFIG_RECONNECT_RMV_WL       = BLE_CONFIG_RECONNECT | 0x11,

    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_RSSI = BLE_CONFIG_RECONNECT | 0x20,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_UUID = BLE_CONFIG_RECONNECT | 0x21,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_ADD_CHECK_NAME = BLE_CONFIG_RECONNECT | 0x22,

    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_RSSI = BLE_CONFIG_RECONNECT | 0x30,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_UUID = BLE_CONFIG_RECONNECT | 0x31,
    BLE_CONFIG_RECONNECT_SCAN_RECONN_RMV_CHECK_NAME = BLE_CONFIG_RECONNECT | 0x32,
} bs_ble_config_type_t;

/**
 * @if Eng
 * @brief Enum of config switch parameter.
 * @else
 * @brief 配置开关参数枚举。
 * @endif
 */
typedef enum {
    BS_BLE_DISABLE,
    BS_BLE_ENABLE,
} bs_ble_config_enable_t;

typedef struct {
    uint16_t interval_min;       /*!< @if Eng min interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最小间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t interval_max;       /*!< @if Eng max interval,the value range is 6-3200,the unit is 1.25ms
                                      @else   最大间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t slave_latency;      /*!< @if Eng slave reply min latency,the value range is 0-499
                                      @else   从设备回复最小间隔，值范围0-499 @endif */
    uint16_t timeout_multiplier; /*!< @if Eng interval for disconnection due to timeout,
                                              the value range is 10-3200,the unit is 10ms
                                      @else   超时断连间隔，值范围10-3200，单位为10ms @endif */
} bs_ble_conn_param_t;

typedef struct {
    uint8_t all_phys;     /*!< @if Eng all phys
                              @else   所有PHY @endif */
    uint8_t tx_phys;      /*!< @if Eng tx phys
                              @else   发送PHY @endif */
    uint8_t rx_phys;      /*!< @if Eng rx phys
                              @else   接收PHY @endif */
    uint16_t phy_options; /*!< @if Eng phy options
                              @else   PHY选项 @endif */
} bs_ble_phy_param_t;

typedef struct {
    uint16_t scan_interval;     /*!< @if Eng Scan interval[N * 0.625ms]
                                     @else   扫描间隔，[N * 0.625 ms] @endif */
    uint16_t scan_window;       /*!< @if Eng Scan window[N * 0.625ms]
                                     @else   扫描窗长，[N * 0.625 ms] @endif */
    uint8_t scan_type;          /*!< @if Eng Scan type { @ref gap_ble_scan_type_t }
                                     @else   扫描类型 { @ref gap_ble_scan_type_t } @endif */
    uint8_t scan_phy;           /*!< @if Eng PHY type { @ref gap_ble_phy_type_t }
                                     @else   PHY类型 { @ref gap_ble_phy_type_t } @endif */
    uint8_t scan_filter_policy; /*!< @if Eng Scan fileter policy { @ref gap_ble_scan_filter_policy_t }
                                     @else   扫描过滤策略 { @ref gap_ble_scan_filter_policy_t } @endif */
} bs_ble_scan_param_t;

typedef struct {
    uint32_t min_interval;     /*!< @if Eng Min interval[N * 0.625ms]
                                            The value range: 0x0020 to 0x4000
                                    @else   最小的广播间隔[N * 0.625ms]
                                            可取值范围: 0x0020 to 0x4000 @endif */
    uint32_t max_interval;     /*!< @if Eng Max interval[N * 0.625ms]
                                            The value range: 0x0020 to 0x4000
                                    @else   最大的广播间隔[N * 0.625ms]
                                            可取值范围: 0x0020 to 0x4000 @endif */
    uint8_t adv_type;          /*!< @if Eng Advertising type { @ref gap_ble_adv_type_t }
                                    @else   广播类型 { @ref gap_ble_adv_type_t } @endif */
    uint8_t channel_map;       /*!< @if Eng channel bitmap
                                    @else   广播通道选择:
                                            0x01----使用37通道
                                            0x07----使用37/38/39三个通道 @endif */
    int8_t  tx_power;          /*!< @if Eng Transmissive power, The unit is dBm. The value ranges from -127 to 20.
                                    @else   发送功率,单位dbm,范围-127~20 @endif */
    uint16_t duration;         /*!< @if Eng Duration. unit: 10ms. The value ranges from 0x0 to 0xFFFF.
                                            The value 0x0 indicates that the broadcast time is not limited.
                                    @else   广播持续发送时长，取值范围0x0~0xFFFF，0x0表示广播时间无限制。单位10ms @endif */
} bs_ble_adv_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index. The value cannot be 0.
                                         @else   模式id，用于作为唯一配置索引，值不能等于0 @endif */
    bs_ble_conn_param_t para; /*!< @if Eng connection paramter
                                  @else   连接参数 @endif */
} bs_ble_pre_set_conn_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index. The value cannot be 0.
                                 @else   模式id，用于作为唯一配置索引，值不能等于0 @endif */
    bs_ble_phy_param_t para; /*!< @if Eng PHY paramter
                                  @else   PHY参数 @endif */
} bs_ble_pre_set_phy_param_t;

typedef enum {
    BS_BLE_SCAN_MODEID_FAST = BLE_CONFIG_SCAN_FAST, /* !< @if Eng custom preset fast gear.
                                If preset scanning parameters have been added by using the
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before using BLE_CONFIG_SCAN_FAST to config,
                                and the mode ID of the added preset scanning parameters is BS_BLE_SCAN_MODEID_FAST,
                                the preset scanning parameters take effect first.
                            @else 自定义的预置高速档位。如果在使用 BLE_CONFIG_SCAN_FAST 进行档位配置前已使用
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的 扫描参数，且添加的预置扫描参数的模式ID为
                                BS_BLE_SCAN_MODEID_FAST ，则对应的用户预置扫描参数优先生效。@endif */
    BS_BLE_SCAN_MODEID_MIDDLE = BLE_CONFIG_SCAN_MIDDLE, /* !< @if Eng custom preset middle gear.
                                If preset scanning parameters have been added by using the
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before using BLE_CONFIG_SCAN_MIDDLE to config,
                                and the mode ID of the added preset scanning parameters is BS_BLE_SCAN_MODEID_MIDDLE,
                                the preset scanning parameters take effect first.
                            @else 自定义的预置中速档位。如果在使用 BLE_CONFIG_SCAN_MIDDLE 进行档位配置前已使用
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的 扫描参数，且添加的预置扫描参数的模式ID为
                                BS_BLE_SCAN_MODEID_MIDDLE ，则对应的用户预置扫描参数优先生效。@endif */
    BS_BLE_SCAN_MODEID_SLOW = BLE_CONFIG_SCAN_SLOW, /* !< @if Eng custom preset slow gear.
                                If preset scanning parameters have been added by using the
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM before using BLE_CONFIG_SCAN_SLOW to config,
                                and the mode ID of the added preset scanning parameters is BS_BLE_SCAN_MODEID_SLOW,
                                the preset scanning parameters take effect first.
                            @else 自定义的预置低速档位。如果在使用 BLE_CONFIG_SCAN_SLOW 进行档位配置前已使用
                                BLE_CONFIG_SCAN_OPR_CUSTOM_PARAM 添加了用户预置的 扫描参数，且添加的预置扫描参数的模式ID为
                                BS_BLE_SCAN_MODEID_SLOW ，则对应的用户预置扫描参数优先生效。@endif */
} bs_ble_pre_set_scan_param_mode_id_t;

typedef struct {
    uint16_t mode_id; /*!< @if Eng mode id, usd as unique configuration index.
                            {@ref bs_ble_pre_set_scan_param_mode_id_t}.
                          @else   模式id，用于作为唯一配置索引。 {@ref bs_ble_pre_set_scan_param_mode_id_t}.
                          @endif */
    bs_ble_scan_param_t para; /*!< @if Eng scan paramter
                                  @else   扫描参数 @endif */
} bs_ble_pre_set_scan_param_t;

typedef enum {
    BS_BLE_ADV_MODEID_UNDIR_FAST    = BLE_CONFIG_ADV_UNDIR_FAST, /* !< @if Eng custom preset fast gear.
                                If preset advertising parameters have been added by using the
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before using BLE_CONFIG_ADV_UNDIR_FAST to config,
                                and the mode ID of the added preset advertising parameters is
                                BS_BLE_ADV_MODEID_UNDIR_FAST, the preset advertising parameters take effect first.
                            @else 自定义的预置高速档位。如果在使用 BLE_CONFIG_SCAN_FAST 进行档位配置前已使用
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的 广播参数，且添加的预置广播参数的模式ID为
                                BS_BLE_ADV_MODEID_UNDIR_FAST ，则对应的用户预置广播参数优先生效。@endif */
    BS_BLE_ADV_MODEID_UNDIR_MIDDLE  = BLE_CONFIG_ADV_UNDIR_MIDDLE, /* !< @if Eng custom preset middle gear.
                                If preset advertising parameters have been added by using the
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before using BLE_CONFIG_ADV_UNDIR_MIDDLE to config,
                                and the mode ID of the added preset advertising parameters is
                                BS_BLE_ADV_MODEID_UNDIR_MIDDLE, the preset advertising parameters take effect first.
                            @else 自定义的预置中速档位。如果在使用 BLE_CONFIG_ADV_UNDIR_MIDDLE 进行档位配置前已使用
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的 广播参数，且添加的预置广播参数的模式ID为
                                BS_BLE_ADV_MODEID_UNDIR_MIDDLE ，则对应的用户预置广播参数优先生效。@endif */
    BS_BLE_ADV_MODEID_UNDIR_SLOW    = BLE_CONFIG_ADV_UNDIR_SLOW, /* !< @if Eng custom preset slow gear.
                                If preset advertising parameters have been added by using the
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM before using BLE_CONFIG_ADV_UNDIR_SLOW to config,
                                and the mode ID of the added preset advertising parameters is
                                BS_BLE_ADV_MODEID_UNDIR_SLOW, the preset advertising parameters take effect first.
                            @else 自定义的预置低速档位。如果在使用 BLE_CONFIG_ADV_UNDIR_SLOW 进行档位配置前已使用
                                BLE_CONFIG_ADV_OPR_CUSTOM_PARAM 添加了用户预置的 广播参数，且添加的预置广播参数的模式ID为
                                BS_BLE_ADV_MODEID_UNDIR_SLOW ，则对应的用户预置广播参数优先生效。@endif */
} bs_ble_pre_set_adv_param_mode_id_t;

typedef struct {
    uint16_t mode_id;/*!< @if Eng mode id, usd as unique configuration index. {@ref bs_ble_pre_set_adv_param_mode_id_t}.
                          @else   模式id，用于作为唯一配置索引。{@ref bs_ble_pre_set_adv_param_mode_id_t}
                          @endif */
    bs_ble_adv_param_t para; /*!< @if Eng advertising paramter
                                  @else   广播参数 @endif */
} bs_ble_pre_set_adv_param_t;

typedef enum {
    BS_BLE_PRESET_OPR_TYPE_ADD = 0, /*!< @if Eng the opration for adding a preset parameter. If the record corresponding
                                            to the key (mode_id) already exists, the record will be updated.
                                        @else 该枚举值表示增加一条预置的参数。 若在执行添加操作时，对应key（mode_id）的记录
                                            已经存在， 则会更新对应记录。 @endif */
    BS_BLE_PRESET_OPR_TYPE_RMV = 1, /*!< @if Eng the opration for removing a preset parameter.
                                        @else 该枚举值表示删除一条预置的参数 @endif */
    BS_BLE_PRESET_OPR_TYPE_QRY = 2, /*!< @if Eng the opration for querying a preset parameter.
                                        @else 该枚举值表示查询一条预置的参数 @endif */
    BS_BLE_PRESET_OPR_TYPE_LST = 3, /*!< @if Eng the opration for listing all preset parameter mode ids.
                                        @else 该枚举值表示列出全部预置的参数的模式id @endif */
} bs_ble_preset_opr_type_t;        /*!< @if Eng Operation type of the preset parameter.
                                        @else 预置参数的操作类型 @endif */

#define BS_BLE_MAX_PRE_SET_CONN_PARAM_NUM 5

#define BS_BLE_MAX_PRE_SET_PHY_PARAM_NUM 3

#define BS_BLE_MAX_PRE_SET_SCAN_PARAM_NUM 3

#define BS_BLE_MAX_PRE_SET_ADV_PARAM_NUM 3

typedef struct {
    uint8_t num;                                       /*!< @if Eng Indicates the number of mode ids queried
                                                           @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_BLE_MAX_PRE_SET_CONN_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_ble_pre_set_conn_param_lst_t;

typedef struct {
    uint8_t num;                                      /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_BLE_MAX_PRE_SET_PHY_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_ble_pre_set_phy_param_lst_t;

typedef struct {
    uint8_t num;                                      /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_BLE_MAX_PRE_SET_SCAN_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_ble_pre_set_scan_param_lst_t;

typedef struct {
    uint8_t num;                                      /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_BLE_MAX_PRE_SET_ADV_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_ble_pre_set_adv_param_lst_t;

typedef struct {
    uint16_t mode_id; /*!< @if Eng mode id, usd as unique configuration index
                         @else   模式id，用于作为唯一配置索引 @endif */
} bs_ble_pre_set_param_rmv_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_ble_preset_opr_type_t *}
                    @else 操作类型。{@ref bs_ble_preset_opr_type_t *} @endif */
    union {
        bs_ble_pre_set_conn_param_t in_add_param;    /*!< @if Eng used to enter other input params when operation is ADD
                                                       @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_param_rmv_t in_rmv_param;     /*!< @if Eng used to enter other input params when operation is
                                                            REMOVE
                                                            @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_conn_param_t inout_qry_param; /*!< @if Eng used to enter other input params and receive query
                                                        result when operation is QUERY
                                                        @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                        @endif */
        bs_ble_pre_set_conn_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_ble_opr_pre_set_conn_param_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_ble_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_ble_preset_opr_type_t *} @endif */
    union {
        bs_ble_pre_set_phy_param_t in_add_param;     /*!< @if Eng used to enter other input params when operation is ADD
                                                        @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_param_rmv_t in_rmv_param; /*!< @if Eng used to enter other input params when operation is
                                                        REMOVE
                                                        @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_phy_param_t inout_qry_param;  /*!< @if Eng used to enter other input params and receive query
                                                        result when operation is QUERY
                                                         @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                        @endif */
        bs_ble_pre_set_phy_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_ble_opr_pre_set_phy_param_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_ble_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_ble_preset_opr_type_t *} @endif */
    union {
        bs_ble_pre_set_scan_param_t in_add_param;     /*!< @if Eng used to enter other input params when operation is ADD
                                                        @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_param_rmv_t in_rmv_param; /*!< @if Eng used to enter other input params when operation is
                                                        REMOVE
                                                        @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_scan_param_t inout_qry_param;  /*!< @if Eng used to enter other input params and receive query
                                                        result when operation is QUERY
                                                         @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                        @endif */
        bs_ble_pre_set_scan_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_ble_opr_pre_set_scan_param_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_ble_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_ble_preset_opr_type_t *} @endif */
    union {
        bs_ble_pre_set_adv_param_t in_add_param;     /*!< @if Eng used to enter other input params when operation is ADD
                                                        @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_param_rmv_t in_rmv_param; /*!< @if Eng used to enter other input params when operation is
                                                        REMOVE
                                                        @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_ble_pre_set_adv_param_t inout_qry_param;  /*!< @if Eng used to enter other input params and receive query
                                                        result when operation is QUERY
                                                         @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                        @endif */
        bs_ble_pre_set_adv_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_ble_opr_pre_set_adv_param_t;

typedef struct {
    uint16_t mode_id; /*!< @if Eng mode id, usd as unique configuration index
                         @else   模式id，用于作为唯一配置索引 @endif */
} bs_ble_use_pre_set_scan_param_t;

typedef struct {
    uint8_t adv_id; /*!< @if Eng advertising ID
                         @else   广播id @endif */
    uint16_t mode_id; /*!< @if Eng mode id
                         @else   模式id @endif */
} bs_ble_use_pre_set_adv_param_t;

#ifdef __cplusplus
}
#endif
#endif /* BS_BLE_SERVICE_CONFIG_H */
