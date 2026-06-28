/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu hal type define file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_TYPE_H
#define DRV_GRAPHIC_HAL_VAU_HAL_TYPE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VAU_AFFFINE_MATRIX_LEN 3
typedef td_float affine_matrix[VAU_AFFFINE_MATRIX_LEN][VAU_AFFFINE_MATRIX_LEN];

typedef struct {
    td_u32 src_width;
    td_u32 src_height;
    td_u32 dst_width;
    td_u32 dst_height;
    td_u8 src_fmt;
    td_u8 dst_fmt;
    td_u8 transform_mode;
    affine_matrix inv_m;
    td_u32 pattern_mode;
    td_u32 pattern_argb;
} vau_affine_cfg;

typedef enum {
    HAL_VAU_COMPONENT_ALPHA = 0,
    HAL_VAU_COMPONENT_RED   = 1,
    HAL_VAU_COMPONENT_GREEN = 2,
    HAL_VAU_COMPONENT_BLUE  = 3,
    HAL_VAU_COMPONENT_MAX,
} hal_vau_component_num;

typedef enum {
    HAL_VAU_COLORKEY_VALUE_IN      = 0,
    HAL_VAU_COLORKEY_VALUE_OUT     = 1,
    HAL_VAU_COLORKEY_VALUE_IGNORE  = 2
} hal_vau_colorkey_mode;

typedef enum {
    HAL_VAU_COLORKEY_BACKGROUND             = 0,
    HAL_VAU_COLORKEY_FOREGROUND_BEFORE_CLUT = 2,
    HAL_VAU_COLORKEY_FOREGROUND_AFTER_CLUT  = 3
} hal_vau_colorkey_sel;

typedef struct {
    td_u16 key_min;
    td_u16 key_max;
    td_u16 key_mask;
    hal_vau_colorkey_mode key_mode;
} hal_vau_colorkey_component;

typedef struct {
    td_bool key_en;
    hal_vau_colorkey_sel key_sel;
    hal_vau_colorkey_component component[HAL_VAU_COMPONENT_MAX];
} hal_vau_colorkey_opt;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif