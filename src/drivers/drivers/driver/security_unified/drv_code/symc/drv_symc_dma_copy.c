/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv symc dma copy implement.
 *
 * Create: 2025-05-04
*/
#include "drv_symc.h"
#include "hal_symc_romable.h"

#define FILE_ID FILE_ID_DRV_CODE_SYMC_DRV_SYMC_DMA_COPY

int32_t drv_cipher_symc_dma_copy(uint64_t src_addr, uint64_t dst_addr, uint32_t len)
{
    return hal_symc_dma_romable(src_addr, dst_addr, len);
}

