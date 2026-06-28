/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto dispatch inner code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "psa_crypto_wrap.h"

#include "dispatch_psa_crypto_inner.h"

static psa_key_slot_t g_psa_key_slot[PSA_KEY_SLOT_SIZE] = {0};

psa_status_t psa_lock_key_slot(psa_key_id_t *volatile_key_id, psa_key_slot_t **pp_slot)
{
    int32_t i;
    psa_key_slot_t *p_selected_slot = NULL;

    for (i = 0; i < PSA_KEY_SLOT_SIZE; i++) {
        if (!g_psa_key_slot[i].is_lock) {
            g_psa_key_slot[i].is_lock = true;
            p_selected_slot = &g_psa_key_slot[i];
            break;
        }
    }
    if (p_selected_slot == NULL) {
        return PSA_ERR_KEY_SLOT_IS_FULL;
    }

    *volatile_key_id = i + 1;
    *pp_slot = p_selected_slot;
    return PSA_INTERNAL_SUCCESS;
}

void psa_unlock_key_slot(psa_key_slot_t *p_slot)
{
    memset_sec(p_slot, sizeof(psa_key_slot_t), 0, sizeof(psa_key_slot_t));
}

psa_status_t psa_get_key_slot(psa_key_id_t key, psa_key_slot_t **pp_slot)
{
    int32_t i;
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_slot_t *p_selected_slot = NULL;

    if (key == 0) {
        return PSA_ERROR_INVALID_HANDLE;
    }

    /* try to find key in ram */
    for (i = 0; i < PSA_KEY_SLOT_SIZE; i++) {
        if (g_psa_key_slot[i].attr.key == key) {
            p_selected_slot = &g_psa_key_slot[i];
            break;
        }
    }
    if (p_selected_slot != NULL) {
        *pp_slot = p_selected_slot;
        return PSA_INTERNAL_SUCCESS;
    }

    /* try to find key in secure storage */
    ret = psa_recovery_persistent_key(key, pp_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_recovery_persistent_key error!\n");

    return ret;
}

psa_status_t psa_alloc_buffer_to_slot(psa_key_slot_t *p_slot, size_t data_length)
{
    if (p_slot->key.data != NULL) {
        return PSA_ERROR_ALREADY_EXISTS;
    }

    p_slot->key.data = psa_crypto_malloc(data_length);
    if (p_slot->key.data == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    p_slot->key.bytes = data_length;
    return PSA_INTERNAL_SUCCESS;
}

static psa_status_t psa_key_algorithm_permits(psa_algorithm_t slot_alg, psa_algorithm_t alg_to_check)
{
    uint32_t check_len;

    if (slot_alg == alg_to_check) {
        return PSA_INTERNAL_SUCCESS;
    }

    if (PSA_ALG_IS_AEAD(slot_alg) &&
        ((slot_alg & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG) == PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG)) {
        /* get real process len */
        if (PSA_ALG_AEAD_GET_TAG_LENGTH(alg_to_check) != 0) {
            check_len = PSA_ALG_AEAD_GET_TAG_LENGTH(alg_to_check);
        } else {
            check_len = PSA_AEAD_DEFAULT_TAG_SIZE;
        }

        /* check process len */
        if (check_len < PSA_ALG_AEAD_GET_TAG_LENGTH(slot_alg)) {
            return PSA_ERROR_NOT_PERMITTED;
        }
        return PSA_INTERNAL_SUCCESS;
    }

    if (PSA_ALG_IS_MAC(slot_alg) &&
        ((slot_alg & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG) == PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG)) {
        /* get real process len */
        if (PSA_MAC_TRUNCATED_LENGTH(alg_to_check) != 0) {
            check_len = PSA_MAC_TRUNCATED_LENGTH(alg_to_check);
        } else {
            check_len = ((alg_to_check == PSA_ALG_CMAC || alg_to_check == PSA_ALG_CBC_MAC) ?
                PSA_CMAC_SIZE : PSA_HMAC_SHA256_SIZE);
        }

        /* check process len */
        if (check_len < PSA_MAC_TRUNCATED_LENGTH(slot_alg)) {
            return PSA_ERROR_NOT_PERMITTED;
        }
        return PSA_INTERNAL_SUCCESS;
    }
    return PSA_ERROR_NOT_PERMITTED;
}

/*   usage,                       alg,                                               type */
static const psa_crypto_table_item_pro g_usage_alg_type[] = {
    // mac
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_HMAC(PSA_ALG_SHA_256),                     PSA_KEY_TYPE_HMAC},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_HMAC(PSA_ALG_SM3),                         PSA_KEY_TYPE_HMAC},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_CBC_MAC,                                   PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_CMAC,                                      PSA_KEY_TYPE_AES},

    // ecc sign
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1)},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_ECDSA(PSA_ALG_SM3),
        PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2)},

    // ecc verify
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1)},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_ECDSA(PSA_ALG_SM3),
        PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2)},

    // rsa sign
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),        PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),                  PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_SIGN_HASH,     PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256),         PSA_KEY_TYPE_RSA_KEY_PAIR},

    // rsa verify
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),        PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),        PSA_KEY_TYPE_RSA_PUBLIC_KEY},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),                  PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),                  PSA_KEY_TYPE_RSA_PUBLIC_KEY},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256),         PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_VERIFY_HASH,   PSA_ALG_RSA_PSS_ANY_SALT(PSA_ALG_SHA_256),         PSA_KEY_TYPE_RSA_PUBLIC_KEY},

    // symc encrypt
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_ECB_NO_PADDING,                            PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CBC_NO_PADDING,                            PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CTR,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CCM,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_GCM,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_ECB_NO_PADDING,                            PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CBC_NO_PADDING,                            PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CTR,                                       PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_CCM,                                       PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_GCM,                                       PSA_KEY_TYPE_SM4},

    // symc decrypt
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_ECB_NO_PADDING,                            PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CBC_NO_PADDING,                            PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CTR,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CCM,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_GCM,                                       PSA_KEY_TYPE_AES},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_ECB_NO_PADDING,                            PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CBC_NO_PADDING,                            PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CTR,                                       PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_CCM,                                       PSA_KEY_TYPE_SM4},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_GCM,                                       PSA_KEY_TYPE_SM4},

    // rsa encrypt
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_RSA_PKCS1V15_CRYPT,                        PSA_KEY_TYPE_RSA_PUBLIC_KEY},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_RSA_PKCS1V15_CRYPT,                        PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),                 PSA_KEY_TYPE_RSA_PUBLIC_KEY},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),                 PSA_KEY_TYPE_RSA_KEY_PAIR},

    // rsa decrypt
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_RSA_PKCS1V15_CRYPT,                        PSA_KEY_TYPE_RSA_KEY_PAIR},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),                 PSA_KEY_TYPE_RSA_KEY_PAIR},

    // ec_elgamal encrypt
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_BRAINPOOL_P_R1)},
    {PSA_KEY_USAGE_ENCRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2)},

    // ec_elgamal decrypt
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1)},
    {PSA_KEY_USAGE_DECRYPT,       PSA_ALG_EC_ELGAMAL, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2)},

    // ecdh
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_ECDH,       PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_ECDH,       PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_BRAINPOOL_P_R1)},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_ECDH,       PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2)},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_ECDH,       PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY)},
    // ffdh
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_FFDH,       PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919)},

    // pbkdf2
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),       PSA_KEY_TYPE_DERIVE},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_PBKDF2_HMAC(PSA_ALG_SM3),           PSA_KEY_TYPE_DERIVE},

    // hkdf
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_HKDF(PSA_ALG_SHA_256),              PSA_KEY_TYPE_DERIVE},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_HKDF(PSA_ALG_SM3),                  PSA_KEY_TYPE_DERIVE},

    // spake2+
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256),      PSA_KEY_TYPE_DERIVE},
    // jpake
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_JPAKE(PSA_ALG_SHA_256),             PSA_KEY_TYPE_DERIVE},
    {PSA_KEY_USAGE_DERIVE,        PSA_ALG_JPAKE(PSA_ALG_SM3),             PSA_KEY_TYPE_DERIVE},
};

static int32_t psa_crypto_check_key_usage_algorithm_type_match(const psa_crypto_table_item_pro *table,
    uint32_t table_size, uint32_t index1, uint32_t index2, uint32_t index3)
{
    const psa_crypto_table_item_pro *item = NULL;
    uint32_t i;

    psa_crypto_chk_return(table == NULL, PSA_ERROR_INVALID_ARGUMENT, "table is NULL\n");

    /* index1 is usage */
    for (i = 0; i < table_size; i++) {
        item = &table[i];
        if (((item->index1 & index1) == item->index1) && item->index2 == index2 && item->value == index3) {
            return PSA_INTERNAL_SUCCESS;
        }
    }
    return PSA_ERROR_INVALID_ARGUMENT;
}

psa_status_t psa_key_usage_algorithm_type_permits(psa_key_usage_t usage, psa_algorithm_t alg, psa_key_type_t type)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_algorithm_t loacl_alg;

    if (PSA_ALG_IS_AEAD(alg)) {
        loacl_alg = PSA_ALG_AEAD_GET_ALG(alg);
    } else if (PSA_ALG_IS_MAC(alg)) {
        loacl_alg = PSA_ALG_MAC_GET_ALG(alg);
    } else {
        loacl_alg = alg;
    }
    ret = psa_crypto_check_key_usage_algorithm_type_match(g_usage_alg_type, psa_crypto_array_size(g_usage_alg_type),
        usage, loacl_alg, type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, PSA_ERROR_INVALID_ARGUMENT, "psa_crypto_check_match_by_3index error!\n");

    return ret;
}

psa_status_t psa_get_and_lock_key_slot_with_policy(psa_key_id_t key, psa_key_slot_t **pp_slot,
    psa_key_usage_t usage_to_check, psa_algorithm_t alg_to_check)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_slot_t *p_slot = NULL;

    ret = psa_get_key_slot(key, &p_slot);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_key_slot error!\n");

    /* check if slot->usage and usage_to_check is match */
    if ((p_slot->attr.policy.usage & usage_to_check) != usage_to_check) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    if (alg_to_check != 0) {
        /* check if slot->alg and alg_to_check is match */
        ret = psa_key_algorithm_permits(p_slot->attr.policy.alg, alg_to_check);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_key_algorithm_permits error!\n");

        /* check if usage & alg & type is match */
        ret = psa_key_usage_algorithm_type_permits(p_slot->attr.policy.usage, p_slot->attr.policy.alg,
            p_slot->attr.type);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_key_usage_algorithm_type_permits error!\n");
    }

    *pp_slot = p_slot;
    return ret;
}
