/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver symc header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_SYMC_H
#define DRV_SYMC_H

#include "crypto_type.h"
#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DRV_SYMC_MAC_BUF_LEN        256

typedef struct {
    unsigned char buf[DRV_SYMC_MAC_BUF_LEN];
    unsigned int tail_len;
    unsigned char mac[16];
    unsigned char key[32];
    unsigned int key_len;
    unsigned int keyslot_handle;
    crypto_symc_work_mode mode;
    crypto_symc_alg symc_alg;
} drv_mac_ctx_t;

typedef struct {
    uint8_t iv[CRYPTO_IV_LEN_IN_BYTES];
    uint32_t state_len;   /* the middle result byte len. */
    uint32_t result_xor_value;
    crypto_symc_work_mode mode; /* store crc work mode. */
    bool is_output_reverse;
} drv_crc_ctx_t;

int drv_symc_ghash_start(void **ctx, crypto_symc_alg symc_alg,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle);

int drv_symc_ghash_update(void *ctx, const unsigned char *data, unsigned int data_len);

int drv_symc_ghash_finish(void *ctx, unsigned char ghash[16]);

int drv_symc_ghash_destroy(void *ctx);

int32_t drv_cipher_symc_gcm_encrypt(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_gcm_decrypt_verify(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_ccm_encrypt(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_ccm_decrypt_verify(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_gcm_setup(void **ctx, crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt);

int32_t drv_cipher_symc_gcm_update_ad(void *ctx, const uint8_t *aad, uint32_t aad_len);

int32_t drv_cipher_symc_gcm_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);

int32_t drv_cipher_symc_gcm_finish(void *ctx, uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_gcm_teardown(void *ctx);

int32_t drv_cipher_symc_ccm_setup(void **ctx, crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt);

int32_t drv_cipher_symc_ccm_set_lengths(void *ctx, uint32_t total_aad_len,
    uint32_t data_len, uint32_t tag_len);

int32_t drv_cipher_symc_ccm_update_ad(void *ctx, const uint8_t *aad, uint32_t aad_len);

int32_t drv_cipher_symc_ccm_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);

int32_t drv_cipher_symc_ccm_finish(void *ctx, uint8_t *tag, uint32_t tag_len);

int32_t drv_cipher_symc_ccm_teardown(void *ctx);

int drv_symc_mac_ctx_init(void *ctx, uint32_t ctx_size, crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

int drv_symc_mac_start(void **ctx, crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle);

int drv_symc_mac_update(void *ctx, const unsigned char *data, unsigned int data_len);

int drv_symc_mac_finish(void *ctx, unsigned char mac[16]);

int drv_symc_mac_destroy(void *ctx);

int drv_symc_mac_compute(crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle,
    const unsigned char *data, unsigned int data_len, unsigned char mac[16]);

int drv_symc_crc_get_initial_value(drv_crc_ctx_t *crc_ctx);

int drv_symc_crc_post_process(const drv_crc_ctx_t *crc_ctx, unsigned char *out_data, unsigned int out_len);

int drv_symc_crc_start(void **ctx, crypto_symc_work_mode mode);

int drv_symc_crc_update(void *ctx, const unsigned char *data, unsigned int data_len);

int drv_symc_crc_finish(void *ctx, unsigned char *out_crc, unsigned int out_crc_len);

int drv_symc_crc_destroy(void *ctx);

int drv_symc_crc_compute(crypto_symc_work_mode mode, const unsigned char *data, unsigned int data_len,
    unsigned char *out_crc, unsigned int out_crc_len);

int drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt);

int32_t drv_cipher_symc_setup(void **ctx, uint32_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt);

int32_t drv_cipher_symc_ecb_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len);

int32_t drv_cipher_symc_cbc_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len, uint8_t iv[16]);

int32_t drv_cipher_symc_ctr_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t nonce_counter[16], uint8_t *nc_off, uint8_t stream_block[16]);

int32_t drv_cipher_symc_ofb_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_cfb1_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_cfb8_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_cfb128_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_cfb8_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_cfb1_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off);

int32_t drv_cipher_symc_teardown(void *ctx);

int32_t drv_cipher_symc_dma_copy(uint64_t src_addr, uint64_t dst_addr, uint32_t len);

int32_t drv_cipher_symc_set_key(void *ctx, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

int32_t drv_cipher_symc_set_alg(void *ctx, uint32_t alg);

int32_t drv_cipher_symc_set_crypt_mode(void *ctx, bool is_encrypt);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
