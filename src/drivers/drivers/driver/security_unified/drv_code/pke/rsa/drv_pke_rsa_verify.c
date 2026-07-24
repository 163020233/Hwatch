/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke rsa verify implement.
 *
 * Create: 2025-01-07
*/
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_VERIFY

static int inner_rsa_verify(
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    unsigned char *em,
    const unsigned int klen,
    const unsigned int em_bit,
    const drv_pke_data *msg)
{
    int ret;
    rsa_pkcs1_pack vf_pack = {
        .klen = klen,
        .em_bit = em_bit,
        .em = em,
        .em_len = klen,
        .hash = msg->data,
        .hash_len = msg->length,
        .data = TD_NULL,
        .data_len = 0
    };

    crypto_unused(vf_pack);
    crypto_unused(hash_type);

    switch (scheme) {
#if defined(CONFIG_PKE_RSA_V15_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V15: {
            ret = pkcs1_v15_verify(hash_type, &vf_pack);
            drv_err_chk_return(ret != TD_SUCCESS, ret, "pkcs1_v15_verify failed, ret is 0x%x\n", ret);
            break;
        }
#endif
#if defined(CONFIG_PKE_RSA_V21_SUPPORT)
        case DRV_PKE_RSA_SCHEME_PKCS1_V21: {
            ret = pkcs1_pss_verify(hash_type, &vf_pack);
            drv_err_chk_return(ret != TD_SUCCESS, ret, "pkcs1_pss_verify failed, ret is 0x%x\n", ret);
            break;
        }
#endif
        default:

            drv_set_err(PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "direct error return");
            return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
    }

    return ret;
}

static int inner_pke_rsa_verify(
    const drv_pke_rsa_pub_key *pub_key,
    drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    int ret;
    unsigned char *em;
    unsigned int klen = pub_key->len;
    unsigned int em_bit = 0;
    drv_pke_data n_data = {.data = (unsigned char *)pub_key->n, .length = klen};
    drv_pke_data k_data = {.data = (unsigned char *)pub_key->e, .length = klen};
    drv_pke_data out_data;

    em = crypto_malloc_coherent(DRV_PKE_LEN_4096);
    drv_err_chk_return(em == NULL, PKE_COMPAT_ERRNO(ERROR_MALLOC), "malloc failed\n");
    (void)memset_s(em, DRV_PKE_LEN_4096, 0x00, DRV_PKE_LEN_4096);

    crypto_pke_dump_data("pub_key->n", pub_key->n, pub_key->len);
    crypto_pke_dump_data("pub_key->e", pub_key->e, pub_key->len);
    crypto_pke_dump_data("sign->data", sign->data, pub_key->len);

    out_data.data = em;
    out_data.length = klen;
    ret = drv_cipher_pke_exp_mod(&n_data, &k_data, sign, &out_data);
    drv_err_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit, ret, "drv_cipher_pke_exp_mod failed, ret is 0x%x\n", ret);

    crypto_pke_dump_data("em", em, pub_key->len);

    if (memcmp(sign->data, out_data.data, klen) == 0) {
        drv_set_err(ERROR_UNEXPECTED, "logged error");
        ret = PKE_COMPAT_ERRNO(ERROR_PKE_RSA_SAME_DATA);
        goto exit;
    }

    em_bit = rsa_get_bit_num((const unsigned char *)pub_key->n, pub_key->len);
    ret = inner_rsa_verify(scheme, hash_type, em, klen, em_bit, input_hash);
    drv_err_chk_goto_with_ret(ret, ret != TD_SUCCESS, exit, ret, "inner_rsa_verify failed, ret is 0x%x\n", ret);

exit:
    if (em != TD_NULL) {
        crypto_free_coherent(em);
    }
    return ret;
}

int drv_cipher_pke_rsa_verify(
    const drv_pke_rsa_pub_key *pub_key,
    const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type,
    const drv_pke_data *input_hash,
    const drv_pke_data *sign)
{
    int ret;
    unsigned int klen = 0;
    unsigned int hash_len;
    /* check ptr. */
    pke_null_ptr_chk(pub_key);
    pke_null_ptr_chk(pub_key->n);
    pke_null_ptr_chk(pub_key->e);
    pke_null_ptr_chk(input_hash);
    pke_null_ptr_chk(input_hash->data);
    pke_null_ptr_chk(sign);
    pke_null_ptr_chk(sign->data);

    /* check length. */
    hash_len = drv_get_hash_len(hash_type);
    drv_err_chk_return(input_hash->length != hash_len, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "input_hash->length is Invalid\n");

    klen = pub_key->len;
    drv_err_chk_return(klen != DRV_PKE_LEN_1024 && klen != DRV_PKE_LEN_2048 && klen != DRV_PKE_LEN_3072 &&
        klen != DRV_PKE_LEN_4096, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "klen is Invalid\n");

    drv_err_chk_return(sign->length < klen, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "sign_len is Invalid\n");
    drv_err_chk_return(crypto_pke_rsa_hash_len_check(klen, hash_type) != CRYPTO_SUCCESS,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid hash_type!\n");

    ret = inner_pke_rsa_verify(pub_key, scheme, hash_type, input_hash, sign);

    return ret;
}