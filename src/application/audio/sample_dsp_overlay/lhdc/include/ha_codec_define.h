/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: acodec common define.
 * Author: audio
 * Create: 2021-12-30
 */

#ifndef __HA_CODEC_DEFINE_H__
#define __HA_CODEC_DEFINE_H__

#include "audio_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HACODEC_UNITCONVERSION 1000
#define HACODEC_BIT2BYTE_DIVISOR 8
#define LPCM71SIZE 8
#define LPCM51SIZE 6
#define HACODEC_24BIT_SIZE 4

/* align function */
static inline td_u32 size_align(td_u32 size, td_u32 align)
{
    return (((size) + ((align) - 1)) & ~((align) - 1));
}

/* ret check function */
#define check_ret(ret)                  \
    do {                                     \
        if ((ret) != EOK) {              \
            return (ret);    \
        }                                    \
    } while (0)

/* ptr check function */
#define check_null_ptr(ptr)                  \
    do {                                     \
        if ((ptr) == TD_NULL) {              \
            return ERR_HACODEC_NULL_PTR;    \
        }                                    \
    } while (0)

/* lc3 err fun */
#define check_goto_err(condition, ret_val, err_str, err_num) \
    do {                                                     \
        if ((condition) == 0) {                              \
            ret = ret_val;                                   \
            sap_err_log_info(err_str);                       \
            sap_err_log_s32(err_num);                        \
            goto err_ret;                                    \
        }                                                    \
    } while (0)

/* defines the bit shift. */
/* CNcomment: 定义比特移位枚举 */
typedef enum {
    HA_CODEC_BIT_SHIFT_2 = 2,
    HA_CODEC_BIT_SHIFT_4 = 4,
    HA_CODEC_BIT_SHIFT_6 = 6,
    HA_CODEC_BIT_SHIFT_8 = 8,
    HA_CODEC_BIT_SHIFT_12 = 12,
    HA_CODEC_BIT_SHIFT_16 = 16,
    HA_CODEC_BIT_SHIFT_24 = 24,
    HA_CODEC_BIT_SHIFT_32 = 32,
} ha_codec_bit_shift;

/* defines the magic number. */
/* CNcomment: 定义通用数字枚举 */
typedef enum {
    HA_CODEC_NUM_0 = 0,
    HA_CODEC_NUM_1 = 1,
    HA_CODEC_NUM_2 = 2,
    HA_CODEC_NUM_3 = 3,
    HA_CODEC_NUM_4 = 4,
    HA_CODEC_NUM_5 = 5,
    HA_CODEC_NUM_6 = 6,
    HA_CODEC_NUM_7 = 7,
    HA_CODEC_NUM_8 = 8,
    HA_CODEC_NUM_9 = 9,
    HA_CODEC_NUM_10 = 10,
} ha_codec_number;

/* defines the min bytes size of header. */
/* CNcomment: 定义码流头部最小size枚举 */
typedef enum {
    HA_CODEC_BYTES_SIZE_1 = 1,
    HA_CODEC_BYTES_SIZE_2 = 2,
    HA_CODEC_BYTES_SIZE_4 = 4,
    HA_CODEC_BYTES_SIZE_6 = 6,
    HA_CODEC_BYTES_SIZE_8 = 8,
    HA_CODEC_BYTES_SIZE_10 = 10,
    HA_CODEC_BYTES_SIZE_12 = 12,
    HA_CODEC_BYTES_SIZE_16 = 16,
} ha_codec_bytes_shift;

/* defines the channels. */
/* CNcomment: 定义音频通道枚举 */
typedef enum {
    HA_CODEC_CH_NUM_1 = 1,
    HA_CODEC_CH_NUM_2 = 2,
    HA_CODEC_CH_NUM_3 = 3,
    HA_CODEC_CH_NUM_4 = 4,
    HA_CODEC_CH_NUM_5 = 5,
    HA_CODEC_CH_NUM_6 = 6,
    HA_CODEC_CH_NUM_7 = 7,
    HA_CODEC_CH_NUM_8 = 8,

    HA_CODEC_CH_NUM_MAX,
} ha_codec_ch_num;

/* defines the bit depth during audio sampling. */
/* CNcomment: 定义音频采样位宽枚举 */
typedef enum {
    HA_CODEC_BIT_DEPTH_UNKNOWN = 0, /* Unknown */  /* CNcomment: 未知采样位宽 */
    HA_CODEC_BIT_DEPTH_8 = 8, /* 8-bit depth */    /* CNcomment: 8位采样位宽  */
    HA_CODEC_BIT_DEPTH_16 = 16, /* 16-bit depth */ /* CNcomment: 16位采样位宽 */
    HA_CODEC_BIT_DEPTH_18 = 18, /* 18-bit depth */ /* CNcomment: 18位采样位宽 */
    HA_CODEC_BIT_DEPTH_20 = 20, /* 20-bit depth */ /* CNcomment: 20位采样位宽 */
    HA_CODEC_BIT_DEPTH_24 = 24, /* 24-bit depth */ /* CNcomment: 24位采样位宽 */
    HA_CODEC_BIT_DEPTH_32 = 32, /* 32-bit depth */ /* CNcomment: 32位采样位宽 */
    HA_CODEC_BIT_DEPTH_32F = 33, /* 32-Float-bit depth */ /* CNcomment: 32位浮点采样位宽 */

    HA_CODEC_BIT_DEPTH_MAX
} ha_codec_bit_depth;

/* CNcomment: 定义音频采样率枚举 */
typedef enum {
    HA_CODEC_SAMPLE_RATE_UNKNOWN = 0, /* Unknown */                  /* CNcomment: 未知采样频率 */
    HA_CODEC_SAMPLE_RATE_8K = 8000, /* 8 kHz sampling rate */        /* CNcomment: 8K采样频率 */
    HA_CODEC_SAMPLE_RATE_11K = 11025, /* 11.025 kHz sampling rate */ /* CNcomment: 11.025K采样频率 */
    HA_CODEC_SAMPLE_RATE_12K = 12000, /* 12 kHz sampling rate */     /* CNcomment: 12K采样频率 */
    HA_CODEC_SAMPLE_RATE_16K = 16000, /* 16 kHz sampling rate */     /* CNcomment: 16K采样频率 */
    HA_CODEC_SAMPLE_RATE_22K = 22050, /* 22.050 kHz sampling rate */ /* CNcomment: 22.050K采样频率 */
    HA_CODEC_SAMPLE_RATE_24K = 24000, /* 24 kHz sampling rate */     /* CNcomment: 24K采样频率 */
    HA_CODEC_SAMPLE_RATE_32K = 32000, /* 32 kHz sampling rate */     /* CNcomment: 32K采样频率 */
    HA_CODEC_SAMPLE_RATE_44K = 44100, /* 44.1 kHz sampling rate */   /* CNcomment: 44.1K采样频率 */
    HA_CODEC_SAMPLE_RATE_48K = 48000, /* 48 kHz sampling rate */     /* CNcomment: 48K采样频率 */
    HA_CODEC_SAMPLE_RATE_64K = 64000, /* 64 kHz sampling rate */     /* CNcomment: 64K采样频率 */
    HA_CODEC_SAMPLE_RATE_88K = 88200, /* 88.2 kHz sampling rate */   /* CNcomment: 88.2K采样频率 */
    HA_CODEC_SAMPLE_RATE_96K = 96000, /* 96 kHz sampling rate */     /* CNcomment: 96K采样频率 */
    HA_CODEC_SAMPLE_RATE_176K = 176400, /* 176 kHz sampling rate */  /* CNcomment: 176K采样频率 */
    HA_CODEC_SAMPLE_RATE_192K = 192000, /* 192 kHz sampling rate */  /* CNcomment: 192K采样频率 */
    HA_CODEC_SAMPLE_RATE_288K = 288000, /* 288 kHz sampling rate */  /* CNcomment: 288K采样频率 */

    HA_CODEC_SAMPLE_RATE_MAX
} ha_codec_sample_rate;

/* CNcomment: 定义声道对应的数组下标枚举 */
typedef enum {
    HA_CODEC_CHANNEL_INDEX_0 = 0, /* channal 1 index 0 */ /* CNcomment: 第1声道, 下标为0 */
    HA_CODEC_CHANNEL_INDEX_1 = 1, /* channal 2 index 1 */ /* CNcomment: 第2声道, 下标为1 */
    HA_CODEC_CHANNEL_INDEX_2 = 2, /* channal 3 index 2 */ /* CNcomment: 第3声道, 下标为2 */
    HA_CODEC_CHANNEL_INDEX_3 = 3, /* channal 4 index 3 */ /* CNcomment: 第4声道, 下标为3 */
    HA_CODEC_CHANNEL_INDEX_4 = 4, /* channal 5 index 4 */ /* CNcomment: 第5声道, 下标为4 */
    HA_CODEC_CHANNEL_INDEX_5 = 5, /* channal 6 index 5 */ /* CNcomment: 第6声道, 下标为5 */
    HA_CODEC_CHANNEL_INDEX_6 = 6, /* channal 7 index 6 */ /* CNcomment: 第7声道, 下标为6 */
    HA_CODEC_CHANNEL_INDEX_7 = 7, /* channal 8 index 7 */ /* CNcomment: 第8声道, 下标为7 */
} ha_codec_channel_index;

/* defines the bit rate during audio sampling. */
/* CNcomment: 定义音频比特速率枚举 */
typedef enum {
    HA_CODEC_BIT_RATE_UNKNOWN = 0, /* Unknown */ /* CNcomment: 未知比特速率 */
    HA_CODEC_BIT_RATE_64K = 64000, /* 64Kbps */  /* CNcomment: 64kbps */
    HA_CODEC_BIT_RATE_56K = 56000, /* 56Kbps */  /* CNcomment: 56kbps */
    HA_CODEC_BIT_RATE_50K = 50000,
    HA_CODEC_BIT_RATE_48K = 48000, /* 48Kbps */ /* CNcomment: 48kbps */
    HA_CODEC_BIT_RATE_16K = 16000, /* 16Kbps */ /* CNcomment: 16kbps */
} ha_codec_bit_rate;

typedef struct {
    td_u8 *data;
    td_u32 size;

    td_void *in_ext;
} adsp_aflt_input_frame;

typedef struct {
    td_u32 sample_rate; /* 采样率 */
    td_u32 channel;     /* 声道数 */
    td_u32 bit_depth;   /* 位宽，16/24 */
    td_u32 size;        /* 数据长度，单位byte. */
    td_u32 pad_size;    /* 补零长度，单位byte. */
} adsp_aflt_frame_attr;

typedef struct {
    td_u32 in_frame_cost;

    adsp_aflt_frame_attr pcm_attr; /* PCM:输出pcm帧参数 */
    td_void *pcm_data;             /* PCM:buffer指针，由上层分配并传入. */

    /* decoder only. */
    adsp_aflt_frame_attr lbr_attr; /* LBR:输出lbr帧参数 */
    td_void *lbr_data;             /* LBR:buffer指针，由上层分配并传入. */

    /* decoder only. */
    adsp_aflt_frame_attr hbr_attr; /* HBR:输出hbr帧参数 */
    td_void *hbr_data;             /* HBR:buffer指针，由上层分配并传入. */

    td_u32 dec_bytes_left; /* 解码器内部残留数据长度，用于计算PTS，单位byte */

    td_bool dec_samples_left;
    td_bool eos_over;
    td_bool lbr_enable;
    td_bool hbr_enable;
    td_bool ext_data_suppout;
    td_void *out_ext;
} adsp_aflt_output_frame;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HA_CODEC_DEFINE_H__ */
