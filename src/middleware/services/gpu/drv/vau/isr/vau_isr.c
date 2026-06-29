/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: isr
 */

#include "vau_isr.h"
#include "drv_gfx_port.h"
#include "vau_dfx.h"
#include "vau_list.h"
#include "vau_hal.h"

typedef struct {
    td_u32 data;
    osal_spinlock isr_lock;
    osal_workqueue task_workqueue;
} vau_isr_info;

STATIC vau_isr_info g_vau_isr_info = { 0 };

STATIC td_bool vau_isr_is_err_int(td_u32 int_status)
{
    td_u32 err_int = DRV_VAU_INT_TIMEOUT |
        DRV_VAU_INT_DRAW_ERR | DRV_VAU_INT_ET_OVERSIZE | DRV_VAU_INT_DRAW_OVERSIZE |
        DRV_VAU_INT_DCMP_CFG_ERR | DRV_VAU_INT_DCMP_BS_ERR |
        DRV_VAU_INT_ONE_DRAW_ERR | DRV_VAU_INT_ONE_ET_OVERSIZE | DRV_VAU_INT_ONE_DRAW_OVERSIZE |
        DRV_VAU_INT_ONE_DCMP_CFG_ERR | DRV_VAU_INT_ONE_DCMP_BS_ERR;
    if ((int_status & err_int) != 0) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

STATIC td_void vau_isr_dfx_inc_cnt(const td_u32 int_status)
{
    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.isr_cnt++;

    if (int_status == 0x0) {
        vau_info->func_info.isr_cnt.isr_zero++;
    }

    if ((int_status & DRV_VAU_INT_NODE_END) != 0) {
        vau_info->func_info.isr_cnt.isr_node_end++;
    }

    if ((int_status & DRV_VAU_INT_TIMEOUT) != 0) {
        vau_info->func_info.isr_cnt.isr_timeout++;
    }

    if ((int_status & DRV_VAU_INT_BUS_ERROR) != 0) {
        vau_info->func_info.isr_cnt.isr_bus_err++;
    }

    if ((int_status & DRV_VAU_INT_LIST_END) != 0) {
        vau_info->func_info.isr_cnt.isr_list_end++;
    }

    if ((int_status & DRV_VAU_INT_DISPLAY_DONE) != 0) {
        vau_info->func_info.isr_cnt.isr_disp_done++;
    }

    if ((int_status & DRV_VAU_INT_TUNL_DONE) != 0) {
        vau_info->func_info.isr_cnt.isr_tunl_done++;
    }

    if ((int_status & DRV_VAU_INT_CONFLICT) != 0) {
        vau_info->func_info.isr_cnt.isr_conflict++;
    }

    if ((int_status & DRV_VAU_INT_DRAW_ERR) != 0) {
        vau_info->func_info.isr_cnt.isr_draw_err++;
    }

    if ((int_status & DRV_VAU_INT_ET_OVERSIZE) != 0) {
        vau_info->func_info.isr_cnt.isr_et_oversize++;
    }

    if ((int_status & DRV_VAU_INT_DRAW_OVERSIZE) != 0) {
        vau_info->func_info.isr_cnt.isr_draw_oversize++;
    }

    if ((int_status & DRV_VAU_INT_DCMP_CFG_ERR) != 0) {
        vau_info->func_info.isr_cnt.isr_dcmp_cfg_err++;
    }

    if ((int_status & DRV_VAU_INT_DCMP_BS_ERR) != 0) {
        vau_info->func_info.isr_cnt.isr_dcmp_bs_err++;
    }
}

STATIC td_void vau_tasklet_func(osal_workqueue *work)
{
    td_u32 int_status;
    td_bool is_err_int;
    unsigned long flags = 0;

    uapi_unused(work);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.tasklet_enter_cnt++;

    osal_spin_lock_irqsave(&g_vau_isr_info.isr_lock, &flags);
    int_status = g_vau_isr_info.data;
    g_vau_isr_info.data &= (~int_status);
    osal_spin_unlock_irqrestore(&g_vau_isr_info.isr_lock, &flags);

    is_err_int = vau_isr_is_err_int(int_status);
    vau_list_isr_callback(int_status, is_err_int);

#ifndef CONFIG_VAU_DEAL_INT_ONCE
    if (is_err_int) {
        osal_irq_enable(VAU_INT_NUM);
    }
#endif
    vau_info->func_info.func_cnt.tasklet_exit_cnt++;
}

STATIC td_void vau_isr_main(td_u32 param)
{
    uapi_unused(param);
    unsigned long flags = 0;

    td_u32 int_status = vau_hal_get_int_state();
    vau_hal_clear_int_state(int_status);
    vau_isr_dfx_inc_cnt(int_status);

    if ((int_status == DRV_VAU_INT_NONE) || (int_status == DRV_VAU_INT_SUBNODE_END) ||
        (int_status == DRV_VAU_INT_BUS_ERROR)) {
        return; /* no need process */
    }

#ifndef CONFIG_VAU_DEAL_INT_ONCE
    if (vau_isr_is_err_int(int_status)) {
        osal_irq_disable(VAU_INT_NUM);
    }
#endif

    osal_spin_lock_irqsave(&g_vau_isr_info.isr_lock, &flags);
    g_vau_isr_info.data = g_vau_isr_info.data | (int_status);
    osal_spin_unlock_irqrestore(&g_vau_isr_info.isr_lock, &flags);

    osal_workqueue_schedule(&g_vau_isr_info.task_workqueue);
}

td_s32 vau_isr_init(void)
{
    td_s32 ret;

    (void)memset_s(&g_vau_isr_info, sizeof(g_vau_isr_info), 0, sizeof(vau_isr_info));

    ret = osal_spin_lock_init(&g_vau_isr_info.isr_lock);
    if (ret != EXT_SUCCESS) {
        gpu_err("osal_spin_lock_init failed, ret=%#x", ret);
        return ret;
    }

    ret = osal_workqueue_init(&(g_vau_isr_info.task_workqueue), vau_tasklet_func);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
        gpu_err("osal_workqueue_init failed, ret=%#x", ret);
        return ret;
    }

    ret = osal_irq_request(VAU_INT_NUM, (osal_irq_handler)vau_isr_main, TD_NULL, TD_NULL, TD_NULL);
    if (ret != EXT_SUCCESS) {
        osal_workqueue_destroy(&(g_vau_isr_info.task_workqueue));
        osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
        gpu_err("osal_irq_request failed, ret=%#x", ret);
        return ret;
    }

    osal_irq_enable(VAU_INT_NUM);

    return EXT_SUCCESS;
}

td_void vau_isr_deinit(void)
{
    osal_irq_disable(VAU_INT_NUM);
    osal_irq_free(VAU_INT_NUM, TD_NULL);
    osal_workqueue_destroy(&(g_vau_isr_info.task_workqueue));
    osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
}
