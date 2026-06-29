/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: user api for peq audio effect
 * Author: @CompanyNameTag
 */

#ifndef __SOC_UAPI_AEF_PEQ_H__
#define __SOC_UAPI_AEF_PEQ_H__

#include "soc_uapi_aef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_aef_peq soc_uapi_aef_peq
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  Maximum PEQ band number.
 * @else
 * @brief  PEQ频带最大数量
 * @endif
 */
#define UAPI_SND_PEQ_BAND_MAX 10

/**
 * @if Eng
 * @brief  Audio PEQ effect filter type.
 * @else
 * @brief  音频PEQ音效滤波器类型
 * @endif
 */
typedef enum {
    UAPI_PEQ_FILTER_TYPE_HP = 0, /*!< @if Eng high-pass filter.
                                      @else   高通滤波器 @endif */
    UAPI_PEQ_FILTER_TYPE_LS = 1, /*!< @if Eng low-shelving filter.
                                      @else   低频搁架滤波器 @endif */
    UAPI_PEQ_FILTER_TYPE_PK = 2, /*!< @if Eng peaking filter.
                                      @else   峰值滤波器 @endif */
    UAPI_PEQ_FILTER_TYPE_HS = 3, /*!< @if Eng high-shelving filter.
                                      @else   高频搁架滤波器 @endif */
    UAPI_PEQ_FILTER_TYPE_LP = 4, /*!< @if Eng low-pass filter.
                                      @else   低通滤波器 @endif */
    UAPI_PEQ_FILTER_TYPE_MAX
} uapi_peq_filter_type;

/**
 * @if Eng
 * @brief  Audio PEQ effect band attribute.
 * @else
 * @brief  音频PEQ音效频带属性
 * @endif
 */
typedef struct {
    uapi_peq_filter_type filter_type; /*!< @if Eng filter type of the band.
                                           @else   某一频带的滤波器类型 @endif */

    uint32_t freq; /*!< @if Eng center frequency of the band, \n
                              HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000].
                      @else   某一频带的中心频率, \n
                              范围: HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000] @endif */

    uint32_t q; /*!< @if Eng Q value of the band, if 2.5,value is 25, \n
                           range:HS and LS[0.7, 1], PK[0.5, 10],HP and LP is fix to be 0.7
                   @else   某一频带的q值,如值为2.5,设置值为25, \n
                           范围: HS and LS[0.7, 1], PK[0.5, 10], 高低通内部固定为0.7 @endif */

    int32_t gain; /*!< @if Eng Gain of the band, if -2.125, value is -2125, \n
                              Gain ranging from -15 to 15 step by 0.125dB
                      @else   某一频带的增益,如值为-2.125,设置值为-2125,范围:[-15, 15],精度0.125dB @endif */
} uapi_snd_peq_band_attr;

/**
 * @if Eng
 * @brief  Audio PEQ effect attribute.
 * @else
 * @brief  音频PEQ音效属性
 * @endif
 */
typedef struct {
    uint32_t band_num; /*!< @if Eng PEQ band number, ranging from 5 to 10
                          @else   PEQ频带数量,范围:[5,10] @endif */
    uapi_snd_peq_band_attr band_attr[UAPI_SND_PEQ_BAND_MAX]; /*!< @if Eng PEQ band attribute
                                                                  @else   PEQ频带属性 @endif */
} uapi_snd_peq_attr;

/**
 * @if Eng
 * @brief  Max number of COM EQ channel
 * @else
 * @brief  播放通路中的通用EQ通道数量
 * @endif
 */
#define UAPI_SND_COM_EQ_CHN_NUM 2

/**
 * @if Eng
 * @brief  Max band number of COM EQ
 * @else
 * @brief  播放通路中的通用EQ的级数
 * @endif
 */
#define UAPI_SND_COM_EQ_CAS_NUM 22

/**
 * @if Eng
 * @brief  Max number of CALIB EQ channel
 * @else
 * @brief  播放通路中的校准EQ通道数量
 * @endif
 */
#define UAPI_SND_CALIB_EQ_CHN_NUM 2

/**
 * @if Eng
 * @brief  Max band number of CALIB EQ
 * @else
 * @brief  播放通路中的校准EQ的级数
 * @endif
 */
#define UAPI_SND_CALIB_EQ_CAS_NUM 10

/**
 * @if Eng
 * @brief  BQ coefficient.
 * @else
 * @brief  BQ系数
 * @endif
 */
typedef struct {
    uint32_t a1;
    uint32_t a2;
    uint32_t b0;
    uint32_t b1;
    uint32_t b2;
} uapi_audio_bq_coef;

/**
 * @if Eng
 * @brief  Limiter coefficient.
 * @else
 * @brief  Limiter系数
 * @endif
 */
typedef struct {
    uint32_t en;    /* [0,1] limiter使能信号。0：不使能，1：使能 */

    uint32_t alpha; /* [0,256] 参考信号平滑常数，越大越不平滑（q8精度） */

    /* [0,1] 是否使用limiter的输出作为参考信号和阈值比较。0：不使用（使用输入作为参考信号）， 1：使用 */
    uint32_t feedback_en;

    uint32_t threshold; /* [0,1073741823] limiter阈值，超过此值则会触发attack，否则触发release；（q24精度）。 */

    uint32_t upstep;   /* [0,8388607] limiter release时间常数，越大增益释放越快（q23精度） */
    uint32_t dnstep;   /* [0,8388607] limiter attack时间常数，越大增益压制越快（q23精度） */

    uint32_t min_gain; /* [0,8388608] limiter最小增益（q23精度） */
    uint32_t max_gain; /* [0,8388608] limiter最大增益（q23精度） */
} uapi_snd_peq_limiter_coef;

/**
 * @if Eng
 * @brief  COM EQ coefficient.
 * @else
 * @brief  COM EQ系数
 * @endif
 */
typedef struct {
    uint32_t bypass;
    uint32_t global_gain;
    uapi_audio_bq_coef bq[UAPI_SND_COM_EQ_CAS_NUM];
    uapi_snd_peq_limiter_coef limiter;
} uapi_snd_com_eq_coef;

/**
 * @if Eng
 * @brief  COM EQ read-write param.
 * @else
 * @brief  COM EQ读写参数
 * @endif
 */
typedef struct {
    uint32_t chn; /*!< @if Eng COM EQ channel, ranging from 0 to UAPI_SND_COM_EQ_CHN_NUM-1
                       @else   通用EQ通道数,范围:[0,UAPI_SND_COM_EQ_CHN_NUM) @endif */
    uapi_snd_com_eq_coef eq_coef;
} uapi_snd_com_eq_coef_rw_param;

/**
 * @if Eng
 * @brief  CALIB EQ coefficient.
 * @else
 * @brief  CALIB EQ系数
 * @endif
 */
typedef struct {
    uint32_t bypass;
    uint32_t global_gain;
    uapi_audio_bq_coef bq[UAPI_SND_CALIB_EQ_CAS_NUM];
    uapi_snd_peq_limiter_coef limiter;
} uapi_snd_calib_eq_coef;

/**
 * @if Eng
 * @brief  CALIB EQ read-write param.
 * @else
 * @brief  CALIB EQ读写参数
 * @endif
 */
typedef struct {
    uint32_t chn; /*!< @if Eng CALIB EQ channel, ranging from 0 to UAPI_SND_CALIB_EQ_CHN_NUM-1
                       @else   校准EQ通道数,范围:[0,UAPI_SND_CALIB_EQ_CHN_NUM) @endif */
    uapi_snd_calib_eq_coef eq_coef;
} uapi_snd_calib_eq_coef_rw_param;

/**
 * @if Eng
 * @brief  Audio PEQ command.
 * @else
 * @brief  音频PEQ音效接口命令
 * @endif
 */
typedef enum {
    /*!< @if Eng Read and Write PEQ param command.
         @else   获取和设置PEQ音效参数命令 @endif */
    UAPI_PEQ_RW_PARAM_CMD = uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, 0),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    UAPI_PEQ_RW_COM_EQ_CH0_COEF_CMD = uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, 1),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    UAPI_PEQ_RW_COM_EQ_CH1_COEF_CMD = uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, 2),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    UAPI_PEQ_RW_CALIB_EQ_CH0_COEF_CMD = uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, 3),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    UAPI_PEQ_RW_CALIB_EQ_CH1_COEF_CMD = uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, 4),
} uapi_peq_cmd;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_AEF_PEQ_H__ */
