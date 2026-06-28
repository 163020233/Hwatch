/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke sm2 verify implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define SM2_KEY_SIZE        32
/*
 * According to <SM2 Elliptic Curve Public Key cryptographic algorithm>
 * Section 7.1. Digital Signature Verification Algorithm.
 */
static int inner_sm2_verify(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int ret;
    unsigned char t_buf[SM2_KEY_SIZE];
    unsigned char x_buf[SM2_KEY_SIZE];
    drv_pke_data r_data = {
        .data = sig->r,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data s_data = {
        .data = sig->s,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data t_data = {
        .data = t_buf,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data x_data = {
        .data = x_buf,
        .length = SM2_KEY_SIZE
    };
    const unsigned char *sm2_n = hal_pke_alg_ecc_get_n();

    /* Step B1. check if r is in [1, n - 1] */
    crypto_chk_return(inner_drv_is_in_range(sig->r, sm2_n, SM2_KEY_SIZE) == TD_FALSE,
        CRYPTO_FAILURE, "sig->r is not in [1, n - 1]!\n");
    /* Step B2. check if s is in [1, n - 1] */
    crypto_chk_return(inner_drv_is_in_range(sig->s, sm2_n, SM2_KEY_SIZE) == TD_FALSE,
        CRYPTO_FAILURE, "sig->s is not in [1, n - 1]!\n");
    /* Step B3-B4. compute H(ZA || M), we can skip. */
    /* Step B5. compute t = (r + s) mod n, if t = 0, return failure. */
    crypto_pke_print("========== Start Compute t = (r + s) mod n. ==========\n");
    crypto_pke_dump_data("r", r_data.data, r_data.length);
    crypto_pke_dump_data("s", s_data.data, s_data.length);

    ret = hal_pke_alg_sm2_add_mod(&r_data, &s_data, &t_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_sm2_add_mod failed\n");

    crypto_pke_dump_data("t", t_data.data, t_data.length);
    crypto_pke_print("========== End Compute t = (r + s) mod n. ==========\n");
    /* Step B6. compute X(x, y) = s * G + t * P. */
    crypto_pke_print("========== Start Compute X(x, y) = s * G + t * P. ==========\n");
    crypto_pke_dump_data("s", s_data.data, s_data.length);
    crypto_pke_dump_data("t", t_data.data, t_data.length);
    crypto_pke_dump_data("P(x)", pub_key->x, pub_key->length);
    crypto_pke_dump_data("P(y)", pub_key->y, pub_key->length);

    ret = hal_pke_alg_ecc_compute_x(&s_data, &t_data, pub_key, &x_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_compute_x failed\n");

    crypto_pke_dump_data("X(x)", x_data.data, x_data.length);
    crypto_pke_print("========== End Compute X(x, y) = s * G + t * P. ==========\n");
    /* Step B7. compute r = (e + x) mod n, check if r = sig->r, return success, else return failure. */
    crypto_pke_print("========== Start Compute r = (e + x) mod n. ==========\n");
    crypto_pke_dump_data("e", hash->data, hash->length);
    crypto_pke_dump_data("X(x)", x_data.data, x_data.length);

    ret = hal_pke_alg_sm2_add_mod(hash, &x_data, &t_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_sm2_add_mod failed\n");

    crypto_pke_dump_data("r", t_data.data, t_data.length);
    crypto_pke_print("========== End Compute r = (e + x) mod n. ==========\n");
    if (memcmp(t_data.data, sig->r, SM2_KEY_SIZE) == 0) {
        return CRYPTO_SUCCESS;
    }

    crypto_log_err("compute r is invalid!\n");
    return CRYPTO_FAILURE;
}

int drv_cipher_pke_sm2_verify(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int ret;
    unsigned int klen = DRV_PKE_LEN_256;
    drv_pke_data hash_data;

    /* param check. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->x);
    pke_null_ptr_chk(pub_key->y);
    pke_null_ptr_chk(hash);
    pke_null_ptr_chk(hash->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);

    /* length check. */
    crypto_chk_return(pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "pub_key->length is invalid\n");
    crypto_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");
#if defined(CONFIG_PKE_ECC_SECURITY_STRENGTH_CHECK)
    /* increase security strength check. */
    drv_crypto_pke_check_param(hash->length < klen);
#endif
    ret = hal_pke_alg_ecc_init(DRV_PKE_ECC_TYPE_SM2);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    hash_data.data = hash->data;
    hash_data.length = crypto_min(SM2_KEY_SIZE, hash->length);
    ret = inner_sm2_verify(pub_key, &hash_data, sig);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_sm2_verify failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}