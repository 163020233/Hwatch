/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_node_list.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_NODE_LIST_H
#define VIU_NODE_LIST_H

#include "drv_mcp_dependency.h"
#include "mcp_arch_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_MAX_NODE_NUM                3U

typedef struct {
    td_s32 viu_pipe_id; /* PIPE id */
    td_u32 node_id; /* 最后一个节点的ID,不分块就是0，分块的话就是node_cnt-1 */
    td_u64 phy_addr[VIU_MAX_NODE_NUM]; /* node的物理地址 */
    td_void *node[VIU_MAX_NODE_NUM]; /* node的虚拟地址 */
    td_u32 node_cnt; /* 总节点数，不分块就是1，分块的话就是分块的块数 */
    /* 用于计算权重 */
    td_u32 weight;
    struct osal_list_head list;
} viu_node;

typedef struct {
    viu_node *node_buf;
    td_u32 free_num;
    td_u32 viu_offline_vpss_offline_wait_num;
    td_u32 viu_offline_vpss_online_wait_num;
    td_u32 busy_num[ISP_VIPROC_IP_NUM][2]; /* 2 use to ping-pong */
    td_u32 busy_select[ISP_VIPROC_IP_NUM]; /* 每次选择使用的busylist */

    struct osal_list_head free_list;
    struct osal_list_head viu_offline_vpss_offline_wait_list;
    struct osal_list_head viu_offline_vpss_online_wait_list;
    /* 配置给硬件工作的队列，为了实现快速链切换，采用乒乓形式 */
    struct osal_list_head busy_list[ISP_VIPROC_IP_NUM][2]; /* 2 use to ping-pong */

    osal_spinlock spin_lock;
} viu_node_queue;

td_s32 viu_node_create_node_queue(viu_node_queue *node_queue, td_u32 max_task_num);

td_void viu_node_destroy_node_queue(viu_node_queue *node_queue);

viu_node *viu_node_node_queue_get_free(viu_node_queue *node_queue);

td_void viu_node_node_queue_put_busy(viu_node_queue *node_queue, td_u32 viu_proc_id,
    td_u32 working_que_id, viu_node *node);

viu_node *viu_node_node_queue_get_busy(viu_node_queue *node_queue, td_u32 viu_proc_id, td_u32 working_que_id);

td_void viu_node_node_queue_put_viu_offline_vpss_offline_wait(viu_node_queue *node_queue, viu_node *node);
td_void viu_node_node_queue_put_viu_offline_vpss_online_wait(viu_node_queue *node_queue, viu_node *node);

viu_node *viu_node_node_queue_get_viu_offline_vpss_offline_wait(viu_node_queue *node_queue);
viu_node *viu_node_node_queue_get_viu_offline_vpss_online_wait(viu_node_queue *node_queue);

td_void viu_node_node_queue_put_free(viu_node_queue *node_queue, viu_node *node);

td_u32 viu_node_get_viu_offline_vpss_offline_wait_num(viu_node_queue *node_queue);

td_u32 viu_node_get_viu_offline_vpss_online_wait_num(viu_node_queue *node_queue);

td_u32 viu_node_get_busy_num(viu_node_queue *node_queue, td_u32 viu_proc_id, td_u32 working_que_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VIU_NODE_LIST_H */
