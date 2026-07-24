/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: for seliteos osal adapt.
*/

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_CRYPTO_OSAL_SELITEOS_CRYPTO_OSAL_LIB

static crypto_atomic g_deepsleep_vote[MAX_MODULE_ID] = {0};

void crypto_preprocess(unsigned int module_id)
{
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.open module clock */
        drv_cipher_open_clk(module_id);
        /* 2.lock channel. */
        crypto_drv_resume(module_id);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) < CRYPTO_INT32_MAX) {
        crypto_atomic_inc(&g_deepsleep_vote[module_id]);
    }
}

void crypto_postprocess(unsigned int module_id)
{
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) > 0) {
        crypto_atomic_dec(&g_deepsleep_vote[module_id]);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.close module clock. */
        drv_cipher_close_clk(module_id);
    }
}
