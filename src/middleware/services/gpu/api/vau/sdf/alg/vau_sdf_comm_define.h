/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf struct define file
 */

#ifndef _VAU_SDF_COMM_DEFINE_H_
#define _VAU_SDF_COMM_DEFINE_H_

#include <stdio.h>
#include "vau_sdf_reg_define.h"
#include "vau_sdf_type.h"

#define FRAME_NUM 200

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ext_sdf_cam_info *cam_info;
    ext_sdf_light_info *light_info;
    ext_sdf_press_info *press_info;
    ext_vau_face_lmk_attr *face_lmk_attr;
    td_bool is_first_frame;
} ext_sdf_input_info;

// register camera--cpu
typedef struct {
    td_u16 img_width;
    td_u16 img_height;
    td_float cam_radius;      // the distance from the camera center to the target center
    td_float cam_lgitude;     // camera longitude
    td_float cam_latitude;    // camera latitude
    td_float cam_v_fov;       // camera fov
    ext_vau_vec3 cam_target;    // taget center point coordinate xyz
    ext_vau_vec3 cam_rot_angle; // camera rotate angle x
    ext_vau_vec3 cam_rot_trans; // camera trans matrix xyz
    td_float render_fov_scale;
} vau_sdf_cam_param;

typedef struct {
    td_u16 tex_width;
    td_u16 tex_height;
    td_u16 tex_stride;
    td_u32 tex_offset;
} vau_sdf_tex_config;

typedef struct {
    td_u16 frame_inter_num;
    td_u16 key_left_frame;
    td_u16 key_right_frame;
    td_u64 key_prim_mask;
}vau_sdf_animate_frame;

typedef struct {
    td_float puls_x3d;
    td_float puls_y3d;
    td_float puls_z3d;
}vau_sdf_animate_offset;

typedef struct {
    td_u32 global_offset;
    td_u32 bin_prim_offset;

    td_u16 frame_num;
    td_u16 animate_num;
    vau_sdf_animate_frame animate_frame[FRAME_NUM];
    // model outermost 3d bounding box
    ext_vau_vec3 model_st_xyz;
    ext_vau_vec3 model_ed_xyz;

    // partial 3d bounding box
    ext_vau_vec3 partial_st_xyz;
    ext_vau_vec3 partial_ed_xyz;

    vau_sdf_cam_param cam_param;

    td_u16 tex_number;
    vau_sdf_tex_config tex_config[4];
    vau_sdf_animate_offset animate_offset;
} vau_sdf_head;


typedef struct {
    ext_vau_vec3 pix_start_vector;
    td_float focal_len;                // based on the camera's intrinsic parameters
    ext_vau_vec3 pixel_delta_u;        // the spatial distance between two pixels horizontal direction
    ext_vau_vec3 pixel_delta_v;        // the spatial distance two pixels vertical direction
    ext_vau_vec3 ray_origion;         // camera center
    ext_vau_vec3 tget_cam_dir;        // camera orientation
    ext_vau_vec3 cam_target;
    td_u16 image_width;
    td_u16 image_height;
} vau_sdf_cam_output;

typedef struct {
    vau_sdf_hw_prim *prim_kl;
    vau_sdf_hw_prim *prim_kr;
    vau_sdf_hw_prim *prim_diff;
}vau_sdf_key_diff_info;

typedef struct {
    vau_sdf_hw_prim *prim_base;
    vau_sdf_hw_prim *prim_diff;
    vau_sdf_hw_prim *prim_out;
}vau_sdf_key_blend_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif