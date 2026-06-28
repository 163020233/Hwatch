/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mesh algorithm header
 */

#ifndef RM_MESH_3D_ALG_H
#define RM_MESH_3D_ALG_H

#include "td_base.h"
#include "soc_vau_type.h"

#define MODEL_BASE_SIZE 33

typedef struct {
    td_u8 data[10];
} float_packet;

typedef struct {
    td_u32 width;
    td_u32 height;
} vau_mesh_resolution;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_float cx;
    td_float cy;
    td_float len;
    td_float thr;
    td_float time;
    td_u32 animation_region_w; // 0~32整数
    td_u32 animation_region_h;  // 0~32整数
} vau_modelling_mesh;

typedef struct {
    td_float x;                  /*!< @if Eng x coord.
                                      @else   x坐标 @endif */
    td_float y;                  /*!< @if Eng y coord.
                                      @else   y坐标 @endif */
    td_float z;                  /*!< @if Eng z coord.
                                      @else   z坐标 @endif */
    td_u32 flag;                 /*!< @if Eng flag.
                                      0: no intersection-point (bg_color)
                                      1: intersection-point, invisible (shadow_color)
                                      2: intersection-point, visible (mapping & fill_color)
                                      3: reserved
                                      @else   标志 @endif */
} ext_vau_model_data;

typedef union {
    td_float fp32;
    td_u32 u32_fp;
} float_u32_data;

typedef struct {
    float_u32_data x;
    float_u32_data y;
    float_u32_data z;
    td_u32 flag;
} save_model_data;

typedef struct {
    td_u32 data1;
    td_u32 data2;
    td_u16 data3;
    td_u16 reserved;
} float_packet_80bit;

typedef struct {
    td_u32 width;
    td_u32 height;
    ext_vau_model_data *data;
} ext_vau_model_mesh_surface;

td_void rm_update_mesh_model(const ext_vau_surface *api_src, const ext_vau_surface *api_dst, td_u32 model_addr);

#endif
