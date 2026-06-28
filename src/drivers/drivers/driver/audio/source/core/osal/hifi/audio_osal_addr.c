/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: audio mem osal
 * Author: audio
 */

#include <stdlib.h>
#include <string.h>
#include <xtensa/hal.h>

#include "reg_addr.h"
#include "mem_addr.h"

#include "securec.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "common_profile.h"

#ifdef SAP_SUPPORT_RT_SMALL_MEM
#include "rt_smem.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define HIFI_DATA_CACHE_LINE_SIZE 128

extern td_char *heap_ptr;
extern td_u32 _heap_sentry[];

static td_u32 mem_log2(td_u32 i)
{
    td_u32 log2 = 0;

    while ((i >> log2) > 1) {
        log2++;
    }

    return log2;
}

td_void *audio_osal_kmalloc__(size_t size, const td_char *tag)
{
    td_void *addr = TD_NULL;
    /* max to 512M */
    if (size == 0 || size > SIZE_512M) {
        return TD_NULL;
    }

#ifdef SAP_SUPPORT_RT_SMALL_MEM
    addr = rt_smem_alloc(size);
    if (addr != TD_NULL) {
        goto succ;
    }
#endif

    addr = malloc(size);
    if (addr != TD_NULL) {
        goto succ;
    }

    audio_log2(AUDIO_LOG_LEVEL_ERROR, "malloc size(%zu) failed, cur free:%u\r\n", size,
        (td_u32)((uintptr_t)_heap_sentry - (uintptr_t)heap_ptr));
    return TD_NULL;

succ:
    mem_stat_add_block(addr, size, tag);
    audio_unused(tag);

#ifdef SAP_MEM_STATISTIC
    audio_log2(AUDIO_LOG_LEVEL_WARN, "malloc:addr:0x%08x, size:%u\r\n", addr, size);
#endif

    return addr;
}

td_void *audio_osal_calloc__(size_t size, const td_char *tag)
{
    td_void *addr = TD_NULL;
    /* max to 512M */
    if (size == 0 || size > SIZE_512M) {
        return TD_NULL;
    }

#ifdef SAP_SUPPORT_RT_SMALL_MEM
    addr = rt_smem_calloc(size);
    if (addr != TD_NULL) {
        goto succ;
    }
#endif

    addr = calloc(size, 1);
    if (addr != TD_NULL) {
        goto succ;
    }

    return TD_NULL;
succ:
    mem_stat_add_block(addr, size, tag);
    audio_unused(tag);

#ifdef SAP_MEM_STATISTIC
    audio_log2(AUDIO_LOG_LEVEL_WARN, "calloc:addr:0x%08x, size:%u\r\n", addr, size);
#endif

    return addr;
}

td_void *audio_osal_realloc__(td_void *addr, size_t size, const td_char *tag)
{
    td_void *new_addr = TD_NULL;

    new_addr = realloc(addr, size);

    mem_stat_remove_block(addr);
    mem_stat_add_block(new_addr, size, tag);
    audio_unused(tag);

#ifdef SAP_MEM_STATISTIC
    audio_log2(AUDIO_LOG_LEVEL_WARN, "realloc:addr:0x%08x, size:%u\r\n", addr, size);
#endif

    return new_addr;
}

td_void audio_osal_kfree(td_void *addr)
{
    mem_stat_remove_block(addr);

#ifdef SAP_SUPPORT_RT_SMALL_MEM
    if (rt_smem_verify(addr) == TD_TRUE) {
        rt_smem_free(addr);
        return;
    }
#endif

    free(addr);
}

td_void *audio_osal_kmalloc_aligned__(td_u32 size, td_u32 align, const td_char *tag)
{
    td_uintptr_t mask = -1; /* initially set mask to 0xFFFFFFFF */
    td_u32 temp_size = size;
    td_u32 temp_align = align;
    td_u8 *align_addr = TD_NULL;
    td_void *base_addr = TD_NULL;

    temp_size = audio_align_up(temp_size, HIFI_DATA_CACHE_LINE_SIZE);
    temp_align = audio_align_up(temp_align, HIFI_DATA_CACHE_LINE_SIZE);
    if (temp_align <= 1 || temp_align > 256) { /* 256 max align bytes */
        return TD_NULL;
    }

    mask <<= mem_log2(temp_align); /* generate mask to clear lsb's */
    if (temp_align & (~mask)) {
        return TD_NULL;
    }

    if (temp_size > temp_align) {
        if (temp_size % temp_align) {
            return TD_NULL;
        }
    }

#ifdef SAP_SUPPORT_RT_SMALL_MEM
    base_addr = rt_smem_alloc(temp_size + temp_align);
    if (base_addr != TD_NULL) {
        goto succ;
    }
#endif

    base_addr = malloc(temp_size + temp_align);
    if (base_addr != TD_NULL) {
        goto succ;
    }

    return TD_NULL;

succ:
    mem_stat_add_block(base_addr, temp_size + temp_align, tag);
    audio_unused(tag);

#ifdef SAP_MEM_STATISTIC
    audio_log2(AUDIO_LOG_LEVEL_WARN, "kmalloc_aligned:addr:0x%08x, size:%u\r\n", base_addr, temp_size + temp_align);
#endif

    align_addr = (td_u8 *)TD_NULL + (((td_uintptr_t)((td_u8 *)base_addr + temp_align - (td_u8 *)TD_NULL)) & mask);
    ((td_u8 *)align_addr)[-1] = (td_u8)((td_u8 *)align_addr - (td_u8 *)base_addr); /* write offset to new_ptr-1 */

    return (td_void *)align_addr;
}

td_void audio_osal_kfree_aligned(td_void *addr)
{
    td_void *base_addr = TD_NULL;

    if (addr == TD_NULL) {
        return;
    }

    base_addr = addr - ((td_u8 *)addr)[-1];

    mem_stat_remove_block(base_addr);

#ifdef SAP_SUPPORT_RT_SMALL_MEM
    if (rt_smem_verify(base_addr) == TD_TRUE) {
        rt_smem_free(base_addr);
        return;
    }
#endif

    free(base_addr);
}

static td_bool virt_check(td_ulong addr)
{
    /* 0xC8000000~0xE8000000 */
    return (addr > SOC_MEM_AUDIO_VIRT_ADDR && addr < SOC_MEM_AUDIO_PHYS_ADDR);
}

static td_bool dtcm_check(td_ulong addr)
{
    /* 0xE8058000~0xE807FFFF */
    return (addr >= SOC_MEM_AUDIO_DTCM_REMAP_PHYS_ADDR &&
        addr < (SOC_MEM_AUDIO_DTCM_REMAP_PHYS_ADDR + SOC_MEM_AUDIO_DTCM_SIZE));
}

td_uintptr_t audio_osal_virt_to_phys(td_uintptr_t virt_addr)
{
    if (virt_check(virt_addr)) {
        return virt_addr - SOC_MEM_AUDIO_VIRT_ADDR + SOC_MEM_AUDIO_PHYS_ADDR;
    } else {
        return virt_addr;
    }
}

td_uintptr_t audio_osal_virt_to_logic_phys(td_uintptr_t virt_addr)
{
    if (virt_check(virt_addr)) {
        return (virt_addr - SOC_MEM_AUDIO_VIRT_ADDR + SOC_MEM_AUDIO_PHYS_ADDR);
    } else if (dtcm_check(virt_addr)) {
        /* DSP逻辑内部对DTCM区域重新做了映射，需要获取逻辑访问的真实地址 */
        return (virt_addr - SOC_MEM_AUDIO_DTCM_REMAP_PHYS_ADDR + SOC_MEM_AUDIO_DTCM_LOGIC_PHYS_ADDR);
    } else {
        return virt_addr;
    }
}

td_void *audio_osal_address_map(td_ulong phys_addr, td_ulong size)
{
    if (phys_addr > SOC_OCRAM_AUDIO_PHYS_ADDR && phys_addr < AUDIO_CODEC_BASE_ADDR) {
        /* OCRAM Convert to virtual addr */
        return (td_void *)(td_uintptr_t)(phys_addr - SOC_MEM_AUDIO_PHYS_ADDR + SOC_MEM_AUDIO_VIRT_ADDR);
    } else {
        return (td_void *)(td_uintptr_t)phys_addr; /* Reg addr: Directly return */
    }
    audio_unused(size);
}

td_void audio_osal_mmz_map(audio_buffer *mmz_buf)
{
    td_u8 *virt_addr = TD_NULL;

    if (mmz_buf->phys_addr >= SOC_MEM_ACORE_PHYS_ADDR &&
        mmz_buf->phys_addr < (SOC_MEM_ACORE_PHYS_ADDR + SOC_MEM_MAP_SIZE)) {
        /* Acore audio: 0x58000000 ~ 0x6C201000 */
        virt_addr = (td_u8 *)(td_uintptr_t)(mmz_buf->phys_addr - SOC_MEM_ACORE_PHYS_ADDR + SOC_MEM_AUDIO_PHYS_ADDR);
    } else if (mmz_buf->phys_addr >= SOC_PSRAM_ACORE_PHYS_ADDR && mmz_buf->phys_addr < SOC_SHM_ACORE_PHYS_ADDR) {
        /* 0x6C201000 ~ 0x87000000 */
        virt_addr = (td_u8 *)(td_uintptr_t)(mmz_buf->phys_addr - SOC_PSRAM_ACORE_PHYS_ADDR + SOC_PSRAM_AUDIO_PHYS_ADDR);
    } else {
        /* Acore reg: 0x00000000 ~ 0x58000000 */
        /* 0x87000000 ~ 0xFFFFFFFF */
        virt_addr = (td_u8 *)(td_uintptr_t)(mmz_buf->phys_addr);
    }

    mmz_buf->virt_addr = virt_addr;
}

td_void audio_osal_mmz_unmap(audio_buffer *mmz_buf)
{
    mmz_buf->virt_addr = TD_NULL;
}

td_s32 audio_osal_mmz_alloc(audio_buffer *mmz_buf, td_u32 size)
{
    td_void *phys_addr = TD_NULL;

    if (mmz_buf == TD_NULL) {
        return -1;
    }

    phys_addr = audio_osal_kmalloc_aligned(size, HIFI_DATA_CACHE_LINE_SIZE);
    if (phys_addr == TD_NULL) {
        return -1;
    }

    mmz_buf->phys_addr = (td_uintptr_t)phys_addr;
    mmz_buf->virt_addr = audio_osal_address_map((td_ulong)(td_uintptr_t)phys_addr, size);
    mmz_buf->dma_buf = TD_NULL;
    mmz_buf->size = size;
    mmz_buf->cache = TD_FALSE;
    mmz_buf->fd = -1;

    return AUDIO_SUCCESS;
}

td_void audio_osal_mmz_release(audio_buffer *mmz_buf)
{
    td_void *phys_addr = TD_NULL;

    if (mmz_buf == TD_NULL || mmz_buf->virt_addr == TD_NULL) {
        return;
    }

    phys_addr = (td_void *)audio_osal_virt_to_phys((td_uintptr_t)mmz_buf->virt_addr);
    audio_osal_kfree_aligned(phys_addr);
    mmz_buf->phys_addr = 0;
    mmz_buf->virt_addr = TD_NULL;
    mmz_buf->size = 0;
    mmz_buf->fd = -1;
}

td_void audio_osal_dcache_region_wb(td_ulong phys_addr, td_void *virt_addr, td_ulong size)
{
    audio_unused(phys_addr);

    if (dtcm_check((td_uintptr_t)virt_addr) == TD_TRUE) {
        return;
    }

    xthal_dcache_region_writeback(virt_addr, size);
}

td_void audio_osal_dcache_region_inv(td_void *addr, td_ulong size)
{
    if (dtcm_check((td_uintptr_t)addr) == TD_TRUE) {
        return;
    }

    xthal_dcache_region_invalidate(addr, size);
}

td_void audio_osal_icache_region_inv(td_void *addr, td_ulong size)
{
    xthal_icache_region_invalidate(addr, size);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
