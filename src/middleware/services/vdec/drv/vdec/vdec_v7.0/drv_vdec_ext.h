/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-05-16
 */

#ifndef __DRV_VDEC_EXT_H__
#define __DRV_VDEC_EXT_H__

#include "drv_vdec_struct.h"
#include "drv_video_ext.h"

/* CNcomment: 强制约束，严禁include其它外部头文件，要实现解耦，才能做到独立于SDK共主线演进 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef td_s32 (*fn_drv_vdec_create)(td_handle *handle);

typedef td_s32 (*fn_drv_vdec_destroy)(td_handle handle);

typedef td_s32 (*fn_drv_vdec_start)(td_handle handle);

typedef td_s32 (*fn_drv_vdec_stop)(td_handle handle);

typedef td_s32 (*fn_drv_vdec_reset)(td_handle handle);

typedef td_s32 (*fn_drv_vdec_flush)(td_handle handle, const ext_vdec_opt_flush *option);

typedef td_s32 (*fn_drv_vdec_decode)(td_handle handle, const ext_vdec_opt_decode *option, ext_vdec_stream *stream,
                                     ext_vdec_frame *frame);

typedef td_s32 (*fn_drv_vdec_get_attr)(td_handle handle, ext_vdec_attr *attr);

typedef td_s32 (*fn_drv_vdec_set_attr)(td_handle handle, const ext_vdec_attr *attr);

typedef td_s32 (*fn_drv_vdec_register_event)(td_handle handle, td_u32 event_map);

typedef td_s32 (*fn_drv_vdec_listen_event)(td_handle handle, const ext_vdec_opt_listen *option,
                                           ext_vdec_event *event_param);

typedef td_s32 (*fn_drv_vdec_queue_stream)(td_handle handle, const ext_vdec_stream *stream);

typedef td_s32 (*fn_drv_vdec_dequeue_stream)(td_handle handle, const ext_vdec_opt_get_stream *option,
                                             ext_vdec_stream *stream);

typedef td_s32 (*fn_drv_vdec_acquire_frame)(td_handle handle, const ext_vdec_opt_acq_frame *option,
                                            ext_vdec_frame *frame, ext_vdec_ext_frm_info *ext_info);

typedef td_s32 (*fn_drv_vdec_release_frame)(td_handle handle, const ext_vdec_frame *frame);

typedef td_s32 (*fn_drv_vdec_get_param)(td_handle handle, ext_vdec_param *param);

typedef td_s32 (*fn_drv_vdec_set_param)(td_handle handle, ext_vdec_param *param);

typedef td_s32 (*fn_drv_vdec_command)(td_handle handle, ext_vdec_cmd_id cmd_id, td_void *param,
                                      td_u32 param_size);

typedef td_s32 (*fn_drv_vdec_set_callback)(td_handle vdec_handle, td_handle user_handle,
                                           fn_drv_vdec_callback fn_cb);

typedef struct {
    fn_drv_vdec_create         pfn_drv_vdec_create;
    fn_drv_vdec_destroy        pfn_drv_vdec_destroy;
    fn_drv_vdec_start          pfn_drv_vdec_start;
    fn_drv_vdec_stop           pfn_drv_vdec_stop;
    fn_drv_vdec_reset          pfn_drv_vdec_reset;
    fn_drv_vdec_flush          pfn_drv_vdec_flush;
    fn_drv_vdec_decode         pfn_drv_vdec_decode;
    fn_drv_vdec_get_attr       pfn_drv_vdec_get_attr;
    fn_drv_vdec_set_attr       pfn_drv_vdec_set_attr;
    fn_drv_vdec_register_event pfn_drv_vdec_register_event;
    fn_drv_vdec_listen_event   pfn_drv_vdec_listen_event;
    fn_drv_vdec_get_param      pfn_drv_vdec_get_param;
    fn_drv_vdec_set_param      pfn_drv_vdec_set_param;
    fn_drv_vdec_command        pfn_drv_vdec_command;
    fn_drv_vdec_queue_stream   pfn_drv_vdec_queue_stream;
    fn_drv_vdec_dequeue_stream pfn_drv_vdec_dequeue_stream;
    fn_drv_vdec_acquire_frame  pfn_drv_vdec_acquire_frame;
    fn_drv_vdec_release_frame  pfn_drv_vdec_release_frame;
    fn_drv_vdec_set_callback   pfn_drv_vdec_set_callback;
} drv_vdec_export_func;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
