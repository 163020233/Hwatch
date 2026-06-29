/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: osilist manage
 */
#ifndef DRV_GRAPHIC_VAU_LIST_VAU_LIST_H
#define DRV_GRAPHIC_VAU_LIST_VAU_LIST_H

#include "soc_osal.h"
#include "drv_vau_type.h"
#include "drv_gpu_intf.h"
#include "gpu_crg_intf.h"
#include "vau_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DRV_VAU_JOB_NOTIFY_DEFAULT = 0x0,
    DRV_VAU_JOB_NOTIFY_SYNC    = 0x1,
    DRV_VAU_JOB_NOTIFY_ASYNC   = 0x2,
    DRV_VAU_JOB_NOTIFY_MAX
} drv_vau_notify_mode;

typedef enum {
    DRV_VAU_INT_NONE                  = (0 << 0),
    DRV_VAU_INT_NODE_END              = (1 << 0),
    DRV_VAU_INT_TIMEOUT               = (1 << 1),
    DRV_VAU_INT_BUS_ERROR             = (1 << 2),
    DRV_VAU_INT_LIST_END              = (1 << 3),
    DRV_VAU_INT_DISPLAY_DONE          = (1 << 4),
    DRV_VAU_INT_TUNL_DONE             = (1 << 5),
    DRV_VAU_INT_CONFLICT              = (1 << 6),
    DRV_VAU_INT_DRAW_ERR              = (1 << 7),
    DRV_VAU_INT_ET_OVERSIZE           = (1 << 8),
    DRV_VAU_INT_DRAW_OVERSIZE         = (1 << 9),
#ifdef GPU_1A21
    DRV_VAU_INT_DCMP_CFG_ERR          = (1 << 10),
    DRV_VAU_INT_DCMP_BS_ERR           = (1 << 11),
    DRV_VAU_INT_ONE_DRAW_ERR          = (1 << 12),
    DRV_VAU_INT_ONE_ET_OVERSIZE       = (1 << 13),
    DRV_VAU_INT_ONE_DRAW_OVERSIZE     = (1 << 14),
    DRV_VAU_INT_ONE_DCMP_CFG_ERR      = (1 << 15),
    DRV_VAU_INT_ONE_DCMP_BS_ERR       = (1 << 16),
    DRV_VAU_INT_SUBNODE_END           = (1 << 17),
#else
    DRV_VAU_INT_DRAW_MTRX_CFG_ERR     = (1 << 10),
    DRV_VAU_INT_DCMP_CFG_ERR          = (1 << 11),
    DRV_VAU_INT_DCMP_BS_ERR           = (1 << 12),
    DRV_VAU_INT_ONE_DRAW_ERR          = (1 << 13),
    DRV_VAU_INT_ONE_ET_OVERSIZE       = (1 << 14),
    DRV_VAU_INT_ONE_DRAW_OVERSIZE     = (1 << 15),
    DRV_VAU_INT_ONE_DCMP_CFG_ERR      = (1 << 16),
    DRV_VAU_INT_ONE_DRAW_MTRX_CFG_ERR = (1 << 17),
    DRV_VAU_INT_ONE_DCMP_BS_ERR       = (1 << 18),
    DRV_VAU_INT_SUBNODE_END           = (1 << 19),
#endif
    DRV_VAU_INT_MAX
} vau_int_type;

typedef struct {
    struct osal_list_head list_head;
    td_handle handle;
    td_bool has_submitted;

    td_bool is_wait;
    td_u8 wait_done_count;
    osal_wait wait_event;

    vau_int_type int_type;
    drv_vau_notify_mode notify_type;

    td_u32 node_num;
    vau_sw_node *first_node;
    vau_sw_node *last_node;
} vau_sw_job;

typedef struct {
    struct osal_list_head list_head;
    td_u32 worked_node_addr;
    td_handle last_handle;
    vau_sw_job *last_job;
    vau_sw_job *to_commit_jobs;
    vau_sw_job *committed_jobs;
    osal_spinlock lock;
    osal_wait wait_event;
    volatile td_s32 offline_finish_done;
    vau_clk_scene_info clk_scene_info;
} vau_sw_job_mgr;

td_void vau_set_clk_work_scene(drv_vau_work_scene work_scene);
drv_vau_work_scene vau_get_clk_work_scene(td_void);

td_s32 vau_list_init(td_void);
td_void vau_list_deinit(td_void);

td_s32 vau_list_create(td_handle *handle);
td_s32 vau_list_destroy(td_handle handle);
td_s32 vau_list_blit(td_handle handle, vau_sw_node *sw_node);
td_s32 vau_list_submit(td_handle handle, td_u32 time_out, drv_vau_notify_mode notify_type);

td_void vau_list_free_node(vau_sw_node *node);
td_s32 vau_list_wait_for_done(td_handle handle, td_u32 time_out);
td_s32 vau_list_wait_all_done(td_u32 time_out_ms);
td_void vau_list_isr_callback(td_u32 int_status, td_bool is_error_int);
td_void vau_list_set_offline_finish_done(td_s32 finish_done);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHIC_VAU_LIST_VAU_LIST_H */
