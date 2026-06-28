/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau ioctl file
 */

#include "vau_ioctl.h"
#include "drv_vau_struct.h"
#include "vau_job.h"
#include "vau_dfx.h"
#include "vau_hal_apb.h"
#include "rm_math.h"

#ifdef GPU_HDR_SUPPORT
#include "vau_hdr_process.h"
#endif

td_s32 vau_ioctl_create(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle *p_handle = (td_handle *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.create_cnt++;

    if (p_handle == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_create(p_handle);

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.create_cost = (td_float)(time_end - time_start);
    return ret;
}

td_s32 vau_ioctl_destroy(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle handle;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.destroy_cnt++;

    if (arg == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    handle = *(td_handle *)arg;
    ret = drv_vau_destroy(handle);

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.destroy_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_fill(td_u32 cmd, td_void *arg, td_void *file)
{
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    return drv_vau_fill(blit_cmd->hanlde, &blit_cmd->surface_list);
}

td_s32 vau_ioctl_blit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.blit_cnt++;

    if (blit_cmd == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_blit(blit_cmd->hanlde, &blit_cmd->surface_list);

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.blit_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_blit_hdr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret = EXT_FAILURE;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_blit_cmd *hdr_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.blit_cnt++;

    if (hdr_cmd == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

#ifdef GPU_HDR_SUPPORT
    ret = vau_hdr_process(hdr_cmd->hanlde, &hdr_cmd->surface_list);
#else
    gpu_err("unsupport HDR process!");
#endif

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.blit_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_compose(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_blit_cmd *blit_cmd = (drv_vau_blit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.compose_cnt++;

    if (blit_cmd == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_compose(blit_cmd->hanlde, &blit_cmd->surface_list);

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.compose_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_submit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    drv_vau_submit_cmd *submit_cmd = (drv_vau_submit_cmd *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);

    if (submit_cmd == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    if (submit_cmd->is_sync != TD_TRUE) {
        vau_info->func_info.func_cnt.submit_usr_cnt++;
    }

    ret = drv_vau_submit(submit_cmd->handle, submit_cmd->is_sync, submit_cmd->time_out);

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.submit_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_cancel(td_u32 cmd, td_void *arg, td_void *file)
{
    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.cancel_cnt++;
    return vau_ioctl_destroy(cmd, arg, file);
}

td_s32 vau_ioctl_wait_for_done(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;
    td_handle *handle = (td_handle *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.wait_for_done_cnt++;

    if (handle == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    ret = drv_vau_wait_for_done(*handle, 1000); // 1000ms

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.wait_for_done_cost = (td_float)(time_end - time_start);

    return ret;
}

td_s32 vau_ioctl_set_cubic_threshold(td_u32 cmd, td_void *arg, td_void *file)
{
    uapi_unused(cmd);
    uapi_unused(file);

    td_float cubic_threshold = *(td_float *)arg;

    // cubic_threshold Range: 1.0/0.5/0.25/0.125
    if (vau_math_float_is_equal(cubic_threshold, 0.125f) == TD_FALSE &&
        vau_math_float_is_equal(cubic_threshold, 0.25f) == TD_FALSE &&
        vau_math_float_is_equal(cubic_threshold, 0.5f) == TD_FALSE &&
        vau_math_float_is_equal(cubic_threshold, 1.0f) == TD_FALSE) {
        gpu_err("input val [%f] is wrong, need equal 0.125 or 0.25 or 0.5 or 1.0", cubic_threshold);
        return EXT_FAILURE;
    }

    vau_hal_apb_set_cubic_threshold(cubic_threshold);
    return EXT_SUCCESS;
}

td_s32 vau_ioctl_get_draw_rect(td_u32 cmd, td_void *arg, td_void *file)
{
    td_u32 value;
    ext_rect *bbox = (ext_rect *)arg;

    uapi_unused(cmd);
    uapi_unused(file);

    if (bbox == TD_NULL) {
        gpu_err("arg is null!");
        return EXT_FAILURE;
    }

    uapi_reg_read(HAL_VAU_REG_BASEADDR + 0xa80, value);
    bbox->x = (value & 0xFFFF);
    bbox->y = (td_s32)((value & 0xFFFF0000) >> 16); /* 16:index */

    uapi_reg_read(HAL_VAU_REG_BASEADDR + 0xa84, value);
    bbox->width  = (td_u32)((value & 0xFFFF) + 1 - bbox->x);
    bbox->height = (td_u32)(((value & 0xFFFF0000) >> 16) + 1 - bbox->y); /* 16:index */

    return EXT_SUCCESS;
}

td_s32 vau_ioctl_wait_all_done(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_u32 time_start = 0;
    td_u32 time_end = 0;

    uapi_unused(cmd);
    uapi_unused(arg);
    uapi_unused(file);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    gpu_dfx_get_time_us(&time_start);
    vau_info->func_info.func_cnt.wait_all_done_cnt++;

    ret = drv_vau_wait_all_done(1000); // 1000ms

    gpu_dfx_get_time_us(&time_end);
    vau_info->func_info.time_info.wait_all_done_cost = (td_float)(time_end - time_start);

    return ret;
}
