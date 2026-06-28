/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_userframe.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_vicap.h"
#include "viu_pipe.h"
#include "viu_pic_queue.h"
#include "viu_dev.h"
#include "drv_vicap.h"

#if defined(CONFIG_CHIP_3322)
#include "drv_viu_vicap.h"
#else
#include "drv_viu.h"
#include "viu_chn.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_u32 viu_get_chn_depth(td_s32 pipe_id, td_s32 chn_id)
{
    if (is_phy_chn(chn_id)) {
        return viu_drv_get_phy_chn_depth(pipe_id, chn_id);
    } else if (is_ext_chn(chn_id)) {
        return viu_drv_get_ext_chn_depth(pipe_id, chn_id);
    } else {
        viu_err_trace("Invalid chn_id, %d.\r\n", chn_id);
        return 0;
    }
}

td_s32 viu_send_pipe_rgb(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec)
{
    td_s32 ret;
    ext_viu_operating_mode_type pipe_mode;
    ext_viu_pipe_frame_source source;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(video_frame);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    drv_pipe_get_raw_source(pipe_id, &source);

    if (source != EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d can not send rgb while pipe frame source is not EXT_VIU_PIPE_FRAME_SOURCE_USER_BE!\n",
            pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    if (pipe_mkp_ctx->pipe_bypass_mode == EXT_VIU_PIPE_BYPASS_BE) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d can not send rgb while pipe pipe_bypass_mode is EXT_VIU_PIPE_BYPASS_BE!\n", pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    ret = viu_drv_check_rgb_capacity(pipe_id, video_frame);
    viu_pipe_up_sem(pipe_id);
    if (ret != TD_SUCCESS) {
        return SOC_ERR_VIU_INVALID_PARA;
    }

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    ret = viu_line_mode_send_pipe_frame(pipe_id, video_frame, milli_sec, pipe_mode);

    return ret;
}

static td_s32 viu_line_mode_send_pipe_raw_from_be(td_s32 pipe_id,
    ioc_viu_usr_send_raw_timeout *send_raw_timeout, ext_viu_operating_mode_type pipe_mode)
{
    td_s32 ret;
    ext_isp_video_frame_info video_frame_info;
    vb_blk_handle h_blk_hdl;
    ext_video_supplement *supplement = TD_NULL;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    if ((pipe_mode != VIU_OFFLINE_PPU_OFFLINE) && (pipe_mode != VIU_OFFLINE_PPU_ONLINE)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d can't send raw from be while work mode is not offline-offline nor offline-online!\n",
            pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }

    ret = mcp_osal_copy_from_user(&video_frame_info, (td_void *)(td_uintptr_t)send_raw_timeout->raw_info_addr[0],
        sizeof(ext_isp_video_frame_info));
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("raw_data_info copy_from_user failed! ret:0x%x src:0x%llx  dst:0x%lx.\n", ret,
            (td_u64)send_raw_timeout->raw_info_addr[0], (td_ulong)(td_uintptr_t)&video_frame_info);
        return SOC_ERR_VIU_NOT_PERM;
    }

    ret = viu_check_mod_id(video_frame_info.mod_id);
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        return ret;
    }
    ret = viu_drv_check_line_raw_capacity_for_be(pipe_id, &video_frame_info);
    viu_pipe_up_sem(pipe_id);

    if (ret != TD_SUCCESS) {
        return SOC_ERR_VIU_INVALID_PARA;
    } else {
        h_blk_hdl = viu_ext_vb_phy_to_handle(video_frame_info.v_frame.phy_addr[0]);
        supplement = viu_ext_vb_handle_to_supplement(h_blk_hdl);
        if (supplement != NULL) {
            (td_void)memcpy_s(&video_frame_info.v_frame.supplement, sizeof(ext_video_supplement),
                supplement, sizeof(*supplement));
        }
        return viu_line_mode_send_pipe_frame(pipe_id, &video_frame_info, send_raw_timeout->milli_sec, pipe_mode);
    }
}

static td_s32 viu_line_mode_send_pipe_raw(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout,
    ext_viu_pipe_frame_source source, ext_viu_operating_mode_type pipe_mode)
{
    /* 不支持SRC，不会走下面分支 */
    if (source == EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
        return viu_line_mode_send_pipe_raw_from_be(pipe_id, send_raw_timeout, pipe_mode);
    } else {
        viu_err_trace("pipe %d can not support send frame when source is EXT_VIU_PIPE_FRAME_SOURCE_DEV!\n", pipe_id);
        return SOC_ERR_VIU_NOT_SUPPORT;
    }
}

td_s32 viu_send_mult_pipe_raw(td_s32 pipe_id, ioc_viu_usr_send_raw_timeout *send_raw_timeout)
{
    ext_isp_wdr_mode wdr_mode;
    ext_viu_pipe_frame_source source;
    ext_viu_operating_mode_type pipe_mode;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(send_raw_timeout);
    viu_check_null_ptr_return(send_raw_timeout->raw_info_addr);
    viu_check_null_ptr_return(send_raw_timeout->pipe_id);

    if (!VALUE_BETWEEN(send_raw_timeout->pipe_num, 1, ISP_WDR_CHN_MAX)) {
        viu_err_trace("pipe %d illegal pipe_num: %d, should between [%d,%d] !\n",
            pipe_id, send_raw_timeout->pipe_num, 1, ISP_WDR_CHN_MAX);
        return SOC_ERR_VIU_INVALID_PARA;
    }
#if defined(CONFIG_CHIP_3322)
#else
    td_u32 i;
    for (i = 0; i < send_raw_timeout->pipe_num; i++) {
        viu_check_pipe_id_return(send_raw_timeout->pipe_id[i]);
        if ((td_bool)read_user_linear_space_valid((td_u8 *)(td_uintptr_t)(send_raw_timeout->raw_info_addr[i]),
            sizeof(ext_isp_video_frame_info)) == TD_FALSE) {
            viu_err_trace("pipe %d raw_info_addr[%d] address is invalid!\n", pipe_id, i);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }
#endif

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    pipe_mode = viu_drv_get_pipe_mode(pipe_id);

    viu_pipe_up_sem(pipe_id);

    if (viu_drv_is_wdr_mode(wdr_mode) == TD_FALSE) {
        drv_pipe_get_raw_source(pipe_id, &source);
        return viu_line_mode_send_pipe_raw(pipe_id, send_raw_timeout, source, pipe_mode);
    } else {
#if defined(CONFIG_CHIP_3322)
        return TD_FAILURE;
#else
        return viu_wdr_mode_send_pipe_raw_from_be(pipe_id, send_raw_timeout, pipe_mode);
#endif
    }
}

static td_s32 viu_get_pipe_raw_wait(td_s32 pipe_id, td_s32 milli_sec, td_s32 unique_id)
{
    viu_wait_cond_info viu_wait_conf_info;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    td_s32 ret;

    viu_wait_conf_info.viu_pipe_id = pipe_id;
    viu_wait_conf_info.viu_chn_id = 0;
    viu_wait_conf_info.unique_id = unique_id;
    viu_wait_conf_info.wait_flag = TD_FALSE;
    viu_wait_conf_info.wake_condtion = VIU_GETRAW_WAKEUP;

    /* -1表示阻塞式等待，0表示非阻塞，>0表示超时 */
    if (milli_sec > 0) {
        viu_pipe_up_sem(pipe_id);

        ret = mcp_osal_wait_timeout_interruptible(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame,
            viu_wait_condition_call_back, &viu_wait_conf_info, (milli_sec));

        viu_pipe_down_interruptible_sem(pipe_id);

        if (ret < 0) {
            viu_err_trace("func: %s, line: %d ERESTARTSYS\n", __FUNCTION__, __LINE__);
            return -ERESTARTSYS;
        } else if (ret == 0) {
            viu_err_trace("vi pipe %d   get  frame time out, milli_sec=%d\n",
                pipe_id, milli_sec);
            return SOC_ERR_VIU_BUF_EMPTY;
        } else {
            if (viu_wait_conf_info.wait_flag == TD_FALSE) {
                return SOC_ERR_VIU_SYS_NOTREADY;
            }
        }
    } else if (-1 == milli_sec) {
        viu_pipe_up_sem(pipe_id);

        ret = mcp_osal_wait_interruptible(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame,
            viu_wait_condition_call_back, &viu_wait_conf_info);

        viu_pipe_down_interruptible_sem(pipe_id);

        if (ret != 0) {
            viu_err_trace("func: %s, line: %d ERESTARTSYS\n", __FUNCTION__, __LINE__);
            return -ERESTARTSYS;
        } else {
            if (viu_wait_conf_info.wait_flag == TD_FALSE) {
                return SOC_ERR_VIU_SYS_NOTREADY;
            }
        }
    } else {  /* no block */
        if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->dump_pipe_raw_pic_queue) == 0) {
            return SOC_ERR_VIU_BUF_EMPTY;
        }
    }
    return TD_SUCCESS;
}

static td_s32 viu_get_pipe_raw_video(td_s32 pipe_id, ext_isp_video_frame_info *video_frame,
    viu_cap_pic_node *pic_node)
{
    td_s32 ret;

    ret = viu_drv_get_vb(&pic_node->cap_pic_info.pic_info.video_frame, VB_UID_USER);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d add user failure!\n", pipe_id);
    }

    ret = viu_drv_put_vb_with_ret_value(&pic_node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d add VI failure!\n", pipe_id);
    }

    (td_void)memcpy_s(video_frame, sizeof(ext_isp_video_frame_info),
        &pic_node->cap_pic_info.pic_info.video_frame, sizeof(pic_node->cap_pic_info.pic_info.video_frame));

    return ret;
}

td_s32 viu_get_pipe_raw(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec)
{
    td_s32 ret = TD_SUCCESS;
    td_s32 unique_id;
    td_ulong flags;
    viu_cap_pic_node *pic_node = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    viu_check_phy_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(video_frame);

    if (milli_sec < -1) {
        viu_err_trace("milli_sec[%d] must be >= -1!\n", milli_sec);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        goto end;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    viu_pipe_spin_lock(pipe_id, flags);
    unique_id = get_pipe_unique_id(pipe_id);
    viu_pipe_spin_unlock(pipe_id, flags);

    ret = viu_get_pipe_raw_wait(pipe_id, milli_sec, unique_id);
    if (ret != TD_SUCCESS) {
        goto end;
    }

    viu_pipe_spin_lock(pipe_id, flags);

    ret = viu_abnormal_verify_after_wait(pipe_id, unique_id);
    if (ret != TD_SUCCESS) {
        viu_pipe_spin_unlock(pipe_id, flags);
        goto end;
    }

    pic_node = viu_pic_queue_get_busy(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, viu_cap_pic_node);
    if (pic_node == TD_NULL) {
        viu_pipe_spin_unlock(pipe_id, flags);
        ret = SOC_ERR_VIU_BUF_EMPTY;
        goto end;
    }

    ret = viu_get_pipe_raw_video(pipe_id, video_frame, pic_node);

    viu_pic_queue_put_free(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, pic_node);
    viu_pipe_spin_unlock(pipe_id, flags);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

td_s32 viu_set_dev_timing_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr)
{
    td_s32 ret = TD_SUCCESS;
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;

    viu_dev_check_id_return_val(viu_dev_id);
    viu_check_null_ptr_return(dev_timing_attr);

    viu_dev_down_interruptible_sem(viu_dev_id);

    viu_dev_spin_lock(viu_dev_id, flags);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);

    if ((dev_timing_attr->enable != TD_TRUE) && (dev_timing_attr->enable != TD_FALSE)) {
        viu_err_trace("enable should be 0 or 1 which is %d!\n", dev_timing_attr->enable);
        ret = SOC_ERR_VIU_INVALID_PARA;
        goto end;
    }

    if (viu_dev_ctx->dev_en != TD_TRUE) {
        viu_err_trace("viu_dev_id: %d is not enable, set dev_timing failed!\n", viu_dev_id);

        ret = SOC_ERR_VIU_FAILED_NOTENABLE;

        goto end;
    }

    viu_drv_set_timg_attr(viu_dev_id, dev_timing_attr);

end:
    viu_dev_spin_unlock(viu_dev_id, flags);

    viu_dev_up_sem(viu_dev_id);

    return ret;
}

td_s32 viu_get_dev_timing_attr(td_s32 viu_dev_id, ext_viu_dev_timing_attr *dev_timing_attr)
{
    viu_dev_check_id_return_val(viu_dev_id);

    viu_check_null_ptr_return(dev_timing_attr);

    viu_dev_down_interruptible_sem(viu_dev_id);

    viu_drv_get_timg_attr(viu_dev_id, dev_timing_attr);

    viu_dev_up_sem(viu_dev_id);

    return TD_SUCCESS;
}

td_s32 viu_set_pipe_dump_attr(td_s32 pipe_id, const ext_viu_dump_attr *dump_attr)
{
    td_s32 ret;
    td_ulong flags;
    ext_isp_wdr_mode wdr_mode;
    ext_viu_operating_mode_type mode;
    viu_cap_pic_node *node = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    /* 参数检查 */
    viu_check_phy_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(dump_attr);

    ret = viu_drv_check_pipe_dump_attr(pipe_id, dump_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        goto end;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    mode = viu_drv_get_pipe_mode(pipe_id);
    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (((mode != VIU_OFFLINE_PPU_OFFLINE) && (mode != VIU_OFFLINE_PPU_ONLINE)) ||
        (wdr_mode == EXT_ISP_WDR_MODE_QUDRA)) {
        if (pipe_mkp_ctx->cap_raw == TD_TRUE) {
            pipe_mkp_ctx->cap_raw = TD_FALSE;
            viu_wait_capture_frame_stop(pipe_id, &pipe_mkp_ctx->raw_wait_stop_status);

            viu_release_pipe_capture_vb(pipe_id);
        }
    }

    /* 清空raw队列缓存,可以抽出来作为公共方法 */
    viu_pipe_spin_lock(pipe_id, flags);

    while ((node = viu_pic_queue_get_busy(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, viu_cap_pic_node)) != TD_NULL) {
        viu_drv_put_vb(&node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
        viu_pic_queue_put_free(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, node);
    }

    if (dump_attr->enable == TD_TRUE) {
        pipe_mkp_ctx->raw_wait_stop_status = VIU_WAIT_STOP_PREPARE;

        pipe_mkp_ctx->cap_raw = TD_TRUE;
    }

    viu_drv_set_pipe_dump_attr(pipe_id, dump_attr);

    viu_pipe_spin_unlock(pipe_id, flags);

end:
    viu_pipe_up_sem(pipe_id);
    return ret;
}

td_s32 viu_get_pipe_dump_attr(td_s32 pipe_id, ext_viu_dump_attr *dump_attr)
{
    td_s32 ret = TD_SUCCESS;

    /* 参数检查 */
    viu_check_phy_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(dump_attr);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        goto end;
    }

    viu_drv_get_pipe_dump_attr(pipe_id, dump_attr);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
