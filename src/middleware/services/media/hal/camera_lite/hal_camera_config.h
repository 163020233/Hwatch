/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: camera config
 */

#ifndef HAL_CAMERA_CONFIG_H
#define HAL_CAMERA_CONFIG_H

#include "mpi_cis_ext.h"
#include "hal_camera.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CAM_CFG_CAMERA_NUM 1
#define CAM_CFG_OUT_WIDTH 640
#define CAM_CFG_OUT_HEIGHT 480
#define CAM_CFG_STREAM_MAX_NUM 3
#define CAM_CFG_STREAM_SUPPORT_FORMATS {FORMAT_JPEG, FORMAT_PRIVATE, FORMAT_YVU420}
#define CAM_CFG_CAM_DESC "DVP Camera"
#define CAM_CFG_CAM_FPS 30
#define CAM_CFG_VENC_WAIT_TIMEOUT_MS 1000
#define CAM_CFG_VDEC_WAIT_TIMEOUT_MS 10
#define CAM_CFG_VDEC_MAX_BUF 3

// jpeg compression ratio generally 1/5 ~ 1/30, venc max is 1/2
#define CAM_CFG_JPEG_BUF_MAX (CAM_CFG_OUT_WIDTH * CAM_CFG_OUT_HEIGHT / 2)

#define CAM_CFG_VI_DEV_ID 0
#define CAM_CFG_VI_PIPE_ID 1

// Refer to mpi_cis_ext.h
#define CAM_CFG_GET_SENSOR() ext_mpi_get_gc032a_obj()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // HAL_CAMERA_CONFIG_H