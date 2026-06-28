/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap rsa code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "unified_cipher_pke.h"

/* RSA key pair format
    version             INTEGER,  -- must be 0
    modulus             INTEGER,  -- n
    publicExponent      INTEGER,  -- e
    privateExponent     INTEGER,  -- d
    prime1              INTEGER,  -- p
    prime2              INTEGER,  -- q
    exponent1           INTEGER,  -- d mod (p-1)
    exponent2           INTEGER,  -- d mod (q-1)
    coefficient         INTEGER,  -- (inverse of q) mod p
*/

/* RSA public key format
    modulus             INTEGER,   -- n
    publicExponent      INTEGER    -- e
*/

psa_status_t psa_driver_wrapper_rsa_sign(psa_algorithm_t alg, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_rsa_priv_key_t private_key;
    uapi_drv_cipher_pke_rsa_scheme_t scheme;
    uapi_drv_cipher_pke_hash_type_t hash_type = UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256;
    uapi_drv_cipher_pke_data_t input_hash = {.length = hash_length, .data = (uint8_t *)hash};
    uapi_drv_cipher_pke_data_t sign = {.length = signature_size, .data = (uint8_t *)signature};

    ret = psa_get_rsa_private_key(bits, key_data, key_len, &private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_private_key error!\n");

    if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15;
    } else {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21;
    }

    ret = (int32_t)uapi_drv_cipher_pke_rsa_sign(&private_key, scheme, hash_type, &input_hash, &sign);
    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_rsa_verify(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature,
    uint32_t signature_size)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uint8_t *local_e;
    uapi_drv_cipher_pke_rsa_priv_key_t private_key;
    uapi_drv_cipher_pke_rsa_pub_key_t public_key;
    uapi_drv_cipher_pke_rsa_scheme_t scheme;
    uapi_drv_cipher_pke_hash_type_t hash_type = UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256;
    uapi_drv_cipher_pke_data_t input_hash = {.length = hash_length, .data = (uint8_t *)hash};
    uapi_drv_cipher_pke_data_t sign = {.length = signature_size, .data = (uint8_t *)signature};

    if (type == PSA_KEY_TYPE_RSA_PUBLIC_KEY) {
        ret = psa_get_rsa_public_key(bits, key_data, key_len, &public_key);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_public_key error!\n");
    } else {
        ret = psa_get_rsa_private_key(bits, key_data, key_len, &private_key);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_private_key error!\n");

        public_key.n = private_key.n;
        public_key.e = private_key.e;
        public_key.len = private_key.n_len;
    }

    /* malloc e buffer for uapi */
    local_e = psa_crypto_malloc(public_key.len);
    psa_crypto_chk_return(local_e == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");
    memset_sec(local_e, public_key.len, 0, public_key.len);

    /* copy e */
    ret = memcpy_sec(local_e + public_key.len - PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH, PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH,
        public_key.e, PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");
    public_key.e = local_e;

    if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15;
    } else {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21;
    }

    ret = (int32_t)uapi_drv_cipher_pke_rsa_verify(&public_key, scheme, hash_type, &input_hash, &sign);
    psa_crypto_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_pke_rsa_verify error!\n");
    ret = PSA_INTERNAL_SUCCESS;

free:
    psa_crypto_free(local_e);
    return ret;
}

psa_status_t psa_driver_wrapper_rsa_encrypt(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t input_length, const uint8_t *salt,
    uint32_t salt_length, uint8_t *output, uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uint8_t *local_e;
    uapi_drv_cipher_pke_rsa_priv_key_t private_key;
    uapi_drv_cipher_pke_rsa_pub_key_t public_key;
    uapi_drv_cipher_pke_rsa_scheme_t scheme;
    uapi_drv_cipher_pke_hash_type_t hash_type = UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256;
    uapi_drv_cipher_pke_data_t input_data = {.length = input_length, .data = (uint8_t *)input};
    uapi_drv_cipher_pke_data_t label_data = {.length = salt_length, .data = (uint8_t *)salt};
    uapi_drv_cipher_pke_data_t output_data = {.length = output_length, .data = (uint8_t *)output};

    if (type == PSA_KEY_TYPE_RSA_PUBLIC_KEY) {
        ret = psa_get_rsa_public_key(bits, key_data, key_len, &public_key);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_public_key error!\n");
    } else {
        ret = psa_get_rsa_private_key(bits, key_data, key_len, &private_key);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_private_key error!\n");

        public_key.n = private_key.n;
        public_key.e = private_key.e;
        public_key.len = private_key.n_len;
    }

    /* malloc e buffer for uapi */
    local_e = psa_crypto_malloc(public_key.len);
    psa_crypto_chk_return(local_e == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");
    memset_sec(local_e, public_key.len, 0, public_key.len);

    /* copy e */
    ret = memcpy_sec(local_e + public_key.len - PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH, PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH,
        public_key.e, PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");
    public_key.e = local_e;

    if (alg == PSA_ALG_RSA_PKCS1V15_CRYPT) {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15;
    } else {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21;
    }

    if (salt == NULL) {
        ret = (int32_t)uapi_drv_cipher_pke_rsa_public_encrypt(scheme, hash_type, &public_key, &input_data, NULL,
            &output_data);
        psa_crypto_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free,
            psa_get_wrap_errcode(ret), "uapi_drv_cipher_pke_rsa_public_encrypt error!\n");
    } else {
        ret = (int32_t)uapi_drv_cipher_pke_rsa_public_encrypt(scheme, hash_type, &public_key, &input_data, &label_data,
            &output_data);
        psa_crypto_chk_goto_with_ret(ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free,
            psa_get_wrap_errcode(ret), "uapi_drv_cipher_pke_rsa_public_encrypt error!\n");
    }
    ret = PSA_INTERNAL_SUCCESS;

free:
    psa_crypto_free(local_e);
    return ret;
}

psa_status_t psa_driver_wrapper_rsa_decrypt(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t input_length, const uint8_t *salt,
    uint32_t salt_length, uint8_t *output, uint32_t *output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_rsa_priv_key_t private_key;
    uapi_drv_cipher_pke_rsa_scheme_t scheme;
    uapi_drv_cipher_pke_hash_type_t hash_type = UAPI_DRV_CIPHER_PKE_HASH_TYPE_SHA256;
    uapi_drv_cipher_pke_data_t input_data = {.length = input_length, .data = (uint8_t *)input};
    uapi_drv_cipher_pke_data_t label_data = {.length = salt_length, .data = (uint8_t *)salt};
    uapi_drv_cipher_pke_data_t output_data = {.length = *output_length, .data = (uint8_t *)output};

    psa_unused(type);

    ret = psa_get_rsa_private_key(bits, key_data, key_len, &private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_rsa_private_key error!\n");

    if (alg == PSA_ALG_RSA_PKCS1V15_CRYPT) {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V15;
    } else {
        scheme = UAPI_DRV_CIPHER_PKE_RSA_SCHEME_PKCS1_V21;
    }

    if (salt == NULL) {
        ret = (int32_t)uapi_drv_cipher_pke_rsa_private_decrypt(scheme, hash_type, &private_key, &input_data, NULL,
            &output_data);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_rsa_private_decrypt error!\n");
    } else {
        ret = (int32_t)uapi_drv_cipher_pke_rsa_private_decrypt(scheme, hash_type, &private_key, &input_data,
            &label_data, &output_data);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_rsa_private_decrypt error!\n");
    }

    *output_length = output_data.length;
    return psa_get_wrap_errcode(ret);
}