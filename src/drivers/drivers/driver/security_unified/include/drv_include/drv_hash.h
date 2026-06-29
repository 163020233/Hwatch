/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver hash header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_HASH_H
#define DRV_HASH_H

#include "crypto_type.h"
#include "crypto_hash_struct.h"
#include "crypto_kdf_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef CONFIG_DRV_HASH_BUF_LEN
#define CONFIG_DRV_HASH_BUF_LEN        128
#endif

typedef struct {
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    uintptr_t check_word;
#endif
    crypto_hash_type hash_type;
    unsigned char buf[CONFIG_DRV_HASH_BUF_LEN + CRYPTO_HASH_BLOCK_SIZE_MAX];
    unsigned int tail_len;
    unsigned int state[CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD];
    unsigned int state_size;
    unsigned int length[2];
} drv_hash_ctx;

typedef struct {
    crypto_hash_type hash_type;
    void *hash_ctx;
    unsigned char o_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX];
    unsigned int block_size;
} drv_hmac_ctx;

int32_t drv_cipher_hash_compute_clone(void *dst_ctx, const void *src_ctx);

int32_t drv_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, uint8_t *out, const uint32_t out_len);

int32_t drv_cipher_pbkdf2_ex(const crypto_kdf_pbkdf2_ex_param *param, uint8_t *out, const uint32_t out_len);

int drv_cipher_hash_compute(crypto_hash_type hash_type, const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len);

int drv_cipher_hash_compute_setup(void **ctx);

int drv_cipher_hash_compute_start(void *ctx, crypto_hash_type hash_type);

int drv_cipher_hash_compute_update(void *ctx, const unsigned char *data, unsigned int data_len);

int drv_cipher_hash_compute_finish(void *ctx, unsigned char *out_hash, unsigned int out_hash_len);

int drv_cipher_hash_compute_teardown(void *ctx);

int drv_cipher_hmac_compute(crypto_hash_type hash_type,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle,
    const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len);

int drv_cipher_hmac_compute_setup(void **ctx);

int drv_cipher_hmac_compute_start(void *ctx, crypto_hash_type hash_type,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle);

int drv_cipher_hmac_compute_update(void *ctx, const unsigned char *data, unsigned int data_len);

int drv_cipher_hmac_compute_finish(void *ctx, unsigned char *out_hash, unsigned int out_hash_len);

int drv_cipher_hmac_compute_teardown(void *ctx);

int drv_cipher_hkdf_extract(crypto_hkdf_extract_t *extract_param, unsigned char *prk, unsigned int *prk_length);

int drv_cipher_hkdf_expand(const crypto_hkdf_expand_t *expand_param, unsigned char *okm, unsigned int okm_length);

int drv_cipher_hkdf(crypto_hkdf_t *hkdf_param, unsigned char *okm, unsigned int okm_length);

typedef struct {
    crypto_hash_type hash_type;
    uint32_t password[32];
    uint32_t data[32];
    uint32_t result[16];
    uint32_t data_len;
    uint32_t keyslot_handle;
    uint32_t block_size;
    uint32_t result_size;
    bool is_salt_overflow;
    bool is_local_keyslot;
} drv_pbkdf2_ctx_t;

int32_t drv_cipher_pbkdf2_setup(void **ctx, crypto_hash_type hash_type,
    const uint8_t *password, uint32_t plen, uint32_t keyslot_handle);

int32_t drv_cipher_pbkdf2_set_salt(void *ctx, const uint8_t *salt, uint32_t slen, uint32_t count);

int32_t drv_cipher_pbkdf2_update_soft(void *ctx, uint32_t iteration_count);

int32_t drv_cipher_pbkdf2_update_hardware(void *ctx, uint16_t iteration_count);

int32_t drv_cipher_pbkdf2_finish(void *ctx, uint8_t *out_key, uint32_t out_key_len);

int32_t drv_cipher_pbkdf2_teardown(void *ctx);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif