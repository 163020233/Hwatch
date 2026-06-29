/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal trng romable. \n
 *
 * History: \n
 * 2025-06-12, Create file. \n
 */
#include "hal_trng_reg.h"
#include "hal_trng_romable.h"

#include "crypto_drv_common.h"

ROM_SHARE_BSS hal_trng_func_table_t g_trng_table;

bool hal_trng_is_ready_romable(void)
{
    hisc_com_trng_fifo_ready trng_ready;

    trng_ready.u32 = trng_reg_read(HISC_COM_TRNG_FIFO_READY);
    if ((trng_ready.bits).trng_done != TRNG_DONE ||
        (trng_ready.bits).trng_data_ready != TRNG_DATA_READY) {
            return false;
    }
    return true;
}

#define CRYPTO_TRNG_TIMEOUT_IN_US       10000   /* 10ms. */
int32_t hal_trng_get_random_romable(uint32_t *randnum)
{
    uint32_t times = 0;
    uint32_t chk_randnum = 0;

    while (times < CRYPTO_TRNG_TIMEOUT_IN_US) {
        times++;

        if (hal_trng_is_ready_romable() == false) {
            continue;
        }

        *randnum = trng_reg_read(HISC_COM_TRNG_FIFO_DATA);
        chk_randnum = trng_reg_read(HISC_COM_TRNG_FIFO_DATA);
        if ((*randnum != 0x00000000) && (*randnum != 0xffffffff) && (*randnum != chk_randnum)) {
            return CRYPTO_SUCCESS;
        }
    }
    hal_trng_debug_romable();
    return ERROR_GET_TRNG_TIMEOUT;
}

void hal_trng_debug_romable(void)
{
    if (g_trng_table.debug_func != NULL) {
        g_trng_table.debug_func();
    }
}

void hal_trng_register_romable(const hal_trng_func_table_t *func_table)
{
    (void)memcpy_s(&g_trng_table, sizeof(hal_trng_func_table_t), func_table, sizeof(hal_trng_func_table_t));
}