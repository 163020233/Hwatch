/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecdsa sign implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"
#include "crypto_pke_common.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_SIGN

#define MAX_TRY_TIMES   100
#define MAX_ECC_LENGTH        72
#define MAX_ECC_BUF_LEN         (MAX_ECC_LENGTH * 2)

static int32_t inner_ecdsa_sign(const drv_pke_data *priv_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int32_t ret;
    uint8_t *buf = NULL;
    drv_pke_data k_data = {
        .length = priv_key->length
    };
    drv_pke_data r_data = {
        .data = sig->r,
        .length = sig->length
    };
    drv_pke_data s_data = {
        .data = sig->s,
        .length = sig->length
    };
    drv_pke_ecc_point kp_point = {
        .x = sig->r,
        .length = priv_key->length
    };
    const uint8_t *ecc_n = hal_pke_alg_ecc_get_n();

    buf = crypto_malloc_coherent(MAX_ECC_BUF_LEN);
    drv_err_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    k_data.data = buf;
    kp_point.y = k_data.data + MAX_ECC_LENGTH;

    /* Step 1. Generate k, which is in [1, n - 1] */
    ret = inner_get_limit_random(k_data.data, ecc_n, priv_key->length);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "inner_get_limit_random failed\n");
    crypto_pke_dump_data("random k", k_data.data, priv_key->length);

    /* Step 2. Compute kp = k * P(x, y) */
    crypto_pke_print("========== Start Compute kp = k * P(x, y) ==========\n");
    crypto_pke_dump_data("k", k_data.data, priv_key->length);

    ret = hal_pke_alg_ecc_base_point_mul(&k_data, &kp_point);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ecc_base_point_mul failed\n");

    crypto_pke_dump_data("kp(x)", kp_point.x, kp_point.length);
    crypto_pke_dump_data("kp(y)", kp_point.y, kp_point.length);
    crypto_pke_print("========== End Compute kp = k * P(x, y) ==========\n");
    /* Step 3. r = kp(x) mod n, if r == 0, return failure and try again. */
    if (inner_drv_is_zero(kp_point.x, kp_point.length)) {
        ret = PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO);
        goto exit_free;
    }
    /* Step 4. compute e = H(m), we can skip the step. */
    /* Step 5. s = k^-1(e + dr) mod n. if s == 0, return failure and try again */
    crypto_pke_print("========== Start Compute s = k^-1(e + dr) mod n ==========\n");
    crypto_pke_dump_data("k", k_data.data, priv_key->length);
    crypto_pke_dump_data("e", hash->data, hash->length);
    crypto_pke_dump_data("d", priv_key->data, priv_key->length);
    crypto_pke_dump_data("r", r_data.data, r_data.length);

    ret = hal_pke_alg_ecc_compute_s(&k_data, hash, priv_key, &r_data, &s_data);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ecc_compute_s failed\n");

    crypto_pke_dump_data("s", s_data.data, s_data.length);
    crypto_pke_print("========== End Compute s = k^-1(e + dr) mod n ==========\n");
    if (inner_drv_is_zero(s_data.data, s_data.length)) {
        ret = PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_S_IS_ZERO);
        goto exit_free;
    }

    ret = CRYPTO_SUCCESS;
exit_free:
    (void)memset_s(k_data.data, MAX_ECC_LENGTH, 0, MAX_ECC_LENGTH);
    crypto_free_coherent(buf);
    return ret;
}

static int32_t inner_ecdsa_sign_param_chk(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int32_t ret;
    uint32_t klen;
    /* param check. */
    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->data);
    pke_null_ptr_chk(hash);
    pke_null_ptr_chk(hash->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);

    ret = inner_ecdsa_type_check(curve_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid curve_type\n");

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    drv_err_chk_return(priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "priv_key->length is invalid\n");
    drv_err_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");

    drv_err_chk_return(inner_ecc_hash_len_check(klen, hash->length) != CRYPTO_SUCCESS,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hash_type!\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pke_ecdsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    int32_t ret;
    uint32_t i;
    drv_pke_data hash_data;
    uint32_t diff;
    uint8_t hash_buf[DRV_PKE_LEN_256];

    ret = inner_ecdsa_sign_param_chk(curve_type, priv_key, hash, sig);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    hash_data.data = hash->data;
    hash_data.length = crypto_min(priv_key->length, hash->length);

    if (curve_type == DRV_PKE_ECC_TYPE_SM2) {
#if defined(CONFIG_PKE_SUPPORT_SM2_SIGN)
        return drv_cipher_pke_sm2_sign(priv_key, &hash_data, sig);
#else
        drv_set_err(ERROR_UNSUPPORT, "direct error return");
        return ERROR_UNSUPPORT;
#endif
    }
    if (curve_type == DRV_PKE_ECC_TYPE_FIPS_P224R) {
        (void)memset_s(hash_buf, DRV_PKE_LEN_256, 0, DRV_PKE_LEN_256);
        hash_data.length = crypto_min(hash_data.length, HASH_SHA224_RESULT_SIZE);
        diff = DRV_PKE_LEN_256 - hash_data.length;
        ret = memcpy_s(&hash_buf[diff], DRV_PKE_LEN_256 - diff, hash_data.data, hash_data.length);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S);
        hash_data.data = hash_buf;
        hash_data.length = DRV_PKE_LEN_256;
    }
    ret = hal_pke_alg_ecc_init(curve_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    for (i = 0; i < MAX_TRY_TIMES; i++) {
        ret = inner_ecdsa_sign(priv_key, &hash_data, sig);
        if (ret == PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_R_IS_ZERO) ||
            ret == PKE_COMPAT_ERRNO(ERROR_ECDSA_SIGN_S_IS_ZERO)) {
            continue;
        }
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_ecdsa_sign failed\n");
        ret = CRYPTO_SUCCESS;
        goto exit_deinit;
    }

    drv_set_err(ERROR_UNEXPECTED, "logged error");
    ret = PKE_COMPAT_ERRNO(ERROR_TRY_TIMES);
exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}