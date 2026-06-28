/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecdsa verify implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define MAX_ECC_LENGTH          72
#define MAX_ECC_BUF_SIZE        (MAX_ECC_LENGTH * 4)

static int32_t inner_ecdsa_verify(const drv_pke_ecc_point *pub_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig,
    const drv_pke_data *v)
{
    int32_t ret;
    uint8_t *buf = NULL;
    drv_pke_data sig_s_data = {
        .data = sig->s,
        .length = sig->length
    };
    drv_pke_data w_data = {
        .length = pub_key->length
    };
    drv_pke_data r_data = {
        .data = sig->r,
        .length = sig->length
    };
    drv_pke_data u1_data = {
        .length = pub_key->length
    };
    drv_pke_data u2_data = {
        .length = pub_key->length
    };
    drv_pke_data x_data = {
        .length = pub_key->length
    };
    const uint8_t *ecc_n = hal_pke_alg_ecc_get_n();

    /* Step 1. Check if r is in [1, n - 1], s is in [1, n - 1]. */
    crypto_chk_return(inner_drv_is_in_range(sig->r, ecc_n, pub_key->length) == TD_FALSE,
        CRYPTO_FAILURE, "sig->r is not in [1, n - 1]!\n");
    crypto_chk_return(inner_drv_is_in_range(sig->s, ecc_n, pub_key->length) == TD_FALSE,
        CRYPTO_FAILURE, "sig->s is not in [1, n - 1]!\n");

    buf = crypto_malloc_coherent(MAX_ECC_BUF_SIZE);
    crypto_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    w_data.data = buf;
    u1_data.data = w_data.data + MAX_ECC_LENGTH;
    u2_data.data = u1_data.data + MAX_ECC_LENGTH;
    x_data.data = u2_data.data + MAX_ECC_LENGTH;
    if (v != NULL) {    /* pass v outside if v != NULL. */
        x_data.data = v->data;
    }

    /* Step 2. compute e = H(m), we can skip the step. */
    /* Step 3. compute w = s^-1 mod n. */

    crypto_pke_print("========== Start Compute w = s^-1 mod n ==========\n");
    crypto_pke_dump_data("s", sig_s_data.data, sig_s_data.length);

    ret = hal_pke_alg_ecc_inv_mod(&sig_s_data, &w_data);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_exit, "hal_pke_alg_ecc_inv_mod failed\n");

    crypto_pke_dump_data("w=s^-1 mod n", w_data.data, w_data.length);
    crypto_pke_print("========== End Compute w = s^-1 mod n ==========\n");

    /* Step 4. compute u1 = ew mod n, u2 = rw mod n. */
    crypto_pke_print("========== Start Compute u1 = ew mod n, u2 = rw mod n ==========\n");
    crypto_pke_dump_data("e", hash->data, hash->length);
    crypto_pke_dump_data("r", r_data.data, r_data.length);
    crypto_pke_dump_data("w", w_data.data, w_data.length);

    ret = hal_pke_alg_ecc_compute_u1_and_u2(&w_data, hash, &r_data, &u1_data, &u2_data);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_exit, "hal_pke_alg_ecc_compute_u1_and_u2 failed\n");

    crypto_pke_dump_data("u1 = ew mod n", u1_data.data, u1_data.length);
    crypto_pke_dump_data("u2 = ew mod n", u2_data.data, u2_data.length);
    crypto_pke_print("========== End Compute u1 = ew mod n, u2 = rw mod n ==========\n");
    /**
     * Step 5. compute x = u1 * p(x, y) + u2 * q(x, y).
     * Step 6. check if x is infinite point.
    **/
    crypto_pke_print("========== Start Compute x = u1 * p(x, y) + u2 * q(x, y) ==========\n");
    crypto_pke_dump_data("u1", u1_data.data, u1_data.length);
    crypto_pke_dump_data("u2", u2_data.data, u2_data.length);
    crypto_pke_dump_data("Q(x)", pub_key->x, pub_key->length);
    crypto_pke_dump_data("Q(y)", pub_key->y, pub_key->length);

    ret = hal_pke_alg_ecc_compute_x(&u1_data, &u2_data, pub_key, &x_data);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_exit, "hal_pke_alg_ecc_compute_x failed\n");

    crypto_pke_dump_data("x(x)", x_data.data, x_data.length);
    crypto_pke_print("========== End Compute x = u1 * p(x, y) + u2 * q(x, y) ==========\n");
    /**
     * Step 7. compute v = x(x).
     * Step 8. check if v == r.
    **/
    if (memcmp(x_data.data, sig->r, x_data.length) == 0) {
        crypto_free_coherent(buf);
        return CRYPTO_SUCCESS;
    }

    crypto_log_err("v != r\n");
    ret = CRYPTO_FAILURE;
error_exit:
    crypto_free_coherent(buf);
    return ret;
}

static int32_t inner_ecdsa_verify_check(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig, const drv_pke_data *v)
{
    int32_t ret;
    uint32_t klen;

    /* param check. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->x);
    pke_null_ptr_chk(pub_key->y);
    pke_null_ptr_chk(hash);
    pke_null_ptr_chk(hash->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);
    if (v != NULL) {
        pke_null_ptr_chk(v->data);
    }

    ret = inner_ecdsa_type_check(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid curve_type\n");

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "pub_key->length is invalid\n");
    crypto_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");
    if (v != NULL) {
        crypto_chk_return(v->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "v->length is invalid\n");
    }
    crypto_chk_return(inner_ecc_hash_len_check(klen, hash->length) != CRYPTO_SUCCESS,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hash_type!\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pke_ecdsa_verify_s(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig, const drv_pke_data *v)
{
    int32_t ret;
    uint32_t klen;
    drv_pke_data hash_data;
    uint32_t diff;
    uint8_t hash_buf[DRV_PKE_LEN_256];

    ret = inner_ecdsa_verify_check(curve_type, pub_key, hash, sig, v);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    klen = crypto_pke_get_klen(curve_type);
    hash_data.data = hash->data;
    hash_data.length = crypto_min(klen, hash->length);
    if (curve_type == DRV_PKE_ECC_TYPE_SM2) {
#if defined(CONFIG_PKE_SUPPORT_SM2_VERIFY)
        return drv_cipher_pke_sm2_verify(pub_key, &hash_data, sig);
#else
        return ERROR_UNSUPPORT;
#endif
    }
    if (curve_type == DRV_PKE_ECC_TYPE_FIPS_P224R) {
        (void)memset_s(hash_buf, DRV_PKE_LEN_256, 0, DRV_PKE_LEN_256);
        hash_data.length = crypto_min(hash_data.length, HASH_SHA224_RESULT_SIZE);
        diff = DRV_PKE_LEN_256 - hash_data.length;
        ret = memcpy_s(&hash_buf[diff], DRV_PKE_LEN_256 - diff, hash_data.data, hash_data.length);
        crypto_chk_return(ret != EOK, ERROR_MEMCPY_S);
        hash_data.data = hash_buf;
        hash_data.length = DRV_PKE_LEN_256;
    }

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_ecdsa_verify(pub_key, &hash_data, sig, v);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_ecdsa_verify failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}

int drv_cipher_pke_ecdsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig)
{
    return drv_cipher_pke_ecdsa_verify_s(curve_type, pub_key, hash, sig, NULL);
}