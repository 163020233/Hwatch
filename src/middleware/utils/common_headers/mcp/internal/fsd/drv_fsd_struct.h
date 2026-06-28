/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of drv_fsd_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-11-29
 */

#ifndef __DRV_FSD_STRUCT_H__
#define __DRV_FSD_STRUCT_H__

#include "drv_isp_video_ext.h"
#include "drv_mcp_type.h"
#include "drv_mcp_dependency.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define EXT_FSD_FACE_NUM_MAX 10
#define EXT_FSD_LM_NUM_MAX 71
#define EXT_FSD_FACE_POINT_NUM_MAX 9

typedef struct {
    td_u64 face_listnode_addr;
    td_u64 face_weight_addr;
    td_u64 landmark_listnode_addr;
    td_u64 landmark_weight_addr;
    td_u64 scene_listnode_addr;
    td_u64 scene_weight_addr;
    td_u32 landmark_enable;
} ext_fsd_alg_coef;

typedef struct {
    ext_fsd_alg_coef fsd_alg_coef;
    mcp_osal_mem_info fsd_mem_info;
} fsd_coef_mem_config;

typedef enum {
    EXT_FSD_MODE_FACE = 0,
    EXT_FSD_MODE_LANDMARK,
    EXT_FSD_MODE_SCENE,
    EXT_FSD_MODE_MAX
} ext_fsd_mode;

typedef struct {
    td_u16 x;
    td_u16 y;
} ext_fsd_point_info;

typedef struct {
    td_float x;
    td_float y;
} ext_fsd_landmark_info;

typedef struct {
    td_u16 x;
    td_u16 y;
    td_u16 width;
    td_u16 height;
} ext_fsd_face_info;

typedef struct {
    td_u8                face_num;
    ext_fsd_face_info    face[EXT_FSD_FACE_NUM_MAX];
    td_u16               confidence[EXT_FSD_FACE_NUM_MAX];
    ext_fsd_point_info   point_info[EXT_FSD_FACE_NUM_MAX][EXT_FSD_FACE_POINT_NUM_MAX];
} ext_fsd_face_output;

typedef struct {
    td_u8              valid_face_flag;
    ext_fsd_landmark_info point_info[EXT_FSD_LM_NUM_MAX];
} ext_fsd_landmark_output;

typedef enum {
    EXT_FSD_SCENE_ANIMALS = 0,
    EXT_FSD_SCENE_BUILDINGS,
    EXT_FSD_SCENE_COMPUTER,
    EXT_FSD_SCENE_DESERT,
    EXT_FSD_SCENE_FLOWER,
    EXT_FSD_SCENE_FOOD,
    EXT_FSD_SCENE_FOREST,
    EXT_FSD_SCENE_HUMAN,
    EXT_FSD_SCENE_MOUNTAIN,
    EXT_FSD_SCENE_SEA,
    EXT_FSD_SCENE_SKY,
    EXT_FSD_SCENE_INDOOR,
    EXT_FSD_SCENE_MAX
} ext_fsd_scene_detection;

typedef struct {
    td_u32 confidence[EXT_FSD_SCENE_MAX];
} ext_fsd_scene_output;

typedef struct {
    ext_fsd_mode fsd_mode;
    ext_isp_video_frame img_in;
} ext_fsd_task_input_attr;

typedef union {
    ext_fsd_face_output     face_output;
    ext_fsd_landmark_output landmark_output;
    ext_fsd_scene_output    scene_output;
} ext_fsd_task_output_info;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
