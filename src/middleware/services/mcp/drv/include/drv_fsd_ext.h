/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: mipi rx kernel export interface definition.
 * Author: AuthorNameMagicTag
 * Create: 2021-10-18
 */

#ifndef __DRV_FSD_EXT_H__
#define __DRV_FSD_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_fsd_struct.h"

#ifdef __cplusplus

#if __cplusplus
extern "C" {
#endif
#endif

typedef void (*fsd_callback)(td_s32 handle, ext_fsd_task_output_info *result);
td_s32 ext_drv_fsd_init(ext_fsd_alg_coef *ext_alg_coef);
td_s32 ext_drv_fsd_deinit(td_void);
td_s32 ext_drv_fsd_begin_job(td_s32 *handle);
td_s32 ext_drv_fsd_add_task(td_s32 handle, const ext_fsd_task_input_attr *task);
td_s32 ext_drv_fsd_end_job(td_s32 handle, ext_fsd_task_output_info *result);
td_s32 ext_drv_fsd_cancel_job(td_s32 handle);
td_s32 ext_drv_fsd_register_callback(td_s32 handle, fsd_callback callback);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
