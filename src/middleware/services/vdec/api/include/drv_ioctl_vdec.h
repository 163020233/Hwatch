/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-05-16
 */
#ifndef __DRV_VDEC_IOCTL_H__
#define __DRV_VDEC_IOCTL_H__

#include "drv_vdec_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VDEC_DEV_NAME "soc_vdec"
#define VDEC_DEV_PATH "/user"

#define VDEC_IOCTL_BUF_SIZE 256

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_create;

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_destroy;

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_start;

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_stop;

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_reset;

typedef struct {
    td_s32 chan_id;
    ext_vdec_opt_flush option;
} vdec_ioctl_flush;

typedef struct {
    td_s32 chan_id;
    ext_vdec_opt_decode option;
    ext_vdec_stream stream;
    ext_vdec_frame frame;
} vdec_ioctl_decode;

typedef struct {
    td_s32 chan_id;
    ext_vdec_attr attr;
} vdec_ioctl_get_attr;

typedef struct {
    td_s32 chan_id;
    td_handle vdec_handle;
    ext_vdec_attr attr;
} vdec_ioctl_set_attr;

typedef struct {
    td_s32 chan_id;
    td_u64 event_map;
} vdec_ioctl_register;

typedef struct {
    td_s32 chan_id;
    ext_vdec_opt_listen option;
    ext_vdec_event event;
} vdec_ioctl_listen;

typedef struct {
    td_s32 chan_id;
    ext_vdec_stream stream;
} vdec_ioctl_put_stream;

typedef struct {
    td_s32 chan_id;
    ext_vdec_opt_get_stream option;
    ext_vdec_stream stream;
} vdec_ioctl_get_stream;

typedef struct {
    td_s32 chan_id;
    ext_vdec_opt_acq_frame option;
    ext_vdec_frame frame;
    ext_vdec_ext_frm_info ext_frm_info;
} vdec_ioctl_acq_frame;

typedef struct {
    td_s32 chan_id;
    ext_vdec_frame frame;
} vdec_ioctl_rel_frame;

typedef struct {
    td_s32 chan_id;
    ext_vdec_param param;
} vdec_ioctl_param;

typedef struct {
    td_s32 chan_id;
    ext_vdec_feature feature;
} vdec_ioctl_feature;

typedef struct {
    td_s32 chan_id;
    ext_vdec_status status;
} vdec_ioctl_status;

typedef struct {
    ext_vdec_opt_is_std_support support_info;
} vdec_ioctl_std_support;

typedef struct {
    ext_vdec_capability cap;
} vdec_ioctl_capability;

typedef struct {
    td_s32 chan_id;
} vdec_ioctl_send_eos;

typedef struct {
    td_s32 chan_id;
    ext_vdec_cmd_id cmd_id;
    td_u8 param[VDEC_IOCTL_BUF_SIZE];
    td_u32 param_size;
} vdec_ioctl_command;

typedef struct {
    td_s32 chan_id;
    ext_vdec_frame_buf buffer;
} vdec_ioctl_bind_frm_buf;

#ifdef GFX_LINUX
#define UMAPC_VDEC_CREATE             _IOWR(SOC_ID_VDEC, 1,  vdec_ioctl_create)
#define UMAPC_VDEC_DESTROY            _IOW (SOC_ID_VDEC, 2,  vdec_ioctl_destroy)
#define UMAPC_VDEC_START              _IOW (SOC_ID_VDEC, 3,  vdec_ioctl_start)
#define UMAPC_VDEC_STOP               _IOW (SOC_ID_VDEC, 4,  vdec_ioctl_stop)
#define UMAPC_VDEC_RESET              _IOW (SOC_ID_VDEC, 5,  vdec_ioctl_reset)
#define UMAPC_VDEC_FLUSH              _IOW (SOC_ID_VDEC, 6,  vdec_ioctl_flush)
#define UMAPC_VDEC_DECODE             _IOWR(SOC_ID_VDEC, 7,  vdec_ioctl_decode)
#define UMAPC_VDEC_GET_ATTR           _IOWR(SOC_ID_VDEC, 8,  vdec_ioctl_get_attr)
#define UMAPC_VDEC_SET_ATTR           _IOW (SOC_ID_VDEC, 9,  vdec_ioctl_set_attr)
#define UMAPC_VDEC_REGISTER_EVENT     _IOW (SOC_ID_VDEC, 10, vdec_ioctl_register)
#define UMAPC_VDEC_LISTEN_EVENT       _IOWR(SOC_ID_VDEC, 11, vdec_ioctl_listen)
#define UMAPC_VDEC_GET_STREAM         _IOWR(SOC_ID_VDEC, 12, vdec_ioctl_get_stream)
#define UMAPC_VDEC_PUT_STREAM         _IOW (SOC_ID_VDEC, 13, vdec_ioctl_put_stream)
#define UMAPC_VDEC_ACQUIRE_FRAME      _IOWR(SOC_ID_VDEC, 14, vdec_ioctl_acq_frame)
#define UMAPC_VDEC_RELEASE_FRAME      _IOW (SOC_ID_VDEC, 15, vdec_ioctl_rel_frame)
#define UMAPC_VDEC_GET_PARAM          _IOWR(SOC_ID_VDEC, 16, vdec_ioctl_param)
#define UMAPC_VDEC_SET_PARAM          _IOW (SOC_ID_VDEC, 17, vdec_ioctl_param)
#define UMAPC_VDEC_GET_FEATURE        _IOW (SOC_ID_VDEC, 18, vdec_ioctl_feature)
#define UMAPC_VDEC_GET_STATUS         _IOW (SOC_ID_VDEC, 19, vdec_ioctl_status)
#define UMAPC_VDEC_IS_STD_SUPPORT     _IOWR(SOC_ID_VDEC, 20, vdec_ioctl_std_support)
#define UMAPC_VDEC_GET_CAP            _IOWR(SOC_ID_VDEC, 21, vdec_ioctl_capability)
#define UMAPC_VDEC_SEND_EOS           _IOWR(SOC_ID_VDEC, 22, vdec_ioctl_send_eos)
#define UMAPC_VDEC_COMMAND            _IOWR(SOC_ID_VDEC, 23, vdec_ioctl_command)
#define UMAPC_VDEC_BIND_FRM_BUF       _IOWR(SOC_ID_VDEC, 24, vdec_ioctl_bind_frm_buf)

#define vdec_open(path_name, flags, mode)      open((path_name), (flags), (mode))
#define vdec_ioctl(module_idx, request, para)  ioctl((module_idx), (request), (para))
#define vdec_close(module_idx)                 close((module_idx))

#else
typedef enum {
    UMAPC_VDEC_CREATE = 0,
    UMAPC_VDEC_DESTROY,
    UMAPC_VDEC_START,
    UMAPC_VDEC_STOP,
    UMAPC_VDEC_RESET,
    UMAPC_VDEC_FLUSH,
    UMAPC_VDEC_DECODE,
    UMAPC_VDEC_GET_ATTR,
    UMAPC_VDEC_SET_ATTR,
    UMAPC_VDEC_REGISTER_EVENT,
    UMAPC_VDEC_LISTEN_EVENT,
    UMAPC_VDEC_GET_STREAM,
    UMAPC_VDEC_PUT_STREAM,
    UMAPC_VDEC_ACQUIRE_FRAME,
    UMAPC_VDEC_RELEASE_FRAME,
    UMAPC_VDEC_GET_PARAM,
    UMAPC_VDEC_SET_PARAM,
    UMAPC_VDEC_GET_FEATURE,
    UMAPC_VDEC_GET_STATUS,
    UMAPC_VDEC_IS_STD_SUPPORT,
    UMAPC_VDEC_GET_CAP,
    UMAPC_VDEC_SEND_EOS,
    UMAPC_VDEC_COMMAND,
    UMAPC_VDEC_BIND_FRM_BUF,
    UMAPC_VDEC_MAX
} vdec_ioctl_cmd;

td_s32 vdec_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 vdec_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 vdec_close(td_s32 module_idx);
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
