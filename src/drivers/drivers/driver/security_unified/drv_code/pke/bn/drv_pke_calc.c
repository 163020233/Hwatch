/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke cal implement.
 *
 * Create: 2025-01-07
*/

#include "drv_pke_cal.h"
#include "drv_pke_priv.h"

static int32_t inner_common_param4_chk(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c, bool is_odd_required)
{
    pke_null_ptr_chk(a);
    pke_null_ptr_chk(a->data);
    pke_null_ptr_chk(b);
    pke_null_ptr_chk(b->data);
    pke_null_ptr_chk(p);
    pke_null_ptr_chk(p->data);
    pke_null_ptr_chk(c);
    pke_null_ptr_chk(c->data);
    crypto_chk_return(a->length > p->length, ERROR_INVALID_PARAM);
    crypto_chk_return(b->length > p->length, ERROR_INVALID_PARAM);
    crypto_chk_return(c->length != p->length, ERROR_INVALID_PARAM);
    return inner_modular_chk(p, is_odd_required);
}

/* c = a mod p. */
int drv_cipher_pke_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    int ret;
    uint32_t a_valit_bit;
    uint32_t p_valit_bit;

    pke_null_ptr_chk(a);
    pke_null_ptr_chk(a->data);
    pke_null_ptr_chk(p);
    pke_null_ptr_chk(p->data);
    pke_null_ptr_chk(c);
    pke_null_ptr_chk(c->data);
    crypto_chk_return(c->length != p->length, ERROR_INVALID_PARAM);
    crypto_chk_return(a->length > p->length * 2, ERROR_INVALID_PARAM); // 2: double

    ret = inner_modular_chk(p, true);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    a_valit_bit = a->length * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length(a);
    p_valit_bit = p->length * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length(p);
    /* Require valid_bit_len(a) <= valid_bit_len(p) * 2. */
    crypto_chk_return(a_valit_bit > p_valit_bit * 2, ERROR_INVALID_PARAM);  // 2: double

    ret = hal_pke_alg_rsa_mod(a, p, c);
    return ret;
}

int drv_cipher_pke_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;

    ret = inner_common_param4_chk(a, b, p, c, false);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_rsa_add_mod(a, b, p, c);
    return ret;
}

int drv_cipher_pke_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;

    ret = inner_common_param4_chk(a, b, p, c, false);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_rsa_sub_mod(a, b, p, c);

    return ret;
}

int drv_cipher_pke_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c)
{
    int ret;

    ret = inner_common_param4_chk(a, b, p, c, true);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_rsa_mul_mod(a, b, p, c);
    return ret;
}

int drv_cipher_pke_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c)
{
    int ret;

    pke_null_ptr_chk(a);
    pke_null_ptr_chk(a->data);
    pke_null_ptr_chk(p);
    pke_null_ptr_chk(p->data);
    pke_null_ptr_chk(c);
    pke_null_ptr_chk(c->data);
    crypto_chk_return(a->length != p->length, ERROR_INVALID_PARAM);
    crypto_chk_return(c->length != p->length, ERROR_INVALID_PARAM);
    ret = inner_modular_chk(p, true);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_rsa_inv_mod(a, p, c);
    return ret;
}

int drv_cipher_pke_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    int ret;

    pke_null_ptr_chk(a);
    pke_null_ptr_chk(a->data);
    pke_null_ptr_chk(b);
    pke_null_ptr_chk(b->data);
    pke_null_ptr_chk(c);
    pke_null_ptr_chk(c->data);
    crypto_chk_return(a->length > DRV_PKE_LEN_2048, ERROR_INVALID_PARAM);
    crypto_chk_return(b->length > DRV_PKE_LEN_2048, ERROR_INVALID_PARAM);
    crypto_chk_return(c->length != (a->length + b->length), ERROR_INVALID_PARAM);

    ret = hal_pke_alg_rsa_mul(a, b, c);
    return ret;
}

int drv_cipher_pke_mul_dot(drv_pke_ecc_curve_type curve_type, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r)
{
    int ret;
    uint32_t klen;
    drv_pke_data compress_p;

    pke_null_ptr_chk(k);
    pke_null_ptr_chk(k->data);
    pke_null_ptr_chk(p);
    pke_null_ptr_chk(r);

    if (curve_type != DRV_PKE_ECC_TYPE_RFC7748 && curve_type != DRV_PKE_ECC_TYPE_RFC7748_448) {
        pke_null_ptr_chk(p->y);
        pke_null_ptr_chk(r->y);
    }
    if (curve_type != DRV_PKE_ECC_TYPE_RFC8032) {
        pke_null_ptr_chk(p->x);
        pke_null_ptr_chk(r->x);
    }

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(k->length != klen, ERROR_INVALID_PARAM,
        "k->length is invalid\n");
    crypto_chk_return(p->length != klen, ERROR_INVALID_PARAM,
        "p->length is invalid\n");
    crypto_chk_return(r->length != klen, ERROR_INVALID_PARAM,
        "r->length is invalid\n");

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    if (curve_type == DRV_PKE_ECC_TYPE_RFC7748 || curve_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
#if defined(CONFIG_PKE_CURVE_ECDH_SUPPORT)
        ret = hal_pke_alg_curve_point_mul(k, p, r);
#else
        ret = ERROR_UNSUPPORT;
#endif
    } else if (curve_type == DRV_PKE_ECC_TYPE_RFC8032) {
#if defined(CONFIG_PKE_EDDSA_SIGN_SUPPORT)
        compress_p.data = r->y;
        compress_p.length = r->length;
        ret = hal_pke_alg_ed_point_mul_compress(k, p, &compress_p);
#else
        crypto_unused(compress_p);
        ret = ERROR_UNSUPPORT;
#endif
    } else {
        ret = hal_pke_alg_ecc_point_mul(k, p, r);
    }

    hal_pke_alg_ecc_deinit();
    return ret;
}

static int32_t inner_pke_mul_dot_op_param_chk(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int32_t ret;
    uint32_t klen;
    pke_null_ptr_chk(a);
    pke_null_ptr_chk(a->data);
    pke_null_ptr_chk(b);
    pke_null_ptr_chk(b->data);
    pke_null_ptr_chk(q);
    pke_null_ptr_chk(q->x);
    pke_null_ptr_chk(q->y);
    pke_null_ptr_chk(r);
    pke_null_ptr_chk(r->x);
    pke_null_ptr_chk(r->y);
    if (p != NULL) {    /* p support NULL. */
        pke_null_ptr_chk(p->x);
        pke_null_ptr_chk(p->y);
    }

    ret = inner_ecdsa_type_check(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid curve_type\n");

    /* length check. */
    klen = crypto_pke_get_klen(curve_type);
    crypto_chk_return(a->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "a->length is invalid\n");
    crypto_chk_return(b->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "b->length is invalid\n");
    if (p != NULL) {    /* p support NULL. */
        crypto_chk_return(p->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
            "p->length is invalid\n");
    }
    crypto_chk_return(q->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "q->length is invalid\n");
    crypto_chk_return(r->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "r->length is invalid\n");

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_pke_mul_dot_add(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int32_t ret;

    ret = inner_pke_mul_dot_op_param_chk(curve_type, a, b, p, q, r);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = hal_pke_alg_point_mul_add(a, b, p, q, r);
    hal_pke_alg_ecc_deinit();

    return ret;
}

int32_t drv_cipher_pke_add_dot(drv_pke_ecc_curve_type curve_type,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int32_t ret = inner_ecdsa_type_check(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid curve_type\n");
    uint32_t klen = crypto_pke_get_klen(curve_type);
    unsigned char val = 1;
    drv_pke_data data = {.data = &val, .length = 1};

    pke_null_ptr_chk(p);
    pke_null_ptr_chk(p->x);
    pke_null_ptr_chk(p->y);
    pke_null_ptr_chk(q);
    pke_null_ptr_chk(q->x);
    pke_null_ptr_chk(q->y);
    pke_null_ptr_chk(r);
    pke_null_ptr_chk(r->x);
    pke_null_ptr_chk(r->y);
    crypto_chk_return(p->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "p->length is invalid\n");
    crypto_chk_return(q->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "q->length is invalid\n");
    crypto_chk_return(r->length != klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "r->length is invalid\n");

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = hal_pke_alg_point_mul_add(&data, &data, p, q, r);
    hal_pke_alg_ecc_deinit();

    return ret;
}

int32_t drv_cipher_pke_mul_dot_sub(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int32_t ret;

    ret = inner_pke_mul_dot_op_param_chk(curve_type, a, b, p, q, r);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_ecc_init(curve_type);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_ecc_init failed\n");

    ret = hal_pke_alg_point_mul_sub(a, b, p, q, r);
    hal_pke_alg_ecc_deinit();
    return ret;
}

int32_t drv_cipher_pke_get_ecc_param(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_curve **ecc_curve)
{
    return hal_pke_alg_ecc_get_param(curve_type, ecc_curve, NULL);
}