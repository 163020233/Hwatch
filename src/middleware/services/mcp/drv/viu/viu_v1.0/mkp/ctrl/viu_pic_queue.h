/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_pic_queue.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_PIC_QUEUE_H
#define VIU_PIC_QUEUE_H

#include "viu_node_list.h"
#include "viu_drv.h"
#include "securec.h"
#include "viu_vicap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void viu_create_queue(viu_pic_queue *pic_queue);
td_void viu_destroy_queue(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_get_free(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_get_busy(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_get_busy_tail(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_query_busy(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_query_busy_next(viu_pic_queue *pic_queue);
struct osal_list_head *viu_queue_query_busy_tail(viu_pic_queue *pic_queue);
td_void viu_queue_put_free(viu_pic_queue *pic_queue, struct osal_list_head *plist);
td_void viu_queue_put_busy(viu_pic_queue *pic_queue, struct osal_list_head *plist);
td_u32 viu_queue_get_free_num(viu_pic_queue *pic_queue);
td_u32 viu_queue_get_busy_num(viu_pic_queue *pic_queue);

#define viu_malloc_pic_queue(pic_queue, max_num, type) \
    ({ \
        td_s32 ret__ = TD_SUCCESS; \
        (pic_queue)->node_buf = mcp_osal_vmalloc((max_num) * sizeof(type)); \
        if ((pic_queue)->node_buf == TD_NULL) { \
            viu_err_trace("create picture queue fail!\n"); \
            ret__ = TD_FAILURE; \
        } \
        ret__; \
    })

#define viu_init_pic_queue(pic_queue, max_num, type) \
    do { \
        td_u32 i; \
        type *pic_node; \
        (td_void)memset_s((pic_queue)->node_buf, ((max_num) * sizeof(type)), 0, ((max_num) * sizeof(type))); \
        viu_create_queue(pic_queue); \
        for (i = 0, pic_node = (type *)(pic_queue)->node_buf; i < (max_num); i++, pic_node++) { \
            viu_pic_queue_put_free(pic_queue, pic_node); \
        } \
    } while (0)

#define viu_create_pic_queue(pic_queue, max_num, type) \
    ({ \
        td_s32 ret_; \
        ret_ = viu_malloc_pic_queue(pic_queue, max_num, type); \
        if (ret_ == TD_SUCCESS) { \
            viu_init_pic_queue(pic_queue, max_num, type); \
        } \
        ret_; \
    })

#define viu_destroy_pic_queue(pic_queue)             \
    do {                                              \
        viu_destroy_queue(pic_queue);               \
        if (TD_NULL != (pic_queue)->node_buf) { \
            mcp_osal_vfree((pic_queue)->node_buf);  \
            (pic_queue)->node_buf = TD_NULL;    \
        }                                           \
    } while (0)

#define viu_pic_queue_get_free(pic_queue, type)                                \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_get_free(pic_queue);                                \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_get_busy(pic_queue, type)                                \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_get_busy(pic_queue);                                \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_get_busy_tail(pic_queue, type)                            \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_get_busy_tail(pic_queue);                            \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_query_busy(pic_queue, type)                              \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_query_busy(pic_queue);                              \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_query_busy_next(pic_queue, type)                          \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_query_busy_next(pic_queue);                          \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_query_busy_tail(pic_queue, type)                          \
    ({                                                                       \
        struct osal_list_head *plist;                                        \
        plist = viu_queue_query_busy_tail(pic_queue);                          \
        (plist == TD_NULL) ? TD_NULL : (mcp_osal_list_entry(plist, type, list)); \
    })

#define viu_pic_queue_put_free(pic_queue, pic_node) (viu_queue_put_free(pic_queue, &(pic_node)->list))
#define viu_pic_queue_put_busy(pic_queue, pic_node) (viu_queue_put_busy(pic_queue, &(pic_node)->list))

#define viu_pic_queue_get_free_num(pic_queue) (viu_queue_get_free_num(pic_queue))
#define viu_pic_queue_get_busy_num(pic_queue) (viu_queue_get_busy_num(pic_queue))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VIU_PIC_QUEUE_H */
