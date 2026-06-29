/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#ifndef HAL_DMA_MEM_H
#define HAL_DMA_MEM_H

#include "errcode.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief  Dma mem init.
 * @param  [in]  node_size node size.
 * @return The result of dma mem init.
 */
errcode_t hal_dma_mem_init(uint32_t node_size);

/**
 * @brief  Dma mem deinit.
 */
void hal_dma_mem_deinit(void);

/**
 * @brief  Dma alloc mem node.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t.
 * @param  [in]  node_size mem node size.
 * @return mem node.
 */
void *hal_dma_mem_alloc(dma_channel_t ch, uint32_t node_size);

/**
 * @brief  Dma free mem node.
 * @param  [in]  ch The DMA channel. For details, see @ref dma_channel_t.
 * @param  [in]  addr free mem address.
 */
void hal_dma_mem_free(dma_channel_t ch, void *addr);

/**
 * @brief  Dma mem flush lli node cache.
 */
void hal_dma_mem_flush(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
