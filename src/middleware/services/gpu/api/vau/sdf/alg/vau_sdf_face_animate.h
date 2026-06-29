/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf face animate header
 */

#ifndef _VAU_SDF_FACE_ANIMATE_H_
#define _VAU_SDF_FACE_ANIMATE_H_

#include "vau_sdf_type.h"
#include "vau_sdf_comm_define.h"

typedef struct {
    td_float smooth_r;
    td_float smooth_t;
    td_float smooth_eye;
    td_float smooth_mouth;
    td_float open_eyes_thr;
    td_float close_eyes_thr;
    td_float close_mouth_thr;
    td_float mouth_scale;
    td_float eyes_scale;
    td_float cam_k[4]; // fx, fy, cx, cy
    td_float cam_k_inv[9];
    td_float cam_v_fov;
    td_float real_cam_v_fov;
    td_float fov_scale;
    td_u32 invalid_face_continue_max;
} face_animate_cfg;

typedef struct {
    td_float lambda;
    td_float sigma;
    td_float x_prev;
    td_float smooth_prev;
    td_s32 initialized;
} one_euro_filter;

extern face_animate_cfg g_face_cfg;

td_void vau_sdf_memoji_init(td_void);
face_animate_cfg* vau_sdf_get_face_animate_cfg(td_void);
td_void vau_sdf_memoji(ext_vau_face_lmk_attr* fsd_info, ext_vau_vec3* inter_step, ext_vau_rt* pnp_rt,
    vau_sdf_animate_offset* animate_offset);

#endif
