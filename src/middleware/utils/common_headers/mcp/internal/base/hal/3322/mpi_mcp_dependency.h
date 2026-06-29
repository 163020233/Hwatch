/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: viu_adapt_ppu.h
 * Author: AuthorNameMagicTag
 * Create: 2024-03-19
 */

#ifndef MPI_MCP_DEPENDENCY_H
#define MPI_MCP_DEPENDENCY_H

#include "td_type.h"
#include "securec.h"
#include "soc_osal.h"
#include "osal_errno.h"
#include "drv_mcp_errno.h"
#include "drv_mcp_log.h"
#include "drv_mcp_type.h"
#include "drv_mcp_math.h"

struct mcp_list_head {
    struct mcp_list_head *next;
    struct mcp_list_head *prev;
};

td_void list_add_(struct mcp_list_head *new_h, struct mcp_list_head *prev_h, struct mcp_list_head *next_h);
td_void list_del_(struct mcp_list_head *prev, struct mcp_list_head *next);

#define list_entry_(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define list_first_entry_(ptr, type, member) \
    list_entry_((ptr)->next, type, member)

#define list_next_entry_(pos, member) \
    list_entry_((pos)->member.next, __typeof__(*(pos)), member)

#define mcp_list_init_head(list) do { \
    (list)->next = (list);      \
    (list)->prev = (list);      \
} while (0)

#define mcp_list_add(new, head) \
    if ((head)->next != TD_NULL) {          \
        list_add_(new, head, (head)->next); \
    }

#define mcp_list_add_tail(new, head) \
    if ((head)->prev != TD_NULL) {          \
        list_add_(new, (head)->prev, head); \
    }

#define mcp_list_del(entry) \
    if ((entry)->prev != TD_NULL && (entry)->next != TD_NULL) { \
        list_del_((entry)->prev, (entry)->next); \
    }

#define mcp_list_empty(head) \
    ((head)->next == (head))

#define mcp_list_entry(ptr, type, member) \
    list_entry_(ptr, type, member)

#define mcp_list_first_entry(ptr, type, member) \
    list_first_entry_(ptr, type, member)

#define mcp_list_for_each_safe(pos, n, head) \
    for ((pos) = (head)->next, (n) = (pos) != TD_NULL ? (pos)->next : TD_NULL; \
         (pos) != TD_NULL && (pos) != (head); \
         (pos) = (n), (n) = (pos) != TD_NULL ? (pos)->next : TD_NULL)

#define mcp_list_for_each_entry(pos, head, member) \
    for ((pos) = list_first_entry_(head, __typeof__(*(pos)), member); \
         (pos) != TD_NULL && (&(pos)->member != (head)); \
         pos = list_next_entry_(pos, member))

#endif
