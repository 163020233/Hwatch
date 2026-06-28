/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: crypto api header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef DISPATCH_PSA_CRYPTO_H
#define DISPATCH_PSA_CRYPTO_H

#include "errcode.h"
#include "psa_service_ioctl.h"
#include "psa_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int32_t dispatch_psa_unsupport(uint32_t cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    return ERRCODE_NOT_SUPPORT;
}

#ifdef CONFIG_PSA_CRYPTO_SUPPORT
int32_t dispatch_psa_get_key_attr(uint32_t cmd, void *arg);
int32_t dispatch_psa_import_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_generate_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_destroy_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_verify_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_export_public_key(uint32_t cmd, void *arg);

int32_t dispatch_psa_generate_random(uint32_t cmd, void *arg);
int32_t dispatch_psa_cipher_set_iv(uint32_t cmd, void *arg);
int32_t dispatch_psa_cipher_crypto(uint32_t cmd, void *arg);
int32_t dispatch_psa_aead_encrypt(uint32_t cmd, void *arg);
int32_t dispatch_psa_aead_decrypt(uint32_t cmd, void *arg);
int32_t dispatch_psa_hash_compute(uint32_t cmd, void *arg);
int32_t dispatch_psa_mac_compute(uint32_t cmd, void *arg);
int32_t dispatch_psa_key_agreement(uint32_t cmd, void *arg);
int32_t dispatch_psa_sign_hash(uint32_t cmd, void *arg);
int32_t dispatch_psa_verify_hash(uint32_t cmd, void *arg);

#ifdef CONFIG_PSA_ASYMMETRIC_CRYPTO
int32_t dispatch_psa_asymmetric_crypto(uint32_t cmd, void *arg);
#else
#define dispatch_psa_asymmetric_crypto          dispatch_psa_unsupport
#endif

#ifdef CONFIG_PSA_KEY_DERIVATION
int32_t dispatch_psa_key_derivation_input_key(uint32_t cmd, void *arg);
int32_t dispatch_psa_key_derivation(uint32_t cmd, void *arg);
#else
#define dispatch_psa_key_derivation_input_key   dispatch_psa_unsupport
#define dispatch_psa_key_derivation             dispatch_psa_unsupport
#endif

#else
#define dispatch_psa_get_key_attr               dispatch_psa_unsupport
#define dispatch_psa_import_key                 dispatch_psa_unsupport
#define dispatch_psa_generate_key               dispatch_psa_unsupport
#define dispatch_psa_destroy_key                dispatch_psa_unsupport
#define dispatch_psa_verify_key                 dispatch_psa_unsupport
#define dispatch_psa_export_public_key          dispatch_psa_unsupport
#define dispatch_psa_generate_random            dispatch_psa_unsupport
#define dispatch_psa_cipher_set_iv              dispatch_psa_unsupport
#define dispatch_psa_cipher_crypto              dispatch_psa_unsupport
#define dispatch_psa_aead_encrypt               dispatch_psa_unsupport
#define dispatch_psa_aead_decrypt               dispatch_psa_unsupport
#define dispatch_psa_hash_compute               dispatch_psa_unsupport
#define dispatch_psa_mac_compute                dispatch_psa_unsupport
#define dispatch_psa_key_agreement              dispatch_psa_unsupport
#define dispatch_psa_sign_hash                  dispatch_psa_unsupport
#define dispatch_psa_verify_hash                dispatch_psa_unsupport
#define dispatch_psa_key_derivation_input_key   dispatch_psa_unsupport
#define dispatch_psa_key_derivation             dispatch_psa_unsupport
#define dispatch_psa_asymmetric_crypto          dispatch_psa_unsupport
#endif /* CONFIG_PSA_CRYPTO_SUPPORT */

#if defined(CONFIG_PSA_CRYPTO_PAKE_SUPPORT)
int32_t dispatch_psa_pake_setup(uint32_t cmd, void *arg);

int32_t dispatch_psa_pake_abort(uint32_t cmd, void *arg);

int32_t dispatch_psa_pake_output(uint32_t cmd, void *arg);

int32_t dispatch_psa_pake_input(uint32_t cmd, void *arg);

int32_t dispatch_psa_pake_get_shared_key(uint32_t cmd, void *arg);
#else
#define dispatch_psa_pake_setup                 dispatch_psa_unsupport
#define dispatch_psa_pake_abort                 dispatch_psa_unsupport
#define dispatch_psa_pake_output                dispatch_psa_unsupport
#define dispatch_psa_pake_input                 dispatch_psa_unsupport
#define dispatch_psa_pake_get_shared_key        dispatch_psa_unsupport
#endif  /* CONFIG_PSA_CRYPTO_PAKE_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* DISPATCH_PSA_CRYPTO_H */
