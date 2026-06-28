/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc common implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

/* rom share bss */
extern const drv_pke_ecc_curve *g_ecc_curve;
extern const pke_ecc_init_param *g_init_param;

int hal_pke_alg_ecc_init_sec(drv_pke_ecc_curve_type curve_type)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;

    ret = inner_pke_alg_resume_sec();
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_pke_alg_resume_sec failed\n");

    ret = hal_pke_alg_ecc_get_param(curve_type, &g_ecc_curve, &g_init_param);
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_unlock, "hal_pke_alg_ecc_get_param failed\n");
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_unlock, "hal_pke_alg_ecc_get_param failed\n");
    random_delay();
    crypto_chk_goto_with_ret(ret, g_ecc_curve == NULL || g_init_param == NULL, error_unlock, CRYPTO_FAILURE,
        "hal_pke_alg_ecc_get_param for ecc_curve failed\n");
    random_delay();
    crypto_chk_goto_with_ret(ret, g_ecc_curve == NULL || g_init_param == NULL, error_unlock, CRYPTO_FAILURE,
        "hal_pke_alg_ecc_get_param for ecc_curve failed\n");

    hal_pke_set_work_len_romable(g_ecc_curve->ksize);

    inner_set_param(g_init_param, g_ecc_curve);

    return CRYPTO_ENHANCE_SUCCESS;

error_unlock:
    inner_pke_alg_suspend_sec();
    g_ecc_curve = NULL;
    g_init_param = NULL;
    return ret;
}

void hal_pke_alg_ecc_deinit_sec(void)
{
    if (g_ecc_curve == NULL) {
        return;
    }
    g_ecc_curve = NULL;
    g_init_param = NULL;
    inner_pke_alg_suspend_sec();
}