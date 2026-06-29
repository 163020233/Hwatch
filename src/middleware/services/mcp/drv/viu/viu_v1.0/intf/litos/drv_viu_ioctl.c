/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:the header file of ppu intf
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "drv_viu_ioctl.h"
#include "drv_viu_vicap.h"
#include "viu_pipe.h"
#include "viu_dev.h"
#include "viu_vicap.h"
#include "viu_drv.h"
#include "hal_capability_intf.h"
#include "pinctrl.h"

static const pin_t g_vicap_io_pins[] = {
    S_HGPIO10,
    S_HGPIO11,
    S_HGPIO12,
    S_HGPIO13,
    S_HGPIO14,
    S_HGPIO15,

    S_MGPIO12,
    S_MGPIO13,
    S_MGPIO14,
    S_MGPIO15,
    S_MGPIO16
};

static td_u32 g_viu_status = VIU_STATUS_STOPPED;

#define viu_state_check_and_return() \
do { \
    if (g_viu_status != VIU_STATUS_STARTED) { \
        viu_err_trace("viu not init !\n"); \
        return SOC_ERR_VIU_SYS_NOTREADY; \
    } \
} while (0)

td_u32 viu_drv_get_status(td_void)
{
    return g_viu_status;
}

td_s32 viu_ioctl_dev_set_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dev_attr *ioctl_dev_attr = (viu_ioctl_dev_attr *)(arg);
    ext_viu_dev_attr *dev_attr = &ioctl_dev_attr->dev_attr;
    td_s32 dev_id = ioctl_dev_attr->dev_id;

    viu_state_check_and_return();

    return viu_dev_set_attr(dev_id, dev_attr);
}

td_s32 viu_ioctl_dev_get_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);

    viu_ioctl_dev_attr *ioctl_dev_attr = (viu_ioctl_dev_attr *)(arg);
    ext_viu_dev_attr *dev_attr = &ioctl_dev_attr->dev_attr;
    td_s32 dev_id = ioctl_dev_attr->dev_id;

    viu_state_check_and_return();

    return viu_dev_get_attr(dev_id, dev_attr);
}

td_s32 viu_ioctl_dev_enable(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_check_null_ptr_return(arg);
    viu_check_null_ptr_return(priv_data);

    td_s32 dev_id = *(td_s32*)arg;

    viu_state_check_and_return();

    return viu_dev_enable(dev_id, priv_data);
}

td_s32 viu_ioctl_dev_disable(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_check_null_ptr_return(arg);
    viu_check_null_ptr_return(priv_data);

    td_s32 dev_id = *(td_s32*)arg;

    viu_state_check_and_return();

    return viu_dev_disable(dev_id, priv_data);
}

td_s32 viu_ioctl_dev_attach_pipe(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_check_null_ptr_return(arg);
    viu_unused(priv_data);

    viu_ioctl_dev_attach_param *dev_attach_param = (viu_ioctl_dev_attach_param *)arg;

    return viu_dev_attach_pipe(dev_attach_param->dev_id, dev_attach_param->pipe_id);
}

td_s32 viu_ioctl_dev_detach_pipe(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dev_attach_param *dev_attach_param = (viu_ioctl_dev_attach_param *)arg;

    return viu_dev_detach_pipe(dev_attach_param->dev_id, dev_attach_param->pipe_id);
}

td_s32 viu_ioctl_set_dev_timing_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dev_timing_attr *ioctl_dev_timing_attr = (viu_ioctl_dev_timing_attr *)arg;
    ext_viu_dev_timing_attr *dev_timing_attr = &ioctl_dev_timing_attr->dev_timing_attr;
    td_s32 dev_id = ioctl_dev_timing_attr->dev_id;

    viu_state_check_and_return();

    return viu_set_dev_timing_attr(dev_id, dev_timing_attr);
}

td_s32 viu_ioctl_get_dev_timing_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dev_timing_attr *ioctl_dev_timing_attr = (viu_ioctl_dev_timing_attr *)arg;
    ext_viu_dev_timing_attr *dev_timing_attr = &ioctl_dev_timing_attr->dev_timing_attr;
    td_s32 dev_id = ioctl_dev_timing_attr->dev_id;

    viu_state_check_and_return();

    return viu_get_dev_timing_attr(dev_id, dev_timing_attr);
}

td_s32 viu_ioctl_pipe_set_work_mode(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_work_mode_param *param = (viu_ioctl_work_mode_param *)arg;

    return viu_pipe_set_work_mode(param->pipe_id, param->pipe_online, param->chn_online);
}

td_s32 viu_ioctl_pipe_get_work_mode(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_work_mode_param *param = (viu_ioctl_work_mode_param *)arg;

    return viu_pipe_get_work_mode(param->pipe_id, &param->pipe_online, &param->chn_online);
}

td_s32 viu_ioctl_pipe_create(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_check_null_ptr_return(arg);
    viu_check_null_ptr_return(priv_data);

    viu_ioctl_pipe_attr *ioctl_pipe_attr = (viu_ioctl_pipe_attr *)arg;
    ext_viu_pipe_attr *pipe_attr = &ioctl_pipe_attr->pipe_attr;
    td_s32 pipe_id = ioctl_pipe_attr->pipe_id;

    viu_state_check_and_return();

    return viu_pipe_create(pipe_id, pipe_attr, priv_data);
}

td_s32 viu_ioctl_pipe_destroy(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    td_s32 pipe_id = *(td_s32*)arg;

    viu_state_check_and_return();

    return viu_pipe_destroy(pipe_id);
}

td_s32 viu_ioctl_pipe_set_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_pipe_attr *ioctl_pipe_attr = (viu_ioctl_pipe_attr *)arg;
    ext_viu_pipe_attr *pipe_attr = &ioctl_pipe_attr->pipe_attr;
    td_s32 pipe_id = ioctl_pipe_attr->pipe_id;

    viu_state_check_and_return();

    return viu_pipe_set_attr(pipe_id, pipe_attr);
}

td_s32 viu_ioctl_pipe_get_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_pipe_attr *ioctl_pipe_attr = (viu_ioctl_pipe_attr *)(arg);
    ext_viu_pipe_attr *pipe_attr = &ioctl_pipe_attr->pipe_attr;
    td_s32 pipe_id = ioctl_pipe_attr->pipe_id;

    viu_state_check_and_return();

    return viu_pipe_get_attr(pipe_id, pipe_attr);
}

td_s32 viu_ioctl_pipe_start(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    td_s32 pipe_id = *(td_s32*)arg;

    viu_state_check_and_return();

    return viu_pipe_start(pipe_id);
}

td_s32 viu_ioctl_pipe_stop(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    td_s32 pipe_id = *(td_s32*)arg;

    viu_state_check_and_return();

    return viu_pipe_stop(pipe_id);
}

td_s32 viu_ioctl_set_pipe_dump_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dump_attr *ioctl_dump_attr = (viu_ioctl_dump_attr *)arg;
    ext_viu_dump_attr *dump_attr = &ioctl_dump_attr->dump_attr;
    td_s32 pipe_id = ioctl_dump_attr->pipe_id;

    viu_state_check_and_return();

    return viu_set_pipe_dump_attr(pipe_id, dump_attr);
}

td_s32 viu_ioctl_get_pipe_dump_attr(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_dump_attr *ioctl_dump_attr = (viu_ioctl_dump_attr *)arg;
    ext_viu_dump_attr *dump_attr = &ioctl_dump_attr->dump_attr;
    td_s32 pipe_id = ioctl_dump_attr->pipe_id;

    viu_state_check_and_return();

    return viu_get_pipe_dump_attr(pipe_id, dump_attr);
}

td_s32 viu_ioctl_get_pipe_raw(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_isp_video_frame_info *ioctl_isp_frame_info = (viu_ioctl_isp_video_frame_info *)arg;
    td_s32 pipe_id = ioctl_isp_frame_info->pipe_id;

    viu_state_check_and_return();

    return viu_get_pipe_raw(pipe_id, &ioctl_isp_frame_info->frame_info, ioctl_isp_frame_info->milli_sec);
}

td_s32 viu_ioctl_send_pipe_rgb(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_isp_video_frame_info *ioctl_isp_frame_info = (viu_ioctl_isp_video_frame_info *)arg;
    td_s32 pipe_id = ioctl_isp_frame_info->pipe_id;

    viu_state_check_and_return();

    return viu_send_pipe_rgb(pipe_id, &ioctl_isp_frame_info->frame_info, ioctl_isp_frame_info->milli_sec);
}

td_s32 viu_ioctl_send_mult_pipe_raw(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_send_raw_timeout *ioctl_send_raw_timeout = (viu_ioctl_send_raw_timeout *)arg;
    ioc_viu_usr_send_raw_timeout *send_raw_param = &ioctl_send_raw_timeout->send_raw_timeout;
    td_s32 pipe_id = ioctl_send_raw_timeout->pipe_id;

    viu_state_check_and_return();

    return viu_send_mult_pipe_raw(pipe_id, send_raw_param);
}

td_s32 viu_ioctl_chn_attach(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_pipe_attach_param *pipe_attach_param = (viu_ioctl_pipe_attach_param *)arg;

    viu_state_check_and_return();

    return viu_pipe_set_bind(pipe_attach_param->pipe_id, pipe_attach_param->dst_mod, pipe_attach_param->dst_id);
}

td_s32 viu_ioctl_chn_detach(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_pipe_attach_param *pipe_attach_param = (viu_ioctl_pipe_attach_param *)arg;
    viu_state_check_and_return();
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_attach_param->pipe_id);
    pipe_mkp_ctx->dst_mod = -1;
 
    return 0;
}

td_s32 viu_ioctl_mod_release_frame(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(priv_data);
    viu_check_null_ptr_return(arg);

    viu_ioctl_isp_video_frame_info *ioctl_frame_info = (viu_ioctl_isp_video_frame_info *)arg;
    ext_isp_video_frame_info *frame_info = &ioctl_frame_info->frame_info;

    viu_state_check_and_return();

    return viu_mod_release_frame(frame_info);
}

static td_s32 viu_init(td_void*unique)
{
    viu_unused(unique);

    td_s32 ret;

    hal_capability_init();

    ret = vi_mem_init();
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi mem init failed!\n");
        return TD_FAILURE;
    }

    ret = viu_cap_init();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    viu_drv_set_default_work_mode();

    viu_init_time_ref();
    return ret;
}

static td_s32 viu_exit(td_void *unique)
{
    viu_cap_deinit(unique);

    vi_mem_deinit();

    return TD_SUCCESS;
}

static td_void viu_pin_enable(bool enable)
{
    pin_input_enable_t pin_input = enable ? PIN_IE_ENABLE : PIN_IE_DISABLE;
    unsigned int pin_num = sizeof(g_vicap_io_pins) / sizeof(g_vicap_io_pins[0]);
    const pin_t *pins = g_vicap_io_pins;

    for (unsigned int i = 0; i < pin_num; i++) {
        if (uapi_pin_set_ie(pins[i], pin_input) != 0) {
            viu_err_trace("pin %d init fail !\n", pins[i]);
            return;
        }
        if (uapi_pin_set_mode(pins[i], PIN_MODE_3) != 0) {
            viu_err_trace("pinmux %d fail !\n", pins[i]);
            return;
        }
    }
    return;
}

td_s32 viu_ioctl_drv_init(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(arg);
    viu_check_null_ptr_return(priv_data);

    td_s32 ret = TD_SUCCESS;

    if (g_viu_status == VIU_STATUS_STARTED) {
        return TD_SUCCESS;
    }

    ret = viu_init(priv_data);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    g_viu_status = VIU_STATUS_STARTED;

    viu_pin_enable(true);
    return TD_SUCCESS;
}

td_s32 viu_ioctl_drv_deinit(td_u32 cmd, td_void *arg, td_void *priv_data)
{
    viu_unused(cmd);
    viu_unused(arg);
    viu_check_null_ptr_return(priv_data);

    td_s32 ret;

    if (g_viu_status == VIU_STATUS_STOPPED) {
        return TD_SUCCESS;
    }
    viu_pin_enable(false);
    ret = viu_exit(priv_data);
    g_viu_status = VIU_STATUS_STOPPED;

    return ret;
}
