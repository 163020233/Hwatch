/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_stream.h
 * Description:  provide ux audio stream header file.
 */

#ifndef UX_AUDIO_STREAM_H
#define UX_AUDIO_STREAM_H

#include <stdbool.h>
#include "audio_types.h"
#include "ux_common_define.h"
#include "ux_api_audio_common.h"
#include "ux_api_audio_stream.h"
#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UX_MAX_CONNECT_DEVICE_TYPE 4
#define UX_BG_DEFAULT_LINEAR_VOLUME 80

typedef struct tag_audio_stream_params_t {
    uint8_t addr[UX_BG_ADDR_LEN];
    uint16_t device_id;
    uint16_t connect_id;
    ux_api_audio_stream_type_t stream_type;    /* 音频流类型 */
    ux_api_audio_stream_direction_t direction; /* 音频流方向 */
} ux_audio_stream_params_t;

typedef struct tag_audio_stream_config_t {
    codec_format_t codec_format;                /* 音频编码格式 */
    ux_api_audio_stream_type_t stream_type;     /* 音频流类型 */
    ux_api_audio_stream_direction_t direction;  /* 音频流方向 */
    audio_stream_l2hc_protocol_t protocol;      /* L2HC编解码的协议类型 */
    audio_stream_vendor_id_t vendor_id;         /* L2HC编解码厂商ID */
    audio_stream_l2hc_version_t version;        /* L2HC编解码版本 */
    audio_stream_aggregation_t aggregation;     /* 聚合帧模式 */
    uint32_t sample_rate;                       /* 采样率 */
    uint16_t sample_bit_width;                  /* 采样位宽 */
    uint16_t device_id;                         /* 设备号 */
    uint16_t conn_id;                           /* 设备连接号 */
    uint16_t frame_length;                      /* 聚合帧帧长 */
    uint8_t channel_num;                        /* 信道号 */
} ux_audio_stream_config_t;

typedef struct {
    uint8_t channel;
    uint16_t member_hdl;
} ux_audio_iso_bond_param_t;

/**
 * @brief 音频流帧长枚举。
 * @note 默认值为UX_AUDIO_FRAME_LENGTH_10_MS。直播麦场景使用UX_AUDIO_FRAME_LENGTH_5_MS。
 */
typedef enum {
    UX_AUDIO_FRAME_LENGTH_5_MS = 5,  /**< 5ms帧长 */
    UX_AUDIO_FRAME_LENGTH_10_MS = 10, /**< 10ms帧长 */

    UX_AUDIO_FRAME_LENGTH_MAX,
} ux_audio_frame_length_t;

/**
 * @brief 设置音频流帧长。
 * @param [in] frame_length 帧长类型，取值见@ref ux_audio_frame_length_t
 * @retval void
 */
void ux_audio_set_frame_length(ux_audio_frame_length_t frame_length);

/**
 * @brief 获取当前配置的音频流帧长。
 * @retval ux_audio_frame_length_t 当前帧长类型
 * @see ux_audio_set_frame_length
 */
ux_audio_frame_length_t ux_audio_get_frame_length(void);

/**
 * @brief 启动音频流。
 * @param [in] config 指向音频流配置结构体的指针，包含音频流的相关配置信息。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_stream_start(ux_audio_stream_config_t *config);

/**
 * @brief 停止音频流。
 * @param [in] config 指向音频流配置结构体的指针，包含音频流的相关配置信息。
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_stream_stop(ux_audio_stream_config_t *config);

void ux_set_seaclosed(bool state);
errcode_t ux_audio_set_stream_state(ux_api_audio_stream_type_t stream_type, ux_api_audio_stream_state_t state);
AudioStreamType ux_audio_get_active_downlink_stream_type(void);
ux_api_audio_stream_state_t ux_audio_get_stream_state(ux_api_audio_stream_type_t stream_type);

#ifdef __cplusplus
}
#endif
#endif
