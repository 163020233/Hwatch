/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) server audio sream management APIs
 */
#ifndef SSF_AS_STRU_H
#define SSF_AS_STRU_H

#include <stdbool.h>
#include "sle_common.h"
#include "ssf_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AS_CODING_RATE_CONFIG_LENGTH 5

typedef enum {
    SLE_AUDIO_STREAM_CODEC_ID_PCM = 0x00,                /*!< @if Eng codec id PCM
                                                              @else   PCM编码器类型 @endif */
    SLE_AUDIO_STREAM_CODEC_ID_L2HC = 0x01,               /*!< @if Eng codec id L2HC
                                                              @else   L2HC编码器类型 @endif */
    SLE_AUDIO_STREAM_CODEC_ID_L2HC_VOICE = 0x02,         /*!< @if Eng codec id L2HC VOICE
                                                              @else   L2HC VOICE编码器类型 @endif */
    SLE_AUDIO_STREAM_CODEC_BUTT,                         /*!< @if Eng codec id butt
                                                              @else   标准编码器类型尾部 @endif */
    SLE_AUDIO_STREAM_CODEC_ID_EXPANSION = 0xFF,          /*!< @if Eng codec id expansion
                                                              @else   扩展编码器类型 @endif */
} bs_sle_audio_stream_codec_id_t;

typedef enum {
    SLE_AUDIO_L2HC_VERSION_2 = 0x00,             /*!< @if Eng L2HC version 2.0
                                                      @else   L2HC DSP编码器2.0版本 @endif */
    SLE_AUDIO_L2HC_VERSION_2_STD = 0x01,         /*!< @if Eng L2HC version 2.0 STD
                                                      @else   L2HC DSP编码器2.0标准版本 @endif */
    SLE_AUDIO_L2HC_VERSION_3 = 0x02,             /*!< @if Eng L2HC version 3.0
                                                      @else   L2HC DSP编码器3.0版本 @endif */
    SLE_AUDIO_L2HC_VERSION_4_0_HISI = 0x03,             /*!< @if Eng L2HC version 4.0 海思私有
                                                      @else   L2HC DSP编码器4.0版本 @endif */
} bs_sle_audio_stream_dsp_l2hc_version_t;

typedef enum {
    SLE_AUDIO_L2HC_CODEC_VERSION_STD = 0x01,            /*!< @if Eng L2HC version
                                                            @else   L2HC 编码器标准版本 @endif */
    SLE_AUDIO_L2HC_VOICE_CODEC_VERSION = 0x01,          /*!< @if Eng L2HC Voice version
                                                            @else   L2HC Voice 编码器版本 @endif */
    SLE_AUDIO_L2HC_4_0_HISI_CODEC_VERSION = 0x01,       /*!< @if Eng L2HC HISI_4.0 version
                                                            @else   L2HC HISI_4.0 编码器版本 @endif */
} bs_sle_audio_stream_l2hc_codec_version_t;

typedef enum {
    SLE_AUDIO_L2HC_VENDOR_ID_HW = 0x00,         /*!< @if Eng L2HC vendor id HW
                                                     @else   L2HC 厂商ID HW @endif */
    SLE_AUDIO_L2HC_VENDOR_ID_SL = 0x01,         /*!< @if Eng L2HC vendor id SL
                                                     @else   L2HC 厂商ID SL @endif */
    SLE_AUDIO_L2HC_VENDOR_ID_CA = 0x02,         /*!< @if Eng L2HC vendor id CA
                                                     @else   L2HC 厂商ID CA @endif */
} bs_sle_audio_stream_vendor_id_t;

typedef enum {
    SLE_AUDIO_L2HC_PROTOCOL_VENDOR_ID_DEFAULT = 0x00,       /*!< @if Eng L2HC protocol vendor id default
                                                                 @else   L2HC 协议默认厂商ID @endif */
    SLE_AUDIO_L2HC_PROTOCOL_VENDOR_ID_HW = 0x0009,          /*!< @if Eng L2HC protocol vendor id HW
                                                                 @else   L2HC 协议厂商ID HW @endif */
    SLE_AUDIO_L2HC_PROTOCOL_VENDOR_ID_HISI = 0x007C,        /*!< @if Eng L2HC vendor id HISI
                                                                 @else   L2HC 协议厂商ID HISI @endif */
} bs_sle_audio_stream_protocol_vendor_id_t;

typedef enum {
    SLE_AUDIO_L2HC_VENDOR_CODEC_ID_DEFAULT = 0x00,       /*!< @if Eng L2HC protocol vendor id default
                                                                 @else   L2HC 默认厂商编码器ID @endif */
    SLE_AUDIO_L2HC_VENDOR_CODEC_ID_HISI = 0x01,        /*!< @if Eng L2HC vendor id HISI
                                                                 @else   L2HC HISI厂商编码器ID @endif */
} bs_sle_audio_stream_vendor_codec_id_t;

typedef enum {
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_8K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_11025K, // 11.025K
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_16K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_2205K,  // 22.05k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_24K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_32K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_441K,   // 44.1k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_48K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_882K,   // 88.2k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_96K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_1764K,  // 176.4k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_192K,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_384K,
} bs_sle_l2hc_sampling_frequency_t;

typedef enum {
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_8K = 0x1,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_11025K = 0x2, // 11.025K
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_16K = 0x4,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_2205K = 0x8,  // 22.05k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_24K = 0x10,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_32K = 0x20,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_441K = 0x40,   // 44.1k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_48K = 0x80,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_882K = 0x100,   // 88.2k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_96K = 0x200,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_1764K = 0x400,  // 176.4k
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_192K = 0x800,
    SLE_AUDIO_L2HC_SAMPLING_FREQUENCY_BITMAP_384K = 0x1000,
} bs_sle_l2hc_sampling_frequency_bitmap_t;

typedef enum {
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_8BIT_FIX,    // 8bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_16BIT_FIX,   // 16bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_24BIT_FIX,   // 24bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_32BIT_FIX,   // 32bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_32BIT_FLOAT, // 32bit浮点
} bs_sle_l2hc_sampling_depth_t;

typedef enum {
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_BITMAP_8BIT_FIX = 0x01,    // 8bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_BITMAP_16BIT_FIX = 0x02,   // 16bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_BITMAP_24BIT_FIX = 0x04,   // 24bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_BITMAP_32BIT_FIX = 0x08,   // 32bit定点
    SLE_AUDIO_L2HC_SAMPLING_DEPTH_BITMAP_32BIT_FLOAT = 0x10, // 32bit浮点
} bs_sle_l2hc_sampling_depth_bitmap_t;

typedef enum {
    SLE_AUDIO_L2HC_CHANNEL_TYPE_INVALID,           // 无效音频通路类型
    SLE_AUDIO_L2HC_CHANNEL_TYPE_MONO,              // 音频单声道
    SLE_AUDIO_L2HC_CHANNEL_TYPE_DUAL_CHANNEL,      // 音频双声道
} bs_sle_l2hc_audio_channel_type_t;

typedef enum {
    SLE_AUDIO_L2HC_FRAME_LENGTH_MODE_5MS = 0x04,   // bit3 5ms
    SLE_AUDIO_L2HC_FRAME_LENGTH_MODE_10MS = 0x08,  // bit4 10ms
} bs_sle_l2hc_frame_length_mode_t;

typedef enum {
    SLE_AUDIO_L2HC_CODING_RATE_MONO_64KBPS = 0x8,                   // 单声道64kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_96KBPS = 0x10,                  // 单声道96kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_128KBPS = 0x20,                 // 单声道128kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_160KBPS = 0x40,                 // 单声道160kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_240KBPS = 0x80,                 // 单声道240kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_320KBPS = 0x100,                // 单声道320kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_480KBPS = 0x200,                // 单声道480kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_750KBPS = 0x400,                // 单声道750kbps
    SLE_AUDIO_L2HC_CODING_RATE_MONO_960KBPS = 0x800,                // 单声道960kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_128KBPS = 0x800000,     // 双声道128kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_192KBPS = 0x1000000,    // 双声道192kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_256KBPS = 0x2000000,    // 双声道256kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_320KBPS = 0x4000000,    // 双声道320kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_480KBPS = 0x8000000,    // 双声道480kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_640KBPS = 0x10000000,   // 双声道640kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_960KBPS = 0x20000000,   // 双声道960kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_1500KBPS = 0x40000000,  // 双声道1500kbps
    SLE_AUDIO_L2HC_CODING_RATE_DUAL_CHANNEL_1920KBPS = 0x80000000,  // 双声道1920kbps
} bs_sle_l2hc_coding_rate_t;

typedef enum {
    SLE_UNDEFINED_AUDIO_STREAM = 0x00000001,       //  未定义的音频流
    SLE_MUSIC_AUDIO_STREAM = 0x00000002,           //  媒体类型的音频流
    SLE_TELE_AUDIO_STREAM = 0x00000004,            //  通话类型的音频流
    SLE_VOICE_ASSISTANT_AUDIO_STREAM = 0x00000008, // 语音助手类型的音频流
    SLE_RING_AUDIO_STREAM = 0x00000010,            // 铃音类型的音频流
    SLE_IP_CALL_AUDIO_STREAM = 0x00000020,         // IP通话类型的音频流
    SLE_GAME_AUDIO_STREAM = 0x00000040,            // 游戏类型的音频流
    SLE_RECORD_AUDIO_STREAM = 0x00000080,          // 录音类型的音频流
    SLE_REMINDER_AUDIO_STREAM = 0x00000100,        // 提醒类型的音频流
    SLE_VIDEO_AUDIO_STREAM = 0x00000200,           // 视频类型的音频流
    SLE_NAVIGATION_AUDIO_STREAM = 0x00000400,      // 导航类型的音频流
    SLE_ALARM_AUDIO_STREAM = 0x00000800,           // 告警类型的音频流
} bs_sle_audio_stream_standard_type_t;

typedef enum {
    SLE_INVALID_VENDOR_AUDIO_STREAM = 0x00000000,    // 如果vendor_type取值为0，采用标准类型
    SLE_UAC_DOWN_AUDIO_STREAM = 0x00000001,
    SLE_UAC_UP_AUDIO_STREAM = 0x00000002,
    SLE_NONE_AUDIO_STREAM = 0xFFFFFFFF,              // 若取值为0xFFFFFFFF，则表示不支持音频流
} bs_sle_audio_stream_vendor_type_t;

typedef enum {
    SLE_AS_STREAM_TYPE_MUSIC,  // 音乐流类型
    SLE_AS_STREAM_TYPE_CALL,   // 通话流类型
} bs_sle_audio_stream_stream_type_t;

typedef enum {
    SLE_SUPPORT_NONE = 0X00,       // 对端不支持音频流
    SLE_SUPPORT_ONLY_SINK = 0x01,  // 对端仅支持音宿，只能接受数据和消费
    SLE_SUPPORT_ONLY_SRC = 0x02,   // 对端仅支持音源，只能发送和产生数据
    SLE_SUPPORT_BOTH = 0x03,       // 对端即支持音源，也支持音宿
} bs_sle_audio_stream_cap_t;

typedef enum {
    SLE_AUDIO_STREAM_STATE_OPENED,     // 已开启通道
    SLE_AUDIO_STREAM_STATE_STARTING,   // 起流中
    SLE_AUDIO_STREAM_STATE_STREAMING,  // 传输中
    SLE_AUDIO_STREAM_STATE_STOPED,     // 已停止
    SLE_AUDIO_STREAM_STATE_CLOSED,     // 已关闭
} bs_sle_audio_stream_state_t;

typedef enum {
    SLE_AUDIO_STREAM_CHANNEL_MONO = 0x1,           // 单声道
    SLE_AUDIO_STREAM_CHANNEL_LEFT = 0x2,           // 左声道
    SLE_AUDIO_STREAM_CHANNEL_RIGHT = 0x4,          // 右声道
    SLE_AUDIO_STREAM_CHANNEL_LEFT_FORWARD = 0x8,   // 左前声道
    SLE_AUDIO_STREAM_CHANNEL_RIGHT_FORWARD = 0x10, // 右前声道
    SLE_AUDIO_STREAM_CHANNEL_LEFT_BACK = 0x20,     // 左后声道
    SLE_AUDIO_STREAM_CHANNEL_RIGHT_BACK = 0x40,    // 右后声道
    SLE_AUDIO_STREAM_CHANNEL_LFE = 0x80,    // 低音声道
} bs_sle_audio_stream_channel_info_t;

typedef struct {
    bs_sle_audio_stream_vendor_type_t vendor_type;         // vendor_type为0时以standard_type为准
    bs_sle_audio_stream_stream_type_t stream_type;
    bs_sle_audio_stream_standard_type_t standard_type;
} bs_sle_audio_stream_type_t;

typedef struct {
    uint8_t version;                                    // 版本号
    uint8_t sample_rate;                                // 采样率
    uint8_t bit_width;                                  // 采样位宽
    uint8_t frame_length_mode;                          // 帧长模式
    uint8_t coding_rate[AS_CODING_RATE_CONFIG_LENGTH];  // 编码速率
    uint8_t channel;                                    // 音频通道模式
} bs_sle_l2hc_codec_param_t;

typedef union {
    bs_sle_l2hc_codec_param_t l2hc_codec;
} bs_sle_codec_data_param_t;

typedef struct {
    uint8_t codec_id;                 // 编解码器类型 { @ref bs_sle_audio_stream_codec_id_t }
    bool frame_aggregation_enable;    // 聚合帧开关
    uint16_t vendor_id;               // 厂商ID { @ref bs_sle_audio_stream_vendor_id_t }
    uint16_t vendor_codec_id;         // 厂商编解码器ID
    bs_sle_codec_data_param_t codec_data;    // 编码器配置参数
} bs_sle_codec_param_t;

typedef struct {
    uint32_t stream_id;
    uint16_t conn_id;
    bs_sle_audio_stream_type_t stream_type;   // 音频流类型
    uint8_t role;   // 0: source, 1: sink 预留
} bs_sle_stream_stru_t;

typedef struct {
    bs_sle_stream_stru_t stream_stru;
    bs_sle_codec_param_t param;
} bs_sle_stream_codec_param_t;

typedef struct {
    uint32_t stream_id;
    bool is_accept;
} bs_sle_stream_res_stru_t;

typedef struct {
    uint8_t channel;   // 音频通道 { @ref bs_sle_audio_stream_channel_info_t }
    uint16_t member_hdl;
} iso_bond_param_t;

typedef struct {
    uint8_t version; // 编码器版本 { @ref sle_audio_stream_l2hc_version_t }
    uint16_t sampling_frequency_bitmap; // 采样率位图 { @ref bs_sle_l2hc_sampling_frequency_bitmap_t }
    uint8_t sampling_depth_bitmap; // 采样深度位图 { @ref sle_pcm_sampling_depth_t }
    uint16_t channel_bitmap; // 支持的音频通道位图 { @ref bs_sle_l2hc_audio_channel_type_t }
    uint8_t frame_length_mode_bitmap; // 支持的帧长模式位图 { @ref bs_sle_l2hc_frame_length_mode_t }
    uint8_t coding_rate_bitmap[AS_CODING_RATE_CONFIG_LENGTH]; // 编码速率位图 { @ref bs_sle_l2hc_coding_rate_t }
} bs_sle_audio_l2hc_codec_capability_config_param_t;

#ifdef __cplusplus
}
#endif
#endif