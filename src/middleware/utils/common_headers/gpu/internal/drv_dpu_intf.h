/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : drv dpu file
 */

#ifndef DRV_INCLUDE_DRV_DPU_H
#define DRV_INCLUDE_DRV_DPU_H

#include "drv_dpu_type.h"
#include "drv_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    drv_gfx_fmt color_fmt;
    td_u32 addr;
} dpu_boot_logo_info;

td_s32 uapi_dpu_suspend(td_void *private_data);
td_s32 uapi_dpu_resume(td_void *private_data);
td_bool uapi_dpu_get_suspend_flag(void);

td_s32 uapi_dpu_module_init(void);
td_void uapi_dpu_module_exit(void);

td_s32 uapi_dpu_boot_logo(dpu_boot_logo_info *info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_DPU_H */