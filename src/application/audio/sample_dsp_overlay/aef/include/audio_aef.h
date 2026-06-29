/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: define audio effect data structure
 * Author: AuthorNameMagicTag
 */

#ifndef __AUDIO_AEF_H__
#define __AUDIO_AEF_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Audio effect type */
/* CNcomment: 音效类型枚举 */
typedef enum {
    AEF_TYPE_NULL,    /*!< @if Eng NULL audio effect.
                           @else   无效音效 @endif */
    AEF_TYPE_SWS,     /*!< @if Eng SWS audio effect.
                           @else   SWS音效 @endif */
    AEF_TYPE_SMARTPA, /*!< @if Eng Smart PA audio effect.
                           @else   Smart PA音效 @endif */
    AEF_TYPE_WDRC,    /*!< @if Eng WDRC audio effect for hearing aid.
                           @else    助听WDRC音效 @endif */
    AEF_TYPE_AVC,     /*!< @if Eng Auto volume control.
                           @else   自动音量控制 @endif */
    AEF_TYPE_GEQ,     /*!< @if Eng GEQ audio effect.
                           @else   GEQ音效 @endif */
    AEF_TYPE_DRC,     /*!< @if Eng Dynamic range control.
                           @else   音频动态范围控制 @endif */
    AEF_TYPE_PEQ,     /*!< @if Eng PEQ audio effect.
                           @else   PEQ音效 @endif */
    AEF_TYPE_AEF_PRE, /*!< @if Eng Pre-processing aef audio effect.
                           @else   AEF前处理音效 @endif */

    AEF_TYPE_CUSTOMER = 0x80,   /*!< @if Eng Customer audio effect.
                                     @else   客户自研音效 @endif */

    AEF_TYPE_MAX
} aef_type;

/**
 * @if Eng
 * @brief  make audio effect command id.
 * @else
 * @brief  音效接口命令id生成方法
 * @endif
 */
#define aef_cmd(aef_type, cmd_id) (td_u32)(((aef_type) << 16) | (cmd_id))

#define aef_cmd_id(cmd_id) ((cmd_id) & 0xffff)

#define aef_cmd_type(cmd_id) (td_u32)((cmd_id) >> 16)

/* defines audio format */
typedef struct {
    td_u32 channels;    /* Number of channels */
    td_u32 bit_depth;   /* ::audio_bit_depth */
    td_u32 sample_rate; /* ::audio_sample_rate */
    td_u32 samples_per_frame;
} audio_pcm_format;

/* defines the audio frame. */
typedef struct {
    td_bool interleaved;  /* whether the data is interleaved */
    td_u32 channels;     /* Number of Channels */
    td_u32 bit_depth;    /* Bit Depth */
    td_u32 sample_rate;  /* Sampling Rate */
    td_s64 pts;          /* presentation time stamp (PTS, unit: us) */
    td_s32 *pcm_buffer;  /* pointer to the buffer for storing the pulse code modulation (PCM) data */
    td_s32 *bits_buffer; /* pointer to the buffer for storing the stream data */
    td_u32 bits_bytes;   /* IEC61937 data size */
    td_u32 pcm_samples;  /* number of sampling points of the PCM data */
    td_u32 frame_index;  /* Frame ID */

    /* eos */
    /* CNcomment: eos */
    td_bool eos;
    /* the packet loss */
    /* CNcomment: 是否丢包 CNend */
    td_bool pkg_loss;
} audio_frame;

typedef struct {
    td_void *scratch_buf;
    td_u32 scratch_buf_size;
} aef_cfg;

typedef struct effect_ctx* aef_handle;

typedef struct {
    const td_char *reserved1;
    const td_char *reserved2;
    const td_char *name;
    aef_type type;
    td_u32 version;

    td_s32 (*get_default_open_param)(td_void *open_param, td_u32 param_size);
    td_s32 (*create)(const td_void *aef_attr, td_void **aef, const aef_cfg *cfg);
    td_s32 (*destroy)(td_void *aef);
    td_s32 (*set_config)(td_void *aef, td_u32 cmd, const td_void *cfg);
    td_s32 (*get_config)(td_void *aef, td_u32 cmd, td_void *cfg);
    td_s32 (*set_parameter)(td_void *aef, td_u32 cmd, const td_void *parameter, td_u32 param_size);
    td_s32 (*get_parameter)(td_void *aef, td_u32 cmd, td_void *parameter, td_u32 param_size);

    td_s32 (*set_enable)(td_void *aef, td_bool enable);
    td_s32 (*get_enable)(td_void *aef, td_bool *enable);
    td_s32 (*get_max_pcm_in_size)(td_void *aef, td_u32 *in_size);
    td_s32 (*get_max_pcm_out_size)(td_void *aef, td_u32 *out_size);
    td_s32 (*get_input_pcm_attr)(td_void *aef, audio_pcm_format *pcm_attr);
    td_s32 (*proc_frame)(td_void *aef, audio_frame *in_frame, audio_frame *out_frame);
} aef_component;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __AUDIO_AEF_H__ */
