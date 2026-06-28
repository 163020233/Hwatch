/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: afe hal
 * Author: audio
 */

#ifndef __HAL_AFE_DAC_H__
#define __HAL_AFE_DAC_H__

#include "hal_afe_sys.h"
#include "ao_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define HAL_AFE_DAC_MUTE_GAIN (-120)

td_s32 hal_afe_dac_init(td_void);

td_void hal_afe_dac_deinit(td_void);

td_s32 hal_afe_dac_get_chan_sel(td_u32 *chan_sel, td_u32 channels,
    const ao_out_port_attr *port_attr, audio_scene scene);

td_s32 hal_afe_dac_open(td_u32 chan_sel, const audio_pcm_format *pcm_attr,
    const ao_out_port_attr *port_attr);

td_s32 hal_afe_dac_close(td_u32 chan_sel);

td_s32 hal_afe_dac_set_enable(td_u32 chan_sel, td_bool enable);

td_s32 hal_afe_dac_set_gain(td_u32 chan_sel, td_s32 integer, td_s32 decimal);

td_s32 hal_afe_dac_set_post_ha_gain(td_s32 integer, td_s32 decimal);

td_s32 hal_afe_dac_set_pcm_attr(audio_sample_rate sample_rate, audio_bit_depth bit_depth);

td_s32 hal_afe_dac_set_cali_coef(td_u32 chan_sel, const audio_spk_cali *flt_coefs);

td_void hal_afe_dac_close_all(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __HAL_AFE_DAC_H__ */
