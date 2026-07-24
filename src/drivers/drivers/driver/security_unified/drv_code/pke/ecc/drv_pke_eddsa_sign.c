/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecdsa sign implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_EDDSA_SIGN

#define ED25519_KEY_LEN     32
static int inner_eddsa_sign(const drv_pke_data *priv_key, const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    int ret;
    unsigned int keylen = priv_key->length;
    unsigned char a[ED25519_KEY_LEN];
    unsigned char aG_point_compress_buf[ED25519_KEY_LEN];
    unsigned char prefix[ED25519_KEY_LEN];
    unsigned char r_buf[ED25519_KEY_LEN];
    drv_pke_data a_data = {
        .data = a,
        .length = ED25519_KEY_LEN
    };
    drv_pke_data aG_point_compress = {
        .data = aG_point_compress_buf,
        .length = ED25519_KEY_LEN
    };
    drv_pke_data s_data = {
        .data = sig->s,
        .length = sig->length
    };
    drv_pke_data r_data = {
        .data = r_buf,
        .length = sig->length
    };
    drv_pke_data rG_point_compress = {
        .data = sig->r,
        .length = ED25519_KEY_LEN
    };
    drv_pke_data h_data = {
        .data = sig->s,
        .length = sig->length
    };
    drv_pke_data arr[3];

    /* Step 1. a, prefix = secret_expand(secret). */
    ret = crypto_pke_common_secret_expand(priv_key->data, a, prefix, keylen);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "crypto_pke_common_secret_expand failed\n");
    crypto_pke_dump_data("a", a, sizeof(a));
    crypto_pke_dump_data("prefix", prefix, sizeof(prefix));

    /* Step 2. A = point_compress(point_mul(a, G)). */
    ret = hal_pke_alg_ed_base_point_mul_compress(&a_data, &aG_point_compress);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_pke_alg_ed_base_point_mul_compress failed\n");
    crypto_pke_dump_data("aG_point_compress", aG_point_compress.data, aG_point_compress.length);

    /* Step 3. r = sha512_modq(prefix + msg). */
    arr[0].data = prefix;
    arr[0].length = ED25519_KEY_LEN;
    arr[1].data = msg->data;
    arr[1].length = msg->length;
    ret = hal_pke_alg_ed_sha512_modq(arr, 2, &r_data);  // 2: arr len
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_pke_alg_ed_point_compress failed\n");
    crypto_pke_dump_data("r", r_data.data, r_data.length);

    /* Step 4. R = point_mul(r, G). */
    /* Step 5. Rs = point_compress(R). */
    ret = hal_pke_alg_ed_base_point_mul_compress(&r_data, &rG_point_compress);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_pke_alg_ed_base_point_mul_compress failed\n");
    crypto_pke_dump_data("Rs", rG_point_compress.data, rG_point_compress.length);

    /* Step 6. h = sha512_modq(Rs + A + msg). */
    arr[0].data = rG_point_compress.data;
    arr[0].length = rG_point_compress.length;
    arr[1].data = aG_point_compress.data;
    arr[1].length = aG_point_compress.length;
    arr[2].data = msg->data;    // 2: msg idx
    arr[2].length = msg->length;    // 2: msg idx
    ret = hal_pke_alg_ed_sha512_modq(arr, 3, &h_data);  // 3: arr len
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_pke_alg_ed_point_compress failed\n");
    crypto_pke_dump_data("h", h_data.data, h_data.length);
    /* Step 7. s = (r + h * a) % q. */
    ret = hal_pke_alg_ed_compute_s(&r_data, &h_data, &a_data, &s_data);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hal_pke_alg_ed_compute_s failed\n");
    crypto_pke_dump_data("s", s_data.data, s_data.length);

exit_clean:
    (void)memset_s(a, ED25519_KEY_LEN, 0, ED25519_KEY_LEN);
    (void)memset_s(prefix, ED25519_KEY_LEN, 0, ED25519_KEY_LEN);
    return ret;
}

int drv_cipher_pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    int ret;
    uint32_t klen;

    pke_null_ptr_chk(priv_key);
    pke_null_ptr_chk(priv_key->data);
    pke_null_ptr_chk(msg);
    pke_null_ptr_chk(msg->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    drv_err_chk_return(priv_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "priv_key->length is invalid\n");
    drv_err_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");

    ret = hal_pke_alg_ecc_init(curve_type);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_eddsa_sign(priv_key, msg, sig);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_eddsa_sign failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}