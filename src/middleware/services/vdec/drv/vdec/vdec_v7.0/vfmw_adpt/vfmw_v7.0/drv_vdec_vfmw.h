/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_VFMW_H__
#define __DRV_VDEC_VFMW_H__

#include "drv_ioctl_vdec.h"
#include "drv_vdec_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef EXT_MMZ_INVALID_HANDLE
#define EXT_MMZ_INVALID_HANDLE (-1)
#endif
#define MAX_VID_INPUT_BUFFER_SIZE   (128 * 1024 * 1024)

typedef td_s32 (*fn_vdec_vfmw_callback)(td_s32 chan_id, td_u32 type, const td_void *args, td_u32 size);

typedef struct {
    td_handle vdec_handle;
    td_s32 vdec_id;
    td_handle ssm_handle;
    ext_vdec_attr *attr;
    fn_vdec_vfmw_callback callback;
} vdec_vfmw_create_param;

typedef struct {
    td_bool enable_uvmos;
    td_bool enable_low_delay;
    ext_vdec_dpb_mode dpb_mode;
    ext_vdec_hdr_attr hdr_attr;
    ext_vdec_stm_buf_mode stm_mode;
    ext_vdec_frame_buf_mode frm_mode;
    ext_vdec_out_format out_format;
    td_u32  poc_thrshold;
    ext_vdec_src_tag source_tag;
    td_bool is_pvr_info_valid;
    ext_vdec_pvr_info pvr_info;
    td_u32 frm_3d_type;
    ext_vdec_speed speed_info;
    ext_vdec_frame_rate frame_rate;
    ext_vdec_start_pts start_pts;
    ext_vdec_decode_mode dec_mode;
} vdec_vfmw_param;

td_s32 vdec_vfmw_mod_init(td_void);
td_void vdec_vfmw_mod_exit(td_void);
td_s32 vdec_vfmw_open(td_void);
td_s32 vdec_vfmw_close(td_void);
td_s32 vdec_vfmw_init(td_void);
td_s32 vdec_vfmw_exit(td_void);
td_s32 vdec_vfmw_create(td_s32 *chan_id, const vdec_vfmw_create_param *param);
td_s32 vdec_vfmw_destroy(td_s32 chan_id);
td_s32 vdec_vfmw_start(td_s32 chan_id);
td_s32 vdec_vfmw_stop(td_s32 chan_id);
td_s32 vdec_vfmw_reset(td_s32 chan_id);
td_s32 vdec_vfmw_flush(td_s32 chan_id, const ext_vdec_opt_flush *param);
td_s32 vdec_vfmw_set_event_map(td_s32 chan_id, td_u64 event_map);
td_s32 vdec_vfmw_decode(td_s32 chan_id, const ext_vdec_opt_decode *option,
    const ext_vdec_stream *stream, ext_vdec_frame *frame);
td_s32 vdec_vfmw_set_attr(td_s32 chan_id, const ext_vdec_attr *param, const vdec_vfmw_param *ext_attr);
td_s32 vdec_vfmw_put_stream(td_s32 chan_id, const ext_vdec_stream *stream);
td_s32 vdec_vfmw_get_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option, ext_vdec_stream *stream);
td_s32 vdec_vfmw_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame,
    ext_vdec_ext_frm_info *ext_frm_info);
td_s32 vdec_vfmw_release_frame(td_s32 chan_id, const ext_vdec_frame *frame);
td_s32 vdec_vfmw_suspend(td_void);
td_s32 vdec_vfmw_resume(td_void);
td_s32 vdec_vfmw_freeze(td_void);
td_s32 vdec_vfmw_restore(td_void);
td_s32 vdec_vfmw_wakeup(td_s32 chan_id, td_u8 *param, td_u32 size);
td_s32 vdec_vfmw_read_proc(td_void *buf, td_u32 len, td_u32 *used, td_s32 chan_id);
td_s32 vdec_vfmw_get_low_delay(td_s32 chan_id, td_bool *low_delay);
td_s32 vdec_vfmw_set_low_delay(td_s32 chan_id, td_bool low_delay);
td_s32 vdec_vfmw_get_frame_rate(td_s32 chan_id, ext_vdec_frame_rate *frame_rate);
td_s32 vdec_vfmw_set_frame_rate(td_s32 chan_id, ext_vdec_frame_rate *frame_rate);
td_s32 vdec_vfmw_get_feature(td_s32 chan_id, ext_vdec_feature *feature);
td_s32 vdec_vfmw_get_status(td_s32 chan_id, ext_vdec_status *status);
td_s32 vdec_vfmw_send_eos(td_s32 chan_id);
td_s32 vdec_vfmw_init_input_buf(td_s32 chan_id, td_u32 len);
td_s32 vdec_vfmw_deinit_input_buf(td_s32 chan_id);
td_s32 vdec_vfmw_get_cap(ext_vdec_capability *cap);
td_s32 vdec_vfmw_is_std_support(ext_vdec_opt_is_std_support *support);
td_s32 vdec_vfmw_command(td_s32 chan_id, ext_vdec_cmd_id cmd, td_u8 *param, td_u32 size);
td_s32 vdec_vfmw_bind_extern_frame_buffer(td_s32 chan_id, ext_vdec_frame_buf *buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
