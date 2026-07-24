/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for osal adapt.
 *
 * Create: 2023-04-21
*/

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_CRYPTO_OSAL_LITEOS_CRYPTO_OSAL_LIB

static crypto_atomic g_deepsleep_vote[MAX_MODULE_ID] = {0};
static const crypto_table_item g_module_id_table[] = {
    {KM_MODULE_ID, PM_SECURITY_KM_VETO_ID},
    {SPACC_KM_MODULE_ID, PM_SECURITY_KM_VETO_ID},  /* km send key to spacc. */
    {PKE_MODULE_ID, PM_SECURITY_PKE_VETO_ID},
    {TRNG_MODULE_ID, PM_SECURITY_TRNG_VETO_ID},
    {SYMC_MODULE_ID, PM_SECURITY_SYMC_VETO_ID},
    {HASH_MODULE_ID, PM_SECURITY_HASH_VETO_ID},
    {CRC_MODULE_ID, PM_SECURITY_SYMC_VETO_ID},  /* crc use symc chn. */
    {PKE_HASH_MODULE_ID, PM_SECURITY_PKE_VETO_ID},
};

/* use drv api, optimize it later. */
int32_t __attribute__((weak)) crypto_drv_mod_resume(void)
{
    return CRYPTO_SUCCESS;
}

void __attribute__((weak)) crypto_preprocess(unsigned int module_id)
{
    unsigned int ds_vote_id = 0;
    crypto_drv_mod_resume();
    crypto_get_value_by_index(g_module_id_table, crypto_array_size(g_module_id_table), module_id, &ds_vote_id);
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.first vote to not sleep */
        uapi_pm_add_sleep_veto(ds_vote_id);
        /* 2.open module clock */
        drv_cipher_open_clk(module_id);
        /* 3.lock channel. */
        crypto_drv_resume(module_id);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) < CRYPTO_INT32_MAX) {
        crypto_atomic_inc(&g_deepsleep_vote[module_id]);
    }
}

void __attribute__((weak)) crypto_postprocess(unsigned int module_id)
{
    unsigned int ds_vote_id = 0;
    crypto_get_value_by_index(g_module_id_table, crypto_array_size(g_module_id_table), module_id, &ds_vote_id);
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) > 0) {
        crypto_atomic_dec(&g_deepsleep_vote[module_id]);
    }
    if (crypto_atomic_read(&g_deepsleep_vote[module_id]) == 0) {
        /* 1.close module clock. */
        drv_cipher_close_clk(module_id);
        /* 2.vote to sleep. */
        uapi_pm_remove_sleep_veto(ds_vote_id);
    }
}
