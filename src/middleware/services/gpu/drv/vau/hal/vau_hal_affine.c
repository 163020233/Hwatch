/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau adp affine file
 */

#include "vau_hal_affine.h"
#include "vau_dfx.h"

#define VAU_AFFINE_FLOAT_EXP     0.000001
#define VAU_AFFINE_SHIFT         10
#define VAU_AFFINE_SHIFT_VALUE   (1 << VAU_AFFINE_SHIFT)

#define VAU_AFFINE_DIRECT   0
#define VAU_AFFINE_BILINEAR 1

STATIC td_void vau_affine_get_inverse_matrix(const drv_vau_matrix pos_m, drv_vau_matrix inv_m, td_u32 len)
{
    if (len != VAU_AFFFINE_MATRIX_LEN) {
        return;
    }

    td_float det, delta;
    td_float det00, det01, det02;
    td_float det10, det11, det12;
    td_float det20, det21, det22;

    det00 = (pos_m[1][1] * pos_m[2][2] - pos_m[2][1] * pos_m[1][2]); /* 2:index */
    det01 = (pos_m[1][0] * pos_m[2][2] - pos_m[2][0] * pos_m[1][2]); /* 2:index */
    det02 = (pos_m[1][0] * pos_m[2][1] - pos_m[2][0] * pos_m[1][1]); /* 2:index */

    det10 = (pos_m[0][1] * pos_m[2][2] - pos_m[2][1] * pos_m[0][2]); /* 2:index */
    det11 = (pos_m[0][0] * pos_m[2][2] - pos_m[2][0] * pos_m[0][2]); /* 2:index */
    det12 = (pos_m[0][0] * pos_m[2][1] - pos_m[2][0] * pos_m[0][1]); /* 2:index */

    det20 = (pos_m[0][1] * pos_m[1][2] - pos_m[1][1] * pos_m[0][2]); /* 2:index */
    det21 = (pos_m[0][0] * pos_m[1][2] - pos_m[1][0] * pos_m[0][2]); /* 2:index */
    det22 = (pos_m[0][0] * pos_m[1][1] - pos_m[1][0] * pos_m[0][1]); /* 2:index */

    det = pos_m[2][0] * det20 - pos_m[2][1] * det21 + pos_m[2][2] * det22; /* 2:index */
    if ((det > -VAU_AFFINE_FLOAT_EXP) && (det < VAU_AFFINE_FLOAT_EXP)) {
        gpu_dbg("det is zero, rot matrix is invalid!");
        det = VAU_AFFINE_FLOAT_EXP;
    }

    inv_m[0][0] =  (det00 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[0][1] = -(det10 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[0][2] =  (det20 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m[1][0] = -(det01 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[1][1] =  (det11 * VAU_AFFINE_SHIFT_VALUE) / det;
    inv_m[1][2] = -(det21 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m[2][0] =  (det02 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m[2][1] = -(det12 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m[2][2] =  (det22 * VAU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    if ((fabs(pos_m[0][0] * pos_m[0][0] + pos_m[0][1] * pos_m[0][1] - 1.0) <= (1e-6)) &&
        (fabs(pos_m[1][0] * pos_m[1][0] + pos_m[1][1] * pos_m[1][1] - 1.0) <= (1e-6))) {
        /* rotate center position compensation */
        delta = (td_float)VAU_AFFINE_SHIFT_VALUE;
        inv_m[0][2] += ((inv_m[0][0] + inv_m[0][1]) - delta) / 2.0f; /* 2:index */
        inv_m[1][2] += ((inv_m[1][0] + inv_m[1][1]) - delta) / 2.0f; /* 2:index */
        inv_m[2][2] += (inv_m[2][0] + inv_m[2][1]) / 2.0f;           /* 2:index */
    }

    return;
}

td_void vau_hal_affine_config(vau_hardware_node *node, const drv_vau_surface *src_surface,
                              const drv_vau_surface *dst_surface)
{
    vau_affine_cfg hal_cfg = {0};

    hal_cfg.src_width  = src_surface->rect.width;
    hal_cfg.src_height = src_surface->rect.height;

    if (src_surface->attr.affine.is_inv) {
        for (int i = 0; i < 3; i++) { /* matrix size is 3 x 3 */
            for (int j = 0; j < 3; j++) { /* matrix size is 3 x 3 */
                hal_cfg.inv_m[i][j] = src_surface->attr.affine.matrix[i][j];
            }
        }
    } else {
        vau_affine_get_inverse_matrix(src_surface->attr.affine.matrix, hal_cfg.inv_m, VAU_AFFFINE_MATRIX_LEN);
    }

    hal_cfg.dst_width  = dst_surface->rect.width;
    hal_cfg.dst_height = dst_surface->rect.height;

    /* affine support ARGB8888 or RGB888 */
    hal_cfg.src_fmt = (src_surface->fmt == DRV_GFX_FMT_ARGB8888) ? DRV_GFX_FMT_ARGB8888 : DRV_GFX_FMT_RGB888;
    hal_cfg.dst_fmt = (dst_surface->fmt == DRV_GFX_FMT_ARGB8888) ? DRV_GFX_FMT_ARGB8888 : DRV_GFX_FMT_RGB888;
    hal_cfg.transform_mode = VAU_AFFINE_BILINEAR;
    hal_cfg.pattern_mode = (td_u32)src_surface->attr.pattern.mode;
    hal_cfg.pattern_argb = (td_u32)src_surface->attr.pattern.argb;

    vau_hal_node_set_affine_info(node, &hal_cfg, VAU_AFFINE_SHIFT);
}
