/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __DRV_IOCTL_VIU_H__
#define __DRV_IOCTL_VIU_H__

#include "mpi_mcp_dependency.h"
#include "drv_viu_struct.h"
#include "drv_isp_video_ext.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define viu_check_null_ptr_return(pointer)        \
    do {                                          \
        if ((pointer) == TD_NULL) {               \
            viu_err_trace("para is null ptr.\n"); \
            return SOC_ERR_VIU_INVALID_NULL_PTR;  \
        }                                         \
    } while (0)

#define viu_check_dev_id_return(dev_id)                    \
    do {                                                   \
        if ((dev_id) >= VIU_MAX_DEV_NUM || (dev_id) < 0) { \
            viu_err_trace("dev_id is invalid.\n");         \
            return SOC_ERR_VIU_INVALID_DEVID;              \
        }                                                  \
    } while (0)

#define viu_check_pipe_id_return(pipe_id)                     \
    do {                                                      \
        if ((pipe_id) >= ISP_MAX_PIPE_NUM || (pipe_id) < 0) { \
            viu_err_trace("pipe_id is invalid.\n");           \
            return SOC_ERR_VIU_INVALID_DEVID;                 \
        }                                                     \
    } while (0)

#define viu_check_chn_id_return(chn_id)                    \
    do {                                                   \
        if ((chn_id) >= VIU_MAX_CHN_NUM || (chn_id) < 0) { \
            viu_err_trace("chn_id is invalid.\n");         \
            return SOC_ERR_VIU_INVALID_DEVID;              \
        }                                                  \
    } while (0)

typedef struct {
    td_s32 dev_id;
    td_s32 pipe_id;
} viu_ioctl_dev_attach_param;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;

    soc_mod_id dst_mod;
    td_s32 dst_id;
} viu_ioctl_pipe_attach_param;

typedef struct {
    td_s32 pipe_id;
    td_bool pipe_online;
    td_bool chn_online;
} viu_ioctl_work_mode_param;

// ----------------------------------------------------------------------------------
typedef struct {
    td_s32 milli_sec;
    ext_isp_video_frame_info frame_info;
} ioc_viu_usr_get_yuv_timeout;

typedef struct {
    td_u32 pipe_num;
    td_s32 milli_sec;
    td_u64 raw_info_addr[ISP_WDR_CHN_MAX];
    td_s32 pipe_id[ISP_WDR_CHN_MAX];
} ioc_viu_usr_send_raw_timeout;

typedef struct {
    td_s32 pipe_id;
    ioc_viu_usr_send_raw_timeout send_raw_timeout;
} viu_ioctl_send_raw_timeout;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
} viu_ioctl_pipe_chn_id;

typedef struct {
    td_s32 dev_id;
    ext_viu_dev_attr dev_attr;
} viu_ioctl_dev_attr;

typedef struct {
    td_s32 dev_id;
    ext_viu_dev_bind_pipe dev_bind_pipe;
} viu_ioctl_dev_bind_pipe;

typedef struct {
    td_s32 dev_id;
    ext_viu_dev_timing_attr dev_timing_attr;
} viu_ioctl_dev_timing_attr;

typedef struct {
    td_s32 pipe_id;
    ext_viu_pipe_attr pipe_attr;
} viu_ioctl_pipe_attr;

typedef struct {
    td_s32 pipe_id;
    ext_viu_dynamic_attr dynamic_attr;
} viu_ioctl_dynamic_attr;

typedef struct {
    td_s32 pipe_id;
    ext_snap_attr snap_attr;
} viu_ioctl_snap_attr;

typedef struct {
    td_s32 pipe_id;
    ext_viu_dump_attr dump_attr;
} viu_ioctl_dump_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 milli_sec;
    ext_isp_video_frame_info frame_info;
} viu_ioctl_isp_video_frame_info;

typedef struct {
    td_s32 pipe_id;
    ext_viu_pipe_nrx_param pipe_nrx_param;
} viu_ioctl_pipe_nrx_param;

typedef struct {
    td_s32 pipe_id;
    ext_viu_fpn_attr fpn_attr;
} viu_ioctl_fpn_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ext_viu_chn_attr chn_attr;
} viu_ioctl_chn_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    viu_gdc_ldc_attr gdc_ldc_attr;
} viu_ioctl_gdc_ldc_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ext_dis_config dis_config;
} viu_ioctl_dis_config;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ext_dis_attr dis_attr;
} viu_ioctl_dis_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ext_dis_param dis_param;
} viu_ioctl_dis_param;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ext_viu_ext_chn_attr ext_chn_attr;
} viu_ioctl_ext_chn_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 chn_id;
    ioc_viu_usr_get_yuv_timeout get_yuv_timeout;
} viu_ioctl_get_yuv_timeout;

typedef enum {
    IOC_VIU_INIT,
    IOC_VIU_DEINIT,

    IOC_VIU_DEV_ATTR_SET,
    IOC_VIU_DEV_ATTR_GET,
    IOC_VIU_DEV_ENABLE,
    IOC_VIU_DEV_DISABLE,
    IOC_VIU_DEV_ATTACH,
    IOC_VIU_DEV_DETACH,
    IOC_VIU_DEV_SET_TIMING_ATTR,
    IOC_VIU_DEV_GET_TIMING_ATTR,

    IOC_VIU_PIPE_SET_WORK_MODE,
    IOC_VIU_PIPE_GET_WORK_MODE,
    IOC_VIU_PIPE_SNAP_SUSPEND,
    IOC_VIU_PIPE_SNAP_RESUME,
    IOC_VIU_PIPE_CREATE,
    IOC_VIU_PIPE_DESTROY,
    IOC_VIU_PIPE_ATTR_SET,
    IOC_VIU_PIPE_ATTR_GET,
    IOC_VIU_PIPE_START,
    IOC_VIU_PIPE_STOP,
    IOC_VIU_PIPE_DUMP_ATTR_SET,
    IOC_VIU_PIPE_DUMP_ATTR_GET,
    IOC_VIU_PIPE_RAW_GET,
    IOC_VIU_PIPE_RAW_SEND,
    IOC_VIU_PIPE_QUEUE_FRAME,
    IOC_VIU_PIPE_DEQUEUE_FRAME,
    IOC_VIU_PIPE_WAIT_FRAME_DONE,
    IOC_VIU_ZSL_RAW_GET,
    IOC_VIU_DYNAMIC_ATTR_SET,
    IOC_VIU_DYNAMIC_ATTR_GET,

    IOC_VIU_PIPE_SNAP_ATTR_SET,
    IOC_VIU_PIPE_SNAP_ATTR_GET,
    IOC_VIU_PIPE_SNAP_ENABLE,
    IOC_VIU_PIPE_SNAP_DISABLE,
    IOC_VIU_PIPE_SNAP_TRIGGER,

    IOC_VIU_PIPE_NRX_PARAM_SET,
    IOC_VIU_PIPE_NRX_PARAM_GET,
    IOC_VIU_PIPE_SET_FPN_ATTR,
    IOC_VIU_PIPE_GET_FPN_ATTR,

    IOC_VIU_PIPE_ATTACH,
    IOC_VIU_PIPE_DETACH,
    IOC_VIU_CHN_ATTR_SET,
    IOC_VIU_CHN_ATTR_GET,
    IOC_VIU_CHN_ENABLE,
    IOC_VIU_CHN_DISABLE,
    IOC_VIU_EXTCHN_ATTR_SET,
    IOC_VIU_EXTCHN_ATTR_GET,
    IOC_VIU_CHN_FRAME_GET,
    IOC_VIU_MOD_FRAME_RELEASE,
    IOC_VIU_DSF_FRAME_GET,

    IOC_VIU_CHN_GDC_LDC_SET,
    IOC_VIU_CHN_GDC_LDC_GET,
    IOC_VIU_CHN_DIS_PARAM_SET,
    IOC_VIU_CHN_DIS_PARAM_GET,
    IOC_VIU_CHN_DIS_ATTR_SET,
    IOC_VIU_CHN_DIS_ATTR_GET,
    IOC_VIU_CHN_DIS_CONFIG_SET,
    IOC_VIU_CHN_DIS_CONFIG_GET,
    IOC_VIU_CHN_LDC_DRV_ATTR_GET,
} ioc_viu;

#define VIU_INIT_CTRL                       _IO(SOC_ID_VIU, IOC_VIU_INIT)
#define VIU_DEINIT_CTRL                     _IO(SOC_ID_VIU, IOC_VIU_DEINIT)

// dev
#define VIU_DEV_ATTR_SET_CTRL               _IOW(SOC_ID_VIU, IOC_VIU_DEV_ATTR_SET, viu_ioctl_dev_attr)
#define VIU_DEV_ATTR_GET_CTRL               _IOWR(SOC_ID_VIU, IOC_VIU_DEV_ATTR_GET, viu_ioctl_dev_attr)

#define VIU_DEV_ENABLE_CTRL                 _IOW(SOC_ID_VIU, IOC_VIU_DEV_ENABLE, td_s32)
#define VIU_DEV_DISABLE_CTRL                _IOW(SOC_ID_VIU, IOC_VIU_DEV_DISABLE, td_s32)

#define VIU_DEV_ATTACH                      _IOW(SOC_ID_VIU, IOC_VIU_DEV_ATTACH, viu_ioctl_dev_attach_param)
#define VIU_DEV_DETACH                      _IOW(SOC_ID_VIU, IOC_VIU_DEV_DETACH, viu_ioctl_dev_attach_param)

#define VIU_DEV_SET_TIMING_ATTR_CTRL        _IOW(SOC_ID_VIU, IOC_VIU_DEV_SET_TIMING_ATTR, viu_ioctl_dev_timing_attr)
#define VIU_DEV_GET_TIMING_ATTR_CTRL        _IOWR(SOC_ID_VIU, IOC_VIU_DEV_GET_TIMING_ATTR, viu_ioctl_dev_timing_attr)

// pipe
#define VIU_PIPE_SET_WORK_MODE              _IOW(SOC_ID_VIU, IOC_VIU_PIPE_SET_WORK_MODE, viu_ioctl_work_mode_param)
#define VIU_PIPE_GET_WORK_MODE              _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_GET_WORK_MODE, viu_ioctl_work_mode_param)

#define VIU_PIPE_SNAP_SUSPEND              _IO(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_SUSPEND)
#define VIU_PIPE_SNAP_RESUME               _IO(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_RESUME)

#define VIU_PIPE_CREATE_CTRL                _IOW(SOC_ID_VIU, IOC_VIU_PIPE_CREATE, viu_ioctl_pipe_attr)
#define VIU_PIPE_DESTROY_CTRL               _IOW(SOC_ID_VIU, IOC_VIU_PIPE_DESTROY, td_s32)

#define VIU_PIPE_ATTR_SET_CTRL              _IOW(SOC_ID_VIU, IOC_VIU_PIPE_ATTR_SET, viu_ioctl_pipe_attr)
#define VIU_PIPE_ATTR_GET_CTRL              _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_ATTR_GET, viu_ioctl_pipe_attr)

#define VIU_PIPE_START_CTRL                 _IOW(SOC_ID_VIU, IOC_VIU_PIPE_START, td_s32)
#define VIU_PIPE_STOP_CTRL                  _IOW(SOC_ID_VIU, IOC_VIU_PIPE_STOP, td_s32)

#define VIU_PIPE_DUMP_ATTR_SET_CTRL         _IOW(SOC_ID_VIU, IOC_VIU_PIPE_DUMP_ATTR_SET, viu_ioctl_dump_attr)
#define VIU_PIPE_DUMP_ATTR_GET_CTRL         _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_DUMP_ATTR_GET, viu_ioctl_dump_attr)

#define VIU_PIPE_RAW_GET_CTRL               _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_RAW_GET, viu_ioctl_isp_video_frame_info)
#define VIU_PIPE_RAW_SEND_CTRL              _IOW(SOC_ID_VIU, IOC_VIU_PIPE_RAW_SEND, viu_ioctl_send_raw_timeout)
#define VIU_ZSL_RAW_GET_CTRL                _IOWR(SOC_ID_VIU, IOC_VIU_ZSL_RAW_GET, viu_ioctl_isp_video_frame_info)

#define VIU_PIPE_QUEUE_FRAME                _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_QUEUE_FRAME, viu_ioctl_isp_video_frame_info)
#define VIU_PIPE_DEQUEUE_FRAME              _IOW(SOC_ID_VIU, IOC_VIU_PIPE_DEQUEUE_FRAME, viu_ioctl_send_raw_timeout)
#define VIU_PIPE_WAIT_FRAME_DONE              _IOW(SOC_ID_VIU, IOC_VIU_PIPE_WAIT_FRAME_DONE, viu_ioctl_send_raw_timeout)

#define VIU_DYNAMIC_ATTR_SET_CTRL           _IOW(SOC_ID_VIU, IOC_VIU_DYNAMIC_ATTR_SET, viu_ioctl_dynamic_attr)
#define VIU_DYNAMIC_ATTR_GET_CTRL           _IOWR(SOC_ID_VIU, IOC_VIU_DYNAMIC_ATTR_GET, viu_ioctl_dynamic_attr)

// snap
#define VIU_PIPE_SNAP_ATTR_SET_CTRL         _IOW(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_ATTR_SET, viu_ioctl_snap_attr)
#define VIU_PIPE_SNAP_ATTR_GET_CTRL         _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_ATTR_GET, viu_ioctl_snap_attr)

#define VIU_PIPE_SNAP_ENABLE_CTRL           _IOW(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_ENABLE, td_s32)
#define VIU_PIPE_SNAP_DISABLE_CTRL          _IOW(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_DISABLE, td_s32)
#define VIU_PIPE_SNAP_TRIGGER_CTRL          _IOW(SOC_ID_VIU, IOC_VIU_PIPE_SNAP_TRIGGER, td_s32)

// chn
#define VIU_PIPE_ATTACH                     _IOW(SOC_ID_VIU, IOC_VIU_PIPE_ATTACH, viu_ioctl_pipe_attach_param)
#define VIU_PIPE_DETACH                     _IOW(SOC_ID_VIU, IOC_VIU_PIPE_DETACH, viu_ioctl_pipe_attach_param)

#define VIU_CHN_ATTR_SET_CTRL               _IOW(SOC_ID_VIU, IOC_VIU_CHN_ATTR_SET, viu_ioctl_chn_attr)
#define VIU_CHN_ATTR_GET_CTRL               _IOWR(SOC_ID_VIU, IOC_VIU_CHN_ATTR_GET, viu_ioctl_chn_attr)

#define VIU_CHN_ENABLE_CTRL                 _IOW(SOC_ID_VIU, IOC_VIU_CHN_ENABLE, viu_ioctl_pipe_chn_id)
#define VIU_CHN_DISABLE_CTRL                _IOW(SOC_ID_VIU, IOC_VIU_CHN_DISABLE, viu_ioctl_pipe_chn_id)

#define VIU_EXTCHN_ATTR_SET_CTRL            _IOW(SOC_ID_VIU, IOC_VIU_EXTCHN_ATTR_SET, viu_ioctl_ext_chn_attr)
#define VIU_EXTCHN_ATTR_GET_CTRL            _IOWR(SOC_ID_VIU, IOC_VIU_EXTCHN_ATTR_GET, viu_ioctl_ext_chn_attr)

#define VIU_CHN_FRAME_GET_CTRL              _IOWR(SOC_ID_VIU, IOC_VIU_CHN_FRAME_GET, viu_ioctl_get_yuv_timeout)
#define VIU_MOD_FRAME_RELEASE_CTRL          _IOWR(SOC_ID_VIU, IOC_VIU_MOD_FRAME_RELEASE, viu_ioctl_isp_video_frame_info)
#define VIU_DSF_FRAME_GET_CTRL              _IOWR(SOC_ID_VIU, IOC_VIU_DSF_FRAME_GET, viu_ioctl_get_yuv_timeout)

// pipe_alg
#define VIU_PIPE_NRX_PARAM_SET_CTRL         _IOW(SOC_ID_VIU, IOC_VIU_PIPE_NRX_PARAM_SET, viu_ioctl_pipe_nrx_param)
#define VIU_PIPE_NRX_PARAM_GET_CTRL         _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_NRX_PARAM_GET, viu_ioctl_pipe_nrx_param)

#define VIU_PIPE_SET_FPN_ATTR_CTRL          _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_SET_FPN_ATTR, viu_ioctl_fpn_attr)
#define VIU_PIPE_GET_FPN_ATTR_CTRL          _IOWR(SOC_ID_VIU, IOC_VIU_PIPE_GET_FPN_ATTR, viu_ioctl_fpn_attr)

// chn_alg
#define VIU_CHN_GDC_LDC_SET_CTRL            _IOW(SOC_ID_VIU, IOC_VIU_CHN_GDC_LDC_SET, viu_ioctl_gdc_ldc_attr)
#define VIU_CHN_GDC_LDC_GET_CTRL            _IOWR(SOC_ID_VIU, IOC_VIU_CHN_GDC_LDC_GET, viu_ioctl_gdc_ldc_attr)
#define VIU_CHN_GDC_LDC_DRV_ATTR_GET_CTRL   _IOWR(SOC_ID_VIU, IOC_VIU_CHN_LDC_DRV_ATTR_GET, viu_ioctl_gdc_ldc_attr)

#define VIU_CHN_DIS_CONFIG_SET_CTRL         _IOW(SOC_ID_VIU, IOC_VIU_CHN_DIS_CONFIG_SET, viu_ioctl_dis_config)
#define VIU_CHN_DIS_CONFIG_GET_CTRL         _IOWR(SOC_ID_VIU, IOC_VIU_CHN_DIS_CONFIG_GET, viu_ioctl_dis_config)
#define VIU_CHN_DIS_ATTR_SET_CTRL           _IOW(SOC_ID_VIU, IOC_VIU_CHN_DIS_ATTR_SET, viu_ioctl_dis_attr)
#define VIU_CHN_DIS_ATTR_GET_CTRL           _IOWR(SOC_ID_VIU, IOC_VIU_CHN_DIS_ATTR_GET, viu_ioctl_dis_attr)
#define VIU_CHN_DIS_PARAM_SET_CTRL          _IOW(SOC_ID_VIU, IOC_VIU_CHN_DIS_PARAM_SET, viu_ioctl_dis_param)
#define VIU_CHN_DIS_PARAM_GET_CTRL          _IOWR(SOC_ID_VIU, IOC_VIU_CHN_DIS_PARAM_GET, viu_ioctl_dis_param)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __DRV_IOCTL_VIU_H__ */

