/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: wdr_buf.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef WDR_BUF_H
#define WDR_BUF_H

#include "viu_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 pool_id;
    ext_isp_size size;
    ext_compress_mode compress_mode;
    td_u32 bit_width;
    ext_pixel_format pixel_format;
} viu_get_wdr_vb_info;

td_s32 viu_wdr_get_vb(td_s32 pipe_id, viu_get_wdr_vb_info *info, ext_isp_video_frame_info *video_frame);
td_s32 viu_wdr_create_pool(td_s32 viu_pipe_id, td_u32 *pool_id,
    td_u32 buf_num, td_u32 buf_size, const td_char *buf_name);
td_s32 viu_wdr_destory_pool(td_s32 viu_pipe_id, td_u32 pool_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif