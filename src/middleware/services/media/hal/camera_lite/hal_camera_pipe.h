/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: camera pipe
 */

#ifndef HAL_CAMERA_PIPE_H
#define HAL_CAMERA_PIPE_H

#include <stdint.h>
#include "soc_venc_type.h"
#include "hal_camera.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int cam_pipe_init(void);
int cam_pipe_deinit(void);
int cam_pipe_create(uint32_t cam_id);
int cam_pipe_destroy(uint32_t cam_id);
int cam_pipe_stream_on(uint32_t cam_id);
int cam_pipe_stream_off(uint32_t cam_id);
int cam_pipe_get_stream(uint32_t cam_id, ext_venc_stream *buffer, int32_t timeout_ms);
int cam_pipe_put_stream(uint32_t cam_id, ext_venc_stream *buffer);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // HAL_CAMERA_PIPE_H
