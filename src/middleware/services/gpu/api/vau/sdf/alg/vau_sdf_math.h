/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf math head file
 */

#ifndef _VAU_SDF_MATH_H_
#define _VAU_SDF_MATH_H_

#include <math.h>
#include "vau_sdf_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef union {
    unsigned value;
    td_float fvalue;

    struct {
        unsigned fraction : 23;
        unsigned exponent : 8;
        unsigned sign : 1;
    };
} float32;

td_u16 vau_sdf_conv_fp32_fp16(td_float f);
td_float24 vau_sdf_conv_fp16_fp24(const td_u16 f);
td_void vau_sdf_deg_to_rad(td_float input_deg, td_float *out_rad);
td_void vau_sdf_clamp_int(td_s32 input_data, td_s32 min_val, td_s32 max_val, td_s32 *out_data);
td_void vau_sdf_sub_vec3_vec3(ext_vau_vec3 *input1_vec3, ext_vau_vec3 *input2_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_div_vec3_sca3(ext_vau_vec3 *input_vec3, td_float input_sca, ext_vau_vec3 *out_vec3);
td_void vau_sdf_mul_vec3_sca3(ext_vau_vec3 *input_vec3, td_float input_sca, ext_vau_vec3 *out_vec3);
td_void vau_sdf_add_vec3_vec3(ext_vau_vec3 *input1_vec3, ext_vau_vec3 *input2_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_norm2_vec3(ext_vau_vec3 *input_vec3, td_float *out_vec3);
td_void vau_sdf_normal_vec3(ext_vau_vec3 *input_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_dot_vec3_vec3(ext_vau_vec3 *input1_vec3, ext_vau_vec3 *input2_vec3, td_float *out_data);
td_void vau_sdf_norm_vec3(ext_vau_vec3 *input1_vec3, td_float *out_data);
td_void vau_sdf_cross_vec3_vec3(ext_vau_vec3 *input1_vec3, ext_vau_vec3 *input2_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_mul_mat3_vec3(ext_vau_matrix *input_matrix, ext_vau_vec3 *input_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_mul_mat3_mat3(ext_vau_matrix *input1_matrix,
    ext_vau_matrix *input2_matrix, ext_vau_matrix *out_matrix);
td_void vau_sdf_conv_vec3_fp16(ext_vau_vec3 *input1_vec3, ext_vau_vec3 *out_vec3);
td_void vau_sdf_matrix_mul_angle(ext_vau_matrix *input_matrix, ext_vau_vec3 *rot_angle, ext_vau_matrix *out_matrix);
td_void vau_sdf_set_vec3(td_float input1_data, td_float input2_data, td_float input3_data, ext_vau_vec3 *out_data);
td_void vau_sdf_matrix_to_quate(ext_vau_matrix *input_matrix, td_float out_quante[4]); // num:4
td_void vau_sdf_quat_matrix(td_float input_quater[4], ext_vau_matrix *out_matrix); // num:4
 // num:4
td_void vau_sdf_quat_slerp(td_float input1_quater[4], td_float input2_quater[4], td_float step, td_float out_quater[4]);
td_void vau_sdf_matrix_ident(ext_vau_matrix *matrix);
td_void vau_sdf_sub_vec3_fp32(td_float* input1_vec3, td_float* input2_vec3, td_float *out_vec3);
td_void vau_sdf_sub_vec3_fp16(td_u16* input1_vec3, td_u16* input2_vec3, td_u16 *out_vec3);
td_void vau_sdf_sub_vec3_u8(td_u8* input1_vec3, td_u8* input2_vec3, td_u8 *out_vec3);
td_void vau_sdf_inter_vec3_u8(td_u8* input1_vec3, td_u8* input2_vec3, td_float inter_step, td_u8 *out_vec3);
td_void vau_sdf_inter_vec3_fp32(td_float* input1_vec3, td_float* input2_vec3, td_float inter_step, td_float *out_vec3);
td_void vau_sdf_vec3_fp32_fp16(td_float* input_vec3, td_u16 *out_vec3);
td_void vau_sdf_sub_fp16(td_u16 input1_value, td_u16 input2_value, td_u16 *ret_value);
td_void vau_sdf_inter_fp16(td_u16 input1_value, td_u16 input2_value, td_float inter_step, td_u16 *ret_value);
td_void vau_sdf_inter_vec3_fp16(td_u16* input1_vec3, td_u16* input2_vec3, td_float inter_step, td_u16 *out_vec3);
td_void vau_sdf_max(td_s32 input1_value, td_s32 input2_value, td_s32 *output_value);
td_void vau_sdf_min(td_s32 input1_value, td_s32 input2_value, td_s32 *output_value);
td_void vau_sdf_fp16_sub_fp32(td_u16 input1_value, td_u16 input2_value, td_float *out_value);
td_void vau_sdf_svd(ext_vau_matrix_d* w_matrix, ext_vau_matrix* out_u, ext_vau_matrix* out_vt);
td_void vau_sdf_trans_mat3_fp16(ext_vau_matrix* a, ext_vau_matrix* at);
td_void vau_sdf_det_mat3(ext_vau_matrix* a, td_float* result);
td_void vau_sdf_vec3_sub(const double *a, const double *b, double *r);
td_void vau_sdf_vec3_dot(const td_float a[DIM_3], const td_float b[DIM_3], td_float* result);
td_void vau_sdf_vec3_cross(const double a[DIM_3], const double b[DIM_3], td_float r[DIM_3]);
td_void vau_sdf_vec3_norm(const td_float a[DIM_3], td_float* norm);
td_void vau_sdf_mean3(const ext_vau_matrix* arr, td_float result[DIM_3]);
td_void solve_quartic_companion(double temp_bcde[4], double z[4], td_s32* num); // num:4
td_void vau_sdf_calc_mean_2d(const td_float *points, const td_u16 *indices, td_u16 count,
    td_float *meanX, td_float *meanY);
td_void sph_intersect(ext_vau_vec3* ro, ext_vau_vec3* rd, ext_vau_vec4* sph_info, td_float* result);
td_void vau_sdf_quat_slerp_opt(td_float* input_quater, td_float step, td_float* out_quater);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif