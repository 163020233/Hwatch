/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef PSA_CRYPTO_WRAP_H
#define PSA_CRYPTO_WRAP_H

#include "psa/psa_crypto.h"
#include "dispatch_psa_crypto_inner.h"
#include "unified_cipher_pke.h"

#ifdef __cplusplus
extern "C" {
#endif

/* common */
psa_status_t psa_get_ecc_private_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data,
    uint32_t data_length, uapi_drv_cipher_pke_data_t *private_key);
psa_status_t psa_get_ecc_public_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_ecc_point_t *public_key);
psa_status_t psa_get_ecc_sign(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data,
    uint32_t data_length, uapi_drv_cipher_pke_ecc_sig_t *sign);
psa_status_t psa_get_rsa_private_key(psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_rsa_priv_key_t *private_key);
psa_status_t psa_get_rsa_public_key(psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_rsa_pub_key_t *public_key);
void psa_get_ecc_c1_and_c2(psa_key_bits_t bits, uint8_t *data, uapi_drv_cipher_pke_ecc_point_t *c1,
    uapi_drv_cipher_pke_data_t *c2);

/* secure storage */
psa_status_t psa_save_persistent_key(const psa_key_attributes_t *attr, const uint8_t *data, size_t data_length);
psa_status_t psa_recovery_persistent_key(psa_key_id_t key, psa_key_slot_t **pp_slot);
psa_status_t psa_destroy_persistent_key(psa_key_id_t key);

/* symc */
psa_status_t psa_driver_wrapper_cipher_crypto(psa_key_type_t type, psa_algorithm_t alg, uint8_t *key_data,
    uint32_t key_len, const uint8_t *input, uint8_t *output, uint32_t data_len, uint8_t iv[16], bool is_encrypt);
psa_status_t psa_driver_wrapper_aead_crypto(psa_key_type_t type, psa_algorithm_t alg, uint8_t *key_data,
    uint32_t key_len, const uint8_t *nonce, uint32_t nonce_length, const uint8_t *additional_data,
    uint32_t additional_data_length, const uint8_t *input, uint8_t *output, uint32_t data_length,
    uint8_t *tag, uint32_t tag_length, bool is_encrypt);
psa_status_t psa_driver_wrapper_cmac(uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t data_len,
    uint8_t mac[16]);
psa_status_t psa_driver_wrapper_cbc_mac(uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t data_len,
    uint8_t mac[16]);


/* hash */
psa_status_t psa_driver_wrapper_cipher_hash_compute(psa_algorithm_t alg, const uint8_t *input, uint32_t input_length,
    uint8_t *hash);
psa_status_t psa_driver_wrapper_hmac(psa_algorithm_t alg, uint8_t *key_data, uint32_t key_len,
    const uint8_t *input, uint32_t data_len, uint8_t mac[32]);

/* trng */
psa_status_t psa_driver_wrapper_generate_trng(uint8_t *data, uint32_t length);

/* crc */
psa_status_t psa_driver_wrapper_driver_crc32(const uint8_t *data, uint32_t length, uint32_t *crc32);

/* ffdh */
psa_status_t psa_driver_wrapper_ffdh(psa_key_bits_t bits, const uint8_t *private_key,
    uint32_t private_key_length, const uint8_t *public_key, uint32_t public_key_length, uint8_t *shared_key,
    uint32_t shared_key_length);

/* ecc */
psa_status_t psa_driver_wrapper_ecdh(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *private_key,
    uint32_t private_key_length, const uint8_t *public_key, uint32_t public_key_length, uint8_t *shared_key,
    uint32_t shared_key_length);
psa_status_t psa_driver_wrapper_ecc_gen_priv_key(psa_key_type_t type, psa_key_bits_t bits, uint8_t *key_data,
    uint32_t key_len);
psa_status_t psa_driver_wrapper_ecc_gen_pub_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *priv_key_data,
    uint32_t priv_key_len, uint8_t *pub_key_data, uint32_t pub_key_len);
psa_status_t psa_driver_wrapper_ecc_check_on_curve(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len);
psa_status_t psa_driver_wrapper_ecc_sign(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size);
psa_status_t psa_driver_wrapper_ecc_verify(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size);
psa_status_t psa_driver_wrapper_sm2_encrypt(const uint8_t *key_data, uint32_t key_len, const uint8_t *input,
    uint32_t input_length, uint8_t *output, uint32_t output_length);
psa_status_t psa_driver_wrapper_sm2_decrypt(const uint8_t *key_data, uint32_t key_len, const uint8_t *input,
    uint32_t input_length, uint8_t *output, uint32_t output_length);
psa_status_t psa_driver_wrapper_ec_elgamal_encrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *input, uint32_t input_length, uint8_t *output, uint32_t output_length);
psa_status_t psa_driver_wrapper_ec_elgamal_decrypt(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *input, uint32_t input_length, uint8_t *output, uint32_t output_length);

/* rsa */
psa_status_t psa_driver_wrapper_rsa_sign(psa_algorithm_t alg, psa_key_bits_t bits, const uint8_t *key_data,
    uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature, uint32_t signature_size);
psa_status_t psa_driver_wrapper_rsa_verify(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *hash, uint32_t hash_length, const uint8_t *signature,
    uint32_t signature_size);
psa_status_t psa_driver_wrapper_rsa_encrypt(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t input_length, const uint8_t *salt,
    uint32_t salt_length, uint8_t *output, uint32_t output_length);
psa_status_t psa_driver_wrapper_rsa_decrypt(psa_algorithm_t alg, psa_key_type_t type, psa_key_bits_t bits,
    const uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t input_length, const uint8_t *salt,
    uint32_t salt_length, uint8_t *output, uint32_t *output_length);

/* kdf */
psa_status_t psa_driver_wrapper_pbkdf2(psa_algorithm_t alg, const uint8_t *password, uint32_t password_length,
    const uint8_t *salt, uint32_t salt_length, uint16_t count, uint8_t *output, uint32_t output_length);
psa_status_t psa_driver_wrapper_hkdf(psa_algorithm_t alg, const uint8_t *salt, uint32_t salt_length,
    const uint8_t *ikm, uint32_t ikm_length, const uint8_t *info, uint32_t info_length, uint8_t *output,
    uint32_t output_length);

#ifdef __cplusplus
}
#endif

#endif
