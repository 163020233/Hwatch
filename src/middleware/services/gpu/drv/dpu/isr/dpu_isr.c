/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu isr file
 */

#include "dpu_isr.h"
#include "soc_lcd_api.h"
#include "gpu_feature_config.h"
#include "drv_gfx_port.h"
#include "drv_dpu_intf.h"
#include "dpu_dfx.h"
#include "dpu_hal.h"
#if (GPU_LOW_POWER_SUPPORT == YES)
#if TARGET_CHIP_BRANDY
#include "pm.h"
#else
#include "pm_veto_porting.h"
#endif
#include "pm_veto.h"
#endif

typedef struct {
    osal_spinlock isr_lock;
    osal_wait finish_event;
    osal_wait te_signal_event;
#ifdef GPU_SUPPORT_VIDEO_MODE
    osal_wait vthd_percent_event;
    volatile td_s32 vthd_percent_done;
#endif
    volatile td_s32 finish_done;
    volatile td_s32 te_signal_done;
} dpu_isr_info;

STATIC dpu_isr_info g_dpu_isr_info = {0};

STATIC td_void isr_disp_set_dfx_info(td_u32 cur_time_us, dpu_dfx_info *dfx_info)
{
    if ((cur_time_us - dfx_info->one_sec_ago) >= 1000000) { /* 1000000 us */
        dfx_info->one_sec_ago   = cur_time_us;
        dfx_info->draw_fps      = dfx_info->refresh_cnt;
        dfx_info->flip_fps      = dfx_info->frm_done_cnt;
        dfx_info->te_fps        = dfx_info->te_signal_cnt;

        dfx_info->refresh_cnt   = 0;
        dfx_info->frm_done_cnt  = 0;
        dfx_info->te_signal_cnt = 0;

        dpu_dfx_print_info();
    }
}

STATIC td_bool isr_disp_is_frm_done(td_u32 int_type)
{
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
#ifndef SUPPORT_GPU_QSPI
    if ((int_type & DPU_DISP_INT_TYPE_FRM_DONE) == DPU_DISP_INT_TYPE_FRM_DONE) {
        dfx_info->frm_end_total++;
        return TD_TRUE;
    }
#else
    static td_u32 interrupt_status = 0;
    if ((int_type & DPU_DISP_INT_TYPE_FRM_DONE) == DPU_DISP_INT_TYPE_FRM_DONE) {
        interrupt_status |= DPU_DISP_INT_TYPE_FRM_DONE;
        dfx_info->frm_end_total++;
    }

    if ((int_type & DPU_DISP_INT_TYPE_WBC_DONE) == DPU_DISP_INT_TYPE_WBC_DONE) {
        interrupt_status |= DPU_DISP_INT_TYPE_WBC_DONE;
        dfx_info->wbc_done_total++;
    }

    td_u32 qspi_done = (DPU_DISP_INT_TYPE_FRM_DONE | DPU_DISP_INT_TYPE_WBC_DONE);
    if ((interrupt_status & qspi_done) == qspi_done) {
        interrupt_status = 0;
        return TD_TRUE;
    }
#endif

    return TD_FALSE;
}

STATIC td_void isr_disp_err_cnt(td_u32 int_type, dpu_dfx_info *dfx_info)
{
    if ((int_type & DPU_DISP_INT_TYPE_LOWBAND) == DPU_DISP_INT_TYPE_LOWBAND) {
        dfx_info->lowband_cnt++;
    }
    if ((int_type & DPU_DISP_INT_TYPE_BUS_ERR) == DPU_DISP_INT_TYPE_BUS_ERR) {
        dfx_info->bus_err_cnt++;
    }
    if ((int_type & DPU_DISP_INT_TYPE_DCMP_CFG_ERR) == DPU_DISP_INT_TYPE_DCMP_CFG_ERR) {
        dfx_info->dcmp_cfg_err_cnt++;
    }
    if ((int_type & DPU_DISP_INT_TYPE_DCMP_BS_ERR) == DPU_DISP_INT_TYPE_DCMP_BS_ERR) {
        dfx_info->dcmp_bs_err_cnt++;
    }
}

STATIC td_void isr_disp_main(td_u32 param)
{
    td_u32 cur_time_us = 0;
    unsigned long flags = 0;

    uapi_unused(param);

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    td_u32 int_type = dpu_hal_disp_get_int_state();
    dpu_hal_disp_clear_int_state(int_type);

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flags);

    isr_disp_err_cnt(int_type, dfx_info);

#ifdef GPU_SUPPORT_VIDEO_MODE
    if ((int_type & DPU_DISP_INT_TYPE_SETTING) == DPU_DISP_INT_TYPE_SETTING) {
        g_dpu_isr_info.vthd_percent_done = 1;
        osal_wait_wakeup(&g_dpu_isr_info.vthd_percent_event);
    }
    if ((int_type & DPU_DISP_INT_TYPE_FINISH) == DPU_DISP_INT_TYPE_FINISH) {
        g_dpu_isr_info.vthd_percent_done = 0;
    }
#endif
    if ((int_type & DPU_DISP_INT_TYPE_TE_INT) == DPU_DISP_INT_TYPE_TE_INT) {
        gpu_dfx_get_time_us(&cur_time_us);
        dfx_info->te_cost_us = (cur_time_us - dfx_info->last_te_time_us);
        dfx_info->last_te_time_us = cur_time_us;
        dfx_info->te_signal_cnt++;
        dfx_info->te_signal_total++;
        g_dpu_isr_info.te_signal_done = 1;
    }

    if (isr_disp_is_frm_done(int_type) == TD_TRUE) {
        g_dpu_isr_info.finish_done = 1;
        dfx_info->frm_done_cnt++;
        dfx_info->frm_done_total++;
        gpu_dfx_get_time_us(&dfx_info->last_isr_done_time);
        uapi_lcd_frame_done_process();
#if (GPU_LOW_POWER_SUPPORT == YES)
        uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_DPU);
#endif
    }

    gpu_dfx_get_time_us(&cur_time_us);
    isr_disp_set_dfx_info(cur_time_us, dfx_info);
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flags);

    if (g_dpu_isr_info.finish_done == 1) {
        osal_wait_wakeup(&g_dpu_isr_info.finish_event);
    }

    if (g_dpu_isr_info.te_signal_done == 1) {
        osal_wait_wakeup(&g_dpu_isr_info.te_signal_event);
    }
}

STATIC td_s32 isr_get_te_signal_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.te_signal_done;
}

STATIC td_s32 isr_get_finish_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.finish_done;
}

#ifdef GPU_SUPPORT_VIDEO_MODE
static td_s32 isr_get_vthd_percent_signal_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.vthd_percent_done;
}

static td_s32 dpu_init_vvthd_percent_event(void)
{
    td_s32 ret;
    unsigned long flag = 0;

    ret = osal_wait_init(&g_dpu_isr_info.vthd_percent_event);
    if (ret != EXT_SUCCESS) {
        gpu_err("dpu_init_vvthd_percent failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flag);
    g_dpu_isr_info.vthd_percent_done = 1;
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);

    return EXT_SUCCESS;
}
td_void dpu_isr_wait_vthd_percent_signal(void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.vthd_percent_event,
                                          isr_get_vthd_percent_signal_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        gpu_err("osal_wait_timeout!!!!");
        return;
    }

    return;
}

td_void dpu_isr_clear_vthd_percent_flag(void)
{
    g_dpu_isr_info.vthd_percent_done = 0;
}
#endif

td_s32 dpu_isr_init(void)
{
    td_s32 ret;
    unsigned long flag = 0;

    (void)memset_s(&g_dpu_isr_info, sizeof(g_dpu_isr_info), 0, sizeof(dpu_isr_info));

    ret = osal_spin_lock_init(&g_dpu_isr_info.isr_lock);
    if (ret != EXT_SUCCESS) {
        gpu_err("osal_spin_lock_init failed, ret:0x%x", ret);
        return ret;
    }

    ret = osal_wait_init(&g_dpu_isr_info.finish_event);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        gpu_err("dpu_layer_init_wait_event failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    ret = osal_wait_init(&g_dpu_isr_info.te_signal_event);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        gpu_err("dpu_layer_init_wait_event failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flag);
    g_dpu_isr_info.te_signal_done = 1;
    g_dpu_isr_info.finish_done = 1;
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);

#ifdef GPU_SUPPORT_VIDEO_MODE
    ret = dpu_init_vvthd_percent_event();
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        osal_wait_destroy(&g_dpu_isr_info.te_signal_event);
        return DRV_ERR_DPU_OSAL_FAILED;
    }
#endif

    ret = osal_irq_request(DPU_INT_DISP, (osal_irq_handler)isr_disp_main, TD_NULL, TD_NULL, TD_NULL);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        osal_wait_destroy(&g_dpu_isr_info.te_signal_event);
#ifdef GPU_SUPPORT_VIDEO_MODE
        osal_wait_destroy(&g_dpu_isr_info.vthd_percent_event);
#endif
        gpu_err("osal_irq_request failed, ret:0x%x", ret);
        return ret;
    }
    osal_irq_enable(DPU_INT_DISP);

    return EXT_SUCCESS;
}

td_void dpu_isr_deinit(void)
{
    osal_irq_disable(DPU_INT_DISP);
    osal_irq_free(DPU_INT_DISP, TD_NULL);
    osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
}

td_s32 dpu_isr_wait_finish(void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.finish_event,
                                          isr_get_finish_done, TD_NULL, 100); /* 100 ms */
    if (ret <= 0) {
        gpu_err("osal_wait_timeout!!!! finish_done %d suspend_flg %d", isr_get_finish_done(NULL),
            uapi_dpu_get_suspend_flag());
        if (uapi_dpu_get_suspend_flag() == TD_FALSE) {
            ext_errno ret1 = uapi_lcd_ulps_proc();
            if (ret1 != EXT_ERR_SUCCESS) {
                gpu_err("lcd ulps proc err ret %#x", ret1);
            }
        }
#if (GPU_LOW_POWER_SUPPORT == YES)
        uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_DPU);
#endif
        dpu_dfx_info *dfx_info = dpu_dfx_get_info();
        dfx_info->timeout_total++;

#ifdef GPU_SUPPORT_CHR
        static td_u32 dpu_timeout_cnt = 0;
        gpu_chr_log(MT_DPU_MANAGER, MT_DPU_ISR_STATE, MT_DPU_ISR_FINISH_TIMEOUT, 0x3); /* 0x3 is invalid layer id */
        if (dpu_timeout_cnt < 0x1) { /* 1 times */
            print_mipi_and_dpu_reg();
            dpu_timeout_cnt++;
        }
#endif
        return DRV_ERR_DPU_WAIT_TIMEOUT;
    }

    return EXT_SUCCESS;
}

td_void dpu_isr_wait_te_signal(void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.te_signal_event,
                                          isr_get_te_signal_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        gpu_err("osal_wait_timeout!!!! te_signal_done %d", isr_get_te_signal_done(NULL));
        return;
    }

    g_dpu_isr_info.te_signal_done = 0;

    return;
}

td_void dpu_isr_clear_finish_flag(void)
{
    g_dpu_isr_info.finish_done = 0;
}
