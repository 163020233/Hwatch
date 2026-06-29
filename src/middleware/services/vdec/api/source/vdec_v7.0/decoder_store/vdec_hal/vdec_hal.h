/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __VDEC_HAL_H__
#define __VDEC_HAL_H__

#include "drv_ioctl_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 vdec_hal_open(td_void);
td_s32 vdec_hal_close(td_void);
td_s32 vdec_hal_create(td_s32 *chan_id);
td_s32 vdec_hal_destroy(td_s32 chan_id);
td_s32 vdec_hal_start(td_s32 chan_id);
td_s32 vdec_hal_stop(td_s32 chan_id);
td_s32 vdec_hal_reset(td_s32 chan_id);
td_s32 vdec_hal_flush(td_s32 chan_id, const ext_vdec_opt_flush *option);
td_s32 vdec_hal_decode(td_s32 chan_id, const ext_vdec_opt_decode *option,
    const ext_vdec_stream *stream, ext_vdec_frame *frame);
td_s32 vdec_hal_get_attr(td_s32 chan_id, ext_vdec_attr *attr);
td_s32 vdec_hal_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr);
td_s32 vdec_hal_register_event(td_s32 chan_id, td_u64 event_map);
td_s32 vdec_hal_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event);
td_s32 vdec_hal_put_stream(td_s32 chan_id, const ext_vdec_stream *stream);
td_s32 vdec_hal_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream);
td_s32 vdec_hal_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame);
td_s32 vdec_hal_release_frame(td_s32 chan_id, const ext_vdec_frame *frame);
td_s32 vdec_hal_get_param(td_s32 chan_id, ext_vdec_param *param);
td_s32 vdec_hal_set_param(td_s32 chan_id, const ext_vdec_param *param);
td_s32 vdec_hal_get_feature(td_s32 chan_id, ext_vdec_feature *feature);
td_s32 vdec_hal_get_status(td_s32 chan_id, ext_vdec_status *status);
td_s32 vdec_hal_command(td_s32 chan_id, ext_vdec_cmd_id cmd_id, td_void *param, td_u32 param_size);
td_s32 vdec_hal_get_cap(ext_vdec_capability *cap);
td_s32 vdec_hal_is_std_support(ext_vdec_opt_is_std_support *support);
td_s32 vdec_hal_send_eos(td_s32 chan_id);
td_s32 vdec_hal_bind_frm_buffer(td_s32 chan_id, const ext_vdec_frame_buf *buffer);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
