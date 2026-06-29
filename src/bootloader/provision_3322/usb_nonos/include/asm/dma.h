/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 * Description:  os dma header.
 * Author:
 * Create: 2023-03-09
 */

#ifndef _ROMBOOT_DMA_H
#define _ROMBOOT_DMA_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define ALIGN(addr, boundary) (((addr) + (boundary) - 1) & ~((uintptr_t)((boundary) - 1)))

extern void dma_cache_clean(unsigned int start, unsigned int end);
extern void dma_cache_inv(unsigned int start, unsigned int end);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _ROMBOOT_DMA_H */

