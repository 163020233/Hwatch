/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_pic_queue.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_pic_queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void viu_create_queue(viu_pic_queue *pic_queue)
{
    td_ulong flags;

    mcp_osal_spin_lock_init(&pic_queue->spin_lock);

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    /* 初始化各队列头 */
    MCP_OSAL_INIT_LIST_HEAD(&pic_queue->free_list);
    MCP_OSAL_INIT_LIST_HEAD(&pic_queue->busy_list);

    pic_queue->free_num = 0;
    pic_queue->busy_num = 0;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
}

td_void viu_destroy_queue(viu_pic_queue *pic_queue)
{
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    /* 初始化各队列头 */
    MCP_OSAL_INIT_LIST_HEAD(&pic_queue->free_list);
    MCP_OSAL_INIT_LIST_HEAD(&pic_queue->busy_list);

    pic_queue->free_num = 0;
    pic_queue->busy_num = 0;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    mcp_osal_spin_lock_destroy(&pic_queue->spin_lock);
}

struct osal_list_head *viu_queue_get_free(viu_pic_queue *pic_queue)
{
    td_ulong flags;
    struct osal_list_head *plist = TD_NULL;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if (mcp_osal_list_empty(&pic_queue->free_list) != 0) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->free_list.next;

    mcp_osal_list_del(plist);
    pic_queue->free_num--;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

struct osal_list_head *viu_queue_get_busy(viu_pic_queue *pic_queue)
{
    struct osal_list_head *plist = TD_NULL;
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if (mcp_osal_list_empty(&pic_queue->busy_list) != 0) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->busy_list.next;
    mcp_osal_list_del(plist);
    pic_queue->busy_num--;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

struct osal_list_head *viu_queue_get_busy_tail(viu_pic_queue *pic_queue)
{
    td_ulong flags;

    struct osal_list_head *plist = TD_NULL;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if (mcp_osal_list_empty(&pic_queue->busy_list) != 0) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->busy_list.prev;
    if (plist == TD_NULL) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    mcp_osal_list_del(plist);
    pic_queue->busy_num--;
    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

struct osal_list_head *viu_queue_query_busy(viu_pic_queue *pic_queue)
{
    td_ulong flags;
    struct osal_list_head *plist = TD_NULL;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if (mcp_osal_list_empty(&pic_queue->busy_list) != 0) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->busy_list.next;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

struct osal_list_head *viu_queue_query_busy_next(viu_pic_queue *pic_queue)
{
    td_ulong flags;
    struct osal_list_head *plist = TD_NULL;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if ((mcp_osal_list_empty(&pic_queue->busy_list) != 0) ||    /* osal对外是int类型 */
        (pic_queue->busy_num < 2)) { /* busy_num need >= 2 */
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->busy_list.next->next;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

struct osal_list_head *viu_queue_query_busy_tail(viu_pic_queue *pic_queue)
{
    td_ulong flags;
    struct osal_list_head *plist = TD_NULL;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    if (mcp_osal_list_empty(&pic_queue->busy_list) != 0) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    plist = pic_queue->busy_list.prev;
    if (plist == TD_NULL) {
        mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
        return TD_NULL;
    }

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return plist;
}

td_void viu_queue_put_free(viu_pic_queue *pic_queue, struct osal_list_head *plist)
{
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    mcp_osal_list_add_tail(plist, &pic_queue->free_list);
    pic_queue->free_num++;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
}

td_void viu_queue_put_busy(viu_pic_queue *pic_queue, struct osal_list_head *plist)
{
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);

    mcp_osal_list_add_tail(plist, &pic_queue->busy_list);
    pic_queue->busy_num++;

    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);
}

td_u32 viu_queue_get_free_num(viu_pic_queue *pic_queue)
{
    td_u32 free_num;
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);
    free_num = pic_queue->free_num;
    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return free_num;
}

td_u32 viu_queue_get_busy_num(viu_pic_queue *pic_queue)
{
    td_u32 busy_num;
    td_ulong flags;

    mcp_osal_spin_lock_irqsave(&pic_queue->spin_lock, &flags);
    busy_num = pic_queue->busy_num;
    mcp_osal_spin_unlock_irqrestore(&pic_queue->spin_lock, &flags);

    return busy_num;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

