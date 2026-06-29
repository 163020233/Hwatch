/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __MPI_VIU_CONVERT_H__
#define __MPI_VIU_CONVERT_H__

#include "drv_ioctl_viu.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static inline td_s32 mpi_viu_convert_dev_attr_user_to_drv(td_s32 dev_id,
    const ext_viu_dev_attr* viu_dev_attr, viu_ioctl_dev_attr* ioctl_dev_attr)
{
    td_s32 ret;

    ioctl_dev_attr->dev_id = dev_id;
    ret = memcpy_s(&ioctl_dev_attr->dev_attr, sizeof(ext_viu_dev_attr), viu_dev_attr, sizeof(ext_viu_dev_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dev_attr_drv_to_user(td_s32 dev_id,
    const viu_ioctl_dev_attr* ioctl_dev_attr, ext_viu_dev_attr* viu_dev_attr)
{
    viu_unused(dev_id);

    td_s32 ret;

    ret = memcpy_s(viu_dev_attr, sizeof(ext_viu_dev_attr), &ioctl_dev_attr->dev_attr, sizeof(ext_viu_dev_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dev_timing_attr_user_to_drv(td_s32 dev_id,
    const ext_viu_dev_timing_attr* viu_dev_timing_attr, viu_ioctl_dev_timing_attr* ioctl_dev_timing_attr)
{
    td_s32 ret;

    ioctl_dev_timing_attr->dev_id = dev_id;
    ret = memcpy_s(&ioctl_dev_timing_attr->dev_timing_attr, sizeof(ext_viu_dev_timing_attr),
        viu_dev_timing_attr, sizeof(ext_viu_dev_timing_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dev_timing_attr_drv_to_user(td_s32 dev_id,
    const viu_ioctl_dev_timing_attr* ioctl_dev_timing_attr, ext_viu_dev_timing_attr* viu_dev_timing_attr)
{
    viu_unused(dev_id);

    td_s32 ret;

    ret = memcpy_s(viu_dev_timing_attr, sizeof(ext_viu_dev_timing_attr),
        &ioctl_dev_timing_attr->dev_timing_attr, sizeof(ext_viu_dev_timing_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_pipe_attr_user_to_drv(td_s32 pipe_id,
    const ext_viu_pipe_attr* viu_pipe_attr, viu_ioctl_pipe_attr* ioctl_pipe_attr)
{
    td_s32 ret;

    ioctl_pipe_attr->pipe_id = pipe_id;
    ret = memcpy_s(&ioctl_pipe_attr->pipe_attr, sizeof(ext_viu_pipe_attr),
        viu_pipe_attr, sizeof(ext_viu_pipe_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_pipe_attr_drv_to_user(td_s32 pipe_id,
    const viu_ioctl_pipe_attr* ioctl_pipe_attr, ext_viu_pipe_attr* viu_pipe_attr)
{
    viu_unused(pipe_id);

    td_s32 ret;

    ret = memcpy_s(viu_pipe_attr, sizeof(ext_viu_pipe_attr),
        &ioctl_pipe_attr->pipe_attr, sizeof(ext_viu_pipe_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_snap_attr_user_to_drv(td_s32 pipe_id,
    const ext_snap_attr* viu_snap_attr, viu_ioctl_snap_attr* ioctl_snap_attr)
{
    td_s32 ret;

    ioctl_snap_attr->pipe_id = pipe_id;
    ret = memcpy_s(&ioctl_snap_attr->snap_attr, sizeof(ext_snap_attr),
        viu_snap_attr, sizeof(ext_snap_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_snap_attr_drv_to_user(td_s32 pipe_id,
    const viu_ioctl_snap_attr* ioctl_snap_attr, ext_snap_attr* viu_snap_attr)
{
    viu_unused(pipe_id);

    td_s32 ret;

    ret = memcpy_s(viu_snap_attr, sizeof(ext_snap_attr),
        &ioctl_snap_attr->snap_attr, sizeof(ext_snap_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dump_attr_user_to_drv(td_s32 pipe_id,
    const ext_viu_dump_attr* viu_dump_attr, viu_ioctl_dump_attr* ioctl_dump_attr)
{
    td_s32 ret;

    ioctl_dump_attr->pipe_id = pipe_id;
    ret = memcpy_s(&ioctl_dump_attr->dump_attr, sizeof(ext_viu_dump_attr),
        viu_dump_attr, sizeof(ext_viu_dump_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dump_attr_drv_to_user(td_s32 pipe_id,
    const viu_ioctl_dump_attr* ioctl_dump_attr, ext_viu_dump_attr* viu_dump_attr)
{
    viu_unused(pipe_id);

    td_s32 ret;

    ret = memcpy_s(viu_dump_attr, sizeof(ext_viu_dump_attr),
        &ioctl_dump_attr->dump_attr, sizeof(ext_viu_dump_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_frame_info_user_to_drv(td_s32 pipe_id, td_s32 milli_sec,
    const ext_isp_video_frame_info* viu_frame_info, viu_ioctl_isp_video_frame_info* ioctl_frame_info)
{
    td_s32 ret;

    ioctl_frame_info->pipe_id = pipe_id;
    ioctl_frame_info->milli_sec = milli_sec;
    ret = memcpy_s(&ioctl_frame_info->frame_info, sizeof(ext_isp_video_frame_info),
        viu_frame_info, sizeof(ext_isp_video_frame_info));

    return ret;
}

static inline td_s32 mpi_viu_convert_frame_info_drv_to_user(td_s32 pipe_id, td_s32 milli_sec,
    const viu_ioctl_isp_video_frame_info* ioctl_frame_info, ext_isp_video_frame_info* viu_frame_info)
{
    viu_unused(pipe_id);
    viu_unused(milli_sec);

    td_s32 ret;

    ret = memcpy_s(viu_frame_info, sizeof(ext_isp_video_frame_info),
        &ioctl_frame_info->frame_info, sizeof(ext_isp_video_frame_info));

    return ret;
}

static inline td_s32 mpi_viu_convert_pipe_nrx_param_user_to_drv(td_s32 pipe_id,
    const ext_viu_pipe_nrx_param* viu_pipe_nrx_param, viu_ioctl_pipe_nrx_param* ioctl_pipe_nrx_param)
{
    td_s32 ret;

    ioctl_pipe_nrx_param->pipe_id = pipe_id;
    ret = memcpy_s(&ioctl_pipe_nrx_param->pipe_nrx_param, sizeof(ext_viu_pipe_nrx_param),
        viu_pipe_nrx_param, sizeof(ext_viu_pipe_nrx_param));

    return ret;
}

static inline td_s32 mpi_viu_convert_pipe_nrx_param_drv_to_user(td_s32 pipe_id,
    const viu_ioctl_pipe_nrx_param* ioctl_pipe_nrx_param, ext_viu_pipe_nrx_param* viu_pipe_nrx_param)
{
    viu_unused(pipe_id);

    td_s32 ret;

    ret = memcpy_s(viu_pipe_nrx_param, sizeof(ext_viu_pipe_nrx_param),
        &ioctl_pipe_nrx_param->pipe_nrx_param, sizeof(ext_viu_pipe_nrx_param));

    return ret;
}

static inline td_s32 mpi_viu_convert_chn_attr_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ext_viu_chn_attr* viu_chn_attr, viu_ioctl_chn_attr* ioctl_chn_attr)
{
    td_s32 ret;

    ioctl_chn_attr->pipe_id = pipe_id;
    ioctl_chn_attr->chn_id = chn_id;
    ret = memcpy_s(&ioctl_chn_attr->chn_attr, sizeof(ext_viu_chn_attr),
        viu_chn_attr, sizeof(ext_viu_chn_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_chn_attr_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_chn_attr* ioctl_chn_attr, ext_viu_chn_attr* viu_chn_attr)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_chn_attr, sizeof(ext_viu_chn_attr),
        &ioctl_chn_attr->chn_attr, sizeof(ext_viu_chn_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_gdc_ldc_attr_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const viu_gdc_ldc_attr* viu_gdc_ldc_attr_para, viu_ioctl_gdc_ldc_attr* ioctl_gdc_ldc_attr)
{
    td_s32 ret;

    ioctl_gdc_ldc_attr->pipe_id = pipe_id;
    ioctl_gdc_ldc_attr->chn_id = chn_id;
    ret = memcpy_s(&ioctl_gdc_ldc_attr->gdc_ldc_attr, sizeof(viu_gdc_ldc_attr),
        viu_gdc_ldc_attr_para, sizeof(viu_gdc_ldc_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_viu_ldc_attr_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_gdc_ldc_attr* ioctl_gdc_ldc_attr, ext_viu_ldc_attr* viu_gdc_ldc_attr)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_gdc_ldc_attr, sizeof(ext_viu_ldc_attr),
        &ioctl_gdc_ldc_attr->gdc_ldc_attr.ldc_attr, sizeof(ext_viu_ldc_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_gdc_ldc_attr_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_gdc_ldc_attr* ioctl_gdc_ldc_attr, viu_gdc_ldc_attr* gdc_ldc_attr)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(gdc_ldc_attr, sizeof(viu_gdc_ldc_attr),
        &ioctl_gdc_ldc_attr->gdc_ldc_attr, sizeof(viu_gdc_ldc_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_config_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ext_dis_config* viu_dis_config, viu_ioctl_dis_config* ioctl_dis_config)
{
    td_s32 ret;

    ioctl_dis_config->pipe_id = pipe_id;
    ioctl_dis_config->chn_id = chn_id;
    ret = memcpy_s(&ioctl_dis_config->dis_config, sizeof(ext_dis_config),
        viu_dis_config, sizeof(ext_dis_config));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_config_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_dis_config* ioctl_dis_config, ext_dis_config* viu_dis_config)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_dis_config, sizeof(ext_dis_config),
        &ioctl_dis_config->dis_config, sizeof(ext_dis_config));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_attr_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ext_dis_attr* viu_dis_attr, viu_ioctl_dis_attr* ioctl_dis_attr)
{
    td_s32 ret;

    ioctl_dis_attr->pipe_id = pipe_id;
    ioctl_dis_attr->chn_id = chn_id;
    ret = memcpy_s(&ioctl_dis_attr->dis_attr, sizeof(ext_dis_attr),
        viu_dis_attr, sizeof(ext_dis_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_attr_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_dis_attr* ioctl_dis_attr, ext_dis_attr* viu_dis_attr)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_dis_attr, sizeof(ext_dis_attr),
        &ioctl_dis_attr->dis_attr, sizeof(ext_dis_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_param_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ext_dis_param* viu_dis_param, viu_ioctl_dis_param* ioctl_dis_param)
{
    td_s32 ret;

    ioctl_dis_param->pipe_id = pipe_id;
    ioctl_dis_param->chn_id = chn_id;
    ret = memcpy_s(&ioctl_dis_param->dis_param, sizeof(ext_dis_param),
        viu_dis_param, sizeof(ext_dis_param));

    return ret;
}

static inline td_s32 mpi_viu_convert_dis_param_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_dis_param* ioctl_dis_param, ext_dis_param* viu_dis_param)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_dis_param, sizeof(ext_dis_param),
        &ioctl_dis_param->dis_param, sizeof(ext_dis_param));

    return ret;
}

static inline td_s32 mpi_viu_convert_ext_chn_attr_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ext_viu_ext_chn_attr* viu_ext_chn_attr, viu_ioctl_ext_chn_attr* ioctl_ext_chn_attr)
{
    td_s32 ret;

    ioctl_ext_chn_attr->pipe_id = pipe_id;
    ioctl_ext_chn_attr->chn_id = chn_id;
    ret = memcpy_s(&ioctl_ext_chn_attr->ext_chn_attr, sizeof(ext_viu_ext_chn_attr),
        viu_ext_chn_attr, sizeof(ext_viu_ext_chn_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_ext_chn_attr_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_ext_chn_attr* ioctl_ext_chn_attr, ext_viu_ext_chn_attr* viu_ext_chn_attr)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_ext_chn_attr, sizeof(ext_viu_ext_chn_attr),
        &ioctl_ext_chn_attr->ext_chn_attr, sizeof(ext_viu_ext_chn_attr));

    return ret;
}

static inline td_s32 mpi_viu_convert_get_yuv_timeout_user_to_drv(td_s32 pipe_id, td_s32 chn_id,
    const ioc_viu_usr_get_yuv_timeout* viu_get_yuv_timeout, viu_ioctl_get_yuv_timeout* ioctl_get_yuv_timeout)
{
    td_s32 ret;

    ioctl_get_yuv_timeout->pipe_id = pipe_id;
    ioctl_get_yuv_timeout->chn_id = chn_id;
    ret = memcpy_s(&ioctl_get_yuv_timeout->get_yuv_timeout, sizeof(ioc_viu_usr_get_yuv_timeout),
        viu_get_yuv_timeout, sizeof(ioc_viu_usr_get_yuv_timeout));

    return ret;
}

static inline td_s32 mpi_viu_convert_get_yuv_timeout_drv_to_user(td_s32 pipe_id, td_s32 chn_id,
    const viu_ioctl_get_yuv_timeout* ioctl_get_yuv_timeout, ioc_viu_usr_get_yuv_timeout* viu_get_yuv_timeout)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);

    td_s32 ret;

    ret = memcpy_s(viu_get_yuv_timeout, sizeof(ioc_viu_usr_get_yuv_timeout),
        &ioctl_get_yuv_timeout->get_yuv_timeout, sizeof(ioc_viu_usr_get_yuv_timeout));

    return ret;
}

static inline td_s32 mpi_viu_convert_send_raw_timeout_user_to_drv(td_s32 pipe_id,
    const ioc_viu_usr_send_raw_timeout* viu_send_raw_timeout, viu_ioctl_send_raw_timeout* ioctl_send_raw_timeout)
{
    td_s32 ret;

    ioctl_send_raw_timeout->pipe_id = pipe_id;
    ret = memcpy_s(&ioctl_send_raw_timeout->send_raw_timeout, sizeof(ioc_viu_usr_send_raw_timeout),
        viu_send_raw_timeout, sizeof(ioc_viu_usr_send_raw_timeout));

    return ret;
}

static inline td_s32 mpi_viu_convert_send_raw_timeout_drv_to_user(td_s32 pipe_id,
    const viu_ioctl_send_raw_timeout* ioctl_send_raw_timeout, ioc_viu_usr_send_raw_timeout* viu_send_raw_timeout)
{
    viu_unused(pipe_id);

    td_s32 ret;

    ret = memcpy_s(viu_send_raw_timeout, sizeof(ioc_viu_usr_send_raw_timeout),
        &ioctl_send_raw_timeout->send_raw_timeout, sizeof(ioc_viu_usr_send_raw_timeout));

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __MPI_VIU_CONVERT_H__ */
