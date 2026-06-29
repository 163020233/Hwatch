/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: afe hal
 * Author: audio
 */

#ifndef __HAL_AFE_ADC_H__
#define __HAL_AFE_ADC_H__

#include "hal_afe_sys.h"
#include "ai_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define HAL_AFE_ADC_MUTE_GAIN (-120)

td_s32 hal_afe_adc_init(td_void);

td_void hal_afe_adc_deinit(td_void);

td_s32 hal_afe_adc_get_chan_sel(td_u32 *chan_sel, const audio_pcm_format *pcm_format,
    const audio_ai_attr *attr, audio_scene scene);

td_s32 hal_afe_adc_open(td_u32 chan_sel, const audio_pcm_format *pcm_format,
    const ai_adc_attr *adc_attr);

td_s32 hal_afe_adc_close(td_u32 chan_sel);

td_s32 hal_afe_adc_set_enable(td_u32 chan_sel, td_bool enable);

td_s32 hal_afe_adc_set_gain(td_u32 chan_sel, td_s32 integer, td_s32 decimal);

td_s32 hal_afe_adc_get_gain(td_u32 chan_sel, td_s32 *integer, td_s32 *decimal);

td_s32 hal_afe_adc_set_pcm_attr(audio_sample_rate sample_rate, audio_bit_depth bit_depth);

td_s32 hal_afe_adc_set_vol_cfg(td_u32 chan_sel, const ai_adc_attr *adc_attr, const audio_volume_cfg *vol_cfg);

td_void hal_afe_adc_set_mute(td_u32 chan_sel, td_bool mute);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __HAL_AFE_ADC_H__ */
