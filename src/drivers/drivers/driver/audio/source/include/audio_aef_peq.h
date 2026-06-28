/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: api for peq audio effect
 * Author: @CompanyNameTag
 */

#ifndef __AUDIO_AEF_PEQ_H__
#define __AUDIO_AEF_PEQ_H__

#include "audio_aef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Maximum PEQ band number.
 * @else
 * @brief  PEQ频带最大数量
 * @endif
 */
#define PEQ_BAND_MAX 10

/**
 * @if Eng
 * @brief  Audio PEQ effect filter type.
 * @else
 * @brief  音频PEQ音效滤波器类型
 * @endif
 */
typedef enum {
    PEQ_FILTER_TYPE_HP = 0, /*!< @if Eng high-pass filter.
                                 @else   高通滤波器 @endif */
    PEQ_FILTER_TYPE_LS = 1, /*!< @if Eng low-shelving filter.
                                 @else   低频搁架滤波器 @endif */
    PEQ_FILTER_TYPE_PK = 2, /*!< @if Eng peaking filter.
                                 @else   峰值滤波器 @endif */
    PEQ_FILTER_TYPE_HS = 3, /*!< @if Eng high-shelving filter.
                                 @else   高频搁架滤波器 @endif */
    PEQ_FILTER_TYPE_LP = 4, /*!< @if Eng low-pass filter.
                                 @else   低通滤波器 @endif */
    PEQ_FILTER_TYPE_MAX
} peq_filter_type;

/**
 * @if Eng
 * @brief  Audio PEQ effect band attribute.
 * @else
 * @brief  音频PEQ音效频带属性
 * @endif
 */
typedef struct {
    peq_filter_type filter_type; /*!< @if Eng filter type of the band.
                                      @else   某一频带的滤波器类型 @endif */

    td_u32 freq; /*!< @if Eng center frequency of the band, \n
                              HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000].
                      @else   某一频带的中心频率, \n
                              范围: HP and LS[20, 4000], PK[20, 22000], HS[4000, 22000], LP[50, 22000] @endif */

    td_u32 q; /*!< @if Eng Q value of the band, if 2.5,value is 25, \n
                           range:HS and LS[0.7, 1], PK[0.5, 10],HP and LP is fix to be 0.7
                   @else   某一频带的q值,如值为2.5,设置值为25, \n
                           范围: HS and LS[0.7, 1], PK[0.5, 10], 高低通内部固定为0.7 @endif */

    td_s32 gain; /*!< @if Eng Gain of the band, if -2.125, value is -2125, \n
                              Gain ranging from -15 to 15 step by 0.125dB
                      @else   某一频带的增益,如值为-2.125,设置值为-2125,范围:[-15, 15],精度0.125dB @endif */
} peq_band_attr;

/**
 * @if Eng
 * @brief  Audio PEQ effect attribute.
 * @else
 * @brief  音频PEQ音效属性
 * @endif
 */
typedef struct {
    td_u32 band_num; /*!< @if Eng PEQ band number, ranging from 5 to 10
                          @else   PEQ频带数量,范围:[5,10] @endif */
    peq_band_attr band_attr[PEQ_BAND_MAX]; /*!< @if Eng PEQ band attribute
                                                @else   PEQ频带属性 @endif */
} peq_attr;

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
    PEQ_RW_PARAM_CMD = aef_cmd(AEF_TYPE_PEQ, 0),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    PEQ_RW_COM_EQ_CH0_COEF_CMD = aef_cmd(AEF_TYPE_PEQ, 1),

    /*!< @if Eng Read and Write COM EQ biquad filter coef command.
         @else   获取和设置通用EQ滤波器系数命令 @endif */
    PEQ_RW_COM_EQ_CH1_COEF_CMD = aef_cmd(AEF_TYPE_PEQ, 2),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    PEQ_RW_CALIB_EQ_CH0_COEF_CMD = aef_cmd(AEF_TYPE_PEQ, 3),

    /*!< @if Eng Read and Write calibration EQ biquad filter coef command.
         @else   获取和设置校准EQ滤波器系数命令 @endif */
    PEQ_RW_CALIB_EQ_CH1_COEF_CMD = aef_cmd(AEF_TYPE_PEQ, 4),
} peq_cmd;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AUDIO_AEF_PEQ_H__ */
