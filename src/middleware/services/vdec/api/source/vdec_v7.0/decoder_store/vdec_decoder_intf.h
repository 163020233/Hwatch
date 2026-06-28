/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __VDEC_DECODER_INTF_H__
#define __VDEC_DECODER_INTF_H__

#include "drv_ioctl_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VDEC_DECODER_NAME_LEN 30

typedef td_s32 (*fn_decoder_create)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_destroy)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_start)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_stop)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_reset)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_flush)
    (td_s32 chan_id, const ext_vdec_opt_flush *option);

typedef td_s32 (*fn_decoder_decode)
    (td_s32 chan_id, const ext_vdec_opt_decode *option, const ext_vdec_stream *stream, ext_vdec_frame *frame);

typedef td_s32 (*fn_decoder_get_attr)
    (td_s32 chan_id, ext_vdec_attr *attr);

typedef td_s32 (*fn_decoder_set_attr)
    (td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr);

typedef td_s32 (*fn_decoder_register_event)
    (td_s32 chan_id, td_u64 event_map);

typedef td_s32 (*fn_decoder_listen_event)
    (td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event);

typedef td_s32 (*fn_decoder_queue_stream)
    (td_s32 chan_id, const ext_vdec_stream *stream);

typedef td_s32 (*fn_decoder_dequeue_stream)
    (td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream);

typedef td_s32 (*fn_decoder_acquire_frame)
    (td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame);

typedef td_s32 (*fn_decoder_release_frame)
    (td_s32 chan_id, const ext_vdec_frame *frame);

typedef td_s32 (*fn_decoder_get_param)
    (td_s32 chan_id, ext_vdec_param *param);

typedef td_s32 (*fn_decoder_set_param)
    (td_s32 chan_id, const ext_vdec_param *param);

typedef td_s32 (*fn_decoder_get_feature)
    (td_s32 chan_id, ext_vdec_feature *feature);

typedef td_s32 (*fn_decoder_get_status)
    (td_s32 chan_id, ext_vdec_status *status);

typedef td_s32 (*fn_decoder_is_std_support)
    (ext_vdec_opt_is_std_support *support);

typedef td_s32 (*fn_decoder_get_capability)
    (ext_vdec_capability *capability);

typedef td_s32 (*fn_decoder_send_eos)
    (td_s32 chan_id);

typedef td_s32 (*fn_decoder_bind_frm_buffer)
    (td_s32 chan_id, const ext_vdec_frame_buf *buffer);

typedef struct {
    fn_decoder_create                   create;
    fn_decoder_destroy                  destroy;
    fn_decoder_start                    start;
    fn_decoder_stop                     stop;
    fn_decoder_reset                    reset;
    fn_decoder_flush                    flush;
    fn_decoder_decode                   decode;
    fn_decoder_get_attr                 get_attr;
    fn_decoder_set_attr                 set_attr;
    fn_decoder_register_event           register_event;
    fn_decoder_listen_event             listen_event;
    fn_decoder_queue_stream             queue_stream;
    fn_decoder_dequeue_stream           dequeue_stream;
    fn_decoder_acquire_frame            acq_frame;
    fn_decoder_release_frame            rls_frame;
    fn_decoder_get_param                get_param;
    fn_decoder_set_param                set_param;
    fn_decoder_get_feature              get_feature;
    fn_decoder_get_status               get_status;
    fn_decoder_is_std_support           std_support;
    fn_decoder_get_capability           get_cap;
    fn_decoder_send_eos                 send_eos;
    fn_decoder_bind_frm_buffer          bind_frm_buf;
} vdec_decoder_intf;

typedef struct {
    td_char name[VDEC_DECODER_NAME_LEN];
    vdec_decoder_intf intf;
} vdec_decoder;


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
