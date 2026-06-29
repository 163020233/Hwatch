/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: api for drc audio effect
 * Author: @CompanyNameTag
 */

#ifndef __AUDIO_AEF_DRC_H__
#define __AUDIO_AEF_DRC_H__

#include "audio_aef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Audio DRC effect attribute.
 * @else
 * @brief  音频DRC音效属性
 * @endif
 */
typedef struct {
    td_s16 drc_target_gain; /*!< @if Eng DRC Target gain,
                                     range: [-40, 80], Q2, Corresponding range: [-10dB, 20dB]
                                  @else DRC 目标增益,
                                     范围: [-40, -80], Q2, 对应范围: [-10dB, 20dB]
                                  @endif */

    td_s16 drc_max_lvl; /*!< @if Eng Maximum output energy,
                                 range: [-80, 0], Q2, Corresponding range: [-20dB, 0dB]
                              @else 最大输出能量,
                                 范围: [-80, 0], Q2, 对应范围: [-20dB, 0dB]
                              @endif */

    td_s16 drc_silence_threshold; /*!< @if Eng DRC Noise Floor Compression Threshold, default: -240
                                           range: [-360, -120], Q2, Corresponding range: [-90dB, -30dB]
                                        @else DRC底噪压制阈值, 默认: -240
                                           范围: [-360, -120], Q2, 对应范围: [-90dB, -30dB]
                                        @endif */

    td_s16 transit_band_width; /*!< @if Eng Gain transition bandwidth, default: 60
                                        range: [0, 160], Q2, Corresponding range: [0dB, 40dB]
                                     @else 增益过渡带宽度, 默认: 60
                                        范围: [0, 160], Q2, 对应范围: [0dB, 40dB]
                                     @endif */

    td_s16 silence_gain; /*!< @if Eng Noise suppression strength, default: -40
                                  range: [-120, 0], Q2, Corresponding range: [-30dB, 0dB]
                               @else 底噪压制力度, 默认: -40
                                  范围: [-120, 0], Q2, 对应范围: [-30dB, 0dB]
                               @endif */
} drc_attr;

/**
 * @if Eng
 * @brief  Audio DRC command.
 * @else
 * @brief  音频DRC音效接口命令
 * @endif
 */
typedef enum {
    DRC_RW_PARAM_CMD = aef_cmd(AEF_TYPE_DRC, 0), /*!< @if Eng Read and Write DRC param command.
                                                      @else   获取和设置DRC音效参数命令 @endif */
} drc_cmd;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AUDIO_AEF_DRC_H__ */
