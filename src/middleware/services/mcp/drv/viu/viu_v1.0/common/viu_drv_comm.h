/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_drv_comm.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_DRV_COMM_H
#define VIU_DRV_COMM_H

#include "drv_viu_struct.h"
#include "viu_drv_define.h"
#include "viu_base_define.h"
#include "viu_vicap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

viu_comm_ctx *viu_get_comm_ctx(td_void);

td_bool viu_check_frame_match_pipe_width_height(td_s32 mast_pipe, ext_isp_video_frame_info *video_frame_info);

td_void viu_comm_cap_send_wdr_frame(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode);
td_void viu_comm_prepare_next_frame_pipe_attr(td_s32 pipe_id);
td_void viu_comm_cap_send_dump_frame(td_s32 pipe_id);

td_s32 viu_put_node_to_line_input_pic_queue(td_s32 pipe_id);
td_s32 viu_get_venc_node_to_line_input_pic_queue(td_s32 pipe_id, const ext_isp_video_frame *v_frame);

td_void viu_cap_save_isp_info_to_pic(td_s32 pipe_id, viu_buf_info *next_frm);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VIU_H */
