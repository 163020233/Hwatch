/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: common alg  functionfunc declare
 * Author:audio
 * Create:2014
 * Update:2019-12-17
 */

#ifndef IMEDIA_COMMON_API_H
#define IMEDIA_COMMON_API_H

#include "audio_vqe_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* VEC opt  Set */
extern imedia_void audio_vqe_common_vec_set_int8(imedia_int8 *vec, imedia_int32 num, imedia_int8 val);
extern imedia_void audio_vqe_common_vec_set_int16(imedia_int16 *svec, imedia_int32 num, imedia_int16 sval);
extern imedia_void audio_vqe_common_vec_set_int32(imedia_int32 *ivec, imedia_int32 num, imedia_int32 ival);
extern imedia_void audio_vqe_common_vec_set_float(imedia_float *vec, imedia_int32 num, imedia_float val);

/* VEC opt  copy */
extern imedia_void audio_vqe_common_vec_copy_int8_to_int8(const imedia_int8 *vec_src, imedia_int32 num,
    imedia_int8 *vec_dst);
extern imedia_void audio_vqe_common_vec_copy_int16_to_int16(const imedia_int16 *svec_src, imedia_int32 num,
    imedia_int16 *svec_dst);
extern imedia_void audio_vqe_common_vec_copy_int32_to_int16(imedia_int32 *s_in, imedia_int32 i_len,
    imedia_int16 *s_out);
extern imedia_void audio_vqe_common_vec_copy_int32_to_int32(imedia_int32 *pi_in, imedia_int32 i_len,
    imedia_int32 *pi_out);

extern imedia_void audio_vqe_common_vec_copy_float_to_float(const imedia_float *s_in, imedia_int32 i_len,
    imedia_float *s_out);

extern imedia_void audio_vqe_common_vec_float_to_int32(imedia_float *var, imedia_int32 i_len_multiple8,
    imedia_int32 *iout);

imedia_int16 audio_vqe_common_fft_pro_float(imedia_int16 s_fft_order, imedia_float *fft_in_buf,
    imedia_float *fft_out_buf, imedia_int16 q_in, imedia_void *scratch_buf);
imedia_int16 audio_vqe_common_fft_pro_float_q0(imedia_int16 s_fft_order, imedia_float *fft_in_buf,
    imedia_float *fft_out_buf);
imedia_void audio_vqe_common_ifft_pro(imedia_float *pf_in, imedia_float *pf_out,
    imedia_int32 i_fft_len, imedia_void *scratch_buf);

#ifdef __cplusplus
}
#endif

#endif
