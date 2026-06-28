/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_extern_venc.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-21
 */

#include "drv_mcp_dependency.h"
#include "drv_venc_inner.h"
#include "viu_drv_comm.h"

static td_s32 g_pipe_id = -1;

static td_void convert_video_frm_venc2isp(const ext_venc_frame *frame, ext_isp_video_frame *dest_frame)
{
    (td_void)memset_s(dest_frame, sizeof(ext_isp_video_frame), 0, sizeof(ext_isp_video_frame));

    dest_frame->bit_depth = frame->user_frame.bit_depth;

    dest_frame->width = frame->user_frame.width;
    dest_frame->height = frame->user_frame.height;
    dest_frame->field = frame->user_frame.field;
    dest_frame->pixel_format = frame->user_frame.pixel_format;
    dest_frame->video_format = frame->user_frame.video_format;
    dest_frame->compress_mode = frame->user_frame.compress_mode;
    dest_frame->dynamic_range = frame->user_frame.dynamic_range;
    dest_frame->color_space = frame->user_frame.color_space;

    dest_frame->stride[0] = frame->user_frame.stride[0];
    dest_frame->stride[1] = frame->user_frame.stride[1];
    dest_frame->phy_addr[0] = frame->user_frame.phy_addr[0];
    dest_frame->phy_addr[1] = frame->user_frame.phy_addr[1];
    dest_frame->vir_addr[0] = frame->user_frame.vir_addr[0];
    dest_frame->vir_addr[1] = frame->user_frame.vir_addr[1];

    dest_frame->time_ref = frame->user_frame.time_ref;
    dest_frame->pts = frame->user_frame.pts;
    dest_frame->frame_flag = frame->user_frame.frame_flag;
}

static td_void convert_video_frm_isp2venc(const ext_isp_video_frame *frame, ext_venc_frame *venc_frame)
{
    (td_void)memset_s(venc_frame, sizeof(ext_venc_frame), 0, sizeof(ext_venc_frame));

    venc_frame->user_frame.bit_depth = EXT_BIT_DEPTH_8;

    venc_frame->user_frame.width = frame->width;
    venc_frame->user_frame.height = frame->height;
    venc_frame->user_frame.field = frame->field;
    venc_frame->user_frame.pixel_format = frame->pixel_format;
    venc_frame->user_frame.video_format = frame->video_format;
    venc_frame->user_frame.compress_mode = frame->compress_mode;
    venc_frame->user_frame.dynamic_range = frame->dynamic_range;
    venc_frame->user_frame.color_space = frame->color_space;

    venc_frame->user_frame.stride[0] = frame->stride[0];
    venc_frame->user_frame.stride[1] = frame->stride[1];
    venc_frame->user_frame.phy_addr[0] = frame->phy_addr[0];
    venc_frame->user_frame.phy_addr[1] = frame->phy_addr[1];
    venc_frame->user_frame.vir_addr[0] =  frame->vir_addr[0];
    venc_frame->user_frame.vir_addr[1] = frame->vir_addr[1];

    venc_frame->user_frame.time_ref = frame->time_ref;
    venc_frame->user_frame.pts = frame->pts;
    venc_frame->user_frame.frame_flag = frame->frame_flag;
}

td_s32 ext_venc_return_frame(td_handle src_handle, const ext_venc_frame *frame)
{
    viu_unused(src_handle);
    ext_isp_video_frame video_frame = {};

    convert_video_frm_venc2isp(frame, &video_frame);

    viu_get_venc_node_to_line_input_pic_queue(g_pipe_id, &video_frame);

    return TD_SUCCESS;
}

td_s32 viu_ext_venc_quque_frame(td_s32 pipe_id, td_s32 dst_id, ext_isp_video_frame *v_frame)
{
    ext_venc_task task = {0};
    td_u32 ret = TD_SUCCESS;

    g_pipe_id = pipe_id;
    task.callback.src_handle = dst_id;
    task.callback.pfn_venc_return_frame = ext_venc_return_frame;

    convert_video_frm_isp2venc(v_frame, &task.frame);

    ret = uapi_venc_inner_send_task(0, &task);  // venc 处理返回值
    if (ret != EXT_SUCCESS) {
        viu_err_trace("venc reviece frame fail\n");
        ext_venc_return_frame(0, &task.frame);
    }

    return ret;
}
