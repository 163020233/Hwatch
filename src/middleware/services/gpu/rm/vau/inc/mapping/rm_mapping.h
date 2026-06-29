/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: rm mapping
 */

#ifndef GPU_API_RM_RM_MAPPING_H
#define GPU_API_RM_RM_MAPPING_H

#include "vau_rm_api.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/
#define A11 0
#define A12 1
#define A13 2
#define A21 3
#define A22 4
#define A23 5
#define A31 6
#define A32 7
#define A33 8
#define SRC_MAP 1
#define MATRIX_ROW 3
#define MATRIX_COL 4
#define TRANSFORM_MATRIX_SIZE_MAX 9
#define TRANSFORM_MATRIX_SHIFT 10
#define MY_PI 3.14159265357
/*-------------------------------- struct define ------------------------------------------*/
typedef struct {
    ext_vau_matrix m;
    ext_vau_matrix v;
    ext_vau_matrix p;
    ext_vau_matrix mvp;
} vau_mvp_param;

typedef struct {
    td_s32 min;
    td_s32 middle;
    td_s32 max;
} point_statistics_value;

typedef struct {
    td_float matrix[3][3];
} vau_matrix3;

typedef struct {
    td_float left;
    td_float right;
    td_float top;
    td_float bottom;
} pixe_border;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
} drv_rect;
/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void rm_product_mapping_matrix(const ext_vau_surface *api_src, drv_vau_surface *drv_src);
td_void rm_product_3d_mapping_info(const ext_vau_surface *api_src,
    const ext_vau_surface *api_dst, drv_vau_surface *drv_src);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_API_RM_RM_MAPPING_H */
