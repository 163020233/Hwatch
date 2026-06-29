/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto dispatch algorithm code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "dispatch_psa_crypto.h"

#include "dispatch_psa_crypto_inner.h"
#include "psa_crypto_common.h"
#include "psa_crypto_wrap.h"

#ifdef CONFIG_PSA_CRYPTO_SUPPORT
int32_t dispatch_psa_generate_random(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_generate_random_arg_t *local_arg = arg;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_GENERATE_RANDOM);
    psa_crypto_null_param_chk(local_arg->output);
    psa_crypto_param_chk(local_arg->output_size > PSA_RANDOM_MAX_SIZE);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->output, local_arg->output_size) != LOS_SUCCESS);

    ret = psa_driver_wrapper_generate_trng(local_arg->output, local_arg->output_size);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_generate_trng error!\n");

    return ret;
}

int32_t dispatch_psa_cipher_set_iv(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_cipher_set_iv_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_CIPHER_SET_IV);
    psa_crypto_param_chk(local_arg->iv_length != PSA_CIPHER_IV_LEN);

    /* buffer check */
    psa_crypto_param_chk(check_is_not_in_tee_range((uintptr_t)local_arg->iv, local_arg->iv_length) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_ENCRYPT, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    ret = memcpy_s(p_slot->iv, sizeof(p_slot->iv), local_arg->iv, local_arg->iv_length);
    psa_crypto_chk_return(ret != EOK, ret, "memcpy_s error!\n");

    p_slot->is_set_iv = true;

    return PSA_INTERNAL_SUCCESS;
}

int32_t dispatch_psa_cipher_crypto(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_cipher_crypto_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_CIPHER_CRYPTO);
    psa_crypto_null_param_chk(local_arg->input, local_arg->output);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_CIPHER(local_arg->alg));
    psa_crypto_param_chk(local_arg->input_length > PSA_CRYPTO_MAX_SIZE);
    if (local_arg->alg != PSA_ALG_CTR) {
        psa_crypto_param_chk(local_arg->input_length % PSA_CIPHER_BLOCK_LEN != 0);
    }
    if (local_arg->is_encrypt) {
        psa_crypto_param_length_chk(local_arg->input_length == 0);
        psa_crypto_param_length_chk(local_arg->output_size < local_arg->input_length + PSA_CIPHER_IV_LEN);
    } else {
        psa_crypto_param_length_chk(local_arg->input_length <= PSA_CIPHER_IV_LEN);
        psa_crypto_param_length_chk(local_arg->output_size < local_arg->input_length - PSA_CIPHER_IV_LEN);
    }

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->input, local_arg->input_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->output, local_arg->output_size) != LOS_SUCCESS);

    if (local_arg->is_encrypt) {
        ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_ENCRYPT, local_arg->alg);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

        if (p_slot->is_set_iv) {
            ret = memcpy_s(local_arg->output, PSA_CIPHER_IV_LEN, p_slot->iv, sizeof(p_slot->iv));
            psa_crypto_chk_return(ret != EOK, ret, "memcpy_s error!\n");
        } else {
            ret = psa_driver_wrapper_generate_trng(local_arg->output, PSA_CIPHER_IV_LEN);
            psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_trng error!\n");
        }

        /* output = iv(16) | cipher_text */
        ret = psa_driver_wrapper_cipher_crypto(p_slot->attr.type, local_arg->alg, p_slot->key.data, p_slot->key.bytes,
            local_arg->input, local_arg->output + PSA_CIPHER_IV_LEN, local_arg->input_length, local_arg->output, true);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_cipher_crypto error!\n");

        local_arg->output_length = local_arg->input_length + PSA_CIPHER_IV_LEN;
    } else {
        ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_DECRYPT, local_arg->alg);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

        /* input = iv(16) | cipher_text */
        ret = psa_driver_wrapper_cipher_crypto(p_slot->attr.type, local_arg->alg, p_slot->key.data, p_slot->key.bytes,
            (const uint8_t *)((uintptr_t)local_arg->input + PSA_CIPHER_IV_LEN), local_arg->output,
            local_arg->input_length - PSA_CIPHER_IV_LEN, local_arg->input, false);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_cipher_crypto error!\n");

        local_arg->output_length = local_arg->input_length - PSA_CIPHER_IV_LEN;
    }
    return ret;
}

int32_t dispatch_psa_aead_encrypt(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_aead_encrypt_arg_t *local_arg = arg;
    uint32_t tag_len;
    psa_algorithm_t pure_alg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_AEAD_ENCRYPT);
    /* additional_data and plaintext(encrypt) can be NULL */
    psa_crypto_null_param_chk(local_arg->nonce, local_arg->ciphertext);
    psa_crypto_param_chk((local_arg->alg == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0)) ||
        (local_arg->alg == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)));
    psa_crypto_param_chk(local_arg->plaintext_length > PSA_CRYPTO_MAX_SIZE);
    /* get and check real alg and tag len */
    if (PSA_ALG_AEAD_GET_TAG_LENGTH(local_arg->alg) != 0) {
        tag_len = PSA_ALG_AEAD_GET_TAG_LENGTH(local_arg->alg);
        pure_alg = PSA_ALG_AEAD_GET_ALG(local_arg->alg);
    } else {
        tag_len = PSA_AEAD_DEFAULT_TAG_SIZE;
        pure_alg = local_arg->alg;
    }
    psa_crypto_param_chk((tag_len == 0) || (tag_len > PSA_AEAD_DEFAULT_TAG_SIZE));
    psa_crypto_param_alg_chk((pure_alg != PSA_ALG_CCM) && (pure_alg != PSA_ALG_GCM));
    psa_crypto_param_length_chk(local_arg->ciphertext_size < local_arg->plaintext_length + tag_len);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->nonce, local_arg->nonce_length) != LOS_SUCCESS);
    if (local_arg->additional_data != NULL) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->additional_data, local_arg->additional_data_length) != LOS_SUCCESS);
    }
    if (local_arg->plaintext != NULL) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->plaintext, local_arg->plaintext_length) != LOS_SUCCESS);
    }
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->ciphertext, local_arg->ciphertext_size) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_ENCRYPT, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    /* cipher_text = cipher_text | tag(16) */
    ret = psa_driver_wrapper_aead_crypto(p_slot->attr.type, pure_alg, p_slot->key.data, p_slot->key.bytes,
        local_arg->nonce, local_arg->nonce_length, local_arg->additional_data, local_arg->additional_data_length,
        local_arg->plaintext, local_arg->ciphertext, local_arg->plaintext_length,
        (uint8_t *)((uintptr_t)local_arg->ciphertext + local_arg->plaintext_length), tag_len, true);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_aead_crypto error!\n");

    local_arg->ciphertext_length = local_arg->plaintext_length + tag_len;
    return ret;
}

int32_t dispatch_psa_aead_decrypt(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_aead_decrypt_arg_t *local_arg = arg;
    uint32_t tag_len;
    psa_algorithm_t pure_alg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_AEAD_DECRYPT);
    /* additional_data can be NULL */
    psa_crypto_null_param_chk(local_arg->nonce, local_arg->ciphertext, local_arg->plaintext);
    psa_crypto_param_chk((local_arg->alg == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0)) ||
        (local_arg->alg == PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)));
    psa_crypto_param_chk(local_arg->ciphertext_length > PSA_CRYPTO_MAX_SIZE);
    /* get and check real alg and tag len */
    if (PSA_ALG_AEAD_GET_TAG_LENGTH(local_arg->alg) != 0) {
        tag_len = PSA_ALG_AEAD_GET_TAG_LENGTH(local_arg->alg);
        pure_alg = PSA_ALG_AEAD_GET_ALG(local_arg->alg);
    } else {
        tag_len = PSA_AEAD_DEFAULT_TAG_SIZE;
        pure_alg = local_arg->alg;
    }
    psa_crypto_param_chk((tag_len == 0) || (tag_len > PSA_AEAD_DEFAULT_TAG_SIZE));
    psa_crypto_param_alg_chk((pure_alg != PSA_ALG_CCM) && (pure_alg != PSA_ALG_GCM));
    psa_crypto_param_length_chk(local_arg->ciphertext_length < tag_len);
    psa_crypto_param_length_chk(local_arg->plaintext_size < local_arg->ciphertext_length - tag_len);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->nonce, local_arg->nonce_length) != LOS_SUCCESS);
    if (local_arg->additional_data != NULL) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->additional_data, local_arg->additional_data_length) != LOS_SUCCESS);
    }
    psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
        (uintptr_t)local_arg->ciphertext, local_arg->ciphertext_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->plaintext, local_arg->plaintext_size) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_DECRYPT, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    ret = psa_driver_wrapper_aead_crypto(p_slot->attr.type, pure_alg, p_slot->key.data, p_slot->key.bytes,
        local_arg->nonce, local_arg->nonce_length, local_arg->additional_data, local_arg->additional_data_length,
        local_arg->ciphertext, local_arg->plaintext, local_arg->ciphertext_length - tag_len,
        (uint8_t *)((uintptr_t)local_arg->ciphertext + local_arg->ciphertext_length - tag_len), tag_len, false);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_aead_crypto error!\n");

    local_arg->plaintext_length = local_arg->ciphertext_length - tag_len;
    return ret;
}

#define PSA_HASH_SHA256_BLOCK_SIZE 64
#define PSA_HASH_SHA256_HASH_SIZE 32

int32_t dispatch_psa_hash_compute(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_hash_compute_arg_t *local_arg = arg;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_HASH_COMPUTE);
    psa_crypto_null_param_chk(local_arg->input, local_arg->hash);
    psa_crypto_param_alg_chk(local_arg->alg != PSA_ALG_SHA_256 && local_arg->alg != PSA_ALG_SM3);
    psa_crypto_param_chk(local_arg->input_length == 0 || local_arg->input_length > PSA_HASH_INPUT_MAX_SIZE);
    psa_crypto_param_length_chk(local_arg->hash_size < PSA_HASH_SHA256_HASH_SIZE);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->input, local_arg->input_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->hash, local_arg->hash_size) != LOS_SUCCESS);

    ret = psa_driver_wrapper_cipher_hash_compute(local_arg->alg, local_arg->input, local_arg->input_length,
        local_arg->hash);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_cipher_hash_compute error!\n");

    local_arg->hash_length = PSA_HASH_SHA256_HASH_SIZE;
    return ret;
}

int32_t dispatch_psa_mac_compute(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_mac_compute_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;
    psa_algorithm_t pure_alg;
    uint32_t local_mac_len;
    uint8_t local_mac[32];

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_MAC_COMPUTE);
    psa_crypto_null_param_chk(local_arg->input, local_arg->mac);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_MAC(local_arg->alg));
    psa_crypto_param_chk(local_arg->input_length == 0 || local_arg->input_length > PSA_CRYPTO_MAX_SIZE);
    /* get real alg and mac len */
    if (PSA_MAC_TRUNCATED_LENGTH(local_arg->alg) != 0) {
        pure_alg = PSA_ALG_MAC_GET_ALG(local_arg->alg);
        local_mac_len = PSA_MAC_TRUNCATED_LENGTH(local_arg->alg);
    } else {
        pure_alg = local_arg->alg;
        local_mac_len = ((local_arg->alg == PSA_ALG_CMAC || local_arg->alg == PSA_ALG_CBC_MAC) ?
            PSA_CMAC_SIZE : PSA_HMAC_SHA256_SIZE);
    }
    psa_crypto_param_alg_chk((pure_alg != PSA_ALG_CMAC) && (pure_alg != PSA_ALG_CBC_MAC) &&
        (pure_alg != PSA_ALG_HMAC(PSA_ALG_SHA_256)) && (pure_alg != PSA_ALG_HMAC(PSA_ALG_SM3)));
    psa_crypto_param_chk(local_mac_len > sizeof(local_mac));
    psa_crypto_param_length_chk(local_arg->mac_size < local_mac_len);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->input, local_arg->input_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->mac, local_arg->mac_size) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_SIGN_HASH, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    if (pure_alg == PSA_ALG_HMAC(PSA_ALG_SHA_256) || pure_alg == PSA_ALG_HMAC(PSA_ALG_SM3)) {
        ret = psa_driver_wrapper_hmac(pure_alg, p_slot->key.data, p_slot->key.bytes, local_arg->input,
            local_arg->input_length, local_mac);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_hmac error!\n");
    } else if (pure_alg == PSA_ALG_CMAC) {
        ret = psa_driver_wrapper_cmac(p_slot->key.data, p_slot->key.bytes, local_arg->input,
            local_arg->input_length, local_mac);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_cmac error!\n");
    } else { /* PSA_ALG_CBC_MAC */
        ret = psa_driver_wrapper_cbc_mac(p_slot->key.data, p_slot->key.bytes, local_arg->input,
            local_arg->input_length, local_mac);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_cbc_mac error!\n");
    }

    ret = memcpy_s(local_arg->mac, local_arg->mac_size, local_mac, local_mac_len);
    psa_crypto_chk_return(ret != EOK, ret, "memcpy_s error!\n");

    local_arg->mac_length = local_mac_len;
    return PSA_INTERNAL_SUCCESS;
}

int32_t dispatch_psa_key_agreement(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_agreement_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;
    uint8_t *shared_key;
    uint32_t shared_key_len;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_KEY_AGREEMENT);
    psa_crypto_null_param_chk(local_arg->peer_key);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_KEY_AGREEMENT(local_arg->alg));
    psa_crypto_param_chk(local_arg->peer_key_length > PSA_MAX_KEY_DATA_LENGTH);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->peer_key, local_arg->peer_key_length) != LOS_SUCCESS);
    if (local_arg->output != NULL) {
        psa_crypto_param_chk(
            check_is_not_in_tee_range((uintptr_t)local_arg->output, local_arg->output_size) != LOS_SUCCESS);
    }

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->private_key, &p_slot, PSA_KEY_USAGE_DERIVE, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    shared_key_len = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits);
    shared_key = psa_crypto_malloc(shared_key_len);
    psa_crypto_chk_return(shared_key == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");

    if (local_arg->alg == PSA_ALG_ECDH) {
        /* 2: x + y */
        if (local_arg->peer_key_length != (size_t)(PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2)) {
            ret = PSA_ERROR_INVALID_ARGUMENT;
            goto clean;
        }
        if ((local_arg->output != NULL) &&
            (local_arg->output_size < (size_t)PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits))) {
            ret = PSA_ERROR_BUFFER_TOO_SMALL;
            goto clean;
        }
        ret = psa_driver_wrapper_ecdh(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data,
            p_slot->key.bytes, local_arg->peer_key, local_arg->peer_key_length, shared_key, shared_key_len);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, ret, "psa_driver_wrapper_ecdh error!\n");
    } else {
        if (local_arg->peer_key_length != (size_t)PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits)) {
            ret = PSA_ERROR_INVALID_ARGUMENT;
            goto clean;
        }
        if ((local_arg->output != NULL) &&
            (local_arg->output_size < (size_t)PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits))) {
            ret = PSA_ERROR_BUFFER_TOO_SMALL;
            goto clean;
        }
        ret = psa_driver_wrapper_ffdh(p_slot->attr.bits, p_slot->key.data,
            p_slot->key.bytes, local_arg->peer_key, local_arg->peer_key_length, shared_key, shared_key_len);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, free, ret, "psa_driver_wrapper_ffdh error!\n");
    }

    if (local_arg->output != NULL) { /* output is binary */
        ret = memcpy_sec(local_arg->output, local_arg->output_size, shared_key, shared_key_len);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, clean, ret, "memcpy_sec error!\n");

        local_arg->output_length = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits);
    } else { /* output is key id */
        ret = psa_key_usage_algorithm_type_permits(local_arg->attributes.policy.usage, local_arg->attributes.policy.alg,
            local_arg->attributes.type);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, clean, ret,
            "psa_key_usage_algorithm_type_permits error!\n");

        ret = tee_psa_import_key(&local_arg->attributes, shared_key, shared_key_len, &local_arg->key);
        psa_crypto_chk_goto_with_ret(ret, ret != PSA_INTERNAL_SUCCESS, clean, ret, "psa_import_key error!\n");
    }

clean:
    memset_s(shared_key, shared_key_len, 0, shared_key_len);
free:
    psa_crypto_free(shared_key);
    return ret;
}

int32_t dispatch_psa_sign_hash(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_sign_hash_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_SIGN_HASH);
    psa_crypto_null_param_chk(local_arg->hash, local_arg->signature);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_SIGN_HASH(local_arg->alg));
    psa_crypto_param_chk(PSA_HASH_LENGTH(PSA_ALG_GET_HASH(local_arg->alg)) != local_arg->hash_length);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->hash, local_arg->hash_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->signature, local_arg->signature_size) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_SIGN_HASH, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    if (PSA_ALG_IS_ECDSA(local_arg->alg)) {
        if (local_arg->signature_size < (size_t)(PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2)) {  // 2: r + s
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        ret = psa_driver_wrapper_ecc_sign(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data, p_slot->key.bytes,
            local_arg->hash, local_arg->hash_length, local_arg->signature, local_arg->signature_size);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ecc_sign error!\n");
        local_arg->signature_length = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2;  // 2: r + s
    } else {
        if (local_arg->signature_size < p_slot->attr.bits / PSA_BYTES_IN_BIT) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        ret = psa_driver_wrapper_rsa_sign(local_arg->alg, p_slot->attr.bits, p_slot->key.data, p_slot->key.bytes,
            local_arg->hash, local_arg->hash_length, local_arg->signature, local_arg->signature_size);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_rsa_sign error!\n");
        local_arg->signature_length = p_slot->attr.bits / PSA_BYTES_IN_BIT;
    }
    return ret;
}

int32_t dispatch_psa_verify_hash(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_verify_hash_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_VERIFY_HASH);
    psa_crypto_null_param_chk(local_arg->hash, local_arg->signature);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_SIGN_HASH(local_arg->alg));
    psa_crypto_param_chk(PSA_HASH_LENGTH(PSA_ALG_GET_HASH(local_arg->alg)) != local_arg->hash_length);

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->hash, local_arg->hash_length) != LOS_SUCCESS);
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->signature, local_arg->signature_length) != LOS_SUCCESS);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_VERIFY_HASH, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    if (PSA_ALG_IS_ECDSA(local_arg->alg)) {
        if (local_arg->signature_length < (size_t)(PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 2)) { // 2: r + s
            return PSA_ERROR_INVALID_SIGNATURE;
        }
        ret = psa_driver_wrapper_ecc_verify(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data, p_slot->key.bytes,
            local_arg->hash, local_arg->hash_length, local_arg->signature, local_arg->signature_length);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ecc_verify error!\n");
    } else {
        if (local_arg->signature_length < p_slot->attr.bits / PSA_BYTES_IN_BIT) {
            return PSA_ERROR_INVALID_SIGNATURE;
        }
        ret = psa_driver_wrapper_rsa_verify(local_arg->alg, p_slot->attr.type, p_slot->attr.bits, p_slot->key.data,
            p_slot->key.bytes, local_arg->hash, local_arg->hash_length, local_arg->signature,
            local_arg->signature_length);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_rsa_verify error!\n");
    }
    return ret;
}
#endif

#ifdef CONFIG_PSA_ASYMMETRIC_CRYPTO

#define RSA_PADLEN_11 11

static int32_t inner_check_psa_asymmetric_crypto_arg_param(psa_asymmetric_crypto_arg_t *local_arg)
{
    psa_crypto_null_param_chk(local_arg->input, local_arg->output);
    psa_crypto_param_alg_chk(!PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(local_arg->alg));
    if (local_arg->alg == PSA_ALG_RSA_PKCS1V15_CRYPT && local_arg->salt != NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (PSA_ALG_IS_RSA_OAEP(local_arg->alg) && local_arg->salt != NULL && local_arg->salt_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* buffer check */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->input, local_arg->input_length) != LOS_SUCCESS);
    if (local_arg->salt != NULL) {
        psa_crypto_param_chk(
            check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->salt, local_arg->salt_length) != LOS_SUCCESS);
    }
    psa_crypto_param_chk(
        check_is_not_in_tee_range((uintptr_t)local_arg->output, local_arg->output_size) != LOS_SUCCESS);

    return PSA_INTERNAL_SUCCESS;
}

static int32_t inner_ec_elgamal_crypto(psa_asymmetric_crypto_arg_t *local_arg, psa_key_slot_t *p_slot)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    if (local_arg->is_encrypt) {
        if (local_arg->output_size < 3 * (p_slot->attr.bits / PSA_BYTES_IN_BIT)) { // 3: triple of key_len
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        ret = psa_driver_wrapper_ec_elgamal_encrypt(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data,
            p_slot->key.bytes, local_arg->input, local_arg->input_length, local_arg->output, local_arg->output_size);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ec_elgamal_encrypt error!\n");

        local_arg->output_length = 3 * (p_slot->attr.bits / PSA_BYTES_IN_BIT); // 3: triple of key_len
        return ret;
    }

    /* 3: c1.x + c1.y + c2 */
    psa_crypto_chk_return(local_arg->input_length < (size_t)(PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(p_slot->attr.bits) * 3),
        PSA_ERROR_INVALID_ARGUMENT, "input_length invalid\n");

    if (local_arg->output_size < p_slot->attr.bits / PSA_BYTES_IN_BIT) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    ret = psa_driver_wrapper_ec_elgamal_decrypt(p_slot->attr.type, p_slot->attr.bits, p_slot->key.data,
        p_slot->key.bytes, local_arg->input, local_arg->input_length, local_arg->output, local_arg->output_size);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_ec_elgamal_decrypt error!\n");

    local_arg->output_length = p_slot->attr.bits / PSA_BYTES_IN_BIT;
    return ret;
}

int32_t dispatch_psa_asymmetric_crypto(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_asymmetric_crypto_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_ASYMMETRIC_CRYPTO);
    ret = inner_check_psa_asymmetric_crypto_arg_param(local_arg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_check_psa_asymmetric_crypto_arg_param error!\n");

    if (local_arg->is_encrypt) {
        ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_ENCRYPT, local_arg->alg);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

        if (local_arg->alg == PSA_ALG_EC_ELGAMAL &&
            p_slot->attr.type != PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SM2)) {
            return inner_ec_elgamal_crypto(local_arg, p_slot);
        }

        if (local_arg->output_size < p_slot->attr.bits / PSA_BYTES_IN_BIT) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        if (local_arg->alg == PSA_ALG_RSA_PKCS1V15_CRYPT || PSA_ALG_IS_RSA_OAEP(local_arg->alg)) {
            ret = psa_driver_wrapper_rsa_encrypt(local_arg->alg, p_slot->attr.type, p_slot->attr.bits,
                p_slot->key.data, p_slot->key.bytes, local_arg->input, local_arg->input_length, local_arg->salt,
                local_arg->salt_length, local_arg->output, local_arg->output_size);
            psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_rsa_encrypt error!\n");
        } else {
            ret = psa_driver_wrapper_sm2_encrypt(p_slot->key.data, p_slot->key.bytes, local_arg->input,
                local_arg->input_length, local_arg->output, local_arg->output_size);
            psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_sm2_encrypt error!\n");
        }

        local_arg->output_length = p_slot->attr.bits / PSA_BYTES_IN_BIT;
        return ret;
    }

    /* decrypt */
    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_DECRYPT, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    if (local_arg->alg == PSA_ALG_EC_ELGAMAL &&
        p_slot->attr.type != PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SM2)) {
        return inner_ec_elgamal_crypto(local_arg, p_slot);
    }

    if (local_arg->alg == PSA_ALG_RSA_PKCS1V15_CRYPT || PSA_ALG_IS_RSA_OAEP(local_arg->alg)) {
        ret = psa_driver_wrapper_rsa_decrypt(local_arg->alg, p_slot->attr.type, p_slot->attr.bits,
            p_slot->key.data, p_slot->key.bytes, local_arg->input, local_arg->input_length, local_arg->salt,
            local_arg->salt_length, local_arg->output, &local_arg->output_size);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_rsa_decrypt error!\n");
        local_arg->output_length = local_arg->output_size;
    } else {
        ret = psa_driver_wrapper_sm2_decrypt(p_slot->key.data, p_slot->key.bytes, local_arg->input,
            local_arg->input_length, local_arg->output, local_arg->output_size);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_sm2_decrypt error!\n");
        local_arg->output_length = p_slot->attr.bits / PSA_BYTES_IN_BIT;
    }
    return ret;
}
#endif

#ifdef CONFIG_PSA_KEY_DERIVATION

#define PSA_MAX_KEY_DERIVATION_SIZE 0x100

static int32_t inner_check_key_derivation_arg_hkdf_param(psa_key_derivation_arg_t *local_arg)
{
    /* check NULL parameter */
    if ((local_arg->hkdf.salt_length != 0 && local_arg->hkdf.salt == NULL) ||
        (local_arg->hkdf.info == NULL) || (!local_arg->hkdf.is_ikm_key_id && local_arg->hkdf.ikm == NULL)) {
        return PSA_ERROR_BAD_STATE;
    }

    /* check buffer */
    psa_crypto_param_chk(
        check_is_in_ree_alloc_shared_data((uintptr_t)local_arg->hkdf.info, local_arg->hkdf.info_length) != LOS_SUCCESS);
    if (local_arg->hkdf.salt != NULL) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->hkdf.salt, local_arg->hkdf.salt_length) != LOS_SUCCESS);
    }
    if (!local_arg->hkdf.is_ikm_key_id) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->hkdf.ikm, local_arg->hkdf.ikm_length) != LOS_SUCCESS);
    }
    return PSA_INTERNAL_SUCCESS;
}

static int32_t inner_check_key_derivation_arg_pbkdf2_param(psa_key_derivation_arg_t *local_arg)
{
    /* check NULL parameter */
    if ((local_arg->pbkdf2.salt_length != 0 && local_arg->pbkdf2.salt == NULL) ||
        (!local_arg->pbkdf2.is_password_key_id && local_arg->pbkdf2.password == NULL)) {
        return PSA_ERROR_BAD_STATE;
    }

    /* check buffer */
    if (local_arg->pbkdf2.salt != NULL) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->pbkdf2.salt, local_arg->pbkdf2.salt_length) != LOS_SUCCESS);
    }
    if (!local_arg->pbkdf2.is_password_key_id) {
        psa_crypto_param_chk(check_is_in_ree_alloc_shared_data(
            (uintptr_t)local_arg->pbkdf2.password, local_arg->pbkdf2.password_length) != LOS_SUCCESS);
    }
    return PSA_INTERNAL_SUCCESS;
}

static int32_t inner_check_key_derivation_arg_param(psa_key_derivation_arg_t *local_arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    psa_crypto_param_alg_chk(!PSA_ALG_IS_KEY_DERIVATION(local_arg->alg));
    psa_crypto_null_param_chk(local_arg->output);

    if (PSA_ALG_IS_HKDF(local_arg->alg)) {
        ret = inner_check_key_derivation_arg_hkdf_param(local_arg);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_check_key_derivation_arg_hkdf_param error!\n");
    }
    if (PSA_ALG_IS_PBKDF2_HMAC(local_arg->alg)) {
        ret = inner_check_key_derivation_arg_pbkdf2_param(local_arg);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_check_key_derivation_arg_pbkdf2_param error!\n");
    }

    /* chech output_length */
    if ((local_arg->output_length > PSA_MAX_KEY_DERIVATION_SIZE) ||
        (local_arg->capacity != 0 && local_arg->output_length > local_arg->capacity)) {
        return PSA_ERROR_INSUFFICIENT_DATA;
    }

    /* chech output buffer */
    psa_crypto_param_chk(check_is_not_in_tee_range((uintptr_t)local_arg->output, local_arg->output_length) ==
                         LOS_FAILURE);

    return PSA_INTERNAL_SUCCESS;
}

int32_t dispatch_psa_key_derivation_input_key(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_derivation_input_key_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_KEY_DERIVATION_INPUT_KEY);

    ret = psa_get_and_lock_key_slot_with_policy(local_arg->key, &p_slot, PSA_KEY_USAGE_DERIVE, local_arg->alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");

    return ret;
}

int32_t dispatch_psa_key_derivation(uint32_t cmd, void *arg)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    psa_key_derivation_arg_t *local_arg = arg;
    psa_key_slot_t *p_slot;
    uint8_t *hkdf_ikm = local_arg->hkdf.ikm;
    uint32_t hkdf_ikm_length = local_arg->hkdf.ikm_length;
    uint8_t *pbkdf2_password = local_arg->pbkdf2.password;
    uint32_t pbkdf2_password_length = local_arg->pbkdf2.password_length;

    /* param check */
    psa_crypto_param_chk(cmd != CMD_PSA_CRYPTO_KEY_DERIVATION);
    ret = inner_check_key_derivation_arg_param(local_arg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "inner_check_key_derivation_param error!\n");

    if (PSA_ALG_IS_PBKDF2_HMAC(local_arg->alg)) {
        if (local_arg->pbkdf2.is_password_key_id) {
            ret = psa_get_and_lock_key_slot_with_policy(local_arg->pbkdf2.password_key, &p_slot, PSA_KEY_USAGE_DERIVE,
                local_arg->alg);
            psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");
            pbkdf2_password = p_slot->key.data;
            pbkdf2_password_length = p_slot->key.bytes;
        }

        ret = psa_driver_wrapper_pbkdf2(local_arg->alg, pbkdf2_password, pbkdf2_password_length, local_arg->pbkdf2.salt,
            local_arg->pbkdf2.salt_length, local_arg->pbkdf2.count, local_arg->output, local_arg->output_length);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_pbkdf2 error!\n");
    } else {
        if (local_arg->hkdf.is_ikm_key_id) {
            ret = psa_get_and_lock_key_slot_with_policy(local_arg->hkdf.ikm_key, &p_slot, PSA_KEY_USAGE_DERIVE,
                local_arg->alg);
            psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_get_and_lock_key_slot_with_policy error!\n");
            hkdf_ikm = p_slot->key.data;
            hkdf_ikm_length = p_slot->key.bytes;
        }

        ret = psa_driver_wrapper_hkdf(local_arg->alg, local_arg->hkdf.salt, local_arg->hkdf.salt_length, hkdf_ikm,
            hkdf_ikm_length, local_arg->hkdf.info, local_arg->hkdf.info_length, local_arg->output,
            local_arg->output_length);
        psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_driver_wrapper_hkdf error!\n");
    }
    return ret;
}
#endif
