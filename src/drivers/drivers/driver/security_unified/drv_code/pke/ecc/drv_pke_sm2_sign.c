/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke sm2 sign implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_SIGN

#define MAX_TRY_TIMES   100
#define SM2_KEY_SIZE        32

/* According to SM2 Elliptic Curve Public Key cryptographic algorithm.
 * "Section 6.1. Digital Signature Generation Algorithm"
 */
static int inner_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int ret;
    unsigned char k[SM2_KEY_SIZE];
    unsigned char kg_buf[SM2_KEY_SIZE * 2];
    drv_pke_data k_data = {
        .data = k,
        .length = SM2_KEY_SIZE
    };
    drv_pke_ecc_point kg_point = {
        .x = kg_buf,
        .y = kg_buf + SM2_KEY_SIZE,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data r_data = {
        .data = sig->r,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data x1_data = {
        .data = kg_point.x,
        .length = SM2_KEY_SIZE
    };
    drv_pke_data s_data = {
        .data = sig->s,
        .length = SM2_KEY_SIZE
    };
    const unsigned char *sm2_n = hal_pke_alg_ecc_get_n();

    /* Step A1~A2: compute e = H(ZA || M), we can skip. */
    /* Step A3. generate k in [1, n - 1] */
    ret = inner_get_limit_random(k, sm2_n, priv_key->length);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_limit_random failed\n");

    crypto_pke_dump_data("random k", k, priv_key->length);
    /* Step A4. compute k * G. */
    crypto_pke_print("========== Start Compute kg = k * G ==========\n");
    crypto_pke_dump_data("k", k, priv_key->length);

    ret = hal_pke_alg_ecc_base_point_mul(&k_data, &kg_point);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_base_point_mul failed\n");
    crypto_pke_dump_data("kg(x)", kg_point.x, kg_point.length);
    crypto_pke_dump_data("kg(y)", kg_point.y, kg_point.length);
    crypto_pke_print("========== End Compute kg = k * G ==========\n");
    /* Step A5. compute r = (e + x1) mod n, x1 is kg(x), if r = 0 or r + k = n, continue. */
    crypto_pke_print("========== Start Compute r = (e + x1) mod n ==========\n");
    crypto_pke_dump_data("e", hash->data, hash->length);
    crypto_pke_dump_data("x1", x1_data.data, x1_data.length);

    ret = hal_pke_alg_sm2_add_mod(hash, &x1_data, &r_data);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_sm2_add_mod failed\n");

    crypto_pke_dump_data("r", r_data.data, r_data.length);
    crypto_pke_print("========== End Compute r = (e + x1) mod n==========\n");
    /* check if r is zero. */
    drv_err_chk_return(inner_drv_is_zero(r_data.data, r_data.length), PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO),
        "r is zero\n");

    /* check if (r + k) mod n is zero, which means r + k = n. */
    ret = hal_pke_alg_sm2_add_mod(&r_data, &k_data, &s_data);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_sm2_add_mod failed\n");
    drv_err_chk_return(inner_drv_is_zero(s_data.data, s_data.length), PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO),
        "r + k = n\n");

    /* Step A6. compute s = (((1 + d)^-1) * (k - r * d)) mod n. */
    crypto_pke_print("========== Start Compute s = (((1 + d)^-1) * (k - r * d)) mod n ==========\n");
    crypto_pke_dump_data("d", priv_key->data, priv_key->length);
    crypto_pke_dump_data("k", k_data.data, k_data.length);
    crypto_pke_dump_data("r", r_data.data, r_data.length);

    ret = hal_pke_alg_sm2_compute_s(priv_key, &k_data, &r_data, &s_data);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_sm2_compute_s failed\n");
    drv_err_chk_return(inner_drv_is_zero(s_data.data, s_data.length), PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO),
        "s is zero\n");

    crypto_pke_dump_data("s", s_data.data, s_data.length);
    crypto_pke_print("========== End Compute s = (((1 + d)^-1) * (k - r * d)) mod n==========\n");

    ret = CRYPTO_SUCCESS;
    (void)memset_s(k, SM2_KEY_SIZE, 0, SM2_KEY_SIZE);
    return ret;
}

int drv_cipher_pke_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int ret;
    unsigned int i;
    unsigned int klen = DRV_PKE_LEN_256;
    drv_pke_data hash_data;

    /* param check. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->data);
    pke_null_ptr_chk(hash);
    pke_null_ptr_chk(hash->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);

    /* length check. */
    drv_err_chk_return(priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "priv_key->length is invalid\n");
    drv_err_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");
#if defined(CONFIG_PKE_ECC_SECURITY_STRENGTH_CHECK)
    /* increase security strength check. */
    drv_crypto_pke_check_param(hash->length < klen);
#endif

    ret = hal_pke_alg_ecc_init(DRV_PKE_ECC_TYPE_SM2);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    hash_data.data = hash->data;
    hash_data.length = crypto_min(SM2_KEY_SIZE, hash->length);

    for (i = 0; i < MAX_TRY_TIMES; i++) {
        ret = inner_sm2_sign(priv_key, &hash_data, sig);
        if (ret == PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO) ||
            ret == PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_S_IS_ZERO)) {
            continue;
        }
        if (ret != CRYPTO_SUCCESS) {
            drv_set_err(ERROR_UNEXPECTED, "logged error");
            goto exit_deinit;
        } else {
            ret = CRYPTO_SUCCESS;
            goto exit_deinit;
        }
    }

    ret = PKE_COMPAT_ERRNO(ERROR_TRY_TIMES);
exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}
