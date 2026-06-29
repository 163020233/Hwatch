/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: audio vqe common basicop vec
 * Author: audio
 * Create: 2020
 * Update: 2020
 */
#ifndef AUDIO_VQE_COMMON_BASICOP_VEC_H
#define AUDIO_VQE_COMMON_BASICOP_VEC_H

#include "audio_vqe_common_api.h"
#include "audio_vqe_common_basicop.h"

#define IMEDIA_M_PI 3.14159265358979323846f   /* pi */
#define IMEDIA_M_PI_2 1.57079632679489661923f /* pi/2 */

imedia_void audio_vqe_common_vec_set_int8(imedia_int8 *vec, imedia_int32 num, imedia_int8 val);

imedia_void audio_vqe_common_vec_set_int16(imedia_int16 *svec, imedia_int32 num, imedia_int16 sval);

imedia_void audio_vqe_common_vec_set_int32(imedia_int32 *ivec, imedia_int32 num, imedia_int32 ival);

imedia_void audio_vqe_common_vec_set_float(imedia_float *vec, imedia_int32 num, imedia_float val);

imedia_void audio_vqe_common_vec_copy_int8_to_int8(const imedia_int8 *vec_src, imedia_int32 num, imedia_int8 *vec_dst);

imedia_void audio_vqe_common_vec_copy_int16_to_int16(const imedia_int16 *svec_src, imedia_int32 num,
    imedia_int16 *svec_dst);
imedia_void audio_vqe_common_vec_copy_uint16_to_uint16(const imedia_uint16 *svec_src, imedia_int32 num,
    imedia_uint16 *svec_dst);
imedia_void audio_vqe_common_vec_copy_int32_to_int16(imedia_int32 *s_in, imedia_int32 i_len, imedia_int16 *s_out);

imedia_void audio_vqe_common_vec_copy_int32_to_int32(imedia_int32 *pi_in, imedia_int32 i_len, imedia_int32 *pi_out);

imedia_void audio_vqe_common_vec_copy_float_to_float(const imedia_float *s_in,
    imedia_int32 i_len, imedia_float *s_out);

imedia_int32 audio_vqe_common_float_to_int32(imedia_float var);

imedia_void audio_vqe_common_vec_float_to_int32(imedia_float *var, imedia_int32 i_len_multiple8, imedia_int32 *iout);
#endif
