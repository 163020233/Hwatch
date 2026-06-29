/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR serivce config enums
 */

#ifndef BS_BR_SERVICE_CONFIG_H
#define BS_BR_SERVICE_CONFIG_H

#include <stdbool.h>
#include "bs_device_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BR_CONFIG_LOCAL    = 0x0000 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BR_CONFIG_INQUIRY  = 0x0100 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BR_CONFIG_CONNECT  = 0x0200 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BR_CONFIG_PROFILE  = 0x0400 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
    BR_CONFIG_PORT_CONNECT  = 0x0500 | (BS_PROTOCOL_BREDR << BS_PROTOCOL_CONFIG_SHIFT_BITS),
} bs_br_config_group_t;

/**
 * @if Eng
 * @brief Bluetooth major device type.
 * @else
 * @brief 蓝牙主设备类型。
 * @endif
 */
typedef enum {
    MAJOR_DEVICE_TYPE_UNKNOWN = 0,            /*!< @if Eng unknown device type.
                                                   @else 未知设备类型。 @endif */
    MAJOR_DEVICE_TYPE_PC = 0x100,             /*!< @if Eng device type of PC.
                                                   @else 电脑设备类型。 @endif */
    MAJOR_DEVICE_TYPE_MOBILE = 0x200,         /*!< @if Eng device type of mobile.
                                                   @else 移动电话设备类型。 @endif */
    MAJOR_DEVICE_TYPE_NETWORK = 0x300,        /*!< @if Eng device type of network.
                                                   @else 网络设备类型。 @endif */
    MAJOR_DEVICE_TYPE_AUDIO = 0x400,          /*!< @if Eng device type of audio.
                                                   @else 音频设备类型。 @endif */
    MAJOR_DEVICE_TYPE_PERIPHERAL = 0x500,     /*!< @if Eng device type of peripheral.
                                                   @else 外设设备类型。 @endif */
    MAJOR_DEVICE_TYPE_IMAGING = 0x600,        /*!< @if Eng device type of imaging.
                                                   @else 图像设备类型。 @endif */
    MAJOR_DEVICE_TYPE_WEARABLE = 0x700,       /*!< @if Eng device type of wearable.
                                                   @else 穿戴设备类型。 @endif */
    MAJOR_DEVICE_TYPE_TOY = 0x800,            /*!< @if Eng device type of toy.
                                                   @else 玩具设备类型。 @endif */
    MAJOR_DEVICE_TYPE_HEALTH = 0x900,         /*!< @if Eng device type of health.
                                                   @else 健康设备类型。 @endif */
    MAJOR_DEVICE_TYPE_UNCATEGORIZED = 0x1F00, /*!< @if Eng device type of uncategorized.
                                                   @else 未分类设备类型。 @endif */
} bs_br_config_major_device_type_t;

/**
 * @if Eng
 * @brief Structure of config inquiry parameter.
 * @else
 * @brief 配置inquiry参数结构体。
 * @endif
 */
typedef struct {
    uint32_t device_class; /*!< @if Eng class of device.
                                @else 设备类型。 @endif */
} bs_br_config_inquiry_param_t;

/**
 * @if Eng
 * @brief Structure of config device class.
 * @else
 * @brief 配置设备类型参数结构体。
 * @endif
 */
typedef struct {
    uint32_t device_class; /*!< @if Eng class of device.
                                @else 设备类型。 @endif */
} bs_br_config_device_class_t;

/**
 * @if Eng
 * @brief Structure of config support profiles and auto connect profiles.
 * @else
 * @brief 配置支持和自动连接的profile参数结构体。
 * @endif
 */
typedef struct {
    bool a2dp_snk : 1; /*!< @if Eng support a2dp sink profile.
                            @else 支持a2dp sink profile。 @endif */
    bool a2dp_src : 1; /*!< @if Eng support a2dp source profile.
                            @else 支持a2dp source profile。 @endif */
    bool avrcp_ct : 1; /*!< @if Eng support avrcp control profile.
                            @else 支持avrcp control profile。 @endif */
    bool avrcp_tg : 1; /*!< @if Eng support avrcp target profile.
                            @else 支持avrcp target profile。 @endif */
    bool hfp_hf : 1;   /*!< @if Eng support hfp hands free profile.
                            @else 支持hfp hands free profile。 @endif */
    bool hfp_ag : 1;   /*!< @if Eng support audio gate free profile.
                            @else 支持hfp audio gate profile。 @endif */
    bool pbap : 1;   /*!< @if Eng support phone book access protocol profile.
                            @else 支持pbap profile。 @endif */
} bs_br_config_support_profile_t, bs_br_config_connect_profile_t;

/**
 * @if Eng
 * @brief Structure of config switch parameter.
 * @else
 * @brief 配置开关参数结构体。
 * @endif
 */
typedef struct {
    bool on_off; /*!< @if Eng switch on/off.
                      @else 开关。 @endif */
} bs_br_config_switch_t;

/**
 * @if Eng
 * @brief Structure of config max connection number.
 * @else
 * @brief 配置最大连接数结构体。
 * @endif
 */
typedef struct {
    uint8_t master_conn_max; /*!< @if Eng max connection of master device.
                                  @else 主设备最大连接数。 @endif */
    uint8_t slave_conn_max; /*!< @if Eng max connection of slave device.
                                  @else 从设备最大连接数。 @endif */
} bs_br_config_connect_max_t;

/**
 * @if Eng
 * @brief Struct of bs config avrcp media attr.
 * @else
 * @brief 配置avrcp媒体属性位图。
 * @endif
 */
typedef struct {
    uint8_t title : 1;                   /*!< @if Eng Title, song name or content description
                                             @else 标题，歌曲名或者内容描述 @endif */
    uint8_t artist_name : 1;             /*!< @if Eng Artist(s), performer(s) or group
                                             @else 艺人、表演者或团体 @endif */
    uint8_t album_name : 1;              /*!< @if Eng Album @else 唱片，专辑 @endif */
    uint8_t track_number : 1;            /*!< @if Eng The order number of the audio-file on its original recording
                                             @else 音频文件的原始编号 @endif */
    uint8_t total_number_of_tracks : 1;  /*!< @if Eng The total number of tracks/elements on the original recording
											 @else tracks/elements的总个数 @endif */
    uint8_t track_genre : 1;             /*!< @if Eng Genre @else 风格，流派 @endif */
    uint8_t track_playing_time : 1;      /*!< @if Eng The length of the audio file in milliseconds
                                             @else 音频文件的播放时间 @endif */
    uint8_t reserved : 1;               /*!< @if Eng Featrues reserved for future. @else 保留字段。 @endif */
} bs_br_config_music_media_attr_t;

/**
 * @if Eng
 * @brief Enum of config parameters type.
 * @else
 * @brief 配置参数类型枚举.
 * @endif
 */
typedef enum {
    BR_CONFIG_LOCAL_ADDR                    = BR_CONFIG_LOCAL | 0x0, /*!< @if Eng config device addr. {@ref bs_addr_t *}
                                                                          @else 配置设备地址 {@ref bs_addr_t *} @endif */
    BR_CONFIG_LOCAL_NAME                    = BR_CONFIG_LOCAL | 0x1, /*!< @if Eng config device name. {@ref char *}
                                                                          @else 配置设备名称 {@ref char *} @endif */
    BR_CONFIG_LOCAL_DEV_CLASS               = BR_CONFIG_LOCAL | 0x2, /*!< @if Eng config device class.
                                                                                  {@ref bs_br_config_device_class_t *}
                                                                          @else 配置设备类型
                                                                                {@ref bs_br_config_device_class_t *}
                                                                          @endif */

    BR_CONFIG_INQUIRY_PARAM                 = BR_CONFIG_INQUIRY | 0x0, /*!< @if Eng config inquiry parameter.
                                                                                  {@ref bs_br_config_inquiry_param_t *}
                                                                          @else 配置设备inquiry参数
                                                                                {@ref bs_br_config_inquiry_param_t *}
                                                                          @endif */

    BR_CONFIG_CONNECT_FILTER_MODE           = BR_CONFIG_CONNECT | 0x0, /*!< @if Eng config filter mode.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置过滤模式，只能连接一个手机、外设
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_UPPER_DECIDE_CONNECT  = BR_CONFIG_CONNECT | 0x1, /*!< @if Eng config upper decides connect.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置上层决定是否接受连接
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_AUTO_RECONNECT        = BR_CONFIG_CONNECT | 0x2, /*!< @if Eng config auto reconnect.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否自动回连
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_RECONNECT_MASTER      = BR_CONFIG_CONNECT | 0x3, /*!< @if Eng config reconnect master device.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否回连主设备
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_RECONNECT_SLAVE       = BR_CONFIG_CONNECT | 0x4, /*!< @if Eng config reconnect slave device.
                                                                                  {@ref bs_br_config_switch_t *}
                                                                          @else 配置是否回连从设备
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_CONNECT_CONN_MAX              = BR_CONFIG_CONNECT | 0x5, /*!< @if Eng config max connection number.
                                                                                  {@ref bs_br_config_connect_max_t *}
                                                                          @else 配置最大连接数
                                                                                {@ref bs_br_config_connect_max_t *}
                                                                          @endif */

    BR_CONFIG_PROFILE_SUPPORT               = BR_CONFIG_PROFILE | 0x0, /*!< @if Eng config supprot prifiles.
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @else 配置支持的profiles
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @endif */
    BR_CONFIG_PROFILE_CONNECT               = BR_CONFIG_PROFILE | 0x1, /*!< @if Eng config auto connect prifiles.
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @else 配置自动连接的profiles
                                                                                {@ref bs_br_config_support_profile_t *}
                                                                          @endif */
    BR_CONFIG_PORT_CONNECT_MODE             = BR_CONFIG_PORT_CONNECT | 0x0, /*!< @if Eng Configure the port server to
                                                                                 automatically reply to the connection.
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @else 配置 port 服务端自动回复连接
                                                                                {@ref bs_br_config_switch_t *}
                                                                          @endif */
    BR_CONFIG_PROFILE_MEDIA_ATTRIBUTE       = BR_CONFIG_PROFILE | 0x2, /*!< @if Eng config media attribute.
                                                                                {@ref bs_br_config_music_media_attr_t *}
                                                                          @else 配置媒体歌词属性信息
                                                                                {@ref bs_br_config_music_media_attr_t *}
                                                                          @endif */
} bs_br_config_type_t;

#ifdef __cplusplus
}
#endif
#endif /* BS_BR_SERVICE_CONFIG_H */