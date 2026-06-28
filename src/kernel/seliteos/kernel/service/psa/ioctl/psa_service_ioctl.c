/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: PSA service ioctl implement for SELiteOS.
 */

#include "psa_service_ioctl.h"
#include "dispatch_psa_attestation.h"
#include "dispatch_psa_crypto.h"
#include "dispatch_psa_storage.h"

/* normal */
static const psa_service_ioctl_cmd_t g_psa_service_dispatch_func[] = {
    {CMD_PSA_ATTESTATION_GEN_KEY, dispatch_psa_attest_gen_key},
    {CMD_PSA_ATTESTATION_GET_TOKEN, dispatch_psa_attest_get_token},
    {CMD_PSA_PAKE_SETUP, dispatch_psa_pake_setup},
    {CMD_PSA_PAKE_ABORT, dispatch_psa_pake_abort},
    {CMD_PSA_PAKE_OUTPUT, dispatch_psa_pake_output},
    {CMD_PSA_PAKE_INPUT, dispatch_psa_pake_input},
    {CMD_PSA_PAKE_GET_SHARED_KEY, dispatch_psa_pake_get_shared_key},
};

/* enhance */
static const psa_service_ioctl_cmd_t g_psa_service_enhance_dispatch_func[] = {
    {CMD_PSA_STORAGE_WRITE, dispatch_psa_storage_write},
    {CMD_PSA_STORAGE_READ_DATA, dispatch_psa_storage_read},
    {CMD_PSA_STORAGE_READ_HEADER, dispatch_psa_storage_read},
    {CMD_PSA_STORAGE_REMOVE, dispatch_psa_storage_remove},
    {CMD_PSA_CRYPTO_GET_KEY_ATTR, dispatch_psa_get_key_attr},
    {CMD_PSA_CRYPTO_IMPORT_KEY, dispatch_psa_import_key},
    {CMD_PSA_CRYPTO_GENERATE_KEY, dispatch_psa_generate_key},
    {CMD_PSA_CRYPTO_VERIFY_KEY, dispatch_psa_verify_key},
    {CMD_PSA_CRYPTO_EXPORT_PUBLIC_KEY, dispatch_psa_export_public_key},
    {CMD_PSA_CRYPTO_DESTROY_KEY, dispatch_psa_destroy_key},
    {CMD_PSA_CRYPTO_GENERATE_RANDOM, dispatch_psa_generate_random},
    {CMD_PSA_CRYPTO_CIPHER_SET_IV, dispatch_psa_cipher_set_iv},
    {CMD_PSA_CRYPTO_CIPHER_CRYPTO, dispatch_psa_cipher_crypto},
    {CMD_PSA_CRYPTO_AEAD_ENCRYPT, dispatch_psa_aead_encrypt},
    {CMD_PSA_CRYPTO_AEAD_DECRYPT, dispatch_psa_aead_decrypt},
    {CMD_PSA_CRYPTO_HASH_COMPUTE, dispatch_psa_hash_compute},
    {CMD_PSA_CRYPTO_MAC_COMPUTE, dispatch_psa_mac_compute},
    {CMD_PSA_CRYPTO_KEY_AGREEMENT, dispatch_psa_key_agreement},
    {CMD_PSA_CRYPTO_SIGN_HASH, dispatch_psa_sign_hash},
    {CMD_PSA_CRYPTO_VERIFY_HASH, dispatch_psa_verify_hash},
    {CMD_PSA_CRYPTO_ASYMMETRIC_CRYPTO, dispatch_psa_asymmetric_crypto},
    {CMD_PSA_CRYPTO_KEY_DERIVATION_INPUT_KEY, dispatch_psa_key_derivation_input_key},
    {CMD_PSA_CRYPTO_KEY_DERIVATION, dispatch_psa_key_derivation},
};

const psa_service_ioctl_cmd_t *get_psa_service_dispatch_func_list(void)
{
    return g_psa_service_dispatch_func;
}

uint32_t get_psa_service_max_nr(void)
{
    return PSA_SERVICE_IOC_MAX_NR;
}

const psa_service_ioctl_cmd_t *get_psa_service_enhance_dispatch_func_list(void)
{
    return g_psa_service_enhance_dispatch_func;
}

uint32_t get_psa_service_enhance_max_nr(void)
{
    return PSA_SERVICE_ENHANCE_IOC_MAX_NR;
}
