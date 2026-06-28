/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu affine process
 */
#include "dpu_affine.h"
#include <math.h>
#include "dpu_dfx.h"

#define HAL_DPU_MATRIX_ARRAY_LENGTH 3
#define DPU_AFFINE_FLOAT_EXP     0.000001
#define DPU_AFFINE_SHIFT         10
#define DPU_AFFINE_SHIFT_VALUE   (1 << DPU_AFFINE_SHIFT)

STATIC td_void dpu_affine_matrix_init(dpu_hal_affine_cfg *hal_cfg)
{
    /* init inv_matrix */
    hal_cfg->inv_m.matrix[0][0] = 1;
    hal_cfg->inv_m.matrix[0][1] = 0;
    hal_cfg->inv_m.matrix[0][2] = 0; /* 2:index */

    hal_cfg->inv_m.matrix[1][0] = 0;
    hal_cfg->inv_m.matrix[1][1] = 1;
    hal_cfg->inv_m.matrix[1][2] = 0; /* 2:index */

    hal_cfg->inv_m.matrix[2][0] = 0; /* 2:index */
    hal_cfg->inv_m.matrix[2][1] = 0; /* 2:index */
    hal_cfg->inv_m.matrix[2][2] = 1; /* 2:index */

    return;
}

STATIC td_void dpu_generate_affine_assign(const drv_dpu_matrix matrix, dpu_hal_matrix *pos_m)
{
    pos_m->matrix[0][0] = matrix.matrix[0][0];
    pos_m->matrix[0][1] = matrix.matrix[0][1];
    pos_m->matrix[0][2] = matrix.matrix[0][2]; /* 2:index */

    pos_m->matrix[1][0] = matrix.matrix[1][0];
    pos_m->matrix[1][1] = matrix.matrix[1][1];
    pos_m->matrix[1][2] = matrix.matrix[1][2]; /* 2:index */

    pos_m->matrix[2][0] = matrix.matrix[2][0]; /* 2:index */
    pos_m->matrix[2][1] = matrix.matrix[2][1]; /* 2:index */
    pos_m->matrix[2][2] = matrix.matrix[2][2]; /* 2:index */

    return;
}

STATIC td_void dpu_generate_inverse_affine(const dpu_hal_matrix pos_m, dpu_hal_matrix *inv_m, td_u32 len)
{
    if (len != HAL_DPU_MATRIX_ARRAY_LENGTH) {
        return;
    }

    td_float det, delta;
    td_float det00, det01, det02;
    td_float det10, det11, det12;
    td_float det20, det21, det22;

    det00 = (pos_m.matrix[1][1] * pos_m.matrix[2][2] - pos_m.matrix[2][1] * pos_m.matrix[1][2]); /* 2:index */
    det01 = (pos_m.matrix[1][0] * pos_m.matrix[2][2] - pos_m.matrix[2][0] * pos_m.matrix[1][2]); /* 2:index */
    det02 = (pos_m.matrix[1][0] * pos_m.matrix[2][1] - pos_m.matrix[2][0] * pos_m.matrix[1][1]); /* 2:index */

    det10 = (pos_m.matrix[0][1] * pos_m.matrix[2][2] - pos_m.matrix[2][1] * pos_m.matrix[0][2]); /* 2:index */
    det11 = (pos_m.matrix[0][0] * pos_m.matrix[2][2] - pos_m.matrix[2][0] * pos_m.matrix[0][2]); /* 2:index */
    det12 = (pos_m.matrix[0][0] * pos_m.matrix[2][1] - pos_m.matrix[2][0] * pos_m.matrix[0][1]); /* 2:index */

    det20 = (pos_m.matrix[0][1] * pos_m.matrix[1][2] - pos_m.matrix[1][1] * pos_m.matrix[0][2]); /* 2:index */
    det21 = (pos_m.matrix[0][0] * pos_m.matrix[1][2] - pos_m.matrix[1][0] * pos_m.matrix[0][2]); /* 2:index */
    det22 = (pos_m.matrix[0][0] * pos_m.matrix[1][1] - pos_m.matrix[1][0] * pos_m.matrix[0][1]); /* 2:index */

    det = pos_m.matrix[2][0] * det20 - pos_m.matrix[2][1] * det21 + pos_m.matrix[2][2] * det22; /* 2:index */
    if ((det > -DPU_AFFINE_FLOAT_EXP) && (det < DPU_AFFINE_FLOAT_EXP)) {
        gpu_err("det is zero, rot matrix is invalid!");
        det = DPU_AFFINE_FLOAT_EXP;
    }

    inv_m->matrix[0][0] =  (det00 * DPU_AFFINE_SHIFT_VALUE) / det;
    inv_m->matrix[0][1] = -(det10 * DPU_AFFINE_SHIFT_VALUE) / det;
    inv_m->matrix[0][2] =  (det20 * DPU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m->matrix[1][0] = -(det01 * DPU_AFFINE_SHIFT_VALUE) / det;
    inv_m->matrix[1][1] =  (det11 * DPU_AFFINE_SHIFT_VALUE) / det;
    inv_m->matrix[1][2] = -(det21 * DPU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    inv_m->matrix[2][0] =  (det02 * DPU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m->matrix[2][1] = -(det12 * DPU_AFFINE_SHIFT_VALUE) / det; /* 2:index */
    inv_m->matrix[2][2] =  (det22 * DPU_AFFINE_SHIFT_VALUE) / det; /* 2:index */

    if ((fabs(pos_m.matrix[0][0] * pos_m.matrix[0][0] + pos_m.matrix[0][1] * pos_m.matrix[0][1] - 1.0) <= (1e-6)) &&
        (fabs(pos_m.matrix[1][0] * pos_m.matrix[1][0] + pos_m.matrix[1][1] * pos_m.matrix[1][1] - 1.0) <= (1e-6))) {
        /* rotate center position compensation */
        delta = (td_float)DPU_AFFINE_SHIFT_VALUE;
        inv_m->matrix[0][2] += ((inv_m->matrix[0][0] + inv_m->matrix[0][1]) - delta) / 2.0f; /* 2:index */
        inv_m->matrix[1][2] += ((inv_m->matrix[1][0] + inv_m->matrix[1][1]) - delta) / 2.0f; /* 2:index */
        inv_m->matrix[2][2] += (inv_m->matrix[2][0] + inv_m->matrix[2][1]) / 2.0f;           /* 2:index */
    }

    return;
}

td_void dpu_hal_affine_config(dpu_layer_dev *layer_dev)
{
    if (!(layer_dev->run_info.is_mapping)) {
        return;
    }

    dpu_hal_affine_cfg hal_cfg = {0};

    dpu_affine_matrix_init(&hal_cfg);
    dpu_generate_affine_assign(layer_dev->run_info.matrix, &hal_cfg.pos_m);
    dpu_generate_inverse_affine(hal_cfg.pos_m, &hal_cfg.inv_m, DPU_HAL_MATRIX_ARRAY_LENGTH);

    hal_cfg.mapping_ctrl.req_tile_sel = ((layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_2X) ||
        (layer_dev->canvas_surface.compress_mode == DRV_GFX_COMPRESS_HFBC_3X)) ? 0 : 1;
    hal_cfg.mapping_ctrl.mapping_en = TD_TRUE;

    hal_cfg.mapping_ctrl.mapping_mode  = DPU_HAL_TRANSFORM_MODE_BILINEAR;
    hal_cfg.mapping_ctrl.alpha_mapping_mode = 1;
    hal_cfg.mapping_ctrl.tile_linear_sel = 0;

    hal_cfg.t2r0_ctrl.t2r0_ck_gt_en = 1;
    hal_cfg.t2r0_ctrl.t2r0_t2r_en = 1;
    hal_cfg.t2r0_ctrl.t2r0_tile_wth = 4; // 4：t2r0_tile_wth

    hal_cfg.crop.mapping_crop_x = (td_u32)layer_dev->run_info.update_region.x;
    hal_cfg.crop.mapping_crop_y = (td_u32)layer_dev->run_info.update_region.y;
    hal_cfg.crop.mapping_crop_w = layer_dev->run_info.update_region.width - 1;
    hal_cfg.crop.mapping_crop_h = layer_dev->run_info.update_region.height - 1;

    dpu_hal_disp_set_affine_info(&hal_cfg, layer_dev->layer_id);
}