
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Config Structure File.
 */

#ifndef SSF_CONFIG_STRU_H
#define SSF_CONFIG_STRU_H
#include <stdint.h>
#include <stddef.h>
#include "ssf_config.h"
#include "ssf_as_stru.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SSF_MAX_DEVICE_NAME_LEN 64
#define SSF_MAX_VENDOR_DATA_LEN 254 // 字节2~255: 厂商自定义数据内容
#define BS_SLE_MAX_PRE_SET_CONN_PARAM_NUM 5
#define BS_SLE_MAX_PRE_SET_ADV_PARAM_NUM 3

typedef enum {
    SLE_CONFIG_ROLE_CENTRAL = 0x01,
    SLE_CONFIG_ROLE_PERIPHERAL,
} bs_sle_config_role_t;

typedef enum {
    SLE_CONFIG_SUB_ROLE_MASTER,
    SLE_CONFIG_SUB_ROLE_SLAVE,
} bs_sle_config_sub_role_t;

/**
 * @if Eng
 * @brief Enum of config switch parameter.
 * @else
 * @brief 配置开关参数枚举。
 * @endif
 */
typedef enum {
    SLE_CONFIG_DISABLE  = false,
    SLE_CONFIG_ENABLE   = true,
} bs_sle_config_enable_t;

typedef enum {
    SLE_CONFIG_WEARABLE_AUDIO_DEVICE = 0x000800,
    SLE_CONFIG_TWS_EARPHONE = 0x000801,
    SLE_CONFIG_HEADSET_EARPHONE_MIC = 0x000802,
    SLE_CONFIG_HEADSET_EARPHONE = 0x000803,
    SLE_CONFIG_HEADSET_NECKBAND_EARPHONE = 0x000804,

    SLE_CONFIG_ACTION_CAMERA = 0xFF0001, /*!< @if Eng This enumeration value is temporarily used to
                                                    identify the action camera product. It will be
                                                    changed in the future based on SparkLink protocol standard.
                                              @else 这个枚举值是临时用于标识运动相机产品的，将来基于星闪协议标准会更改。@endif */
} bs_sle_config_appearence_t;

typedef enum {
    SLE_CONFIG_FEATURE_INBAND_RING         = 0x01,      /*!< @if Eng ccs enable inband rington
                                                          @else 带内铃声 @endif */
    SLE_CONFIG_FEATURE_STATE_RFU
} bs_sle_config_feature_state_t;

typedef struct {
    uint8_t name_len;
    uint8_t name[SSF_MAX_DEVICE_NAME_LEN];
} bs_sle_config_localhost_name_t;

typedef struct {
    uint16_t vendor_id;
    uint8_t data_len;
    uint8_t data[SSF_MAX_VENDOR_DATA_LEN];
} bs_sle_config_adv_vendor_info_t;

typedef struct {
    uint8_t codec_id; /* 编解码器类型 { @ref bs_sle_audio_stream_codec_id_t } */
    uint16_t vendor_id; /* 厂商标识 { @bs_sle_audio_stream_protocol_vendor_id_t } 此字段在codec_id标识为私有（0XFF）时，需区
        分HW和HISI，若为标准类型(PCM,L2HC)，该值默认为0x0000 */
    uint16_t vendor_codec_id; /* 厂商编码器标识 { @bs_sle_audio_stream_vendor_codec_id_t } 此字段在vendor_id为非默认值时，根
        据具体vendor_id类型，设置编码器标识 */
    union {
        bs_sle_audio_l2hc_codec_capability_config_param_t l2hc;
    } codec_info;
} bs_sle_config_as_codec_capability_param_t;

/**
 * @if Eng
 * @brief codec capability param
 * @else
 * @brief 编解码能力参数
 * @endif
 */
typedef struct {
    uint8_t id;                   /*!< @if Eng codec id. @else 编解码id @endif */
    uint8_t ver;                  /*!< @if Eng codec version. @else 编解码版本 @endif */
    uint16_t samp_freq;           /*!< @if Eng the bitmap of sample frequency. @else 采样率位图 @endif */
    uint8_t samp_dep;             /*!< @if Eng the bitmap of sample depth. @else 采样深度位图 @endif */
    uint16_t channel;             /*!< @if Eng the bitmap of channel. @else 通道位图 @endif */
    uint8_t frame_len_mode;       /*!< @if Eng the bitmap of frame length mode. @else 帧长模式位图 @endif */
    uint8_t coding_rate[AS_CODING_RATE_CONFIG_LENGTH]; /*!< @if Eng the bitmap of coding rate.
                                                            @else 编码速率位图 @endif */
} bs_sle_config_as_codec_cap_param_t;

/**
 * @if Eng
 * @brief when get the config of codec capability, the output result
 * @else
 * @brief 查询编解码能力配置时，返回的编解码能力集,需要指定codec_count
 * @endif
 */
typedef struct {
    uint16_t codec_count;                          /*!< @if Eng codec count. @else 待查询编解码能力个数 @endif */
    bs_sle_config_as_codec_cap_param_t capability[0]; /*!< @if Eng the array of codec capability.
                                                        @else 编解码能力数组 @endif */
} bs_sle_config_as_codec_capability_set_t;

typedef struct {
    uint16_t interval_min;        /*!< @if Eng min interval,the value range is 6-3200,the unit is 1.25ms
                                       @else   最小间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t interval_max;        /*!< @if Eng max interval,the value range is 6-3200,the unit is 1.25ms
                                       @else   最大间隔,值范围6-3200,单位为1.25ms @endif */
    uint16_t max_latency;         /*!< @if Eng slave reply min latency,the value range is 0-499
                                       @else   从设备回复最小间隔，值范围0-499 @endif */
    uint16_t supervision_timeout; /*!< @if Eng interval for disconnection due to timeout,
                                              the value range is 10-3200,the unit is 10ms
                                       @else   超时断连间隔，值范围10-3200，单位为10ms @endif */
} bs_sle_conn_param_t;

typedef struct {
    uint16_t mode_id;         /*!< @if Eng mode id, usd as unique configuration index. The value cannot be 0.
                                         @else   模式id，用于作为唯一配置索引，值不能等于0 @endif */
    bs_sle_conn_param_t para; /*!< @if Eng connection paramter
                                  @else   连接参数 @endif */
} bs_sle_pre_set_conn_param_t;

typedef struct {
    uint8_t num;                                       /*!< @if Eng Indicates the number of mode ids queried
                                                           @else 查询到的mode id的数量 @endif */
    uint16_t params[BS_SLE_MAX_PRE_SET_CONN_PARAM_NUM]; /*!< @if Eng Indicates the list of mode ids queried
                                                         @else 查询到的mode id @endif */
} bs_sle_pre_set_conn_param_lst_t;

typedef struct {
    uint8_t type;                                    /*!< @if Eng operation type. {@ref bs_sle_preset_opr_type_t *}
                                                          @else 操作类型。{@ref bs_sle_preset_opr_type_t *} @endif */
    union {
        bs_sle_pre_set_conn_param_t in_add_param;    /*!< @if Eng used to enter other input params when operation is ADD
                                                          @else 操作类型为添加时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_param_rmv_t in_rmv_param;     /*!< @if Eng used to enter other input params when operation is
                                                              REMOVE
                                                          @else 操作类型为删除时，使用该成员变量输入其他入参 @endif */
        bs_sle_pre_set_conn_param_t inout_qry_param; /*!< @if Eng used to enter other input params and receive query
                                                              result when operation is QUERY
                                                          @else 操作类型为查询时，使用该成员变量输入其他入参并接收查询结果
                                                          @endif */
        bs_sle_pre_set_conn_param_lst_t out_lst_param; /*!< @if Eng used to receive list result when operation is LIST
                                                            @else 操作类型为LIST时，使用该成员变量接收查询结果 @endif */
    };
} bs_sle_opr_pre_set_conn_param_t;

#ifdef __cplusplus
}
#endif
#endif /* SSF_CONFIG_STRU_H */