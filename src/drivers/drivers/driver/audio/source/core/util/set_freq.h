/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dsp frequency get/set implement.
 * Author: audio
 */

#ifndef __SET_FREQ_H__
#define __SET_FREQ_H__

#include "audio_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 set_dsp_freq(td_u32 freq);
td_u32 get_dsp_freq(td_void);

td_void set_dsp_clock_type(audio_clock_type type);
audio_clock_type get_dsp_clock_type(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __SET_FREQ_H__ */

