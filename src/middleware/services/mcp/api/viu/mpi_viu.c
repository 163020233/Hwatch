/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mpi_viu_adapt.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "mpi_viu_inner.h"
#include "mpi_viu_convert.h"
#include "mpi_viu_ext.h"
#include "mpi_snap_ext.h"
#include "mpi_mcp_dependency.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>
#include <pthread.h>
#if !defined(CONFIG_CHIP_PLATFORM_WEAR)
#include <sys/ioctl.h>
#endif
#include "securec.h"
#include "drv_isp_video_ext.h"
#if defined(CONFIG_CHIP_3322)
#include "drv_viu_ext.h"
#endif
#include "mpi_vb_ext.h"
#ifdef SOCT_GDC_SUPPORT
#include "mpi_gdc_config_ext.h"
#include "mpi_gdc_pointquery_ext.h"
#endif

static td_s32 g_viu_drv_fd = -1;

static pthread_mutex_t s_viu_mutex = PTHREAD_MUTEX_INITIALIZER;

#define viu_mutex_lock()     (void)pthread_mutex_lock(&s_viu_mutex)
#define viu_mutex_unlock()   (void)pthread_mutex_unlock(&s_viu_mutex)

#ifdef CONFIG_SOCT_SHAOLINSPEAR_SUPPORT
#define VIU_DEV_NAME "/dev/soc_viu"
#else
#define VIU_DEV_NAME "/dev/viu"
#endif

#define viu_check_init_return()               \
    do {                                      \
        if (g_viu_drv_fd < 0) {               \
            viu_err_trace("viu not init.\n"); \
            return SOC_ERR_VIU_SYS_NOTREADY;  \
        }                                     \
    } while (0)

static td_s32 mpi_viu_check_phy_pipe_id(td_s32 pipe_id)
{
    if ((pipe_id >= ISP_MAX_PHY_PIPE_NUM) || (pipe_id < 0)) {
        viu_err_trace("vi vir pipe ID(%d) is invalid\r\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PIPEID;
    }

    return TD_SUCCESS;
}

#if defined(CONFIG_CHIP_3322)
#else
static td_s32 mpi_viu_check_phy_chn_id(td_s32 viu_chn_id)
{
    if ((viu_chn_id >= VIU_MAX_PHY_CHN_NUM) || (viu_chn_id < 0)) {
        viu_err_trace("vi chn ID(%d) is invalid phy-chn, phy-chn's range is [%d, %d].\r\n", viu_chn_id,
            0, VIU_MAX_PHY_CHN_NUM - 1);
        return SOC_ERR_VIU_INVALID_CHNID;
    }

    return TD_SUCCESS;
}

static td_s32 mpi_viu_check_ext_chn_id(td_s32 viu_chn_id)
{
    if ((viu_chn_id >= VIU_MAX_EXT_CHN_NUM) || (viu_chn_id < 0)) {
        viu_err_trace("vi chn ID(%d) is invalid ext-chn, ext-chn's range is [%d, %d].\r\n", viu_chn_id,
            VIU_EXT_CHN_START, (VIU_MAX_EXT_CHN_NUM - 1));
        return SOC_ERR_VIU_INVALID_CHNID;
    }

    return TD_SUCCESS;
}
#endif

#define viu_check_phy_pipe_id_return(pipe_id)                   \
    do {                                                            \
        td_s32 ret_tmp = mpi_viu_check_phy_pipe_id(pipe_id);    \
        if (ret_tmp != TD_SUCCESS) {                                \
            return ret_tmp;                                         \
        }                                                           \
    } while (0)

#define viu_check_phy_chn_id_return(viu_chn_id)                     \
    do {                                                            \
        td_s32 ret_value = mpi_viu_check_phy_chn_id(viu_chn_id);    \
        if (ret_value != TD_SUCCESS) {                              \
            return ret_value;                                       \
        }                                                           \
    } while (0)

#define viu_check_ext_chn_id_return(viu_chn_id)                     \
    do {                                                            \
        td_s32 ret_value = mpi_viu_check_ext_chn_id(viu_chn_id);    \
        if (ret_value != TD_SUCCESS) {                              \
            return ret_value;                                       \
        }                                                           \
    } while (0)

#define is_phy_chn(viu_chn_id)  ((viu_chn_id) < VIU_MAX_PHY_CHN_NUM && (viu_chn_id) >= 0)

#if defined(CONFIG_CHIP_PLATFORM_WEAR)
#if defined(CONFIG_CHIP_3322)
static td_s32 viu_open(void)
{
    if (viu_do_mod_init() != TD_SUCCESS) {
        return -1;
    }
    return 1;
}
static td_s32 viu_close(void)
{
    viu_do_mod_exit();
    return 0;
}
#define open(name, flags) viu_open()
#define close(handle) viu_close()
#define ioctl(idx, cmd, para) viu_ioctl(idx, cmd, para)
td_s32 viu_ioctl(td_s32 module_idx, td_ulong cmd, td_void *para);
#else
td_s32 viu_do_mod_open(td_void *private_data);
td_s32 viu_do_mod_close(td_void *private_data);
td_s32 viu_ioctl_lite(td_s32 handle, td_s32 cmd, td_void *para);
#define open(name, flags) viu_do_mod_open(0)
#define close(handle) viu_do_mod_close(0)
#define ioctl(handle, cmd, para) viu_ioctl_lite(handle, cmd, para)
#endif
#endif

td_s32 ext_mpi_viu_init(td_void)
{
    td_s32 ret;

    viu_mutex_lock();
    if (g_viu_drv_fd > 0) {
        viu_mutex_unlock();
        return TD_SUCCESS;
    }

    g_viu_drv_fd = open(VIU_DEV_NAME, O_RDONLY);
    if (g_viu_drv_fd < 0) {
        mcp_log_err("open viu dev err, ret:%d \n", g_viu_drv_fd);
        viu_mutex_unlock();
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    ret = ioctl(g_viu_drv_fd, VIU_INIT_CTRL, TD_NULL);
    if (ret != TD_SUCCESS) {
        mcp_log_err("call VIU_INIT_CTRL err, ret:%d \n", ret);
    }

    viu_mutex_unlock();
    return ret;
}

td_s32 ext_mpi_viu_deinit(td_void)
{
    td_s32 ret;
    viu_mutex_lock();
    if (g_viu_drv_fd < 0) {
        viu_mutex_unlock();
        return TD_SUCCESS;
    }

    ret = ioctl(g_viu_drv_fd, VIU_DEINIT_CTRL, TD_NULL);
    if (ret != TD_SUCCESS) {
        viu_err_trace("call VIU_DEINIT_CTRL err, ret:%d \n", ret);
    }

    close(g_viu_drv_fd);
    g_viu_drv_fd = -1;
    viu_mutex_unlock();
    return ret;
}

td_s32 ext_mpi_viu_set_work_mode(td_s32 pipe_id, td_bool pipe_online, td_bool chn_online)
{
    viu_ioctl_work_mode_param work_mode_param = {0};

    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    work_mode_param.pipe_id = pipe_id;
    work_mode_param.pipe_online = pipe_online;
    work_mode_param.chn_online = chn_online;

    return ioctl(g_viu_drv_fd, VIU_PIPE_SET_WORK_MODE, &work_mode_param);
}

td_s32 ext_mpi_viu_get_work_mode(td_s32 pipe_id, td_bool *pipe_online, td_bool *chn_online)
{
    viu_ioctl_work_mode_param work_mode_param = {0};
    td_s32 ret;

    viu_check_init_return();
    viu_check_null_ptr_return(pipe_online);
    viu_check_null_ptr_return(chn_online);
    viu_check_pipe_id_return(pipe_id);

    ret = ioctl(g_viu_drv_fd, VIU_PIPE_GET_WORK_MODE, &work_mode_param);
    if (ret != TD_SUCCESS) {
        viu_err_trace("call VIU_PIPE_GET_WORK_MODE err, ret:%d \n", ret);
        return ret;
    }

    *pipe_online = work_mode_param.pipe_online;
    *chn_online = work_mode_param.chn_online;

    return TD_SUCCESS;
}

td_s32 ext_mpi_viu_snap_suspend(td_s32 pipe_id)
{
    td_s32 ret;

    ret = ioctl(g_viu_drv_fd, VIU_PIPE_SNAP_SUSPEND, &pipe_id);
    if (ret != TD_SUCCESS) {
        mcp_log_err("call VIU_PIPE_SNAP_SUSPEND err, ret:%d \n", ret);
    }

    return ret;
}

td_s32 ext_mpi_viu_snap_resume(td_s32 pipe_id)
{
    td_s32 ret;
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_SNAP_RESUME, &pipe_id);
    if (ret != TD_SUCCESS) {
        mcp_log_err("call VIU_PIPE_SNAP_RESUME err, ret:%d \n", ret);
    }
    return ret;
}

static td_s32 mpi_viu_release_frame(const ext_isp_video_frame_info *frame_info)
{
    td_s32 ret;
    viu_ioctl_isp_video_frame_info ioctl_frame_info = {};
    viu_check_init_return();
    viu_check_null_ptr_return(frame_info);

    ret = mpi_viu_convert_frame_info_user_to_drv(0, 0, frame_info, &ioctl_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_MOD_FRAME_RELEASE_CTRL, &ioctl_frame_info);
    return ret;
}

td_s32 ext_mpi_viu_set_dynamic_attr(td_s32 pipe_id, const ext_viu_dynamic_attr *dynamic_attr)
{
    td_s32 ret;
    viu_ioctl_dynamic_attr ioctl_dynamic_attr = {0};

    viu_check_init_return();
    viu_check_null_ptr_return(dynamic_attr);
    viu_check_pipe_id_return(pipe_id);

    ioctl_dynamic_attr.pipe_id = pipe_id;
    ioctl_dynamic_attr.dynamic_attr = *dynamic_attr;
    ret = ioctl(g_viu_drv_fd, VIU_DYNAMIC_ATTR_SET_CTRL, &ioctl_dynamic_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_dynamic_attr(td_s32 pipe_id, ext_viu_dynamic_attr *dynamic_attr)
{
    td_s32 ret;
    viu_ioctl_dynamic_attr ioctl_dynamic_attr = {0};

    viu_check_init_return();
    viu_check_null_ptr_return(dynamic_attr);
    viu_check_pipe_id_return(pipe_id);

    ioctl_dynamic_attr.pipe_id = pipe_id;
    ret = ioctl(g_viu_drv_fd, VIU_DYNAMIC_ATTR_GET_CTRL, &ioctl_dynamic_attr);
    *dynamic_attr = ioctl_dynamic_attr.dynamic_attr;
    return ret;
}

/* 1 for vi device */
td_s32 ext_mpi_viu_set_dev_attr(td_s32 viu_dev_id, const ext_viu_dev_attr *dev_attr)
{
    td_s32 ret;
    viu_ioctl_dev_attr ioctl_dev_attr;

    viu_check_init_return();
    viu_check_dev_id_return(viu_dev_id);
    viu_check_null_ptr_return(dev_attr);

    ret = mpi_viu_convert_dev_attr_user_to_drv(viu_dev_id, dev_attr, &ioctl_dev_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_DEV_ATTR_SET_CTRL, &ioctl_dev_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_dev_attr(td_s32 viu_dev_id, ext_viu_dev_attr *dev_attr)
{
    td_s32 ret;
    viu_ioctl_dev_attr ioctl_dev_attr;

    viu_check_init_return();

    viu_check_dev_id_return(viu_dev_id);

    viu_check_null_ptr_return(dev_attr);

    ret = mpi_viu_convert_dev_attr_user_to_drv(viu_dev_id, dev_attr, &ioctl_dev_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret =  ioctl(g_viu_drv_fd, VIU_DEV_ATTR_GET_CTRL, &ioctl_dev_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = mpi_viu_convert_dev_attr_drv_to_user(viu_dev_id, &ioctl_dev_attr, dev_attr);

    return ret;
}

td_s32 ext_mpi_viu_enable_dev(td_s32 viu_dev_id)
{
    viu_check_init_return();
    viu_check_dev_id_return(viu_dev_id);

    return ioctl(g_viu_drv_fd, VIU_DEV_ENABLE_CTRL, &viu_dev_id);
}

td_s32 ext_mpi_viu_disable_dev(td_s32 viu_dev_id)
{
    viu_check_init_return();
    viu_check_dev_id_return(viu_dev_id);

    return ioctl(g_viu_drv_fd, VIU_DEV_DISABLE_CTRL, &viu_dev_id);
}

td_s32 ext_mpi_viu_dev_attach(td_s32 dev_id, td_s32 pipe_id)
{
    viu_ioctl_dev_attach_param attach_param;

    viu_check_init_return();
    viu_check_dev_id_return(dev_id);
    viu_check_pipe_id_return(pipe_id);

    attach_param.dev_id = dev_id;
    attach_param.pipe_id = pipe_id;
    return ioctl(g_viu_drv_fd, VIU_DEV_ATTACH, &attach_param);
}

td_s32 ext_mpi_viu_dev_detach(td_s32 dev_id, td_s32 pipe_id)
{
    viu_ioctl_dev_attach_param attach_param;

    viu_check_init_return();
    viu_check_dev_id_return(dev_id);
    viu_check_pipe_id_return(pipe_id);

    attach_param.dev_id = dev_id;
    attach_param.pipe_id = pipe_id;
    return ioctl(g_viu_drv_fd, VIU_DEV_DETACH, &attach_param);
}

td_s32 ext_mpi_viu_set_dev_timing_attr(td_s32 viu_dev_id, const ext_viu_dev_timing_attr *dev_timing_attr)
{
    td_s32 ret;
    viu_ioctl_dev_timing_attr ioctl_dev_timing_attr;

    viu_check_init_return();

    viu_check_dev_id_return(viu_dev_id);

    viu_check_null_ptr_return(dev_timing_attr);

    ret = mpi_viu_convert_dev_timing_attr_user_to_drv(viu_dev_id, dev_timing_attr, &ioctl_dev_timing_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_DEV_SET_TIMING_ATTR_CTRL, &ioctl_dev_timing_attr);

    return ret;
}

td_s32 ext_mpi_viu_get_dev_timing_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr)
{
    td_s32 ret;
    viu_ioctl_dev_timing_attr ioctl_dev_timing_attr;

    viu_check_init_return();

    viu_check_dev_id_return(viu_dev_id);

    viu_check_null_ptr_return(dev_timing_attr);

    ret = mpi_viu_convert_dev_timing_attr_user_to_drv(viu_dev_id, dev_timing_attr, &ioctl_dev_timing_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_DEV_GET_TIMING_ATTR_CTRL, &ioctl_dev_timing_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_dev_timing_attr_drv_to_user(viu_dev_id, &ioctl_dev_timing_attr, dev_timing_attr);

    return ret;
}

/* 2 for vi pipe */
td_s32 ext_mpi_viu_create_pipe(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    td_s32 ret;
    viu_ioctl_pipe_attr ioctl_pipe_attr;
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(pipe_attr);

    ret = mpi_viu_convert_pipe_attr_user_to_drv(pipe_id, pipe_attr, &ioctl_pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_CREATE_CTRL, &ioctl_pipe_attr);

    return ret;
}

td_s32 ext_mpi_viu_destroy_pipe(td_s32 pipe_id)
{
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    return ioctl(g_viu_drv_fd, VIU_PIPE_DESTROY_CTRL, &pipe_id);
}

td_s32 ext_mpi_viu_set_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    td_s32 ret;
    viu_ioctl_pipe_attr ioctl_pipe_attr;
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(pipe_attr);

    ret = mpi_viu_convert_pipe_attr_user_to_drv(pipe_id, pipe_attr, &ioctl_pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_ATTR_SET_CTRL, &ioctl_pipe_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr)
{
    td_s32 ret;
    viu_ioctl_pipe_attr ioctl_pipe_attr;
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(pipe_attr);

    ret = mpi_viu_convert_pipe_attr_user_to_drv(pipe_id, pipe_attr, &ioctl_pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_ATTR_GET_CTRL, &ioctl_pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_pipe_attr_drv_to_user(pipe_id, &ioctl_pipe_attr, pipe_attr);
    return ret;
}

td_s32 ext_mpi_viu_start_pipe(td_s32 pipe_id)
{
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    return ioctl(g_viu_drv_fd, VIU_PIPE_START_CTRL, &pipe_id);
}

td_s32 ext_mpi_viu_stop_pipe(td_s32 pipe_id)
{
    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);

    return ioctl(g_viu_drv_fd, VIU_PIPE_STOP_CTRL, &pipe_id);
}

td_s32 ext_mpi_viu_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr)
{
    td_s32 ret;
    viu_ioctl_dump_attr ioctl_dump_attr;
    viu_check_init_return();
    viu_check_phy_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(dump_attr);
    ret = mpi_viu_convert_dump_attr_user_to_drv(pipe_id, dump_attr, &ioctl_dump_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_DUMP_ATTR_SET_CTRL, &ioctl_dump_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr)
{
    td_s32 ret;
    viu_ioctl_dump_attr ioctl_dump_attr;
    viu_check_init_return();
    viu_check_phy_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(dump_attr);

    ret = mpi_viu_convert_dump_attr_user_to_drv(pipe_id, dump_attr, &ioctl_dump_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_DUMP_ATTR_GET_CTRL, &ioctl_dump_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_dump_attr_drv_to_user(pipe_id, &ioctl_dump_attr, dump_attr);
    return ret;
}

td_s32 ext_mpi_viu_acquire_pipe_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 milli_sec)
{
    td_s32 ret;
    viu_ioctl_isp_video_frame_info ioctl_isp_frame_info = {};
    viu_check_init_return();

    viu_check_phy_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(frame_info);

    ret = mpi_viu_convert_frame_info_user_to_drv(pipe_id, milli_sec, frame_info, &ioctl_isp_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_RAW_GET_CTRL, &ioctl_isp_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_frame_info_drv_to_user(pipe_id, milli_sec, &ioctl_isp_frame_info, frame_info);

    return ret;
}

td_s32 ext_mpi_viu_release_pipe_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info)
{
    viu_check_init_return();
    viu_check_null_ptr_return(frame_info);

    viu_unused(pipe_id);
    return mpi_viu_release_frame(frame_info);
}

#if defined(CONFIG_CHIP_3322)
#else
td_s32 ext_mpi_viu_acquire_zsl_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 milli_sec)
{
    td_s32 ret;
    viu_ioctl_isp_video_frame_info ioctl_isp_frame_info = {};
    viu_check_init_return();

    viu_check_phy_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(frame_info);

    ret = mpi_viu_convert_frame_info_user_to_drv(pipe_id, milli_sec, frame_info, &ioctl_isp_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_ZSL_RAW_GET_CTRL, &ioctl_isp_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_frame_info_drv_to_user(pipe_id, milli_sec, &ioctl_isp_frame_info, frame_info);

    return ret;
}

td_s32 ext_mpi_viu_release_zsl_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info)
{
    viu_check_init_return();
    viu_check_null_ptr_return(frame_info);

    viu_unused(pipe_id);
    return mpi_viu_release_frame(frame_info);
}
#endif

td_s32 ext_mpi_viu_queue_pipe_frame(td_s32 pipe_id, const ext_isp_video_frame_info *frame_info)
{
    td_s32 ret;
    viu_ioctl_isp_video_frame_info ioctl_isp_frame_info = {};
    viu_check_init_return();

    viu_check_phy_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(frame_info);

    ret = mpi_viu_convert_frame_info_user_to_drv(pipe_id, 0, frame_info, &ioctl_isp_frame_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return ioctl(g_viu_drv_fd, VIU_PIPE_QUEUE_FRAME, &ioctl_isp_frame_info);
}

#if defined(CONFIG_CHIP_3322)
#else
td_s32 ext_mpi_viu_set_pipe_nrx_param(td_s32 pipe_id, const ext_viu_pipe_nrx_param *nrx_param)
{
    td_s32 ret;
    viu_ioctl_pipe_nrx_param ioctl_nrx_param = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(nrx_param);

    ret = mpi_viu_convert_pipe_nrx_param_user_to_drv(pipe_id, nrx_param, &ioctl_nrx_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_NRX_PARAM_SET_CTRL, &ioctl_nrx_param);
    return ret;
}

td_s32 ext_mpi_viu_get_pipe_nrx_param(td_s32 pipe_id, ext_viu_pipe_nrx_param *nrx_param)
{
    td_s32 ret;
    viu_ioctl_pipe_nrx_param ioctl_nrx_param = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);

    viu_check_null_ptr_return(nrx_param);

    ret = mpi_viu_convert_pipe_nrx_param_user_to_drv(pipe_id, nrx_param, &ioctl_nrx_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_PIPE_NRX_PARAM_GET_CTRL, &ioctl_nrx_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_pipe_nrx_param_drv_to_user(pipe_id, &ioctl_nrx_param, nrx_param);
    return ret;
}

/* 3 for vi chn */
td_s32 ext_mpi_viu_set_chn_attr(td_s32 pipe_id, td_s32 viu_chn_id, const ext_viu_chn_attr *chn_attr)
{
    td_s32 ret;
    viu_ioctl_chn_attr ioctl_chn_attr;

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(chn_attr);

    ret = mpi_viu_convert_chn_attr_user_to_drv(pipe_id, viu_chn_id, chn_attr, &ioctl_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_ATTR_SET_CTRL, &ioctl_chn_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_chn_attr(td_s32 pipe_id, td_s32 viu_chn_id, ext_viu_chn_attr *chn_attr)
{
    td_s32 ret;
    viu_ioctl_chn_attr ioctl_chn_attr;

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(chn_attr);

    ret = mpi_viu_convert_chn_attr_user_to_drv(pipe_id, viu_chn_id, chn_attr, &ioctl_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_ATTR_GET_CTRL, &ioctl_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_chn_attr_drv_to_user(pipe_id, viu_chn_id, &ioctl_chn_attr, chn_attr);
    return ret;
}

td_s32 ext_mpi_viu_enable_chn(td_s32 pipe_id, td_s32 viu_chn_id)
{
    viu_ioctl_pipe_chn_id ioctl_pipe_chn = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(viu_chn_id);

    ioctl_pipe_chn.chn_id = viu_chn_id;
    ioctl_pipe_chn.pipe_id = pipe_id;
    return ioctl(g_viu_drv_fd, VIU_CHN_ENABLE_CTRL, &ioctl_pipe_chn);
}

td_s32 ext_mpi_viu_disable_chn(td_s32 pipe_id, td_s32 viu_chn_id)
{
    viu_ioctl_pipe_chn_id ioctl_pipe_chn = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(viu_chn_id);

    ioctl_pipe_chn.chn_id = viu_chn_id;
    ioctl_pipe_chn.pipe_id = pipe_id;
    return ioctl(g_viu_drv_fd, VIU_CHN_DISABLE_CTRL, &ioctl_pipe_chn);
}

#ifdef SOCT_VIU_GDC_SUPPORT
static td_s32 mpi_viu_check_viu_vpss_mode_not_support_function(td_s32 pipe_id)
{
    td_s32 ret;
    td_bool pipe_online;
    td_bool chn_online;

    ret = ext_mpi_viu_get_work_mode(pipe_id, &pipe_online, &chn_online);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe_id(%d) get vivpss_mode failed!\n", pipe_id);
        return ret;
    }

    if (chn_online == TD_TRUE) {
        viu_err_trace("pipe_id(%d) chn_mode(%d) not support such function!\n", pipe_id, chn_online);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    return ret;
}

static td_s32 mpi_viu_get_chn_out_put_size(td_s32 pipe_id, td_s32 viu_chn_id, ext_isp_size *pic_size)
{
    viu_unused(pipe_id);
    viu_unused(viu_chn_id);
    viu_unused(pic_size);

    return -1;
}

static td_s32 mpi_viu_set_gdc_comm_cfg(td_s32 pipe_id, td_s32 viu_chn_id, ext_isp_size *pic_size,
    ext_gdc_comm_cfg *comm_cfg)
{
    td_s32 ret;
    ext_viu_ext_chn_attr ext_chn_attr;
    ext_viu_chn_attr phy_chn_attr;
    ext_isp_dynamic_range dynamic_range;

    if (is_phy_chn(viu_chn_id)) {
        ret = ext_mpi_viu_get_chn_attr(pipe_id, viu_chn_id, &phy_chn_attr);
        if (ret != TD_SUCCESS) {
            viu_err_trace("pipe %d chn:%d chn attr failed!\n", pipe_id, viu_chn_id);
            return ret;
        }

        dynamic_range = phy_chn_attr.dynamic_range;
    } else {
        ret = ext_mpi_viu_get_ext_chn_attr(pipe_id, viu_chn_id, &ext_chn_attr);
        if (ret != TD_SUCCESS) {
            viu_err_trace("pipe %d chn:%d ext_chn attr failed!\n", pipe_id, viu_chn_id);
            return ret;
        }

        dynamic_range = ext_chn_attr.dynamic_range;
    }

    comm_cfg->src_img_size.width  = pic_size->width;
    comm_cfg->src_img_size.height = pic_size->height;

    if (dynamic_range == EXT_ISP_DYNAMIC_RANGE_SDR8) {
        comm_cfg->bit_depth_in = 8;  /* 8bit */
    } else if ((dynamic_range >= EXT_ISP_DYNAMIC_RANGE_SDR10) &&
        (dynamic_range <= EXT_ISP_DYNAMIC_RANGE_XDR)) {
        comm_cfg->bit_depth_in = 10; /* 10bit */
    } else {
        viu_err_trace("pipe_id %d viu_chn_id %d dynamic_range:%d is err.\n", pipe_id, viu_chn_id,
            dynamic_range);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return ret;
}

td_s32 ext_mpi_viu_set_chn_ldc_attr(td_s32 pipe_id, td_s32 viu_chn_id, const ext_viu_ldc_attr *ldc_attr)
{
    td_s32 ret;
    ext_isp_size pic_size;
    ext_gdc_comm_cfg comm_cfg;
    viu_gdc_ldc_attr gdc_ldc_attr;
    viu_ioctl_gdc_ldc_attr ioctl_gdc_ldc_attr = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(ldc_attr);

    ret = mpi_viu_check_viu_vpss_mode_not_support_function(pipe_id);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe_id(%d) LDC not support!\n", pipe_id);
        return ret;
    }

    (td_void)memset_s(&gdc_ldc_attr, sizeof(viu_gdc_ldc_attr), 0, sizeof(viu_gdc_ldc_attr));

    ret = mpi_viu_get_chn_out_put_size(pipe_id, viu_chn_id, &pic_size);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe_id(%d) viu_chn_id(%d) chn attr configured!\n", pipe_id, viu_chn_id);
        return ret;
    }

    ret = mpi_viu_set_gdc_comm_cfg(pipe_id, viu_chn_id, &pic_size, &comm_cfg);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe_id(%d) viu_chn_id(%d) chn attr configured!\n", pipe_id, viu_chn_id);
        return ret;
    }

    if (ldc_attr->attr.distortion_ratio != 0) {
        ret = ext_mpi_gdc_ldc_cfg(&comm_cfg, &ldc_attr->attr, &gdc_ldc_attr.gdc_firm_attr.fw_comm_param,
            &gdc_ldc_attr.gdc_firm_attr.fw_fisheye_param);
        if (ret != TD_SUCCESS) {
            viu_err_trace("pipe_id(%d) viu_chn_id(%d) gdc ldc configure failed!\n", pipe_id, viu_chn_id);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    (td_void)memcpy_s(&gdc_ldc_attr.ldc_attr, sizeof(gdc_ldc_attr.ldc_attr), ldc_attr, sizeof(ext_viu_ldc_attr));

    ret = mpi_viu_convert_gdc_ldc_attr_user_to_drv(pipe_id, viu_chn_id, &gdc_ldc_attr, &ioctl_gdc_ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_GDC_LDC_SET_CTRL, &ioctl_gdc_ldc_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_chn_ldc_attr(td_s32 pipe_id, td_s32 viu_chn_id, ext_viu_ldc_attr *ldc_attr)
{
    td_s32 ret;
    viu_gdc_ldc_attr gdc_ldc_attr;
    viu_ioctl_gdc_ldc_attr ioctl_gdc_ldc_attr = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(ldc_attr);

    (td_void)memcpy_s(&gdc_ldc_attr.ldc_attr, sizeof(gdc_ldc_attr.ldc_attr), ldc_attr, sizeof(ext_viu_ldc_attr));

    ret = mpi_viu_convert_gdc_ldc_attr_user_to_drv(pipe_id, viu_chn_id, &gdc_ldc_attr, &ioctl_gdc_ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_GDC_LDC_GET_CTRL, &ioctl_gdc_ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_viu_ldc_attr_drv_to_user(pipe_id, viu_chn_id, &ioctl_gdc_ldc_attr, ldc_attr);
    return ret;
}

static td_s32 mpi_viu_ldc_point_query_cfg(td_s32 pipe_id, td_s32 chn_id, ext_gdc_pointquery_cfg *ldc_point_query_cfg,
    ext_viu_ldc_attr *ldc_attr)
{
    td_s32 ret;
    viu_gdc_ldc_attr ldc_drv_attr;
    viu_ioctl_gdc_ldc_attr ioctl_gdc_ldc_attr = {};
    ext_gdc_fw_pmf_param pmf_param = {0};

    viu_check_init_return();

    ret = mpi_viu_convert_gdc_ldc_attr_user_to_drv(pipe_id, chn_id, &ldc_drv_attr, &ioctl_gdc_ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_GDC_LDC_DRV_ATTR_GET_CTRL, &ioctl_gdc_ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_gdc_ldc_attr_drv_to_user(pipe_id, chn_id, &ioctl_gdc_ldc_attr, &ldc_drv_attr);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d chn %d get ldc drv attr fail !\r\n", pipe_id, chn_id);
        return ret;
    }

    if (ldc_drv_attr.ldc_attr.enable == TD_FALSE) {
        viu_err_trace("pipe %d chn %d ldc not enable !\r\n", pipe_id, chn_id);
        return SOC_ERR_VIU_NOT_PERM;
    }

    (td_void)memcpy_s(ldc_attr, sizeof(ext_viu_ldc_attr), &ldc_drv_attr.ldc_attr, sizeof(ldc_drv_attr.ldc_attr));

    ext_mpi_gdc_ldcv1_point_query_cfg(ldc_point_query_cfg, &ldc_drv_attr.gdc_firm_attr.fw_comm_param,
        &pmf_param, &ldc_drv_attr.gdc_firm_attr.fw_fisheye_param);

    return TD_SUCCESS;
}

static td_s32 mpi_viu_ldc_ratio_zero_pos_query(td_s32 pipe_id, td_s32 viu_chn_id, const ext_isp_point *point_in,
    ext_isp_point *point_out)
{
    ext_isp_size in_size = {0};
    td_s32 ret;

    ret = mpi_viu_get_chn_out_put_size(pipe_id, viu_chn_id, &in_size);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe_id(%d) viu_chn_id(%d) chn attr configured!\n", pipe_id, viu_chn_id);
        return ret;
    }

    if (point_in->x < 0 || (td_u32)point_in->x >= in_size.width ||
        point_in->y < 0 || (td_u32)point_in->y >= in_size.height) {
        viu_err_trace("pipe %d chn %d in point(%d, %d) not in image( [0, w%d), [0, h%d) )!\n", pipe_id, viu_chn_id,
            point_in->x, point_in->y, in_size.width, in_size.height);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    *point_out = *point_in;

    return TD_SUCCESS;
}

td_s32 ext_mpi_viu_ldc_pos_query_dst_to_src(td_s32 pipe_id, td_s32 viu_chn_id,
    const ext_isp_point *dst_point_in, ext_isp_point *src_point_out)
{
    td_s32 ret;
    td_bool point_out_of_range = TD_FALSE;
    ext_isp_point point_in, point_out;
    ext_gdc_pointquery_cfg ldc_point_query_cfg = {0};
    ext_viu_ldc_attr ldc_attr;

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dst_point_in);
    viu_check_null_ptr_return(src_point_out);

    ret = mpi_viu_ldc_point_query_cfg(pipe_id, viu_chn_id, &ldc_point_query_cfg, &ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (ldc_attr.attr.distortion_ratio == 0) {
        return mpi_viu_ldc_ratio_zero_pos_query(pipe_id, viu_chn_id, dst_point_in, src_point_out);
    }

    (td_void)memcpy_s(&point_in, sizeof(point_in), dst_point_in, sizeof(ext_isp_point));
    (td_void)memcpy_s(&point_out, sizeof(point_out), src_point_out, sizeof(ext_isp_point));

    ret = ext_mpi_gdc_point_query(&ldc_point_query_cfg, &point_in, &point_out, &point_out_of_range);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d chn %d point query dst to src failed !\r\n", pipe_id, viu_chn_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (point_out_of_range == TD_TRUE) {
        viu_err_trace("pipe %d chn %d point(%d,%d) out of range !\r\n", pipe_id, viu_chn_id, dst_point_in->x,
            dst_point_in->y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    (td_void)memcpy_s(src_point_out, sizeof(ext_isp_point), &point_out, sizeof(point_out));

    return ret;
}

td_s32 ext_mpi_viu_ldc_pos_query_src_to_dst(td_s32 pipe_id, td_s32 viu_chn_id,
    const ext_isp_point *src_point_in, ext_isp_point *dst_point_out)
{
    td_s32 ret;
    td_bool point_out_of_range = TD_FALSE;
    ext_isp_point point_in, point_out;
    ext_gdc_pointquery_cfg ldc_point_query_cfg = {0};
    ext_viu_ldc_attr ldc_attr;

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(src_point_in);
    viu_check_null_ptr_return(dst_point_out);

    ret = mpi_viu_ldc_point_query_cfg(pipe_id, viu_chn_id, &ldc_point_query_cfg, &ldc_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (ldc_attr.attr.distortion_ratio == 0) {
        return mpi_viu_ldc_ratio_zero_pos_query(pipe_id, viu_chn_id, src_point_in, dst_point_out);
    }

    (td_void)memcpy_s(&point_in, sizeof(point_in), src_point_in, sizeof(ext_isp_point));
    (td_void)memcpy_s(&point_out, sizeof(point_out), dst_point_out, sizeof(ext_isp_point));

    ret = ext_mpi_gdc_point_query_backwards(&ldc_point_query_cfg, &point_out, &point_in, &point_out_of_range);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d chn %d point query src to dst failed !\r\n", pipe_id, viu_chn_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (point_out_of_range == TD_TRUE) {
        viu_err_trace("pipe %d chn %d point(%d,%d) out of range !\r\n", pipe_id, viu_chn_id, src_point_in->x,
            src_point_in->y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    (td_void)memcpy_s(dst_point_out, sizeof(ext_isp_point), &point_out, sizeof(point_out));

    return ret;
}
#endif

td_s32 ext_mpi_viu_set_chn_dis_config(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_config *dis_config)
{
    td_s32 ret;
    viu_ioctl_dis_config ioctl_dis_config;

    viu_check_init_return();
    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_config);

    ret = mpi_viu_convert_dis_config_user_to_drv(viu_pipe_id, viu_chn_id, dis_config, &ioctl_dis_config);
    if (ret != TD_SUCCESS) {
        viu_err_trace("dis config memcpy fail.\n");
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_CONFIG_SET_CTRL, &ioctl_dis_config);
    return ret;
}

td_s32 ext_mpi_viu_get_chn_dis_config(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_config *dis_config)
{
    td_s32 ret;
    viu_ioctl_dis_config ioctl_dis_config;

    viu_check_init_return();

    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_config);

    ret = mpi_viu_convert_dis_config_user_to_drv(viu_pipe_id, viu_chn_id, dis_config, &ioctl_dis_config);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_CONFIG_GET_CTRL, &ioctl_dis_config);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_dis_config_drv_to_user(viu_pipe_id, viu_chn_id, &ioctl_dis_config, dis_config);
    return ret;
}

td_s32 ext_mpi_viu_set_chn_dis_attr(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_attr *dis_attr)
{
    td_s32 ret;
    viu_ioctl_dis_attr ioctl_dis_attr = {};

    viu_check_init_return();
    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_attr);

    ret = mpi_viu_convert_dis_attr_user_to_drv(viu_pipe_id, viu_chn_id, dis_attr, &ioctl_dis_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_ATTR_SET_CTRL, &ioctl_dis_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_chn_dis_attr(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_attr *dis_attr)
{
    td_s32 ret;
    viu_ioctl_dis_attr ioctl_dis_attr = {};

    viu_check_init_return();
    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_attr);

    ret = mpi_viu_convert_dis_attr_user_to_drv(viu_pipe_id, viu_chn_id, dis_attr, &ioctl_dis_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_ATTR_GET_CTRL, &ioctl_dis_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_dis_attr_drv_to_user(viu_pipe_id, viu_chn_id, &ioctl_dis_attr, dis_attr);
    return ret;
}

td_s32 ext_mpi_viu_set_chn_dis_param(td_s32 viu_pipe_id, td_s32 viu_chn_id, const ext_dis_param *dis_param)
{
    td_s32 ret;
    viu_ioctl_dis_param ioctl_dis_param = {};

    viu_check_init_return();
    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_param);

    ret = mpi_viu_convert_dis_param_user_to_drv(viu_pipe_id, viu_chn_id, dis_param, &ioctl_dis_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_PARAM_SET_CTRL, &ioctl_dis_param);
    return ret;
}

td_s32 ext_mpi_viu_get_chn_dis_param(td_s32 viu_pipe_id, td_s32 viu_chn_id, ext_dis_param *dis_param)
{
    td_s32 ret;
    viu_ioctl_dis_param ioctl_dis_param = {};

    viu_check_init_return();
    viu_check_pipe_id_return(viu_pipe_id);
    viu_check_phy_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(dis_param);

    ret = mpi_viu_convert_dis_param_user_to_drv(viu_pipe_id, viu_chn_id, dis_param, &ioctl_dis_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_DIS_PARAM_GET_CTRL, &ioctl_dis_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_dis_param_drv_to_user(viu_pipe_id, viu_chn_id, &ioctl_dis_param, dis_param);
    return ret;
}

td_s32 ext_mpi_viu_set_ext_chn_attr(td_s32 pipe_id, td_s32 chn_id, const ext_viu_ext_chn_attr *ext_chn_attr)
{
    td_s32 ret;
    viu_ioctl_ext_chn_attr ioctl_ext_chn_attr;

    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);
    viu_check_ext_chn_id_return(chn_id);

    viu_check_null_ptr_return(ext_chn_attr);

    ret = mpi_viu_convert_ext_chn_attr_user_to_drv(pipe_id, chn_id, ext_chn_attr, &ioctl_ext_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_EXTCHN_ATTR_SET_CTRL, &ioctl_ext_chn_attr);
    return ret;
}

td_s32 ext_mpi_viu_get_ext_chn_attr(td_s32 pipe_id, td_s32 viu_chn_id, ext_viu_ext_chn_attr *ext_chn_attr)
{
    td_s32 ret;
    viu_ioctl_ext_chn_attr ioctl_ext_chn_attr;

    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);
    viu_check_ext_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(ext_chn_attr);

    ret = mpi_viu_convert_ext_chn_attr_user_to_drv(pipe_id, viu_chn_id, ext_chn_attr, &ioctl_ext_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_EXTCHN_ATTR_GET_CTRL, &ioctl_ext_chn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_ext_chn_attr_drv_to_user(pipe_id, viu_chn_id, &ioctl_ext_chn_attr, ext_chn_attr);
    return ret;
}

td_s32 ext_mpi_viu_acquire_chn_frame(td_s32 pipe_id, td_s32 viu_chn_id, ext_isp_video_frame_info *frame_info,
    td_s32 milli_sec)
{
    td_s32 ret;
    ioc_viu_usr_get_yuv_timeout get_yuv_timeout = {};
    viu_ioctl_get_yuv_timeout ioctl_get_yuv_timeout = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(viu_chn_id);

    viu_check_null_ptr_return(frame_info);

    get_yuv_timeout.milli_sec = milli_sec;
    get_yuv_timeout.frame_info.pool_id = frame_info->pool_id;
    ret = mpi_viu_convert_get_yuv_timeout_user_to_drv(pipe_id, viu_chn_id, &get_yuv_timeout, &ioctl_get_yuv_timeout);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_CHN_FRAME_GET_CTRL, &ioctl_get_yuv_timeout);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_get_yuv_timeout_drv_to_user(pipe_id, viu_chn_id, &ioctl_get_yuv_timeout, &get_yuv_timeout);
    if (ret == TD_SUCCESS) {
        (td_void)memcpy_s(frame_info, sizeof(ext_isp_video_frame_info),
            &get_yuv_timeout.frame_info, sizeof(ext_isp_video_frame_info));
    }

    return ret;
}

td_s32 ext_mpi_viu_acquire_dsf_frame(td_s32 pipe_id, td_s32 chn_id, ext_isp_video_frame_info *frame_info, td_s32 ms)
{
    td_s32 ret;
    ioc_viu_usr_get_yuv_timeout get_yuv_timeout = {};
    viu_ioctl_get_yuv_timeout ioctl_get_yuv_timeout = {};

    viu_check_init_return();

    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(chn_id);

    viu_check_null_ptr_return(frame_info);

    get_yuv_timeout.milli_sec = ms;
    get_yuv_timeout.frame_info.pool_id = frame_info->pool_id;
    ret = mpi_viu_convert_get_yuv_timeout_user_to_drv(pipe_id, chn_id, &get_yuv_timeout, &ioctl_get_yuv_timeout);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = ioctl(g_viu_drv_fd, VIU_DSF_FRAME_GET_CTRL, &ioctl_get_yuv_timeout);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = mpi_viu_convert_get_yuv_timeout_drv_to_user(pipe_id, chn_id, &ioctl_get_yuv_timeout, &get_yuv_timeout);
    if (ret == TD_SUCCESS) {
        (td_void)memcpy_s(frame_info, sizeof(ext_isp_video_frame_info),
            &get_yuv_timeout.frame_info, sizeof(ext_isp_video_frame_info));
    }

    return ret;
}

td_s32 ext_mpi_viu_release_chn_frame(td_s32 pipe_id, td_s32 viu_chn_id,
    const ext_isp_video_frame_info *frame_info)
{
    viu_check_init_return();

    viu_check_null_ptr_return(frame_info);

    viu_unused(pipe_id);
    viu_unused(viu_chn_id);
    return mpi_viu_release_frame(frame_info);
}

td_s32 ext_mpi_viu_queue_chn_frame(td_u32 pipe_num, td_s32 pipe_id[], const ext_isp_video_frame_info *frame_info[])
{
    td_u32 i;
    td_s32 ret;
    ioc_viu_usr_send_raw_timeout send_raw_timeout;
    viu_ioctl_send_raw_timeout ioctl_send_raw_timeout;

    viu_check_init_return();
    viu_check_null_ptr_return(pipe_id);
    viu_check_null_ptr_return(frame_info);

    if (pipe_num > ISP_WDR_CHN_MAX) {
        viu_err_trace("pipe_num:%d is invalid.\n", pipe_num);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    for (i = 0; i < pipe_num; i++) {
        viu_check_pipe_id_return(pipe_id[i]);
        viu_check_null_ptr_return(frame_info[i]);

        if (frame_info[i]->v_frame.pixel_format < EXT_ISP_PIXEL_FORMAT_RGB_BAYER_8BPP ||
            frame_info[i]->v_frame.pixel_format > EXT_ISP_PIXEL_FORMAT_RGB_BAYER_16BPP) {
            viu_err_trace("only support raw pixel_format.(%u)\n", frame_info[i]->v_frame.pixel_format);
            return TD_FAILURE;
        }

        send_raw_timeout.raw_info_addr[i] = (td_u64)(td_uintptr_t)frame_info[i];
        send_raw_timeout.pipe_id[i] = pipe_id[i];
    }

    send_raw_timeout.pipe_num = pipe_num;
    send_raw_timeout.milli_sec = 0x32; // 50 ms

    ret = mpi_viu_convert_send_raw_timeout_user_to_drv(pipe_id[0], &send_raw_timeout, &ioctl_send_raw_timeout);
    if (ret != TD_SUCCESS) {
        viu_err_trace("conver send raw fail.\n");
        return ret;
    }

    return ioctl(g_viu_drv_fd, VIU_PIPE_RAW_SEND_CTRL, &ioctl_send_raw_timeout);
}

td_s32 ext_mpi_viu_dequeue_chn_frame(td_s32 pipe_id, ext_isp_video_frame_info *frame_info, td_s32 ms)
{
    return 0;
}

td_s32 ext_mpi_viu_wait_chn_frame_done(
    td_u32 pipe_num, td_s32 pipe_id[], const ext_isp_video_frame_info *frame_info[], td_s32 timeout_ms)
{
    td_u32 i;
    td_s32 ret;
    viu_ioctl_isp_video_frame_info ioctl_isp_frame_info = {};
    viu_check_init_return();
    if (pipe_num > 1) {
        viu_err_trace("viu_wait_chn_frame_done not support pipe num %d\n", pipe_num);
        return TD_FAILURE;
    }

    for (i = 0; i < pipe_num; i++) {
        viu_check_pipe_id_return(pipe_id[i]);
        viu_check_null_ptr_return(frame_info[i]);

        ret = mpi_viu_convert_frame_info_user_to_drv(pipe_id[i], timeout_ms, frame_info[i], &ioctl_isp_frame_info);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ret = ioctl(g_viu_drv_fd, VIU_PIPE_WAIT_FRAME_DONE, &ioctl_isp_frame_info);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }
    return TD_SUCCESS;
}
#endif

td_s32 ext_mpi_viu_pipe_attach(td_s32 pipe_id, td_s32 chn_id, soc_mod_id dst_mod, td_s32 dst_id)
{
    viu_ioctl_pipe_attach_param pipe_attach_param;

    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(chn_id);

    pipe_attach_param.pipe_id = pipe_id;
    pipe_attach_param.chn_id = chn_id;
    pipe_attach_param.dst_mod = dst_mod;
    pipe_attach_param.dst_id = dst_id;
    return ioctl(g_viu_drv_fd, VIU_PIPE_ATTACH, &pipe_attach_param);
}

td_s32 ext_mpi_viu_pipe_detach(td_s32 pipe_id, td_s32 chn_id, soc_mod_id dst_mod, td_s32 dst_id)
{
    viu_ioctl_pipe_attach_param pipe_attach_param;

    viu_check_init_return();
    viu_check_pipe_id_return(pipe_id);
    viu_check_chn_id_return(chn_id);

    pipe_attach_param.pipe_id = pipe_id;
    pipe_attach_param.chn_id = chn_id;
    pipe_attach_param.dst_mod = dst_mod;
    pipe_attach_param.dst_id = dst_id;
    return ioctl(g_viu_drv_fd, VIU_PIPE_DETACH, &pipe_attach_param);
}
