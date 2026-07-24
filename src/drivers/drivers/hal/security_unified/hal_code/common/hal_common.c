/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CA_CTRL reg set.
 *
 * Create: 2025-01-20
 */

#include "hal_common.h"
#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_HAL_CODE_COMMON_HAL_COMMON

/* for all trng, different MICRO for different realizement. */
void __attribute__((weak)) hal_sec_open_clk(unsigned int module_id)
{
    crypto_unused(module_id);
    return;
}

void __attribute__((weak)) hal_sec_close_all_clk(void)
{
    return;
}

void __attribute__((weak)) hal_sec_vote_enable(unsigned int module_id)
{
    crypto_unused(module_id);
    return;
}

void __attribute__((weak)) hal_sec_vote_disable(unsigned int module_id)
{
    crypto_unused(module_id);
    return;
}

void __attribute__((weak)) hal_sec_vote(td_bool enable, unsigned int module_id)
{
    crypto_unused(enable);
    crypto_unused(module_id);
    return;
}

void __attribute__((weak)) hal_trng_set_entropy_source(void)
{
    return;
}

void __attribute__((weak)) hal_trng_open_clk(void)
{
    return;
}

void __attribute__((weak)) hal_trng_close_clk(void)
{
    return;
}

crypto_cpu_type __attribute__((weak)) hal_sec_get_cpu_type(void)
{
    return CRYPTO_CPU_TYPE_ACPU;
}

void __attribute__((weak)) get_sec_vote_enable_value(uint8_t *is_sec_vote_disable)
{
    *is_sec_vote_disable = 0;
}