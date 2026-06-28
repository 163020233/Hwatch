/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_drv_comm_vicap.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_pic_queue.h"
#include "viu_drv.h"
#include "viu_drv_define.h"
#include "viu_drv_comm.h"
#include "viu_extern.h"

#include "hal_adapt_comm.h"
#include "drv_vicap.h"
#include "hal_vicap_intf.h"
#include "pmu_sub_control.h"
#include "hal_viproc_intf.h"

#include "hal_capability_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 viu_comm_check_video_frame_valid(td_s32 pipe_id, const ext_isp_video_frame *video_frame, td_bool is_wdr)
{
    if (video_frame == TD_NULL) {
        viu_err_trace("pipe %d, input ptr NULL error!\n", pipe_id);
        return SOC_ERR_VIU_INVALID_NULL_PTR;
    }

    if (video_frame->field != EXT_VIDEO_FIELD_FRAME) {
        viu_err_trace("pipe %d, invalid field:%d, only support EXT_VIDEO_FIELD_FRAME.\n",
            pipe_id, video_frame->field);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (video_frame->video_format != EXT_VIDEO_FORMAT_LINEAR) {
        viu_err_trace("pipe %d, invalid video_format:%d, only support EXT_VIDEO_FORMAT_LINEAR.\n",
            pipe_id, video_frame->video_format);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if ((is_wdr == TD_FALSE) && (video_frame->dynamic_range != EXT_DYNAMIC_RANGE_SDR8)) {
        viu_err_trace("pipe %d, invalid dynamic_range:%d, only support EXT_DYNAMIC_RANGE_SDR8.\n",
            pipe_id, video_frame->dynamic_range);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (video_frame->color_space >= EXT_ISP_COLOR_GAMUT_MAX) {
        viu_err_trace("pipe %d, invalid color_gamut:%d, should between [%d, %d).\n",
            pipe_id, video_frame->color_space, EXT_ISP_COLOR_GAMUT_BT601, EXT_ISP_COLOR_GAMUT_MAX);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    return TD_SUCCESS;
}

td_s32 viu_model_create_one_model(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
#else
    td_s32 ret;
    viu_drv_model *viu_model = TD_NULL;
    viu_comm_ctx *comm_ctx = TD_NULL;
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode != VIU_OFFLINE_PPU_OFFLINE) && (pipe_mode != VIU_OFFLINE_PPU_ONLINE)) {
        return TD_SUCCESS;
    }

    /* 获取上下文 */
    comm_ctx = viu_get_comm_ctx();

    /* 根据grp_id找到该mode */
    viu_model = &comm_ctx->model[pipe_id];

    /* 为model分配内存 */
    ret = viu_drv_malloc_model_buf(pipe_id, viu_model);
    if (ret != TD_SUCCESS) {
        return ret;
    }
#endif
    return TD_SUCCESS;
}

td_void viu_model_destroy_one_model(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
#else
    viu_drv_model *viu_model = TD_NULL;
    viu_comm_ctx *comm_ctx = TD_NULL;
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode != VIU_OFFLINE_PPU_OFFLINE) && (pipe_mode != VIU_OFFLINE_PPU_ONLINE)) {
        return;
    }

    /* 获取上下文 */
    comm_ctx = viu_get_comm_ctx();

    /* 根据pipe_id找到该model */
    viu_model = &comm_ctx->model[pipe_id];

    /* 释放该model的buffer */
    viu_drv_free_model_buf(viu_model);
#endif
}

td_void viu_comm_prepare_next_frame_pipe_attr(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
#else
    ext_viu_operating_mode_type mode;

    mode = viu_drv_get_pipe_mode(pipe_id);
    if ((mode == VIU_ONLINE_PPU_OFFLINE) || (mode == VIU_ONLINE_PPU_ONLINE)) {
        viu_comm_proc_online_update_attr(pipe_id);
    } else {
        /* Invalid pipe work mode. Only be called in vi online scenario */
    }
#endif
}

static td_void viu_static_err_int_drop_deal(viu_err_int_detect *err_int_detect)
{
    err_int_detect->cur_err_int = TD_TRUE; /* 强制丢drop_err_frm_cnt帧 */
    err_int_detect->cur_drop_err_frm_cnt++;

    /* 这个周期已经丢帧完毕 */
    if (err_int_detect->cur_drop_err_frm_cnt == err_int_detect->drop_err_frm_cnt) {
        err_int_detect->need_lost_frame = TD_FALSE;
        err_int_detect->cur_drop_err_frm_cnt = 0;
        err_int_detect->cur_err_int_cnt_to_drop_frame = 0;
    }
}

static td_void viu_static_err_int_drop_detect(viu_err_int_detect *err_int_detect, td_bool int_err)
{
    if (int_err == TD_TRUE) {
        /* 如果有错误，错误中断计数加1，如果错误中断计数到阈值，则就要丢帧了 */
        err_int_detect->cur_err_int_cnt_to_drop_frame++;
        if (err_int_detect->cur_err_int_cnt_to_drop_frame == err_int_detect->err_int_cnt_to_drop_frame) {
            err_int_detect->cur_err_int = TD_TRUE;
            err_int_detect->need_lost_frame = TD_TRUE;
            err_int_detect->cur_drop_err_frm_cnt = 1; /* 从本帧开始丢 */
            err_int_detect->cur_err_int_cnt_to_drop_frame = 0;

            /* 这个周期已经丢帧完毕 */
            if (err_int_detect->cur_drop_err_frm_cnt == err_int_detect->drop_err_frm_cnt) {
                err_int_detect->need_lost_frame = TD_FALSE;
                err_int_detect->cur_drop_err_frm_cnt = 0;
                err_int_detect->cur_err_int_cnt_to_drop_frame = 0;
            }
        } else {
            err_int_detect->cur_err_int = TD_FALSE;
        }
    } else {
        err_int_detect->cur_err_int = TD_FALSE;
        /* 如果没有错误，错误中断直接清零 */
        err_int_detect->need_lost_frame = TD_FALSE;
        err_int_detect->cur_drop_err_frm_cnt = 0;
        err_int_detect->cur_err_int_cnt_to_drop_frame = 0;
    }
}

td_bool viu_statistic_err_int_detect(viu_err_int_detect *err_int_detect, td_bool int_err)
{
    /* 不开错误检测 强制不丢帧 */
    if (err_int_detect->detect_err_int == TD_FALSE) {
        err_int_detect->cur_err_int = TD_FALSE;
    } else {
        /* 检测到需要丢帧 */
        if (err_int_detect->need_lost_frame == TD_TRUE) {
            viu_static_err_int_drop_deal(err_int_detect);
        } else { /* 检测到不需要丢帧 */
            viu_static_err_int_drop_detect(err_int_detect, int_err);
        }
    }

    return err_int_detect->cur_err_int;
}

static td_void viu_comm_lost_cfg_int_repair_err(td_s32 pipe_id)
{
    td_s32 chn_id;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_pipe_statistic *pipe_statistic = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_statistic = &pipe_mkp_ctx->pipe_statistic;

    if (pipe_statistic->lost_cfg_stage == VIU_LOST_CFG_STAGE_NONE) {
        /* STEP1:刚检测到丢配置中断，当前需要发送的帧不能发送，因为逻辑接下来还会往这个VB写，VB也不能释放 */
        pipe_statistic->cur_int_err = TD_TRUE;

        for (chn_id = 0; chn_id < VIU_MAX_PHY_CHN_NUM; chn_id++) {
            (td_void)memcpy_s(&pipe_mkp_ctx->lost_cfg_frm[chn_id], sizeof(viu_chn_buf_info),
                &pipe_mkp_ctx->chn_this_frm[chn_id], sizeof(pipe_mkp_ctx->chn_this_frm[chn_id]));

            if (pipe_mkp_ctx->lost_cfg_frm[chn_id].buf_ok == TD_TRUE) {
                viu_drv_get_vb(&pipe_mkp_ctx->lost_cfg_frm[chn_id].pic_info.video_frame, VB_UID_VIU);
            }

            if (pipe_mkp_ctx->lost_cfg_frm[chn_id].dis_buf_ok == TD_TRUE) {
                viu_drv_get_vb(&pipe_mkp_ctx->lost_cfg_frm[chn_id].pic_info.single_y_frame, VB_UID_VIU);
            }
        }

        pipe_statistic->lost_cfg_stage = VIU_LOST_CFG_STAGE_THIS_INT;
    } else if (pipe_statistic->lost_cfg_stage == VIU_LOST_CFG_STAGE_THIS_INT) {
        /* STEP2:检测到丢配置中断的下一个中断，当前需要发送的帧不能发送,但是可以释放，因为逻辑没往这个VB写 */
        pipe_statistic->cur_int_err = TD_TRUE;
        /* STEP3:之前丢配置中断未释放的VB需要释放，这里是连续丢配置则不释放 */
        pipe_statistic->lost_cfg_stage = VIU_LOST_CFG_STAGE_THIS_INT;
    }
}

static td_void viu_comm_lost_cfg_int_repair_no_err(td_s32 pipe_id)
{
    td_s32 chn_id;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_pipe_statistic *pipe_statistic = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_statistic = &pipe_mkp_ctx->pipe_statistic;

    if (pipe_statistic->lost_cfg_stage == VIU_LOST_CFG_STAGE_THIS_INT) {
        /* STEP1:检测到丢配置中断的下一个中断，当前需要发送的帧不能发送，但是可以释放，因为逻辑没往这个VB写 */
        pipe_statistic->lost_cfg_stage = VIU_LOST_CFG_STAGE_NONE;
        pipe_statistic->cur_int_err = TD_TRUE;

        /* STEP2:之前丢配置中断未释放的VB需要释放 */
        for (chn_id = 0; chn_id < VIU_MAX_PHY_CHN_NUM; chn_id++) {
            if (pipe_mkp_ctx->lost_cfg_frm[chn_id].buf_ok == TD_TRUE) {
                viu_drv_put_vb(&pipe_mkp_ctx->lost_cfg_frm[chn_id].pic_info.video_frame, VB_UID_VIU);
                pipe_mkp_ctx->lost_cfg_frm[chn_id].buf_ok = TD_FALSE;
            }

            if (pipe_mkp_ctx->lost_cfg_frm[chn_id].dis_buf_ok == TD_TRUE) {
                viu_drv_put_vb(&pipe_mkp_ctx->lost_cfg_frm[chn_id].pic_info.single_y_frame, VB_UID_VIU);
                pipe_mkp_ctx->lost_cfg_frm[chn_id].dis_buf_ok = TD_FALSE;
            }
        }
    }
}

td_void viu_comm_lost_cfg_int_repair(td_s32 pipe_id, td_bool lost_int_err)
{
    if (lost_int_err == TD_TRUE) {
        viu_comm_lost_cfg_int_repair_err(pipe_id);
    } else {
        viu_comm_lost_cfg_int_repair_no_err(pipe_id);
    }
}

td_u64 viu_comm_get_first_int_pts(td_u32 proc_id)
{
    td_ulong flags;
    td_s32 pipe_id;
    td_u64 first_pts;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_id = viu_drv_get_pipe_by_proc_id(proc_id);
    /* 用到了pipe上下文，需要和destroy pipe或reset pipe 互斥 */
    viu_pipe_spin_lock(pipe_id, flags);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return 0;
    }
    if ((is_pipe_destroying(pipe_id)) || (is_pipe_resetting(pipe_id))) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return 0;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    first_pts = pipe_mkp_ctx->first_int_pts;

    viu_pipe_spin_unlock(pipe_id, flags);

    return first_pts;
}

td_bool viu_comm_is_pipe_start(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return TD_FALSE;
    }
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if ((pipe_mkp_ctx->pipe_mkp_attr_working.start != TD_TRUE) &&
        (pipe_mkp_ctx->viu_snap_status != VIU_SNAP_STATUS_TRIGGER)) {
        viu_err_trace("pipe %d has not been started or triggered.\n", pipe_id);
        return TD_FALSE;
    }

    return TD_TRUE;
}

/******************************vicap init 调用的函数***************************************************************/
td_void viu_comm_cap_statistics_int_time(td_s32 pipe_id, td_u64 start_time)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_u64 end_time;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->wch_dbg.not_first_int == TD_TRUE) {
        pipe_mkp_ctx->wch_dbg.int_gap_time = (td_u32)(start_time - pipe_mkp_ctx->wch_dbg.last_int_time);

        if (pipe_mkp_ctx->wch_dbg.int_gap_time > pipe_mkp_ctx->wch_dbg.int_gap_time_max) {
            pipe_mkp_ctx->wch_dbg.int_gap_time_max = pipe_mkp_ctx->wch_dbg.int_gap_time;
        }
    }

    pipe_mkp_ctx->wch_dbg.last_int_time = start_time;

    end_time = mcp_osal_div64_u64(osal_sched_clock(), 1000U);
    pipe_mkp_ctx->wch_dbg.int_time = (td_u32)(end_time - start_time);
    /* 统计中断处理时长超过int_time值的次数 */
    if (pipe_mkp_ctx->wch_dbg.int_time > VIU_MAX_INT_TIME) {
        viu_warn_trace("pipe %d int time is too long: %u\n", pipe_id, pipe_mkp_ctx->wch_dbg.int_time);
        pipe_mkp_ctx->wch_dbg.too_long_int_cnt++;
    }

    if (pipe_mkp_ctx->wch_dbg.int_time > pipe_mkp_ctx->wch_dbg.int_time_max) {
        pipe_mkp_ctx->wch_dbg.int_time_max = pipe_mkp_ctx->wch_dbg.int_time;
    }

    pipe_mkp_ctx->wch_dbg.not_first_int = TD_TRUE;
}

td_bool viu_check_frame_match_pipe_width_height(td_s32 mast_pipe, ext_isp_video_frame_info *video_frame_info)
{
    ext_viu_pipe_attr pipe_attr;

    viu_drv_get_pipe_attr(mast_pipe, &pipe_attr);
    if (pipe_attr.max_w != video_frame_info->v_frame.width) {
        viu_err_trace("max_w[%d] of pipe_attr is not equal to width[%d] of input frame.\n",
            pipe_attr.max_w, video_frame_info->v_frame.width);
        return TD_FALSE;
    }

    if (pipe_attr.max_h != video_frame_info->v_frame.height) {
        viu_err_trace("max_h[%d] of pipe_attr is not equal to height[%d] of input frame.\n",
            pipe_attr.max_h, video_frame_info->v_frame.height);
        return TD_FALSE;
    }

    return TD_TRUE;
}

#if defined(CONFIG_CHIP_3322)
static void viu_stop_wait_venc(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    td_u32 venc_queue_busy_num = 0;
    if (pipe_mkp_ctx->raw_wait_stop_status == VIU_WAIT_VENC_RETURN_FRAME) {
        venc_queue_busy_num = viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue);
        if (venc_queue_busy_num == 0) {
            pipe_mkp_ctx->raw_wait_stop_status = VIU_WAIT_STOP_END;
        }
        viu_info_trace("wait venc return %d frame\n", venc_queue_busy_num);
    }
}

td_s32 viu_get_venc_node_to_line_input_pic_queue(td_s32 pipe_id, const ext_isp_video_frame *v_frame)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_cap_pic_node *node = TD_NULL;
    td_ulong flags;
    td_u64 phy_addr = 0;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    viu_pipe_spin_lock(pipe_id, flags);
    while (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue) > 0) {
        node = viu_pic_queue_get_busy(&pipe_mkp_ctx->line_input_pic_queue, viu_cap_pic_node);
        if (node == TD_NULL) {
            viu_pipe_spin_unlock(pipe_id, flags);
            viu_err_trace("pipe %d no space to receive this frm!\r\n", pipe_id);
            return TD_FAILURE;
        }

        phy_addr = node->cap_pic_info.pic_info.video_frame.v_frame.phy_addr[0];
        if (phy_addr == v_frame->phy_addr[0]) {
            viu_drv_put_vb(&node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
            viu_pic_queue_put_free(&pipe_mkp_ctx->line_input_pic_queue, node);
            viu_pipe_spin_unlock(pipe_id, flags);
            viu_info_trace("-----venc DQ.[0x%llx]\n", v_frame->phy_addr[0]);
            viu_stop_wait_venc(pipe_id);
            return TD_SUCCESS;
        }
    }
    viu_pipe_spin_unlock(pipe_id, flags);

    osal_printk("-----venc err.\n");
    return TD_SUCCESS;
}

static td_s32 viu_put_venc_node_to_line_input_pic_queue(td_s32 pipe_id)
{
    td_s32 ret = TD_SUCCESS;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_buf_info *this_frm = TD_NULL;
    viu_cap_pic_node *node = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->dst_mod == -1) {
        return TD_SUCCESS;
    }
    this_frm = &pipe_mkp_ctx->this_frm;

    node = viu_pic_queue_get_free(&pipe_mkp_ctx->line_input_pic_queue, viu_cap_pic_node);
    if (node == TD_NULL) {
        viu_err_trace("pipe %d no space to receive this frm!\r\n", pipe_id);
        return TD_FAILURE;
    }

    this_frm->pic_info.start_time = mcp_osal_div64_u64(osal_sched_clock(), 1000U);
    ret = viu_drv_get_vb(&this_frm->pic_info.video_frame, VB_UID_VIU);
    if (ret != TD_SUCCESS) {
        viu_err_trace("put_venc_node_to_queue, pipe %d add getframe err 0x%x!\n", pipe_id, ret);
        viu_pic_queue_put_free(&pipe_mkp_ctx->line_input_pic_queue, node);
        return TD_FAILURE;
    }

    (td_void)memcpy_s(&node->cap_pic_info, sizeof(viu_buf_info), this_frm, sizeof(*this_frm));

    node->cap_pic_info.lost_repeat_frame = TD_FALSE;
    viu_pic_queue_put_busy(&pipe_mkp_ctx->line_input_pic_queue, node);

    viu_ext_venc_quque_frame(pipe_id, 0, &this_frm->pic_info.video_frame.v_frame);
    viu_info_trace("-----venc Q.[0x%llx]\n", this_frm->pic_info.video_frame.v_frame.phy_addr[0]);

    return TD_SUCCESS;
}
#endif

/*****************************************************************************
  1.Date         : 2016/11/15
    Modification : 发送线性模式采集的帧
                   1.首先判断是线性模式(wdr模式，return)
                   2.从line_input_pic_queue free队列取出节点，接收采集的帧
                   3.加入busy队列(VB++)，启动proc ons task ，出来busy队列节点，
*****************************************************************************/
static td_void viu_comm_cap_send_linear_frame(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode)
{
    ext_viu_pipe_frame_source source;
    viu_buf_info *this_frm = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    drv_pipe_get_raw_source(pipe_id, &source);
    if (source == EXT_VIU_PIPE_FRAME_SOURCE_USER_BE) {
        return;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    this_frm = &pipe_mkp_ctx->this_frm;
    if ((this_frm->buf_ok == TD_FALSE) || (this_frm->cap_ok == TD_FALSE)) {
        return;
    }

    if ((pipe_mkp_ctx->viu_snap_pipe_ctx.double_pipe == TD_TRUE) &&
        (pipe_mkp_ctx->viu_snap_status != VIU_SNAP_STATUS_DISABLE)) {
        return;
    }

    if (viu_check_frame_match_pipe_width_height(pipe_id, &this_frm->pic_info.video_frame) != TD_TRUE) {
        pipe_mkp_ctx->wch_dbg.lost_int++;
        pipe_mkp_ctx->pipe_statistic.pipe_frmlost_cnt++;
        return;
    }
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_mode);
    viu_put_venc_node_to_line_input_pic_queue(pipe_id);
#else
    td_s32 ret;
    ret = viu_put_node_to_line_input_pic_queue(pipe_id);
    if (ret != TD_SUCCESS) {
        pipe_mkp_ctx->wch_dbg.lost_int++;
        pipe_mkp_ctx->pipe_statistic.pipe_frmlost_cnt++;
    }

    pipe_mkp_ctx->pipe_statistic.recv_pic++;

    (td_void)viu_start_one_task(pipe_id, pipe_mode);
#endif
    return;
}

/*****************************************************************************
  1.Date         : 2016/11/15
    Modification : 发送bayerdump帧
                   1.首先判断bayerdump enable，并且 depth >0 (否则return)
                   2.如果busy 队 大于depth ，出队，让出空间
                   3.加入busy队列(VB++)，唤醒用户get_frame 信号量
*****************************************************************************/
td_void viu_comm_cap_send_dump_frame(td_s32 pipe_id)
{
    td_s32 ret = TD_SUCCESS;
    viu_buf_info *this_frm = TD_NULL;
    viu_cap_pic_node *node = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    ext_viu_dump_attr dump_attr;
    td_ulong flags;

    /* 如果bayerdump 没开启，直接return */
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    viu_drv_get_working_pipe_dump_attr(pipe_id, &dump_attr);

    if (dump_attr.enable != TD_TRUE) {
        return;
    }

    this_frm = &pipe_mkp_ctx->this_frm;
    if ((this_frm->buf_ok == TD_FALSE) || (this_frm->cap_ok == TD_FALSE)) {
        return;
    }

#if defined(CONFIG_CHIP_3322)
#else
    if (viu_snap_is_valid_dump_frame(pipe_id) == TD_FALSE) {
        return;
    }
#endif

    /* 队列访问加锁,下面每个方法实现已加锁，因此调用处可不再加锁 */
    if (dump_attr.depth > 0) {
        /* 删除旧结点 */
        viu_pipe_spin_lock(pipe_id, flags);
        while (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->dump_pipe_raw_pic_queue) >= dump_attr.depth) {
            node = viu_pic_queue_get_busy(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, viu_cap_pic_node);
            if (node == TD_NULL) {
                viu_pipe_spin_unlock(pipe_id, flags);
                viu_err_trace("cap dump, pipe %d no space to receive this frm!\r\n", pipe_id);
                return;
            }

            viu_drv_put_vb(&node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
            viu_pic_queue_put_free(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, node);
        }

        /* 新结点入队列 */
        node = viu_pic_queue_get_free(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, viu_cap_pic_node);
        if (node != TD_NULL) {
            (td_void)memcpy_s(&node->cap_pic_info, sizeof(viu_buf_info), this_frm, sizeof(*this_frm));

            ret = viu_drv_get_vb(&node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
            if (ret != TD_SUCCESS) {
                viu_err_trace("cap dump, pipe %d add getframe err 0x%x!\n", pipe_id, ret);
                viu_pic_queue_put_free(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, node);
                viu_pipe_spin_unlock(pipe_id, flags);
                return;
            }
            viu_pic_queue_put_busy(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, node);
            mcp_osal_wait_wakeup(&(g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame));
        }
        viu_pipe_spin_unlock(pipe_id, flags);
    }
}

/*****************************************************************************
  1.Date         : 2016/11/15
    Modification : pipe offline 模式，发送上一帧(wdr,线性，raw)
                   1.首先判断buf是否ok，如果ok 记得 发送成功失败 都要VB--
                   2.采集是否ok，如果ok，调用各自发送函数
                   3.出错情况，proc统计信息累加
*****************************************************************************/
static td_void viu_comm_cap_send_offline_frame(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode)
{
    td_bool frame_ok = TD_FALSE;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if ((pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) &&
        (pipe_mkp_ctx->this_frm.cap_ok == TD_TRUE)) {
        frame_ok = TD_TRUE;
    }

    if (frame_ok == TD_FALSE) {
        /* 出错丢帧，错误计数 */
        if (pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) {
            pipe_mkp_ctx->wch_dbg.lost_int++;
        }
        viu_info_trace("VI pipe_id %d buf_ok:%d cap_ok:%d lost_int++!\n", pipe_id, pipe_mkp_ctx->this_frm.buf_ok,
            pipe_mkp_ctx->this_frm.cap_ok);
        goto fail;
    }
#if defined(CONFIG_CHIP_3322)
    viu_comm_cap_send_dump_frame(pipe_id);
    viu_comm_cap_send_linear_frame(pipe_id, pipe_mode);
#else
    ext_isp_wdr_mode wdr_mode;
    if ((pipe_mkp_ctx->this_frm.pic_info.video_frame.v_frame.compress_mode != EXT_ISP_COMPRESS_MODE_NONE) &&
        (viu_drv_need_split(pipe_id) == TD_TRUE)) {
        /* 离线分块不支持压缩模式，错误计数，返回 */
        pipe_mkp_ctx->wch_dbg.lost_int++;
        viu_err_trace("===> offline split need no compress mode, return.\n");
        goto fail;
    }

    /****************************中断处理*************************************/
    viu_snap_cap_send_frame(pipe_id);

    viu_comm_cap_send_dump_frame(pipe_id);

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (viu_drv_is_wdr_mode(wdr_mode) == TD_TRUE) {
        viu_comm_cap_send_wdr_frame(pipe_id, pipe_mode);
    } else {
        viu_comm_cap_send_linear_frame(pipe_id, pipe_mode);
    }
#endif
    /* +1说明:VB申请时+1,例外
          发送到line队列(或wdr队列)+1
          发送到raw队列+1

    -1说明:发送完-1,对应申请时
           proc处理完line队列一个节点 -1
           用户从raw队列get一个节点，-1，MPI+1
    */
    viu_pipe_frame_rate_statistic(pipe_id, frame_ok);

fail:
    if (pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) {
        viu_drv_put_vb(&pipe_mkp_ctx->this_frm.pic_info.video_frame, VB_UID_VIU);
        pipe_mkp_ctx->this_frm.buf_ok = TD_FALSE;
    }
}

static td_s32 viu_cap_prepare_next_frame_buffer(td_s32 pipe_id, viu_buf_info *next_frm)
{
    td_s32 ret = TD_SUCCESS;
    viu_vb_source vb_source;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    vb_source = viu_get_vb_source();

    ret = viu_drv_get_wch_vb_blk(pipe_id, vb_source, pipe_mkp_ctx->pipe_vb_pool, &next_frm->pic_info.video_frame);
    return ret;
}

/*
 * 1.Date         : 2016/11/15
 *   Modification : 准备采集下一帧
 *                  1.wdr模式，bayerdump关闭，用私有vb采集
 *                       主pipe在线，buf不轮转(方案统一成，next 与 this指针 用同一个buf轮转)
 *                  2.线性模式 ，或wdr模式下开启bayerdump 用公共vb
 */
td_void viu_comm_cap_prepare_frame(td_s32 pipe_id)
{
    td_s32 ret;
    viu_buf_info *next_frm = TD_NULL;
    viu_buf_info *this_frm = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    this_frm = &pipe_mkp_ctx->this_frm;
    if (this_frm->buf_ok == TD_TRUE) {
        viu_drv_put_vb(&this_frm->pic_info.video_frame, VB_UID_VIU);
        this_frm->buf_ok = TD_FALSE;
        viu_err_trace("pipe %d lost frame because of wrong int order!\n", pipe_id);
    }

    next_frm = &pipe_mkp_ctx->next_frm;
    (td_void)memcpy_s(this_frm, sizeof(viu_buf_info), next_frm, sizeof(*next_frm));

    if (viu_is_wait_cap_fram_stop(pipe_mkp_ctx, &pipe_mkp_ctx->raw_wait_stop_status) == TD_TRUE) {
        goto vb_fail;
    }

    if (pipe_mkp_ctx->cap_raw != TD_TRUE) {
        goto vb_fail;
    }

    if (viu_drv_pipe_frame_rate_ctl(pipe_id) == TD_FALSE) {
        viu_info_trace("pipe %d lost frame according frame rate control !\n", pipe_id);
        goto vb_fail;
    }

    ret = viu_cap_prepare_next_frame_buffer(pipe_id, next_frm);
    if (ret != TD_SUCCESS) {
        pipe_mkp_ctx->wch_dbg.get_vb_fail++;
        pipe_mkp_ctx->wch_dbg.lost_int++;
        viu_err_trace("pipe id %d get vb fail\n", pipe_id);
        goto vb_fail;
    }

    next_frm->buf_ok = TD_TRUE;
    return;

vb_fail:
    next_frm->buf_ok = TD_FALSE;
    return;
}

td_void viu_comm_update_first_frame_pipe_attr(td_s32 pipe_id)
{
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_ulong flags;
    ext_viu_operating_mode_type mode;

    mode = viu_drv_get_pipe_mode(pipe_id);
    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (((wdr_mode == EXT_ISP_WDR_MODE_NONE) || (wdr_mode == EXT_ISP_WDR_MODE_BUILT_IN)) &&
        ((mode == VIU_ONLINE_PPU_OFFLINE) || (mode == VIU_ONLINE_PPU_ONLINE))) {
        viu_pipe_spin_lock(pipe_id, flags);
        pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
        if (pipe_mkp_ctx->update_first_frame_pipe_attr == TD_TRUE) {
            viu_pipe_spin_unlock(pipe_id, flags);
            return;
        }
        viu_update_working_pipe_mkp_attr(pipe_id);
        viu_pipe_spin_unlock(pipe_id, flags);

        viu_comm_prepare_next_frame_pipe_attr(pipe_id);

        viu_pipe_spin_lock(pipe_id, flags);
        pipe_mkp_ctx->update_first_frame_pipe_attr = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
    }
}

/*
 * 1.Date         : 2016/11/15
 *   Modification : 修改bayerdump中断处理流程，
 *                  1.pipe上下文非空检测，帧起始检测等提到上层(WDR,bayer_dump均需要)
 *                  2.准备下一帧，启动采集调用关系提到上一层，本函数仅保留发送功能。
 */
static td_void viu_comm_bayer_dump_irq_route(td_s32 pipe_id, td_u64 irq_begin_time)
{
    td_bool frame_ok = TD_FALSE;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) {
        if (pipe_mkp_ctx->this_frm.cap_ok == TD_TRUE) {
            frame_ok = TD_TRUE;
            pipe_mkp_ctx->this_frm.pic_info.video_frame.v_frame.pts = irq_begin_time;
            pipe_mkp_ctx->this_frm.pic_info.video_frame.v_frame.time_ref = viu_calc_raw_time_ref(pipe_id);

            /* 发送一帧 */
            viu_comm_cap_send_dump_frame(pipe_id);
        } else {
            /* 出错丢帧，错误计数 */
            pipe_mkp_ctx->wch_dbg.lost_int++;
            viu_info_trace("VI pipe_id %d lost_int++!\n", pipe_id);
        }

        viu_drv_put_vb(&pipe_mkp_ctx->this_frm.pic_info.video_frame, VB_UID_VIU);
        pipe_mkp_ctx->this_frm.buf_ok = TD_FALSE;
    }

    viu_pipe_frame_rate_statistic(pipe_id, frame_ok);
}

td_bool viu_comm_need_do_irq_route(td_s32 pipe_id, td_bool master_pipe_do_irq)
{
    td_s32 master_pipe;

    master_pipe = viu_drv_get_mast_pipe_by_pipe(pipe_id);

    if (master_pipe_do_irq == TD_TRUE) {
        if (master_pipe != pipe_id) {
            return TD_FALSE;
        }
    } else {
        if (master_pipe == pipe_id) {
            return TD_FALSE;
        }
    }

    return TD_TRUE;
}

static td_void viu_comm_cap_mark_frame_ok(td_s32 pipe_id, ext_isp_frame_interrupt_type int_type,
    td_u32 int_status)
{
    td_bool cap_ok = TD_FALSE; /* default false */
    td_bool cur_int_err;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) {
        cur_int_err = viu_drv_cap_is_wrong_int(pipe_id, int_type, int_status);
        if (viu_statistic_err_int_detect(&pipe_mkp_ctx->wch_dbg.err_int_detect, cur_int_err) == TD_FALSE) {
            cap_ok = TD_TRUE;
        }
    }

    pipe_mkp_ctx->this_frm.cap_ok = cap_ok;
}

static td_void viu_comm_cap_online_send_last_frame(td_s32 pipe_id, td_u64 irq_begin_time)
{
#if defined(CONFIG_CHIP_3322)
#else
    viu_snap_find_pro_frame(pipe_id);
    viu_snap_online_irq_proc(pipe_id);
#endif
    viu_comm_bayer_dump_irq_route(pipe_id, irq_begin_time);
}

static td_void viu_comm_cap_online_prepare_next_frame(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode)
{
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_dev_drv_ctx *dev_drv_ctx = TD_NULL;
    ext_isp_crop_info *crop_info = TD_NULL;
    viu_buf_info *buf_info = TD_NULL;
    td_bool yuv_skip;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    dev_drv_ctx = &g_ast_viu_drv_dev_ctx[0];

    viu_unused(pipe_mode);

    /* 准备下一帧 */
    viu_comm_cap_prepare_frame(pipe_id);

    /* 配置帧 */
    crop_info = &pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info;
    yuv_skip = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.yuv_skip;
    buf_info = &pipe_mkp_ctx->next_frm;
    hal_videv_process_loop(0, &dev_drv_ctx->viu_dev_attr);
    hal_vicap_process(pipe_id, yuv_skip, buf_info->buf_ok, crop_info, &buf_info->pic_info.video_frame);
}

static td_void viu_comm_cap_online_frame_start_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_u64 irq_begin_time)
{
    viu_unused(irq_begin_time);

    td_u32 int_status;

    int_status = hal_vicap_get_ch_int_status(pipe_id);
    if (is_cap_frame_start_irq(int_status) == 0) {
        /* 检测不到帧起始，退出 */
        return;
    }

    /* 清帧起始+错误中断+帧完成 */
    hal_vicap_clear_ch_int(pipe_id, (td_u32)(VICAP_FRAME_START_INT | VICAP_FRAME_END_INT | VICAP_FRAME_ERR_INT));

    /* 判断上一帧是否采集正常 */
    viu_comm_cap_mark_frame_ok(pipe_id, EXT_ISP_FRAME_INTERRUPT_START, int_status);

    /* 发送上一帧 */
    viu_comm_cap_online_send_last_frame(pipe_id, irq_begin_time);

    /* 准备下一帧 */
    viu_comm_cap_online_prepare_next_frame(pipe_id, pipe_mode);
}

static td_void viu_cap_increase_wch_int_count(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_mkp_ctx->wch_dbg.int_cnt++;
}

td_void viu_comm_cap_online_frame_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_u64 irq_begin_time)
{
    ext_isp_frame_interrupt_attr frame_int_attr;

    viu_drv_get_pipe_frame_interrupt_attr(pipe_id, &frame_int_attr);
    switch (frame_int_attr.int_type) {
        case EXT_ISP_FRAME_INTERRUPT_START:
            viu_comm_cap_online_frame_start_irq_route(pipe_id, pipe_mode, irq_begin_time);
            break;
        default:
            viu_err_trace("Invalid frame int type:%d\n", frame_int_attr.int_type);
            break;
    }
}

static td_void viu_comm_cap_online_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_bool master_pipe_do_irq, td_u64 irq_begin_time)
{
    td_ulong flags;

    viu_pipe_spin_lock(pipe_id, flags);

    if (is_pipe_unexist(pipe_id)) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return;
    }

    if ((is_pipe_destroying(pipe_id)) || (is_pipe_resetting(pipe_id))) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_debug_trace("pipe %d is destroying/resetting/working!\n", pipe_id);
        return;
    }

    if (viu_comm_need_do_irq_route(pipe_id, master_pipe_do_irq) == TD_FALSE) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return;
    }

    pipe_cap_working_start(pipe_id);

    viu_cap_increase_wch_int_count(pipe_id);

    viu_update_working_pipe_chn_mkp_attr(pipe_id);

    viu_pipe_spin_unlock(pipe_id, flags);

    if (viu_comm_is_pipe_start(pipe_id) == TD_FALSE) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        goto end;
    }

    viu_comm_cap_online_frame_irq_route(pipe_id, pipe_mode, irq_begin_time);

    viu_comm_cap_statistics_int_time(pipe_id, irq_begin_time);

end:
    viu_pipe_spin_lock(pipe_id, flags);
    pipe_cap_working_finish(pipe_id);
    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working);
    viu_pipe_spin_unlock(pipe_id, flags);
}

static td_void viu_comm_cap_offline_send_last_frame(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_u64 irq_begin_time)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    /* 采集完成打上PTS,TIME REF */
    pipe_mkp_ctx->this_frm.pic_info.start_time = irq_begin_time;
    pipe_mkp_ctx->this_frm.pic_info.video_frame.v_frame.pts = irq_begin_time;
    pipe_mkp_ctx->this_frm.pic_info.video_frame.v_frame.time_ref = viu_calc_raw_time_ref(pipe_id);

    /* 发送上一帧:包含wdr raw，line, fusion */
    viu_comm_cap_send_offline_frame(pipe_id, pipe_mode);
}

static td_void viu_comm_cap_offline_prepare_next_frame(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_pipe_drv_ctx *pipe_drv_ctx = TD_NULL;
    viu_dev_drv_ctx *dev_drv_ctx = TD_NULL;
    ext_isp_crop_info *crop_info = TD_NULL;
    viu_buf_info *buf_info = TD_NULL;
    td_bool yuv_skip;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_drv_ctx = g_past_pipe_drv_ctx[pipe_id];
    dev_drv_ctx = &g_ast_viu_drv_dev_ctx[0];

    /* 准备下一帧 */
    viu_comm_cap_prepare_frame(pipe_id);

    /* 配置帧 */
    crop_info = &pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.crop_info;
    yuv_skip = pipe_drv_ctx->pipe_drv_attr_working.viu_pipe_attr.yuv_skip;
    buf_info = &pipe_mkp_ctx->next_frm;
    hal_videv_process_loop(0, &dev_drv_ctx->viu_dev_attr);
    hal_vicap_process(pipe_id, yuv_skip, buf_info->buf_ok, crop_info, &buf_info->pic_info.video_frame);
}

static td_void viu_comm_cap_offline_frame_start_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_u64 irq_begin_time)
{
    viu_unused(pipe_mode);
    viu_unused(irq_begin_time);

    td_u32 int_status;

    int_status = hal_vicap_get_ch_int_status(pipe_id);
    if (is_cap_frame_start_irq(int_status) == 0) {
        /* 检测不到帧起始 */
        return;
    }

    /* 清帧起始+错误中断+帧完成 */
    hal_vicap_clear_ch_int(pipe_id, (td_u32)(VICAP_FRAME_START_INT | VICAP_FRAME_END_INT | VICAP_FRAME_ERR_INT));

    /* 判断上一帧是否采集正常 */
    viu_comm_cap_mark_frame_ok(pipe_id, EXT_ISP_FRAME_INTERRUPT_START, int_status);

    /* 发送上一帧 */
    viu_comm_cap_offline_send_last_frame(pipe_id, pipe_mode, irq_begin_time);

    /* 准备下一帧 */
    viu_comm_cap_offline_prepare_next_frame(pipe_id);
}

static td_void viu_comm_cap_offline_frame_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_u64 irq_begin_time)
{
    ext_isp_frame_interrupt_attr frame_int_attr;

    viu_drv_get_pipe_frame_interrupt_attr(pipe_id, &frame_int_attr);
    switch (frame_int_attr.int_type) {
        case EXT_ISP_FRAME_INTERRUPT_START:
            viu_comm_cap_offline_frame_start_irq_route(pipe_id, pipe_mode, irq_begin_time);
            break;
        default:
            viu_err_trace("Invalid frame int type:%d\n", frame_int_attr.int_type);
            break;
    }
}

static td_void viu_comm_cap_offline_irq_route(td_s32 pipe_id, ext_viu_operating_mode_type pipe_mode,
    td_bool master_pipe_do_irq, td_u64 irq_begin_time)
{
    td_ulong flags;

    viu_pipe_spin_lock(pipe_id, flags);

    if (is_pipe_unexist(pipe_id)) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return;
    }

    if ((is_pipe_destroying(pipe_id)) || (is_pipe_resetting(pipe_id))) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d is destroying/resetting!\n", pipe_id);
        return;
    }

    if (viu_comm_need_do_irq_route(pipe_id, master_pipe_do_irq) == TD_FALSE) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return;
    }

    pipe_cap_working_start(pipe_id);
    viu_update_working_pipe_chn_mkp_attr(pipe_id);

    viu_cap_increase_wch_int_count(pipe_id);

    viu_pipe_spin_unlock(pipe_id, flags);

    if (viu_comm_is_pipe_start(pipe_id) == TD_FALSE) {
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        goto end;
    }

    viu_comm_cap_offline_frame_irq_route(pipe_id, pipe_mode, irq_begin_time);

end:
    viu_comm_cap_statistics_int_time(pipe_id, irq_begin_time);

    viu_pipe_spin_lock(pipe_id, flags);
    pipe_cap_working_finish(pipe_id);
    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working);
    viu_pipe_spin_unlock(pipe_id, flags);
}

static td_s32 viu_check_wdr_capacity(td_s32 pipe_id)
{
    td_s32 ret;
    td_ulong flags;

    /* 检查能力集用到了pipe上下文，需要和destroy pipe或reset pipe 互斥 */
    viu_pipe_spin_lock(pipe_id, flags);
    if ((is_pipe_destroying(pipe_id)) || (is_pipe_resetting(pipe_id))) {
        /* 清中断 */
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d is destroying/resetting!\n", pipe_id);
        return TD_FAILURE;
    }

    ret = viu_drv_check_wdr_capacity(pipe_id);
    if (ret != TD_SUCCESS) {
        /* 清中断 */
        hal_vicap_clear_ch_int(pipe_id, (td_u32)VICAP_FRAME_ALL_INT);
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d use is invalid\n", pipe_id);
        return TD_FAILURE;
    }
    viu_pipe_spin_unlock(pipe_id, flags);
    return TD_SUCCESS;
}
static td_void viu_comm_cap_all_mode_irq_route(td_s32 pipe_id, td_bool master_pipe_do_irq,
    td_u64 irq_begin_time)
{
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    switch (pipe_mode) {
        case VIU_OFFLINE_PPU_OFFLINE:
        case VIU_OFFLINE_PPU_ONLINE:
            viu_comm_cap_offline_irq_route(pipe_id, pipe_mode, master_pipe_do_irq, irq_begin_time);
            break;

        case VIU_ONLINE_PPU_OFFLINE:
        case VIU_ONLINE_PPU_ONLINE:
            viu_comm_cap_online_irq_route(pipe_id, pipe_mode, master_pipe_do_irq, irq_begin_time);
            break;

        default:
            viu_err_trace("Invalid pipe mode:%d\n", pipe_mode);
            break;
    }
}

#if defined(CONFIG_CHIP_3322)
#else
mcp_osal_workqueue g_vicap_bottom_half_work;
static td_void viu_cap_bottom_half_work_process(mcp_osal_workqueue *work)
{
    viu_drv_isp_int_bottom_half(0, 0);
}
#endif

static int viu_comm_cap_irq_route(td_s32 irq, td_void *dev_id)
{
    td_s32 pipe_id;
    td_u32 cap_int_status;
    td_u64 irq_begin_time;
    /*
     * 记录中断起始时间
     * osal_sched_clock()返回值单位为纳秒, 这里转换为微秒
     */
    irq_begin_time = hal_vicap_reg_get_timestamp();

    /* 获取全局中断状态 */
    cap_int_status = hal_vicap_get_int_status();

#if defined(CONFIG_CHIP_3322)
    viu_unused(irq);
    viu_unused(dev_id);
#else
    viu_drv_isp_isr(irq, dev_id);
#endif

    /*  中断状态是否有效 */
    if (viu_drv_is_cap_valid_int(cap_int_status) == TD_FALSE) {
#if !defined(CONFIG_CHIP_PLATFORM_WEAR)
        mcp_osal_workqueue_schedule(&g_vicap_bottom_half_work);
#endif
        return MCP_OSAL_IRQ_HANDLED;
    }

    /* 第1遍:先处理非主PIPE的中断(WDR模式需要) */
    for (pipe_id = 0; pipe_id < ISP_MAX_PHY_PIPE_NUM; pipe_id++) {
        /* 判断是否有这个PIPE对应的中断 */
        if (hal_vicap_get_int_report(pipe_id, cap_int_status) != TD_TRUE) {
            continue;
        }
        /* 检查wdr能力集 失败处理其他pipe */
        if (viu_check_wdr_capacity(pipe_id) != TD_SUCCESS) {
            continue;
        }

        viu_comm_cap_all_mode_irq_route(pipe_id, TD_FALSE, irq_begin_time);
    }

    /* 第2遍:再处理主PIPE的中断(WDR模式需要) */
    for (pipe_id = 0; pipe_id < ISP_MAX_PHY_PIPE_NUM; pipe_id++) {
        /* 判断是否有这个PIPE对应的中断 */
        if (hal_vicap_get_int_report(pipe_id, cap_int_status) != TD_TRUE) {
            continue;
        }

        viu_comm_cap_all_mode_irq_route(pipe_id, TD_TRUE, irq_begin_time);
    }

#if !defined(CONFIG_CHIP_PLATFORM_WEAR)
    mcp_osal_workqueue_schedule(&g_vicap_bottom_half_work);
#endif
    return MCP_OSAL_IRQ_HANDLED;
}

/* 以下添加VICAP 相关代码 */
static td_s32 viu_cap_comm_per_dev_init(td_u32 cap_id)
{
    hal_capability_vicap *cap_vicap = hal_cap_get_vicap();

    if (cap_id >= ISP_VICAP_IP_NUM) {
        return TD_FAILURE;
    }

    viu_info_trace("------------create isp workqueue--------------\n");

    /* 映射寄存器 */
    if (hal_vicap_init(cap_id) != TD_SUCCESS) {
        viu_alert_trace("viu_cap%d ioremap reg fail !\n", cap_id);
        return TD_FAILURE;
    }

    /* 注册vicap中断处理函数 */
    if (mcp_osal_irq_request(cap_vicap->irq_num, viu_comm_cap_irq_route, NULL,
        cap_vicap->irq_name, (void *)&cap_vicap->irq_num) != 0) {
        hal_vicap_deinit(cap_id);
        viu_alert_trace("vi %s register IRQ(%u) fail!\n", cap_vicap->irq_name, cap_vicap->irq_num);
        return TD_FAILURE;
    }
    viu_info_trace("------------create isp workqueue2--------------\n");

    mcp_osal_irq_enable(cap_vicap->irq_num);

#if !defined(CONFIG_CHIP_PLATFORM_WEAR)
    mcp_osal_workqueue_init(&g_vicap_bottom_half_work, viu_cap_bottom_half_work_process);
#endif

    return TD_SUCCESS;
}

static td_void viu_cap_comm_per_dev_exit(td_u32 cap_id)
{
    hal_capability_vicap *cap_vicap = hal_cap_get_vicap();

    mcp_osal_irq_disable(cap_vicap->irq_num);

    mcp_osal_irq_free(cap_vicap->irq_num, (void *)&cap_vicap->irq_num);
#if !defined(CONFIG_CHIP_PLATFORM_WEAR)
    mcp_osal_workqueue_destroy(&g_vicap_bottom_half_work);
#endif
    hal_vicap_deinit(cap_id);
}

td_s32 viu_cap_comm_dev_init(td_void)
{
    td_s32 i;
    td_u32 cap_id;

    /* 上电 */
#ifdef CONFIG_CHIP_3322
    uapi_pmu_hs_sub_control(HS_SUB_VICAP_ID, PMU_POWER_ON_AND_RUN);
#endif

    /* 初始化寄存器 */
    for (cap_id = 0; cap_id < ISP_VICAP_IP_NUM; cap_id++) {
        if (viu_cap_comm_per_dev_init(cap_id) != TD_SUCCESS) {
            viu_alert_trace("viu_cap%d init fail !\n", cap_id);

            for (i = (td_s32)cap_id - 1; i >= 0; i--) {
                viu_cap_comm_per_dev_exit(i);
            }

            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

td_void viu_cap_comm_dev_exit(td_void)
{
    td_u32 i;

#ifdef CONFIG_CHIP_3322
    uapi_pmu_hs_sub_control(HS_SUB_VICAP_ID, PMU_POWER_OFF);
#endif
    for (i = 0; i < ISP_VICAP_IP_NUM; i++) {
        viu_cap_comm_per_dev_exit(i);
    }
}

viu_proc_statistic* viu_proc_get_statistic(td_void)
{
    viu_comm_ctx *comm_ctx = viu_get_comm_ctx();
    return &comm_ctx->proc_statistic;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

