/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_vicap.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_vicap.h"
#include "viu_pic_queue.h"
#include "viu_dev.h"
#include "viu_pipe.h"
#include "drv_viu_ioctl.h"
#include "drv_vicap.h"

#if defined(CONFIG_CHIP_3322)
#else
#include "viu_snap.h"
#include "viu_wdr.h"
#include "viu_extern.h"
#include "viu_extchn.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

viu_vb_source g_vb_source = VIU_VB_SOURCE_COMMON;

ext_viu_mod_param g_mod_param;

viu_pipe_synchronization_ctx g_ast_pipe_synch_ctx[ISP_MAX_PIPE_NUM];
viu_pipe_mkp_ctx *g_past_pipe_mkp_ctx[ISP_MAX_PIPE_NUM] = { TD_NULL };
viu_dev_synchronization_ctx g_ast_dev_synch_ctx[VIU_MAX_DEV_NUM];
viu_dev_mkp_ctx g_ast_dev_mkp_ctx[VIU_MAX_DEV_NUM];

viu_time_ref_info g_ast_pipe_time_ref_ctx[ISP_MAX_PIPE_NUM];
mcp_osal_spinlock g_viu_module_lock;
mcp_osal_spinlock g_viu_comm_lock;

viu_pipe_synchronization_ctx* viu_get_pipe_synch_ctx(td_s32 pipe_id)
{
    return &g_ast_pipe_synch_ctx[pipe_id];
}

td_u64 viu_get_sys_time_by_usec(td_void)
{
    mcp_osal_timeval time;
    mcp_osal_gettimeofday(&time);
    return (td_u64)((time.tv_sec * 1000000LLU) + time.tv_usec);
}

td_s32 viu_pipe_down_sem(td_s32 pipe_id)
{
    if (mcp_osal_sem_down_interruptible(&g_ast_pipe_synch_ctx[pipe_id].pipe_sem) != TD_SUCCESS) {
        return -ERESTARTSYS;
    }
    return TD_SUCCESS;
}

td_s32 viu_check_chn_id_for_ioctl(td_s32 chn_id)
{
    if ((chn_id >= (VIU_MAX_CHN_NUM + 1)) || (chn_id < 0)) {
        viu_err_trace("vi chn ID[%d] is invalid\r\n", chn_id);
        return SOC_ERR_VIU_INVALID_CHNID;
    }

    return TD_SUCCESS;
}

td_bool viu_is_wait_cap_fram_stop(viu_pipe_mkp_ctx *pipe_mkp_ctx, viu_wait_stop *stop_status)
{
    if (*stop_status == VIU_WAIT_STOP_START) {
        *stop_status = VIU_WAIT_STOP_ING;
        return TD_TRUE;
    } else if (*stop_status == VIU_WAIT_STOP_ING) {
        td_u32 venc_queue_busy_num = viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue);
        if (venc_queue_busy_num > 0) {
            *stop_status = VIU_WAIT_VENC_RETURN_FRAME;
        } else {
            *stop_status = VIU_WAIT_STOP_END;
        }
        return TD_TRUE;
    } else if (*stop_status == VIU_WAIT_STOP_END) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

/* 线性离线模式判定 */
td_bool viu_linear_offline(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode)
{
    ext_viu_operating_mode_type mode;

    mode = viu_drv_get_pipe_mode(pipe_id);
    if ((viu_drv_is_wdr_mode(wdr_mode) == TD_FALSE) &&
        ((mode == VIU_OFFLINE_PPU_OFFLINE) || (mode == VIU_OFFLINE_PPU_ONLINE))) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

/* WDR离线模式且viu_pipe是 mast_pipe 判定, */
td_bool viu_wdr_offline(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode)
{
    viu_unused(pipe_id);
    viu_unused(wdr_mode);
    return TD_FALSE;
}

td_bool viu_lost_repeat_frame(td_s32 pipe_id)
{
    td_bool lost = TD_FALSE;
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (viu_wdr_offline(pipe_id, wdr_mode) == TD_TRUE) {
        if (pipe_mkp_ctx->cur_wdr_pic != TD_NULL) {
            lost = pipe_mkp_ctx->cur_wdr_pic->pic_info[0].lost_repeat_frame;
        }
    } else if (viu_linear_offline(pipe_id, wdr_mode) == TD_TRUE) {
        if (pipe_mkp_ctx->cur_line_pic != TD_NULL) {
            lost = pipe_mkp_ctx->cur_line_pic->cap_pic_info.lost_repeat_frame;
        }
    } else {
        lost = TD_FALSE;
    }

    return lost;
}

td_void viu_release_cur_pic(td_s32 pipe_id)
{
    ext_isp_wdr_mode wdr_mode;

    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    /* 线性模式，离线pipe */
    if (viu_linear_offline(pipe_id, wdr_mode) == TD_TRUE) {
        if (pipe_mkp_ctx->cur_line_pic != TD_NULL) {
            viu_drv_put_vb(&pipe_mkp_ctx->cur_line_pic->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
#if defined(CONFIG_CHIP_3322)
#else
            viu_drv_put_free_be_buf(pipe_id, &pipe_mkp_ctx->cur_line_pic->be_cfg_buf);
#endif
            viu_pic_queue_put_free(&pipe_mkp_ctx->line_input_pic_queue, pipe_mkp_ctx->cur_line_pic);
            pipe_mkp_ctx->cur_line_pic = TD_NULL;
        }
    } else if (viu_wdr_offline(pipe_id, wdr_mode) == TD_TRUE) { /* 离线WDR */
#if defined(CONFIG_CHIP_3322)
#else
        if (pipe_mkp_ctx->cur_wdr_pic != TD_NULL) {
            viu_wdr_offline_clear_pic_node(pipe_id, pipe_mkp_ctx->cur_wdr_pic);
            viu_pic_queue_put_free(&pipe_mkp_ctx->wdr_input_pic_queue, pipe_mkp_ctx->cur_wdr_pic);
            pipe_mkp_ctx->cur_wdr_pic = TD_NULL;
        }
#endif
    }

    pipe_mkp_ctx->cur_pic_info = TD_NULL;
}

td_void viu_pipe_frame_rate_statistic(td_s32 pipe_id, td_bool frame_ok)
{
    td_u64 cur_time;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_wch_dbg_info *wch_info = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    wch_info = &pipe_mkp_ctx->wch_dbg;

    if (frame_ok == TD_TRUE) {
        wch_info->cur_pipe_frm_cnt++;
    }

    cur_time = mcp_osal_div64_u64(osal_sched_clock(), 1000U);
    if (viu_time_diff(cur_time, wch_info->pipe_begin_tm) >= 1000000) { /* 1000000 us */
        wch_info->pipe_frm_rate = (td_u32)(wch_info->cur_pipe_frm_cnt - wch_info->last_sec_pipe_frm_cnt);

        wch_info->last_sec_pipe_frm_cnt = wch_info->cur_pipe_frm_cnt;

        if ((viu_time_diff(cur_time, wch_info->pipe_begin_tm) >= 2000000ULL)) { /* 2000000 us */
            wch_info->pipe_begin_tm = cur_time;
        } else {
            wch_info->pipe_begin_tm += 1000000ULL; /* 1000000 us */
        }
    }
}

#ifdef SOCT_VIU_CHN_LOW_DELAY_SUPPORT
static td_bool viu_is_low_delay_frame(viu_pic_info *pic_info)
{
    if (pic_info->frame_info == FRAME_LOWDELAY) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}
#endif

static td_void viu_update_pipe_start_from_user_set(td_s32 pipe_id)
{
    viu_pipe_mkp_attr *pipe_mkp_attr_setting_ctx = TD_NULL;
    viu_pipe_mkp_attr *pipe_mkp_attr_working_ctx = TD_NULL;

    pipe_mkp_attr_setting_ctx = viu_get_pipe_mkp_setting_attr_ctx(pipe_id);
    pipe_mkp_attr_working_ctx = viu_get_pipe_mkp_working_attr_ctx(pipe_id);

    if (pipe_mkp_attr_setting_ctx->pipe_start_update == TD_TRUE) {
        pipe_mkp_attr_working_ctx->start = pipe_mkp_attr_setting_ctx->start;
        pipe_mkp_attr_setting_ctx->pipe_start_update = TD_FALSE;
    }
}

static td_void viu_update_chn_enable_from_user_set(td_s32 pipe_id, td_s32 chn_id)
{
    viu_pipe_mkp_attr *pipe_mkp_attr_setting_ctx = TD_NULL;
    viu_pipe_mkp_attr *pipe_mkp_attr_working_ctx = TD_NULL;

    pipe_mkp_attr_setting_ctx = viu_get_pipe_mkp_setting_attr_ctx(pipe_id);
    pipe_mkp_attr_working_ctx = viu_get_pipe_mkp_working_attr_ctx(pipe_id);

    if (pipe_mkp_attr_setting_ctx->chn_enable_update[chn_id] == TD_TRUE) {
        pipe_mkp_attr_working_ctx->chn_en[chn_id] = pipe_mkp_attr_setting_ctx->chn_en[chn_id];
        pipe_mkp_attr_setting_ctx->chn_enable_update[chn_id] = TD_FALSE;
    }
}

static td_void viu_update_chn_align_from_user_set(td_s32 pipe_id, td_s32 chn_id)
{
    viu_unused(pipe_id);
    viu_unused(chn_id);
    // do nothing
}

td_void viu_update_working_pipe_mkp_attr(td_s32 pipe_id)
{
    viu_update_pipe_start_from_user_set(pipe_id);
#if defined(CONFIG_CHIP_3322)
#else
    viu_snap_update_pipe_attr(pipe_id);
#endif
    viu_drv_update_working_pipe_drv_attr(pipe_id);
}

td_void viu_update_working_chn_mkp_attr(td_s32 pipe_id, td_s32 chn_id)
{
    if ((chn_id >= 0) && (chn_id < VIU_MAX_CHN_NUM)) {
        viu_update_chn_enable_from_user_set(pipe_id, chn_id);
        viu_update_chn_align_from_user_set(pipe_id, chn_id);

        if (chn_id < VIU_MAX_PHY_CHN_NUM) {
        }
    }
#if defined(CONFIG_CHIP_3322)
#else
    viu_drv_update_working_chn_drv_attr(pipe_id, chn_id);
#endif
}

td_void viu_update_working_pipe_chn_mkp_attr(td_s32 pipe_id)
{
    td_s32 chn_id;

    viu_update_working_pipe_mkp_attr(pipe_id);

    for (chn_id = 0; chn_id < VIU_MAX_CHN_NUM; chn_id++) {
        viu_update_working_chn_mkp_attr(pipe_id, chn_id);
    }
}

static td_void viu_mkp_destory_input_queue(td_s32 pipe_id)
{
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if ((viu_linear_offline(pipe_id, wdr_mode) == TD_TRUE) &&
        (pipe_mkp_ctx->line_input_pic_queue.node_buf != TD_NULL)) {
        viu_destroy_pic_queue(&pipe_mkp_ctx->line_input_pic_queue);
    }

    if ((viu_wdr_offline(pipe_id, wdr_mode) == TD_TRUE) &&
        (pipe_mkp_ctx->wdr_input_pic_queue.node_buf != TD_NULL)) {
        viu_destroy_pic_queue(&pipe_mkp_ctx->wdr_input_pic_queue);
    }
}

td_void viu_clear_linear_pic_queue(td_s32 pipe_id, viu_pic_queue *node_queue)
{
    viu_unused(pipe_id);
    viu_cap_pic_node *temp_pic = TD_NULL;

    if (node_queue == NULL || node_queue->node_buf == NULL) {
        return;
    }

    while (viu_pic_queue_get_busy_num(node_queue) > 0) {
        temp_pic = viu_pic_queue_get_busy(node_queue, viu_cap_pic_node);
        if (temp_pic != TD_NULL) {
            viu_drv_put_vb(&temp_pic->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
#if defined(CONFIG_CHIP_3322)
#else
            viu_drv_put_free_be_buf(pipe_id, &temp_pic->be_cfg_buf);
#endif
            viu_pic_queue_put_free(node_queue, temp_pic);
        }
    }

    return;
}

static td_s32 viu_clear_wdr_pic_queue(td_s32 pipe_id, viu_pic_queue *node_queue)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
    viu_unused(node_queue);
    return TD_SUCCESS;
#else
    viu_wdr_pic_node *temp_pic = TD_NULL;

    viu_check_null_ptr_return(node_queue);
    viu_check_null_ptr_return(node_queue->node_buf);

    while (viu_pic_queue_get_busy_num(node_queue) > 0) {
        temp_pic = viu_pic_queue_get_busy(node_queue, viu_wdr_pic_node);
        if (temp_pic != TD_NULL) {
            viu_wdr_offline_clear_pic_node(pipe_id, temp_pic);

            viu_pic_queue_put_free(node_queue, temp_pic);
        }
    }

    return TD_SUCCESS;
#endif
}

td_void viu_clear_input_queue(td_s32 pipe_id)
{
    ext_isp_wdr_mode wdr_mode;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    wdr_mode = viu_drv_get_wdr_mode_by_pipe(pipe_id);
    if (viu_linear_offline(pipe_id, wdr_mode) == TD_TRUE) {
        /* 清空线性输入图像队列 */
        viu_clear_linear_pic_queue(pipe_id, &pipe_mkp_ctx->line_input_pic_queue);
    } else if (viu_wdr_offline(pipe_id, wdr_mode) == TD_TRUE) {
        /* 清空wdr输入图像队列 */
        (td_void)viu_clear_wdr_pic_queue(pipe_id, &pipe_mkp_ctx->wdr_input_pic_queue);
    } else {
    }
}

td_s32 viu_mkp_create_input_queue(td_s32 pipe_id, ext_isp_wdr_mode wdr_mode)
{
    td_s32 ret = TD_SUCCESS;

    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    if (viu_linear_offline(pipe_id, wdr_mode) == TD_TRUE) {
        /* 创建线性输入图像队列 */
        ret = viu_create_pic_queue(&pipe_mkp_ctx->line_input_pic_queue, VIU_MAX_PIC_QUEUE_LENGTH, viu_cap_pic_node);
        if (ret != TD_SUCCESS) {
            viu_err_trace("pipe %d create line picture queue fail!\n", pipe_id);
            ret = SOC_ERR_VIU_NOMEM;
        }
    } else if (viu_wdr_offline(pipe_id, wdr_mode) == TD_TRUE) { /* 创建wdr输入图像队列 */
        ret = viu_create_pic_queue(&pipe_mkp_ctx->wdr_input_pic_queue, VIU_MAX_PIC_QUEUE_LENGTH, viu_wdr_pic_node);
        if (ret != TD_SUCCESS) {
            viu_err_trace("pipe %d create wdr picture queue fail!\n", pipe_id);
            ret = SOC_ERR_VIU_NOMEM;
        }
    }

    return ret;
}

td_void viu_init_err_int_detect(viu_err_int_detect *err_int_detect)
{
    ext_viu_mod_param *mod_param = TD_NULL;

    mod_param = viu_get_mod_param_ctx();
    if (mod_param->detect_err_frame < 0) {
        err_int_detect->detect_err_int = TD_FALSE;
        err_int_detect->cur_err_int = TD_FALSE;
        err_int_detect->need_lost_frame = TD_FALSE;
        err_int_detect->err_int_cnt_to_drop_frame = 0;
    } else if (mod_param->detect_err_frame == 0) {
        err_int_detect->detect_err_int = TD_TRUE;
        err_int_detect->cur_err_int = TD_FALSE;
        err_int_detect->need_lost_frame = TD_FALSE;
        err_int_detect->err_int_cnt_to_drop_frame = 1;
    } else {
        err_int_detect->detect_err_int = TD_TRUE;
        err_int_detect->cur_err_int = TD_FALSE;
        err_int_detect->need_lost_frame = TD_FALSE;
        err_int_detect->err_int_cnt_to_drop_frame = mod_param->detect_err_frame;
    }

    if (mod_param->drop_err_frame <= 1) {
        err_int_detect->drop_err_frm_cnt = 1;
    } else {
        err_int_detect->drop_err_frm_cnt = mod_param->drop_err_frame;
    }
}

static td_void viu_init_attached_vb_pool(td_s32 pipe_id)
{
    td_s32 i;
    viu_pipe_mkp_ctx *viu_pipe_ctx = TD_NULL;

    viu_pipe_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    viu_pipe_ctx->pipe_vb_pool = VB_INVALID_POOLID;

    for (i = 0; i < VIU_MAX_CHN_NUM; i++) {
        viu_pipe_ctx->a_chn_vb_pool[i] = VB_INVALID_POOLID;
    }
}

td_s32 viu_pipe_mkp_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, td_void *unique)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_ulong flags;

    pipe_mkp_ctx = mcp_osal_vmalloc(sizeof(viu_pipe_mkp_ctx));
    if (pipe_mkp_ctx == TD_NULL) {
        viu_err_trace("pipe %d malloc MKP context fail!\n", pipe_id);
        return SOC_ERR_VIU_NOMEM;
    }

    (td_void)memset_s(pipe_mkp_ctx, sizeof(viu_pipe_mkp_ctx), 0, sizeof(viu_pipe_mkp_ctx));

    pipe_mkp_ctx->pipe_bypass_mode = pipe_attr->pipe_bypass_mode;
    pipe_mkp_ctx->unique = unique;
    pipe_mkp_ctx->dst_mod = -1;
    pipe_mkp_ctx->dst_id = -1;

    viu_pipe_spin_lock(pipe_id, flags);
    viu_set_pipe_mkp_ctx(pipe_id, pipe_mkp_ctx);

    viu_init_err_int_detect(&pipe_mkp_ctx->wch_dbg.err_int_detect);
    viu_init_err_int_detect(&pipe_mkp_ctx->pipe_statistic.err_int_detect);

    viu_init_attached_vb_pool(pipe_id);

    viu_pipe_spin_unlock(pipe_id, flags);

    return TD_SUCCESS;
}

td_void viu_pipe_mkp_ctx_exit(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_ulong flags;

    viu_pipe_spin_lock(pipe_id, flags);
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    viu_set_pipe_mkp_ctx(pipe_id, TD_NULL);
    viu_pipe_spin_unlock(pipe_id, flags);

    if (pipe_mkp_ctx != TD_NULL) {
        mcp_osal_vfree(pipe_mkp_ctx);
    }
}

td_s32 viu_pipe_ctx_init(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr,
    td_s32 bind_viu_dev, ext_isp_wdr_mode wdr_mode, td_void *unique)
{
    td_s32 ret;

    ret = viu_pipe_mkp_ctx_init(pipe_id, pipe_attr, unique);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d MKP context init fail.\n", pipe_id);
        goto mkp_ctx_init_failed;
    }

    ret = viu_drv_pipe_drv_ctx_init(pipe_id, pipe_attr, bind_viu_dev, wdr_mode);
    if (ret != TD_SUCCESS) {
        viu_err_trace("pipe %d DRV context init fail.\n", pipe_id);
        goto drv_ctx_init_failed;
    }

    return TD_SUCCESS;

drv_ctx_init_failed:
    viu_pipe_mkp_ctx_exit(pipe_id);
mkp_ctx_init_failed:
    return ret;
}

td_void viu_pipe_ctx_exit(td_s32 pipe_id)
{
    viu_drv_pipe_drv_ctx_exit(pipe_id);
    viu_pipe_mkp_ctx_exit(pipe_id);
}

#if defined(CONFIG_CHIP_3322)
td_s32 viu_pipe_malloc_mem(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, ext_isp_wdr_mode wdr_mode)
{
    viu_unused(wdr_mode);
    td_s32 ret;
    ext_vb_base_info vb_info;
    ext_isp_vb_cal_config vb_cal_config;
    td_char buf_name[16] = {0}; // 16 buf len
    td_u32 pool_id = 0xFFFFFFFF;

    ret = snprintf_s(buf_name, sizeof(buf_name), sizeof(buf_name) - 1, "vicap%d_buf", pipe_id);
    if (ret < 0) {
        viu_err_trace("snprintf_s buf_name fail!\n");
        return TD_FAILURE;
    }

    vb_info.compress_mode = pipe_attr->compress_mode;
    vb_info.pixel_format = pipe_attr->pix_fmt;
    vb_info.dynamic_range = EXT_DYNAMIC_RANGE_SDR8;
    vb_info.video_format = EXT_VIDEO_FORMAT_LINEAR;
    vb_info.width = pipe_attr->max_w;
    vb_info.height = pipe_attr->max_h;
    vb_info.is_3dnr_buffer = TD_FALSE;
    vb_info.align = 16; // align 16
    vb_info.buf_line = 0;

    viu_ext_vb_get_vb_cfg(&vb_info, &vb_cal_config);

    ret = viu_ext_vb_create_pool(&pool_id, VICAP_BUF_NUM, vb_cal_config.vb_size, buf_name, VB_REMAP_MODE_NOCACHE);
    return ret;
}
#else
td_s32 viu_pipe_malloc_mem(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, ext_isp_wdr_mode wdr_mode)
{
    td_s32 ret;
    td_bool need_malloc = TD_TRUE;  /* linear or wdr mode need malloc buf. */
    td_s32 mast_pipe;

    if (pipe_attr->pipe_bypass_mode == EXT_VIU_PIPE_BYPASS_BE) {
        return TD_SUCCESS;
    }

    /* 此处WDR需要解耦，分配内存和寄存器配置分开 */
    ret = viu_mkp_pipe_wdr_init(pipe_id);
    if (ret != TD_SUCCESS) {
        goto wdr_init_failed;
    }

    if (viu_drv_is_wdr_mode(wdr_mode) == TD_TRUE) {
        mast_pipe = viu_drv_get_mast_pipe_by_pipe(pipe_id);
        if (mast_pipe != pipe_id) {
            need_malloc = TD_FALSE; /* only mast pipe need malloc buf when wdr mode. */
        }
    }

    if (need_malloc == TD_TRUE) {
        ret = viu_model_create_one_model(pipe_id);
        if (ret != TD_SUCCESS) {
            goto model_buffer_malloc_failed;
        }

        ret = viu_drv_malloc_image_process_buffer(pipe_id, pipe_attr);
        if (ret != TD_SUCCESS) {
            goto image_process_buffer_malloc_failed;
        }
    }

    return TD_SUCCESS;

image_process_buffer_malloc_failed:
    viu_model_destroy_one_model(pipe_id);
model_buffer_malloc_failed:
    viu_mkp_pipe_wdr_de_init(pipe_id);
    (td_void)viu_drv_wdr_pool_de_init(pipe_id);
wdr_init_failed:
    return ret;
}
#endif

td_void viu_pipe_free_mem(td_s32 pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_id);
#else
    viu_drv_free_image_process_buffer(pipe_id);

    viu_model_destroy_one_model(pipe_id);

    viu_mkp_pipe_wdr_de_init(pipe_id);

    viu_drv_relase_fpn_frame_addr(pipe_id);
#endif
}

td_s32 viu_create_pipe_pic_queue(td_s32 pipe_id, const ext_viu_pipe_attr *pipe_attr, ext_isp_wdr_mode wdr_mode)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    td_s32 ret;

    viu_unused(pipe_attr);
    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    ret = viu_mkp_create_input_queue(pipe_id, wdr_mode);
    if (ret != TD_SUCCESS) {
        ret = SOC_ERR_VIU_NOMEM;
        viu_err_trace("pipe %d create input picture queue fail!\n", pipe_id);
        goto input_queue_create_failed;
    }

    ret = viu_create_pic_queue(&pipe_mkp_ctx->dump_pipe_raw_pic_queue, VIU_MAX_PIC_QUEUE_LENGTH, viu_cap_pic_node);
    if (ret != TD_SUCCESS) {
        ret = SOC_ERR_VIU_NOMEM;
        viu_err_trace("pipe %d create dump raw picture queue fail!\n", pipe_id);
        goto dump_queue_create_failed;
    }

    return TD_SUCCESS;

dump_queue_create_failed:
    viu_mkp_destory_input_queue(pipe_id);
input_queue_create_failed:
    return ret;
}

td_void viu_destory_pipe_pic_queue(td_s32 pipe_id)
{
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    /* 清空并销毁输入帧的队列，释放buffer */
    viu_clear_input_queue(pipe_id);

    /* 清空并销毁raw data队列，释放buffer */
    viu_clear_linear_pic_queue(pipe_id, &pipe_mkp_ctx->dump_pipe_raw_pic_queue);

    viu_mkp_destory_input_queue(pipe_id);

    if (pipe_mkp_ctx->dump_pipe_raw_pic_queue.node_buf != TD_NULL) {
        viu_destroy_pic_queue(&pipe_mkp_ctx->dump_pipe_raw_pic_queue);
    }
}

td_void viu_pipe_sync_ctx_init(td_s32 pipe_id)
{
    td_ulong flags;

    viu_pipe_spin_lock(pipe_id, flags);
    mcp_osal_atomic_set(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt, 0);
    mcp_osal_atomic_set(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt, 0);
    g_ast_pipe_synch_ctx[pipe_id].destroying = TD_FALSE;
    g_ast_pipe_synch_ctx[pipe_id].resetting = TD_FALSE;
    g_ast_pipe_synch_ctx[pipe_id].working = TD_FALSE;
    g_ast_pipe_synch_ctx[pipe_id].cap_working = TD_FALSE;
    g_ast_pipe_synch_ctx[pipe_id].no_int_timer_working = TD_FALSE;
    g_ast_pipe_synch_ctx[pipe_id].other_pipe_visiting = TD_FALSE;
    add_pipe_unique_id(pipe_id);
    viu_pipe_spin_unlock(pipe_id, flags);
}

/*****************************************************************************
  1.Date         : 2016/11/22
    Create       : 清空一个队列
    Motify       : 此处策略待商定
                   此处temp_pic返回NULL,说明busy队列为空，
                   但viu_pic_queue_get_busy_num(node_queue)>0，说明队列不为空
                   这种情况不应该出现，处理策略有两种:
                       1.直接退出
                       2.修改busynum,置值为0；如果置值为0，个人倾向把这种情况处理，放到队列操作里。
*****************************************************************************/
td_s32 viu_clear_pic_queue(viu_pic_queue *node_queue)
{
    viu_pic_node *temp_pic = TD_NULL;

    viu_check_null_ptr_return(node_queue);
    viu_check_null_ptr_return(node_queue->node_buf);

    while (viu_pic_queue_get_busy_num(node_queue) > 0) {
        temp_pic = viu_pic_queue_get_busy(node_queue, viu_pic_node);
        if (temp_pic != TD_NULL) {
            viu_drv_put_vb(&temp_pic->video_frame, VB_UID_VIU);
            viu_pic_queue_put_free(node_queue, temp_pic);
        }
    }

    return TD_SUCCESS;
}

static td_s32 viu_wait_normal_wakeup(td_s32 pipe_id, td_s32 chn_id, viu_waitqueue_wakeup wake_condtion)
{
    td_s32 ret = TD_FAILURE;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    switch (wake_condtion) {
        case VIU_SENDFRAME_WAKEUP:
            if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue) < MAX_PIC_QUEUE_LENGTH_FOR_USER) {
                ret = TD_SUCCESS;
            }
            break;

        case VIU_GETCHNLUMA_WAKEUP:
            if (is_chn_luma_finish(pipe_id, chn_id) == TD_TRUE) {
                ret = TD_SUCCESS;
            }
            break;

        case VIU_GETFRAME_WAKEUP:
            if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->chn_pic_queue[chn_id]) != 0) {
                ret = TD_SUCCESS;
            }
            break;

        case VIU_GETRAW_WAKEUP:
            if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->dump_pipe_raw_pic_queue) != 0) {
                ret = TD_SUCCESS;
            }
            break;

        case VIU_SEND_WDR_FRAME_WAKEUP:
            if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->wdr_input_pic_queue) < MAX_PIC_QUEUE_LENGTH_FOR_USER) {
                ret = TD_SUCCESS;
            }
            break;

        default:
            viu_err_trace("Invalid wake condtion:%d\n", wake_condtion);
            break;
    }

    return ret;
}

static td_bool viu_wake_up_condition(td_s32 pipe_id, td_s32 chn_id, td_s32 unique_id, td_bool *wait_flag,
    viu_waitqueue_wakeup wake_condtion)
{
    td_ulong flags;
    td_bool ret = TD_FALSE;
    *wait_flag = TD_FALSE;

    viu_pipe_spin_lock(pipe_id, flags);

    if (viu_drv_get_status() != VIU_STATUS_STARTED) {
        viu_err_trace("sys is not init\n");
        ret = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    if (unique_id != get_pipe_unique_id(pipe_id)) {
        viu_err_trace("get chn luma failure\n");
        ret = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    if (is_pipe_destroying(pipe_id)) {
        viu_err_trace("chanel destroy \n");
        ret = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    if (is_pipe_resetting(pipe_id)) {
        viu_err_trace("chanel resetting\n");
        ret = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    if (viu_wait_normal_wakeup(pipe_id, chn_id, wake_condtion) == TD_SUCCESS) {
        viu_debug_trace("normal wakeup\n");
        *wait_flag = TD_TRUE;
        ret = TD_TRUE;
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    viu_pipe_spin_unlock(pipe_id, flags);

    return ret;
}

td_s32 viu_abnormal_verify_after_wait(td_s32 pipe_id, td_s32 unique_id)
{
    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    if (unique_id != get_pipe_unique_id(pipe_id)) {
        viu_err_trace("dev context has recreate\n");
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    if (is_pipe_destroying(pipe_id)) {
        viu_err_trace("dev context is destroying\n");
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    if (is_pipe_resetting(pipe_id)) {
        viu_err_trace("dev context is resetting\n");
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    return TD_SUCCESS;
}

td_s32 viu_wait_condition_call_back(const td_void *param)
{
    viu_wait_cond_info *viu_wait_conf_info = TD_NULL;
    td_s32 ret;
    td_bool condition;

    if (param == TD_NULL) {
        viu_err_trace("param is NULL !\n");
        return 0;
    }

    viu_wait_conf_info = (viu_wait_cond_info *)param;
    condition = viu_wake_up_condition(viu_wait_conf_info->viu_pipe_id, viu_wait_conf_info->viu_chn_id,
        viu_wait_conf_info->unique_id, &viu_wait_conf_info->wait_flag, viu_wait_conf_info->wake_condtion);
    if (condition == TD_TRUE) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

#ifdef SOCT_VIU_DEBUG_SUPPORT
td_s32 viu_set_image_quality_cfg(td_s32 pipe_id, viu_img_quality_cfg *image_quality_cfg)
{
    td_s32 ret;

    td_ulong flags;

    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(image_quality_cfg);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    ret = viu_drv_copy_image_quality_cfg(pipe_id, image_quality_cfg);
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    viu_pipe_spin_lock(pipe_id, flags);

    viu_drv_set_image_quality_cfg(pipe_id, image_quality_cfg);

    viu_pipe_spin_unlock(pipe_id, flags);

    viu_pipe_up_sem(pipe_id);

    return TD_SUCCESS;
}

td_s32 viu_get_image_quality_cfg(td_s32 pipe_id, viu_img_quality_cfg *image_quality_cfg)
{
    td_s32 ret = TD_SUCCESS;
    /* 参数检查 */
    viu_check_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(image_quality_cfg);
    viu_check_null_ptr_return(image_quality_cfg->regs);

    viu_pipe_down_interruptible_sem(pipe_id);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    ret = viu_drv_get_image_quality_cfg(pipe_id, image_quality_cfg);
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        viu_err_trace("pipe %d get cfg fail!\n", pipe_id);
        return ret;
    }

    viu_pipe_up_sem(pipe_id);

    return TD_SUCCESS;
}
#endif

td_s32 viu_check_comm_crop(ext_viu_crop_info *crop_info)
{
    if ((crop_info->enable != TD_TRUE) && (crop_info->enable != TD_FALSE)) {
        viu_err_trace("crop_en should be 0 or 1 which is %d!\n", crop_info->enable);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (crop_info->crop_coordinate > EXT_VIU_CROP_ABS_COOR) {
        viu_err_trace("crop coordinate  should be EXT_VIU_CROP_RATIO_COOR or EXT_VIU_CROP_ABS_COOR!\n");
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (crop_info->crop_coordinate == EXT_VIU_CROP_RATIO_COOR) {
        if (!VALUE_BETWEEN(crop_info->crop_rect.x, 0, 999)) { /* radio coordinate: x [0, 999] */
            viu_err_trace("crop start ratio_x should be 0 to 999 which is %d!\n", crop_info->crop_rect.x);
            return SOC_ERR_VIU_INVALID_PARA;
        }

        if (!VALUE_BETWEEN(crop_info->crop_rect.y, 0, 999)) { /* radio coordinate: y [0, 999] */
            viu_err_trace("crop start ratio_y should be 0 to 999 which is %d!\n", crop_info->crop_rect.y);
            return SOC_ERR_VIU_INVALID_PARA;
        }

        if (!VALUE_BETWEEN(crop_info->crop_rect.width, 1, 1000)) { /* radio coordinate: width [1, 1000] */
            viu_err_trace("crop start ratio width should be 0 to 1000 which is %d!\n",
                crop_info->crop_rect.width);
            return SOC_ERR_VIU_INVALID_PARA;
        }

        if (!VALUE_BETWEEN(crop_info->crop_rect.height, 1, 1000)) { /* radio coordinate: height [1, 1000] */
            viu_err_trace("crop start ratio height should be 0 to 1000 which is %d!\n",
                crop_info->crop_rect.height);
            return SOC_ERR_VIU_INVALID_PARA;
        }
    }

    return TD_SUCCESS;
}

td_void viu_wait_capture_frame_stop(td_s32 pipe_id, viu_wait_stop *stop_status)
{
    td_ulong flags;
    td_u64 time_begin;
    td_u64 time_val;

    viu_pipe_spin_lock(pipe_id, flags);
    *stop_status = VIU_WAIT_STOP_START;
    viu_pipe_spin_unlock(pipe_id, flags);

    time_begin = viu_get_sys_time_by_usec();

    while (*stop_status != VIU_WAIT_STOP_END) {
        time_val = viu_get_sys_time_by_usec() - time_begin;
        if (time_val >= 120000) { /* 120000 us */
            viu_err_trace("wait pipe %d capture stop timeout 120ms!!! state %x\n", pipe_id, *stop_status);
            break;
        }

        (td_void)mcp_osal_msleep(10); /* msleep 10 */
    }
}

td_void viu_release_chn_capture_vb(td_s32 pipe_id, td_s32 chn_id)
{
    viu_chn_buf_info *chn_frm = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
    chn_frm = &pipe_mkp_ctx->chn_this_frm[chn_id];
    if (chn_frm->buf_ok == TD_TRUE) {
        viu_drv_put_vb(&chn_frm->pic_info.video_frame, VB_UID_VIU);
        chn_frm->buf_ok = TD_FALSE;
    }

    chn_frm = &pipe_mkp_ctx->chn_next_frm[chn_id];
    if (chn_frm->buf_ok == TD_TRUE) {
        viu_drv_put_vb(&chn_frm->pic_info.video_frame, VB_UID_VIU);
        chn_frm->buf_ok = TD_FALSE;
    }
}

#if defined(CONFIG_CHIP_3322)
td_s32 viu_put_linear_frame_to_queue(td_s32 pipe_id, ext_isp_video_frame_info *video_frame,
    td_bool repeat_frame)
#else
td_s32 viu_put_linear_frame_to_queue(td_s32 pipe_id, ext_isp_video_frame_info *video_frame,
    isp_be_cfg *be_cfg, td_bool repeat_frame)
#endif
{
    td_s32 ret = TD_SUCCESS;
    viu_cap_pic_node *pic_node = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    vb_blk_handle h_blk_hdl;
    ext_video_supplement *supplement = TD_NULL;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    pic_node = viu_pic_queue_get_free(&pipe_mkp_ctx->line_input_pic_queue, viu_cap_pic_node);
    if (pic_node == NULL) {
        viu_err_trace("null pointer!\n");
        return SOC_ERR_VIU_INVALID_NULL_PTR;
    }

    (td_void)memcpy_s(&pic_node->cap_pic_info.pic_info.video_frame, sizeof(ext_isp_video_frame_info),
        video_frame, sizeof(*video_frame));
    pic_node->cap_pic_info.pic_info.start_time = mcp_osal_div64_u64(osal_sched_clock(), 1000U);
    h_blk_hdl = viu_ext_vb_phy_to_handle(pic_node->cap_pic_info.pic_info.video_frame.v_frame.phy_addr[0]);
    supplement = viu_ext_vb_handle_to_supplement(h_blk_hdl);
    if (supplement != NULL) {
        (td_void)memcpy_s(&pic_node->cap_pic_info.pic_info.video_frame.v_frame.supplement,
            sizeof(ext_video_supplement), supplement, sizeof(*supplement));
    }
    pic_node->cap_pic_info.buf_ok = TD_TRUE;
    pic_node->cap_pic_info.cap_ok = TD_TRUE;
    pic_node->cap_pic_info.lost_repeat_frame = repeat_frame;
    pic_node->cap_pic_info.send_ok = TD_TRUE;

    ret = viu_drv_get_vb(&pic_node->cap_pic_info.pic_info.video_frame, VB_UID_VIU);
    if (ret != TD_SUCCESS) {
        viu_pic_queue_put_free(&pipe_mkp_ctx->line_input_pic_queue, pic_node);
        viu_err_trace("VI pic pool_id:%d, phy_addr0:0x%llx, ret:0x%x.\n",
            pic_node->cap_pic_info.pic_info.video_frame.pool_id,
            pic_node->cap_pic_info.pic_info.video_frame.v_frame.phy_addr[0], ret);
        return SOC_ERR_VIU_INVALID_PARA;
    }

#if defined(CONFIG_CHIP_3322)
#else
    (td_void)memcpy_s(&pic_node->be_cfg_buf, sizeof(pic_node->be_cfg_buf),
        &be_cfg->be_cfg_buf, sizeof(isp_cfg_rec));
    (td_void)memcpy_s(&pic_node->be_sync_param, sizeof(isp_be_sync_para),
        &be_cfg->be_sync_param, sizeof(be_cfg->be_sync_param));
#endif
    viu_pic_queue_put_busy(&pipe_mkp_ctx->line_input_pic_queue, pic_node);

    return TD_SUCCESS;
}

static td_s32 wait_for_line_mode_pipe_frame_que(td_s32 pipe_id, td_s32 unique_id, td_s32 milli_sec)
{
    td_s32 ret = TD_SUCCESS;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;
    viu_waitqueue_wakeup wake_condtion = VIU_SENDFRAME_WAKEUP;
    viu_wait_cond_info viu_wait_conf_info;

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    viu_wait_conf_info.viu_pipe_id = pipe_id;
    viu_wait_conf_info.viu_chn_id = 0;
    viu_wait_conf_info.unique_id = unique_id;
    viu_wait_conf_info.wait_flag = TD_FALSE;
    viu_wait_conf_info.wake_condtion = wake_condtion;

    if (milli_sec > 0) {
        viu_pipe_up_sem(pipe_id);
        ret = mcp_osal_wait_timeout_interruptible(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame,
            viu_wait_condition_call_back, &viu_wait_conf_info, milli_sec);
        viu_pipe_down_interruptible_sem(pipe_id);
        if (ret < 0) {
            return -ERESTARTSYS;
        } else if (ret == 0) {  /* 等待的时间到期 */
            viu_err_trace("pipe %d user pic queue is full!!!!\n", pipe_id);
            return SOC_ERR_VIU_BUF_FULL;
        } else {
            if (viu_wait_conf_info.wait_flag == TD_FALSE) {
                return SOC_ERR_VIU_SYS_NOTREADY;
            }
        }
    } else if (milli_sec == 0) {
        if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue) >= MAX_PIC_QUEUE_LENGTH_FOR_USER) {
            viu_err_trace("pipe %d user pic queue is full!!!!milli_sec %d \n", pipe_id, milli_sec);
            return SOC_ERR_VIU_BUF_FULL;
        }
    } else {
        viu_pipe_up_sem(pipe_id);
        ret = mcp_osal_wait_interruptible(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame,
            viu_wait_condition_call_back, &viu_wait_conf_info);
        viu_pipe_down_interruptible_sem(pipe_id);
        if (ret != TD_SUCCESS) {
            return -ERESTARTSYS;
        } else {
            if (viu_wait_conf_info.wait_flag == TD_FALSE) {
                return SOC_ERR_VIU_SYS_NOTREADY;
            }
        }
    }

    return TD_SUCCESS;
}

static td_s32 viu_line_mode_send_pipe_frame_check(td_s32 pipe_id, td_s32 milli_sec, viu_pipe_mkp_ctx *pipe_mkp_ctx)
{
    td_s32 ret;
    td_ulong flags;
    td_s32 unique_id;

    if (is_pipe_unexist(pipe_id)) {
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    if (milli_sec < -1) {
        viu_err_trace("pipe %d milli_sec %d err!\n", pipe_id, milli_sec);
        return SOC_ERR_VIU_INVALID_PARA;
    }

    if (is_pipe_destroying(pipe_id)) {
        return SOC_ERR_VIU_BUSY;
    }

    if (pipe_mkp_ctx->pipe_mkp_attr_setting.start == TD_FALSE) {
        viu_err_trace("pipe %d please start pipe first!!\n", pipe_id);
        return SOC_ERR_VIU_SYS_NOTREADY;
    }

    viu_pipe_spin_lock(pipe_id, flags);
    unique_id = get_pipe_unique_id(pipe_id);
    viu_pipe_spin_unlock(pipe_id, flags);

    ret = wait_for_line_mode_pipe_frame_que(pipe_id, unique_id, milli_sec);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    viu_pipe_spin_lock(pipe_id, flags);
    ret = viu_abnormal_verify_after_wait(pipe_id, unique_id);
    if (ret != TD_SUCCESS) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return ret;
    }

    if (viu_pic_queue_get_busy_num(&pipe_mkp_ctx->line_input_pic_queue) >= MAX_PIC_QUEUE_LENGTH_FOR_USER) {
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d  queue is full,can't receive.!!\n", pipe_id);
        return SOC_ERR_VIU_BUF_FULL;
    }

    viu_pipe_spin_unlock(pipe_id, flags);
    return TD_SUCCESS;
}

#if defined(CONFIG_CHIP_3322)
static td_s32 viu_line_mode_send_pipe_frame_queue(td_s32 pipe_id, ext_isp_video_frame_info *video_frame,
    viu_pipe_mkp_ctx *pipe_mkp_ctx)
{
    td_s32 ret;
    td_ulong flags;

    viu_pipe_spin_lock(pipe_id, flags);

    ret = viu_put_linear_frame_to_queue(pipe_id, video_frame, TD_FALSE);
    if (ret != TD_SUCCESS) {
        viu_pipe_spin_unlock(pipe_id, flags);
        return SOC_ERR_VIU_BUF_FULL;
    }

    pipe_mkp_ctx->ori_height = video_frame->v_frame.height;
    pipe_mkp_ctx->ori_width = video_frame->v_frame.width;
    pipe_mkp_ctx->pipe_statistic.recv_pic++;
    viu_update_working_pipe_chn_mkp_attr(pipe_id);

    viu_pipe_spin_unlock(pipe_id, flags);
    return TD_SUCCESS;
}
#else
static td_s32 viu_line_mode_send_pipe_frame_queue(td_s32 pipe_id, ext_isp_video_frame_info *video_frame,
    viu_pipe_mkp_ctx *pipe_mkp_ctx)
{
    td_s32 ret;
    isp_be_cfg be_cfg;
    td_ulong flags;

    ret = viu_drv_get_be_cfg(pipe_id, &be_cfg.be_cfg_buf, &be_cfg.be_sync_param);
    if (ret != TD_SUCCESS) {
        return SOC_ERR_VIU_BUSY;
    }

    viu_pipe_spin_lock(pipe_id, flags);

    ret = viu_put_linear_frame_to_queue(pipe_id, video_frame, &be_cfg, TD_FALSE);
    if (ret != TD_SUCCESS) {
        viu_drv_put_free_be_buf(pipe_id, &be_cfg.be_cfg_buf);
        viu_pipe_spin_unlock(pipe_id, flags);
        return SOC_ERR_VIU_BUF_FULL;
    }

    pipe_mkp_ctx->ori_height = video_frame->v_frame.height;
    pipe_mkp_ctx->ori_width = video_frame->v_frame.width;
    pipe_mkp_ctx->pipe_statistic.recv_pic++;
    viu_update_working_pipe_chn_mkp_attr(pipe_id);

    viu_pipe_spin_unlock(pipe_id, flags);
    return TD_SUCCESS;
}
#endif

td_s32 viu_line_mode_send_pipe_frame(td_s32 pipe_id, ext_isp_video_frame_info *video_frame, td_s32 milli_sec,
    ext_viu_operating_mode_type pipe_mode)
{
    td_s32 ret = TD_SUCCESS;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = TD_NULL;

    viu_check_null_ptr_return(video_frame);
    viu_check_pipe_id_return(pipe_id);

    viu_pipe_down_interruptible_sem(pipe_id);

    pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);

    ret = viu_line_mode_send_pipe_frame_check(pipe_id, milli_sec, pipe_mkp_ctx);
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        return ret;
    }

    /* 图像进入图像队列中 */
    ret = viu_line_mode_send_pipe_frame_queue(pipe_id, video_frame, pipe_mkp_ctx);
    if (ret != TD_SUCCESS) {
        viu_pipe_up_sem(pipe_id);
        return ret;
    }

#if defined(CONFIG_CHIP_3322)
    viu_unused(pipe_mode);
#else
    (td_void)viu_start_one_task(pipe_id, pipe_mode);
#endif
    viu_pipe_up_sem(pipe_id);
    return ret;
}

td_s32 viu_mod_release_frame(ext_isp_video_frame_info *frame_info)
{
    return viu_drv_put_vb_with_ret_value(frame_info, VB_UID_USER);
}

viu_vb_source viu_get_vb_source(td_void)
{
    return g_vb_source;
}

/**********************************VICAP MPI->MKP end***********************************/
td_s32 viu_cap_init(td_void)
{
    td_s32 ret;

    /* 上下文初始化 */
    viu_dev_mkp_ctx_global_init();

    viu_drv_cap_dev_ctx_init();

    /* 注册vicap 中断处理 */
    ret = viu_cap_comm_dev_init();

    return ret;
}

td_void viu_cap_deinit(td_void *unique)
{
    td_s32 i;

    for (i = 0; i < VIU_MAX_DEV_NUM; i++) {
        (td_void)viu_dev_disable(i, unique);
    }

    viu_cap_comm_dev_exit();

    return;
}

td_void viu_set_default_mod_param(td_void)
{
    ext_viu_mod_param *mod_param_ctx = TD_NULL;

    mod_param_ctx = viu_get_mod_param_ctx();

    mod_param_ctx->detect_err_frame = 0;
    mod_param_ctx->drop_err_frame = 0;
}

td_void viu_init_time_ref(td_void)
{
    td_s32 pipe_id;

    for (pipe_id = 0; pipe_id < ISP_MAX_PIPE_NUM; pipe_id++) {
        (td_void)memset_s(&g_ast_pipe_time_ref_ctx[pipe_id], sizeof(g_ast_pipe_time_ref_ctx[pipe_id]), 0x0,
            sizeof(viu_time_ref_info));
    }
}

td_u32 viu_calc_chn_time_ref(td_s32 pipe_id, td_s32 chn_id)
{
    td_u32 time_ref;

    time_ref = g_ast_pipe_time_ref_ctx[pipe_id].chn_time_ref[chn_id];

    g_ast_pipe_time_ref_ctx[pipe_id].chn_time_ref[chn_id] = time_ref + 2; /* time_ref add step 2 */

    return time_ref;
}

td_u32 viu_calc_raw_time_ref(td_s32 pipe_id)
{
    td_u32 time_ref;

    time_ref = g_ast_pipe_time_ref_ctx[pipe_id].raw_time_ref;

    g_ast_pipe_time_ref_ctx[pipe_id].raw_time_ref = time_ref + 2; /* time_ref add step 2 */

    return time_ref;
}

td_u32 viu_get_chn_last_time_ref(td_s32 pipe_id, td_s32 chn_id)
{
    td_u32 time_ref;

    time_ref = g_ast_pipe_time_ref_ctx[pipe_id].chn_last_time_ref[chn_id];

    return time_ref;
}

td_void viu_update_chn_last_time_ref(td_s32 pipe_id, td_s32 chn_id, td_u32 time_ref)
{
    g_ast_pipe_time_ref_ctx[pipe_id].chn_last_time_ref[chn_id] = time_ref;
}

static td_s32 viu_dev_lock_init(td_s32 viu_dev_id)
{
    td_s32 ret;

    ret = mcp_osal_sem_init(&g_ast_dev_synch_ctx[viu_dev_id].dev_sem, 1);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi dev %d sema init failed!\n", viu_dev_id);
        goto fail0;
    }

    ret = mcp_osal_spin_lock_init(&g_ast_dev_synch_ctx[viu_dev_id].dev_lock);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi dev %d spinlock init failed!\n", viu_dev_id);
        goto fail1;
    }

    return TD_SUCCESS;

fail1:
    mcp_osal_sem_destroy(&g_ast_dev_synch_ctx[viu_dev_id].dev_sem);
fail0:
    return TD_FAILURE;
}

static td_void viu_dev_lock_de_init(td_s32 viu_dev_id)
{
    mcp_osal_spin_lock_destroy(&g_ast_dev_synch_ctx[viu_dev_id].dev_lock);
    mcp_osal_sem_destroy(&g_ast_dev_synch_ctx[viu_dev_id].dev_sem);
}

static td_s32 viu_dev_all_lock_init(td_void)
{
    td_s32 viu_dev_id;
    td_s32 ret;

    for (viu_dev_id = 0; viu_dev_id < VIU_MAX_DEV_NUM; viu_dev_id++) {
        ret = viu_dev_lock_init(viu_dev_id);
        if (ret != TD_SUCCESS) {
            goto fail;
        }
    }

    return TD_SUCCESS;

fail:
    for (viu_dev_id = viu_dev_id - 1; viu_dev_id >= 0; viu_dev_id--) {
        viu_dev_lock_de_init(viu_dev_id);
    }

    return TD_FAILURE;
}

static td_void viu_dev_all_lock_deinit(td_void)
{
    td_s32 viu_dev_id;

    for (viu_dev_id = 0; viu_dev_id < VIU_MAX_DEV_NUM; viu_dev_id++) {
        viu_dev_lock_de_init(viu_dev_id);
    }
}

static td_s32 viu_single_pipe_lock_init_wait(td_s32 pipe_id)
{
    td_s32 ret;

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_user_send_frame init failed!\n", pipe_id);
        goto fail0;
    }

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_region_luma);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_user_get_region_luma init failed!\n", pipe_id);
        goto fail1;
    }

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_use_get_raw_frame init failed!\n", pipe_id);
        goto fail2;
    }

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_stop_working init failed!\n", pipe_id);
        goto fail3;
    }

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_get_bnr_raw_frame);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_get_bnr_raw_frame init failed!\n", pipe_id);
        goto fail6;
    }

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_dis_fin);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d wait_dis_fin init failed!\n", pipe_id);
        goto fail10;
    }

    return TD_SUCCESS;

fail10:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_get_bnr_raw_frame);
fail6:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working);
fail3:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame);
fail2:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_region_luma);
fail1:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame);
fail0:
    return TD_FAILURE;
}

static td_s32 viu_single_pipe_lock_init(td_s32 pipe_id)
{
    td_s32 ret;

    ret = mcp_osal_sem_init(&g_ast_pipe_synch_ctx[pipe_id].get_chn_luma_sem, 1);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d get_chn_luma_sem init failed!\n", pipe_id);
        goto fail4;
    }

    ret = mcp_osal_sem_init(&g_ast_pipe_synch_ctx[pipe_id].pipe_sem, 1);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d pipe_sem init failed!\n", pipe_id);
        goto fail5;
    }

    ret = mcp_osal_spin_lock_init(&g_ast_pipe_synch_ctx[pipe_id].pipe_lock);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d pipe_lock init failed!\n", pipe_id);
        goto fail7;
    }

    ret = mcp_osal_atomic_init(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d other_mod_job_cnt init failed!\n", pipe_id);
        goto fail8;
    }

    ret = mcp_osal_atomic_init(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d dis_job_cnt init failed!\n", pipe_id);
        goto fail9;
    }

    ret = viu_single_pipe_lock_init_wait(pipe_id);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d viu_single_pipe_lock_init_wait init failed!\n", pipe_id);
        goto fail10;
    }

    return TD_SUCCESS;

fail10:
    mcp_osal_atomic_destroy(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt);
fail9:
    mcp_osal_atomic_destroy(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt);
fail8:
    mcp_osal_spin_lock_destroy(&g_ast_pipe_synch_ctx[pipe_id].pipe_lock);
fail7:
    mcp_osal_sem_destroy(&g_ast_pipe_synch_ctx[pipe_id].pipe_sem);
fail5:
    mcp_osal_sem_destroy(&g_ast_pipe_synch_ctx[pipe_id].get_chn_luma_sem);
fail4:
    return TD_FAILURE;
}

static td_void viu_single_pipe_lock_de_init(td_s32 pipe_id)
{
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_dis_fin);
    mcp_osal_atomic_destroy(&g_ast_pipe_synch_ctx[pipe_id].dis_job_cnt);
    mcp_osal_atomic_destroy(&g_ast_pipe_synch_ctx[pipe_id].other_mod_job_cnt);
    mcp_osal_spin_lock_destroy(&g_ast_pipe_synch_ctx[pipe_id].pipe_lock);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_get_bnr_raw_frame);
    mcp_osal_sem_destroy(&g_ast_pipe_synch_ctx[pipe_id].pipe_sem);
    mcp_osal_sem_destroy(&g_ast_pipe_synch_ctx[pipe_id].get_chn_luma_sem);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_stop_working);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_use_get_raw_frame);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_region_luma);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_send_frame);
}

static td_s32 viu_chn_lock_init(td_s32 pipe_id, td_s32 chn_id)
{
    td_s32 ret;

    ret = mcp_osal_wait_init(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_chn_frame[chn_id]);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d chn %d wait_user_get_chn_frame init failed!\n", pipe_id, chn_id);
        goto fail0;
    }

    ret = mcp_osal_sem_init(&g_ast_pipe_synch_ctx[pipe_id].get_chn_frame_sem[chn_id], 1);
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi pipe %d chn %d get_chn_frame_sem init failed!\n", pipe_id, chn_id);
        goto fail1;
    }

    return TD_SUCCESS;

fail1:
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_chn_frame[chn_id]);
fail0:
    return TD_FAILURE;
}

static td_void viu_chn_lock_de_init(td_s32 pipe_id, td_s32 chn_id)
{
    mcp_osal_sem_destroy(&g_ast_pipe_synch_ctx[pipe_id].get_chn_frame_sem[chn_id]);
    mcp_osal_wait_destroy(&g_ast_pipe_synch_ctx[pipe_id].wait_user_get_chn_frame[chn_id]);
}

static td_s32 viu_chn_all_lock_init(td_s32 pipe_id)
{
    td_s32 chn_id;
    td_s32 ret;

    for (chn_id = 0; chn_id < VIU_MAX_CHN_NUM; chn_id++) {
        ret = viu_chn_lock_init(pipe_id, chn_id);
        if (ret != TD_SUCCESS) {
            goto fail;
        }
    }

    return TD_SUCCESS;

fail:
    for (chn_id = chn_id - 1; chn_id >= 0; chn_id--) {
        viu_chn_lock_de_init(pipe_id, chn_id);
    }

    return TD_FAILURE;
}

static td_void viu_chn_all_lock_de_init(td_s32 pipe_id)
{
    td_s32 chn_id;

    for (chn_id = 0; chn_id < VIU_MAX_CHN_NUM; chn_id++) {
        viu_chn_lock_de_init(pipe_id, chn_id);
    }
}

static td_s32 viu_pipe_lock_init(td_s32 pipe_id)
{
    td_s32 ret;

    ret = viu_single_pipe_lock_init(pipe_id);
    if (ret != TD_SUCCESS) {
        goto fail0;
    }

    ret = viu_chn_all_lock_init(pipe_id);
    if (ret != TD_SUCCESS) {
        goto fail1;
    }

    return TD_SUCCESS;

fail1:
    viu_single_pipe_lock_de_init(pipe_id);
fail0:
    return TD_FAILURE;
}

static td_void viu_pipe_lock_de_init(td_s32 pipe_id)
{
    viu_chn_all_lock_de_init(pipe_id);
    viu_single_pipe_lock_de_init(pipe_id);
}

static td_s32 viu_pipe_all_lock_init(td_void)
{
    td_s32 pipe_id;
    td_s32 ret;

    for (pipe_id = 0; pipe_id < ISP_MAX_PIPE_NUM; pipe_id++) {
        ret = viu_pipe_lock_init(pipe_id);
        if (ret != TD_SUCCESS) {
            goto fail;
        }
    }

    return TD_SUCCESS;

fail:
    for (pipe_id = pipe_id - 1; pipe_id >= 0; pipe_id--) {
        viu_pipe_lock_de_init(pipe_id);
    }

    return TD_FAILURE;
}

static td_void viu_pipe_all_lock_de_init(td_void)
{
    td_s32 pipe_id;

    for (pipe_id = 0; pipe_id < ISP_MAX_PIPE_NUM; pipe_id++) {
        viu_pipe_lock_de_init(pipe_id);
    }
}

td_s32 viu_synch_ctx_init(td_void)
{
    if (mcp_osal_spin_lock_init(&g_viu_module_lock) != TD_SUCCESS) {
        viu_err_trace("g_viu_module_lock init failed!\n");
        goto fail0;
    }

    if (mcp_osal_spin_lock_init(&g_viu_comm_lock) != TD_SUCCESS) {
        viu_err_trace("g_viu_comm_lock init failed!\n");
        goto fail1;
    }

    if (viu_dev_all_lock_init() != TD_SUCCESS) {
        goto fail3;
    }

    if (viu_pipe_all_lock_init() != TD_SUCCESS) {
        goto fail4;
    }

    return TD_SUCCESS;

fail4:
    viu_dev_all_lock_deinit();
fail3:
#if defined(CONFIG_CHIP_3322)
#else
    viu_drv_destroy_fpn_spin_lock();
fail2:
#endif
    mcp_osal_spin_lock_destroy(&g_viu_comm_lock);
fail1:
    mcp_osal_spin_lock_destroy(&g_viu_module_lock);
fail0:
    return TD_FAILURE;
}

td_void viu_synch_ctx_exit(td_void)
{
    viu_pipe_all_lock_de_init();
    viu_dev_all_lock_deinit();
#if defined(CONFIG_CHIP_3322)
#else
    viu_drv_destroy_fpn_spin_lock();
#endif
    mcp_osal_spin_lock_destroy(&g_viu_comm_lock);
    mcp_osal_spin_lock_destroy(&g_viu_module_lock);
}

td_bool viu_is_pipe_existed(td_s32 pipe_id)
{
    td_ulong flags;
    td_bool existed;

    if (viu_check_pipe_id(pipe_id) != TD_SUCCESS) {
        return TD_FALSE;
    }

    viu_pipe_spin_lock(pipe_id, flags);

    if (is_pipe_unexist(pipe_id)) {
        existed = TD_FALSE;
    } else {
        existed = TD_TRUE;
    }

    viu_pipe_spin_unlock(pipe_id, flags);

    return existed;
}

td_s32 viu_get_pipe_bind_dev_size(td_s32 pipe_id, ext_isp_size *dev_size)
{
    td_s32 viu_dev_id;
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;

    /* 参数检查 */
    viu_check_phy_pipe_id_return(pipe_id);
    viu_check_null_ptr_return(dev_size);

    viu_pipe_spin_lock(pipe_id, flags);

    if (is_pipe_unexist(pipe_id)) {
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d unexisted!\n", pipe_id);
        return SOC_ERR_VIU_PIPE_UNEXIST;
    }

    if (is_pipe_destroying(pipe_id)) {
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("pipe %d is destroying!\n", pipe_id);
        return SOC_ERR_VIU_BUSY;
    }

    viu_dev_id = viu_drv_get_pipe_bind_dev_id(pipe_id);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);
    if (viu_dev_ctx->dev_cfged != TD_TRUE) {
        viu_pipe_spin_unlock(pipe_id, flags);
        viu_err_trace("attr of vi dev %d not configured\n", viu_dev_id);
        return SOC_ERR_VIU_FAILED_NOTCONFIG;
    }

    viu_drv_get_pipe_pre_size(pipe_id, dev_size);

    viu_pipe_spin_unlock(pipe_id, flags);

    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
