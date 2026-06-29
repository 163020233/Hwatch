/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke eddsa verify implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define ED25519_KEY_LEN     32
#define ED25519_BUF_LEN     (ED25519_KEY_LEN * 6)

static int32_t inner_eddsa_verify(const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    int32_t ret;
    uint8_t *buf = NULL;
    drv_pke_ecc_point A_point = {
        .length = ED25519_KEY_LEN
    };
    drv_pke_ecc_point R_point = {
        .length = ED25519_KEY_LEN
    };
    drv_pke_data public_data = {
        .data = pub_key->y,
        .length = ED25519_KEY_LEN
    };
    drv_pke_data s_data = {
        .length = ED25519_KEY_LEN
    };
    drv_pke_data Rs_data = {
        .data = sig->r,
        .length = ED25519_KEY_LEN
    };
    drv_pke_data h_data = {
        .length = ED25519_KEY_LEN
    };
    drv_pke_data arr[3];    // 3: for Rs, public and msg.
    const unsigned char *ed_n = hal_pke_alg_ecc_get_n();

    buf = crypto_malloc_coherent(ED25519_BUF_LEN);
    crypto_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    A_point.x = buf;
    A_point.y = A_point.x + ED25519_KEY_LEN;
    R_point.x = A_point.y + ED25519_KEY_LEN;
    R_point.y = R_point.x + ED25519_KEY_LEN;
    s_data.data = R_point.y + ED25519_KEY_LEN;
    h_data.data = s_data.data + ED25519_KEY_LEN;

    /* Step 1. A = point_decompress(public), if fail, reutrn verify failure. */
    crypto_pke_dump_data("public", public_data.data, public_data.length);
    ret = hal_pke_alg_ecc_point_decompress(&public_data, &A_point);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ecc_point_decompress failed\n");
    crypto_pke_dump_data("A(x)", A_point.x, A_point.length);
    crypto_pke_dump_data("A(y)", A_point.y, A_point.length);

    /* Step 2. R = point_decompress(Rs), if fail, return verify failure. */
    crypto_pke_dump_data("Rs", Rs_data.data, Rs_data.length);
    ret = hal_pke_alg_ecc_point_decompress(&Rs_data, &R_point);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ecc_point_decompress failed\n");
    crypto_pke_dump_data("R(x)", R_point.x, R_point.length);
    crypto_pke_dump_data("R(y)", R_point.y, R_point.length);

    /* Step 3. check if s in [0, n - 1]. */
    crypto_pke_common_little_big_endian_trans(s_data.data, sig->s, sig->length);

    crypto_chk_goto_with_ret(ret, memcmp(s_data.data, ed_n, ED25519_KEY_LEN) >= 0,  // s >= n
        exit_free, ERROR_INVALID_PARAM, "s must be in [0, n - 1].\n");

    /* Step 4. h = sha512_modq(Rs + public + msg). */
    arr[0].data = Rs_data.data;
    arr[0].length = Rs_data.length;
    arr[1].data = public_data.data;
    arr[1].length = public_data.length;
    arr[2].data = msg->data;    // 2: msg idx
    arr[2].length = msg->length;    // 2: msg idx
    ret = hal_pke_alg_ed_sha512_modq(arr, 3, &h_data);  // 3: arr len
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ed_sha512_modq failed\n");
    crypto_pke_dump_data("h", h_data.data, h_data.length);

    /* Step 5. sB = point_mul(s, G). */
    /* Step 6. hA = point_mul(h, A). */
    /* Step 7. check if sB is equal to (R + hA). */
    /* 1) compute C = sB - hA. */
    /* 2) check whether C is equal to R. */
    ret = hal_pke_alg_ed_mul_sub(&s_data, &h_data, &A_point, &R_point);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_alg_ed_mul_sub failed\n");

    ret = CRYPTO_SUCCESS;
exit_free:
    crypto_free_coherent(buf);
    return ret;
}

int32_t drv_cipher_pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig)
{
    int32_t ret;
    uint32_t klen;

    /* param check. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->y);
    pke_null_ptr_chk(msg);
    pke_null_ptr_chk(msg->data);
    pke_null_ptr_chk(sig);
    pke_null_ptr_chk(sig->r);
    pke_null_ptr_chk(sig->s);

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(pub_key->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "pub_key->length is invalid\n");
    crypto_chk_return(sig->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "sig->length is invalid\n");

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = inner_eddsa_verify(pub_key, msg, sig);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_deinit, "inner_eddsa_verify failed\n");

exit_deinit:
    hal_pke_alg_ecc_deinit();
    return ret;
}