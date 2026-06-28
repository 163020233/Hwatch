/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_drv.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef DRV_VICAP_H
#define DRV_VICAP_H

#include "drv_viu_struct.h"
#include "drv_viu_vicap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

ext_isp_bayer_format drv_pipe_get_bayer_format(td_u8 pipe_id);
td_void drv_pipe_get_raw_source(td_s32 pipe_id, ext_viu_pipe_frame_source *source);
td_void drv_pipe_get_pre_crop(td_u8 pipe_id, ext_isp_crop_info *crop_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif