/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: iMedia common basicop_esti
 * Author:audio
 * Create:2014
 * Update:2019-12-17
 */

#ifndef AUDIO_VQE_COMMON_BASICOP_H
#define AUDIO_VQE_COMMON_BASICOP_H

#include "audio_vqe_typedef.h"
#include "audio_vqe_func_macro.h"

#ifdef __AUDIO_NEON_OPTIMIZE__
#include <arm_neon.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef MAX_32
#define MAX_32 (0x7fffffffL)
#endif
#ifndef MIN_32
#define MIN_32 ((imedia_int32)0x80000000L)
#endif
#ifndef MAX_16
#define MAX_16 ((imedia_int16)0x7fff)
#endif
#ifndef MIN_16
#define MIN_16 ((imedia_int16)0x8000)
#endif

#define LW_SIGN ((imedia_int32)0x80000000) /* sign bit */
#define AUDIO_VQE_COMMON_EPS 2.220446049250313e-16

#define TV_MAX_16 ((imedia_int16)0x7fff)
#define TV_MIN_16 ((imedia_int16)0x8000)

#define TV_MAX_32 ((imedia_int32)0x7fffffffL)
#define TV_MIN_32 ((imedia_int32)0x80000000L)

#define TV_MAX_64 ((imedia_int64)0x7fffffffffffffff)
#define TV_MIN_64 ((imedia_int64)0x8000000000000000)

/* true/false macro definition */
#define IMEDIA_FALSE 0
#define IMEDIA_TRUE 1

extern imedia_int16 audio_vqe_saturate(imedia_int32 sw_var);
extern imedia_int16 audio_vqe_round_l(imedia_int32 l_var1);
extern imedia_int16 audio_vqe_abs_s(imedia_int16 shw_var1);
extern imedia_int32 audio_vqe_multip_l(imedia_int16 shw_var1, imedia_int16 shw_var2);
extern imedia_int16 audio_vqe_multip_s(imedia_int16 var1, imedia_int16 var2);
extern imedia_int16 audio_vqe_norm_l(imedia_int32 l_var1);

extern imedia_int32 audio_vqe_add_l(imedia_int32 sw_var1, imedia_int32 sw_var2);
extern imedia_int32 audio_vqe_sub_l(imedia_int32 l_var1, imedia_int32 l_var2);
extern imedia_int32 audio_vqe_shl_l(imedia_int32 sw_var1, imedia_int16 shw_var2);
extern imedia_int32 audio_vqe_shr_l(imedia_int32 sw_var1, imedia_int16 shw_var2);
extern imedia_int32 audio_vqe_abs_l(imedia_int32 sw_var1);

extern imedia_int64 audio_vqe_shl64_s(imedia_int64 var, imedia_int16 s_q);
extern imedia_int64 audio_vqe_shr64_s(imedia_int64 var, imedia_int16 s_q);
extern imedia_int64 audio_vqe_add64_s(imedia_int64 var1, imedia_int64 var2);
extern imedia_int32 audio_vqe_saturate64(imedia_int64 var1);

extern imedia_float32 audio_vqe_sinfc(imedia_float32 varin);

#ifdef __AUDIO_NEON_OPTIMIZE__
float32x4_t audio_vqe_sin_f32x4(float32x4_t varin);
#endif

extern imedia_float32 audio_vqe_cosfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_asinfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_acosfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_atanfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_atan2fc(imedia_float32 varin_y, imedia_float32 varin_x);

extern imedia_float32 audio_vqe_expfc(imedia_float32 varin);

#ifdef __AUDIO_NEON_OPTIMIZE__
extern float32x4_t audio_vqe_exp_f32x4(float32x4_t f32x4_varin);
#endif

extern imedia_float32 audio_vqe_logfc(imedia_float32 varin);

#ifdef __AUDIO_NEON_OPTIMIZE__
extern float32x4_t audio_vqe_ln_f32x4(float32x4_t f32x4_varin);
#endif

extern imedia_float32 audio_vqe_log10fc(imedia_float32 varin);

#ifdef __AUDIO_NEON_OPTIMIZE__
extern float32x4_t audio_vqe_log10_f32x4(float32x4_t f32x4_varin);
#endif

extern imedia_float32 audio_vqe_powerfc(imedia_float32 varin, imedia_float32 var_n);

#ifdef __AUDIO_NEON_OPTIMIZE__
extern float32x4_t audio_vqe_power_f32x4(float32x4_t f32x4_varin, float32x4_t f32x4_var_n);
#endif

extern imedia_float32 audio_vqe_floorfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_ceilfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_fabsfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_fmodfc(imedia_float32 varin_x, imedia_float32 varin_y);

extern imedia_float32 audio_vqe_modfc(imedia_float32 varin, imedia_int32 *i_tmp);

extern imedia_float32 audio_vqe_sqrtfc(imedia_float32 varin);

extern imedia_float32 audio_vqe_inv_sqrtfc(imedia_float32 varin);

extern imedia_void audio_vqe_division_uint(imedia_uint32 n, imedia_uint32 p, imedia_uint32 *quotient,
    imedia_uint32 *remainder);
extern imedia_void audio_vqe_division_int(imedia_int32 numerator, imedia_int32 denominator, imedia_int32 *quotient,
    imedia_int32 *remainder);

#ifdef __AUDIO_NEON_OPTIMIZE__
__attribute((always_inline)) inline imedia_void audio_vqe_inv_f32x4(float32x4_t *inv, float32x4_t var)
{
    *inv = vrecpeq_f32(var);
    *inv = vmulq_f32(vrecpsq_f32(var, *inv), *inv);
    *inv = vmulq_f32(vrecpsq_f32(var, *inv), *inv);
}

static inline float32x4_t audio_vqe_inv_sqrt_f32x4(float32x4_t l_num2)
{
    float32x4_t vf32x4_demon, vf32x4_tmp, vf32x4_tmp1;
    vf32x4_demon = vrsqrteq_f32(l_num2);

    vf32x4_tmp1 = vmulq_f32(vf32x4_demon, vf32x4_demon);
    vf32x4_tmp = vrsqrtsq_f32(vf32x4_tmp1, l_num2);
    vf32x4_demon = vmulq_f32(vf32x4_demon, vf32x4_tmp);

    vf32x4_tmp1 = vmulq_f32(vf32x4_demon, vf32x4_demon);
    vf32x4_tmp = vrsqrtsq_f32(vf32x4_tmp1, l_num2);
    vf32x4_demon = vmulq_f32(vf32x4_demon, vf32x4_tmp);

    return vf32x4_demon;
}

static inline float32x4_t audio_vqe_sqrt_f32x4(float32x4_t var)
{
    float32x4_t res = audio_vqe_inv_sqrt_f32x4(var);
    audio_vqe_inv_f32x4(&res, res);

    return res;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif

#endif

#endif /* end of med_com_etsi_op.h */
