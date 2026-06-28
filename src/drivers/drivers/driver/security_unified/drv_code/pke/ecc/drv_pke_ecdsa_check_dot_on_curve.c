/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke check dot on curve implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

int drv_cipher_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve)
{
    unsigned int klen;
    int ret;

    /* param check. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->x);
    pke_null_ptr_chk(pub_key->y);
    pke_null_ptr_chk(is_on_curve);
    ret = inner_ecdsa_type_check(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid curve_type\n");

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "pub_key->length is invalid\n");

    if (inner_drv_is_zero(pub_key->x, pub_key->length)) {
        *is_on_curve = TD_FALSE;
        return CRYPTO_SUCCESS;
    }

    if (inner_drv_is_zero(pub_key->y, pub_key->length)) {
        *is_on_curve = TD_FALSE;
        return CRYPTO_SUCCESS;
    }

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    *is_on_curve = TD_FALSE;
    ret = hal_pke_alg_ecc_check_dot_on_curve(pub_key, is_on_curve);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "hal_pke_alg_ecc_check_dot_on_curve failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}