/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : gpu chr file
 */

#ifndef API_GPU_CHR_H
#define API_GPU_CHR_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MT_VAU_MANAGER 3
#define MT_JPEG_MANAGER 4
#define MT_DPU_MANAGER 6

typedef enum {
    MT_VAU_CREATE = 0x1,
    MT_VAU_CANCEL,
    MT_VAU_DESTROY,
    MT_VAU_FILL,
    MT_VAU_BLUR,
    MT_VAU_RENDER,
    MT_VAU_COMPOSE,
    MT_VAU_REGISTER,
    MT_VAU_RESUME,
    MT_VAU_SUSPEND,
    MT_VAU_WAIT_FOR_DONE,
    MT_VAU_WAIT_ALL_DONE,

    MT_VAU_INFO1_MAX = 0xff
} vau_chr_info1;

typedef enum {
    MT_VAU_OPEN_FAIL = 0x1,
    MT_VAU_CREATE_HANDLE_FAIL = 0x2,

    MT_VAU_CANCEL_FAIL = 0x1,

    MT_VAU_DESTROY_FAIL = 0x1,

    MT_VAU_FILL_PARAMETER_CHECK_FAIL = 0x1,
    MT_VAU_FILL_FAIL = 0x2,

    MT_VAU_BLUR_PARAMETER_CHECK_FAIL = 0x1,
    MT_VAU_BLUR_FAIL = 0x2,

    MT_VAU_RENDER_PARAMETER_CHECK_FAIL = 0x1,
    MT_VAU_RENDER_FAIL = 0x2,

    MT_VAU_COMPOSE_CHECK_FAIL = 0x1,
    MT_VAU_COMPOSE_FAIL = 0x2,

    MT_VAU_TIMEOUT_REGISTER = 0x1,

    MT_VAU_RESUME_BEGIN = 0x1,
    MT_VAU_RESUME_END = 0x2,

    MT_VAU_SUSPEND_BEGIN = 0x1,
    MT_VAU_SUSPEND_END = 0x2,

    MT_VAU_WAIT_FOR_DONE_TIMEOUT = 0x1,

    MT_VAU_INFO2_MAX = 0xffff
} vau_chr_info2;

typedef enum {
    MT_DPU_ISR_STATE = 0x1,
    MT_REGISTER_INFO,

    MT_DPU_MAX_INFO1 = 0xFF
} dpu_chr_info1;

typedef enum {
    MT_DPU_ISR_FINISH_TIMEOUT = 0x1,

    MT_DPU_REGISTER_INFO = 0x1,
    MT_MIPI_REGISTER_INFO = 0x2,

    MT_DPU_MAX_INFO2 = 0xFFFF
} dpu_chr_info2;

typedef enum {
    MT_JPEG_CREATE = 0x1,
    MT_JPEG_START,

    MT_JPEG_INFO1_MAX = 0xff
} jpeg_chr_info1;

typedef enum {
    MT_JPEG_CREATE_ALLOC_FAIL = 0x1,
    MT_JPEG_CREATE_STREAM_INIT_FAIL = 0x2,
    MT_JPEG_CREATE_START_FAIL = 0x3,
    MT_JPEG_CREATE_HANDLE_FAIL = 0x4,

    MT_JPEG_START_DECOMPRESS_INIT_FAIL = 0x1,
    MT_JPEG_START_DECOMPRESS_START_FAIL = 0x2,
    MT_JPEG_START_CSC_CONVERT_FAIL = 0x3,

    MT_JPEG_INFO2_MAX = 0xffff
} jpeg_chr_info2;

td_void gpu_record_chr_reg(td_u32 addr, td_u32 cnt, td_u32 event, td_u32 info1, td_u32 info2);
td_void print_mipi_and_dpu_reg(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_GPU_CHR_H */