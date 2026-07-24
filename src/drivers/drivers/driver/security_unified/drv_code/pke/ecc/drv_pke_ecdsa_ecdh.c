/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecdsa ecdh implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_ECDH

#define MAX_ECC_LENGTH      72
/* According to SEC1 "3.3.2 Elliptic Curve Cofactor Diffie-Hellman Primitive" */
static int inner_ecc_gen_ecdh_key(const drv_pke_ecc_point *input_pub_key, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_shared_key)
{
    int ret;
    drv_pke_ecc_point rr = {0};
    unsigned char rr_y[MAX_ECC_LENGTH];

    rr.x = output_shared_key->data;
    rr.y = rr_y;
    rr.length = output_shared_key->length;

    crypto_pke_dump_data("d", input_priv_key->data, input_priv_key->length);
    crypto_pke_dump_data("Q(x)", input_pub_key->x, input_pub_key->length);
    crypto_pke_dump_data("Q(y)", input_pub_key->y, input_pub_key->length);
    /* Step 1. Compute point P(x, y) = h*Du*Qv.
     * 1) h is the ecc parameter, for brainpool/fips/sm2, the h is 1, so we can compute P(x, y) = Du*Qv directly.
     * 2) Du is the input priv_key.
     * 3) Qv is the output pub_key.
    */

    ret = hal_pke_alg_ecc_point_mul(input_priv_key, input_pub_key, &rr);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_point_mul failed\n");

    crypto_pke_dump_data("P(x)", rr.x, input_priv_key->length);
    /* Step 2. Check that if P(x, y) is ZERO. */
    /* Step 3. Output z = Xp. */

    return CRYPTO_SUCCESS;
}

int drv_cipher_pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key)
{
    int ret;
    unsigned int klen;

    if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
#if defined(CONFIG_PKE_CURVE_ECDH_SUPPORT)
        return drv_cipher_pke_curve_gen_ecdh_key(curve_type, input_pub_key, input_priv_key, output_shared_key);
#else
        drv_set_err(ERROR_UNSUPPORT, "direct error return");
        return ERROR_UNSUPPORT;
#endif
    }

    /* param null check. */
    pke_null_ptr_chk(input_pub_key);
    pke_null_ptr_chk(input_pub_key->x);
    pke_null_ptr_chk(input_pub_key->y);
    pke_null_ptr_chk(input_priv_key);
    pke_null_ptr_chk(input_priv_key->data);
    pke_null_ptr_chk(output_shared_key);
    pke_null_ptr_chk(output_shared_key->data);

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    drv_err_chk_return(input_priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "input_priv_key->length is invalid\n");
    drv_err_chk_return(input_pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "input_pub_key->length is invalid\n");
    drv_err_chk_return(output_shared_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "output_shared_key->length is invalid\n");

    ret = hal_pke_alg_ecc_init(curve_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_ecc_gen_ecdh_key(input_pub_key, input_priv_key, output_shared_key);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_ecc_gen_ecdh_key failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}