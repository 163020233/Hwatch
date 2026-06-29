/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file for profile module
 * Author: audio
 */

#include <sys/times.h>
#include <xtensa/hal.h>
#include "securec.h"
#include "audio_debug.h"
#include "core_module.h"
#include "core_cps.h"
#include "core_shrb.h"
#include "mem_addr.h"
#include "core_lds_info.h"
#include "core_overlay_info.h"
#include "audio_osal.h"
#ifdef SAP_SUPPORT_RT_SMALL_MEM
#include "rt_smem.h"
#endif
#include "common_profile.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define PROF_U32_MAX 0xFFFFFFFF
#define MCPS_CYCLES_PER_MHZ 1000000

typedef struct {
    td_char name[SEGMENT_NAME_LEN];
    uintptr_t start;
    uintptr_t current;
    uintptr_t end;
} segment_info;

typedef struct {
    td_u32 total;
    td_u32 used;
} mem_info;

static td_void mem_get_static_segment_info(sys_proc_item *proc, td_u32 smem_used)
{
    td_u32 i;
    td_u32 total = 0;
    td_u32 used = 0;
    segment_block *item = proc->static_list;

    td_char total_name[SEGMENT_NAME_LEN] = "total";

    const segment_info seg_tab[] = {
        {"main", (uintptr_t)_memmap_mem_sram_start, (uintptr_t)heap_ptr, (uintptr_t)_memmap_mem_sram_end},
        {"iram", (uintptr_t)_memmap_mem_iram_start, (uintptr_t)_iram_text_end, (uintptr_t)_memmap_mem_iram_end},
#ifndef SAP_AUDIO_DSP_BSS_IN_DRAM_SUPPORT
        {"dram", (uintptr_t)_memmap_mem_dram_start, (uintptr_t)_dram_bss_end + smem_used, (uintptr_t)_memmap_mem_dram_end},
#endif
#ifdef SAP_DSP_LSP_AEF
        {"aef", (uintptr_t)_memmap_mem_aef_start, (uintptr_t)_memmap_mem_aef_end, (uintptr_t)_memmap_mem_aef_end},
#endif
#ifdef SAP_DSP_LSP_CODEC
        {"codec", (uintptr_t)_memmap_mem_codec_start, (uintptr_t)_memmap_mem_codec_end, (uintptr_t)_memmap_mem_codec_end},
#endif
#ifdef SAP_DSP_LSP_SEA
        {"sea", (uintptr_t)_memmap_mem_sea_start, (uintptr_t)_memmap_mem_sea_end, (uintptr_t)_memmap_mem_sea_end},
#endif
        {"share", (uintptr_t)0, (uintptr_t)SOC_SYS_SHM_MAX_SIZE, (uintptr_t)SOC_SYS_SHM_MAX_SIZE},
    };

    for (i = 0; i < sizeof(seg_tab) / sizeof(seg_tab[0]); i++) {
        item->size = (seg_tab[i].end - seg_tab[i].start);
        item->used = (seg_tab[i].current - seg_tab[i].start);
        total += item->size;
        used += item->used;

        (td_void)memcpy_s(item->name, SEGMENT_NAME_LEN, seg_tab[i].name, SEGMENT_NAME_LEN);
        item++;
    }

    /* total */
    (td_void)memcpy_s(item->name, SEGMENT_NAME_LEN, total_name, SEGMENT_NAME_LEN);
    item->size = total;
    item->used = used;
    item++;

    proc->mem_total = total;
    proc->mem_used = used;
    audio_unused(smem_used);
}

static td_void mem_get_smem_info(mem_info *info)
{
#ifdef SAP_SUPPORT_RT_SMALL_MEM
    rt_memory smem_info;
    rt_smem_get_info(&smem_info);

    info->total = smem_info.total;
    info->used = smem_info.used;
#else
    info->total = 0;
    info->used = 0;
#endif
}

#ifdef SAP_MEM_STATISTIC

#define MEM_BLK_NUM  128
typedef struct audio_mem_block {
    td_void *addr;
    td_u32  size;
    td_char name[PROFILE_TAG_LEN];
    struct osal_list_head node;
} audio_mem_block;

static OSAL_LIST_HEAD(g_mem_used_list);
static OSAL_LIST_HEAD(g_mem_free_list);
static audio_mem_block *g_mem_blocks = TD_NULL;

static td_void mem_stat_get_tag(td_char *name, const td_char *tag, td_u32 name_size)
{
    td_u32 name_len;
    td_char *name_sta = TD_NULL;
    td_char *name_end = TD_NULL;

    if (tag != TD_NULL) {
        name_sta = strrchr(tag, '/');
        name_sta = (name_sta == TD_NULL ? (td_char *)tag : name_sta + 1);
        name_end = strrchr(tag, '.');
        name_len = (name_end != TD_NULL ? (td_u32)(name_end - name_sta) : (name_size - 1));
        name_len = sap_min(name_len, name_size);
        (td_void)memcpy_s(name, name_size, name_sta, name_len);
        name[name_len] = '\0';
    } else {
        name[0] = '\0';
    }
}

static td_s32 mem_stat_init(td_void)
{
    td_u32 i, mem_size;
    audio_mem_block *mem_blk = TD_NULL;

    if (g_mem_blocks != TD_NULL) {
        return AUDIO_SUCCESS;
    }

    mem_size = MEM_BLK_NUM * sizeof(audio_mem_block);
    g_mem_blocks = (audio_mem_block *)malloc(mem_size);
    if (g_mem_blocks == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }
    (td_void)memset_s(g_mem_blocks, mem_size, 0, mem_size);

    for (i = 0; i < MEM_BLK_NUM; i++) {
        mem_blk = &g_mem_blocks[i];
        osal_list_add_tail(&mem_blk->node, &g_mem_free_list);
    }

    return AUDIO_SUCCESS;
}

static td_s32 mem_stat_deinit(td_void)
{
    if (g_mem_blocks == TD_NULL) {
        return AUDIO_SUCCESS;
    }

    osal_list_del_init(&g_mem_used_list);
    osal_list_del_init(&g_mem_free_list);

    free(g_mem_blocks);
    g_mem_blocks = TD_NULL;
    return AUDIO_SUCCESS;
}

td_void mem_stat_add_block(td_void *addr, td_u32 size, const td_char *tag)
{
    audio_mem_block *mem_blk = TD_NULL;

    if (osal_list_empty(&g_mem_free_list) != TD_FALSE) {
        sap_err_log_info("free_list is empty.");
        return;
    }

    mem_blk = osal_list_first_entry(&g_mem_free_list, audio_mem_block, node);
    osal_list_del(&mem_blk->node);
    mem_blk->addr = addr;
    mem_blk->size = size;
    mem_stat_get_tag(mem_blk->name, tag, PROFILE_TAG_LEN - 1);
    osal_list_add_tail(&mem_blk->node, &g_mem_used_list);
}

td_void mem_stat_remove_block(td_void *addr)
{
    audio_mem_block *mem_blk = TD_NULL;

    osal_list_for_each_entry(mem_blk, &g_mem_used_list, node) {
        if (mem_blk->addr == addr) {
            osal_list_del(&mem_blk->node);
            mem_blk->addr = TD_NULL;
            mem_blk->size = 0;
            mem_blk->name[0] = '\0';
            osal_list_add_tail(&mem_blk->node, &g_mem_free_list);
            return;
        }
    }

    sap_err_log_info("find block failed.");
}

static td_void mem_get_malloc_info(sys_proc_item *proc)
{
    td_u32 used_size = 0;
    td_u32 malloc_cnt = 0;
    malloc_block *malloc_item = proc->malloc_list;
    audio_mem_block *mem_blk = TD_NULL;

    osal_list_for_each_entry(mem_blk, &g_mem_used_list, node) {
        used_size += (mem_blk->size - (mem_blk->size & 0xf) + 0x10);
        malloc_cnt++;
        if (malloc_cnt > MALLOC_BLOCK_NUM) {
            continue;
        }

        /* 由于IPC log不支持%s格式打印，故本条打印仅在串口打印 */
        audio_log_alert("<MEM: %-16s> addr[0x%08x, 0x%08x], size[%u]", mem_blk->name, mem_blk->addr,
            mem_blk->addr + mem_blk->size, mem_blk->size);
        malloc_item->start_addr = (td_u32)(td_uintptr_t)mem_blk->addr;
        malloc_item->size = mem_blk->size;

        malloc_item++;
    }

    if (malloc_cnt > MALLOC_BLOCK_NUM) {
        sap_err_log_u32(malloc_cnt);
        sap_err_log_u32(MALLOC_BLOCK_NUM);
    } else {
        sap_trace_log_u32(malloc_cnt);
    }

    proc->malloc_used = used_size;
    proc->malloc_cnt = malloc_cnt;
}

/* stack内存的起点 */
extern td_u8 _stack_sentry[];

/* stack内存的末尾，栈由高地址往低地址生长 */
extern td_u8 __stack[];

#define STACK_BLOCK_SIZE 128

static inline td_bool stack_is_unused(const td_u32 *ptr)
{
    return ((*ptr) == STACK_MARKER) ? TD_TRUE : TD_FALSE;
}

static inline td_u32 *stack_find_next_boundary(td_u32 *ptr)
{
    td_u32 *cur_ptr = ptr;
    td_u32 *new_ptr = ptr;
    td_u32 i = 0;

    for (i = 0; i < STACK_BLOCK_SIZE; i++) {
        if (stack_is_unused(cur_ptr) != TD_TRUE) {
            new_ptr = cur_ptr - 1;
        }
        cur_ptr--;
    }

    return sap_max(new_ptr, (td_u32 *)(td_uintptr_t)_stack_sentry);
}

static td_u32 stack_get_used_size(td_void)
{
    td_u32 *low_ptr = (td_u32 *)(td_uintptr_t)_stack_sentry;
    td_u32 *high_ptr = (td_u32 *)(td_uintptr_t)__stack;
    td_u32 *mid_ptr = TD_NULL;
    td_u32 *old_ptr = TD_NULL;
    td_u32 *new_ptr = TD_NULL;

    /* 通过二分查找，先找到大致的分界点 */
    while (low_ptr <= high_ptr) {
        mid_ptr = low_ptr + (high_ptr - low_ptr) / 2; /* 2: middle */
        if (stack_is_unused(mid_ptr) == TD_TRUE && stack_is_unused(mid_ptr + 1) != TD_TRUE) {
            break;
        } else if (stack_is_unused(mid_ptr) != TD_TRUE) {
            high_ptr = mid_ptr - 1;
        } else {
            low_ptr = mid_ptr + 1;
        }
    }

    /* 再往低地址继续遍历，确认最终的分界点 */
    old_ptr = mid_ptr;
    new_ptr = stack_find_next_boundary(mid_ptr);
    while (new_ptr != old_ptr) {
        old_ptr = new_ptr;
        new_ptr = stack_find_next_boundary(new_ptr);
    }
    mid_ptr = new_ptr;

    return ((td_uintptr_t)__stack - (td_uintptr_t)mid_ptr);
}

static td_void mem_get_dynamic_segment_info(sys_proc_item *proc, const mem_info *smem_info)
{
    segment_block *item = proc->dynamic_list;

    td_char heap_name[SEGMENT_NAME_LEN] = "heap";
    td_char smem_name[SEGMENT_NAME_LEN] = "smem";
    td_char stack_name[SEGMENT_NAME_LEN] = "stack";

    /* heap */
    (td_void)memcpy_s(item->name, SEGMENT_NAME_LEN, heap_name, SEGMENT_NAME_LEN);
    item->size = (td_u32)((uintptr_t)_heap_sentry - (uintptr_t)_bss_end);
    item->used = (td_u32)((uintptr_t)heap_ptr - (uintptr_t)_bss_end);
    item++;

    /* smem */
    (td_void)memcpy_s(item->name, SEGMENT_NAME_LEN, smem_name, SEGMENT_NAME_LEN);
    item->size = smem_info->total;
    item->used = smem_info->used;
    item++;

    /* stack */
    (td_void)memcpy_s(item->name, SEGMENT_NAME_LEN, stack_name, SEGMENT_NAME_LEN);
    item->size = (td_uintptr_t)__stack - (td_uintptr_t)_stack_sentry;
    item->used = stack_get_used_size();
    item++;
}
#endif /* SAP_MEM_STATISTIC */

#ifdef SAP_MCPS_STATISTIC
typedef struct {
    td_float total_mcps;

    td_u32 idle_start_time;
    td_u32 idle_stop_time;
    td_u32 idle_cost_time;
    td_float idle_mcps;

    td_u32 msg_start_time;
    td_u32 msg_stop_time;
    td_u32 msg_cost_time;
    td_float msg_mcps;

    td_u32 driver_start_time;
    td_u32 driver_stop_time;
    td_u32 driver_cost_time;
    td_float driver_mcps;
} core_cps_period;

static core_cps_period g_cps_period;

td_void cps_stat_period_proc(td_u32 total_cps, td_u32 interval_ms)
{
    core_cps_period *cps_period = &g_cps_period;

    cps_period->total_mcps = (td_float)(total_cps / MCPS_CYCLES_PER_MHZ);
    cps_period->idle_mcps = (td_float)(cps_period->idle_cost_time) / (AUDIO_US_NUM_PER_MS * interval_ms) *
        cps_period->total_mcps;
    cps_period->msg_mcps = (td_float)(cps_period->msg_cost_time) / (AUDIO_US_NUM_PER_MS * interval_ms) *
        cps_period->total_mcps;
    cps_period->driver_mcps = (td_float)(cps_period->driver_cost_time) / (AUDIO_US_NUM_PER_MS * interval_ms) *
        cps_period->total_mcps;

    cps_period->idle_cost_time = 0;
    cps_period->msg_cost_time = 0;
    cps_period->driver_cost_time = 0;
}

td_void cps_stat_idle_start(td_void)
{
    core_cps_period *cps_period = &g_cps_period;
    cps_period->idle_start_time = audio_osal_gettime_us();
}

td_void cps_stat_idle_stop(td_void)
{
    core_cps_period *cps_period = &g_cps_period;

    cps_period->idle_stop_time = audio_osal_gettime_us();
    cps_period->idle_cost_time += audio_osal_cali_time_diff_us(cps_period->idle_start_time, cps_period->idle_stop_time);
}

td_void cps_stat_msg_start(td_void)
{
    core_cps_period *cps_period = &g_cps_period;
    cps_period->msg_start_time = audio_osal_gettime_us();
}

td_void cps_stat_msg_stop(td_void)
{
    core_cps_period *cps_period = &g_cps_period;

    cps_period->msg_stop_time = audio_osal_gettime_us();
    cps_period->msg_cost_time += audio_osal_cali_time_diff_us(cps_period->msg_start_time, cps_period->msg_stop_time);
}

td_void cps_stat_driver_start(td_void)
{
    core_cps_period *cps_period = &g_cps_period;
    cps_period->driver_start_time = audio_osal_gettime_us();
}

td_void cps_stat_driver_stop(td_void)
{
    core_cps_period *cps_period = &g_cps_period;

    cps_period->driver_stop_time = audio_osal_gettime_us();
    cps_period->driver_cost_time += audio_osal_cali_time_diff_us(cps_period->driver_start_time,
        cps_period->driver_stop_time);
}
#endif /* SAP_MCPS_STATISTIC */

td_void cps_stat_query_total(sys_proc_item *proc)
{
    td_float total_mcps = 0.0;
    td_float idle_mcps = 0.0;
    td_float msg_mcps = 0.0;
    td_float driver_mcps = 0.0;

#ifdef SAP_MCPS_STATISTIC
    core_cps_period *cps_period = &g_cps_period;

    total_mcps = cps_period->total_mcps;
    idle_mcps = cps_period->idle_mcps;
    msg_mcps = cps_period->msg_mcps;
    driver_mcps = cps_period->driver_mcps;
#endif

    proc->total_kcps = (td_u32)(total_mcps * MCPS_TO_KCPS_RATIO);
    proc->idle_kcps = (td_u32)(idle_mcps * MCPS_TO_KCPS_RATIO);
    proc->msg_kcps = (td_u32)(msg_mcps * MCPS_TO_KCPS_RATIO);
    proc->driver_kcps = (td_u32)(driver_mcps * MCPS_TO_KCPS_RATIO);
}

td_void mem_stat_query_detail(sys_proc_item *proc)
{
    mem_info smem_info;

    mem_get_smem_info(&smem_info);

    proc->heap_ptr = (td_u32)(uintptr_t)heap_ptr;
    proc->heap_cached = (td_u32)((uintptr_t)heap_ptr - (uintptr_t)_bss_end);

    mem_get_static_segment_info(proc, smem_info.used);

#ifdef SAP_MEM_STATISTIC
    mem_get_dynamic_segment_info(proc, &smem_info);
    mem_get_malloc_info(proc);
#endif /* SAP_MEM_STATISTIC */
}

td_void shm_stat_query_detail(sys_proc_item *proc)
{
    td_s32 ret;
    td_u32 shm_used = 0;
    td_u32 data_size, info_size;
    td_u32 i, mod_cnt;
    td_u32 mod_ids[SHRB_BUFFER_MAX_NUM] = {0};
    shrb_handle shrb = TD_NULL;
    share_mem_block *item = proc->share_mem_list;

    ret = core_shrb_query_mod_ids(mod_ids, &mod_cnt);
    if (ret != AUDIO_SUCCESS) {
        return;
    }

    /* mmz buf */
    core_shrb_query_size(TD_NULL, &data_size, &info_size);
    item->addr = 0;
    item->data_size = data_size;
    item->info_size = info_size;
    item++;
    shm_used += (data_size + info_size);

    /* share ring buf */
    for (i = 0; i < mod_cnt; i++) {
        ret = core_shrb_get_id(mod_ids[i], &shrb);
        if (ret != AUDIO_SUCCESS) {
            continue;
        }
        ret = core_shrb_query_size(shrb, &data_size, &info_size);
        if (ret != AUDIO_SUCCESS) {
            continue;
        }

        item->addr = mod_ids[i];
        item->data_size = data_size;
        item->info_size = info_size;
        item++;

        shm_used += (data_size + info_size);
    }

    proc->share_mem_total = SOC_SYS_SHM_MAX_SIZE + DFX_SHM_SIZE;
    proc->share_mem_used = shm_used;
}

td_s32 common_profile_init(td_void)
{
#ifdef SAP_MEM_STATISTIC
    td_s32 ret;
    ret = mem_stat_init();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mem_stat_init, ret);
        return ret;
    }
#endif /* SAP_MEM_STATISTIC */
    return AUDIO_SUCCESS;
}

td_void common_profile_deinit(td_void)
{
#ifdef SAP_MEM_STATISTIC
    td_s32 ret;
    ret = mem_stat_deinit();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(mem_stat_deinit, ret);
    }
#endif /* SAP_MEM_STATISTIC */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
