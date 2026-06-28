/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke curve gen key implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

static int inner_curve_gen_key(const drv_pke_data *input_priv_key, const drv_pke_data *output_priv_key,
    const drv_pke_ecc_point *output_pub_key)
{
    int ret;
    unsigned int klen = hal_pke_alg_ecc_get_klen();

    /* Step 1. Randomly or pseudorandomly select an integer d.
     * for curve25519, the cofactor h is not 0x1, so that the private key is not limited in [1, n-1]
     */
    if (input_priv_key != NULL && input_priv_key->data != NULL) {
        ret = memcpy_s(output_priv_key->data, output_priv_key->length, input_priv_key->data, input_priv_key->length);
        crypto_chk_return(ret != EOK, PKE_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");
    } else {
        ret = inner_get_random(output_priv_key->data, klen);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_random failed\n");
    }

    crypto_pke_dump_data("d", output_priv_key->data, output_priv_key->length);

    /* Step 2. P(x, _) = d * G(x, y). */
    ret = hal_pke_alg_curve_base_point_mul(output_priv_key, output_pub_key);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "hal_pke_alg_curve_base_point_mul failed\n");

    crypto_pke_dump_data("P(x)", output_pub_key->x, output_pub_key->length);

    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(output_priv_key->data, output_priv_key->length, 0, output_priv_key->length);
    return ret;
}

int drv_cipher_pke_curve_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key)
{
    int ret;
    unsigned int klen;

    /* param null check */
    pke_null_ptr_chk(output_priv_key);
    pke_null_ptr_chk(output_priv_key->data);
    pke_null_ptr_chk(output_pub_key);
    pke_null_ptr_chk(output_pub_key->x);

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(output_priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "output_priv_key->length is invalid\n");
    crypto_chk_return(output_pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "output_pub_key->length is invalid\n");
    if (input_priv_key != NULL && input_priv_key->data != NULL) {
        crypto_chk_return(input_priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "input_priv_key->length is invalid\n");
    }

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_curve_gen_key(input_priv_key, output_priv_key, output_pub_key);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_curve_gen_key failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}