/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: header file for audio encoders
 * Author: audio
 */

#ifndef __AENC_STRUCT_H__
#define __AENC_STRUCT_H__

#include "audio_struct.h"
#include "ha_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* one work output; one debug adp */
#define AENC_RBUF_MAX_NUM 2
#define AENC_RBUF_DURA_MS 40
#define AENC_FRAME_MIN_PER_SEC AUDIO_5MS_NUM_PER_SECOND
#define AENC_FRAME_MAX_PER_SEC AUDIO_20MS_NUM_PER_SECOND
#if defined(SAP_DONGLE_PRODUCT)
#define AENC_PARAM_SIZE_MAX 80
#else
#define AENC_PARAM_SIZE_MAX 64
#endif

typedef enum {
    AENC_PARAM_BIT_RATE,
    AENC_PARAM_CHANNEL_MODE,
    AENC_PARAM_BGC_RATE_CFG,
    AENC_PARAM_DATA_SOURCE_CFG, /* 直播mic内录中配置编码器的数据源 */
    AENC_PARAM_MAX,
} aenc_param_type;

typedef struct {
    ha_codec_id codec_id;
    td_bool frame_mode_output;     /* Frame mode output; Closed by default, encoder outputs data by frame when opened */
    td_u32 max_trans_unit_size;    /* Maximum transport unit size; Default 0:Frame of encoder; unit: Bytes */
    ha_codec_enc_param open_param; /* Encoder open param */
} aenc_attr;

typedef struct {
    audio_scene_config asc;
    audio_volume_cfg vol_cfg;

    td_bool bypass;
    td_bool eos;
    td_s32 state;
    td_u32 frame_pms;
    td_u32 frame_size;
    td_u32 ibuf_dur_ms;
    td_u32 obuf_dur_ms;
    td_u32 max_bit_out_size;
    td_u32 proc_data_count;
    td_u32 not_enough_data_cnt;
    td_u32 frame_cnt;
    td_u32 send_fail_cnt;

    ha_codec_id codec_id;
    audio_pcm_format pcm_attr;

    td_u32 bit_rate;

    td_u32 private_data[AENC_PRIVATE_DATA_MAX_SIZE];
    td_u32 private_data_size;

    audio_ring_buf rbuf_in;
    audio_ring_buf rbuf_out[AENC_RBUF_MAX_NUM];
} aenc_proc_item;

typedef enum {
    BMIC_ENC_OUT = 0, /* 用于l2hc编码器及算法后定点内录 */
    BMIC_HDR_OUT = 1, /* 支持算法前浮点内录 */
    BMIC_REC_OUT = 2, /* 支持算法后浮点内录 */
    BMIC_AENC_DATA_SOURCE_MAX,
} bmic_aenc_data_source;

typedef struct {
    td_u32 data_source; /* 0用于l2hc编码器及算法后定点内录, 1支持算法前浮点内录, 2支持算法后浮点内录 */
} aenc_context_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __AENC_STRUCT_H__ */
