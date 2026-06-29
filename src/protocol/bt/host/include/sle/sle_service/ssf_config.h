
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Config API File.
 */

#ifndef SSF_CONFIG_H
#define SSF_CONFIG_H
#include <stdint.h>
#include <stddef.h>
#include "errcode.h"
#include "bs_device_mgr.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BS_PROTOCOL_CONFIG_SHIFT_BITS 13
#define BS_SLE_MAX_PRE_SET_PHY_PARAM_NUM 3

typedef enum {
    SLE_CONFIG_COMMON = 0x0000 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_ADV = 0x0100 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_SCAN = 0x0200 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_AS = 0x0300 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_VOLUME = 0x0400 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_CALL = 0x0500 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_AUTO_CONN = 0x0600 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_CONN = 0x0700 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    SLE_CONFIG_PORT = 0x0800 | (BS_PROTOCOL_SLE << BS_PROTOCOL_CONFIG_SHIFT_BITS),
} bs_sle_config_group_t;

typedef enum {
    SLE_CONFIG_COMMON_ROLE = SLE_CONFIG_COMMON | 0x01,      /* bs_sle_config_role_t */
    SLE_CONFIG_COMMON_PEER_ADDR = SLE_CONFIG_COMMON | 0x02, /* 6 bytes */
    SLE_CONFIG_COMMON_SUB_ROLE = SLE_CONFIG_COMMON | 0x03,  /* bs_sle_config_sub_role_t */
    SLE_CONFIG_COMMON_LOCAL_ADDR = SLE_CONFIG_COMMON | 0x04, /* sle_config_local_addr_t */

    SLE_CONFIG_ADV_SHORT_NAME = SLE_CONFIG_ADV | 0x01,   /* uint8_t */
    SLE_CONFIG_ADV_FULL_NAME = SLE_CONFIG_ADV | 0x02,    /* uint8_t */
    SLE_CONFIG_ADV_VENDOR = SLE_CONFIG_ADV | 0x03,       /* uint8_t */
    SLE_CONFIG_ADV_APPEARANCE = SLE_CONFIG_ADV | 0x04,   /* bs_sle_config_appearence_t */
    SLE_CONFIG_ADV_PRESET_PARAM = SLE_CONFIG_ADV | 0x05, /* !< @if Eng Add preset advertising parameters
                                                          {@ref bs_sle_opr_pre_set_adv_param_t *}
                                                      @else 添加用户预制广播参数
                                                          {@ref bs_sle_opr_pre_set_adv_param_t *}
                                                      @endif */
    SLE_CONFIG_ADV_DISCOVERING_SPEED = SLE_CONFIG_ADV | 0x06, /* !< @if Eng Use the preset parameters for
                                device advertising. Type is uint16_t.
                                The input value 0 indicates that preset parameters are not used.
                                Other input value must in range of {@ref bs_sle_preset_adv_speed_mode_id_t *}.
                                If the preset parameter gear level (mode_id) is specified by this configuration but the
                                corresponding preset parameter is not be added, the default parameter will take effect
                            @else 使用用户预置的广播参数作为广播参数，参数数据类型为uint16_t。
                                取值为0时表示不使用预置参数。
                                其他有效值应满足枚举值范围 {@ref bs_sle_preset_adv_speed_mode_id_t *}
                                若使用该配置项指定了预置参数档位（即mode_id）但对应的预置参数并未添加，则在使用时默认参数生效。
                            @endif */
    SLE_CONFIG_ADV_CDSM_KEY_ID = SLE_CONFIG_ADV | 0x07,     /* uint8_t */
    SLE_CONFIG_ADV_CDSM_SECRET_KEY = SLE_CONFIG_ADV | 0x08, /* 16byte (uint8_t)*16 */

    SLE_CONFIG_SCAN_FILTER_DUPLICATE = SLE_CONFIG_SCAN | 0x01,
    SLE_CONFIG_SCAN_SEEK_FILTER_POLICY = SLE_CONFIG_SCAN | 0x02,
    SLE_CONFIG_SCAN_SEEK_TYPE = SLE_CONFIG_SCAN | 0x03,
    SLE_CONFIG_SCAN_SEEK_PRESET_PARAM = SLE_CONFIG_SCAN | 0x04, /* !< @if Eng Add preset scan parameters
                                                              {@ref bs_sle_opr_pre_set_seek_param_t *}
                                                          @else 添加用户预制扫描参数
                                                              {@ref bs_sle_opr_pre_set_seek_param_t *}
                                                          @endif */
    SLE_CONFIG_SCAN_SEEK_SPEED = SLE_CONFIG_SCAN | 0x05, /* !< @if Eng Use the preset parameters for

                                device discovery. Type is uint16_t.
                                The input value 0 indicates that preset parameters are not used.
                                Other input value must in range of {@ref bs_sle_preset_seek_speed_mode_id_t *}
                                If the preset parameter gear level (mode_id) is specified by this configuration but the
                                corresponding preset parameter is not be added, the default parameter will take effect
                            @else 使用用户预置的扫描参数作为扫描参数，参数数据类型为uint16_t。
                                取值为0时表示不使用预置参数。
                                其他有效值应满足枚举值范围  {@ref bs_sle_preset_seek_speed_mode_id_t *}
                                若使用该配置项指定了预置参数档位（即mode_id）但对应的预置参数并未添加，则在使用时默认参数生效。
                            @endif */
    SLE_CONFIG_SCAN_MAX_CACHE_RESULT = SLE_CONFIG_SCAN | 0x06,  /* uint8_t */

    SLE_CONFIG_AS_CODEC_CAPABILITY = SLE_CONFIG_AS | 0x01, /*!< @if Eng the config of codec capability.
                                                                @else 编解码能力配置，查询编解码能力配置时，
                                                                      需要先查询编解码能力个数，出参结构体指针
                                                                      {bs_sle_config_as_codec_capability_set_t}
                                                                @endif */
    SLE_CONFIG_AS_LOCATION_BITMAP = SLE_CONFIG_AS | 0x02,
    SLE_CONFIG_AS_CODEC_CAPABILITY_COUNT = SLE_CONFIG_AS | 0x03, /*!< @if Eng the count of codec capability.
                                                                      @else 编解码能力个数，查询编解码能力配置时，
                                                                            需要先查询编解码能力个数 @endif */

    SLE_CONFIG_VOLUME_INITAL_CALL_VOLUME = SLE_CONFIG_VOLUME | 0x01,
    SLE_CONFIG_VOLUME_INITAL_MEDIA_VOLUME = SLE_CONFIG_VOLUME | 0x02,

    SLE_CONFIG_CALL_FEATURE_STATE = SLE_CONFIG_CALL | 0X01, /* bs_sle_config_feature_state_t */

    SLE_CONFIG_AUTO_CONN_BROADCAST_DURATION = SLE_CONFIG_AUTO_CONN | 0X01,

    SLE_CONFIG_CONNECT_PRESET_PARAM = SLE_CONFIG_CONN | 0x1, /* !< @if Eng config pre-set connection params. {@ref bs_sle_opr_pre_set_conn_param_t *}
                                                                   @else 配置预定义的连接参数 {@ref bs_sle_opr_pre_set_conn_param_t *} @endif */
    SLE_CONFIG_CONNECT_SET_DEFAULT_PARAM = SLE_CONFIG_CONN | 0x2, /* mode_id, uint16_t * */
    SLE_CONFIG_CONNECT_PRESET_PHY    = SLE_CONFIG_CONN | 0x3, /* !< @if Eng config pre-set PHY. {@ref bs_sle_opr_pre_set_phy_param_t *}
                                                                    @else 配置预定义的连接参数 {@ref bs_sle_opr_pre_set_phy_param_t *} @endif */
    SLE_CONFIG_CONNECT_NOT_AUTO_PAIR = SLE_CONFIG_CONN | 0x4,  /*!< @if Eng This parameter is used to configure
                                                                    whether to disable automatic pair after the
                                                                    connection is complete.{@ref bs_sle_config_enable_t}
                                                                @else 用于配置在完成连接后是否不自动进行配对。
                                                                    {@ref bs_sle_config_enable_t}
                                                                @endif */
    SLE_CONFIG_CONNECT_IS_WILLING_TO_DISCONNECT_ALL = SLE_CONFIG_CONN | 0x5, /* bool */
    SLE_CONFIG_CONNECT_ENABLE_PREEMPTION = SLE_CONFIG_CONN | 0x6, /* bool 不支持动态配置，需在sle_enable前配置完毕 */

    SLE_CONFIG_PORT_MAX_PORT = SLE_CONFIG_PORT | 0x01,               /* uint8_t */
    SLE_CONFIG_PORT_MAX_CHANNEL = SLE_CONFIG_PORT | 0x02,            /* uint8_t */
    SLE_CONFIG_PORT_MAX_CHANNEL_PER_PORT = SLE_CONFIG_PORT | 0x03,   /* uint8_t */
    SLE_CONFIG_TYPE_MAX,
} bs_sle_config_type_t;

typedef enum {
    SLE_CONFIG_TEMPLATE_TYPE_HEADSET,
    SLE_CONFIG_TEMPLATE_TYPE_PHONE,
    SLE_CONFIG_TEMPLATE_TYPE_MAX,
} bs_sle_config_template_type_t;

typedef enum {
    BS_SLE_PRESET_OPR_TYPE_ADD = 0, /*!< @if Eng the opration for adding a preset parameter.
                                        @else 该枚举值表示增加一条预置的参数 @endif */
    BS_SLE_PRESET_OPR_TYPE_RMV = 1, /*!< @if Eng the opration for removing a preset parameter.
                                        @else 该枚举值表示删除一条预置的参数 @endif */
    BS_SLE_PRESET_OPR_TYPE_QRY = 2, /*!< @if Eng the opration for querying a preset parameter.
                                        @else 该枚举值表示查询一条预置的参数 @endif */
    BS_SLE_PRESET_OPR_TYPE_LST = 3, /*!< @if Eng the opration for listing all preset parameter mode ids.
                                        @else 该枚举值表示列出全部预置的参数的模式id @endif */
} bs_sle_preset_opr_type_t;        /*!< @if Eng Operation type of the preset parameter.
                                        @else 预置参数的操作类型 @endif */

typedef enum {
    BS_SLE_PRESET_SEEK_SPEED_FAST = 1,   /*!< @if Eng fast-speed gear of preset parameters.
                                          @else 预置参数的高速档位 @endif */
    BS_SLE_PRESET_SEEK_SPEED_MIDDLE = 2, /*!< @if Eng middle-speed gear of preset parameters.
                                        @else 预置参数的中速档位 @endif */
    BS_SLE_PRESET_SEEK_SPEED_SLOW = 3,   /*!< @if Eng slow-speed gear of preset parameters.
                                          @else 预置参数的低速档位 @endif */
} bs_sle_preset_seek_speed_mode_id_t;

/**
 * @if Eng
 * @brief  Seek filter type.
 * @else
 * @brief  设备发现过滤类型。
 * @endif
 */
typedef enum {
    BS_SLE_SEEK_FILTER_ALLOW_ALL   = 0x00, /*!< @if Eng allow all
                                                       @else   允许来自任何人的设备发现数据包 @endif */
    BS_SLE_SEEK_FILTER_ALLOW_WLST  = 0x01, /*!< @if Eng allow only white list, reserve
                                                       @else   允许来自白名单设备的设备发现数据包，预留 @endif */
} bs_sle_seek_filter_t;

/**
 * @if Eng
 * @brief  Seek phy type.
 * @else
 * @brief  设备发现PHY类型。
 * @endif
 */
typedef enum {
    BS_SLE_SEEK_PHY_1M = 0x1, /*!< @if Eng 1M PHY
                                     @else   1M PHY @endif */
    BS_SLE_SEEK_PHY_2M = 0x2, /*!< @if Eng 2M PHY
                                     @else   2M PHY @endif */
    BS_SLE_SEEK_PHY_4M = 0x4, /*!< @if Eng 4M PHY
                                     @else   4M PHY @endif */
} bs_sle_seek_phy_t;

/**
 * @if Eng
 * @brief  seek type.
 * @else
 * @brief  设备发现类型。
 * @endif
 */
typedef enum {
    BS_SLE_SEEK_PASSIVE = 0x00, /*!< @if Eng passive seek
                                            @else   被动扫描 @endif */
    BS_SLE_SEEK_ACTIVE  = 0x01, /*!< @if Eng active seek
                                            @else   主动扫描 @endif */
} bs_sle_seek_type_t;

/**
 * @if Eng
 * @brief  Seek scan parameter.
 * @else
 * @brief  设备发现扫描参数。
 * @endif
 */
typedef struct bs_sle_seek_param {
    uint8_t seek_filter_policy; /*!< @if Eng scan filter policy { @ref bs_sle_seek_filter_t }
                                     @else   扫描设备使用的过滤类型，
                                             { @ref bs_sle_seek_filter_t } @endif */
    uint8_t seek_phy;           /*!< @if Eng scan PHY type { @ref bs_sle_seek_phy_t }
                                      @else   扫描设备所使用的PHY，{ @ref bs_sle_seek_phy_t }
                                      @endif */
    uint8_t seek_type;          /*!< @if Eng scan type { @ref bs_sle_seek_type_t }
                                                           @else   扫描类型，{ @ref bs_sle_seek_type_t }
                                                           @endif */
    uint16_t seek_interval;     /*!< @if Eng scan interval
                                                           @else   扫描间隔，取值范围[0x0014, 0xFFFF]，time = N * 0.125ms
                                                           @endif */
    uint16_t seek_window;       /*!< @if Eng scan window
                                                           @else   扫描窗口，取值范围[0x0014, 0xFFFF]，time = N * 0.125ms
                                                           @endif */
} bs_sle_seek_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index.
                                            The value must in range of {@ref bs_sle_preset_seek_speed_mode_id_t *}.
                                         @else   模式id，用于作为唯一配置索引。
                                            取值范围参考 {@ref bs_sle_preset_seek_speed_mode_id_t *} @endif */
    bs_sle_seek_param_t para; /*!< @if Eng seek paramter
                                  @else   扫描参数 @endif */
} bs_sle_pre_set_seek_param_t;

#define BS_SLE_MAX_PRE_SET_SEEK_PARAM_NUM 3

typedef struct {
    uint8_t num;                                        /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_SLE_MAX_PRE_SET_SEEK_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_sle_pre_set_seek_param_lst_t;

typedef struct {
    uint16_t mode_id; /*!< @if Eng mode id, usd as unique configuration index
                         @else   模式id，用于作为唯一配置索引 @endif */
} bs_sle_pre_set_param_rmv_t;

typedef enum {
    BS_SLE_PRESET_ADV_SPEED_FAST = 1, /*!< @if Eng fast-speed gear of preset parameters.
                                        @else 预置参数的高速档位 @endif */
    BS_SLE_PRESET_ADV_SPEED_MIDDLE = 2, /*!< @if Eng middle-speed gear of preset parameters.
                                        @else 预置参数的中速档位 @endif */
    BS_SLE_PRESET_ADV_SPEED_SLOW = 3, /*!< @if Eng slow-speed gear of preset parameters.
                                        @else 预置参数的低速档位 @endif */
} bs_sle_preset_adv_speed_mode_id_t;


/**
 * @if Eng
 * @brief  announce mode.
 * @else
 * @brief  设备公开类型。
 * @endif
 */
typedef enum {
    BS_SLE_ANNOUNCE_MODE_NONCONN_NONSCAN                       = 0x00, /*!< @if Eng non-connectable, non-scannable.
                                                                         @else   不可连接不可扫描。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_NONSCAN                   = 0x01, /*!< @if Eng connectable, non-scannable.
                                                                         @else   可连接不可扫描。 @endif */
    BS_SLE_ANNOUNCE_MODE_NONCONN_SCANABLE                      = 0x02, /*!< @if Eng non-connectable, scannable.
                                                                         @else   不可连接可扫描。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE                  = 0x03, /*!< @if Eng connectable, scannable.
                                                                         @else   可连接可扫描。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_NONSCAN_DIRECTED          = 0x05, /*!< @if Eng connectable, non-scannable,directed.
                                                                         @else   可连接不可扫描定向。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_DIRECTED                  = 0x07, /*!< @if Eng connectable, scannable, directed.
                                                                         @else   可连接可扫描定向。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_NONSCAN_IGNORE_WHITELIST  = 0x09, /*!< @if Eng connectable, non-scannable,
                                                                                 ignore whitelist.
                                                                         @else   可连接不可扫描，忽略白名单。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_IGNORE_WHITELIST          = 0x0B, /*!< @if Eng connectable, scannable,
                                                                                 ignore whitelist.
                                                                         @else   可连接可扫描，忽略白名单。 @endif */
    BS_SLE_ANNOUNCE_MODE_CONNECTABLE_DIRECTED_IGNORE_WHITELIST = 0x0F, /*!< @if Eng connectable, scannable, directed,
                                                                                 ignore whitelist.
                                                                         @else   可连接可扫描定向，忽略白名单。 @endif */
} bs_sle_announce_mode_t;

/**
 * @if Eng
 * @brief  announce level.
 * @else
 * @brief  被发现方可发现等级
 * @endif
 */
typedef enum {
    BS_SLE_ANNOUNCE_LEVEL_NONE      = 0,    /*!< @if Eng announce level none, reserve
                                                @else   不可见发现，预留 @endif */
    BS_SLE_ANNOUNCE_LEVEL_NORMAL    = 1,    /*!< @if Eng announce level normal
                                                @else   一般可发现 @endif */
    BS_SLE_ANNOUNCE_LEVEL_PRIORITY  = 2,    /*!< @if Eng announce level priority, reserve
                                                @else   优先可发现，预留 @endif */
    BS_SLE_ANNOUNCE_LEVEL_PAIRED    = 3,    /*!< @if Eng announce level paired, reserve
                                                @else   被曾配对过的设备发现，预留 @endif */
    BS_SLE_ANNOUNCE_LEVEL_SPECIAL   = 4,    /*!< @if Eng announce level special
                                                @else   被指定设备发现 @endif */
} bs_sle_announce_level_t;

typedef struct {
    uint8_t  announce_mode;                 /*!< @if Eng announce mode { @ref bs_sle_announce_mode_t }
                                                 @else   设备公开类型， { @ref bs_sle_announce_mode_t } @endif */
    uint8_t  announce_level;                /*!< @if Eng announce level
                                                         { @ref bs_sle_announce_level_t }
                                                 @else   发现等级，
                                                         { @ref bs_sle_announce_level_t } @endif */
    uint32_t announce_interval_min;         /*!< @if Eng minimum of announce interval
                                                 @else   最小设备公开周期, 0x000020~0xffffff, 单位125us @endif */
    uint32_t announce_interval_max;         /*!< @if Eng maximum of announce interval
                                                 @else   最大设备公开周期, 0x000020~0xffffff, 单位125us @endif */
    uint8_t  announce_channel_map;          /*!< @if Eng announce channel map
                                                 @else   设备公开信道, bit0:ch77, bit1:ch78, bit2:ch79 @endif */
    int8_t   announce_tx_power;             /*!< @if Eng adv transmit power
                                                 @else   广播发射功率，单位dbm, 取值范围[-127, 20],
                                                         0x7F：不设置特定发送功率 @endif */
    uint16_t conn_interval_min;             /*!< @if Eng minimum of connection interval
                                                 @else   连接间隔最小取值，取值范围[0x001E,0x3E80]，
                                                         announce_gt_role 为 SLE_ANNOUNCE_ROLE_T_NO_NEGO
                                                         时无需配置 @endif */
    uint16_t conn_interval_max;             /*!< @if Eng maximum of connection interval
                                                 @else   连接间隔最大取值，取值范围[0x001E,0x3E80]，
                                                         announce_gt_role 为 SLE_ANNOUNCE_ROLE_T_NO_NEGO
                                                         无需配置 @endif */
    uint16_t conn_max_latency;              /*!< @if Eng max connection latency
                                                 @else   最大休眠连接间隔，取值范围[0x0000,0x01F3]，
                                                         announce_gt_role 为 SLE_ANNOUNCE_ROLE_T_NO_NEGO
                                                         无需配置 @endif */
    uint16_t conn_supervision_timeout;      /*!< @if Eng connect supervision timeout
                                                 @else   最大超时时间，取值范围[0x000A,0x0C80]，
                                                         announce_gt_role 为 SLE_ANNOUNCE_ROLE_T_NO_NEGO
                                                         无需配置 @endif */
} bs_sle_adv_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index.
                                            The value must in range of {@ref bs_sle_preset_adv_speed_mode_id_t *}.
                                         @else   模式id，用于作为唯一配置索引。
                                            取值范围参考 {@ref bs_sle_preset_adv_speed_mode_id_t *} @endif */
    bs_sle_adv_param_t para; /*!< @if Eng announce paramter
                                  @else   广播参数 @endif */
} bs_sle_pre_set_adv_param_t;

#define BS_SLE_MAX_PRE_SET_ADV_PARAM_NUM 3

typedef struct {
    uint8_t num;                                        /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_SLE_MAX_PRE_SET_ADV_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_sle_pre_set_adv_param_lst_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_sle_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_sle_preset_opr_type_t *} @endif */
    union {
        bs_sle_pre_set_seek_param_t in_add_param; /*!< @if Eng used to enter other input params when operation is ADD
                                                      @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_param_rmv_t in_rmv_param;  /*!< @if Eng used to enter other input params when operation is REMOVE
                                                         @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_seek_param_t inout_qry_param;   /*!< @if Eng used to enter other input params and receive query
                                                         result when operation is QUERY
                                                          @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                     @endif */
        bs_sle_pre_set_seek_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_sle_opr_pre_set_seek_param_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_sle_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_sle_preset_opr_type_t *} @endif */
    union {
        bs_sle_pre_set_adv_param_t in_add_param;      /*!< @if Eng used to enter other input params when operation is
                                                              ADD
                                                           @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_param_rmv_t in_rmv_param;      /*!< @if Eng used to enter other input params when operation is
                                                             REMOVE
                                                             @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_adv_param_t inout_qry_param;   /*!< @if Eng used to enter other input params and receive query
                                                         result when operation is QUERY
                                                          @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                         @endif */
        bs_sle_pre_set_adv_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is
                                                            LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_sle_opr_pre_set_adv_param_t;

/**
 * @brief 星闪无线帧类型，bitmap表示
 */
typedef enum {
    BS_SLE_RADIO_FRAME_1    = 0,      /*!< @if Eng Radio frame type 1.
                                             @else 无线帧类型1。 @endif */
    BS_SLE_RADIO_FRAME_2    = 1,      /*!< @if Eng Eng Radio frame type 2.
                                             @else 无线帧类型2。 @endif */
    BS_SLE_RADIO_FRAME_3_M0 = 2,      /*!< @if Eng Radio frame type 3 and m sequence 0.
                                             @else 无线帧类型3，m序列0。 @endif */
    BS_SLE_RADIO_FRAME_3_M1 = 3,      /*!< @if Eng Radio frame type 3 and m sequence 1.
                                             @else 无线帧类型3，m序列1。 @endif */
    BS_SLE_RADIO_FRAME_3_M2 = 4,      /*!< @if Eng Radio frame type 3 and m sequence 2.
                                             @else 无线帧类型3，m序列2。 @endif */
    BS_SLE_RADIO_FRAME_3_M3 = 5,      /*!< @if Eng Radio frame type 3 and m sequence 3.
                                             @else 无线帧类型3，m序列3。 @endif */
    BS_SLE_RADIO_FRAME_3_M4 = 6,      /*!< @if Eng Radio frame type 3 and m sequence 4.
                                             @else 无线帧类型3，m序列4。 @endif */
    BS_SLE_RADIO_FRAME_3_M5 = 7,      /*!< @if Eng Radio frame type 3 and m sequence 5.
                                             @else 无线帧类型3，m序列5。 @endif */
    BS_SLE_RADIO_FRAME_4_M0 = 8,      /*!< @if Eng Radio frame type 4 and m sequence 0.
                                             @else 无线帧类型4，m序列0 @endif */
    BS_SLE_RADIO_FRAME_4_M1 = 9,      /*!< @if Eng Radio frame type 4 and m sequence 1.
                                             @else 无线帧类型4，m序列1 @endif */
    BS_SLE_RADIO_FRAME_4_M2 = 10,     /*!< @if Eng Radio frame type 4 and m sequence 2.
                                             @else 无线帧类型4，m序列2。 @endif */
    BS_SLE_RADIO_FRAME_4_M3 = 11,     /*!< @if Eng Radio frame type 4 and m sequence 3.
                                             @else 无线帧类型4，m序列3。 @endif */
    BS_SLE_RADIO_FRAME_4_M4 = 12,     /*!< @if Eng Radio frame type 4 and m sequence 4.
                                             @else 无线帧类型4，m序列4。 @endif */
    BS_SLE_RADIO_FRAME_4_M5 = 13,     /*!< @if Eng Radio frame type 4 and m sequence 5.
                                             @else 无线帧类型4，m序列5。 @endif */
    BS_SLE_RADIO_FRAME_END
} bs_sle_radio_frame_t;

/**
 * @brief 星闪发送/接收PHY类型，bitmap表示
 */
typedef enum {
    BS_SLE_PHY_1M = 0x0,          /*!< @if Eng 1M PHY
                                         @else 1M PHY @endif */
    BS_SLE_PHY_2M = 0x1,          /*!< @if Eng 2M PHY
                                         @else 2M PHY @endif */
    BS_SLE_PHY_4M = 0x2,          /*!< @if Eng 4M PHY
                                         @else 4M PHY @endif */
    BS_SLE_PHY_SUPPORT_NUM,
} bs_sle_phy_tx_rx_t;

/**
 * @brief 星闪发送/接收导频密度指示，bitmap表示
 */
typedef enum {
    BS_SLE_PHY_PILOT_DENSITY_4_TO_1  = 0x0,    /*!< @if Eng The pilot density is 4:1.
                                             @else 导频密度为4:1 @endif */
    BS_SLE_PHY_PILOT_DENSITY_8_TO_1  = 0x1,    /*!< @if Eng The pilot density is 8:1.
                                             @else 导频密度为8:1 @endif */
    BS_SLE_PHY_PILOT_DENSITY_16_TO_1 = 0x2,    /*!< @if Eng The pilot density is 16:1.
                                             @else 导频密度为16:1 @endif */
    BS_SLE_PHY_PILOT_DENSITY_NO = 0x3,         /*!< @if Eng No pilot.
                                             @else 无导频 @endif */
    BS_SLE_PHY_PILOT_DENSITY_NUM,
} bs_sle_phy_tx_rx_pilot_density_t;

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
} bs_sle_phy_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index. The value cannot be 0.
                                 @else   模式id，用于作为唯一配置索引，值不能等于0 @endif */
    bs_sle_phy_param_t para; /*!< @if Eng PHY paramter
                                  @else   PHY参数 @endif */
} bs_sle_pre_set_phy_param_t;

typedef struct {
    uint8_t num;                                      /*!< @if Eng Indicates the number of mode ids queried
                                                          @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_SLE_MAX_PRE_SET_PHY_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_sle_pre_set_phy_param_lst_t;

typedef struct {
    uint8_t type; /*!< @if Eng operation type. {@ref bs_sle_preset_opr_type_t *}
                   @else 操作类型。{@ref bs_sle_preset_opr_type_t *} @endif */
    union {
        bs_sle_pre_set_phy_param_t in_add_param;     /*!< @if Eng used to enter other input params when operation is ADD
                                                        @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_param_rmv_t in_rmv_param; /*!< @if Eng used to enter other input params when operation is
                                                        REMOVE
                                                        @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_phy_param_t inout_qry_param;  /*!< @if Eng used to enter other input params and receive query
                                                        result when operation is QUERY
                                                         @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                        @endif */
        bs_sle_pre_set_phy_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_sle_opr_pre_set_phy_param_t;

/**
 * @if Eng
 * @brief Config the default configuration of TLK.
 * @par   This function allows users to modify the default configuration of TLK by specifying the configuration
 * command and sub-item.
 * @attention NULL
 * @param  [in] type Configuration item, detailed in bs_sle_config_type_t. The first 3 bits represent the protocol type,
 * the next 5 bits represent the module type, and the last 8 bits represent the specific configuration item.
 * @param  [in] value Configuration value, the type depends on the specific configuration item.
 * @param  [in] len Length of the configuration value.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief 星闪配置函数
 * @par  该函数用于调整星闪的默认配置，通过指定配置命令和子项来修改配置。
 * @attention 无
 * @param  [in] type 配置项，详情见bs_sle_config_type_t，前3bit代表协议类型，其后5bit表示模块类型，最后8bit代表具体配置项。
 * @param  [in] value 配置值，具体类型根据配置项而定。
 * @param  [in] len 配置值，配置项大小。
 * @retval 执行结果错误码，表示操作是否成功。
 * @endif
 */
errcode_t bs_sle_config(uint16_t type, void *param, uint16_t len);

/**
 * @if Eng
 * @brief Query the current configuration of TLK.
 * @par   This function allows users to retrieve the current configuration of TLK by specifying the configuration
 * command and sub-item.
 * @attention NULL
 * @param  [in] type Configuration item, detailed in bs_sle_config_type_t. The first 3 bits represent the protocol type,
 * the next 5 bits represent the module type, and the last 8 bits represent the specific configuration item.
 * @param  [out] value Pointer to a variable where the queried configuration value will be stored.
 * @param  [out] len Length of the configuration value.
 * @retval error code, indicating whether the operation was successful.
 * @else
 * @brief 查询配置函数
 * @par  该函数用于查询星闪的当前配置，通过指定配置命令和子项来获取配置值。
 * @attention 无
 * @param  [in] type 配置项，详情见bs_sle_config_type_t，前3bit代表协议类型，其后5bit表示模块类型，最后8bit代表具体配置项。
 * @param  [out] value 配置项目前的值为多少
 * @param  [out] len 配置项的大小。
 * @retval 执行结果错误码，表示操作是否成功。
 * @endif
 */
errcode_t bs_sle_get_config(uint16_t type, void *param, uint16_t *len);

/**
 * @if Eng
 * @brief Configure the specified template's configuration based on the type.
 * @par This function allows users to configure the specified template's configuration by specifying the configuration
 * type.
 * @attention None
 * @param  [in] type Configuration type, details in bs_sle_config_template_type_t.
 * @retval Error code, indicating whether the operation was successful.
 * @else
 * @brief 配置模板函数
 * @par  该函数用于根据指定的配置项配置对应的模板配置。
 * @attention 无
 * @param  [in] type 配置项，详情见bs_sle_config_template_type_t。
 * @retval 执行结果错误码，表示操作是否成功。
 * @endif
 */
errcode_t bs_sle_config_template(uint16_t type);

#ifdef __cplusplus
}
#endif
#endif /* SSF_CONFIG_H */