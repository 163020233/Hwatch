/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau job
 */

#ifndef DRV_GRAPHIC_VAU_JOB_VAU_JOB_H
#define DRV_GRAPHIC_VAU_JOB_VAU_JOB_H

#include "drv_vau_type.h"
#include "vau_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_GFX_SPLIT_MAX_WIDTH   624
#define DRV_GFX_NOT_SPLIT_MAX_WIDTH   640

td_s32 drv_vau_create(td_handle *handle);
td_s32 drv_vau_destroy(td_handle handle);
td_s32 drv_vau_fill(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_blit(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_compose(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_submit(td_handle handle, td_bool is_sync, td_u32 time_out);
td_s32 drv_vau_wait_for_done(td_handle handle, td_u32 timeout);
td_s32 drv_vau_wait_all_done(td_u32 time_out_ms);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
