/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_pipe.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_pipe.h"
#include "viu_drv.h"
#include "viu_pic_queue.h"
#include "viu_public.h"
#include "viu_extern.h"
#include "drv_vicap.h"
#include "hal_vicap_intf.h"
#include "hal_capability_intf.h"
#include "hal_viproc_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 viu_check_vir_pipe_id(td_s32 pipe_id)
{
    if ((pipe_id >= (ISP_MAX_PHY_PIPE_NUM + ISP_MAX_VIR_PIPE_NUM)) || (pipe_id < ISP_MAX_PHY_PIPE_NUM)) {
        viu_err_trace("vi phy pipe ID[%d] is invalid\r\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PIPEID;
    }

    return TD_SUCCESS;
}

td_s32 viu_check_phy_pipe_id(td_s32 pipe_id)
{
    if ((pipe_id >= ISP_MAX_PHY_PIPE_NUM) || (pipe_id < 0)) {
        viu_err_trace("vi vir pipe ID[%d] is invalid\r\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PIPEID;
    }

    return TD_SUCCESS;
}

td_s32 viu_check_pipe_id(td_s32 pipe_id)
{
    if ((pipe_id >= ISP_MAX_PIPE_NUM) || (pipe_id < 0)) {
        viu_err_trace("vi pipe ID[%d] is invalid\r\n", pipe_id);
        return SOC_ERR_VIU_INVALID_PIPEID;
    }

    return TD_SUCCESS;
}

td_bool viu_pipe_is_online_mode(td_s32 pipe_id)
{
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode == VIU_ONLINE_PPU_OFFLINE) ||
        (pipe_mode == VIU_ONLINE_PPU_ONLINE)) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_bool viu_pipe_is_ppu_online(td_s32 pipe_id)
{
    ext_viu_operating_mode_type pipe_mode;

    pipe_mode = viu_drv_get_pipe_mode(pipe_id);
    if ((pipe_mode == VIU_ONLINE_PPU_ONLINE) ||
        (pipe_mode == VIU_OFFLINE_PPU_ONLINE)) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

static td_s32 viu_wait_stop_working_call_back(const td_void *param)
{
    td_s32 pipe_id;

    pipe_id = *(const td_s32 *)param;

    if ((!is_pipe_working(pipe_id)) &&
        (!is_pipe_cap_working(pipe_id)) &&
        (!is_usr_pic_timer_working(pipe_id)) &&
        (!is_other_pipe_visiting(pipe_id)) &&
        (other_mod_job_cnt_get(pipe_id) == 0)) {
        return 1;
    } else {
        return 0;
    }
}

td_s32 viu_pipe_set_work_mode(td_u8 pipe_id, td_bool pipe_online, td_bool chn_online)
{
    viu_drv_set_work_mode(pipe_id, pipe_online, chn_online);
    return TD_SUCCESS;
}

td_s32 viu_pipe_get_work_mode(td_u8 pipe_id, td_bool *pipe_online, td_bool *chn_online)
{
    viu_drv_get_work_mode(pipe_id, pipe_online, chn_online);
    return TD_SUCCESS;
}

static td_s32 viu_pipe_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode, td_void *unique)
{
    td_s32 ret;
    ext_viu_operating_mode_type mode;
    ext_isp_rect rect = {0};
    ext_isp_size size;

    ret = viu_pipe_ctx_init(pipe_id, pipe_attr, bind_viu_dev, wdr_mode, unique);
    if (ret != TD_SUCCESS) {
        goto pipe_exist_error;
    }

    mode = viu_drv_get_pipe_mode(pipe_id);

    if (pipe_attr->crop_info.enable == TD_FALSE) {
        viu_drv_get_pipe_bind_dev_output_size(pipe_id, &size);
        rect.height = size.height;
        rect.width = size.width;
    } else {
        rect = pipe_attr->crop_info.rect;
    }

    hal_vicap_create(pipe_id, &rect);
    if (mode == VIU_ONLINE_PPU_OFFLINE || mode == VIU_ONLINE_PPU_ONLINE) {
        hal_viproc_prepare(0);
    }

    ret = viu_pipe_malloc_mem(pipe_id, pipe_attr, wdr_mode);
    if (ret != TD_SUCCESS) {
        goto mem_init_failed;
    }

    ret = viu_create_pipe_pic_queue(pipe_id, pipe_attr, wdr_mode);
    if (ret != TD_SUCCESS) {
        goto create_queue_failed;
    }

    viu_pipe_sync_ctx_init(pipe_id);

    return TD_SUCCESS;

create_queue_failed:
    viu_pipe_free_mem(pipe_id);
mem_init_failed:
    hal_vicap_destroy(pipe_id);
    if (mode == VIU_ONLINE_PPU_OFFLINE || mode == VIU_ONLINE_PPU_ONLINE) {
        hal_viproc_unprepare(0);
    }
    viu_pipe_ctx_exit(pipe_id);
pipe_exist_error:

    return ret;
}

td_s32 viu_pipe_create(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_void *unique)
{
    td_s32 ret;
    ext_isp_wdr_mode wdr_mode;
    td_s32 bind_viu_dev;
    td_u32 isp_sel = 0xF;

    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(pipe_attr);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_exist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_EXIST;
        viu_err_trace("pipe %d has been created !\n", pipe_id);
        goto pipe_exist_error;
    }

    if (is_phy_pipe(pipe_id)) {
        bind_viu_dev = viu_drv_get_pipe_bind_dev_id_before_init(pipe_id);
        wdr_mode = viu_drv_get_wdr_mode_from_dev(pipe_id);
    } else {
        bind_viu_dev = INVALID_DEV;
        wdr_mode = EXT_ISP_WDR_MODE_NONE;
    }

    if (is_phy_pipe(pipe_id) && (bind_viu_dev == INVALID_DEV)) {
        ret = SOC_ERR_VIU_NOT_PERM;
        viu_err_trace("pipe %d do not bind dev!\n", pipe_id);
        goto pipe_exist_error;
    }

    ret = viu_drv_check_pipe_attr(pipe_id, pipe_attr, bind_viu_dev, wdr_mode);
    if (ret != TD_SUCCESS) {
        goto pipe_exist_error;
    }

    ret = viu_pipe_init(pipe_id, pipe_attr, bind_viu_dev, wdr_mode, unique);
    if (ret != TD_SUCCESS) {
        goto pipe_exist_error;
    }

    isp_sel = bind_viu_dev;

    hal_vicap_set_isp_sel(pipe_id, isp_sel);
    viu_pipe_up_sem(pipe_id);

    return TD_SUCCESS;

pipe_exist_error:
    viu_pipe_up_sem(pipe_id);
    return ret;
}

static td_void viu_wakeup_pipe_wait_queue(td_s32 pipe_id)
{
    td_s32 i;

    /* 唤醒等待队列 */
    for (i = 0; i < VIU_MAX_CHN_NUM; i++) {
        mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_chn_frame[i]);
    }

    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame);
    /* 唤醒VGS回调队列的等待 */
    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_region_luma);
    /* wakeup用户发送帧进程，防止因为grp销毁而导致一直等待 */
    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame);

    mcp_osal_wait_wakeup(&g_ast_pipe_synch_ctx[pipe_id].wait_get_bnr_raw_frame);
}

static void viu_exit_destory_chn_pic_queue(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_s32 i;
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    /* 清空并销毁通道获取帧的队列，释放buffer */
    for (i = 0; i < VIU_MAX_CHN_NUM; i++) {
        if (pipe_mkp_ctx->chn_pic_queue[i].node_buf != TD_NULL) {
            (td_void) viu_clear_pic_queue(&pipe_mkp_ctx->chn_pic_queue[i]);
            viu_destroy_pic_queue(&pipe_mkp_ctx->chn_pic_queue[i]);
        }
    }
}

static td_void viu_pipe_exit(td_s32 pipe_id)
{
    td_s32 i;
    ext_viu_operating_mode_type mode;

    hal_vicap_set_isp_sel(pipe_id, INVALID_CMUX_SEL);

    viu_exit_destory_chn_pic_queue(pipe_id);

    mode = viu_drv_get_pipe_mode(pipe_id);
#if defined(CONFIG_CHIP_3322)
#else
    if (mode == VIU_ONLINE_PPU_OFFLINE) {
        viu_release_chn_buffer(pipe_id);
    }
#endif
    viu_release_pipe_capture_vb(pipe_id);
    viu_pipe_free_mem(pipe_id);
    viu_release_cur_pic(pipe_id);
    viu_destory_pipe_pic_queue(pipe_id);
#if defined(CONFIG_CHIP_3322)
    viu_ext_vb_destroy_pool(0);
#else
    (td_void)viu_drv_wdr_pool_de_init(pipe_id);
#endif

    hal_vicap_destroy(pipe_id);
    if (mode == VIU_ONLINE_PPU_OFFLINE || mode == VIU_ONLINE_PPU_ONLINE) {
        hal_viproc_unprepare(0);
    }

    viu_pipe_ctx_exit(pipe_id);
    viu_wakeup_pipe_wait_queue(pipe_id);

    viu_drv_set_pipe_bind_dev_id(pipe_id, INVALID_DEV);

    for (i = 0; i < VIU_MAX_CHN_NUM; i++) {
        viu_update_chn_last_time_ref(pipe_id, i, 0);
    }
}

td_s32 viu_pipe_destroy(td_s32 pipe_id)
{
    td_s32 timeout;
    td_s32 ret = TD_SUCCESS;
    td_ulong flags;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    viu_check_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        goto end;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->pipe_mkp_attr_setting.start == TD_TRUE) {
        ret = SOC_ERR_VIU_NOT_PERM;
        viu_err_trace("pipe %d must be stopped!\n", pipe_id);
        goto end;
    }

    if ((pipe_mkp_ctx->viu_snap_pipe_ctx.double_pipe == TD_TRUE) &&
        (pipe_mkp_ctx->viu_snap_status != VIU_SNAP_STATUS_DISABLE)) {
        ret = SOC_ERR_VIU_NOT_PERM;
        viu_err_trace("snap pipe %d must be disable first when destroy!\n", pipe_id);
        goto end;
    }

    viu_pipe_spin_lock(pipe_id, flags);
    g_ast_pipe_synch_ctx[pipe_id].destroying = TD_TRUE;
    viu_pipe_spin_unlock(pipe_id, flags);

    /* 等待中断处理结束后被唤醒 */
    timeout = mcp_osal_wait_timeout_interruptible(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working,
        viu_wait_stop_working_call_back, &pipe_id, 2000); // timeout 2000
    if (timeout <= 0) {
        viu_err_trace("wait stop working fial.\n");
    }

    viu_pipe_exit(pipe_id);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

td_s32 viu_pipe_set_attr(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr)
{
    td_s32 ret;
    td_s32 viu_dev_id;
    ext_isp_wdr_mode wdr_mode;
    td_ulong flags;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(pipe_attr);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        goto end;
    }

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    viu_dev_id = viu_drv_get_pipe_bind_dev_id(pipe_id);

    /* 检查pipe 属性 */
    ret = viu_drv_check_pipe_attr(pipe_id, pipe_attr, viu_dev_id, wdr_mode);
    if (ret != TD_SUCCESS) {
        ret = SOC_ERR_VIU_INVALID_PARA;
        goto end;
    }

    /* 检查静态属性 */
    ret = viu_drv_check_pipe_static_attr(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        ret = SOC_ERR_VIU_NOT_PERM;
        goto end;
    }
#if defined(CONFIG_CHIP_3322)
#else
    ret = viu_drv_malloc_image_process_buffer(pipe_id, pipe_attr);
    if (ret != TD_SUCCESS) {
        goto end;
    }
#endif
    viu_pipe_spin_lock(pipe_id, flags);

    viu_drv_set_pipe_attr(pipe_id, pipe_attr);

    viu_pipe_spin_unlock(pipe_id, flags);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

td_s32 viu_pipe_get_attr(td_s32 pipe_id, ext_viu_pipe_attr *pipe_attr)
{
    td_s32 ret = TD_SUCCESS;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(pipe_attr);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        goto end;
    }

    viu_drv_get_pipe_attr(pipe_id, pipe_attr);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

static td_void viu_update_pipe_capture_raw_status(td_s32 pipe_id)
{
    ext_viu_operating_mode_type mode;
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    mode = viu_drv_get_pipe_mode(pipe_id);
    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    if (wdr_mode == EXT_ISP_WDR_MODE_QUDRA) {
        pipe_mkp_ctx->raw_wait_stop_status = VIU_WAIT_STOP_END;
        pipe_mkp_ctx->cap_raw = TD_FALSE;
    } else {
        if ((mode == VIU_OFFLINE_PPU_OFFLINE) || (mode == VIU_OFFLINE_PPU_ONLINE)) {
            pipe_mkp_ctx->raw_wait_stop_status = VIU_WAIT_STOP_PREPARE;
            pipe_mkp_ctx->cap_raw = TD_TRUE;
        } else {
            pipe_mkp_ctx->raw_wait_stop_status = VIU_WAIT_STOP_PREPARE;
            pipe_mkp_ctx->cap_raw = TD_FALSE;
        }
    }
}

td_s32 viu_pipe_start(td_s32 pipe_id)
{
    td_s32 ret;
    td_ulong flags;
    viu_pipe_mkp_attr *pipe_mkp_attr_setting_ctx = TD_NULL;
    ext_isp_frame_interrupt_attr frame_int_attr = {0};
    td_bool online = TD_TRUE;

    viu_check_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d  unexisted!\n", pipe_id);
        ret = SOC_ERR_VIU_PIPE_UNEXIST;
        goto end;
    }

    pipe_mkp_attr_setting_ctx = viu_get_pipe_mkp_setting_attr_ctx(pipe_id);
    if (pipe_mkp_attr_setting_ctx->start == TD_TRUE) {
        ret = TD_SUCCESS;
        goto end;
    }

    ret = viu_drv_check_pipe_support(pipe_id);
    if (ret != TD_SUCCESS) {
        goto end;
    }

    viu_pipe_spin_lock(pipe_id, flags);

    pipe_mkp_attr_setting_ctx->start = TD_TRUE;
    pipe_mkp_attr_setting_ctx->pipe_start_update = TD_TRUE;

    viu_update_pipe_capture_raw_status(pipe_id);

    viu_drv_get_pipe_frame_interrupt_attr(pipe_id, &frame_int_attr);
    online = viu_pipe_is_online_mode(pipe_id) ;
    hal_vicap_start(pipe_id, frame_int_attr.int_type, !online);
    hal_viproc_start(0, online);

    viu_pipe_spin_unlock(pipe_id, flags);

    viu_comm_update_first_frame_pipe_attr(pipe_id);

end:
    viu_pipe_up_sem(pipe_id);

    return ret;
}

td_s32 viu_pipe_stop(td_s32 pipe_id)
{
    td_ulong flags;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_pipe_mkp_attr *pipe_mkp_attr_setting_ctx = TD_NULL;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d  unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    pipe_mkp_attr_setting_ctx = viu_get_pipe_mkp_setting_attr_ctx(pipe_id);
    if (pipe_mkp_attr_setting_ctx->start != TD_TRUE) {
        viu_pipe_up_sem(pipe_id);
        return TD_SUCCESS;
    }

    if (pipe_mkp_ctx->cap_raw == TD_TRUE) {
        pipe_mkp_ctx->cap_raw = TD_FALSE;
#ifdef CONFIG_CHIP_3322
        pipe_mkp_ctx->dst_mod = -1; // detach venc
#endif
        viu_wait_capture_frame_stop(pipe_id, &pipe_mkp_ctx->raw_wait_stop_status);

        viu_release_pipe_capture_vb(pipe_id);

        viu_pipe_spin_lock(pipe_id, flags);
        hal_vicap_disable(pipe_id);
        viu_pipe_spin_unlock(pipe_id, flags);
    }

    viu_pipe_spin_lock(pipe_id, flags);
    pipe_mkp_attr_setting_ctx->start = TD_FALSE;
    pipe_mkp_attr_setting_ctx->pipe_start_update = TD_TRUE;
    if (pipe_mkp_ctx->viu_snap_pipe_ctx.double_pipe == TD_FALSE) {
        pipe_mkp_ctx->viu_snap_status = VIU_SNAP_STATUS_DISABLE;
    }
    hal_vicap_stop(pipe_id);
    hal_viproc_stop(0, viu_pipe_is_online_mode(pipe_id));
    viu_pipe_spin_unlock(pipe_id, flags);

    viu_pipe_up_sem(pipe_id);

    return TD_SUCCESS;
}

td_void viu_release_pipe_capture_vb(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (pipe_mkp_ctx->this_frm.buf_ok == TD_TRUE) {
        viu_drv_put_vb(&pipe_mkp_ctx->this_frm.pic_info.video_frame, VB_UID_VIU);
        pipe_mkp_ctx->this_frm.buf_ok = TD_FALSE;
    }

    if (pipe_mkp_ctx->next_frm.buf_ok == TD_TRUE) {
        viu_drv_put_vb(&pipe_mkp_ctx->next_frm.pic_info.video_frame, VB_UID_VIU);
        pipe_mkp_ctx->next_frm.buf_ok = TD_FALSE;
    }
}

static td_s32 viu_get_comm_frame_int_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr)
{
    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    viu_drv_get_pipe_frame_interrupt_attr(pipe_id, frame_int_attr);

    return TD_SUCCESS;
}

/*
1.isp start前要获取 pipe 中断属性，决定同步方式
2.为防止isp在锁内调用，viu_isp_get_pipe_frame_interrupt_attr，所以另外定义无信号量的viu_isp_get_pipe_frame_interrupt_attr预防死锁
*/
td_s32 viu_pipe_get_isp_frame_int_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr)
{
    td_s32 ret = TD_SUCCESS;
    td_ulong flags;

    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(frame_int_attr);

    viu_pipe_spin_lock(pipe_id, flags);

    ret = viu_get_comm_frame_int_attr(pipe_id, frame_int_attr);

    viu_pipe_spin_unlock(pipe_id, flags);

    return ret;
}

td_s32 viu_pipe_set_bind(td_u8 pipe_id, td_s32 dst_mod, td_s32 dst_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    hal_capability_vicap *vicap = hal_cap_get_vicap();

    viu_check_phy_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    if (vicap->instance[0].support_bind_mod == dst_mod) {
        pipe_mkp_ctx->dst_mod = dst_mod;
        pipe_mkp_ctx->dst_id = dst_id;
    } else {
        pipe_mkp_ctx->dst_mod = 0;
        pipe_mkp_ctx->dst_id = -1;
    }

    viu_pipe_up_sem(pipe_id);

    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

