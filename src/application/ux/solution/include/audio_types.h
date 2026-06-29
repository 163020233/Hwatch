/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          provide audio type define.
 * Description:  provide ux audio recorder header file.
 */

#ifndef AUDIO_TYPES_H
#define AUDIO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_DEVICE_NUM 4

typedef enum {
    AUDIO_DEVICE_DEFAULTE = 0x0, /* 默认设备,若不传device,则默认为此设备 */

    AUDIO_DEVICE_NEARLINK_IN,   /* 星闪输入设备 */
    AUDIO_DEVICE_NEARLINK_OUT,  /* 星闪输出设备 */
    AUDIO_DEVICE_USB_IN,        /* USB输入设备 */
    AUDIO_DEVICE_USB_OUT,       /* USB输出设备 */
    AUDIO_DEVICE_JACK_AUDIO_IN, /* 3.5mm输入设备 */
    AUDIO_DEVICE_JACK_AUDIO_OUT /* 3.5mm输出设备 */
} audio_device_t;

typedef enum codec_format_t {
    CODEC_FORMAT_SBC,    /* SBC编码格式 */
    CODEC_FORMAT_AAC,    /* AAC编码格式 */
    CODEC_FORMAT_L2HC,   /* L2HC编码格式 */
    CODEC_FORMAT_L2HC_VOICE,
    CODEC_FORMAT_LC3,    /* LC3编码格式 */
    CODEC_FORMAT_LHDC,   /* LHDC编码格式 */
    CODEC_FORMAT_HAC,    /* HAC编码格式 */
    CODEC_FORMAT_MSBC,   /* MSBC编码格式 */
    CODEC_FORMAT_CVSD,   /* CVSD编码格式 */
    CODEC_FORMAT_PCM,    /* PCM编码格式 */
    CODEC_FORMAT_LDAC,   /* LDAC编码格式 */
    CODEC_FORMAT_INVALID /* 无效编码格式 */
} codec_format_t;

typedef enum {
    SAMPLE_BIT_WIDTH_NONE = 0,
    SAMPLE_BIT_WIDTH_8 = 8,
    SAMPLE_BIT_WIDTH_16 = 16,
    SAMPLE_BIT_WIDTH_24 = 24,
    SAMPLE_BIT_WIDTH_32 = 32,
    SAMPLE_BIT_WIDTH_32F = 33,
    SAMPLE_BIT_WIDTH_MAX
} sample_bit_width_t;

typedef enum {
    SAMPLE_RATE_NONE = 0,
    SAMPLE_RATE_8000 = 8000,
    SAMPLE_RATE_16000 = 16000,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_24000 = 24000,
    SAMPLE_RATE_32000 = 32000,
    SAMPLE_RATE_44100 = 44100,
    SAMPLE_RATE_48000 = 48000,
    SAMPLE_RATE_96000 = 96000,
    SAMPLE_RATE_192000 = 192000,
    SAMPLE_RATE_384000 = 384000,
    SAMPLE_RATE_MAX
} sample_rate_t;

typedef enum {
    CHANNEL_NUM_1 = 1,
    CHANNEL_NUM_2 = 2,
} channel_num_t;

typedef enum {
    AUDIO_L2HC_CODEC_PROTOCOL_BT = 0,
    AUDIO_L2HC_CODEC_PROTOCOL_SL = 1,
    AUDIO_L2HC_CODEC_PROTOCOL_LR = 2,
    AUDIO_L2HC_CODEC_PROTOCOL_MAX,
} audio_stream_l2hc_protocol_t;

typedef enum {
    AUDIO_L2HC_VERSION_2 = 0x00,
    AUDIO_L2HC_VERSION_2_STD = 0x01,
    AUDIO_L2HC_VERSION_3 = 0x02,
    AUDIO_L2HC_VERSION_4 = 0x03,
} audio_stream_l2hc_version_t; /* link@ bs_sle_audio_stream_dsp_l2hc_version_t */

typedef enum {
    AUDIO_L2HC_VENDOR_ID_HW = 0x00,
    AUDIO_L2HC_VENDOR_ID_SL = 0x01,
    AUDIO_L2HC_VENDOR_ID_CA = 0x02,
} audio_stream_vendor_id_t; /* link@ bs_sle_audio_stream_vendor_id_t */

typedef enum {
    AUDIO_FRAME_AGGREGATION_DISABLE = 0,
    AUDIO_FRAME_AGGREGATION_ENABLE = 1,
    AUDIO_FRAME_AGGREGATION_MAX,
} audio_stream_aggregation_t;

typedef enum {
    AUDIO_L2HC_FRAME_LENGTH_MODE_5MS = 0x04,   // bit3 5ms
    AUDIO_L2HC_FRAME_LENGTH_MODE_10MS = 0x08,  // bit4 10ms
} audio_l2hc_frame_length_mode_t; /* link@ bs_sle_l2hc_frame_length_mode_t */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif