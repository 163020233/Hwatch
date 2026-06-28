/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf type file
 */

#ifndef __TEST_SDF_TYPE_H__
#define __TEST_SDF_TYPE_H__

#include "soc_dpu_api.h"
#include "soc_vau_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define DIM_3 3
#ifndef EPSILON
#define EPSILON 1e-5
#endif

#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923
#endif

typedef struct {
    double matrix[3][3];
} ext_vau_matrix_d;

typedef struct {
    double re;
    double im;
} ext_vau_complex;

typedef struct {
    ext_vau_matrix r;
    ext_vau_vec3 t;
} ext_vau_rt;

typedef struct {
    ext_vau_rt rts[4];
    td_u16 count;
} ext_vau_rt_set;

typedef struct {
    td_float fx, fy, cx, cy;
} ext_vau_camint;

typedef struct {
    double val;
    double vec[DIM_3];
} ext_vau_eigpair;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif

