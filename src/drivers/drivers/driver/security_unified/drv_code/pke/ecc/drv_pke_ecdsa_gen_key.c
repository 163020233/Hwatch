/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecdsa gen key implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_GEN_KEY

/* According to SEC1 "3.2.1 Elliptic Curve Key Pair Generation Primitiv"
*/
static int inner_ecc_gen_key(const drv_pke_data *input_priv_key, const drv_pke_data *output_priv_key,
    const drv_pke_ecc_point *output_pub_key)
{
    int ret;
    const unsigned char *ecc_n = hal_pke_alg_ecc_get_n();
    unsigned int klen = hal_pke_alg_ecc_get_klen();

    /* Step 1. Randomly or pseudorandomly select an integer d in the interval [1, n − 1].
     * Notice: If input_priv_key->data != NULL, then use it as d and skip step1.
     */
    if (input_priv_key != NULL && input_priv_key->data != NULL) {
        ret = memcpy_s(output_priv_key->data, output_priv_key->length, input_priv_key->data, input_priv_key->length);
        drv_err_chk_return(ret != EOK, PKE_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");
    } else {
        ret = inner_get_limit_random(output_priv_key->data, ecc_n, klen);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_limit_random failed\n");
    }

    /* Step 2. Compute Q = d*G. */
    /* Step 3. Output (d, Q). */
    crypto_pke_print("========== Start Compute Q = d*G. ==========\n");
    crypto_pke_dump_data("d", output_priv_key->data, output_priv_key->length);

    ret = hal_pke_alg_ecc_base_point_mul(output_priv_key, output_pub_key);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "hal_pke_alg_ecc_base_point_mul failed\n");

    crypto_pke_dump_data("Q(x)", output_pub_key->x, output_pub_key->length);
    crypto_pke_dump_data("Q(y)", output_pub_key->y, output_pub_key->length);
    crypto_pke_print("========== End Compute Q = d*G. ==========\n");
    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(output_priv_key->data, output_priv_key->length, 0, output_priv_key->length);
    return ret;
}

int drv_cipher_pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key)
{
    int ret = CRYPTO_FAILURE;
    unsigned int klen;

    if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
#if defined(CONFIG_PKE_SUPPORT_CURVE)
        return drv_cipher_pke_curve_gen_key(curve_type, input_priv_key, output_priv_key, output_pub_key);
#else
        drv_set_err(ERROR_UNSUPPORT, "direct error return");
        return ERROR_UNSUPPORT;
#endif
    }
    if (curve_type == DRV_PKE_ECC_TYPE_RFC8032) {
#if defined(CONFIG_PKE_SUPPORT_EDWARD)
        return drv_cipher_pke_ed_gen_key(curve_type, input_priv_key, output_priv_key, output_pub_key);
#else
        drv_set_err(ERROR_UNSUPPORT, "direct error return");
        return ERROR_UNSUPPORT;
#endif
    }

    /* param null check */
    pke_null_ptr_chk(output_priv_key);
    pke_null_ptr_chk(output_priv_key->data);
    pke_null_ptr_chk(output_pub_key);
    pke_null_ptr_chk(output_pub_key->x);
    pke_null_ptr_chk(output_pub_key->y);

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    drv_err_chk_return(output_priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "output_priv_key->length is invalid\n");
    drv_err_chk_return(output_pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "output_pub_key->length is invalid\n");
    if (input_priv_key != NULL && input_priv_key->data != NULL) {
        drv_err_chk_return(input_priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "input_priv_key->length is invalid\n");
    }

    ret = hal_pke_alg_ecc_init(curve_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_ecc_gen_key(input_priv_key, output_priv_key, output_pub_key);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_ecc_gen_key failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}