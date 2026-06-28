/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: anc coef define for socmn2
 * Author: audio
 */

#ifndef __ANC_COEF_H__
#define __ANC_COEF_H__

#include "audio_type.h"
#include "audio_struct.h"

typedef enum {
    /* anc ch */
    ANC_CH_FF0,
    ANC_CH_FF1,
    ANC_CH_FB,

    /* anc spe */
    ANC_SPE,

    /* anc post proc0 and anc post proc1 */
    ANC_POST_PROC0,
    ANC_POST_PROC1,

    /* anc wfir */
    ANC_CH_WFIR,

    ANC_CH_NUM,
} anc_channel_type;

/* anc mixer id */
enum {
    ANC_MIXER_SPE,

    ANC_MIXER_FB,

    ANC_MIXER_POST_FF0,
    ANC_MIXER_POST_FF1,

    ANC_MIXER_PATH_MIX0,
    ANC_MIXER_PATH_MIX1,

    ANC_MIXER_ID_MAX,
};

enum {
    ANC_COEF_TYPE_PGA,
    ANC_COEF_TYPE_BQ,
    ANC_COEF_TYPE_LMT,
    ANC_COEF_TYPE_WFIR,

    ANC_COEF_TYPE_MAX,
};

#define ANC_BQ_NUM_MAX 20
#define ANC_BIQUAD_COEF_NUM 5

#define ANC_CH_SPE_BIQUAD_NUM 16
#define ANC_CH_SPE_BIQUAD_REG 0x4004 /* ANC_SPE_BQ_COEFA1_0: ANC_SPE_BQ_COEFA1_0 */

#define ANC_CH_FB_BIQUAD_NUM 20
#define ANC_CH_FB_BIQUAD_REG 0x4144 /* ANC_FB_BQ_COEFA1_0: ANC FB BQ COEFA1 */

#define ANC_CH_FF0_BIQUAD_NUM 20
#define ANC_CH_FF0_BIQUAD_REG 0x42D4 /* ANC_B_CH_BQ_COEFA1_0: ANC CH0 BQ COEFA1 */

#define ANC_CH_FF1_BIQUAD_NUM 20
#define ANC_CH_FF1_BIQUAD_REG 0x4464 /* ANC_B_CH_BQ_COEFA1_20: ANC CH1 BQ COEFA1 */

typedef struct anc_bq_coef {
    td_u32 a1;
    td_u32 a2;
    td_u32 b0;
    td_u32 b1;
    td_u32 b2;
} anc_bq_coef;

/* 将anccs的输出全部按照无符号数处理，方便配置到寄存器 */
typedef struct anc_ch_bq_coef {
    td_u32 bypass;
    td_u32 gain;
    anc_bq_coef coef[ANC_BQ_NUM_MAX]; /* signed, bitwidth coeff_wl */
} anc_ch_bq_coef;

/* 将anccs的输出全部按照无符号数处理，方便配置到寄存器 */
typedef struct anc_pga_coef {
    td_u32 sft;            /* PGA step sft default 0 [0 15] 5bit  */
    td_u32 gain;           /* PGA step gain default 1 [0 2^15] 16bit  */
    td_u32 pga_fade_in;    /* PGA step  time=2^N/fs [0 15], 5bit */
    td_u32 pga_fade_out;   /* PGA step  time=2^N/fs [0 15], 5bit */
    td_s32 pga_gain;       /* PGA step  S4.20 [-8388608, 8388607] */
    td_u32 pga_auto_en;    /* PGA step  [0 1] 1bit */
    td_u32 pga_zero_en;    /* PGA step  [0 1] 1bit */
    td_u32 gain_byp;       /* pga nonstep 1 bypass 0 nonbypass [0 1] 1bit */
    td_u32 pga_byp;        /* pga step 1 bypass 0 nonbypass [0 1] 1bit */
    td_u32 pga_auto_thred; /* PGA step  [0 96000] 24bit */
} anc_pga_coef;

/* 将anccs的输出全部按照无符号数处理，方便配置到寄存器 */
typedef struct anc_limiter_coef {
    td_u32 bypass;
    td_u32 mode;
    td_u32 thresh;
    td_u32 comp_type; /* 0-peak, 1-rms */
    td_u32 attack_step;
    td_u32 release_step;
    td_u32 alpha_rms;
    td_u32 zc_on; /* apply gain change on zero-crossing only */
} anc_limiter_coef;

typedef struct anc_mixer_coef {
    td_u32 mute;
} anc_mixer_coef;

typedef struct {
    audio_buffer mmz_buf; /* [in/out] */
    td_u32 cmd; /* [in] */
    td_u32 param_size; /* [in] */
    td_u8 *param; /* [in/out] */
} anc_common_rw_param;

/**
 * @if Eng
 * @brief  ANC one BQ coefficient read-write param, used by #ANC_RW_BQ_COEF_CMD.
 * @else
 * @brief  ANC一个BQ的系数读写参数，对应命令#ANC_RW_BQ_COEF_CMD
 * @endif
 */
typedef struct {
    td_u32 channel; /* [in] see #anc_channel_type */
    td_u32 id; /* [in] */
    anc_bq_coef bq_coef; /* [in/out] */
} anc_bq_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC channel BQ coefficient read-write param, used by #ANC_RW_CH_BQ_COEF_CMD.
 * @else
 * @brief  ANC一个通道所有BQ的系数读写参数，对应命令#ANC_RW_CH_BQ_COEF_CMD
 * @endif
 */
typedef struct {
    td_u32 channel; /* [in] see #anc_channel_type */
    anc_ch_bq_coef bq_coef; /* [in/out] */
} anc_ch_bq_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC channel BQ gain read-write param, used by #ANC_RW_CH_BQ_GLOBAL_GAIN_CMD.
 * @else
 * @brief  ANC一个通道BQ的全局增益读写参数，对应命令#ANC_RW_CH_BQ_GLOBAL_GAIN_CMD
 * @endif
 */
typedef struct {
    td_u32 channel; /* [in] see #anc_channel_type */
    td_u32 gain; /* [in/out] */
} anc_ch_bq_gain_rw_param;

/**
 * @if Eng
 * @brief  ANC one PGA coefficient read-write param, used by #ANC_RW_PGA_COEF_CMD.
 * @else
 * @brief  ANC一个PGA的系数读写参数，对应命令#ANC_RW_PGA_COEF_CMD
 * @endif
 */
typedef struct {
    td_u32 channel; /* [in] see #anc_channel_type */
    td_u32 id; /* [in] */
    anc_pga_coef pga_coef; /* [in/out] */
} anc_pga_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC one specific limiter's coefficient read-write param, used by #ANC_RW_LIMITER_COEF_CMD.
 * @else
 * @brief  ANC一个limiter的系数读写参数，对应命令#ANC_RW_LIMITER_COEF_CMD
 * @endif
 */
typedef struct {
    td_u32 channel; /* [in] see #anc_channel_type */
    td_u32 id; /* [in] */
    anc_limiter_coef limiter_coef; /* [in/out] */
} anc_limiter_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC one specific mixer's coefficient read-write param, used by #ANC_RW_MIXER_COEF_CMD.
 * @else
 * @brief  ANC一个mixer的系数读写参数，对应命令#ANC_RW_MIXER_COEF_CMD
 * @endif
 */
typedef struct {
    td_u32 mixer_id; /* [in] see #anc_mixer_id */
    anc_mixer_coef mixer_coef; /* [in/out] */
} anc_mixer_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC all mixer's coefficient read-write param, used by #ANC_RW_ALL_MIXER_COEF_CMD.
 * @else
 * @brief  ANC所有mixer的系数读写参数，对应命令#ANC_RW_ALL_MIXER_COEF_CMD
 * @endif
 */
typedef struct {
    anc_mixer_coef mixer_coef[ANC_MIXER_ID_MAX]; /* [in/out] */
} anc_all_mixer_coef_rw_param;

/**
 * @if Eng
 * @brief  maximum coefficient of wfir
 * @else
 * @brief  wfir系数最大组数
 * @endif
 */
#define ANC_WFIR_COEFB_LENGTH 128

/**
 * @if Eng
 * @brief  Defines the anc wfir connect type.
 * @else
 * @brief  wfir模块连接方式
 * @endif
 */
typedef enum {
    ANC_WFIR_SERIAL_CONN_BQ = 0, /*!< @if Eng wfir serial connect with BQ.
                                       @else   wfir串联BQ，wfir在前 @endif */

    ANC_BQ_SERIAL_CONN_WFIR,     /*!< @if Eng BQ serial connect with wfir.
                                      @else   BQ串联wfir，wfir在后 @endif */

    ANC_WFIR_PARALLEL_CONN_BQ, /*!< @if Eng wfir parallel connect with BQ.
                                    @else   wfir并联BQ @endif */

    ANC_WFIR_CONN_TYPE_MAX,
} anc_wfir_conn_type;

/**
 * @if Eng
 * @brief  ANC wfir coefficient, used by #ANC_RW_WFIR_COEF_CMD.
 * @else
 * @brief  ANC wfir通道系数，对应命令#ANC_RW_WFIR_COEF_CMD
 * @endif
 */
typedef struct anc_wfir_coef {
    td_u32 filter_order; /* anc_wfir_cas_sel [0~127] wfir滤波器阶数 */
    td_u32 conn_type; /* anc_ch_filters_cas_sel #uapi_anc_wfir_conn_type */
    td_u32 work_channel; /* anc_wfir_ch_sel ANC_CH_FF0/FF1/FB/SPE */

    td_u32 lamda; /* anc_wfir_coefa S2.14 */
    td_u32 b[ANC_WFIR_COEFB_LENGTH]; /* anc_wfir_coefb S8.16 */
} anc_wfir_coef;

/**
 * @if Eng
 * @brief  ANC ramp coefficient, used by #ANC_RW_RAMP_COEF_CMD.
 * @else
 * @brief  ANC ramp模块系数，对应命令#ANC_RW_RAMP_COEF_CMD
 * @endif
 */
typedef struct anc_ramp_coef {
    td_u32 zero_time;
    td_u32 ramp_time;
    td_s32 ramp_lamda;
    td_u32 zero_sel;
    td_u32 zero_mode;
    td_u32 ramp_mode;
} anc_ramp_coef;

/**
 * @if Eng
 * @brief  ANC core entire coefficient read-write param, used by #ANC_RW_ALL_COEF_CMD.
 * @else
 * @brief  ANC core全量系数读写参数，对应命令#ANC_RW_ALL_COEF_CMD
 * @endif
 */
typedef struct {
    /* anc ff0 channel */
    anc_pga_coef pga_ff0;
    anc_limiter_coef limiter_ff0;
    anc_ch_bq_coef bq_ff0;

    /* anc ff1 channel */
    anc_pga_coef pga_ff1;
    anc_limiter_coef limiter_ff1;
    anc_ch_bq_coef bq_ff1;

    /* anc fb channel */
    anc_pga_coef pga_fb;
    anc_limiter_coef limiter_fb;
    anc_ch_bq_coef bq_fb;

    /* anc spe channel */
    anc_pga_coef pga_spe;
    anc_ch_bq_coef bq_spe;

    /* anc post_proc0/anc0 channel */
    anc_pga_coef pga_anc0;
    anc_limiter_coef limiter_anc0;

    /* anc post_proc1/anc1 channel */
    anc_pga_coef pga_anc1;
    anc_limiter_coef limiter_anc1;

    /* anc mixer */
    anc_mixer_coef mixer[ANC_MIXER_ID_MAX];
} anc_core_coef_rw_param;

#endif /* __ANC_COEF_H__ */
