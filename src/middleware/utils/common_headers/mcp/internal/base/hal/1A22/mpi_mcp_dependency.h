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
#include "platform_section.h"

#define MCP_ITCM_OPTIMIZED_T   // 修饰函数，可以让函数部署在ITCM提高性能
#define MCP_ITCM_OPTIMIZED_R   // 修饰只读const类型的变量，让其部署在ITCM提供访问性能
// #define MCP_DTCM_OPTIMIZED_D   // 修饰有初值的全局变量，让其部署在DTCM提供访问性能
// #define MCP_DTCM_OPTIMIZED_B   // 修饰无初值的全局变量，让其部署在DTCM提供访问性能

#ifdef MCP_DTCM_OPTIMIZED_B
#define MCP_PLT_SEC_DTCM_B PLT_SEC_DTCM_B // 修饰无初值的全局变量，让其部署在DTCM提供访问性能
#else
#define MCP_PLT_SEC_DTCM_B
#endif

#ifdef MCP_ITCM_OPTIMIZED_T
#define MCP_PLT_SEC_ITCM_T PLT_SEC_ITCM_T // 修饰函数，可以让函数部署在ITCM提高性能
#else
#define MCP_PLT_SEC_ITCM_T
#endif

#ifdef MCP_ITCM_OPTIMIZED_R
#define MCP_PLT_SEC_ITCM_R PLT_SEC_ITCM_R // 修饰只读const类型的变量，让其部署在ITCM提供访问性能
#else
#define MCP_PLT_SEC_ITCM_R
#endif

#ifdef MCP_DTCM_OPTIMIZED_D
#define MCP_PLT_SEC_DTCM_D PLT_SEC_DTCM_D // 修饰有初值的全局变量，让其部署在DTCM提供访问性能
#else
#define MCP_PLT_SEC_DTCM_D
#endif

#define mcp_list_head osal_list_head

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

#define mcp_list_add(new, head) osal_list_add(new, head)

#define mcp_list_add_tail(new, head) osal_list_add_tail(new, head)

#define mcp_list_del(entry) osal_list_del(entry)

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
