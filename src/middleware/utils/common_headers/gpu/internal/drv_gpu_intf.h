/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#ifndef DRV_GPU_INTF_H
#define DRV_GPU_INTF_H

#include "drv_dpu_intf.h"
#include "drv_vau_intf.h"
#ifdef SUPPORT_GPU_GMMU
#include "drv_gmmu_intf.h"
#endif
#ifdef SUPPORT_GPU_JPEG
#include "drv_jpeg_intf.h"
#endif
#include "drv_gpu_dfx.h"
#include "drv_gpu_module.h"

typedef enum {
    VIDEO_POWER_OFF = 0,
    VIDEO_POWER_ON  = 1,
} video_power_ctrl_mode;

typedef enum {
    DRV_VAU_WORK_SCENE_PERFORMANCE_MODE = 0,
    DRV_VAU_WORK_SCENE_BALANCE_MODE,
    DRV_VAU_WORK_SCENE_MAX
} drv_vau_work_scene;

// 当前只用于JPEG VDEC电源控制
td_void uapi_video_power_ctrl(gpu_module_id id, video_power_ctrl_mode mode);

#endif