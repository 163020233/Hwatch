/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: PSA service ioctl header for SELiteOS.
 */

#ifndef __PSA_SERVICE_IOCTL_H__
#define __PSA_SERVICE_IOCTL_H__

#include "psa/psa_crypto.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    PSA_ATTESTATION_GEN_KEY_NR,
    PSA_ATTESTATION_GET_TOKEN_NR,
    PSA_PAKE_SETUP_NR,
    PSA_PAKE_ABORT_NR,
    PSA_PAKE_OUTPUT_NR,
    PSA_PAKE_INPUT_NR,
    PSA_PAKE_GET_SHARED_KEY_NR,
    PSA_SERVICE_IOC_MAX_NR,
} psa_service_ioc_nr_t;

typedef enum {
    PSA_STORAGE_WRITE_NR,
    PSA_STORAGE_READ_DATA_NR,
    PSA_STORAGE_READ_HEADER_NR,
    PSA_STORAGE_REMOVE_NR,
    PSA_CRYPTO_GET_KEY_ATTR_NR,
    PSA_CRYPTO_IMPORT_KEY_NR,
    PSA_CRYPTO_GENERATE_KEY_NR,
    PSA_CRYPTO_VERIFY_KEY_NR,
    PSA_CRYPTO_EXPORT_PUBLIC_KEY_NR,
    PSA_CRYPTO_DESTROY_KEY_NR,
    PSA_CRYPTO_GENERATE_RANDOM_NR,
    PSA_CRYPTO_CIPHER_SET_IV_NR,
    PSA_CRYPTO_CIPHER_CRYPTO_NR,
    PSA_CRYPTO_AEAD_ENCRYPT_NR,
    PSA_CRYPTO_AEAD_DECRYPT_NR,
    PSA_CRYPTO_HASH_COMPUTE_NR,
    PSA_CRYPTO_MAC_COMPUTE_NR,
    PSA_CRYPTO_KEY_AGREEMENT_NR,
    PSA_CRYPTO_SIGN_HASH_NR,
    PSA_CRYPTO_VERIFY_HASH_NR,
    PSA_CRYPTO_ASYMMETRIC_CRYPTO_NR,
    PSA_CRYPTO_KEY_DERIVATION_INPUT_KEY_NR,
    PSA_CRYPTO_KEY_DERIVATION_NR,
    PSA_SERVICE_ENHANCE_IOC_MAX_NR,
} psa_service_enhance_ioc_nr_t;

typedef struct psa_storage_write_arg {
    uint64_t sec_storage_id;
    uint32_t data_length;
    void *data;
    uint32_t flags;
} psa_storage_write_arg_t;

typedef struct psa_storage_read_arg {
    uint64_t sec_storage_id;
    uint32_t data_offset;
    uint32_t data_size;
    void *data;
    uint32_t data_length;
} psa_storage_read_arg_t;

typedef struct psa_attestation_gen_key_arg {
    uint32_t alg;
    uint8_t *public_key;
    uint32_t key_size;
    uint32_t *key_length;
} psa_attestation_gen_key_arg_t;

typedef struct psa_attestation_get_token_arg {
    const uint8_t *auth_challenge;
    uint32_t challenge_size;
    uint8_t *token_buf;
    uint32_t token_buf_size;
    uint32_t *token_size;
} psa_attestation_get_token_arg_t;

typedef struct {
    psa_key_id_t key;
    /* actually out param */
    psa_key_attributes_t attributes;
} psa_get_key_attr_arg_t;

typedef struct {
    psa_key_attributes_t attributes;
    const uint8_t *data;
    size_t data_length;
    /* actually out param */
    psa_key_id_t key;
} psa_import_key_arg_t;

typedef struct {
    psa_key_attributes_t attributes;
    /* actually out param */
    psa_key_id_t key;
} psa_generate_key_arg_t;

typedef struct {
    psa_key_id_t key;
    uint32_t crc32;
} psa_verify_key_arg_t;

typedef struct {
    psa_key_id_t key;
    uint8_t *data;
    size_t data_size;
    /* actually out param */
    size_t data_length;
} psa_export_public_key_arg_t;

typedef struct {
    psa_key_id_t key;
} psa_destroy_key_arg_t;

typedef struct {
    uint8_t *output;
    size_t output_size;
} psa_generate_random_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    const uint8_t *iv;
    size_t iv_length;
} psa_cipher_set_iv_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *input;
    size_t input_length;
    uint8_t *output;
    size_t output_size;
    /* actually out param */
    size_t output_length;
    bool is_encrypt;
} psa_cipher_crypto_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *nonce;
    size_t nonce_length;
    uint8_t *additional_data;
    size_t additional_data_length;
    uint8_t *plaintext;
    size_t plaintext_length;
    uint8_t *ciphertext;
    size_t ciphertext_size;
    /* actually out param */
    size_t ciphertext_length;
} psa_aead_encrypt_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *nonce;
    size_t nonce_length;
    uint8_t *additional_data;
    size_t additional_data_length;
    uint8_t *ciphertext;
    size_t ciphertext_length;
    uint8_t *plaintext;
    size_t plaintext_size;
    /* actually out param */
    size_t plaintext_length;
} psa_aead_decrypt_arg_t;

typedef struct {
    psa_algorithm_t alg;
    uint8_t *input;
    size_t input_length;
    uint8_t *hash;
    size_t hash_size;
    /* actually out param */
    size_t hash_length;
} psa_hash_compute_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *input;
    size_t input_length;
    uint8_t *mac;
    size_t mac_size;
    /* actually out param */
    size_t mac_length;
} psa_mac_compute_arg_t;

typedef struct {
    psa_key_id_t private_key;
    uint8_t *peer_key;
    size_t peer_key_length;
    psa_algorithm_t alg;
    psa_key_attributes_t attributes;
    /* actually out param */
    psa_key_id_t key;
    uint8_t *output;
    size_t output_size;
    /* actually out param */
    size_t output_length;
} psa_key_agreement_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *hash;
    size_t hash_length;
    uint8_t *signature;
    size_t signature_size;
    /* actually out param */
    size_t signature_length;
} psa_sign_hash_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *hash;
    size_t hash_length;
    uint8_t *signature;
    size_t signature_length;
} psa_verify_hash_arg_t;

typedef struct {
    bool is_encrypt;
    psa_key_id_t key;
    psa_algorithm_t alg;
    uint8_t *input;
    size_t input_length;
    uint8_t *salt;
    size_t salt_length;
    uint8_t *output;
    size_t output_size;
    /* actually out param */
    size_t output_length;
} psa_asymmetric_crypto_arg_t;

typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
} psa_key_derivation_input_key_arg_t;

typedef struct {
    psa_algorithm_t alg;
    size_t capacity;
    psa_hkdf_key_derivation_t hkdf;
    psa_pbkdf2_key_derivation_t pbkdf2;
    uint8_t *output;
    uint32_t output_length;
} psa_key_derivation_arg_t;

typedef struct {
    psa_key_id_t password_key;
    psa_pake_cipher_suite_t cipher_suite;
    uint8_t op_id;
} psa_pake_setup_arg_t;

typedef struct {
    psa_pake_operation_t operation;
} psa_pake_abort_arg_t;

typedef struct {
    uint32_t output_size;
    psa_pake_operation_t operation;
    psa_pake_step_t step;
    uint8_t *output;
} psa_pake_output_arg_t;

typedef struct {
    psa_pake_operation_t operation;
    psa_pake_step_t step;
    const uint8_t *input;
    uint32_t input_size;
} psa_pake_input_arg_t;

typedef struct {
    psa_pake_operation_t operation;
    psa_key_attributes_t attributes;
    psa_key_id_t key;
} psa_pake_get_shared_key_arg_t;

// psa service ioctl entry
#define IOC_TYPE_PSA_SERVICE 'p'
#define IOC_TYPE_PSA_SERVICE_ENHANCE 'e'

#define PSA_SERVICE_IOC_NA 0U
#define PSA_SERVICE_IOC_W  1U
#define PSA_SERVICE_IOC_R  2U
#define PSA_SERVICE_IOC_RW 3U

#define PSA_SERVICE_IOC(dir, type, nr, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | ((nr) << 0))

#define PSA_SERVICE_IO(nr) PSA_SERVICE_IOC(PSA_SERVICE_IOC_NA, IOC_TYPE_PSA_SERVICE, (nr), 0)
#define PSA_SERVICE_IOWR(nr, struct_name) \
    PSA_SERVICE_IOC(PSA_SERVICE_IOC_RW, IOC_TYPE_PSA_SERVICE, (nr), sizeof(struct_name))
#define PSA_SERVICE_IOW(nr, struct_name) \
    PSA_SERVICE_IOC(PSA_SERVICE_IOC_W, IOC_TYPE_PSA_SERVICE, (nr), sizeof(struct_name))

#define PSA_SERVICE_ENHANCE_IO(nr) PSA_SERVICE_IOC(PSA_SERVICE_IOC_NA, IOC_TYPE_PSA_SERVICE_ENHANCE, (nr), 0)
#define PSA_SERVICE_ENHANCE_IOWR(nr, struct_name) \
PSA_SERVICE_IOC(PSA_SERVICE_IOC_RW, IOC_TYPE_PSA_SERVICE_ENHANCE, (nr), sizeof(struct_name))
#define PSA_SERVICE_ENHANCE_IOW(nr, struct_name) \
PSA_SERVICE_IOC(PSA_SERVICE_IOC_W, IOC_TYPE_PSA_SERVICE_ENHANCE, (nr), sizeof(struct_name))

/* normal */
#define CMD_PSA_ATTESTATION_GEN_KEY   PSA_SERVICE_IOWR(PSA_ATTESTATION_GEN_KEY_NR, psa_attestation_gen_key_arg_t)
#define CMD_PSA_ATTESTATION_GET_TOKEN PSA_SERVICE_IOWR(PSA_ATTESTATION_GET_TOKEN_NR, psa_attestation_get_token_arg_t)

#define CMD_PSA_PAKE_SETUP          PSA_SERVICE_IOWR(PSA_PAKE_SETUP_NR, psa_pake_setup_arg_t)
#define CMD_PSA_PAKE_ABORT          PSA_SERVICE_IOW(PSA_PAKE_ABORT_NR, psa_pake_abort_arg_t)
#define CMD_PSA_PAKE_OUTPUT         PSA_SERVICE_IOWR(PSA_PAKE_OUTPUT_NR, psa_pake_output_arg_t)
#define CMD_PSA_PAKE_INPUT          PSA_SERVICE_IOW(PSA_PAKE_INPUT_NR, psa_pake_input_arg_t)
#define CMD_PSA_PAKE_GET_SHARED_KEY PSA_SERVICE_IOWR(PSA_PAKE_GET_SHARED_KEY_NR, psa_pake_get_shared_key_arg_t)

/* enhance */
#define CMD_PSA_STORAGE_WRITE       PSA_SERVICE_ENHANCE_IOW(PSA_STORAGE_WRITE_NR, psa_storage_write_arg_t)
#define CMD_PSA_STORAGE_READ_DATA   PSA_SERVICE_ENHANCE_IOWR(PSA_STORAGE_READ_DATA_NR, psa_storage_read_arg_t)
#define CMD_PSA_STORAGE_READ_HEADER PSA_SERVICE_ENHANCE_IOWR(PSA_STORAGE_READ_HEADER_NR, psa_storage_read_arg_t)
#define CMD_PSA_STORAGE_REMOVE      PSA_SERVICE_ENHANCE_IOW(PSA_STORAGE_REMOVE_NR, uint64_t)

#define CMD_PSA_CRYPTO_GET_KEY_ATTR      PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_GET_KEY_ATTR_NR, psa_get_key_attr_arg_t)
#define CMD_PSA_CRYPTO_IMPORT_KEY        PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_IMPORT_KEY_NR, psa_import_key_arg_t)
#define CMD_PSA_CRYPTO_GENERATE_KEY      PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_GENERATE_KEY_NR, psa_generate_key_arg_t)
#define CMD_PSA_CRYPTO_VERIFY_KEY        PSA_SERVICE_ENHANCE_IOW(PSA_CRYPTO_VERIFY_KEY_NR, psa_verify_key_arg_t)
#define CMD_PSA_CRYPTO_EXPORT_PUBLIC_KEY PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_EXPORT_PUBLIC_KEY_NR, psa_export_public_key_arg_t)
#define CMD_PSA_CRYPTO_DESTROY_KEY       PSA_SERVICE_ENHANCE_IOW(PSA_CRYPTO_DESTROY_KEY_NR, psa_destroy_key_arg_t)
#define CMD_PSA_CRYPTO_GENERATE_RANDOM   PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_GENERATE_RANDOM_NR, psa_generate_random_arg_t)
#define CMD_PSA_CRYPTO_CIPHER_SET_IV     PSA_SERVICE_ENHANCE_IOW(PSA_CRYPTO_CIPHER_SET_IV_NR, psa_cipher_set_iv_arg_t)
#define CMD_PSA_CRYPTO_CIPHER_CRYPTO     PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_CIPHER_CRYPTO_NR, psa_cipher_crypto_arg_t)
#define CMD_PSA_CRYPTO_AEAD_ENCRYPT      PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_AEAD_ENCRYPT_NR, psa_aead_encrypt_arg_t)
#define CMD_PSA_CRYPTO_AEAD_DECRYPT      PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_AEAD_DECRYPT_NR, psa_aead_decrypt_arg_t)
#define CMD_PSA_CRYPTO_HASH_COMPUTE      PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_HASH_COMPUTE_NR, psa_hash_compute_arg_t)
#define CMD_PSA_CRYPTO_MAC_COMPUTE       PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_MAC_COMPUTE_NR, psa_mac_compute_arg_t)
#define CMD_PSA_CRYPTO_KEY_AGREEMENT     PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_KEY_AGREEMENT_NR, psa_key_agreement_arg_t)
#define CMD_PSA_CRYPTO_SIGN_HASH         PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_SIGN_HASH_NR, psa_sign_hash_arg_t)
#define CMD_PSA_CRYPTO_VERIFY_HASH       PSA_SERVICE_ENHANCE_IOW(PSA_CRYPTO_VERIFY_HASH_NR, psa_verify_hash_arg_t)
#define CMD_PSA_CRYPTO_ASYMMETRIC_CRYPTO PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_ASYMMETRIC_CRYPTO_NR, psa_asymmetric_crypto_arg_t)
#define CMD_PSA_CRYPTO_KEY_DERIVATION    PSA_SERVICE_ENHANCE_IOWR(PSA_CRYPTO_KEY_DERIVATION_NR, psa_key_derivation_arg_t)
#define CMD_PSA_CRYPTO_KEY_DERIVATION_INPUT_KEY \
    PSA_SERVICE_ENHANCE_IOW(PSA_CRYPTO_KEY_DERIVATION_INPUT_KEY_NR, psa_key_derivation_input_key_arg_t)

typedef int32_t (*psa_service_dispatch_func)(uint32_t cmd, void *arg);

typedef struct {
    uint32_t cmd;
    psa_service_dispatch_func func;
} psa_service_ioctl_cmd_t;

const psa_service_ioctl_cmd_t *get_psa_service_dispatch_func_list(void);
uint32_t get_psa_service_max_nr(void);

const psa_service_ioctl_cmd_t *get_psa_service_enhance_dispatch_func_list(void);
uint32_t get_psa_service_enhance_max_nr(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PSA_SERVICE_IOCTL_H__ */
