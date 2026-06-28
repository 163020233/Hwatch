/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: Audio vqe functional macro define
 * Author: audio
 * Create: 2020
 * Update: 2020
 */
#ifndef AUDIO_VQE_FUNC_MACRO_H
#define AUDIO_VQE_FUNC_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define audio_vqe_round(val) (((val) < 0) ? ((val)-0.5f) : ((val) + 0.5f))

#ifndef round_float
#define round_float(x) ((x) > 0 ? ((x) + 0.5) : ((x) - 0.5))
#endif

#ifndef mem_align_8
#define mem_align_8(base, offset) (((((base) + 7) + (offset)) >> 3) << 3) /* 8 byte algin */
#endif

#ifndef audio_vqe_abs
#define audio_vqe_abs(a) (((a) > (0)) ? (a) : (-(a)))
#endif

#ifdef __cplusplus
}
#endif

#endif
