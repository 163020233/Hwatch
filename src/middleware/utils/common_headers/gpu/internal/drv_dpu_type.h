/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : drv dpu type file
 */

#ifndef DRV_GRAPHIC_INCLUDE_DRV_DPU_TYPE_H
#define DRV_GRAPHIC_INCLUDE_DRV_DPU_TYPE_H

#include "td_base.h"
#include "drv_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_DPU_INVALID_HANDLE 0xFFFFFFFF
#define DRV_DPU_SAT_MEDIUM 0.5f  // 中等饱和度

typedef enum {
    DRV_DPU_LAYER_ID1 = 0,
    DRV_DPU_LAYER_ID2,
    DRV_DPU_LAYER_ID3,
    DRV_DPU_LAYER_ID4,
    DRV_DPU_LAYER_ID_MAX
} drv_dpu_layer_id;

#define DRV_DPU_MAX_LAYER_ID     DRV_DPU_LAYER_ID2

typedef enum {
    DRV_DPU_ZORDER_MODE_MOVETOP = 0,
    DRV_DPU_ZORDER_MODE_MOVEUP,
    DRV_DPU_ZORDER_MODE_MOVEBOTTOM,
    DRV_DPU_ZORDER_MODE_MOVEDOWN,
    DRV_DPU_ZORDER_MODE_MAX
} drv_dpu_zorder_mode;

typedef enum {
    DRV_DPU_BUF_MODE_NORMAL = 0,
    DRV_DPU_BUF_MODE_SINGLE,
    DRV_DPU_BUF_MODE_MAX
} drv_dpu_buf_mode;

typedef struct {
    td_bool key_en;
    td_u32 key_value;
} drv_dpu_colorkey;

typedef struct {
    td_u32 width;
    td_u32 height;
    drv_gfx_fmt layer_fmt;
    drv_dpu_layer_id layer_id;
    drv_dpu_buf_mode buf_mode;
    drv_gfx_compress_mode compress_mode;
    drv_gfx_rotation_mode rotation_mode;
} drv_dpu_layer_info;

typedef struct {
    td_float matrix[3][3];
} drv_dpu_matrix;

typedef struct {
    td_u32 wbc_bind_mode;
    td_u32 wbc_stride;
    td_u32 wbc_height;
    td_u32 wbc_width;
    td_u32 wbc_addr;
    drv_gfx_fmt color_fmt;
} drv_dpu_wbc_info;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    td_u32 uv_stride;
    td_u32 uv_offset;
    td_u32 size;
    td_u32 phy_addr;
    td_uchar *vir_addr;
    ext_rect *update_rect;
    drv_gfx_fmt color_fmt;
    td_bool premulti_en;
    td_bool flip_en;
    drv_gfx_compress_mode compress_mode;
    drv_gfx_rotation_mode rotation_mode;
    drv_dpu_matrix *matrix;
} drv_dpu_surface;

typedef enum {
    DRV_DPU_WORK_SCENE_NORMAL = 0,
    DRV_DPU_WORK_SCENE_AOD,
    DRV_DPU_WORK_SCENE_MAX
} drv_dpu_work_scene;

typedef enum {
    DRV_DPU_COLOR_NONE = 0,                 /*!< @if Eng Normal color mode.
                                                @else   常规颜色模式 @endif */
    DRV_DPU_COLOR_WEAK_RED,                 /*!< @if Eng Red color weak.
                                                @else   红色色弱 @endif */
    DRV_DPU_COLOR_WEAK_GREEN,               /*!< @if Eng Green color weak.
                                                @else   绿色色弱 @endif */
    DRV_DPU_COLOR_WEAK_BLUE,                /*!< @if Eng Blue color weak.
                                                @else   蓝色色弱 @endif */
    DRV_DPU_COLOR_INVERT_RED,               /*!< @if Eng Red color weak and color invert.
                                                @else   红色色弱和颜色反转 @endif */
    DRV_DPU_COLOR_INVERT_GREEN,             /*!< @if Eng Green color weak and color invert.
                                                @else   绿色色弱和颜色反转 @endif */
    DRV_DPU_COLOR_INVERT_BLUE,              /*!< @if Eng Blue color weak and color invert.
                                                @else   蓝色色弱和颜色反转 @endif */
    DRV_DPU_COLOR_INVERT                    /*!< @if Eng color invert.
                                                @else   颜色反转 @endif */
} drv_dpu_color_type;

typedef struct {
    drv_dpu_work_scene  work_scene;         /*!< @if Eng work scene
                                                @else   工作场景 @endif */
    td_bool             is_la_valid;        /*!< @if Eng is Real-time light intensity valid.
                                                @else   实时光照强度是否有效 @endif */
    td_u32              la;                 /*!< @if Eng Real-time light intensity, in lux.
                                                @else   实时光照强度，以lux为单位 @endif */
    td_bool             mir_en;             /*!< @if Eng left-right reversal enable flag.
                                                @else   左右翻转使能标记 @endif */
    td_bool             compress_output_mode; /*!< @if Eng Compressed transmission and display enable flag.
                                                @else   压缩送显使能标记 @endif */
    drv_dpu_wbc_info    *wbc_info;          /**< For test */

    drv_dpu_color_type  color_type;         /*!< @if Eng Color Gamut Conversion Type.
                                                @else   色域转换类型 @endif */
    td_float            color_sat;          /*!< @if Eng color saturation.
                                                @else   色彩饱和度, 范围[0-1], 0.5为中等饱和度 @endif */
} drv_dpu_output;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
