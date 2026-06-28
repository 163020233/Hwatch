/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_H__
#define __DRV_VDEC_H__

#include "drv_ioctl_vdec.h"
#include "drv_vdec_osal.h"
#include "drv_vdec_sdk.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ext_vdec_stream *stream;
    ext_vdec_frame *frame;
    ext_vdec_opt_decode option;
} drv_vdec_decode_param;

td_void drv_vdec_init(td_void);
td_void drv_vdec_deinit(td_void);
td_s32 drv_vdec_open(td_void);
td_s32 drv_vdec_close(td_void);
td_s32 drv_vdec_create(td_s32 *chan_id, td_void *private_data);
td_s32 drv_vdec_destroy(td_s32 chan_id);
td_s32 drv_vdec_start(td_s32 chan_id);
td_s32 drv_vdec_stop(td_s32 chan_id);
td_s32 drv_vdec_reset(td_s32 chan_id);
td_s32 drv_vdec_flush(td_s32 chan_id, const ext_vdec_opt_flush *option);
td_s32 drv_vdec_decode(td_s32 chan_id, drv_vdec_decode_param *param);
td_s32 drv_vdec_get_attr(td_s32 chan_id, ext_vdec_attr *attr);
td_s32 drv_vdec_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr);
td_s32 drv_vdec_register_event(td_s32 chan_id, td_u64 event_map);
td_s32 drv_vdec_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event);
td_s32 drv_vdec_put_stream(td_s32 chan_id, const ext_vdec_stream *stream);
td_s32 drv_vdec_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream);
td_s32 drv_vdec_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame,
    ext_vdec_ext_frm_info *ext_frm_info);
td_s32 drv_vdec_release_frame(td_s32 chan_id, const ext_vdec_frame *frame);
td_s32 drv_vdec_get_param(td_s32 chan_id, ext_vdec_param *param);
td_s32 drv_vdec_set_param(td_s32 chan_id, ext_vdec_param *param);
td_s32 drv_vdec_command(td_s32 chan_id, ext_vdec_cmd_id cmd_id, td_u8 *param, td_u32 param_size);
td_s32 drv_vdec_register_callback(td_s32 chan_id, td_handle user_handle, fn_drv_vdec_callback func);
td_s32 drv_vdec_suspend(td_void);
td_s32 drv_vdec_resume(td_void);
td_s32 drv_vdec_freeze(td_void);
td_s32 drv_vdec_restore(td_void);
td_void drv_vdec_release(const td_void *private_data);
td_s32 drv_vdec_read_proc(td_void *buf, td_u32 len, td_u32 *used, td_s32 chan_id);
td_s32 drv_vdec_get_feature(td_s32 chan_id, ext_vdec_feature *feature);
td_s32 drv_vdec_get_status(td_s32 chan_id, ext_vdec_status *status);
td_s32 drv_vdec_is_std_support(ext_vdec_opt_is_std_support *support);
td_s32 drv_vdec_get_cap(ext_vdec_capability *cap);
td_s32 drv_vdec_send_eos(td_s32 chan_id);
td_s32 drv_vdec_bind_frm_buffer(td_s32 chan_id, ext_vdec_frame_buf *buffer);

td_s32 drv_vdec_vfmw_mod_init(td_void);
td_void drv_vdec_vfmw_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
