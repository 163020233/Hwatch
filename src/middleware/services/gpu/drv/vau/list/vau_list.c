/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau list manage
 */

#include "vau_list.h"
#include "vau_dfx.h"
#include "vau_mem.h"
#include "vau_handle.h"
#include "vau_hal.h"
#include "vau_hal_apb.h"
#include "gpu_crg_intf.h"

STATIC vau_sw_job_mgr *g_vau_sw_job_mgr = TD_NULL;

td_void vau_set_clk_work_scene(drv_vau_work_scene work_scene)
{
    if (g_vau_sw_job_mgr != TD_NULL) {
        g_vau_sw_job_mgr->clk_scene_info.work_scene = work_scene;
    }
}

drv_vau_work_scene vau_get_clk_work_scene(td_void)
{
    if (g_vau_sw_job_mgr != TD_NULL) {
        return g_vau_sw_job_mgr->clk_scene_info.work_scene;
    }

    gpu_err("g_vau_sw_job_mgr is null!");
    return DRV_VAU_WORK_SCENE_PERFORMANCE_MODE;
}

STATIC td_void list_free_all_node(vau_sw_node *first_node, vau_sw_node *last_node)
{
    vau_sw_node *curr_node = TD_NULL;
    vau_sw_node *next_node = TD_NULL;

    if ((first_node == TD_NULL) || (last_node == TD_NULL)) {
        return;
    }

    curr_node = first_node;
    next_node = first_node;

    while (next_node != last_node) {
        next_node = osal_list_entry(curr_node->list_head.next, vau_sw_node, list_head);
        vau_list_free_node(curr_node);
        if (next_node == TD_NULL) {
            break;
        }
        curr_node = next_node;
    }
    vau_list_free_node(last_node);

    return;
}

STATIC td_void list_excute_job(vau_sw_job *sw_job)
{
    vau_dfx_info *vau_info = vau_dfx_get_info();

    if (g_vau_sw_job_mgr->to_commit_jobs == TD_NULL) {
        g_vau_sw_job_mgr->to_commit_jobs = sw_job;
    } else {
        vau_node_add_next_node((vau_hardware_node *)g_vau_sw_job_mgr->last_job->last_node->hw_node,
                               sw_job->first_node->phy_addr);
    }

    osal_list_add_tail(&sw_job->list_head, &g_vau_sw_job_mgr->list_head);
    g_vau_sw_job_mgr->last_job    = sw_job;
    g_vau_sw_job_mgr->last_handle = sw_job->handle;

    if (vau_hal_check_is_idle() == TD_FALSE) {
        return; // logis is working
    }

    td_s32 ret = vau_hal_execute_node(g_vau_sw_job_mgr->to_commit_jobs->first_node->phy_addr);
    if (ret == EXT_SUCCESS) {
        g_vau_sw_job_mgr->committed_jobs = g_vau_sw_job_mgr->to_commit_jobs;
        g_vau_sw_job_mgr->to_commit_jobs = TD_NULL;
        g_vau_sw_job_mgr->worked_node_addr = 0x0;
        vau_info->func_info.func_cnt.execute_node_cnt++;
        vau_info->func_info.func_cnt.submit_from_user++;
    } else {
        gpu_err("execute node err! node is not idle\n");
    }

    return;
}

STATIC td_void list_destroy_job(vau_sw_job *sw_job)
{
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = vau_handle_search(sw_job->handle);
    if (handle_mgr == TD_NULL) {
        gpu_err("vau_handle_search:%u failed", sw_job->handle);
        return;
    }

    list_free_all_node(sw_job->first_node, sw_job->last_node);
    osal_list_del_init(&sw_job->list_head);
    osal_wait_destroy(&sw_job->wait_event);
    vau_handle_destroy(handle_mgr->handle);

    return;
}

STATIC td_s32 list_submit_wait_check(const td_void *data)
{
    if (*(td_s32 *)data == DRV_VAU_JOB_NOTIFY_MAX) {
        return 1;
    }
    return 0;
}

STATIC td_void list_timeout_resume(td_void)
{
    td_size_t lockflags = 0;
    vau_sw_job *sw_job = TD_NULL;
    vau_dfx_info *dfx_info = vau_dfx_get_info();
    gpu_err("timeout last handle = %d", dfx_info->last_handle);

    if (vau_dfx_check_mask(VAU_DFX_MASK_NODE_ALL)) {
        uapi_vau_dfx_proc_read(0, TD_NULL);
        vau_dfx_print_reg();
    }

    vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = vau_get_clk_work_scene()};
    vau_hal_init(&scene_info);

    while (osal_list_empty(&g_vau_sw_job_mgr->list_head) == 0) {
        sw_job = osal_list_entry(g_vau_sw_job_mgr->list_head.next, vau_sw_job, list_head);
        list_destroy_job(sw_job);
    }

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
    g_vau_sw_job_mgr->last_job       = TD_NULL;
    g_vau_sw_job_mgr->to_commit_jobs = TD_NULL;
    g_vau_sw_job_mgr->committed_jobs = TD_NULL;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    return;
}

STATIC vau_sw_job *list_get_sw_job(td_handle handle)
{
    vau_sw_job *sw_job = TD_NULL;
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        return TD_NULL;
    }

    sw_job = (vau_sw_job *)handle_mgr->sw_job;
    if (sw_job == NULL) {
        return TD_NULL;
    }

    if (sw_job->has_submitted == TD_TRUE) {
        return TD_NULL;
    }

    if (sw_job->first_node == TD_NULL) {
        return TD_NULL;
    }

    if (sw_job->last_node == TD_NULL) {
        return TD_NULL;
    }

    return sw_job;
}

STATIC td_void list_submit_to_commit_job(td_void)
{
    td_s32 ret;
    vau_dfx_info *vau_info = vau_dfx_get_info();

    if ((g_vau_sw_job_mgr->to_commit_jobs != TD_NULL) &&
        (g_vau_sw_job_mgr->to_commit_jobs->first_node != TD_NULL)) {
        ret = vau_hal_execute_node(g_vau_sw_job_mgr->to_commit_jobs->first_node->phy_addr);
        if (ret == EXT_SUCCESS) {
            g_vau_sw_job_mgr->committed_jobs = g_vau_sw_job_mgr->to_commit_jobs;
            g_vau_sw_job_mgr->to_commit_jobs = TD_NULL;
            g_vau_sw_job_mgr->worked_node_addr = 0x0;
            vau_info->func_info.func_cnt.execute_node_cnt++;
            vau_info->func_info.func_cnt.submit_from_int++;
        }
    }

    return;
}

STATIC td_s32 list_comp_delete_job(td_handle finished_handle, vau_int_type int_type)
{
    td_handle del_handle;
    vau_sw_job *del_job = TD_NULL;

    while (osal_list_empty(&g_vau_sw_job_mgr->list_head) == 0) {
        del_job = osal_list_entry(g_vau_sw_job_mgr->list_head.next, vau_sw_job, list_head);
        del_handle = del_job->handle;
        if (del_handle == g_vau_sw_job_mgr->last_handle) {
            g_vau_sw_job_mgr->last_handle = VAU_HANDLE_INVALID;
        }
        osal_list_del_init(&del_job->list_head);
        del_job->int_type = int_type;
        if (del_job->notify_type == DRV_VAU_JOB_NOTIFY_SYNC) {
            del_job->notify_type = DRV_VAU_JOB_NOTIFY_MAX;
            if (del_job->is_wait) {
                osal_wait_wakeup(&del_job->wait_event);
            }
            osal_wait_wakeup(&g_vau_sw_job_mgr->wait_event);
        } else if (del_job->notify_type == DRV_VAU_JOB_NOTIFY_ASYNC) {
            if (del_job->wait_done_count != 0) {
                del_job->notify_type = DRV_VAU_JOB_NOTIFY_MAX;
                osal_wait_wakeup(&del_job->wait_event);
            } else {
                list_destroy_job(del_job);
            }
        } else {
            gpu_err("error status!!");
        }
        if (del_handle == finished_handle) {
            return EXT_SUCCESS;
        }
    }
    return EXT_FAILURE;
}

td_s32 vau_list_init(td_void)
{
    if (vau_handle_list_init() != EXT_SUCCESS) {
        gpu_err("call vau_handle_list_init failed");
        return EXT_FAILURE;
    }

    g_vau_sw_job_mgr = (vau_sw_job_mgr *)osal_kmalloc(sizeof(vau_sw_job_mgr), OSAL_GFP_KERNEL);
    if (g_vau_sw_job_mgr == TD_NULL) {
        vau_handle_list_deinit();
        gpu_err("call osal_kmalloc failed");
        return EXT_FAILURE;
    }
    (td_void)memset_s(g_vau_sw_job_mgr, sizeof(*g_vau_sw_job_mgr), 0x0, sizeof(vau_sw_job_mgr));
    g_vau_sw_job_mgr->offline_finish_done = TD_TRUE;

    OSAL_INIT_LIST_HEAD(&g_vau_sw_job_mgr->list_head);
    osal_spin_lock_init(&g_vau_sw_job_mgr->lock);
    osal_wait_init(&g_vau_sw_job_mgr->wait_event);

    g_vau_sw_job_mgr->clk_scene_info.cfg_scene = GPU_CLK_CFG_INIT;
    g_vau_sw_job_mgr->clk_scene_info.work_scene = DRV_VAU_WORK_SCENE_PERFORMANCE_MODE;

    return EXT_SUCCESS;
}

td_void vau_list_deinit(td_void)
{
    vau_sw_job *sw_job = TD_NULL;

    if (g_vau_sw_job_mgr == TD_NULL) {
        gpu_err("g_vau_sw_job_mgr is null pointer!");
        return;
    }

    while (osal_list_empty(&g_vau_sw_job_mgr->list_head) == 0) {
        sw_job = osal_list_entry(g_vau_sw_job_mgr->list_head.next, vau_sw_job, list_head);
        list_destroy_job(sw_job);
    }

    osal_spin_lock_destroy(&g_vau_sw_job_mgr->lock);
    osal_wait_destroy(&g_vau_sw_job_mgr->wait_event);

    osal_kfree(g_vau_sw_job_mgr);
    g_vau_sw_job_mgr = TD_NULL;

    vau_handle_list_deinit();

    return;
}

td_s32 vau_list_create(td_handle *handle)
{
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = vau_handle_create(sizeof(vau_handle_mgr) + sizeof(vau_sw_job));
    if (handle_mgr == TD_NULL) {
        gpu_err("vau_handle_create failed!");
        return DRV_ERR_VAU_NO_MEM;
    }

    *handle = handle_mgr->handle;
    handle_mgr->sw_job = (td_void *)((td_u8 *)handle_mgr + sizeof(vau_handle_mgr));

    vau_sw_job *sw_job = (vau_sw_job *)handle_mgr->sw_job;
    sw_job->handle     = handle_mgr->handle;

    osal_wait_init(&sw_job->wait_event);
    OSAL_INIT_LIST_HEAD(&sw_job->list_head);

    return EXT_SUCCESS;
}

td_s32 vau_list_destroy(td_handle handle)
{
    td_size_t lockflags = 0;
    vau_sw_job *sw_job = TD_NULL;
    vau_handle_mgr *handle_mgr = TD_NULL;

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        return EXT_SUCCESS;
    }
    sw_job = (vau_sw_job *)handle_mgr->sw_job;

    list_free_all_node(sw_job->first_node, sw_job->last_node);
    osal_list_del_init(&sw_job->list_head);
    osal_wait_destroy(&sw_job->wait_event);
    vau_handle_destroy(handle_mgr->handle);

    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    return EXT_SUCCESS;
}

td_s32 vau_list_blit(td_handle handle, vau_sw_node *sw_node)
{
    vau_sw_job *sw_job = TD_NULL;
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        gpu_err("vau_handle_search %u failed", handle);
        return DRV_ERR_VAU_INVALID_HANDLE;
    }

    sw_job = (vau_sw_job *)handle_mgr->sw_job;
    if (sw_job->has_submitted) {
        gpu_err("handle[%u] job_handle[%u] has_submitted[%u], int_type[%d]",
                handle, sw_job->handle, sw_job->has_submitted, sw_job->int_type);
        return DRV_ERR_VAU_INVALID_HANDLE;
    }

    sw_job->node_num++;
    if (sw_job->node_num == 1) {
        sw_job->first_node = sw_node;
        OSAL_INIT_LIST_HEAD(&sw_node->list_head); /* sw_node head init */
    }
    sw_node->handle = sw_job->handle;

    if (sw_job->last_node != TD_NULL) {
        /* node之间用pnext连接起来 */
        vau_node_add_next_node((vau_hardware_node *)sw_job->last_node->hw_node, sw_node->phy_addr);
    }

    osal_list_add_tail(&sw_node->list_head, &sw_job->first_node->list_head);
    sw_job->last_node = sw_node;

    return EXT_SUCCESS;
}

td_s32 vau_list_submit(td_handle handle, td_u32 time_out, drv_vau_notify_mode notify_type)
{
    td_s32 ret;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);

    vau_sw_job *sw_job = list_get_sw_job(handle);
    if (sw_job == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        return EXT_SUCCESS;
    }

    sw_job->has_submitted = TD_TRUE;
    sw_job->notify_type   = notify_type;

    /* set isr enable */
    vau_node_set_finish_isr((vau_hardware_node *)sw_job->last_node->hw_node, TD_FALSE);

    /* set hw startup */
    list_excute_job(sw_job);

    if (notify_type == DRV_VAU_JOB_NOTIFY_SYNC) {
        sw_job->wait_done_count++;
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

        ret = osal_wait_timeout_interruptible(&g_vau_sw_job_mgr->wait_event, list_submit_wait_check,
                                              (td_void *)&sw_job->notify_type, time_out);

        osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
        sw_job->wait_done_count--;

        if (sw_job->notify_type != DRV_VAU_JOB_NOTIFY_MAX) {
            osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
            gpu_err("vau_list_submit timeout handle[%u] ret[%d]", sw_job->handle, ret);
            list_timeout_resume();
            return DRV_ERR_VAU_QUERY_TIMEOUT;
        }

        if (sw_job->wait_done_count == 0) {
            list_destroy_job(sw_job);
        }
    }

    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    return EXT_SUCCESS;
}

td_void vau_list_free_node(vau_sw_node *node)
{
    if (node == TD_NULL) {
        gpu_err("node is null pointer!");
        return;
    }

    osal_list_del_init(&node->list_head);
    vau_hal_free_vector_mem((vau_hardware_node *)node->hw_node);

    if (node->is_mesh_mem_set == TD_TRUE) {
        vau_hal_free_mesh_mem((vau_hardware_node *)node->hw_node);
    }

    vau_mem_free(node->mem_type, (td_uchar *)node);
    return;
}

td_s32 vau_list_wait_for_done(td_handle handle, td_u32 time_out)
{
    td_s32 ret;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);

    vau_handle_mgr *handle_mgr = vau_handle_search(handle);
    if (handle_mgr == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        return EXT_SUCCESS;
    }
    vau_sw_job *sw_job = (vau_sw_job *)handle_mgr->sw_job;
    if (sw_job->has_submitted != TD_TRUE) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        return DRV_ERR_VAU_INVALID_HANDLE;
    }
    sw_job->is_wait = TD_TRUE;
    sw_job->wait_done_count++;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    ret = osal_wait_timeout_interruptible(&sw_job->wait_event, list_submit_wait_check,
                                          (td_void *)&sw_job->notify_type, time_out);

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
    sw_job->wait_done_count--;

    if (sw_job->notify_type != DRV_VAU_JOB_NOTIFY_MAX) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        gpu_err("vau_list_wait_for_done timeout handle[%u] ret[%d]", sw_job->handle, ret);
        list_timeout_resume();
        return DRV_ERR_VAU_QUERY_TIMEOUT;
    }

    if (sw_job->wait_done_count == 0) {
        list_destroy_job(sw_job);
    }

    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    return EXT_SUCCESS;
}

td_s32 vau_list_get_offline_finish_done(const td_void *data)
{
    uapi_unused(data);
    return g_vau_sw_job_mgr->offline_finish_done;
}

td_void vau_list_set_offline_finish_done(td_s32 finish_done)
{
    g_vau_sw_job_mgr->offline_finish_done = finish_done;
}

td_s32 vau_list_wait_for_online_done(td_u32 time_out_ms)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
    td_u32 wait_handle = g_vau_sw_job_mgr->last_handle;
    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
    if (wait_handle == VAU_HANDLE_INVALID) {
        return EXT_SUCCESS;
    }

    td_s32 ret = vau_list_wait_for_done(wait_handle, time_out_ms);
    if (ret != EXT_SUCCESS) {
        gpu_err("wait online list done timeout\n");
        return ret;
    }

    return EXT_SUCCESS;
}

td_s32 vau_list_wait_for_offline_done(td_u32 time_out_ms)
{
    td_s32 ret;
    td_size_t lockflags = 0;

    ret = osal_wait_timeout_interruptible(&g_vau_sw_job_mgr->wait_event, vau_list_get_offline_finish_done,
                                          TD_NULL, time_out_ms);

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);
    if (vau_list_get_offline_finish_done(TD_NULL) != TD_TRUE) {
        osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
        gpu_err("vau_list_wait_for_offline_done timeout ret[%d]", ret);
        list_timeout_resume();
        return DRV_ERR_VAU_QUERY_TIMEOUT;
    }
    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);

    return EXT_SUCCESS;
}

td_s32 vau_list_wait_all_done(td_u32 time_out_ms)
{
    td_s32 ret = vau_list_wait_for_online_done(time_out_ms);
    if (ret != EXT_SUCCESS) {
        gpu_err("wait online list done timeout\n");
        return ret;
    }

#ifdef GPU_SUBNODE_SUPPORT
    ret = vau_list_wait_for_offline_done(time_out_ms);
    if (ret != EXT_SUCCESS) {
        gpu_err("wait offline list done timeout\n");
        return ret;
    }
#endif

    return ret;
}

td_void vau_list_isr_offline_callback(td_void)
{
    if (vau_list_get_offline_finish_done(TD_NULL) == TD_FALSE) {
        vau_list_set_offline_finish_done(TD_TRUE);
        osal_wait_wakeup(&g_vau_sw_job_mgr->wait_event);
    }
}

td_void vau_list_isr_callback(td_u32 int_status, td_bool is_error_int)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&g_vau_sw_job_mgr->lock, &lockflags);

    vau_dfx_info *dfx_info = vau_dfx_get_info();
    if (dfx_info->suspend_cnt > dfx_info->resume_cnt) {
        dfx_info->power_err_cnt++;
        gpu_err("gpu has power off, suspend[%d] resume[%d]", dfx_info->suspend_cnt, dfx_info->resume_cnt);
        goto unlock;
    }

    td_bool is_idle_state = vau_hal_check_is_idle();
    td_u32 cur_hw_node    = vau_hal_get_current_node();

    if (is_error_int) {
        gpu_err("error int = %#x!", int_status);
        vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = vau_get_clk_work_scene()};
        vau_hal_init(&scene_info); // LOGIC RESET
    } else {
        vau_list_isr_offline_callback();
    }

    if ((cur_hw_node == 0) || (cur_hw_node == g_vau_sw_job_mgr->worked_node_addr)) {
        gpu_dbg("vau get cur node[%#x] fail", cur_hw_node);
        goto unlock;
    }

    vau_sw_node *sw_node = (vau_sw_node *)(uintptr_t)(cur_hw_node - sizeof(vau_sw_node));
    dfx_info->last_handle  = sw_node->handle;
    td_u32 finished_handle = sw_node->handle;
    vau_handle_mgr *handle_mgr = vau_handle_search(finished_handle);
    if (handle_mgr == TD_NULL) {
        goto unlock;
    }

    if (is_idle_state || is_error_int) {
        g_vau_sw_job_mgr->worked_node_addr = cur_hw_node;
    } else {
        vau_sw_job *sw_job = (vau_sw_job *)handle_mgr->sw_job;
        if (sw_job->list_head.prev == &g_vau_sw_job_mgr->list_head) {
            gpu_dbg("no pre sw_job left, finished handle:%u!", finished_handle);
            goto unlock;
        }

        vau_sw_job *del_job = osal_list_entry(sw_job->list_head.prev, vau_sw_job, list_head);
        finished_handle = del_job->handle;
        g_vau_sw_job_mgr->worked_node_addr = 0x0;
    }

    if (list_comp_delete_job(finished_handle, int_status) == EXT_SUCCESS) {
        list_submit_to_commit_job();
    }

unlock:
    osal_spin_unlock_irqrestore(&g_vau_sw_job_mgr->lock, &lockflags);
}