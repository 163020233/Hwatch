/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: audio mem osal
 * Author: audio
 */

#include "securec.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "mem_addr.h"
#include "core_overlay_info.h"

#include "soc_osal.h"
#include "osal_cache.h"

#ifdef BRANDY_PROT
#include "los_memory.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void audio_osal_dcache_region_wb(td_ulong phys_addr, td_void *virt_addr, td_ulong size)
{
#if (SAP_CHIP_TYPE != socmn1)
    osal_dcache_region_wb(virt_addr, phys_addr, size);
#endif
    audio_unused(phys_addr);
    audio_unused(virt_addr);
    audio_unused(size);
}

td_void audio_osal_dcache_region_inv(td_void *addr, td_ulong size)
{
    osal_dcache_region_inv(addr, size);
}

static td_u32 my_log2(td_u32 i)
{
    /* returns n where n = log2(2 ^ n) = log2(2 ^ (n + 1) - 1) */
    td_u32 i_log2 = 0;

    while ((i >> i_log2) > 1) {
        i_log2++;
    }

    return i_log2;
}

td_void *audio_osal_kmalloc(td_ulong size)
{
#ifdef BRANDY_PROT
    return LOS_MemAlloc(m_aucSysMem0, size);
#else
    return osal_kmalloc(size, 0);
#endif
}

/* returns a zeroed memory on success. */
td_void *audio_osal_calloc(td_ulong size)
{
    td_void *mem = TD_NULL;

    mem = audio_osal_kmalloc(size);
    if (mem == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(mem, (size_t)size, 0, (size_t)size);

    return mem;
}

td_void audio_osal_kfree(td_void *addr)
{
#ifdef BRANDY_PROT
    LOS_MemFree(m_aucSysMem0, addr);
#else
    osal_kfree(addr);
#endif
}

td_void *audio_osal_kmalloc_aligned(td_u32 size, td_u32 align_bytes)
{
    td_uintptr_t mask = -1; /* initially set mask to 0xFFFFFFFF */
    td_u8 *ret_buffer = TD_NULL;
    td_void *buffer = TD_NULL;

    if (align_bytes <= 1 || align_bytes > 256) { /* 256 max align bytes */
        return TD_NULL;
    }

    mask <<= my_log2(align_bytes); /* generate mask to clear lsb's */
    if ((align_bytes & (~mask)) != 0) {
        return TD_NULL;
    }

    if (size > align_bytes) {
        if ((size % align_bytes) != 0) {
            return TD_NULL;
        }
    }

    buffer = audio_osal_kmalloc(size + align_bytes);
    if (buffer == TD_NULL) {
        return TD_NULL;
    }

    ret_buffer = (td_u8 *)(td_uintptr_t)(((td_uintptr_t)((td_u8 *)buffer + align_bytes)) & mask);
    ((td_u8 *)ret_buffer)[-1] = (td_u8)((td_u8 *)ret_buffer - (td_u8 *)buffer); /* write offset to new_ptr-1 */

    return (td_void *)ret_buffer;
}

td_void audio_osal_kfree_aligned(td_void *addr)
{
    td_u8 bytes_back;
    td_u8 *real_buffer = (td_u8 *)addr;

    if (real_buffer == TD_NULL) {
        return;
    }

    bytes_back = ((td_u8 *)addr)[-1]; /* get offset to real pointer from -1 position */
    real_buffer -= bytes_back;        /* get original pointer address */

    audio_osal_kfree((td_void *)real_buffer);
}

td_void *audio_osal_malloc_aligned(td_u32 size, td_u32 align_bytes)
{
    td_uintptr_t mask = -1; /* initially set mask to 0xFFFFFFFF */
    td_u8 *ret_buffer = TD_NULL;
    td_void *buffer = TD_NULL;

    if (align_bytes <= 1 || align_bytes > 256) { /* 256 max align bytes */
        return TD_NULL;
    }

    mask <<= my_log2(align_bytes); /* generate mask to clear lsb's */
    if ((align_bytes & (~mask)) != 0) {
        return TD_NULL;
    }

    if (size > align_bytes) {
        if ((size % align_bytes) != 0) {
            return TD_NULL;
        }
    }

    buffer = audio_osal_malloc(size + align_bytes);
    if (buffer == TD_NULL) {
        return TD_NULL;
    }

    ret_buffer = (td_u8 *)(td_uintptr_t)(((td_uintptr_t)((td_u8 *)buffer + align_bytes)) & mask);
    ((td_u8 *)ret_buffer)[-1] = (td_u8)((td_u8 *)ret_buffer - (td_u8 *)buffer); /* write offset to new_ptr-1 */

    return (td_void *)ret_buffer;
}

td_void audio_osal_free_aligned(td_void *addr)
{
    td_u8 bytes_back;
    td_u8 *real_buffer = (td_u8 *)addr;

    if (real_buffer == TD_NULL) {
        return;
    }

    bytes_back = ((td_u8 *)addr)[-1]; /* get offset to real pointer from -1 position */
    real_buffer -= bytes_back;        /* get original pointer address */

    audio_osal_free((td_void *)real_buffer);
}

td_uintptr_t audio_osal_virt_to_phys(td_uintptr_t virt_addr)
{
    if (virt_addr >= SOC_PSRAM_AUDIO_PHYS_ADDR && virt_addr < SOC_MEM_AUDIO_VIRT_ADDR) {
        /* PSRAM audio: 0x8C201000 ~ 0xC8000000 */
        return (td_uintptr_t)(virt_addr - SOC_PSRAM_AUDIO_PHYS_ADDR + SOC_PSRAM_ACORE_PHYS_ADDR);
    } else if (virt_addr >= SOC_MEM_AUDIO_VIRT_ADDR && virt_addr < SOC_MEM_AUDIO_PHYS_ADDR) {
        /* 0xC8000000 ~ 0xE8000000 */
        return (td_uintptr_t)(virt_addr - SOC_MEM_AUDIO_VIRT_ADDR + SOC_MEM_ACORE_PHYS_ADDR);
    } else if (virt_addr >= SOC_MEM_AUDIO_PHYS_ADDR) {
        /* 0xE8000000 ~ 0xFFFFFFFF */
        return (td_uintptr_t)(virt_addr - SOC_MEM_AUDIO_PHYS_ADDR + SOC_MEM_ACORE_PHYS_ADDR);
    } else {
        /* Acore addr: 0x00000000 ~ 0x8C201000 */
        return (td_uintptr_t)virt_addr;
    }
}

td_void *audio_osal_address_map(td_ulong phys_addr, td_ulong size)
{
    audio_unused(size);
    return (td_void *)(td_uintptr_t)phys_addr; /* Acore addr: Directly return */
}

td_void audio_osal_address_unmap(td_ulong phys_addr, td_void *virt_addr)
{
    audio_unused(phys_addr);
    audio_unused(virt_addr);

    return;
}

#define PAGE_SHIFT 9
#define PAGE_SIZE (1 << PAGE_SHIFT)

static OSAL_LIST_HEAD(g_mmz_page_list);

typedef struct {
    td_u32 block;
    td_u32 start_addr;
    td_u32 size;
} mmz_block_info;

typedef struct {
    struct osal_list_head node;
    td_u32 block;
    td_u32 offset;
    td_u32 pages;
} mmz_page;

static const mmz_block_info g_mmz_blks[] = {
    {0x0, MMZ_BLOCK0_START_ADDR, MMZ_BLOCK0_SIZE},
    {0x1, MMZ_BLOCK1_START_ADDR, MMZ_BLOCK1_SIZE},
    {0x2, MMZ_BLOCK2_START_ADDR, MMZ_BLOCK2_SIZE},
    {0x3, MMZ_BLOCK3_START_ADDR, MMZ_BLOCK3_SIZE},
    {0x4, MMZ_BLOCK4_START_ADDR, MMZ_BLOCK4_SIZE},
};

static td_bool mmz_buf_check_overlap(const mmz_page *mmz, td_u32 block, td_u32 offset, td_u32 pages)
{
    if (block != mmz->block) {
        return TD_FALSE;
    }

    if (offset + pages <= mmz->offset) {
        return TD_FALSE;
    }

    if (offset >= mmz->offset + mmz->pages) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static td_bool mmz_check_pages_free(td_u32 block, td_u32 offset, td_u32 pages)
{
    mmz_page *mmz = TD_NULL;

    osal_list_for_each_entry(mmz, &g_mmz_page_list, node) {
        if (mmz_buf_check_overlap(mmz, block, offset, pages)) {
            return TD_FALSE;
        }
    }

    return TD_TRUE;
}

static td_s32 mmz_alloc_pages(td_u32 block, td_u32 offset, td_u32 pages)
{
    mmz_page *mmz = (mmz_page *)audio_osal_kmalloc(sizeof(mmz_page));
    if (mmz == TD_NULL) {
        return -1;
    }

    mmz->block = block;
    mmz->offset = offset;
    mmz->pages = pages;

    osal_list_add_tail(&mmz->node, &g_mmz_page_list);
    return 0;
}

static td_void mmz_free_pages(td_u32 block, td_u32 offset, td_u32 pages)
{
    mmz_page *mmz = TD_NULL;
    mmz_page *tmp = TD_NULL;

    osal_list_for_each_entry_safe(mmz, tmp, &g_mmz_page_list, node) {
        if (mmz->block != block) {
            continue;
        }

        if (mmz->offset != offset) {
            continue;
        }

        if (mmz->pages != pages) {
            continue;
        }

        osal_list_del(&mmz->node);
        audio_osal_kfree(mmz);
        return;
    }
}

/*
 * 返回alloc内存的起始位置：offset
 * 发现没有free空间用于分配时，返回block_pages，该值代表内存块的尾部，不可用
 * 有free空间用于分配时，从前往后，从后往前，寻找到对内存碎片化影响较小的空间，返回对应的offset
 */
static td_u32 mmz_search_pages(td_u32 block, td_u32 block_pages, td_u32 pages)
{
    td_u32 left_offset, right_offset, left_pages, right_pages;
    left_pages = block_pages;
    right_pages = block_pages;

    for (left_offset = 0; left_offset <= block_pages - pages; left_offset++) {
        if (mmz_check_pages_free(block, left_offset, pages)) {
            left_pages = left_offset + pages;
            break;
        }
    }

    if (left_offset > block_pages - pages) {
        return block_pages;
    }

    for (right_offset = block_pages - pages; right_offset >= left_offset; right_offset--) {
        if (mmz_check_pages_free(block, right_offset, pages)) {
            right_pages = block_pages - right_offset;
            break;
        }
    }

    return (left_pages <= right_pages) ? left_offset : right_offset;
}

td_void audio_osal_mmz_map(audio_buffer *mmz_buf)
{
    mmz_buf->fd = -1;
    mmz_buf->virt_addr = (td_u8 *)audio_osal_virt_to_phys((td_uintptr_t)mmz_buf->phys_addr);
}

td_void audio_osal_mmz_unmap(audio_buffer *mmz_buf)
{
    mmz_buf->virt_addr = TD_NULL;
}

td_s32 audio_osal_mmz_alloc(audio_buffer *mmz_buf, td_u32 size)
{
    td_u32 i, offset;
    td_u32 pages, block_pages;
    const td_u32 block_num = (td_u32)(sizeof(g_mmz_blks) / sizeof(g_mmz_blks[0]));
    const mmz_block_info *mmz_blk = TD_NULL;

    if (mmz_buf == TD_NULL) {
        sap_err_log_h32(ERR_SYS_NULL_PTR);
        return ERR_SYS_NULL_PTR;
    }

    mmz_buf->phys_addr = 0LL;
    mmz_buf->virt_addr = TD_NULL;
    mmz_buf->dma_buf = TD_NULL;
    mmz_buf->size = size;
    mmz_buf->cache = TD_FALSE;
    mmz_buf->fd = -1;

    pages = (audio_align_up(size, PAGE_SIZE) >> PAGE_SHIFT);
    for (i = 0; i < block_num; i++) {
        mmz_blk = &g_mmz_blks[i];

        block_pages = (mmz_blk->size >> PAGE_SHIFT);
        if (pages > block_pages) {
            continue;
        }

        /* offset为分配内存的起始位置，不可能在尾部，返回值为block_pages代表在尾部，跳过，寻找下一块 */
        offset = mmz_search_pages(i, block_pages, pages);
        if (offset != block_pages) {
            if (mmz_alloc_pages(i, offset, pages) != 0) {
                sap_err_log_h32(size);
                return ERR_SYS_MEM_OVERLOAD;
            }

            mmz_buf->phys_addr = (td_u64)(mmz_blk->start_addr + (offset << PAGE_SHIFT));
            mmz_buf->virt_addr = (td_u8 *)(td_uintptr_t)mmz_buf->phys_addr;
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(size);
    return ERR_SYS_MEM_OVERLOAD;
}

td_void audio_osal_mmz_release(audio_buffer *mmz_buf)
{
    td_u32 i;
    td_u32 pages, offset;
    td_u32 buf_addr;
    const td_u32 block_num = (td_u32)(sizeof(g_mmz_blks) / sizeof(g_mmz_blks[0]));
    const mmz_block_info *mmz_blk = TD_NULL;

    if (mmz_buf == TD_NULL) {
        return;
    }
    buf_addr = (td_u32)mmz_buf->phys_addr;

    for (i = 0; i < block_num; i++) {
        mmz_blk = &g_mmz_blks[i];
        if ((buf_addr >= mmz_blk->start_addr) && (buf_addr < mmz_blk->start_addr + mmz_blk->size)) {
            break;
        }
    }

    if (i >= block_num) {
        return;
    }

    offset = (buf_addr - mmz_blk->start_addr) >> PAGE_SHIFT;
    pages = (audio_align_up(mmz_buf->size, PAGE_SIZE) >> PAGE_SHIFT);
    mmz_free_pages(mmz_blk->block, offset, pages);

    mmz_buf->phys_addr = 0LL;
    mmz_buf->virt_addr = TD_NULL;
    mmz_buf->dma_buf = TD_NULL;
    mmz_buf->size = 0;
    mmz_buf->fd = -1;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
