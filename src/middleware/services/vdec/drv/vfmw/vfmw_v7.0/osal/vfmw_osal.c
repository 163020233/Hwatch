/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_osal.h"
#include "vfmw_proc.h"
#include "dbg.h"

#if defined(ENV_ARMLINUX_KERNEL)
#include "linux_osal.h"
#else
#include "liteos_osal.h"
#endif

vfmw_osal_ops g_vfmw_osal_ops;

#define OS_ALLOC_VIR_MEM g_vfmw_osal_ops.alloc_vir_mem
#define OS_FREE_VIR_MEM  g_vfmw_osal_ops.free_vir_mem

#ifdef VIR_ARRAY_SUPPORT
#define MAX_VIR_NAME (20)
#define MAX_VIR_NODE (1000)

typedef struct {
    td_bool is_used;
    td_void *vir_addr;
    td_s32 size;
    td_u8 name[MAX_VIR_NAME];
} os_vir_node;

static td_s32 g_total_num = 0;
static td_s32 g_total_size = 0;
static os_vir_node g_vir_array[MAX_VIR_NODE];
static os_sema g_vir_sem;

td_void os_init_vir_array(td_void)
{
    g_total_num = 0;
    g_total_size = 0;
    (void)memset_s(g_vir_array, sizeof(g_vir_array), 0, sizeof(g_vir_array));
    os_sema_init(&g_vir_sem);
}

td_s32 os_add_vir_node(const td_u8 *name, td_void *vir_addr, td_s32 size)
{
    td_u32 i;
    td_s32 ret = OSAL_ERR;
    os_vir_node *node = TD_NULL;

    os_sema_down(g_vir_sem);

    for (i = 0; i < MAX_VIR_NODE; i++) {
        node = &(g_vir_array[i]);
        if (node->is_used == TD_FALSE) {
            node->is_used = TD_TRUE;
            node->vir_addr = vir_addr;
            node->size = size;
            if (snprintf_s(node->name, MAX_VIR_NAME, MAX_VIR_NAME - 1, name) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            node->name[MAX_VIR_NAME - 1] = '\0';
            g_total_num++;
            g_total_size += size;
            ret = OSAL_OK;
            break;
        }
    }

    os_sema_up(g_vir_sem);

    return ret;
}

td_s32 os_del_vir_node(td_void *vir_addr)
{
    td_u32 i;
    td_s32 ret = OSAL_ERR;
    os_vir_node *node = TD_NULL;

    os_sema_down(g_vir_sem);

    for (i = 0; i < MAX_VIR_NODE; i++) {
        node = &(g_vir_array[i]);
        if (node->is_used == TD_TRUE && node->vir_addr == vir_addr) {
            node->is_used = TD_FALSE;
            g_total_num = (g_total_num > 0) ? g_total_num - 1 : 0;
            g_total_size = (g_total_size - node->size >= 0) ? (g_total_size - node->size) : 0;
            ret = OSAL_OK;
            break;
        }
    }

    os_sema_up(g_vir_sem);

    return ret;
}

td_void os_find_used_node(os_vir_node **pp_node, td_u32 *idx)
{
    td_u32 i;
    td_u32 pos = *idx;
    os_vir_node *node = TD_NULL;

    *pp_node = TD_NULL;
    for (i = pos; i < MAX_VIR_NODE; i++) {
        node = &g_vir_array[i];
        if (node->is_used == TD_TRUE) {
            *pp_node = node;
            *idx = i + 1;
            break;
        }
    }
}

td_void os_read_vir_array(td_void *buf, td_s32 len, td_s32 *used)
{
    td_u32 idx = 0;
    os_vir_node *node0 = TD_NULL;
    os_vir_node *node1 = TD_NULL;

    os_sema_down(g_vir_sem);

    os_dump_proc(buf, len, used, 0, LOG_FMT_HEAD, "VIR");

    do {
        os_find_used_node(&node0, &idx);
        if (node0 == TD_NULL) {
            break;
        }
        os_find_used_node(&node1, &idx);
        if (node1 == TD_NULL) {
            os_dump_proc(buf, len, used, 0, LOG_FMT_D, node0->name, node0->size);
            break;
        }

        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, node0->name, node0->size, node1->name, node1->size);
    } while (node1 != TD_NULL);

    os_dump_proc(buf, len, used, 0, "  Total num: %d / Total size: %d (%dKB)\n", g_total_num, g_total_size,
                 (g_total_size / 1024)); /* 1024 :a number */
    os_dump_proc(buf, len, used, 0, "\n");

    os_sema_up(g_vir_sem);
}
#endif

STATIC td_void os_read_vir_mem(td_void *buf, td_s32 len, td_s32 *used)
{
#ifdef VIR_ARRAY_SUPPORT
    os_read_vir_array(buf, len, used);
#else
    vfmw_unused(buf);
    vfmw_unused(len);
    vfmw_unused(used);
#endif
}

td_void os_intf_exit(td_void)
{
    osal_intf_exit(&g_vfmw_osal_ops);
}

STATIC td_void *os_alloc_vir_mem(td_s32 chan_id, const td_char *name, td_s32 size)
{
    td_void *vir_addr = TD_NULL;

    vfmw_check_ret(name != TD_NULL, TD_NULL);
    vfmw_check_ret_print(size > 0, TD_NULL, "Alloc vir %s failed of size %d invalid!\n", name, size);

    if (OS_ALLOC_VIR_MEM == TD_NULL) {
        return TD_NULL;
    }

    vir_addr = OS_ALLOC_VIR_MEM(chan_id, name, size);
    if (vir_addr == TD_NULL) {
        return TD_NULL;
    }

#ifdef VIR_ARRAY_SUPPORT
    if (os_add_vir_node(name, vir_addr, size) != OSAL_OK) {
        dprint(PRN_ERROR, "Alloc vir %s failed of vir array is full\n", name);
        if (OS_FREE_VIR_MEM != TD_NULL) {
            OS_FREE_VIR_MEM(vir_addr);
        }
        return TD_NULL;
    }
#endif

    return vir_addr;
}

STATIC td_void os_free_vir_mem(td_void *vir_addr)
{
#ifdef VIR_ARRAY_SUPPORT
    if (os_del_vir_node(vir_addr) == OSAL_ERR) {
        dprint(PRN_ERROR, "%s del vir node error!\n", __func__);
    }
#endif

    if (OS_FREE_VIR_MEM != TD_NULL) {
        OS_FREE_VIR_MEM(vir_addr);
    }
}

td_void list_add_(struct os_list_head *new_h, struct os_list_head *prev_h, struct os_list_head *next_h)
{
    if (new_h != TD_NULL && prev_h != TD_NULL && next_h != TD_NULL) {
        next_h->prev = new_h;
        new_h->next = next_h;
        new_h->prev = prev_h;
        prev_h->next = new_h;
    }
}

td_void list_del_(struct os_list_head *prev, struct os_list_head *next)
{
    if (next != TD_NULL && prev != TD_NULL) {
        next->prev = prev;
        prev->next = next;
    }
}

td_void os_intf_init(td_void)
{
    osal_intf_init(&g_vfmw_osal_ops);
    g_vfmw_osal_ops.read_osal = os_read_vir_mem;
    g_vfmw_osal_ops.alloc_vir = os_alloc_vir_mem;
    g_vfmw_osal_ops.free_vir = os_free_vir_mem;
#ifdef VIR_ARRAY_SUPPORT
    os_init_vir_array();
#endif
}
