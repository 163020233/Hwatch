/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: mem manage
 */

#include "vau_mem.h"
#include "platform_core.h"
#include "soc_osal.h"
#include "vau_list.h"
#include "vau_handle.h"
#include "vau_dfx.h"
#include "gpu_mem_config.h"

/*-------------------------------- macro define -------------------------------------------*/
#define MEM_SRAM_DRAW_POOL  GPU_SRAM_DRAW_ADDR
#define MEM_SRAM_DRAW_LEN   GPU_SRAM_DRAW_SIZE

#define MEM_SRAM_NODE_ADDR  GPU_SRAM_NODE_ADDR
#define MEM_SRAM_NODE_LEN   GPU_SRAM_NODE_SIZE

#define MEM_PSRAM_NODE_ADDR GPU_PSRAM_NODE_ADDR
#define MEM_PSRAM_NODE_LEN  GPU_PSRAM_NODE_SIZE

#define MEM_PSRAM_POOL      GPU_PSRAM_POOL_ADDR
#define MEM_PSRAM_POOL_LEN  GPU_PSRAM_POOL_SIZE

#define MEM_NODE_ALIGN                   16
#define MEM_HANDLE_ALIGN                 16
#define MEM_PATH_CMD_ALIGN               1
#define MEM_PATH_DATA_ALIGN              16
#define MEM_TESSLATION_ALIGN             64

#define MEM_NODE_MAX_NUM                 48
#define MEM_HANDLE_MAX_NUM               48
#define MEM_PATH_CMD_MAX_NUM             48
#define MEM_PATH_DATA_MAX_NUM            48
#define MEM_TESSLATION_ET_MAX_NUM        1
#define MEM_TESSLATION_LINK_MAX_NUM      1

#define MEM_NODE_HDR_SIZE                (376 * 4)
#ifdef GPU_HDR_SUPPORT
#define MEM_NODE_HDR_MAX_NUM             2
#else
#define MEM_NODE_HDR_MAX_NUM             0
#endif

#ifdef GPU_SDF_SUPPORT
#define MEM_NODE_SDF_MAX_NUM             4
#define MEM_NODE_SDF_SIZE                (1904 * 4)
#endif

#if defined(GPU_PSRAM_SUPPORT) || defined(GPU_DDR_SUPPORT)
#define MEM_NODE_EXT_MAX_NUM             592
#define MEM_TESSLATION_LINK_EXT_MAX_NUM  1
#else
#define MEM_NODE_EXT_MAX_NUM             0
#define MEM_TESSLATION_LINK_EXT_MAX_NUM  0
#endif

#define MEM_PATH_CMD_UNIT_SIZE          MEM_SRAM_DRAW_LEN
#define MEM_PATH_DATA_UNIT_SIZE         MEM_SRAM_DRAW_LEN
#define MEM_TESSLATION_ET_UNIT_SIZE     (64 * 600)

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    struct osal_list_head list_head;
    td_bool is_sram;
    td_uchar *vir_addr;
    td_u32 size;
    td_u32 ref_cnt;
} mem_node;

typedef struct {
    char* mem_name;
    td_bool is_sram;
    td_bool is_draw_mem;
    td_bool is_dynamic_size;
    td_bool is_mem_lock_init;
    td_uchar *vir_addr;
    td_u32 phy_addr;
    td_u32 total_size;
    td_u32 unit_num;
    td_u32 unit_size;
    mem_node *node;
    osal_spinlock mem_lock;
} mem_info;

/*-------------------------------- func declares ------------------------------------------*/
STATIC mem_node *mem_node_search(vau_mem_type mem_type, const td_uchar *vir_addr);

STATIC td_s32 mem_info_init(td_void);
STATIC td_void mem_list_init(td_void);

STATIC td_void mem_info_deinit(td_void);
STATIC td_void mem_list_deinit(td_void);

/*-------------------------------- par define ---------------------------------------------*/

STATIC mem_info g_mem_info[VAU_MEM_TYPE_BUTT] = {
    /* VAU_MEM_TYPE_NODE */
    { .mem_name = "node",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_MAX_NUM,
      .unit_size       = uapi_byte_align((sizeof(vau_sw_node) + sizeof(vau_hardware_node)), MEM_NODE_ALIGN)
    },
    /* VAU_MEM_TYPE_NODE_EXT */
    { .mem_name = "node_ext",
      .is_sram         = TD_FALSE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_EXT_MAX_NUM,
      .unit_size       = uapi_byte_align((sizeof(vau_sw_node) + sizeof(vau_hardware_node)), MEM_NODE_ALIGN)
    },
    /* VAU_MEM_TYPE_NODE_HDR */
    { .mem_name = "node_hdr",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_HDR_MAX_NUM,
      .unit_size       = uapi_byte_align((sizeof(vau_sw_node) + MEM_NODE_HDR_SIZE), MEM_NODE_ALIGN)
    },
#ifdef GPU_SDF_SUPPORT
    /* VAU_MEM_TYPE_NODE_SDF */
    { .mem_name = "node_sdf",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_NODE_SDF_MAX_NUM,
      .unit_size       = uapi_byte_align((sizeof(vau_sw_node) + MEM_NODE_SDF_SIZE), MEM_NODE_ALIGN)
    },
#endif
    /* VAU_MEM_TYPE_HANDLE_MNG */
    { .mem_name = "handle_mng",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_HANDLE_MAX_NUM,
      .unit_size       = uapi_byte_align(sizeof(vau_handle_mgr) + sizeof(vau_sw_job), MEM_HANDLE_ALIGN)
    },
    /* VAU_MEM_TYPE_PATH_CMD */
    { .mem_name = "path_cmd",
      .is_sram         = TD_TRUE,
      .is_draw_mem     = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_PATH_CMD_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_PATH_CMD_UNIT_SIZE, MEM_PATH_CMD_ALIGN)
    },
    /* VAU_MEM_TYPE_PATH_DATA */
    { .mem_name = "path_data",
      .is_sram         = TD_TRUE,
      .is_draw_mem     = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_PATH_DATA_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_PATH_DATA_UNIT_SIZE, MEM_PATH_DATA_ALIGN)
    },
    /* VAU_MEM_TYPE_TESSLATION_ET */
    { .mem_name = "et",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_FALSE,
      .unit_num        = MEM_TESSLATION_ET_MAX_NUM,
      .unit_size       = uapi_byte_align(MEM_TESSLATION_ET_UNIT_SIZE, MEM_TESSLATION_ALIGN)
    },
    /* VAU_MEM_TYPE_TESSLATION_LINK */
    { .mem_name = "et_link",
      .is_sram         = TD_TRUE,
      .is_dynamic_size = TD_TRUE, /* use sram remaining mem */
      .unit_num        = MEM_TESSLATION_LINK_MAX_NUM,
      .unit_size       = 0
    },
    /* VAU_MEM_TYPE_TESSLATION_ET_EXT */
    { .mem_name = "et_ext",
      .is_sram         = TD_FALSE,
      .is_dynamic_size = TD_TRUE, /* use psram remaining mem */
      .unit_num        = MEM_TESSLATION_LINK_EXT_MAX_NUM,
      .unit_size       = 0
    },
};

STATIC td_bool g_mem_init_flag = TD_FALSE;
STATIC struct osal_list_head g_mem_busy_list_head[VAU_MEM_TYPE_BUTT] = {0};
STATIC struct osal_list_head g_mem_free_list_head[VAU_MEM_TYPE_BUTT] = {0};

/*-------------------------------- func release -------------------------------------------*/

/* mocker by ut testcase */
td_u32 vau_mem_get_sram_addr(td_void)
{
    return MEM_SRAM_NODE_ADDR;
}

td_u32 vau_mem_get_psram_addr(td_void)
{
    return MEM_PSRAM_NODE_ADDR;
}

PLT_SEC_ITCM_T td_s32 vau_mem_draw_pool_init(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_init((td_void *)MEM_SRAM_DRAW_POOL, MEM_SRAM_DRAW_LEN);
    if (ret != OSAL_SUCCESS) {
        gpu_err("sram draw mem pool init failed, ret[%#x]!", ret);
        return EXT_FAILURE;
    }

    ret = osal_pool_mem_init((td_void *)MEM_PSRAM_POOL, MEM_PSRAM_POOL_LEN);
    if (ret != OSAL_SUCCESS) {
        gpu_err("psram mem pool init failed, ret[%#x]!", ret);
        osal_pool_mem_deinit((td_void *)MEM_SRAM_DRAW_POOL);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vau_mem_draw_pool_deinit(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_deinit((td_void *)MEM_SRAM_DRAW_POOL);
    if (ret != OSAL_SUCCESS) {
        gpu_err("sram draw mem pool deinit failed, ret[%#x]!", ret);
        return EXT_FAILURE;
    }

    ret = osal_pool_mem_deinit((td_void *)MEM_PSRAM_POOL);
    if (ret != OSAL_SUCCESS) {
        gpu_err("psram mem pool deinit failed, ret[%#x]!", ret);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_uchar *mem_draw_alloc(td_u32 size)
{
    return (td_uchar *)osal_pool_mem_alloc_align((td_void *)MEM_SRAM_DRAW_POOL, size, MEM_PATH_DATA_ALIGN);
}

STATIC td_void mem_draw_free(const td_uchar *vir_addr)
{
    if (vir_addr != TD_NULL) {
        osal_pool_mem_free((td_void *)MEM_SRAM_DRAW_POOL, (td_void *)vir_addr);
    }
}

td_uchar *vau_psram_alloc(td_u32 size)
{
    // 16: align index
    return (td_uchar *)osal_pool_mem_alloc_align((td_void *)MEM_PSRAM_POOL, size, 16);
}

td_void vau_psram_free(const td_uchar *vir_addr)
{
    if (vir_addr != TD_NULL) {
        osal_pool_mem_free((td_void *)MEM_PSRAM_POOL, (td_void *)vir_addr);
    }
}

/* insmod driver ko or system init for once */
td_s32 vau_mem_init(td_void)
{
    td_s32 ret;

    if (g_mem_init_flag == TD_TRUE) {
        return EXT_SUCCESS;
    }

    ret = vau_mem_draw_pool_init();
    if (ret != EXT_SUCCESS) {
        gpu_err("call vau_mem_draw_pool_init failed");
        return ret;
    }

    ret = mem_info_init();
    if (ret != EXT_SUCCESS) {
        vau_mem_draw_pool_deinit();
        mem_info_deinit();
        gpu_err("call mem_info_init failed");
        return ret;
    }

    mem_list_init();

    g_mem_init_flag = TD_TRUE;

    return EXT_SUCCESS;
}

STATIC td_s32 mem_info_init(td_void)
{
    mem_info *info;
    td_u32 addr_start;
    td_u32 free_size;
    td_u32 sram_free_size  = MEM_SRAM_NODE_LEN;
    td_u32 psram_free_size = MEM_PSRAM_NODE_LEN;

    td_u32 sram_start  = vau_mem_get_sram_addr();
    td_u32 psram_start = vau_mem_get_psram_addr();

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        info = &g_mem_info[mem_type];
        free_size = (info->is_sram == TD_TRUE) ? sram_free_size : psram_free_size;

        if (info->is_dynamic_size == TD_TRUE) {
            info->unit_size = free_size;
        }

        if (info->is_draw_mem == TD_TRUE) {
            info->total_size = MEM_SRAM_DRAW_LEN;
            info->phy_addr   = MEM_SRAM_DRAW_POOL;
            info->vir_addr   = (td_uchar *)(uintptr_t)info->phy_addr;
        } else {
            info->total_size = info->unit_size * info->unit_num;
            if (free_size < info->total_size) {
                gpu_err("not enough mem, free_size[%u] need_size[%u]", free_size, info->total_size);
                return EXT_FAILURE;
            }

            if (info->is_sram == TD_TRUE) {
                addr_start = sram_start + (MEM_SRAM_NODE_LEN - free_size);
                sram_free_size = sram_free_size - info->total_size;
            } else {
                addr_start = psram_start + (MEM_PSRAM_NODE_LEN - free_size);
                psram_free_size = psram_free_size - info->total_size;
            }
            info->phy_addr = addr_start;
            info->vir_addr = (td_uchar *)(uintptr_t)info->phy_addr;
        }

        osal_spin_lock_init(&(info->mem_lock));
        info->is_mem_lock_init = TD_TRUE;

        if (info->unit_num != 0) {
            info->node = (mem_node *)osal_kmalloc(info->unit_num * sizeof(mem_node), OSAL_GFP_KERNEL);
            if (info->node == TD_NULL) {
                gpu_err("osal_kmalloc mem node memory failed");
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

STATIC td_void mem_list_init(td_void)
{
    mem_info *info = TD_NULL;

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        OSAL_INIT_LIST_HEAD(&g_mem_busy_list_head[mem_type]);
        OSAL_INIT_LIST_HEAD(&g_mem_free_list_head[mem_type]);

        info = &g_mem_info[mem_type];
        for (td_u32 i = 0; i < info->unit_num; i++) {
            info->node[i].is_sram  = info->is_sram;
            info->node[i].vir_addr = (td_uchar *)((uintptr_t)info->vir_addr + i * info->unit_size);
            info->node[i].size     = info->unit_size;
            info->node[i].ref_cnt  = 0;
            /* 1 -> 2 -> 3 -> N */
            osal_list_add_tail(&(info->node[i].list_head), &g_mem_free_list_head[mem_type]);
        }
    }
    /* merge list */
    osal_list_splice_tail(&g_mem_free_list_head[VAU_MEM_TYPE_NODE_EXT],
                          &g_mem_free_list_head[VAU_MEM_TYPE_NODE]);
}

/* rmmod driver ko or system deinit for once */
td_void vau_mem_deinit(td_void)
{
    if (g_mem_init_flag == TD_FALSE) {
        gpu_err("vau_mem_deinit has been deinit");
        return;
    }

    mem_list_deinit();
    mem_info_deinit();
    vau_mem_draw_pool_deinit();
    g_mem_init_flag = TD_FALSE;

    return;
}

STATIC td_void mem_list_deinit(td_void)
{
    mem_node *tmp  = TD_NULL;
    mem_node *node = TD_NULL;

    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        /* 1 -> 2 -> 3 -> N */
        osal_list_for_each_entry_safe(node, tmp, &g_mem_free_list_head[mem_type], list_head) {
            if (node != TD_NULL) {
                osal_list_del_init(&(node->list_head));
            }
        }
        /* 1 -> 2 -> 3 -> N */
        osal_list_for_each_entry_safe(node, tmp, &g_mem_busy_list_head[mem_type], list_head) {
            if (node != TD_NULL) {
                osal_list_del_init(&(node->list_head));
            }
        }
    }
}

STATIC td_void mem_info_deinit(td_void)
{
    for (td_u32 mem_type = 0; mem_type < (td_u32)VAU_MEM_TYPE_BUTT; mem_type++) {
        if (g_mem_info[mem_type].node != TD_NULL) {
            osal_kfree(g_mem_info[mem_type].node);
            g_mem_info[mem_type].node = TD_NULL;
        }

        if (g_mem_info[mem_type].is_mem_lock_init == TD_TRUE) {
            osal_spin_lock_destroy(&(g_mem_info[mem_type].mem_lock));
            g_mem_info[mem_type].is_mem_lock_init = TD_FALSE;
        }

        g_mem_info[mem_type].vir_addr   = TD_NULL;
        g_mem_info[mem_type].phy_addr   = 0;
        g_mem_info[mem_type].total_size = 0;
    }
}

td_uchar *vau_mem_alloc(vau_mem_type mem_type, td_u32 size)
{
    mem_node *node = TD_NULL;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);

    if (osal_list_empty(&g_mem_free_list_head[mem_type]) != 0) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        gpu_err("mem_free_list[%d] is empty!", mem_type);
        return TD_NULL;
    }

    node = osal_list_entry((&g_mem_free_list_head[mem_type])->next, mem_node, list_head); /* 1 -> 2 -> 3 -> N */
    if ((node == TD_NULL) || (size > node->size)) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        gpu_err("get free list node fail, type[%u], size[%u]!", (td_u32)mem_type, size);
        return TD_NULL;
    }

    if (g_mem_info[mem_type].is_draw_mem == TD_TRUE) {
        node->vir_addr = mem_draw_alloc(size);
        if (node->vir_addr == TD_NULL) {
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            gpu_err("mem_draw_alloc failed, type[%u], size[%u]!", (td_u32)mem_type, size);
            return TD_NULL;
        }
    }

    osal_list_del_init(&(node->list_head));
    node->ref_cnt = 1;
    osal_list_add_tail(&(node->list_head), &g_mem_busy_list_head[mem_type]);

    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return node->vir_addr;
}

td_uchar *vau_mem_realloc(vau_mem_type mem_type, const td_uchar *vir_addr, td_u32 new_size)
{
    /* vg append new path should realloc mem */
    mem_node *node = TD_NULL;
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);

    if (g_mem_info[mem_type].is_draw_mem != TD_TRUE) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        gpu_err("only support cmd/data type realloc, type[%u]!", (td_u32)mem_type);
        return TD_NULL;
    }

    node = mem_node_search(mem_type, vir_addr);
    if (node == TD_NULL) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        gpu_err("vau_mem_node_search failed!");
        return TD_NULL;
    }

    mem_draw_free(node->vir_addr);
    node->vir_addr = mem_draw_alloc(new_size);
    if (node->vir_addr == TD_NULL) {
        osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
        gpu_err("vau_mem_realloc failed, type[%u], size[%u]!", (td_u32)mem_type, new_size);
        return TD_NULL;
    }

    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return node->vir_addr;
}

td_void vau_mem_free(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);
    mem_node *node = mem_node_search(mem_type, vir_addr);

    if (node != TD_NULL) { /* 1 be used only once, so can free */
        if (node->ref_cnt > 1) { /* can not free mem */
            node->ref_cnt--;
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            return;
        }
        if (node->ref_cnt == 0) {
            gpu_err("can not free node\n");
            osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
            return;
        }

        if (g_mem_info[mem_type].is_draw_mem == TD_TRUE) {
            mem_draw_free(node->vir_addr);
        }

        node->ref_cnt = 0;
        osal_list_del_init(&(node->list_head));
        if (node->is_sram == TD_TRUE) { /* mostly malloc sram mem from pre to tail */
            osal_list_add(&(node->list_head), &g_mem_free_list_head[mem_type]);
        } else {
            osal_list_add_tail(&(node->list_head), &g_mem_free_list_head[mem_type]);
        }
    }
    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);

    return;
}

td_s32 vau_mem_round_copy(vau_mem_type mem_type, td_uchar *dst, td_u32 dst_size, const td_uchar *src, td_u32 src_size)
{
    errno_t ret;
    td_uchar *start   = (td_uchar *)(uintptr_t)g_mem_info[mem_type].vir_addr;
    td_uchar *end     = (td_uchar *)((uintptr_t)g_mem_info[mem_type].vir_addr + g_mem_info[mem_type].total_size);
    td_uchar *act_dst = dst;

    if ((mem_type != VAU_MEM_TYPE_PATH_DATA) && (mem_type != VAU_MEM_TYPE_PATH_CMD)) {
        gpu_err("only path support rewind mem be used by vg append new path");
        return EXT_FAILURE;
    }

    act_dst = (act_dst >= end) ? (start + (act_dst - start) % g_mem_info[mem_type].total_size) : (act_dst);

    if (act_dst + src_size > end) {
        td_u32 one_size = end - act_dst;
        ret = memcpy_s(act_dst, one_size, src, one_size);
        if (ret != EOK) {
            gpu_err("call memcpy_s failed\n");
            return EXT_FAILURE;
        }
        ret = memcpy_s(start, dst_size - one_size, (td_uchar *)src + one_size, src_size - one_size);
    } else {
        ret = memcpy_s(act_dst, dst_size, src, src_size);
    }
    if (ret != EOK) {
        gpu_err("call memcpy_s failed\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_u32 vau_mem_free_count(vau_mem_type mem_type)
{
    td_u32 node_num = 0;
    mem_node *tmp = TD_NULL;
    mem_node *node = TD_NULL;

    /* count mem free list has node num */
    osal_list_for_each_entry_safe(node, tmp, &g_mem_free_list_head[mem_type], list_head) {
        node_num++;
    }

    return node_num;
}

td_void vau_mem_ref_inc(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    td_size_t lockflags = 0;

    osal_spin_lock_irqsave(&(g_mem_info[mem_type].mem_lock), &lockflags);
    mem_node *node = mem_node_search(mem_type, vir_addr);
    if (node != TD_NULL) {
        node->ref_cnt++;
    }
    osal_spin_unlock_irqrestore(&(g_mem_info[mem_type].mem_lock), &lockflags);
    return;
}

td_void vau_mem_ref_dec(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    vau_mem_free(mem_type, vir_addr);
}

STATIC mem_node *mem_node_search(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    mem_node *tmp  = TD_NULL;
    mem_node *node = TD_NULL;

    osal_list_for_each_entry_safe(node, tmp, &g_mem_busy_list_head[mem_type], list_head) {
        if ((node != TD_NULL) && (vir_addr == node->vir_addr)) {
            return node;
        }
    }

    gpu_err("search mem node failure, mem_type[%d] vir[%#x]", mem_type, (td_u32)(uintptr_t)vir_addr);
    return TD_NULL;
}

td_bool vau_mem_is_internal(vau_mem_type mem_type, const td_uchar *vir_addr)
{
    /* this mem is from user or from mem pool */
    if (((uintptr_t)vir_addr < (uintptr_t)g_mem_info[mem_type].vir_addr) ||
        ((uintptr_t)vir_addr >= ((uintptr_t)g_mem_info[mem_type].vir_addr + g_mem_info[mem_type].total_size))) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

PLT_SEC_ITCM_T td_void vau_mem_get_range(vau_mem_type mem_type, td_u32 *phy_start, td_u32 *phy_end)
{
    *phy_start = g_mem_info[mem_type].phy_addr;
    *phy_end = *phy_start + g_mem_info[mem_type].total_size;
}

td_void uapi_vau_proc_mem(td_void)
{
    vau_mem_type type;

    gpu_print("=== Memory Layout Info ===\n");

    gpu_print("%-12s 0x%08X - 0x%08X   %7u(B)\n", "sram", GPU_SRAM_START, GPU_SRAM_END,
        GPU_SRAM_END - GPU_SRAM_START);
    gpu_print("%-12s 0x%08X - 0x%08X   %7u(B)\n", "psram_dump", GPU_DUMP_DEBUG_ADDR,
        GPU_DUMP_DEBUG_ADDR + GPU_DUMP_DEBUG_SIZE, GPU_DUMP_DEBUG_SIZE);
    gpu_print("%-12s 0x%08X - 0x%08X   %7u(B)\n", "psram", GPU_PSRAM_ADDR, GPU_PSRAM_ADDR + GPU_PSRAM_SIZE,
        GPU_PSRAM_SIZE);
    gpu_print("%-12s 0x%08X - 0x%08X   %7u(B)\n", "psram_pool", GPU_PSRAM_POOL_ADDR,
        GPU_PSRAM_POOL_ADDR + GPU_PSRAM_POOL_SIZE, GPU_PSRAM_POOL_SIZE);

    gpu_print("===========================\n");
    for (type = 0; type < VAU_MEM_TYPE_BUTT; type++) {
        mem_info *info = &g_mem_info[type];

        td_u32 phy_start = info->phy_addr;
        td_u32 phy_end = info->phy_addr + info->total_size;

        gpu_print("%-12s %-8s 0x%08X - 0x%08X   %7u(B)\n",
            info->mem_name,
            info->is_sram == TD_TRUE ? "sram" : "psram",
            phy_start,
            phy_end,
            info->total_size);
    }
    gpu_print("===========================\n");
}