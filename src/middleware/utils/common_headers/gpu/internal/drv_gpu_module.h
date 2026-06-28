/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#ifndef DRV_GPU_MODULE_H
#define DRV_GPU_MODULE_H

typedef enum {
    GPU_ID_VAU      = 0x00,
    GPU_ID_DPU      = 0x01,
    GPU_ID_JPEG     = 0x02,
    GPU_ID_GMMU     = 0x03,
    GPU_ID_VDEC     = 0x04,
    GPU_ID_JPGE     = 0x05,
    GPU_ID_VEDU     = 0x06,
    GPU_ID_MAX
} gpu_module_id;

#endif