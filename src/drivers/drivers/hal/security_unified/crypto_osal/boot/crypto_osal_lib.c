/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: crypto osal lib header for boot env.
 *
*/

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_CRYPTO_OSAL_BOOT_CRYPTO_OSAL_LIB

void __attribute__((weak)) crypto_udelay(uint32_t useconds)
{
    uapi_tcxo_delay_us(useconds);
}