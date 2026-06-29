/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap ecc code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "unified_cipher_pke.h"
#ifdef CONFIG_PKE_EC_ELGAMAL_CRYPT_SUPPORT
#include "ec_elgamal_crypt.h"
#endif
#include "ecc_curve_param.h"
#include "trng.h"

/* ECC pair key format(not curve)
   privateKey           STRING
*/

/* ECC public key format(not curve)
   compress flag        BYTE,     --0x04 means uncompressed, 0x02 or 0x03 means compressed, other value not allowed
   ECCPoint-X           STRING,
   ECCPoint-Y           STRING,
*/

static const psa_crypto_table_item_pro g_ecc_curve_type[] = {
    {PSA_ECC_FAMILY_SECP_R1, 256, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P256R},
    {PSA_ECC_FAMILY_SECP_R1, 384, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P384R},
    {PSA_ECC_FAMILY_SECP_R1, 521, UAPI_DRV_CIPHER_PKE_ECC_TYPE_FIPS_P521R},
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 256, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P256},
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 384, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P384},
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 512, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC5639_P512},
    {PSA_ECC_FAMILY_MONTGOMERY, 255, UAPI_DRV_CIPHER_PKE_ECC_TYPE_RFC7748},
    {PSA_ECC_FAMILY_SM2, 256, UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2},
};

psa_status_t psa_driver_wrapper_ecdh(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *private_key,
    uint32_t private_key_length, const uint8_t *public_key, uint32_t public_key_length, uint8_t *shared_key,
    uint32_t shared_key_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t local_private_key;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;
    uapi_drv_cipher_pke_data_t output_shared_key = {.length = shared_key_length, .data = shared_key};

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_private_key(type, bits, private_key, private_key_length, &local_private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_private_key error!\n");

    ret = psa_get_ecc_public_key(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), bits, public_key,
        public_key_length, &local_public_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_public_key error!\n");

#ifdef SECURITY_ENHANCEMENT
    ret = (psa_status_t)uapi_drv_cipher_pke_ecc_gen_ecdh_key_sec(curve_type, &local_public_key,
        &local_private_key, &output_shared_key);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_pke_ecc_gen_ecdh_key_sec error!\n");
#else
    ret = (psa_status_t)uapi_drv_cipher_pke_ecc_gen_ecdh_key(curve_type, &local_public_key,
        &local_private_key, &output_shared_key);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_pke_ecc_gen_ecdh_key error!\n");
#endif
    return psa_get_wrap_errcode(ret);
}

static psa_crypto_table_item_ptr g_ecc_curve_param[] = {
#ifdef CONFIG_PKE_SUPPORT_ECC_FIPS_P256R
    {PSA_ECC_FAMILY_SECP_R1, 256, (uint8_t *)&g_nist_p256_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_FIPS_P384R
    {PSA_ECC_FAMILY_SECP_R1, 384, (uint8_t *)&g_nist_p384_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_FIPS_P521R
    {PSA_ECC_FAMILY_SECP_R1, 521, (uint8_t *)&g_nist_p521_param},
#endif
#if defined(CONFIG_PKE_SUPPORT_ECC_BP256R)
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 256, (uint8_t *)&g_brainpool_256r1_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_BP384R
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 384, (uint8_t *)&g_brainpool_384r1_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_BP512R
    {PSA_ECC_FAMILY_BRAINPOOL_P_R1, 512, (uint8_t *)&g_brainpool_512r1_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_ECC_CURVE_25519
    {PSA_ECC_FAMILY_MONTGOMERY, 255, (uint8_t *)&g_curve_25519_param},
#endif
#ifdef CONFIG_PKE_SUPPORT_SM2
    {PSA_ECC_FAMILY_SM2, 256, (uint8_t *)&g_sm2_param},
#endif
};

static int inner_get_limit_random(unsigned char *rand, const unsigned char *limit, const unsigned int size)
{
    volatile_sec int ret = PSA_INTERNAL_FAILURE;

    (void)memset_sec(rand, size, 0, size);

    ret = (int)uapi_drv_cipher_trng_get_random_bytes(rand, size);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_trng_get_random_bytes failed\n");

    rand[0] = rand[0] % limit[0];

    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_driver_wrapper_ecc_gen_priv_key(psa_key_type_t type, psa_key_bits_t bits, uint8_t *key_data,
    uint32_t key_len)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    drv_pke_ecc_curve *ecc_curve;

    ret = psa_crypto_get_ptr_by_2index(g_ecc_curve_param, psa_crypto_array_size(g_ecc_curve_param),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, (uint8_t **)&ecc_curve);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_ptr_by_2index error!\n");

    ret = inner_get_limit_random(key_data, (uint8_t *)((uintptr_t)ecc_curve->n + ecc_curve->ksize - key_len), key_len);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_get_limit_random error!\n");

    return ret;
}

psa_status_t psa_driver_wrapper_ecc_gen_pub_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *priv_key_data,
    uint32_t priv_key_len, uint8_t *pub_key_data, uint32_t pub_key_len)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t local_private_key;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;

    psa_unused(pub_key_len);

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_private_key(type, bits, priv_key_data, priv_key_len, &local_private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_private_key error!\n");

    pub_key_data[0] = 0x04;

    local_public_key.x = pub_key_data + 1;
    local_public_key.y = (uint8_t *)((uintptr_t)pub_key_data + 1 + priv_key_len);
    local_public_key.length = priv_key_len;

#ifdef SECURITY_ENHANCEMENT
    ret = (psa_status_t)uapi_drv_cipher_pke_ecc_gen_key_sec(curve_type, &local_private_key, &local_private_key,
        &local_public_key);
#else
    ret = (psa_status_t)uapi_drv_cipher_pke_ecc_gen_key(curve_type, &local_private_key, &local_private_key,
        &local_public_key);
#endif
    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_ecc_check_on_curve(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;
    bool is_on_curve = false;

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_public_key(type, bits, key_data, key_len, &local_public_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_public_key error!\n");

    ret = (psa_status_t)uapi_drv_cipher_pke_check_dot_on_curve(curve_type, &local_public_key, &is_on_curve);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_pke_check_dot_on_curve error!\n");

    if (!is_on_curve) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_ecc_sign(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_data_t local_private_key;
    uapi_drv_cipher_pke_data_t local_hash;
    uapi_drv_cipher_pke_ecc_sig_t local_sign;

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_private_key(type, bits, key_data, key_len, &local_private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_private_key error!\n");

    ret = psa_get_ecc_sign(type, bits, signature, signature_size, &local_sign);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_sign error!\n");

    local_hash.data = (uint8_t *)hash;
    local_hash.length = hash_length;

    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2) {
        ret = (psa_status_t)uapi_drv_cipher_pke_sm2_sign(&local_private_key, &local_hash, &local_sign);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_sm2_sign error!\n");
    } else {
#ifdef SECURITY_ENHANCEMENT
        ret = (psa_status_t)uapi_drv_cipher_pke_ecdsa_sign_sec(curve_type, &local_private_key, &local_hash, &local_sign);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_ecdsa_sign_sec error!\n");
#else
        ret = (psa_status_t)uapi_drv_cipher_pke_ecdsa_sign(curve_type, &local_private_key, &local_hash, &local_sign);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_ecdsa_sign error!\n");
#endif
    }

    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_ecc_verify(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;
    uapi_drv_cipher_pke_data_t local_hash;
    uapi_drv_cipher_pke_ecc_sig_t local_sign;

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_public_key(type, bits, key_data, key_len, &local_public_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_public_key error!\n");

    ret = psa_get_ecc_sign(type, bits, signature, signature_size, &local_sign);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_sign error!\n");

    local_hash.data = (uint8_t *)hash;
    local_hash.length = hash_length;

    if (curve_type == UAPI_DRV_CIPHER_PKE_ECC_TYPE_SM2) {
        ret = (psa_status_t)uapi_drv_cipher_pke_sm2_verify(
            (const uapi_drv_cipher_pke_ecc_point_t *)&local_public_key,
            (const uapi_drv_cipher_pke_data_t *)&local_hash,
            (const uapi_drv_cipher_pke_ecc_sig_t *)&local_sign);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_sm2_verify error!\n");
    } else {
        ret = (psa_status_t)uapi_drv_cipher_pke_ecdsa_verify(curve_type,
            (const uapi_drv_cipher_pke_ecc_point_t *)&local_public_key,
            (const uapi_drv_cipher_pke_data_t *)&local_hash,
            (const uapi_drv_cipher_pke_ecc_sig_t *)&local_sign);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_pke_ecdsa_verify error!\n");
    }

    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_sm2_encrypt(const uint8_t *key_data, uint32_t key_len, const uint8_t *input,
    uint32_t input_length, uint8_t *output, uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;
    uapi_drv_cipher_pke_data_t plain_text = {.length = input_length, .data = (uint8_t *)input};
    uapi_drv_cipher_pke_data_t cipher_text = {.length = output_length, .data = (uint8_t *)output};

    ret = psa_get_ecc_public_key(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2), 256,  /* 256: curve param bits */
        key_data, key_len, &local_public_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_public_key error!\n");

    ret = (psa_status_t)uapi_drv_cipher_pke_sm2_public_encrypt(&local_public_key, &plain_text, &cipher_text);
    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_sm2_decrypt(const uint8_t *key_data, uint32_t key_len, const uint8_t *input,
    uint32_t input_length, uint8_t *output, uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_data_t local_private_key;
    uapi_drv_cipher_pke_data_t cipher_text = {.length = input_length, .data = (uint8_t *)input};
    uapi_drv_cipher_pke_data_t plain_text = {.length = output_length, .data = (uint8_t *)output};

    ret = psa_get_ecc_private_key(PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2), 256,    /* 256: curve param bits */
        key_data, key_len, &local_private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_private_key error!\n");

    ret = (psa_status_t)uapi_drv_cipher_pke_sm2_private_decrypt(&local_private_key, &cipher_text, &plain_text);
    return psa_get_wrap_errcode(ret);
}

#ifdef CONFIG_PKE_EC_ELGAMAL_CRYPT_SUPPORT
psa_status_t psa_driver_wrapper_ec_elgamal_encrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
                                                   uint32_t key_len, const uint8_t *input, uint32_t input_length,
                                                   uint8_t *output, uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uint32_t curve_type;
    uapi_drv_cipher_pke_ecc_point_t local_public_key;
    uapi_drv_cipher_pke_ecc_point_t local_c1;
    uapi_drv_cipher_pke_data_t local_c2;
    uapi_drv_cipher_pke_data_t local_plain_text;

    psa_unused(output_length);

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_public_key(type, bits, key_data, key_len, &local_public_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_public_key error!\n");

    psa_get_ecc_c1_and_c2(bits, output, &local_c1, &local_c2);
    local_plain_text.data = (uint8_t *)input;
    local_plain_text.length = input_length;

    ret = (int32_t)uapi_drv_cipher_pke_ec_elgamal_encrypt(curve_type,
        (const uapi_drv_cipher_pke_data_t *)&local_plain_text,
        (const uapi_drv_cipher_pke_ecc_point_t *)&local_public_key,
        (uapi_drv_cipher_pke_ecc_point_t *)&local_c1,
        (uapi_drv_cipher_pke_data_t *)&local_c2);
    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_ec_elgamal_decrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
                                                   uint32_t key_len, const uint8_t *input, uint32_t input_length,
                                                   uint8_t *output, uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uint32_t curve_type;
    uapi_drv_cipher_pke_data_t local_private_key;
    uapi_drv_cipher_pke_ecc_point_t local_c1;
    uapi_drv_cipher_pke_data_t local_c2;
    uapi_drv_cipher_pke_data_t local_output_text;
    psa_unused(input_length);

    ret = psa_crypto_get_value_by_2index(g_ecc_curve_type, psa_crypto_array_size(g_ecc_curve_type),
        PSA_KEY_TYPE_ECC_GET_FAMILY(type), bits, &curve_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_2index error!\n");

    ret = psa_get_ecc_private_key(type, bits, key_data, key_len, &local_private_key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_ecc_private_key error!\n");
 
    psa_get_ecc_c1_and_c2(bits, (uint8_t *)input, &local_c1, &local_c2);
    local_output_text.data = output;
    local_output_text.length = output_length;

    ret = (int32_t)uapi_drv_cipher_pke_ec_elgamal_decrypt(curve_type,
        (const uapi_drv_cipher_pke_data_t *)&local_private_key,
        (const uapi_drv_cipher_pke_ecc_point_t *)&local_c1,
        (const uapi_drv_cipher_pke_data_t *)&local_c2,
        (uapi_drv_cipher_pke_data_t *)&local_output_text);
    return psa_get_wrap_errcode(ret);
}
#else
psa_status_t psa_driver_wrapper_ec_elgamal_encrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
                                                   uint32_t key_len, const uint8_t *input, uint32_t input_length,
                                                   uint8_t *output, uint32_t output_length)
{
    (void)type;
    (void)bits;
    (void)key_data;
    (void)key_len;
    (void)input;
    (void)input_length;
    (void)output;
    (void)output_length;
    return ERRCODE_NOT_SUPPORT;
}

psa_status_t psa_driver_wrapper_ec_elgamal_decrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
                                                   uint32_t key_len, const uint8_t *input, uint32_t input_length,
                                                   uint8_t *output, uint32_t output_length)
{
    (void)type;
    (void)bits;
    (void)key_data;
    (void)key_len;
    (void)input;
    (void)input_length;
    (void)output;
    (void)output_length;
    return ERRCODE_NOT_SUPPORT;
}
#endif
