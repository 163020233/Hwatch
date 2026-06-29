/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto dispatch key manage code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "dispatch_psa_crypto.h"

#include "dispatch_psa_crypto_inner.h"
#include "psa_crypto_common.h"
#include "psa_crypto_wrap.h"

#ifdef CONFIG_PSA_CRYPTO_SUPPORT
int32_t dispatch_psa_get_key_attr(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_get_key_attr_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot = NULL;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_GET_KEY_ATTR);

    ret = psa_get_key_slot(local_arg->key, &p_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_key_slot error!\n");

    local_arg->attributes = p_slot->attr;
    return ret;
}

static const psa_key_type_t g_key_type[] = {
    PSA_KEY_TYPE_RAW_DATA,
    PSA_KEY_TYPE_HMAC,
    PSA_KEY_TYPE_AES, PSA_KEY_TYPE_SM4,
    PSA_KEY_TYPE_RSA_PUBLIC_KEY, PSA_KEY_TYPE_RSA_KEY_PAIR,
    PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1),
    PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2),
    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1),
    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY),
    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2),
    PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919),
    PSA_KEY_TYPE_DERIVE,
};

static psa_status_t psa_validate_key_type(psa_key_type_t type)
{
    unsigned int i;

    for (i = 0; i < psa_crypto_array_size(g_key_type); i++) {
        if (type == g_key_type[i]) {
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_NOT_SUPPORTED;
}

static const psa_crypto_table_item g_bits_type[] = {
    /* aes */
    {PSA_KEY_TYPE_AES, PSA_KEY_BITS_AES_128_BIT},
    {PSA_KEY_TYPE_AES, PSA_KEY_BITS_AES_192_BIT},
    {PSA_KEY_TYPE_AES, PSA_KEY_BITS_AES_256_BIT},
    /* sm4 */
    {PSA_KEY_TYPE_SM4, PSA_KEY_BITS_AES_128_BIT},
    /* rsa */
    {PSA_KEY_TYPE_RSA_PUBLIC_KEY, PSA_KEY_BITS_RSA_1024_BIT},
    {PSA_KEY_TYPE_RSA_PUBLIC_KEY, PSA_KEY_BITS_RSA_2048_BIT},
    {PSA_KEY_TYPE_RSA_PUBLIC_KEY, PSA_KEY_BITS_RSA_3072_BIT},
    {PSA_KEY_TYPE_RSA_PUBLIC_KEY, PSA_KEY_BITS_RSA_4096_BIT},
    {PSA_KEY_TYPE_RSA_KEY_PAIR, PSA_KEY_BITS_RSA_1024_BIT},
    {PSA_KEY_TYPE_RSA_KEY_PAIR, PSA_KEY_BITS_RSA_2048_BIT},
    {PSA_KEY_TYPE_RSA_KEY_PAIR, PSA_KEY_BITS_RSA_3072_BIT},
    {PSA_KEY_TYPE_RSA_KEY_PAIR, PSA_KEY_BITS_RSA_4096_BIT},
    /* dh */
    {PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_1024_BIT},
    {PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_2048_BIT},
    {PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_3072_BIT},
    {PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_4096_BIT},
    {PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_1024_BIT},
    {PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_2048_BIT},
    {PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_3072_BIT},
    {PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919), PSA_KEY_BITS_RSA_4096_BIT},
    /* nist */
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_256_BIT},
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_384_BIT},
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_521_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_256_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_384_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), PSA_KEY_BITS_ECC_521_BIT},
    /* bp */
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_256_BIT},
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_384_BIT},
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_512_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_256_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_384_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1), PSA_KEY_BITS_ECC_512_BIT},
    /* sm2 */
    {PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2), PSA_KEY_BITS_ECC_256_BIT},
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2), PSA_KEY_BITS_ECC_256_BIT},
    /* curve25519 */
    {PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY), PSA_KEY_BITS_ECC_255_BIT},
};

static psa_status_t psa_validate_key_bits(psa_key_bits_t bits, psa_key_type_t type)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    if (type == PSA_KEY_TYPE_HMAC || type == PSA_KEY_TYPE_RAW_DATA || type == PSA_KEY_TYPE_DERIVE) {    // check hmac
        if (bits > PSA_MAX_KEY_BITS) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        return PSA_INTERNAL_SUCCESS;
    }

    ret = psa_crypto_check_match_by_2index(g_bits_type, psa_crypto_array_size(g_bits_type), type, bits);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_check_match_by_2index error!\n");

    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t psa_validate_key_lifetime(psa_key_lifetime_t lifetime)
{
    if (PSA_KEY_LIFETIME_GET_LOCATION(lifetime) != PSA_KEY_LOCATION_LOCAL_STORAGE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) != PSA_KEY_PERSISTENCE_VOLATILE &&
        PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) != PSA_KEY_PERSISTENCE_DEFAULT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return PSA_INTERNAL_SUCCESS;
}

psa_key_usage_t g_key_usage[] = {
    PSA_KEY_USAGE_ENCRYPT, PSA_KEY_USAGE_DECRYPT, PSA_KEY_USAGE_SIGN_MESSAGE, PSA_KEY_USAGE_VERIFY_MESSAGE,
    PSA_KEY_USAGE_SIGN_HASH, PSA_KEY_USAGE_VERIFY_HASH, PSA_KEY_USAGE_DERIVE
};

static psa_status_t psa_validate_key_usage(psa_key_usage_t usage)
{
    unsigned int i;
    unsigned int usage_all = 0;

    if (usage == 0) {
        return PSA_INTERNAL_SUCCESS;
    }

    for (i = 0; i < psa_crypto_array_size(g_key_usage); i++) {
        usage_all |= g_key_usage[i];
    }
    if ((usage & usage_all) != usage) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    return PSA_INTERNAL_SUCCESS;
}

static const psa_algorithm_t g_key_algorithm[] = {
    PSA_ALG_SHA_256, PSA_ALG_SM3, /* hash */
    PSA_ALG_HMAC(PSA_ALG_SHA_256), PSA_ALG_HMAC(PSA_ALG_SM3), PSA_ALG_CBC_MAC, PSA_ALG_CMAC,   /* mac */
    PSA_ALG_ECB_NO_PADDING, PSA_ALG_CBC_NO_PADDING, PSA_ALG_CFB, PSA_ALG_CTR, PSA_ALG_CCM,
    PSA_ALG_GCM,  /* symc */
    PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256), PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),
    PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256),    /* rsa sign & verify */
    PSA_ALG_ECDSA_ANY,
    PSA_ALG_ECDSA(PSA_ALG_SHA_256),    /* ecc sign & verify */
    PSA_ALG_ECDSA(PSA_ALG_SM3),    /* sm2 sign & verify */
    PSA_ALG_ECDH,    /* ecdh */
    PSA_ALG_RSA_PKCS1V15_CRYPT, PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),    /* rsa encrypt & decrypt */
    PSA_ALG_FFDH,    /* dh */
    PSA_ALG_HKDF(PSA_ALG_SHA_256), PSA_ALG_HKDF(PSA_ALG_SM3), PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    PSA_ALG_PBKDF2_HMAC(PSA_ALG_SM3),  /* kdf */
    PSA_ALG_EC_ELGAMAL, /* ec_elgamal */
    PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256),
    PSA_ALG_JPAKE(PSA_ALG_SHA_256),
    PSA_ALG_JPAKE(PSA_ALG_SM3),
};

static psa_status_t psa_validate_key_algorithm(psa_algorithm_t alg)
{
    unsigned int i;
    psa_algorithm_t local_alg;

    if (PSA_ALG_IS_AEAD(alg)) {
        local_alg = PSA_ALG_AEAD_GET_ALG(alg);
    } else if (PSA_ALG_IS_MAC(alg)) {
        local_alg = PSA_ALG_MAC_GET_ALG(alg);
    } else {
        local_alg = alg;
    }

    for (i = 0; i < psa_crypto_array_size(g_key_algorithm); i++) {
        if (local_alg == g_key_algorithm[i]) {
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_NOT_SUPPORTED;
}

/* volatile key_id: [1, PSA_KEY_SLOT_SIZE] (start from 1)
   persistent key id: [PSA_PERSISTENT_KEY_ID_MIN, PSA_PERSISTENT_KEY_ID_MAX) */
static psa_status_t psa_validate_key_id(psa_key_id_t key, psa_key_lifetime_t lifetime)
{
    if (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_DEFAULT) {
        if (key < PSA_PERSISTENT_KEY_ID_MIN || key >= PSA_PERSISTENT_KEY_ID_MAX) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    }
    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_validate_key_attributes(const psa_key_attributes_t *attributes)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    ret = psa_validate_key_type(attributes->type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_type error!\n");

    if (attributes->bits != 0) {
        ret = psa_validate_key_bits(attributes->bits, attributes->type);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_bits error!\n");
    }

    ret = psa_validate_key_lifetime(attributes->lifetime);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_lifetime error!\n");

    ret = psa_validate_key_algorithm(attributes->policy.alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_algorithm error!\n");

    ret = psa_validate_key_usage(attributes->policy.usage);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_usage error!\n");

    ret = psa_validate_key_id(attributes->key, attributes->lifetime);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_id error!\n");

    return ret;
}

static psa_status_t inner_psa_get_unstructured_key_bits(psa_key_type_t type, size_t data_length, psa_key_bits_t *bits)
{
    size_t data_bits = data_length * PSA_BYTES_IN_BIT;

    switch (type) {
        case PSA_KEY_TYPE_RAW_DATA:
        case PSA_KEY_TYPE_HMAC:
        case PSA_KEY_TYPE_DERIVE:
            if (data_bits > PSA_MAX_KEY_BITS) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = data_bits;
            break;
        case PSA_KEY_TYPE_AES:
            if (data_bits != PSA_KEY_BITS_AES_128_BIT && data_bits != PSA_KEY_BITS_AES_192_BIT &&
                data_bits != PSA_KEY_BITS_AES_256_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = data_bits;
            break;
        case PSA_KEY_TYPE_SM4:
            if (data_bits != PSA_KEY_BITS_AES_128_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = data_bits;
            break;
    }
    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t inner_psa_get_ecc_public_key_bits(psa_key_type_t type, size_t data_length, psa_key_bits_t *bits)
{
    size_t data_bits = data_length * PSA_BYTES_IN_BIT;

    switch (type) {
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1):
            if (data_bits != PSA_ECC_256_PUBLIC_KEY_DATA_BIT && data_bits != PSA_ECC_384_PUBLIC_KEY_DATA_BIT &&
                data_bits != PSA_ECC_521_PUBLIC_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = (data_bits == PSA_ECC_521_PUBLIC_KEY_DATA_BIT) ? PSA_KEY_BITS_ECC_521_BIT :
                PSA_ECC_PUBLIC_KEY_GET_BITS(data_bits);
            break;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1):
            if (data_bits != PSA_ECC_256_PUBLIC_KEY_DATA_BIT && data_bits != PSA_ECC_384_PUBLIC_KEY_DATA_BIT &&
                data_bits != PSA_ECC_512_PUBLIC_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = PSA_ECC_PUBLIC_KEY_GET_BITS(data_bits);
            break;
        case PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2):
            if (data_bits != PSA_ECC_256_PUBLIC_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = PSA_ECC_PUBLIC_KEY_GET_BITS(data_bits);
            break;
    }
    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t inner_psa_get_ecc_private_key_bits(psa_key_type_t type, size_t data_length, psa_key_bits_t *bits)
{
    size_t data_bits = data_length * PSA_BYTES_IN_BIT;

    switch (type) {
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1):
            if (data_bits != PSA_ECC_256_PRIVATE_KEY_DATA_BIT && data_bits != PSA_ECC_384_PRIVATE_KEY_DATA_BIT &&
                data_bits != PSA_ECC_521_PRIVATE_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = (data_bits == PSA_ECC_521_PRIVATE_KEY_DATA_BIT) ? PSA_KEY_BITS_ECC_521_BIT : data_bits;
            break;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1):
            if (data_bits != PSA_ECC_256_PRIVATE_KEY_DATA_BIT && data_bits != PSA_ECC_384_PRIVATE_KEY_DATA_BIT &&
                data_bits != PSA_ECC_512_PRIVATE_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
             *bits = data_bits;
            break;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2):
            if (data_bits != PSA_ECC_256_PRIVATE_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = data_bits;
            break;
        case PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY):
            if (data_bits != PSA_ECC_256_PRIVATE_KEY_DATA_BIT) {
                return PSA_ERROR_NOT_SUPPORTED;
            }
            *bits = PSA_KEY_BITS_ECC_255_BIT;
            break;
    }
    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t inner_psa_get_ecc_key_bits(psa_key_type_t type, size_t data_length, psa_key_bits_t *bits)
{
    if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type)) {
        return inner_psa_get_ecc_public_key_bits(type, data_length, bits);
    } else {
        return inner_psa_get_ecc_private_key_bits(type, data_length, bits);
    }
}

static psa_status_t inner_psa_get_rsa_key_bits(psa_key_type_t type, size_t data_length, psa_key_bits_t *bits)
{
    size_t local_bits;

    if (type == PSA_KEY_TYPE_RSA_KEY_PAIR) {
        local_bits = (data_length - PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH) / 2 * PSA_BYTES_IN_BIT; /* 2: x + y */
    } else {
        local_bits = (data_length - PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH) * PSA_BYTES_IN_BIT;
    }

    if (local_bits != PSA_KEY_BITS_RSA_1024_BIT && local_bits != PSA_KEY_BITS_RSA_2048_BIT &&
        local_bits != PSA_KEY_BITS_RSA_3072_BIT && local_bits != PSA_KEY_BITS_RSA_4096_BIT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    *bits = local_bits;
    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t inner_psa_get_dh_key_bits(size_t data_length, psa_key_bits_t *bits)
{
    size_t data_bits = data_length * PSA_BYTES_IN_BIT;

    if (data_bits != PSA_KEY_BITS_RSA_1024_BIT && data_bits != PSA_KEY_BITS_RSA_2048_BIT &&
        data_bits != PSA_KEY_BITS_RSA_3072_BIT && data_bits != PSA_KEY_BITS_RSA_4096_BIT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    *bits = data_bits;
    return PSA_INTERNAL_SUCCESS;
}

static int32_t psa_get_key_bits_from_key_data_length(psa_key_type_t type, size_t data_length,
    psa_key_bits_t *bits)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        ret = inner_psa_get_unstructured_key_bits(type, data_length, bits);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_psa_get_raw_key_bits error!\n");
    } else if (PSA_KEY_TYPE_IS_ECC(type)) {
        ret = inner_psa_get_ecc_key_bits(type, data_length, bits);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_psa_get_ecc_key_bits error!\n");
    } else if (PSA_KEY_TYPE_IS_RSA(type)) {
        ret = inner_psa_get_rsa_key_bits(type, data_length, bits);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_psa_get_rsa_key_bits error!\n");
    } else {    /* PSA_KEY_TYPE_IS_DH(type) */
        ret = inner_psa_get_dh_key_bits(data_length, bits);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_psa_get_dh_key_bits error!\n");
    }
    return PSA_INTERNAL_SUCCESS;
}

/* if param from ree, data can not be tee buffer, if param from tee inner, data can be tee buffer
   key must be ree-tee shared buffer both */
int32_t tee_psa_import_key(psa_key_attributes_t *attributes, const uint8_t *data, size_t data_length,
    psa_key_id_t *key)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_slot_t *p_slot;
    psa_key_id_t volatile_key_id;
    psa_key_attributes_t local_attributes = *attributes;
    psa_key_bits_t bits;

    /* validate every paranm in attributes struct */
    ret = psa_validate_key_attributes(&local_attributes);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_attributes error!\n");

    /* get key bits */
    ret = psa_get_key_bits_from_key_data_length(local_attributes.type, data_length, &bits);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_key_bits_from_key_data_length error!\n");

    if (local_attributes.bits == 0) {
        local_attributes.bits = bits;
    } else {
        if (local_attributes.bits != bits) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    /* if ecc pulic key, check on curve */
    if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(local_attributes.type)) {
        ret = psa_driver_wrapper_ecc_check_on_curve(local_attributes.type, local_attributes.bits, data, data_length);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ecc_check_on_curve error!\n");
    }

    /* if persistent key already exist, return failed */
    if (!PSA_KEY_LIFETIME_IS_VOLATILE(local_attributes.lifetime)) {
        ret = psa_get_key_slot(local_attributes.key, &p_slot);
        psa_crypto_chk_return(ret == PSA_INTERNAL_SUCCESS, PSA_ERROR_ALREADY_EXISTS, "Persistent key already exist!\n");
    }

    /* lock free slot and get key_id if volatile */
    ret = psa_lock_key_slot(&volatile_key_id, &p_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_lock_key_slot error!\n");

    p_slot->attr = local_attributes;
    if (PSA_KEY_LIFETIME_IS_VOLATILE(p_slot->attr.lifetime)) {
        p_slot->attr.key = volatile_key_id;
    }

    /* malloc buffer to p_slot->key.data */
    ret = psa_alloc_buffer_to_slot(p_slot, data_length);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, psa_unlock_key_slot, ret,
        "psa_alloc_buffer_to_slot error!\n");

    /* copy key to p_slot->key.data */
    ret = memcpy_sec(p_slot->key.data, p_slot->key.bytes, data, data_length);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, psa_free_buffer_to_slot, PSA_ERR_MEMCPY_S, "memcpy_sec error!\n");

    /* save key to secure storage if persistent */
    if (PSA_KEY_LIFETIME_GET_PERSISTENCE(local_attributes.lifetime) == PSA_KEY_PERSISTENCE_DEFAULT) {
        ret = psa_save_persistent_key(&local_attributes, p_slot->key.data, p_slot->key.bytes);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, clean_key, ret, "psa_save_persistent_key error!\n");
    }

    *key = p_slot->attr.key;
    return ret;

clean_key:
    memset_s(p_slot->key.data, p_slot->key.bytes, 0, p_slot->key.bytes);
psa_free_buffer_to_slot:
    psa_crypto_free(p_slot->key.data);
psa_unlock_key_slot:
    psa_unlock_key_slot(p_slot);
    return ret;
}

int32_t dispatch_psa_import_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_import_key_arg_t *local_arg = arg;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_IMPORT_KEY);
    psa_crypto_null_param_chk(local_arg->data);
    psa_crypto_param_chk(local_arg->data_length == 0 || (local_arg->data_length > PSA_MAX_KEY_DATA_LENGTH));

    /* buffer check */
    psa_crypto_param_chk(check_is_not_in_tee_range((uintptr_t)local_arg->data, local_arg->data_length) != LOS_SUCCESS);

    ret = tee_psa_import_key(&local_arg->attributes, local_arg->data, local_arg->data_length, &local_arg->key);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "tee_psa_import_key error!\n");

    return ret;
}

int32_t dispatch_psa_destroy_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_destroy_key_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_DESTROY_KEY);

    ret = psa_get_key_slot(local_arg->key, &p_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_key_slot error!\n");

    /* destroy secure storage if persistent */
    if (!PSA_KEY_LIFETIME_IS_VOLATILE(p_slot->attr.lifetime)) {
        ret = psa_destroy_persistent_key(local_arg->key);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_destroy_persistent_key error!\n");
    }

    /* clean key data */
    memset_s(p_slot->key.data, p_slot->key.bytes, 0, p_slot->key.bytes);
    /* free key data */
    psa_crypto_free(p_slot->key.data);
    /* clean and unlock key slot */
    psa_unlock_key_slot(p_slot);
    return ret;
}

int32_t dispatch_psa_verify_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_verify_key_arg_t *local_arg = arg;
    uint32_t crc32;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_VERIFY_KEY);

    ret = psa_get_key_slot(local_arg->key, &p_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_key_slot error!\n");

    ret = psa_driver_wrapper_driver_crc32(p_slot->key.data, p_slot->key.bytes, &crc32);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_driver_crc32 error!\n");

    if (crc32 != local_arg->crc32) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return ret;
}

int32_t dispatch_psa_generate_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_generate_key_arg_t *local_arg = arg;
    uint8_t *key_data;
    uint32_t key_length;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_GENERATE_KEY);
    psa_crypto_param_alg_chk(local_arg->attributes.type != PSA_KEY_TYPE_HMAC &&
        local_arg->attributes.type != PSA_KEY_TYPE_AES &&
        local_arg->attributes.type != PSA_KEY_TYPE_SM4 &&
        local_arg->attributes.type != PSA_KEY_TYPE_DERIVE &&
        !PSA_KEY_TYPE_IS_ECC_KEY_PAIR(local_arg->attributes.type));

    ret = psa_validate_key_bits(local_arg->attributes.bits, local_arg->attributes.type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_validate_key_bits error!\n");

    key_length = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(local_arg->attributes.bits);
    key_data = psa_crypto_malloc(key_length);
    psa_crypto_chk_return(key_data == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");

    if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(local_arg->attributes.type)) {
        ret = psa_driver_wrapper_generate_trng(key_data, key_length);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, ret, "psa_driver_wrapper_generate_trng error!\n");
    } else {
        /* key pair only contain private key, not contain public key */
        ret = psa_driver_wrapper_ecc_gen_priv_key(local_arg->attributes.type, local_arg->attributes.bits, key_data,
            key_length);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, ret, "psa_driver_wrapper_ecc_gen_key error!\n");
    }

    ret = tee_psa_import_key(&local_arg->attributes, key_data, key_length, &local_arg->key);
    psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, clean, ret, "tee_psa_import_key error!\n");

clean:
    memset_s(key_data, key_length, 0, key_length);
free:
    psa_crypto_free(key_data);
    return ret;
}

int32_t dispatch_psa_export_public_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_export_public_key_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_EXPORT_PUBLIC_KEY);

    /* buffer check */
    psa_crypto_param_chk(check_is_not_in_tee_range((uintptr_t)local_arg->data, local_arg->data_size) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, 0, 0);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    /* 2: x + y */
    psa_crypto_param_chk(local_arg->data_size < (size_t)(PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2 + 1));

    ret = psa_driver_wrapper_ecc_gen_pub_key(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data, p_slot->key.bytes,
        local_arg->data, local_arg->data_size);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ecc_gen_pub_key error!\n");

    local_arg->data_length = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2 + 1; /* 2: x + y */

    return ret;
}
#endif
