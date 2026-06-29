/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides hal dma mem alloc\n
 *
 * History: \n
 * 2023-04-20， Create file. \n
 */

#include "hal_dma_mem.h"
#include "common_def.h"
#include "memory/osal_addr.h"

#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
STATIC void *g_hal_dma_lli_node = NULL;
#ifdef CONFIG_DMA_SUPPORT_FLUSH_LLI_NODE_CACHE
STATIC void *g_hal_dma_lli_node_end_addr = NULL;
#endif
STATIC uint8_t g_hal_dma_lli_index[DMA_CHANNEL_MAX_NUM] = {0};
STATIC uint32_t g_hal_dma_lli_node_maxnum = CONFIG_DMA_LLI_NODE_MAX;

/* 消除Wmissing-prototypes编译选项引起的告警 */
void hal_dma_set_lli_node_num(uint32_t val);

void hal_dma_set_lli_node_num(uint32_t val)
{
    g_hal_dma_lli_node_maxnum = val;
}
#elif defined(CONFIG_DMA_LLI_BLOCK_LIST_MEM)
typedef struct hal_dma_block_node {
    uint16_t start; /* 有效数据开始的位置 */
    uint16_t count; /* 有效数据的数量 */
    struct hal_dma_block_node *next_block;
    uint8_t data[];
} hal_dma_block_node_t;

typedef struct hal_dma_block_list {
    hal_dma_block_node_t *head;
    hal_dma_block_node_t *tail;
} hal_dma_block_list_t;

STATIC hal_dma_block_list_t g_hal_dma_block_list[DMA_CHANNEL_MAX_NUM];
#endif

#ifndef CACHE_ALIGNED_SIZE
#define CACHE_ALIGNED_SIZE        32
#endif

/**
 * @brief  Dma mem init.
 * @param  [in]  mem node size.
 * @return The result of dma mem init.
 */
errcode_t hal_dma_mem_init(uint32_t node_size)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_node == NULL) {
        uint32_t size = node_size * DMA_CHANNEL_MAX_NUM * g_hal_dma_lli_node_maxnum;
#ifdef CONFIG_DMA_SUPPORT_FLUSH_LLI_NODE_CACHE
        g_hal_dma_lli_node = osal_kmalloc_align(size, 0, CACHE_ALIGNED_SIZE);
#else
        g_hal_dma_lli_node = osal_kmalloc(size, 0);
#endif
        if (g_hal_dma_lli_node == NULL) {
            return ERRCODE_MALLOC;
        }
#ifdef CONFIG_DMA_SUPPORT_FLUSH_LLI_NODE_CACHE
        g_hal_dma_lli_node_end_addr = (void *)((uintptr_t)g_hal_dma_lli_node + size);
#endif
    }
#else
    unused(node_size);
#endif
    return ERRCODE_SUCC;
}

/**
 * @brief  Dma mem deinit.
 */
void hal_dma_mem_deinit(void)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_node != NULL) {
        osal_kfree(g_hal_dma_lli_node);
        g_hal_dma_lli_node = NULL;
    }
#endif
}

/**
 * @brief  Dma alloc mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
           [in]  mem node size.
 * @return mem node.
 */
void *hal_dma_mem_alloc(dma_channel_t ch, uint32_t node_size)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if ((g_hal_dma_lli_index[ch] >= g_hal_dma_lli_node_maxnum) || (g_hal_dma_lli_node == NULL)) {
        return NULL;
    }
    g_hal_dma_lli_index[ch]++;
    return (uint8_t *)g_hal_dma_lli_node +
        ((uint8_t)ch * g_hal_dma_lli_node_maxnum + g_hal_dma_lli_index[ch] - 1) * node_size;
#elif defined(CONFIG_DMA_LLI_BLOCK_LIST_MEM)
    hal_dma_block_list_t *list = &g_hal_dma_block_list[ch];
    if ((list->head == NULL) || (list->tail->count == CONFIG_DMA_LLI_BLOCK_LIST_SIZE)) {
        /* 没有块或块已满，申请新的块 */
        hal_dma_block_node_t *node = (hal_dma_block_node_t*)osal_kzalloc(
            sizeof(hal_dma_block_node_t) + CONFIG_DMA_LLI_BLOCK_LIST_SIZE * node_size, 0);
        if (node == NULL) {
            return NULL;
        }
        /* 如果没有头，新块就是头 */
        if (list->head == NULL) {
            list->head = node;
        } else {
            /* 如果有头，把新块接到尾上 */
            list->tail->next_block = node;
        }
        /* 新块是尾 */
        list->tail = node;
        node->count = 1;
        return (void*)&node->data[0];
    } else {
        /* 有不满的块，占用一个 */
        hal_dma_block_node_t *node = list->tail;
        void *mem = (void*)&node->data[node->count * node_size];
        node->count++;
        return mem;
    }
#elif defined(CONFIG_DMA_SUPPORT_FLUSH_LLI_NODE_CACHE)
    unused(ch);
    return osal_kmalloc_align(node_size, 0, CACHE_ALIGNED_SIZE);
#else
    unused(ch);
    return osal_kmalloc(node_size, 0);
#endif
}

/**
 * @brief  Dma free mem node.
 * @param  [in]  channel The DMA channel. For details, see @ref dma_channel_t.
 *         [in]  free mem addr.
 */
void hal_dma_mem_free(dma_channel_t ch, void *addr)
{
#ifdef CONFIG_DMA_LLI_NODE_FIX_MEM
    if (g_hal_dma_lli_index[ch] > 0) {
        g_hal_dma_lli_index[ch]--;
    }
    unused(addr);
#elif defined(CONFIG_DMA_LLI_BLOCK_LIST_MEM)
    hal_dma_block_list_t *list = &g_hal_dma_block_list[ch];
    hal_dma_block_node_t *node = list->head;
    while (node != NULL) {
        hal_dma_block_node_t *tmp_node = node;
        node = node->next_block;
        osal_kfree(tmp_node);
    }
    list->head = NULL;
    list->tail = NULL;
    unused(addr);
#else
    unused(ch);
    return osal_kfree(addr);
#endif
}

void hal_dma_mem_flush(void)
{
#if defined(CONFIG_DMA_SUPPORT_FLUSH_LLI_NODE_CACHE) && defined(CONFIG_DMA_LLI_NODE_FIX_MEM)
    ArchDCacheFlushByAddr((uintptr_t)g_hal_dma_lli_node, (uintptr_t)g_hal_dma_lli_node_end_addr);
#else
    return;
#endif
}

