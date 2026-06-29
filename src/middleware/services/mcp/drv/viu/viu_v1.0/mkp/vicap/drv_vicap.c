/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_pipe.c
 * Author: AuthorNameMagicTag
 * Create: 2025-04-17
 */

#include "viu_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

viu_pipe_drv_ctx *g_past_pipe_drv_ctx[MCP_VICAP_INSTANCE_MAX] = {TD_NULL};

ext_isp_bayer_format drv_pipe_get_bayer_format(td_u8 pipe_id)
{
    return g_past_pipe_drv_ctx[pipe_id]->pipe_drv_attr_setting.viu_pipe_attr.bayer_format;
}

td_void drv_pipe_get_raw_source(td_s32 pipe_id, ext_viu_pipe_frame_source *source)
{
    if (g_past_pipe_drv_ctx[pipe_id] == TD_NULL) {
        /* WDR模式的时候，退出时，首先销毁了mastpipe,然后其他的pipe还在运行，这时候会访问到空指针 */
        *source = EXT_VIU_PIPE_FRAME_SOURCE_DEV;
    } else {
        *source = g_past_pipe_drv_ctx[pipe_id]->pipe_drv_attr_setting.viu_pipe_attr.frame_source;
    }
}

td_void drv_pipe_get_pre_crop(td_u8 pipe_id, ext_isp_crop_info *crop_info)
{
    *crop_info = g_past_pipe_drv_ctx[pipe_id]->pipe_drv_attr_setting.viu_pipe_attr.crop_info;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
