/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : drv gpu dfx
 */

#ifndef DRV_INCLUDE_DRV_GPU_DFX_H
#define DRV_INCLUDE_DRV_GPU_DFX_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 uapi_vau_dfx_proc_read(int argc, const char *argv[]);
td_s32 uapi_dpu_dfx_proc_read(int argc, const char *argv[]);
td_s32 uapi_dpu_dfx_print_enable(int argc, const char *argv[]);
td_s32 uapi_jpeg_dfx_proc_read(int argc, const char *argv[]);
td_s32 uapi_gmmu_dfx_proc_read(int argc, const char *argv[]);
td_s32 uapi_dpu_dfx_save_fb_enable(int argc, const char *argv[]);
td_s32 uapi_dpu_dfx_enable_dpu_refresh_skip(int argc, const char *argv[]);
td_void uapi_vau_proc_mem(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_GPU_DFX_H */
