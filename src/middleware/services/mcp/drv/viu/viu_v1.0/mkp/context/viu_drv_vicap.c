/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_drv.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "drv_vicap.h"

#include "viu_drv.h"
#include "viu_public.h"
#include "viu_drv_define.h"
#include "viu_hal.h"

#if defined(CONFIG_CHIP_3322)
#else
#include "viu_hal_3dnr.h"
#endif
#include "hal_viproc_intf.h"
#include "hal_capability_intf.h"
#include "hal_videv_intf.h"
#include "hal_vicap_intf.h"
#include "hal_adapt_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

ext_viu_operating_mode_type g_aen_proc_mode[ISP_VIPROC_IP_NUM] = {0};
ext_viu_operating_mode g_viu_vpss_mode = {0};

viu_dev_drv_ctx g_ast_viu_drv_dev_ctx[VIU_MAX_DEV_NUM];

td_s32 g_a_pipe_bind_dev_id[ISP_MAX_PIPE_NUM] = { [0 ...(ISP_MAX_PIPE_NUM - 1)] = INVALID_DEV };

ext_viu_operating_mode_type viu_drv_proc_get_mode(td_u32 proc_id)
{
    return g_aen_proc_mode[proc_id];
}

td_u32 viu_drv_get_chn_min_width(td_s32 pipe_id, td_s32 chn_id)
{
    viu_unused(chn_id);

    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 min_w = vicap->instance[0].min_w;

    if (pipe_id < MCP_VICAP_INSTANCE_MAX) {
        min_w = vicap->instance[pipe_id].min_w;
    }

    return min_w;
}

td_u32 viu_drv_get_chn_min_height(td_s32 pipe_id, td_s32 chn_id)
{
    viu_unused(chn_id);

    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 min_h = vicap->instance[0].min_h;

    if (pipe_id < MCP_VICAP_INSTANCE_MAX) {
        min_h = vicap->instance[pipe_id].min_h;
    }

    return min_h;
}

td_u32 viu_drv_get_chn_max_width(td_s32 pipe_id, td_s32 chn_id)
{
    viu_unused(chn_id);

    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 max_w = vicap->instance[0].max_w;

    if (pipe_id < MCP_VICAP_INSTANCE_MAX) {
        max_w = vicap->instance[pipe_id].max_w;
    }

    return max_w;
}

td_u32 viu_drv_get_chn_max_height(td_s32 pipe_id, td_s32 chn_id)
{
    viu_unused(chn_id);

    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 max_h = vicap->instance[0].max_h;

    if (pipe_id < MCP_VICAP_INSTANCE_MAX) {
        max_h = vicap->instance[pipe_id].max_h;
    }

    return max_h;
}

td_s32 viu_drv_get_mast_pipe_by_pipe(td_s32 pipe_id)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    return pipe_drv_ctx->master_pipe;
}

td_u32 viu_drv_get_sensor_bit_width(td_s32 viu_dev_id)
{
    return g_ast_viu_drv_dev_ctx[viu_dev_id].sensor_bit_width;
}

ext_viu_operating_mode_type viu_drv_get_pipe_mode(td_s32 pipe_id)
{
    ext_viu_operating_mode_type mode;

    mode = g_viu_vpss_mode.mode[pipe_id];

    return mode;
}

td_s32 viu_drv_check_frame_rate(const ext_isp_frame_rate_ctrl *frame_rate)
{
    if ((frame_rate->src_frame_rate == -1) && (frame_rate->dst_frame_rate == -1)) {
        return TD_SUCCESS;
    }

    if ((frame_rate->src_frame_rate < 1) || (frame_rate->src_frame_rate > VIU_MAX_FPS)) {
        viu_err_trace("src_frame_rate[%d] wrong,should be[1, %d]\n",
            frame_rate->src_frame_rate, VIU_MAX_FPS);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((frame_rate->dst_frame_rate < 0) ||
        (frame_rate->dst_frame_rate > frame_rate->src_frame_rate)) {
        viu_err_trace("dst_frame_rate[%d] is wrong,should be[0, %d]\n",
            frame_rate->dst_frame_rate, frame_rate->src_frame_rate);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_u32 viu_drv_get_pipe_max_size(td_s32 pipe_id)
{
    td_u32 pipe_max_size;
    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 max_w = vicap->instance[0].max_w;
    td_u32 max_h = vicap->instance[0].max_h;

    if (pipe_id < MCP_VICAP_INSTANCE_MAX) {
        max_w = vicap->instance[pipe_id].max_w;
        max_h = vicap->instance[pipe_id].max_h;
    }

    pipe_max_size = max_w * max_h;
    return pipe_max_size;
}

static td_s32 viu_drv_check_pipe_mode(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);

    if ((pipe_attr->pipe_bypass_mode != EXT_VIU_PIPE_BYPASS_NONE) &&
        (pipe_attr->pipe_bypass_mode != EXT_VIU_PIPE_BYPASS_BE)) {
        viu_err_trace("pipe_bypass_mode[%d] should be EXT_VIU_PIPE_BYPASS_NONE or EXT_VIU_PIPE_BYPASS_BE!\n",
            pipe_attr->pipe_bypass_mode);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (pipe_mode == VIU_ONLINE_PPU_OFFLINE || pipe_mode == VIU_ONLINE_PPU_ONLINE) {
        if (pipe_attr->pipe_bypass_mode != EXT_VIU_PIPE_BYPASS_NONE) {
            viu_err_trace("vi pipe_mode[%d] only support EXT_VIU_PIPE_BYPASS_NONE!\n", pipe_mode);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    } else if (pipe_mode == VIU_OFFLINE_PPU_ONLINE) {
        if (pipe_attr->pipe_bypass_mode != EXT_VIU_PIPE_BYPASS_NONE) {
            viu_err_trace("vi pipe_mode[%d] only support EXT_VIU_PIPE_BYPASS_NONE!\n", pipe_mode);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    } else {
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_pipe_wh(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    td_u32 viu_pipe_max_width;
    td_u32 viu_pipe_min_width;
    td_u32 viu_pipe_max_height;
    td_u32 viu_pipe_min_height;
    td_u32 viu_pipe_max_size;

    hal_capability_vicap *cap_vicap = hal_cap_get_vicap();

    viu_pipe_max_width = cap_vicap->instance[pipe_id].max_w;
    viu_pipe_min_width = cap_vicap->instance[pipe_id].min_w;
    viu_pipe_max_height = cap_vicap->instance[pipe_id].max_h;
    viu_pipe_min_height = cap_vicap->instance[pipe_id].min_h;

    if (!VALUE_BETWEEN(pipe_attr->max_w, viu_pipe_min_width, viu_pipe_max_width)) {
        viu_err_trace("pipe_id:%d picture width should between [%d,%d] which is %d!\n",
            pipe_id, viu_pipe_min_width, viu_pipe_max_width, pipe_attr->max_w);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(pipe_attr->max_h, viu_pipe_min_height, viu_pipe_max_height)) {
        viu_err_trace("pipe_id:%d picture height should between [%d,%d] which is %d!\n",
            pipe_id, viu_pipe_min_height, viu_pipe_max_height, pipe_attr->max_h);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    viu_pipe_max_size = viu_drv_get_pipe_max_size(pipe_id);
    if (viu_pipe_max_size < (pipe_attr->max_w * pipe_attr->max_h)) {
        viu_err_trace("vi pipe[%d] ext_isp_size can't be bigger than %d.\n", pipe_id, viu_pipe_max_size);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((pipe_attr->max_w & (VIU_HW_WIDTH_ALIGN - 1)) != 0) {
        viu_err_trace("pipe %d max_w should be %d align which is %d!\n",
            pipe_id, VIU_HW_WIDTH_ALIGN, pipe_attr->max_w);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((pipe_attr->max_h & (VIU_HW_HEIGHT_ALIGN - 1)) != 0) {
        viu_err_trace("pipe %d max_h should be %d align which is %d!\n",
            pipe_id, VIU_HW_HEIGHT_ALIGN, pipe_attr->max_h);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_pipe_pix_fmt_mode(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    ext_isp_wdr_mode wdr_mode)
{
    ext_viu_operating_mode_type pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if (pipe_mode == VIU_ONLINE_PPU_OFFLINE || pipe_mode == VIU_ONLINE_PPU_ONLINE) {
        if (is_format_yuv_support(pipe_attr->pix_fmt) == TD_TRUE) {
            viu_err_trace("vi online pipe_mode do not support SP420/SP422/YUV400!\n");
            return SOC_ERR_VIU_INVALID_PARA;
        }

        if ((wdr_mode == EXT_ISP_WDR_MODE_2TO1_LINE) &&
            (pipe_attr->compress_mode == EXT_COMPRESS_MODE_FRAME)) {
            viu_err_trace("picture compress mode should be EXT_COMPRESS_MODE_NONE "
                "EXT_COMPRESS_MODE_LINE when online_wdr mode!\n");
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

static td_bool check_pix_fmt(td_s32 pipe_id, ext_pixel_format format)
{
    hal_capability_vicap *vicap = hal_cap_get_vicap();
    td_u32 i;

    for (i = 0; i < vicap->instance[pipe_id].support_pix_fmt_num; i++) {
        if (format == vicap->instance[pipe_id].support_pix_fmt[i]) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

static td_s32 viu_drv_check_pipe_pix_fmt(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    ext_isp_wdr_mode wdr_mode)
{
    if ((is_format_yuv_support(pipe_attr->pix_fmt) == TD_FALSE) &&
        (is_format_raw(pipe_attr->pix_fmt) == TD_FALSE) &&
        (is_format_rgb_support(pipe_attr->pix_fmt) == TD_FALSE)) {
        viu_err_trace("picture pix format should be RGB, YUV or BAYER which is %d!\n",
            pipe_attr->pix_fmt);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (is_format_raw(pipe_attr->pix_fmt) == TD_TRUE) {
        if (pipe_attr->isp_bypass != TD_FALSE) {
            viu_err_trace("isp_bypass should be TD_FALSE which is %d when picture pix format is BAYER!\n",
                pipe_attr->isp_bypass);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    if (pipe_attr->pix_fmt == EXT_PIXEL_FORMAT_RGB_BAYER_16BPP ||
        pipe_attr->pix_fmt == EXT_PIXEL_FORMAT_RGB_BAYER_8BPP) {
        if (pipe_attr->compress_mode != EXT_COMPRESS_MODE_NONE) {
            viu_err_trace("pipe %d can not support compressed raw data when picture pix format is BAYER 8BPP 16BPP!\n",
                pipe_id);
            return TD_FAILURE;
        }
    }

    if ((is_format_yuv_support(pipe_attr->pix_fmt) == TD_TRUE) ||
        (is_format_rgb_support(pipe_attr->pix_fmt) == TD_TRUE)) {
        if (pipe_attr->compress_mode != EXT_COMPRESS_MODE_NONE) {
            viu_err_trace("compress_mode should be COMPRESS_MODE_NONE which is %d when picture pix fmt is YUV/RGB!\n",
                pipe_attr->compress_mode);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    if (check_pix_fmt(pipe_id, pipe_attr->pix_fmt) != TD_TRUE) {
        viu_err_trace("pipe_id[%d] do not support(%u) pix_fmt!\n", pipe_id, pipe_attr->pix_fmt);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return viu_drv_check_pipe_pix_fmt_mode(pipe_id, pipe_attr, wdr_mode);
}

static td_s32 viu_drv_check_pipe_compress_mode(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    if (pipe_id == 0 || pipe_id == 1 || is_vir_pipe(pipe_id)) {
        if ((pipe_attr->compress_mode != EXT_COMPRESS_MODE_NONE) &&
            (pipe_attr->compress_mode != EXT_COMPRESS_MODE_LINE) &&
            (pipe_attr->compress_mode != EXT_COMPRESS_MODE_FRAME)) {
            viu_err_trace("picture compress mode should be COMPRESS_NONE/COMPRESS_LINE/COMPRESS_FRAME which is %d!\n",
                pipe_attr->compress_mode);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    } else {
        if (pipe_attr->compress_mode != EXT_COMPRESS_MODE_NONE) {
            viu_err_trace("pipe_id[%d] only support EXT_COMPRESS_MODE_NONE!\n", pipe_id);

            return SOC_ERR_VIU_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

#define VALUE_BETWEEN(x, min, max) (((x) >= (min)) && ((x) <= (max)))

static td_s32 viu_drv_check_pipe_bit_width(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    viu_unused(pipe_id);
    /* 检查像素位宽 */
    if (!VALUE_BETWEEN(pipe_attr->bit_width, EXT_ISP_DATA_BITWIDTH_8, EXT_ISP_DATA_BITWIDTH_16)) {
        viu_err_trace("bit width should between [%d,%d] which is %d!\n",
            EXT_ISP_DATA_BITWIDTH_8, EXT_ISP_DATA_BITWIDTH_16, pipe_attr->bit_width);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (is_format_yuv_support(pipe_attr->pix_fmt) == TD_TRUE) {
        if (pipe_attr->bit_width != EXT_ISP_DATA_BITWIDTH_8) {
            viu_err_trace("bit_width: %d illegal, should be %d when YUV!\n",
                pipe_attr->bit_width, EXT_ISP_DATA_BITWIDTH_8);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

static td_s32 viu_drv_check_pipe_common_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    ext_isp_wdr_mode wdr_mode)
{
    td_s32 ret;
    /* 2 检查YUV skip */
    if ((pipe_attr->yuv_skip != TD_FALSE) && pipe_attr->yuv_skip != TD_TRUE) {
        viu_err_trace("yuv_skip should between [%d,%d] which is %d!\n",
            TD_FALSE, TD_TRUE, pipe_attr->yuv_skip);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    /* 3 检查ISP bypass */
    if ((pipe_attr->isp_bypass != TD_FALSE) && pipe_attr->isp_bypass != TD_TRUE) {
        viu_err_trace("isp_bypass should between [%d,%d] which is %d!\n",
            TD_FALSE, TD_TRUE, pipe_attr->isp_bypass);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    /*  4 检查宽高限制 */
    ret = viu_drv_check_pipe_wh(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* 5 检查像素格式 */
    ret = viu_drv_check_pipe_pix_fmt(pipe_id, pipe_attr, wdr_mode);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* 7 检查3DNR 开关 */
    if ((pipe_attr->nr_en != TD_FALSE) && (pipe_attr->nr_en != TD_TRUE)) {
        viu_err_trace("nr_en should be 0 or 1 which is %d!\n", pipe_attr->nr_en);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    /* 检查压缩，pipe0和pipe1只支持行压缩和帧压缩，其他pipe不支持压缩 */
    ret = viu_drv_check_pipe_compress_mode(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* 检查像素位宽 */
    ret = viu_drv_check_pipe_bit_width(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* 检查帧率控制 */
    ret = viu_drv_check_frame_rate(&pipe_attr->frame_rate);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_phy_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode)
{
    td_s32 ret;

    viu_unused(wdr_mode);
    if ((bind_viu_dev >= VIU_MAX_DEV_NUM) || (bind_viu_dev < 0)) {
        viu_err_trace("pipe %d do not bind valid dev %d!\n", pipe_id, bind_viu_dev);
        return SOC_ERR_VIU_NOT_PERM;
    }

    /* 1 检查PIPE bypass_mode   */
    ret = viu_drv_check_pipe_mode(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_s32 viu_drv_check_vir_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode)
{
    viu_unused(bind_viu_dev);
    viu_unused(wdr_mode);

    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode != VIU_OFFLINE_PPU_OFFLINE) && (pipe_mode != VIU_OFFLINE_PPU_ONLINE)) {
        viu_err_trace(
            "vir_pipe:%d should be in VIU_OFFLINE_PPU_OFFLINE or VIU_OFFLINE_PPU_ONLINE mode which is %d !\n",
            pipe_id, pipe_mode);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    /* 2 检查PIPE bypass_mode   */
    if (pipe_attr->pipe_bypass_mode != EXT_VIU_PIPE_BYPASS_FE) {
        viu_err_trace("pipe_bypass_mode should be %d which is %d!\n",
            EXT_VIU_PIPE_BYPASS_FE, pipe_attr->pipe_bypass_mode);

        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_s32 viu_drv_check_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_s32 bind_viu_dev,
    ext_isp_wdr_mode wdr_mode)
{
    td_s32 ret;

    if ((pipe_id >= ISP_MAX_PIPE_NUM) || (pipe_id < 0)) { /* codeDEX OVERRUN warning. */
        viu_err_trace("vi pipe ID[%d] is invalid\r\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PIPEID;
    }

    if (is_phy_pipe(pipe_id)) {
        ret = viu_drv_check_phy_pipe_attr(pipe_id, pipe_attr, bind_viu_dev, wdr_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    if (is_vir_pipe(pipe_id)) {
        ret = viu_drv_check_vir_pipe_attr(pipe_id, pipe_attr, bind_viu_dev, wdr_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    return viu_drv_check_pipe_common_attr(pipe_id, pipe_attr, wdr_mode);
}

td_s32 viu_drv_check_pipe_static_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    td_s32 ret;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    if (pipe_attr->pipe_bypass_mode != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.pipe_bypass_mode) {
        viu_err_trace("pipe_bypass_mode of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (pipe_attr->yuv_skip != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.yuv_skip) {
        viu_err_trace("yuv_skip of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (pipe_attr->isp_bypass != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.isp_bypass) {
        viu_err_trace("isp_bypass of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (pipe_attr->max_w != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w) {
        viu_err_trace("max_w of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (pipe_attr->max_h != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h) {
        viu_err_trace("max_h of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (pipe_attr->bit_width != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.bit_width) {
        viu_err_trace("bit_width of pipe %d attribute can't be changed!\n ", pipe_id);
        return TD_FAILURE;
    }

    ret = viu_drv_check_pipe_pre_crop_attr(pipe_id, &pipe_attr->crop_info);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_void viu_drv_set_pipe_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    /* 如果属性没有改变，则直接返回 */
    if (mcp_osal_memncmp(&pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr, pipe_attr,
        sizeof(ext_viu_pipe_attr)) == 0) {
        return;
    }

    (td_void)memcpy_s(&pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr, sizeof(ext_viu_pipe_attr),
        pipe_attr, sizeof(*pipe_attr));

    pipe_drv_ctx->pipe_drv_attr_setting.pipe_attr_update = TD_TRUE;

    return;
}

td_void viu_drv_get_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    *pipe_attr = pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr;
}

td_void viu_drv_get_working_pipe_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    *pipe_attr = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr;
}

td_s32 viu_drv_check_pipe_pre_crop_attr(td_s32 pipe_id, const ext_isp_crop_info *crop_info)
{
    td_s32 ret;
    ext_isp_size dev_size;
    ext_viu_pipe_attr pipe_attr;

    if ((crop_info->enable != TD_TRUE) && (crop_info->enable != TD_FALSE)) {
        viu_err_trace("crop_en should be 0 or 1 which is %d!\n", crop_info->enable);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    viu_drv_get_pipe_attr(pipe_id, &pipe_attr);
    viu_drv_get_pipe_bind_dev_output_size(pipe_id, &dev_size);

    if (!VALUE_BETWEEN(crop_info->rect.x, 0, (td_s32)(dev_size.width - 1))) {
        viu_err_trace("pipe_id:%d crop width should between [0,%d) which is %d!\n",
            pipe_id, dev_size.width, crop_info->rect.x);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(crop_info->rect.y, 0, (td_s32)(dev_size.height - 1))) {
        viu_err_trace("pipe_id:%d crop height should between [0,%d) which is %d!\n",
            pipe_id, dev_size.height, crop_info->rect.y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(crop_info->rect.width, pipe_attr.max_w, dev_size.width)) {
        viu_err_trace("pipe_id:%d crop width should between [%d,%d] which is %d!\n",
            pipe_id, pipe_attr.max_w, dev_size.width, crop_info->rect.width);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(crop_info->rect.height, pipe_attr.max_h, dev_size.height)) {
        viu_err_trace("pipe_id:%d crop height should between [%d,%d] which is %d!\n",
            pipe_id, pipe_attr.max_h, dev_size.height, crop_info->rect.height);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((crop_info->rect.x + crop_info->rect.width) > dev_size.width) {
        viu_err_trace("pipe_id:%d crop x+width can not bigger than dev_width!\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((crop_info->rect.y + crop_info->rect.height) > dev_size.height) {
        viu_err_trace("pipe_id:%d crop y+height can not bigger than dev_height!\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    ret = viu_drv_check_crop_align(crop_info->rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_post_crop_align(ext_isp_rect crop_rect)
{
    if (crop_rect.x % 2 != 0) {  /* 起始点2对齐 */
        viu_err_trace("crop X should be 2 align which is %d!\n", crop_rect.x);
        return SOC_ERR_VIU_INVALID_PARA;
    }
    if (crop_rect.y % 2 != 0) {  /* 起始点2对齐 */
        viu_err_trace("crop Y should be 2 align which is %d!\n", crop_rect.y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((crop_rect.width & 0x1U) != 0) { /* 与上0X1,宽高2对齐 */
        viu_err_trace("crop width should be 2 align which is %d!\n", crop_rect.width);
        return SOC_ERR_VIU_INVALID_PARA;
    }
    if ((crop_rect.height & 0x1U) != 0) { /* 与上0X1,宽高2对齐 */
        viu_err_trace("crop height should be 2 align which is %d!\n", crop_rect.height);
        return SOC_ERR_VIU_INVALID_PARA;
    }
    return TD_SUCCESS;
}

static td_s32 viu_drv_check_post_crop_rect(td_s32 pipe_id, const ext_isp_rect *rect)
{
    ext_viu_pipe_attr pipe_attr;

    viu_drv_get_pipe_attr(pipe_id, &pipe_attr);

    if (!VALUE_BETWEEN(rect->x, 0, (td_s32)(pipe_attr.max_w - 1))) {
        viu_err_trace("pipe_id:%d crop.x should between [0,%d) which is %d!\n",
            pipe_id, pipe_attr.max_w, rect->x);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(rect->y, 0, (td_s32)(pipe_attr.max_h - 1))) {
        viu_err_trace("pipe_id:%d crop.y should between [0,%d) which is %d!\n",
            pipe_id, pipe_attr.max_h, rect->y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(rect->width, VIU_PIPE_POST_CROP_MIN_WIDTH, pipe_attr.max_w)) {
        viu_err_trace("pipe_id:%d crop width should between [%d,%d] which is %d!\n",
            pipe_id, VIU_PIPE_POST_CROP_MIN_WIDTH, pipe_attr.max_w, rect->width);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (!VALUE_BETWEEN(rect->height, VIU_PIPE_POST_CROP_MIN_HEIGHT, pipe_attr.max_h)) {
        viu_err_trace("pipe_id:%d crop height should between [%d,%d] which is %d!\n",
            pipe_id, VIU_PIPE_POST_CROP_MIN_HEIGHT, pipe_attr.max_h, rect->height);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((rect->x + rect->width) > pipe_attr.max_w) {
        viu_err_trace("pipe_id:%d crop x+width can not bigger than max_w!\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((rect->y + rect->height) > pipe_attr.max_h) {
        viu_err_trace("pipe_id:%d crop y+height can not bigger than max_h!\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_s32 viu_drv_check_pipe_post_crop_attr(td_s32 pipe_id, const ext_isp_crop_info *crop_info)
{
    td_s32 ret;
    ext_isp_wdr_mode wdr_mode;

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (viu_drv_is_wdr_mode(wdr_mode) == TD_TRUE) {
        viu_err_trace("not support pipe post crop when wdr mode!\n");
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    if ((crop_info->enable != TD_TRUE) && (crop_info->enable != TD_FALSE)) {
        viu_err_trace("crop_en should be 0 or 1 which is %d!\n", crop_info->enable);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    ret = viu_drv_check_post_crop_rect(pipe_id, &crop_info->rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = viu_drv_check_post_crop_align(crop_info->rect);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_void viu_drv_get_pipe_pre_size(td_s32 pipe_id, ext_isp_size *pipe_pre_size)
{
    ext_isp_crop_info *crop_info = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    crop_info = &pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.crop_info;

    if (crop_info->enable == TD_FALSE) {
        ext_isp_size size;

        viu_drv_get_pipe_bind_dev_output_size(pipe_id, &size);

        (td_void)memcpy_s(pipe_pre_size, sizeof(ext_isp_size), &size, sizeof(size));
    } else {
        pipe_pre_size->width = crop_info->rect.width;
        pipe_pre_size->height = crop_info->rect.height;
    }
}

td_void viu_drv_get_pipe_output_size(td_s32 pipe_id, ext_isp_size *pipe_output_size)
{
    ext_isp_crop_info *crop_info = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    crop_info = &pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info;

    if (crop_info->enable == TD_FALSE) {
        pipe_output_size->width = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_w;
        pipe_output_size->height = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_h;
    } else {
        pipe_output_size->width = crop_info->rect.width;
        pipe_output_size->height = crop_info->rect.height;
    }
}

td_s32 viu_drv_pipe_drv_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_s32 bind_viu_dev,
    ext_isp_wdr_mode wdr_mode)
{
    viu_unused(wdr_mode);
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = mcp_osal_vmalloc(sizeof(viu_pipe_drv_ctx));
    if (pipe_drv_ctx == TD_NULL) {
        viu_err_trace("pipe %d malloc DRV context fail!\n", pipe_id);
        return SOC_ERR_VIU_NOMEM;
    }

    (td_void)memset_s(pipe_drv_ctx, sizeof(viu_pipe_drv_ctx), 0, sizeof(viu_pipe_drv_ctx));
    (td_void)memcpy_s(&pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr, sizeof(ext_viu_pipe_attr),
        pipe_attr, sizeof(*pipe_attr));
    (td_void)memcpy_s(&pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr, sizeof(ext_viu_pipe_attr),
        pipe_attr, sizeof(*pipe_attr));
    pipe_drv_ctx->pipe_drv_attr_setting.pipe_attr_update = TD_TRUE;

    pipe_drv_ctx->master_pipe = 0;

    g_past_pipe_drv_ctx[pipe_id] = pipe_drv_ctx;

    pipe_drv_ctx->pipe_id = pipe_id;
    pipe_drv_ctx->bind_viu_dev = bind_viu_dev;

    pipe_drv_ctx->frame_int_attr.int_type = EXT_ISP_FRAME_INTERRUPT_START;
    pipe_drv_ctx->frame_int_attr.early_line = 0;

    return TD_SUCCESS;
}

td_void viu_drv_pipe_drv_ctx_exit(td_s32 pipe_id)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    mcp_osal_vfree(pipe_drv_ctx);
    g_past_pipe_drv_ctx[pipe_id] = TD_NULL;
}

td_s32 viu_drv_check_crop_align(ext_isp_rect crop_rect)
{
    /* 起始点2对齐 */
    if (((td_u32)crop_rect.x & 0x1U) != 0) {
        viu_err_trace("crop X should be 2 align which is %d!\n", crop_rect.x);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (((td_u32)crop_rect.y & 0x1U) != 0) {
        viu_err_trace("crop Y should be 2 align which is %d!\n", crop_rect.y);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    /* 宽高4对齐 */
    if ((crop_rect.width & (VIU_HW_WIDTH_ALIGN - 1)) != 0) {
        viu_err_trace("crop width should be %d align which is %d!\n",
            VIU_HW_WIDTH_ALIGN, crop_rect.width);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((crop_rect.height & (VIU_HW_HEIGHT_ALIGN - 1)) != 0) {
        viu_err_trace("crop height should be %d align which is %d!\n",
            VIU_HW_HEIGHT_ALIGN, crop_rect.height);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

static td_void viu_drv_update_pipe_post_crop_from_user_set(td_s32 pipe_id)
{
    ext_isp_crop_info *viu_pipe_post_crop_set = TD_NULL;
    ext_isp_crop_info *viu_pipe_post_crop_use = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (pipe_drv_ctx->pipe_drv_attr_setting.pipe_post_crop_update == TD_TRUE) {
        viu_pipe_post_crop_set = &pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.crop_info;
        viu_pipe_post_crop_use = &pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info;
        (td_void)memcpy_s(viu_pipe_post_crop_use, sizeof(ext_isp_crop_info),
            viu_pipe_post_crop_set, sizeof(*viu_pipe_post_crop_set));
        pipe_drv_ctx->pipe_drv_attr_setting.pipe_post_crop_update = TD_FALSE;
    }
}

static td_void viu_drv_update_pipe_attr_from_user_set(td_s32 pipe_id)
{
    ext_viu_pipe_attr *viu_pipe_attr_set = TD_NULL;
    ext_viu_pipe_attr *viu_pipe_attr_use = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (pipe_drv_ctx->pipe_drv_attr_setting.pipe_attr_update == TD_TRUE) {
        viu_pipe_attr_set = &pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr;
        viu_pipe_attr_use = &pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr;
        (td_void)memcpy_s(viu_pipe_attr_use, sizeof(ext_viu_pipe_attr), viu_pipe_attr_set,
            sizeof(*viu_pipe_attr_set));
        pipe_drv_ctx->pipe_drv_attr_setting.pipe_attr_update = TD_FALSE;
    }
}

static td_void viu_drv_update_pipe_frame_type_from_user_set(td_s32 pipe_id)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (pipe_drv_ctx->pipe_drv_attr_setting.frame_type_update == TD_TRUE) {
        pipe_drv_ctx->pipe_drv_attr_working.frame_type = pipe_drv_ctx->pipe_drv_attr_setting.frame_type;
        pipe_drv_ctx->pipe_drv_attr_setting.frame_type_update = TD_FALSE;
    }
}

static td_void viu_drv_update_pipe_nr_param_from_user_set(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
#else
    viu_nrx_cfg *nrx_cfg_set = TD_NULL;
    viu_nrx_cfg *nrx_cfg_use = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    nrx_cfg_use = &pipe_drv_ctx->pipe_drv_attr_working.nrx_cfg;

    if (pipe_drv_ctx->pipe_drv_attr_setting.nr_param_update == TD_TRUE) {
        nrx_cfg_set = &pipe_drv_ctx->pipe_drv_attr_setting.nrx_cfg;
        (td_void)memcpy_s(nrx_cfg_use, sizeof(viu_nrx_cfg), nrx_cfg_set, sizeof(*nrx_cfg_set));
        pipe_drv_ctx->pipe_drv_attr_setting.nr_param_update = TD_FALSE;
        pipe_drv_ctx->update_nr = TD_TRUE;
        pipe_drv_ctx->ai_set = TD_FALSE;
    }
#endif
}

static td_void viu_drv_update_pipe_dump_attr_from_user_set(td_s32 pipe_id)
{
    ext_viu_dump_attr *viu_pipe_dump_attr_set = TD_NULL;
    ext_viu_dump_attr *viu_pipe_dump_attr_use = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (pipe_drv_ctx->pipe_drv_attr_setting.dump_attr_update == TD_TRUE) {
        viu_pipe_dump_attr_set = &pipe_drv_ctx->pipe_drv_attr_setting.dump_attr;
        viu_pipe_dump_attr_use = &pipe_drv_ctx->pipe_drv_attr_working.dump_attr;
        (td_void)memcpy_s(viu_pipe_dump_attr_use, sizeof(ext_viu_dump_attr),
            viu_pipe_dump_attr_set, sizeof(*viu_pipe_dump_attr_set));
        pipe_drv_ctx->pipe_drv_attr_setting.dump_attr_update = TD_FALSE;
    }
}

td_void viu_drv_update_working_pipe_drv_attr(td_s32 pipe_id)
{
    viu_drv_update_pipe_post_crop_from_user_set(pipe_id);

    viu_drv_update_pipe_attr_from_user_set(pipe_id);

    viu_drv_update_pipe_frame_type_from_user_set(pipe_id);

    viu_drv_update_pipe_nr_param_from_user_set(pipe_id);

    viu_drv_update_pipe_dump_attr_from_user_set(pipe_id);
}

td_bool viu_drv_chn_frm_rate_ctl(ext_isp_frame_rate_ctrl *frame_rate, td_s32 *lost_frm_key)
{
    td_s32 des_frm_rate;
    td_s32 src_frm_rate;

    src_frm_rate = frame_rate->src_frame_rate;
    des_frm_rate = frame_rate->dst_frame_rate;

    /* 如果是满帧通道则每一帧都捕获 */
    if ((src_frm_rate == VIU_INVALID_FRMRATE) ||
        (des_frm_rate == VIU_INVALID_FRMRATE) ||
        (src_frm_rate == des_frm_rate)) {
        return TD_TRUE;
    }

    /* 丢帧算法 */
    *lost_frm_key += des_frm_rate;

    if (*lost_frm_key >= src_frm_rate) { /* 按照算法，本帧应该捕获 */
        *lost_frm_key -= src_frm_rate;

        return TD_TRUE;
    }

    return TD_FALSE;
}

td_bool viu_drv_pipe_frame_rate_ctl(td_s32 pipe_id)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    return viu_drv_chn_frm_rate_ctl(&pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.frame_rate,
        &pipe_drv_ctx->pipe_lost_frm_key);
}

static td_s32 viu_drv_check_rgb_size_format(td_s32 pipe_id, const ext_isp_video_frame_info *video_frame)
{
    /* 暂时只考虑在双PIPE拍照场景下支撑RGB注入, 复用拍照PIPE通路:
     * 拍照PIPE与视频PIPE绑定同一个PT, 此时拍照PIPE处理的格式为RAW
     * 会导致与用户注入的图像format(RGB)不一致, 这里考虑PIPE可以复用,
     * 不检查在图像format, 最终在drv/hal中根据实际pix_fmt配置逻辑
     */
    viu_pipe_drv_ctx *pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (video_frame->v_frame.width != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w) {
        viu_err_trace("pipe %d send rgb width %d is not same as max_w %d!\n ",
            pipe_id, video_frame->v_frame.width, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w);
        return TD_FAILURE;
    }

    if (video_frame->v_frame.height != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h) {
        viu_err_trace("pipe %d send rgb height %d is not same as max_h %d!\n ",
            pipe_id, video_frame->v_frame.height, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_s32 viu_drv_check_rgb_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame)
{
    td_s32 ret;
    ext_isp_wdr_mode wdr_mode;

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    /* WDR模式不支持送RGB */
    if (viu_drv_is_wdr_mode(wdr_mode) == TD_TRUE) {
        viu_err_trace("pipe %d is WDR mode can not send rgb.\n", pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    /* 必须为真实的VB */
    if (viu_ext_vb_is_blk_valid(video_frame->pool_id, video_frame->v_frame.phy_addr[0]) != TD_TRUE) {
        viu_err_trace("pipe %d invalid VB\n", pipe_id);
        return TD_FAILURE;
    }

    ret = viu_drv_check_rgb_size_format(pipe_id, video_frame);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = viu_comm_check_video_frame_valid(pipe_id, &video_frame->v_frame, TD_FALSE);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* 压缩必须为 非压 */
    if (video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_NONE) {
        viu_err_trace("pipe %d illegal param: compress_mode:%d,should be EXT_COMPRESS_MODE_NONE !\n", pipe_id,
            video_frame->v_frame.compress_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_yuv_size_format(td_s32 pipe_id, const ext_isp_video_frame_info *video_frame)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (video_frame->v_frame.width != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w) {
        viu_err_trace("pipe %d send yuv width %d is not same as max_w %d!\n ",
            pipe_id, video_frame->v_frame.width, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w);
        return TD_FAILURE;
    }

    if (video_frame->v_frame.height != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h) {
        viu_err_trace("pipe %d send yuv height %d is not same as max_h %d!\n ",
            pipe_id, video_frame->v_frame.height, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h);
        return TD_FAILURE;
    }

    if (video_frame->v_frame.pixel_format != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.pix_fmt) {
        viu_err_trace("pipe %d send yuv pixel_format %d is not same as pix_fmt %d!\n ",
            pipe_id, video_frame->v_frame.pixel_format, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.pix_fmt);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 viu_drv_check_yuv_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame)
{
    td_s32 ret;
    ext_isp_wdr_mode wdr_mode;

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    /* WDR模式不支持送YUV */
    if (viu_drv_is_wdr_mode(wdr_mode) == TD_TRUE) {
        viu_err_trace("pipe %d is WDR mode can not send yuv.\n", pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    /* 必须为真实的VB */
    if (viu_ext_vb_is_blk_valid(video_frame->pool_id, video_frame->v_frame.phy_addr[0]) != TD_TRUE) {
        viu_err_trace("pipe %d invalid VB\n", pipe_id);
        return TD_FAILURE;
    }

    ret = viu_drv_check_yuv_size_format(pipe_id, video_frame);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = viu_comm_check_video_frame_valid(pipe_id, &video_frame->v_frame, TD_FALSE);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* 压缩必须为 非压 */
    if (video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_NONE) {
        viu_err_trace("pipe %d illegal param: compress_mode:%d,should be EXT_COMPRESS_MODE_NONE !\n", pipe_id,
            video_frame->v_frame.compress_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 viu_drv_check_line_raw_capacity_for_be(td_s32 pipe_id, ext_isp_video_frame_info *video_frame)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    /* PIPE的像素格式必须为RAW */
    if (is_format_raw(pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.pix_fmt) == TD_FALSE) {
        viu_err_trace("pipe %d can not send raw from BE when pipe pixel format is %d !\n ",
            pipe_id, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.pix_fmt);
        return TD_FAILURE;
    }

    /* BE必须不能bypass */
    if (pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.isp_bypass != TD_FALSE) {
        viu_err_trace("pipe %d can not send raw from BE when pipe isp_bypass is %d !\n ",
            pipe_id, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.isp_bypass);
        return TD_FAILURE;
    }

    /* 必须为真实的VB */
    if (viu_ext_vb_is_blk_valid(video_frame->pool_id, video_frame->v_frame.phy_addr[0]) != TD_TRUE) {
        viu_err_trace("pipe %d invalid VB\n", pipe_id);
        return TD_FAILURE;
    }

    /* 宽高必须与PIPE的宽高一致 */
    if (video_frame->v_frame.width != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w) {
        viu_err_trace("pipe %d send LINE raw width %d is not same as pipe's max_w %d!\n ",
            pipe_id, video_frame->v_frame.width, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_w);
        return TD_FAILURE;
    }

    if (video_frame->v_frame.height != pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h) {
        viu_err_trace("pipe %d send LINE raw height %d is not same as pipe's max_h %d!\n ",
            pipe_id, video_frame->v_frame.height, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.max_h);
        return TD_FAILURE;
    }

    /* BE支持的RAW像素格式 */
    if (is_format_raw(video_frame->v_frame.pixel_format) == TD_FALSE) {
        viu_err_trace("pipe %d wrong LINE raw pixel format %d !\n ", pipe_id, video_frame->v_frame.pixel_format);
        return TD_FAILURE;
    }

    /* 压缩必须为 非压/行压/帧压 */
    if ((video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_NONE) &&
        (video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_LINE) &&
        (video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_FRAME)) {
        viu_err_trace("pipe %d illegal param: compress_mode:%d, "
            "should be EXT_COMPRESS_MODE_NONE EXT_COMPRESS_MODE_LINE EXT_COMPRESS_MODE_FRAME!\n",
            pipe_id, video_frame->v_frame.compress_mode);
        return TD_FAILURE;
    }

    /* 当为16BIT的RAW时，必须为非压 */
    if (video_frame->v_frame.pixel_format == EXT_PIXEL_FORMAT_RGB_BAYER_16BPP ||
        video_frame->v_frame.pixel_format == EXT_PIXEL_FORMAT_RGB_BAYER_8BPP) {
        if (video_frame->v_frame.compress_mode != EXT_COMPRESS_MODE_NONE) {
            viu_err_trace("pipe %d illegal param: compress_mode:%d, "
                "should be EXT_COMPRESS_MODE_NONE when pixel is BAYER 8BPP 16BPP!\n",
                pipe_id, video_frame->v_frame.compress_mode);
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_wdr_for_multi_frame(td_s32 pipe_id, ext_isp_video_frame_info *video_frame_info,
    td_u32 vc_num)
{
    td_s32 i;
    ext_isp_video_frame_info *master_frame = &video_frame_info[0];
    ext_isp_video_frame_info *slave_frame = TD_NULL;

    for (i = 1; i < (td_s32)vc_num; i++) {
        slave_frame = &video_frame_info[i];

        /* 必须为真实的VB */
        if (viu_ext_vb_is_blk_valid(slave_frame->pool_id, slave_frame->v_frame.phy_addr[0]) != TD_TRUE) {
            viu_err_trace("invalid VB\n");
            return TD_FAILURE;
        }

        /* 多帧的宽高必须一致 */
        if (master_frame->v_frame.width != slave_frame->v_frame.width) {
            viu_err_trace("pipe %d send WDR raw (vc_num:%d) width %d is not same as (vc_num:0) width %d!\n ",
                pipe_id, i, slave_frame->v_frame.width, master_frame->v_frame.width);
            return TD_FAILURE;
        }

        if (master_frame->v_frame.height != slave_frame->v_frame.height) {
            viu_err_trace("pipe %d send WDR raw (vc_num:%d) height %d is not same as (vc_num:0) height %d!\n ",
                pipe_id, i, slave_frame->v_frame.height, master_frame->v_frame.height);
            return TD_FAILURE;
        }

        /* 多帧的像素格式必须一致 */
        if (master_frame->v_frame.pixel_format != slave_frame->v_frame.pixel_format) {
            viu_err_trace("pipe%d send WDR raw(vc_num:%d) pixel_format %d is not same as (vc_num:0) pixel_format %d!\n",
                pipe_id, i, slave_frame->v_frame.pixel_format, master_frame->v_frame.pixel_format);
            return TD_FAILURE;
        }

        /* 多帧的video_format必须一致 */
        if (master_frame->v_frame.video_format != slave_frame->v_frame.video_format) {
            viu_err_trace("pipe%d send WDR raw(vc_num:%d) video_format %d is not same as (vc_num:0) video_format %d!\n",
                pipe_id, i, slave_frame->v_frame.video_format, master_frame->v_frame.video_format);
            return TD_FAILURE;
        }

        /* 多帧的compress_mode必须一致 */
        if (master_frame->v_frame.compress_mode != slave_frame->v_frame.compress_mode) {
            viu_err_trace("pipe %d send WDR raw (vc_num:%d) compress_mode %d "
                "is not same as (vc_num:0) compress_mode %d!\n ",
                pipe_id, i, slave_frame->v_frame.compress_mode, master_frame->v_frame.compress_mode);
            return TD_FAILURE;
        }

        /* 多帧的field必须一致 */
        if (master_frame->v_frame.field != slave_frame->v_frame.field) {
            viu_err_trace("pipe %d send WDR raw (vc_num:%d) illegal param: field:%d\n",
                pipe_id, i, slave_frame->v_frame.field);
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_s32 viu_drv_check_wdr_raw_capacity_cmp_mode(td_s32 pipe_id, ext_isp_video_frame_info *video_frame_info)
{
    ext_isp_video_frame_info *video_frame = &video_frame_info[0];

    ext_compress_mode compress_mode = video_frame->v_frame.compress_mode;
    ext_pixel_format pixel_format = video_frame->v_frame.pixel_format;

    /* 压缩必须为 非压/行压/帧压 */
    if ((compress_mode != EXT_COMPRESS_MODE_NONE) && (compress_mode != EXT_COMPRESS_MODE_FRAME) &&
        (compress_mode != EXT_COMPRESS_MODE_LINE)) {
        viu_err_trace("pipe %d illegal param: compress_mode:%d,should be COMPRESS_NONE/COMPRESS_FRAME/COMPRESS_LINE!\n",
            pipe_id, compress_mode);
        return TD_FAILURE;
    }

    /* 当为16BIT的RAW时，必须为非压 */
    if (pixel_format == EXT_PIXEL_FORMAT_RGB_BAYER_16BPP) {
        if (compress_mode != EXT_COMPRESS_MODE_NONE) {
            viu_err_trace("pipe %d illegal param: compress_mode:%d, "
                "should be EXT_COMPRESS_MODE_NONE when pixel is EXT_PIXEL_FORMAT_RGB_BAYER_16BPP!\n",
                pipe_id, video_frame->v_frame.compress_mode);
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

td_s32 viu_drv_check_wdr_raw_capacity(td_s32 pipe_id, ext_isp_video_frame_info *video_frame_info, td_u32 vc_num)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    ext_isp_video_frame_info *video_frame = &video_frame_info[0];

    td_u32 width = video_frame->v_frame.width;
    td_u32 height = video_frame->v_frame.height;
    ext_pixel_format pixel_format = video_frame->v_frame.pixel_format;

    /* PIPE的像素格式必须为RAW */
    if (is_format_raw(pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt) == TD_FALSE) {
        viu_err_trace("pipe %d can not send raw from BE when pipe pixel format is %d !\n ",
            pipe_id, pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt);
        return TD_FAILURE;
    }

    /* BE必须不能bypass */
    if (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.isp_bypass != TD_FALSE) {
        viu_err_trace("pipe %d can not send raw from BE when pipe isp_bypass is %d !\n ",
            pipe_id, pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.isp_bypass);
        return TD_FAILURE;
    }

    /* 必须为真实的VB */
    if (viu_ext_vb_is_blk_valid(video_frame->pool_id, video_frame->v_frame.phy_addr[0]) != TD_TRUE) {
        viu_err_trace("pipe %d invalid VB\n", pipe_id);
        return TD_FAILURE;
    }

    /* 宽高必须与PIPE的宽高一致 */
    if (width != pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_w) {
        viu_err_trace("pipe %d send WDR raw (vc_num:0) width %d is not same as max_w %d!\n ",
            pipe_id, width, pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_w);
        return TD_FAILURE;
    }

    if (height != pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_h) {
        viu_err_trace("pipe %d send WDR raw (vc_num:0) width %d is not same as max_h %d!\n ",
            pipe_id, height, pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_h);
        return TD_FAILURE;
    }

    /* BE支持的RAW像素格式 */
    if (is_format_raw(pixel_format) == TD_FALSE) {
        viu_err_trace("pipe %d wrong LINE raw pixel format %d !\n ", pipe_id, pixel_format);
        return TD_FAILURE;
    }

    if (viu_comm_check_video_frame_valid(pipe_id, &video_frame->v_frame, TD_TRUE) != TD_SUCCESS) {
        viu_err_trace("pipe %d video frame format invalid!\n ", pipe_id);
        return TD_FAILURE;
    }

    if (viu_drv_check_wdr_raw_capacity_cmp_mode(pipe_id, video_frame_info) != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return viu_drv_check_wdr_for_multi_frame(pipe_id, video_frame_info, vc_num);
}

static td_void viu_drv_get_wch_vb_blk_crop(td_s32 pipe_id, viu_get_frame_vb_info *get_frm_vb_info)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info.enable == TD_TRUE) {
        get_frm_vb_info->size.width = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info.rect.width;
        get_frm_vb_info->size.height = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info.rect.height;
    } else {
        if (((pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
            (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
            (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YUV_400)) &&
            (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.isp_bypass == TD_TRUE)) {
                get_frm_vb_info->size.width = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_w;
                get_frm_vb_info->size.height =  pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_h;
        } else {
            get_frm_vb_info->size.width = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_w;
            get_frm_vb_info->size.height = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.max_h;
        }
    }

    get_frm_vb_info->pixel_format = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt;
    get_frm_vb_info->compress_mode = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.compress_mode;
}

/* 获取FE写出raw数据的buffer */
td_s32 viu_drv_get_wch_vb_blk(td_s32 pipe_id, viu_vb_source vb_source, vb_pool vb_pool_id,
    ext_isp_video_frame_info *video_frame)
{
    viu_get_frame_vb_info get_frm_vb_info;
    viu_pipe_drv_ctx *pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    hal_capability_vicap *cap_vicap = hal_cap_get_vicap();

    /* 分配内存 */
    get_frm_vb_info.viu_pipe_id = pipe_id;
    get_frm_vb_info.viu_chn_id = 0;
    get_frm_vb_info.lowdelay = TD_FALSE;
    get_frm_vb_info.align = cap_vicap->instance[pipe_id].stride_align; // align 16
    get_frm_vb_info.vb_source = vb_source;
    get_frm_vb_info.vb_pool_id = vb_pool_id;

    /* 如果crop 打开，分配内存，需按照crop后的宽高 */
    viu_drv_get_wch_vb_blk_crop(pipe_id, &get_frm_vb_info);

    if ((pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YUV_400) ||
        (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.pix_fmt == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_444)) {
        if (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.bit_width == EXT_ISP_DATA_BITWIDTH_8) {
            get_frm_vb_info.dynamic_range = EXT_ISP_DYNAMIC_RANGE_SDR8;
        } else if (pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.bit_width == EXT_ISP_DATA_BITWIDTH_10) {
            get_frm_vb_info.dynamic_range = EXT_ISP_DYNAMIC_RANGE_SDR10;
        } else {
            get_frm_vb_info.dynamic_range = EXT_ISP_DYNAMIC_RANGE_SDR8;
        }
    } else {
        get_frm_vb_info.dynamic_range = EXT_ISP_DYNAMIC_RANGE_SDR8;
    }

    get_frm_vb_info.video_format = EXT_ISP_VIDEO_FORMAT_LINEAR;

    video_frame->v_frame.color_space = EXT_CS_BT709;

    return viu_drv_get_frm_vb_blk(&get_frm_vb_info, video_frame);
}

td_u32 viu_drv_get_yuv_stride_by_bit_width(td_u32 width, td_u32 bit_depth, ext_compress_mode compress_mode)
{
    td_u32 width_tmp;
    td_u32 y_stride;
    td_u32 header_size;

    /* 计算stride */
    if (compress_mode == EXT_COMPRESS_MODE_LINE) {
        /* 0 RAW的行压缩每行有一个2_byte的头，保存着该行的数据大小信息(n),
           该行压缩后的数据总大小为16 * n byte,(包括2_byte的头) */
        header_size = 2; /* 2 byte */

        /* RAW的行压缩每16个像素最多膨胀4bit,也就是32个像素最多膨胀8bit，即1_byte */
        /* 1 宽度32对齐 */
        width_tmp = viu_align_up(width, 32);

        /* 2 计算STRIDE,数据本身的大小 */
        y_stride = (width_tmp * bit_depth + 7) >> 3; /* (+7) >> 3 is 8 align */
        y_stride = viu_align_up(y_stride, 16); /* 16 align */

        /* 3 计算STRIDE,加上膨胀部分数据的大小 */
        y_stride = y_stride + width_tmp / 32; /* width_tmp / 32 -- expand value */
        y_stride = viu_align_up(y_stride, 16);    /* 16 align */

        /* 4 STRIDE需要加每行的头大小2_byte */
        y_stride = y_stride + header_size;

        /* 5 STRIDE对齐 */
        y_stride = viu_align_up(y_stride, 16); /* 16 align */
    } else {
        y_stride = (width * bit_depth + 7) >> 3; /* (+7) >> 3 is 8 align */

        y_stride = viu_align_up(y_stride, 16); /* 16 align */
    }

    return y_stride;
}

static td_void viu_drv_prepare_frm_info(vb_blk_handle h_blk_hdl, const ext_vb_base_info *vb_info,
    ext_isp_vb_cal_config vb_cal_config, ext_isp_video_frame_info *video_frame)
{
    td_s32 i;
    ext_video_supplement *supplement = TD_NULL;
    ext_isp_video_frame *p_video_frame = TD_NULL;

    video_frame->pool_id = viu_ext_vb_handle_to_pool_id(h_blk_hdl);
    video_frame->mod_id = MCP_MOD_ID_VIU;
    p_video_frame = &video_frame->v_frame;

    p_video_frame->width  = vb_info->width;
    p_video_frame->height = vb_info->height;
    p_video_frame->field  = EXT_ISP_VIDEO_FIELD_FRAME;
    p_video_frame->dynamic_range = vb_info->dynamic_range;
    p_video_frame->compress_mode = vb_info->compress_mode;
    p_video_frame->video_format  = vb_info->video_format;
    p_video_frame->pixel_format  = vb_info->pixel_format;

    supplement = viu_ext_vb_handle_to_supplement(h_blk_hdl);
    if (supplement != TD_NULL) {
        (td_void)memcpy_s(&p_video_frame->supplement, sizeof(ext_video_supplement),
            supplement, sizeof(*supplement));
    }

    p_video_frame->header_phy_addr[0] = viu_ext_vb_handle_to_phys(h_blk_hdl);
    p_video_frame->header_phy_addr[1] = p_video_frame->header_phy_addr[0] + vb_cal_config.head_y_size;
    p_video_frame->header_phy_addr[2] = p_video_frame->header_phy_addr[1];  /* [2]--YUV的V */

    p_video_frame->header_vir_addr[0] = viu_ext_vb_handle_to_kern(h_blk_hdl);
    p_video_frame->header_vir_addr[1] = p_video_frame->header_vir_addr[0] + vb_cal_config.head_y_size;
    p_video_frame->header_vir_addr[2] = p_video_frame->header_vir_addr[1];  /* [2]--YUV的V */

    p_video_frame->phy_addr[0] = p_video_frame->header_phy_addr[0] + vb_cal_config.head_size;
    p_video_frame->phy_addr[1] = p_video_frame->phy_addr[0] + vb_cal_config.main_y_size;
    p_video_frame->phy_addr[2] = p_video_frame->phy_addr[1];                /* [2]--YUV的V */

    p_video_frame->vir_addr[0] = p_video_frame->header_vir_addr[0] + vb_cal_config.head_size;
    p_video_frame->vir_addr[1] = p_video_frame->vir_addr[0] + vb_cal_config.main_y_size;
    p_video_frame->vir_addr[2] = p_video_frame->vir_addr[1];                /* [2]--YUV的V */

    for (i = 0; i < 3; i++) {   /* 3: YUV 3个分量 */
        p_video_frame->header_stride[i] = vb_cal_config.head_stride;
        p_video_frame->stride[i] = vb_cal_config.main_stride;
    }
}

static td_void viu_drv_get_frm_vb_base_info(viu_get_frame_vb_info *info, ext_vb_base_info *vb_info)
{
    vb_info->compress_mode = info->compress_mode;
    vb_info->dynamic_range = info->dynamic_range;
    vb_info->pixel_format = info->pixel_format;
    vb_info->video_format = info->video_format;
    vb_info->width = info->size.width;
    vb_info->height = info->size.height;
    vb_info->is_3dnr_buffer = TD_FALSE;
    vb_info->align = info->align;
    vb_info->buf_line = 0;
}

td_s32 viu_drv_get_frm_vb_blk(viu_get_frame_vb_info *info, ext_isp_video_frame_info *video_frame)
{
    td_s32 pipe_id;
    td_s32 chn_id;
    vb_blk_handle h_blk_hdl;
    ext_vb_base_info vb_info;
    ext_isp_vb_cal_config vb_cal_config;

    viu_drv_get_frm_vb_base_info(info, &vb_info);
    viu_ext_vb_get_vb_cfg(&vb_info, &vb_cal_config);

    /* 分配内存 */
    pipe_id = info->viu_pipe_id;
    chn_id = info->viu_chn_id;

    if (info->vb_source == VIU_VB_SOURCE_COMMON) {
        h_blk_hdl = viu_ext_vb_get_blk_by_size(vb_cal_config.vb_size, VB_UID_VIU);
    } else if (info->vb_source == VIU_VB_SOURCE_USER) {
        h_blk_hdl = viu_ext_vb_get_blk_by_size_and_pool_id(info->vb_pool_id, vb_cal_config.vb_size, VB_UID_VIU);
    } else if (info->vb_source == VIU_VB_SOURCE_PRIVATE) {
        h_blk_hdl = viu_ext_vb_get_blk_by_pool_id(info->vb_pool_id, VB_UID_VIU);
    } else {
        viu_err_trace("pipe %d chn %d unsupported vb source %d !\n", pipe_id, chn_id, info->vb_source);
        return TD_FAILURE;
    }

    if (h_blk_hdl == VB_INVALID_HANDLE) {
        viu_err_trace("pipe %d chn %d get buffer fail, size %d !\n", pipe_id, chn_id, vb_cal_config.vb_size);
        return TD_FAILURE;
    }

    viu_drv_prepare_frm_info(h_blk_hdl, &vb_info, vb_cal_config, video_frame);

    return TD_SUCCESS;
}

td_u32 viu_drv_trans_pixel(ext_pixel_format pix_fmt)
{
    td_u32 pixel = 0;

    switch (pix_fmt) {
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_8BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_10BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_12BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_14BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_16BPP:
            pixel = 2; /* 2 -- to set cur_format, 0_SP420, 1_SP422, 2_RAW, 5_Planar444 */
            break;

        case EXT_ISP_PIXEL_FORMAT_YUV_400:
        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
            pixel = 0;
            break;

        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            pixel = 1;
            break;

        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_444:
            pixel = 5; /* 2 -- to set cur_format, 0_SP420, 1_SP422, 2_RAW, 5_Planar444 */
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_888:
        case EXT_ISP_PIXEL_FORMAT_RGB_10BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_12BPP:
            pixel = 0x6; /* pkg rgb */
            break;
        default:
            viu_debug_trace("drv viu, not support this pixel format :%d!\n", pix_fmt);
            break;
    }

    return pixel;
}

/* as same ae logic define */
enum{
    VIPROC_BIT_WIDTH_8 = 0,
    VIPROC_BIT_WIDTH_10 = 1,
    VIPROC_BIT_WIDTH_12 = 2,
    VIPROC_BIT_WIDTH_14 = 3,
    VIPROC_BIT_WIDTH_16 = 4,
    VIPROC_BIT_WIDTH_32 = 5,
};

td_u32 viu_drv_trans_bit_width(ext_pixel_format pix_fmt, ext_dynamic_range dynamic_range)
{
    td_u32 bit_width = 0;

    switch (pix_fmt) {
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_8BPP:
            bit_width = VIPROC_BIT_WIDTH_8;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_10BPP:
            bit_width = VIPROC_BIT_WIDTH_10;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_12BPP:
            bit_width = VIPROC_BIT_WIDTH_12;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_14BPP:
            bit_width = VIPROC_BIT_WIDTH_14;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_16BPP:
            bit_width = VIPROC_BIT_WIDTH_16;
            break;

        case EXT_ISP_PIXEL_FORMAT_YUV_400:
        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            bit_width = (dynamic_range == EXT_ISP_DYNAMIC_RANGE_SDR8) ?
                VIPROC_BIT_WIDTH_8 : VIPROC_BIT_WIDTH_10;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_888:
            bit_width = VIPROC_BIT_WIDTH_8; /* 8 bit */
            break;
        case EXT_ISP_PIXEL_FORMAT_RGB_10BPP:
            bit_width = VIPROC_BIT_WIDTH_10;  /* 10 bit */
            break;
        case EXT_ISP_PIXEL_FORMAT_RGB_12BPP:
            bit_width = VIPROC_BIT_WIDTH_12; /* 12 bit */
            break;

        default:
            viu_info_trace("drv viu bit, not support this pixel format !\n");
            break;
    }

    return bit_width;
}

ext_isp_data_bitwidth viu_drv_trans_dynamic_range2_bit_width_en(ext_pixel_format pix_fmt,
    ext_dynamic_range dynamic_range)
{
    ext_isp_data_bitwidth bit_width = 0;

    switch (pix_fmt) {
        case EXT_ISP_PIXEL_FORMAT_RGB_888:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_8BPP:
            bit_width = EXT_ISP_DATA_BITWIDTH_8;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_10BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_10BPP:
            bit_width = EXT_ISP_DATA_BITWIDTH_10;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_12BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_12BPP:
            bit_width = EXT_ISP_DATA_BITWIDTH_12;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_14BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_14BPP:
            bit_width = EXT_ISP_DATA_BITWIDTH_14;
            break;

        case EXT_ISP_PIXEL_FORMAT_RGB_16BPP:
        case EXT_ISP_PIXEL_FORMAT_RGB_BAYER_16BPP:
            bit_width = EXT_ISP_DATA_BITWIDTH_16;
            break;

        case EXT_ISP_PIXEL_FORMAT_YUV_400:
        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            bit_width = (EXT_ISP_DYNAMIC_RANGE_SDR8 == dynamic_range) ?
                EXT_ISP_DATA_BITWIDTH_8 : EXT_ISP_DATA_BITWIDTH_10;
            break;

        default:
            viu_info_trace("drv viu dyn, not support this pixel format %d!\n", pix_fmt);
            break;
    }

    return bit_width;
}

td_s32 viu_drv_check_pipe_frame_type(td_s32 pipe_id, const ext_viu_pipe_frame_type type)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    ext_viu_pipe_frame_source source = EXT_VIU_PIPE_FRAME_SOURCE_DEV;
    ext_viu_operating_mode_type pipe_mode;

    switch (type) {
        case EXT_VIU_PIPE_FRAME_TYPE_RAW:
        case EXT_VIU_PIPE_FRAME_TYPE_YUV:
        case EXT_VIU_PIPE_FRAME_TYPE_RGB:
            break;
        default:
            viu_err_trace("frame_type %d is invalid param!\n", type);
            return SOC_ERR_VIU_INVALID_PARA;
    }

    if (pipe_drv_ctx) {
        source = pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.frame_source;
    }

    if (source != EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
        viu_err_trace("cur source is %d, only supported 'EXT_VIU_PIPE_FRAME_SOURCE_USER_BE'.\n", source);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode == VIU_ONLINE_PPU_OFFLINE) || (pipe_mode == VIU_ONLINE_PPU_ONLINE)) {
        viu_err_trace("pipe %d VIU_ONLINE mode don't support set pipe frame type.\n", pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    if (is_vir_pipe(pipe_id)) {
        if (source != EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
            viu_err_trace("virtual pipe :%d unsupported source mode %d, should be %d.\n", pipe_id, source,
                EXT_VIU_PIPE_FRAME_SOURCE_USER_BE);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }
    return TD_SUCCESS;
}

td_s32 viu_drv_get_pipe_bind_dev_id_before_init(td_s32 pipe_id)
{
    return g_a_pipe_bind_dev_id[pipe_id];
}

td_s32 viu_drv_get_pipe_bind_dev_id(td_s32 pipe_id)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    return pipe_drv_ctx->bind_viu_dev;
}

td_void viu_drv_set_pipe_bind_dev_id(td_s32 pipe_id, td_s32 viu_dev_id)
{
    g_a_pipe_bind_dev_id[pipe_id] = viu_dev_id;
}

ext_isp_wdr_mode viu_drv_get_wdr_mode_by_pipe(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
    return 0;
#else
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    return pipe_drv_ctx->wdr_ctx.wdr_mode;
#endif
}

td_void viu_drv_set_dev_attach_pipe(td_u8 dev_id, td_u8 pipe_id)
{
    td_u32 i;
    viu_dev_drv_ctx *viu_dev_ctx = viu_drv_get_dev_ctx(dev_id);
    td_bool isp_bypass = TD_FALSE;
    td_bool isp_in_yuv = TD_FALSE;
    td_u32 pipe_num = viu_dev_ctx->dev_bind_pipe.num;

    for (i = 0; i < pipe_num; i++) {
        if (viu_dev_ctx->dev_bind_pipe.pipe_id[i] == pipe_id) {
            return;
        }
    }

    viu_dev_ctx->dev_bind_pipe.pipe_id[pipe_num] = pipe_id;
    viu_dev_ctx->dev_bind_pipe.num += 1;

    viu_drv_set_pipe_bind_dev_id(pipe_id, dev_id);

    if (viu_dev_ctx->viu_dev_attr.input_data_type == EXT_VIU_DATA_TYPE_YUV) {
        isp_in_yuv = TD_TRUE;
        isp_bypass = TD_TRUE;
    } else {
        isp_in_yuv = TD_FALSE;
        isp_bypass = TD_FALSE;
    }

    hal_vicap_set_isp_mode(pipe_id, isp_bypass, isp_in_yuv);
}

td_void viu_drv_set_dev_detach_pipe(td_u8 dev_id, td_u8 pipe_id)
{
    viu_dev_drv_ctx *viu_dev_ctx = viu_drv_get_dev_ctx(dev_id);
    viu_dev_ctx->dev_bind_pipe.num--;
    viu_drv_set_pipe_bind_dev_id(pipe_id, INVALID_DEV);
    return;
}

td_void viu_drv_get_dev_bind_pipe(td_s32 viu_dev_id, ext_viu_dev_bind_pipe *dev_bind_pipe)
{
    viu_dev_drv_ctx *viu_dev_ctx = TD_NULL;
    viu_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
    (td_void)memcpy_s(dev_bind_pipe, sizeof(*dev_bind_pipe), &(viu_dev_ctx->dev_bind_pipe),
        sizeof(ext_viu_dev_bind_pipe));
}

td_void viu_drv_set_timg_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr)
{
    viu_dev_drv_ctx *viu_dev_ctx = TD_NULL;
    ext_isp_size size;

    viu_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
    (td_void)memcpy_s(&viu_dev_ctx->viu_timing_attr, sizeof(ext_viu_dev_timing_attr),
        dev_timing_attr, sizeof(*dev_timing_attr));

    size.width = viu_dev_ctx->viu_dev_attr.size.width;
    size.height = viu_dev_ctx->viu_dev_attr.size.height;
    if (viu_dev_ctx->viu_timing_attr.enable == TD_TRUE) {
        if (viu_dev_ctx->viu_dev_attr.data_rate == EXT_ISP_DATA_RATE_X2) {
            size.width = viu_dev_ctx->viu_dev_attr.size.width / 2; /* X2 need divide 2 */
        }
    }

    hal_videv_set_timing(viu_dev_id, dev_timing_attr, &size);
}

td_void viu_drv_get_timg_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr)
{
    viu_dev_drv_ctx *viu_dev_ctx = TD_NULL;

    viu_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
    (td_void)memcpy_s(dev_timing_attr, sizeof(*dev_timing_attr),
        &viu_dev_ctx->viu_timing_attr, sizeof(ext_viu_dev_timing_attr));
}

td_s32 viu_drv_check_chip_id(td_s32 viu_dev_id)
{
    viu_unused(viu_dev_id);

    return TD_SUCCESS;
}

td_void viu_drv_get_dev_attr(td_s32 viu_dev_id, ext_viu_dev_attr *viu_dev_attr)
{
    viu_dev_drv_ctx *viu_dev_ctx = TD_NULL;
    viu_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
    (td_void)memcpy_s(viu_dev_attr, sizeof(*viu_dev_attr), &viu_dev_ctx->viu_dev_attr, sizeof(ext_viu_dev_attr));
}

td_void viu_drv_get_pipe_bind_dev_output_size(td_s32 pipe_id, ext_isp_size *dev_size)
{
    td_s32 viu_dev_id;
    ext_viu_dev_attr viu_dev_attr;

    viu_dev_id = viu_drv_get_pipe_bind_dev_id(pipe_id);
    viu_drv_get_dev_attr(viu_dev_id, &viu_dev_attr);

    dev_size->width = viu_dev_attr.size.width;
    dev_size->height = viu_dev_attr.size.height;
}

td_void viu_drv_get_pipe_nrx_status(td_s32 pipe_id, td_bool *nrx_en)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    *nrx_en = pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.nr_en;
}

td_void viu_drv_set_work_mode(td_u8 pipe_id, td_bool pipe_online, td_bool chn_online)
{
    if (pipe_id == 0) {
        if (pipe_online == TD_TRUE && chn_online == TD_TRUE) {
            g_viu_vpss_mode.mode[0] = VIU_ONLINE_PPU_ONLINE;
        } else if (pipe_online == TD_TRUE && chn_online == TD_FALSE) {
            g_viu_vpss_mode.mode[0] = VIU_ONLINE_PPU_OFFLINE;
        } else if (pipe_online == TD_FALSE && chn_online == TD_TRUE) {
            g_viu_vpss_mode.mode[0] = VIU_OFFLINE_PPU_ONLINE;
        } else {
            g_viu_vpss_mode.mode[0] = VIU_OFFLINE_PPU_OFFLINE;
        }
    } else {
        if (g_viu_vpss_mode.mode[0] == VIU_OFFLINE_PPU_ONLINE) {
            g_viu_vpss_mode.mode[pipe_id] = VIU_OFFLINE_PPU_ONLINE;
            chn_online = TD_TRUE;
        } else {
            g_viu_vpss_mode.mode[pipe_id] = VIU_OFFLINE_PPU_OFFLINE;
            pipe_online = TD_FALSE;
            chn_online = TD_FALSE;
        }
    }

    g_aen_proc_mode[0] = g_viu_vpss_mode.mode[0];

    /* vicap */
    hal_vicap_work_mode(0, pipe_online);

    /* viproc */
    hal_viproc_work_mode(0, chn_online);
}

td_void viu_drv_get_work_mode(td_u8 pipe_id, td_bool *pipe_online, td_bool *chn_online)
{
    if (g_viu_vpss_mode.mode[pipe_id] == VIU_ONLINE_PPU_ONLINE) {
        *pipe_online = TD_TRUE;
        *chn_online = TD_TRUE;
    } else if (g_viu_vpss_mode.mode[pipe_id] == VIU_ONLINE_PPU_OFFLINE) {
        *pipe_online = TD_TRUE;
        *chn_online = TD_FALSE;
    } else if (g_viu_vpss_mode.mode[pipe_id] == VIU_OFFLINE_PPU_ONLINE) {
        *pipe_online = TD_FALSE;
        *chn_online = TD_TRUE;
    } else {
        *pipe_online = TD_FALSE;
        *chn_online = TD_FALSE;
    }
}

td_void viu_drv_set_default_work_mode(td_void)
{
    td_u32 viu_proc_id;
    td_bool cap_proc_online_en = TD_FALSE;
    td_bool proc_vpss_online_en = TD_FALSE;

    /* 根据pipemode 设置 procmode */
    g_viu_vpss_mode.mode[0] = 0;
    g_aen_proc_mode[0] = 0;

    /* 注意:这里需要配置寄存器，但是时钟是否打开呢? */
    for (viu_proc_id = 0; viu_proc_id < ISP_VIPROC_IP_NUM; viu_proc_id++) {
        if ((g_aen_proc_mode[viu_proc_id] == VIU_ONLINE_PPU_OFFLINE) ||
            (g_aen_proc_mode[viu_proc_id] == VIU_ONLINE_PPU_ONLINE)) {
            cap_proc_online_en = TD_TRUE;
        } else {
            cap_proc_online_en = TD_FALSE;
        }

        hal_vicap_work_mode(viu_proc_id, cap_proc_online_en);
    }

    for (viu_proc_id = 0; viu_proc_id < ISP_VIPROC_IP_NUM; viu_proc_id++) {
        if ((g_aen_proc_mode[viu_proc_id] == VIU_OFFLINE_PPU_ONLINE) ||
            (g_aen_proc_mode[viu_proc_id] == VIU_ONLINE_PPU_ONLINE)) {
            proc_vpss_online_en = TD_TRUE;
        } else {
            proc_vpss_online_en = TD_FALSE;
        }

        hal_viproc_work_mode(viu_proc_id, proc_vpss_online_en);
    }
}

td_s32 viu_drv_check_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr)
{
    viu_unused(pipe_id);
    if (dump_attr->enable != TD_TRUE && dump_attr->enable != TD_FALSE) {
        viu_err_trace("enable should be TD_TRUE or TD_FALSE which is %d\n", dump_attr->enable);

        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (dump_attr->depth > VIU_PIPE_RAW_QUEUE_MAX_DEPTH) {
        viu_err_trace("depth should be 0 to %d which is %d\n",
            VIU_PIPE_RAW_QUEUE_MAX_DEPTH, dump_attr->depth);

        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_void viu_drv_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    if (mcp_osal_memncmp(&pipe_drv_ctx->pipe_drv_attr_setting.dump_attr,
        dump_attr, sizeof(ext_viu_dump_attr)) == 0) {
        return;
    }

    (td_void)memcpy_s(&pipe_drv_ctx->pipe_drv_attr_setting.dump_attr, sizeof(ext_viu_dump_attr),
        dump_attr, sizeof(*dump_attr));

    pipe_drv_ctx->pipe_drv_attr_setting.dump_attr_update = TD_TRUE;

    return;
}

td_void viu_drv_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    (td_void)memcpy_s(dump_attr, sizeof(*dump_attr),
        &pipe_drv_ctx->pipe_drv_attr_setting.dump_attr, sizeof(ext_viu_dump_attr));
}

td_void viu_drv_get_working_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    (td_void)memcpy_s(dump_attr, sizeof(*dump_attr),
        &pipe_drv_ctx->pipe_drv_attr_working.dump_attr, sizeof(ext_viu_dump_attr));
}

td_void viu_drv_cap_dev_ctx_init(td_void)
{
    (td_void)memset_s(&g_ast_viu_drv_dev_ctx, VIU_MAX_DEV_NUM * sizeof(viu_dev_drv_ctx),
        0, VIU_MAX_DEV_NUM * sizeof(viu_dev_drv_ctx));
}

td_void viu_drv_update_wch_cmp_bayer_format(td_s32 pipe_id, ext_isp_bayer_format bayer_format)
{
    td_u32 bayer_mode;

    bayer_mode = (td_u32)bayer_format;

    viu_hal_set_wch_cmp_bayer_mode(pipe_id, bayer_mode);
    hal_vicap_regnew(pipe_id);
}

td_bool viu_drv_is_cap_valid_int(td_u32 cap_int_status)
{
    if (is_cap_report_irq(cap_int_status) != 0) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_s32 viu_drv_check_send_raw_in_wdr_mode(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout)
{
    td_u32 i;
    td_s32 viu_dev_id;
    ext_viu_pipe_frame_source source;
    ext_viu_dev_bind_pipe dev_bind_pipe;

    viu_dev_id = viu_drv_get_pipe_bind_dev_id(pipe_id);
    viu_drv_get_dev_bind_pipe(viu_dev_id, &dev_bind_pipe);

    if (dev_bind_pipe.num != send_raw_timeout->pipe_num) {
        viu_err_trace("pipe %d pipe_num should be %d which is %d in wdr mode!\n", pipe_id, dev_bind_pipe.num,
            send_raw_timeout->pipe_num);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    for (i = 0; i < dev_bind_pipe.num; i++) {
        if (dev_bind_pipe.pipe_id[i] != send_raw_timeout->pipe_id[i]) {
            viu_err_trace("pipe %d pipe_id[%d] should be %d which is %d in wdr mode!\n", pipe_id, i,
                dev_bind_pipe.pipe_id[i], send_raw_timeout->pipe_id[i]);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    drv_pipe_get_raw_source(pipe_id, &source);

    if (source != EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
        viu_err_trace("pipe %d send raw should from BE which is %d in wdr mode!\n", pipe_id, source);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_void viu_drv_get_pipe_frame_interrupt_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];

    (td_void)memcpy_s(frame_int_attr, sizeof(ext_isp_frame_interrupt_attr),
        &pipe_drv_ctx->frame_int_attr, sizeof(pipe_drv_ctx->frame_int_attr));
}

td_void viu_drv_get_isp_detection_size(td_s32 pipe_id, ext_isp_size *valid_size, ext_isp_size *total_size)
{
    viu_unused(pipe_id);
    viu_unused(valid_size);
    viu_unused(total_size);

    return;
}

static td_u64 g_last_err_time = 0;
static td_s32 g_wrong_int_cnt_sec = 0;
td_bool viu_drv_cap_is_wrong_int(td_s32 pipe_id, ext_isp_frame_interrupt_type int_type, td_u32 int_status)
{
    td_bool err_int = TD_FALSE;
    td_u64 cur_time;

    cur_time = osal_div_u64(osal_sched_clock(), 1000U);

    switch (int_type) {
        case EXT_ISP_FRAME_INTERRUPT_START:
            if ((int_status & VIU_CAP_ERR_INT) != 0) {
                err_int = TD_TRUE;
            }

            if ((int_status & WCH_CC_INT) == 0) {
                err_int = TD_TRUE;
            }
            break;
        default:
            viu_err_trace("unsupported int_type %d \n", int_type);
            break;
    }

    if (err_int == TD_TRUE) {
        g_wrong_int_cnt_sec++;
        if (g_last_err_time == 0 || (cur_time - g_last_err_time) > 1000000) {   /* 1000000: 1s */
            viu_err_trace("pipe:%d err intterupt status:0x%x !! EOF:%d BUFOVF:%d DATATHROW:%d, last sec err cnt %d\n",
                pipe_id, int_status, int_status & WCH_CC_INT,
                int_status & WCH_BUFOVF_INT, int_status & WCH_DATATHROW_INT, g_wrong_int_cnt_sec);
            g_last_err_time = cur_time;
            g_wrong_int_cnt_sec = 0;
        }
    }

    return err_int;
}

td_s32 viu_drv_check_wdr_capacity(td_s32 pipe_id)
{
    viu_unused(pipe_id);
    return TD_SUCCESS;
}

td_s32 viu_drv_check_pipe_support(td_s32 pipe_id)
{
    td_bool support = TD_TRUE;
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;

    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (viu_drv_is_wdr_mode(wdr_mode) == TD_FALSE) {
        if (pipe_id == 3) { /* pipe3 */
            support = TD_FALSE;
        }

        if ((support == TD_FALSE) &&
            (pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.frame_source != EXT_VIU_PIPE_FRAME_SOURCE_USER_BE)) {
                viu_err_trace("pipe_id %d raw_source %d illegal,"
                    "only support send raw from BE when linear mode!\n",
                    pipe_id, pipe_drv_ctx->pipe_drv_attr_setting.viu_pipe_attr.frame_source);
                return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    return TD_SUCCESS;
}

td_s32 viu_drv_set_auto_pattern_info(td_u32 timing_en, td_u32 data_en, td_u32 frame_rate)
{
    viu_unused(data_en);

    viu_dev_drv_ctx *viu_dev_ctx = viu_drv_get_dev_ctx(0);

    viu_dev_ctx->viu_timing_attr.enable = timing_en;
    viu_dev_ctx->viu_timing_attr.frm_rate = frame_rate;

    viu_drv_set_timg_attr(0, &viu_dev_ctx->viu_timing_attr);
    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif
