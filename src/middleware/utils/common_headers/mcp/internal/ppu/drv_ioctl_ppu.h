/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ppu ioctl header
 * Author: AuthorNameMagicTag
 * Create: 2021-9-1
 */

#ifndef __DRV_IOCTL_PPU_H__
#define __DRV_IOCTL_PPU_H__

#include "drv_ppu_struct.h"
#include "isp_sharpen_struct.h"
#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define PPU_MAKE_IOCFD(grp, chn) ((((grp)&0xff) << 16) | ((chn)&0xff))

#define PPU_BIT_WIDTH_8            8
#define PPU_BIT_WIDTH_10           10
#define PPU_MAX_SPREAD_COEF        18
#define PPU_ROTATION_ANGLE_90      90
#define PPU_ROTATION_ANGLE_180     180
#define PPU_ROTATION_ANGLE_270     270

typedef struct {
    ext_isp_video_frame_info video_frame;
    td_s32      grp_pipe;
    td_s32      milli_sec;
} ext_ppu_frame_timeout;

typedef struct {
    td_s32 milli_sec;
    ext_isp_video_frame_info video_frame;
} ext_ppu_usr_get_frm_timeout;

typedef struct {
    td_u32     frame_index; /* reserved */
    td_s32     grp_pipe;
    ext_isp_video_frame_info video_frame;
} ext_ppu_get_grp_frame;

typedef struct {
    td_u32     frame_index; /* reserved */
    td_s32     grp_pipe;
    ext_isp_video_frame_info video_frame;
} ext_ppu_release_grp_frame;

typedef struct {
    td_s32 milli_sec;
    ext_isp_video_region_info region_info;   /* information of the region */
    td_u32 luma_data[ISP_LUMA_STAT_NUM * ISP_LUMA_REGION_DATA_NUM];    /* luma data of the region */
} ext_ppu_get_region_luma;

typedef struct {
    ext_ppu_ldc_attr      ldc_attr;
    ext_gdc_firmware_attr gdc_firm_attr;
} ext_ppu_gdc_ldc_attr;

#ifdef SOCT_GDC_SUPPORT
typedef struct {
    ext_isp_rotation      rotation;
    ext_gdc_fw_pmf_param  gdc_fw_pmf_params;
    ext_gdc_firmware_attr gdc_firm_attr;
} ext_ppu_gdc_rotation_attr;

typedef struct {
    ext_gdc_fw_pmf_param     gdc_fw_pmf_params;
    ext_gdc_firmware_attr    gdc_firm_attr;
} ext_ppu_gdc_rotationex_attr;

typedef struct {
    ext_gdc_spread_attr    spread_attr;
    ext_gdc_fw_pmf_param   gdc_fw_pmf_params;
    ext_gdc_firmware_attr  gdc_firm_attr;
} ext_ppu_gdc_spread_attr;
#endif

typedef struct {
    td_bool enable;
    td_void *regs;
    td_void *nrc_regs;
} ext_ppu_img_quality_cfg;

typedef struct {
    td_s32 msec;
    ext_vb_user_buffer vb;
} ext_ppu_extbuf_arg;

typedef enum {
    IOC_PPU_DEV_INIT,
    IOC_PPU_DEV_DEINIT,
    IOC_PPU_GRP_GETDEFAULT_ATTR,
    IOC_PPU_GRP_CREATE,
    IOC_PPU_GRP_DESTROY,
    IOC_PPU_GRP_ATTR_SET,
    IOC_PPU_GRP_ATTR_GET,
    IOC_PPU_GRP_START,
    IOC_PPU_GRP_STOP,
    IOC_PPU_GRP_RESET,
    IOC_PPU_GRP_SET_DELAY,
    IOC_PPU_GRP_GET_DELAY,
    IOC_PPU_SET_GRP_NRX_PARAM,
    IOC_PPU_GET_GRP_NRX_PARAM,
    IOC_PPU_SET_GRP_WNR_PARAM,
    IOC_PPU_GET_GRP_WNR_PARAM,
    IOC_PPU_SET_GRP_SHARPEN_PARAM,
    IOC_PPU_GET_GRP_SHARPEN_PARAM,
    IOC_PPU_SET_GRP_FSD_PARAM,
    IOC_PPU_GET_GRP_FSD_PARAM,
    IOC_PPU_CHN_ATTR_GETDEFAULT,
    IOC_PPU_CHN_ATTR_SET,
    IOC_PPU_CHN_ATTR_GET,
    IOC_PPU_CHN_ENABLE,
    IOC_PPU_CHN_DISABLE,
    IOC_PPU_USER_SENDFRAME,
    IOC_PPU_GET_REGION_LUMA,
    IOC_PPU_COVER_SET_ATTR,
    IOC_PPU_COVER_GET_ATTR,
    IOC_PPU_CORNER_SET_ATTR,
    IOC_PPU_CORNER_GET_ATTR,
    IOC_PPU_USER_GETFRAME,
    IOC_PPU_USER_RELEASEFRAME,
    IOC_PPU_USER_GETGROUPFRAME,
    IOC_PPU_USER_RELEASEGROUPFRAME,
    IOC_PPU_SET_EXTCHN_ATTR,
    IOC_PPU_GET_EXTCHN_ATTR,
    IOC_PPU_BIND_FLAG2FD,
    IOC_PPU_CHN_LDC_ATTR_SET,
    IOC_PPU_CHN_LDC_ATTR_GET,
    IOC_PPU_CHN_PROC_MODE_SET,
    IOC_PPU_CHN_PROC_MODE_GET,
    IOC_PPU_CHN_OUTPUT_SIZE_GET,
    IOC_PPU_SET_WORKMODE,
    IOC_PPU_GET_WORKMODE,
    IOC_PPU_CHN_SET_ATTACH,
    IOC_PPU_CHN_SET_DETACH,
    IOC_PPU_ATTACH_VBPOOL,
    IOC_PPU_DETACH_VBPOOL
} ioc_ppu;

#define PPU_DEV_INIT                   _IO(SOC_ID_PPU, IOC_PPU_DEV_INIT)
#define PPU_DEV_DEINIT                 _IO(SOC_ID_PPU, IOC_PPU_DEV_DEINIT)
#define PPU_BIND_FLAG2FD               _IOW(SOC_ID_PPU, IOC_PPU_BIND_FLAG2FD, td_u32)

#define PPU_GRP_ATTR_SET_CTRL          _IOW(SOC_ID_PPU, IOC_PPU_GRP_ATTR_SET, ext_ppu_grp_attr)
#define PPU_GRP_ATTR_GET_CTRL          _IOR(SOC_ID_PPU, IOC_PPU_GRP_ATTR_GET, ext_ppu_grp_attr)

#define PPU_GRP_START_CTRL             _IO(SOC_ID_PPU, IOC_PPU_GRP_START)
#define PPU_GRP_STOP_CTRL              _IO(SOC_ID_PPU, IOC_PPU_GRP_STOP)

#define PPU_GRP_RESET_CTRL             _IO(SOC_ID_PPU, IOC_PPU_GRP_RESET)

#define PPU_CHN_ATTR_GET_DEFAULT        _IOR(SOC_ID_PPU, IOC_PPU_CHN_ATTR_GETDEFAULT, ext_ppu_chn_attr)
#define PPU_CHN_ATTR_SET_CTRL          _IOW(SOC_ID_PPU, IOC_PPU_CHN_ATTR_SET, ext_ppu_chn_attr)
#define PPU_CHN_ATTR_GET_CTRL          _IOR(SOC_ID_PPU, IOC_PPU_CHN_ATTR_GET, ext_ppu_chn_attr)

#define PPU_CHN_ENABLE_CTRL            _IO(SOC_ID_PPU, IOC_PPU_CHN_ENABLE)
#define PPU_CHN_DISABLE_CTRL           _IO(SOC_ID_PPU, IOC_PPU_CHN_DISABLE)

#define PPU_GRP_GET_DEFAULT_ATTR        _IOWR(SOC_ID_PPU, IOC_PPU_GRP_GETDEFAULT_ATTR, ext_ppu_grp_attr)
#define PPU_GRP_CREATE_CTRL            _IOW(SOC_ID_PPU, IOC_PPU_GRP_CREATE, ext_ppu_grp_attr)
#define PPU_GRP_DESTROY_CTRL           _IO(SOC_ID_PPU, IOC_PPU_GRP_DESTROY)

#define PPU_USER_SENDFRAME             _IOW(SOC_ID_PPU, IOC_PPU_USER_SENDFRAME, ext_ppu_frame_timeout)
#define PPU_GET_REGION_LUMA            _IOWR(SOC_ID_PPU, IOC_PPU_GET_REGION_LUMA, ext_isp_get_region_luma)
#define PPU_COVER_SET_ATTR            _IOWR(SOC_ID_PPU, IOC_PPU_COVER_SET_ATTR, ext_ppu_cover_info)
#define PPU_COVER_GET_ATTR            _IOWR(SOC_ID_PPU, IOC_PPU_COVER_GET_ATTR, ext_ppu_cover_info)
#define PPU_CORNER_SET_ATTR            _IOWR(SOC_ID_PPU, IOC_PPU_CORNER_SET_ATTR, ext_ppu_corner_info)
#define PPU_CORNER_GET_ATTR            _IOWR(SOC_ID_PPU, IOC_PPU_CORNER_GET_ATTR, ext_ppu_corner_info)

#define PPU_USER_GETFRAME              _IOWR(SOC_ID_PPU, IOC_PPU_USER_GETFRAME, ext_ppu_usr_get_frm_timeout)
#define PPU_USER_RELEASEFRAME          _IOW(SOC_ID_PPU, IOC_PPU_USER_RELEASEFRAME, ext_isp_video_frame_info)

#define PPU_USER_GETGROUPFRAME         _IOWR(SOC_ID_PPU, IOC_PPU_USER_GETGROUPFRAME, ext_ppu_get_grp_frame)
#define PPU_USER_RELEASEGROUPFRAME     _IOW(SOC_ID_PPU, IOC_PPU_USER_RELEASEGROUPFRAME, ext_ppu_release_grp_frame)

#define PPU_SET_EXTCHN_ATTR            _IOW(SOC_ID_PPU, IOC_PPU_SET_EXTCHN_ATTR, ext_ppu_extchn_attr)
#define PPU_GET_EXTCHN_ATTR            _IOR(SOC_ID_PPU, IOC_PPU_GET_EXTCHN_ATTR, ext_ppu_extchn_attr)
#define PPU_SET_GRP_NRX_PARAM          _IOW(SOC_ID_PPU, IOC_PPU_SET_GRP_NRX_PARAM, ext_pq_grp_nrx_param)
#define PPU_GET_GRP_NRX_PARAM          _IOWR(SOC_ID_PPU, IOC_PPU_GET_GRP_NRX_PARAM, ext_pq_grp_nrx_param)
#define PPU_SET_GRP_WNR_PARAM          _IOW(SOC_ID_PPU, IOC_PPU_SET_GRP_WNR_PARAM, ext_pq_wnr_param)
#define PPU_GET_GRP_WNR_PARAM          _IOWR(SOC_ID_PPU, IOC_PPU_GET_GRP_WNR_PARAM, ext_pq_wnr_param)
#define PPU_SET_GRP_SHARPEN_PARAM      _IOW(SOC_ID_PPU, IOC_PPU_SET_GRP_SHARPEN_PARAM, ext_isp_sharpen_attr)
#define PPU_GET_GRP_SHARPEN_PARAM      _IOWR(SOC_ID_PPU, IOC_PPU_GET_GRP_SHARPEN_PARAM, ext_isp_sharpen_attr)
#define PPU_SET_GRP_FSD_PARAM         _IOW(SOC_ID_PPU, IOC_PPU_SET_GRP_FSD_PARAM, ext_ppu_fsd_attr)
#define PPU_GET_GRP_FSD_PARAM         _IOWR(SOC_ID_PPU, IOC_PPU_GET_GRP_FSD_PARAM, ext_ppu_fsd_attr)

#define PPU_SET_CHN_LDC_ATTR           _IOW(SOC_ID_PPU, IOC_PPU_CHN_LDC_ATTR_SET, ext_ppu_gdc_ldc_attr)
#define PPU_GET_CHN_LDC_ATTR           _IOR(SOC_ID_PPU, IOC_PPU_CHN_LDC_ATTR_GET, ext_ppu_ldc_attr)
#define PPU_CHN_OUTPUT_SIZE_GET_CTRL   _IOR(SOC_ID_PPU, IOC_PPU_CHN_OUTPUT_SIZE_GET, ext_isp_size)
#define PPU_CHN_PROC_MODE_SET_CTRL     _IOW(SOC_ID_PPU, IOC_PPU_CHN_PROC_MODE_SET, ext_ppu_chn_proc_mode)
#define PPU_CHN_PROC_MODE_GET_CTRL     _IOR(SOC_ID_PPU, IOC_PPU_CHN_PROC_MODE_GET, ext_ppu_chn_proc_mode)

#define PPU_CHN_SET_ATTACH             _IOW(SOC_ID_PPU, IOC_PPU_CHN_SET_ATTACH, td_s32)
#define PPU_CHN_SET_DETACH             _IOR(SOC_ID_PPU, IOC_PPU_CHN_SET_DETACH, td_s32)

#define PPU_SET_GRP_WORKMODE          _IOW(SOC_ID_PPU, IOC_PPU_SET_WORKMODE, ext_ppu_work_mode_attr)
#define PPU_GET_GRP_WORKMODE          _IOWR(SOC_ID_PPU, IOC_PPU_GET_WORKMODE, ext_ppu_work_mode_attr)
#define PPU_GRP_ATTACH_VBPOOL_CTRL     _IOW(SOC_ID_PPU, IOC_PPU_ATTACH_VBPOOL, vb_pool)
#define PPU_GRP_DETACH_VBPOOL_CTRL     _IO(SOC_ID_PPU, IOC_PPU_DETACH_VBPOOL)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif