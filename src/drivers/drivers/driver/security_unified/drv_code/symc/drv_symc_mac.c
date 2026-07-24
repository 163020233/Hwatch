/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drv symc mac simple implement.
 *
 * Create: 2024-07-30
*/
#include "drv_symc.h"
#include "hal_symc.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_DRV_CODE_SYMC_DRV_SYMC_MAC

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    drv_err_chk_return((ptr) == NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

CRYPTO_STATIC int32_t inner_drv_mac_ctx_chk(const drv_mac_ctx_t *ctx)
{
    drv_err_chk_return(ctx->tail_len > DRV_SYMC_MAC_BUF_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "tail_len[%u] invalid\n", ctx->tail_len);
    drv_err_chk_return(ctx->key_len != CRYPTO_128_KEY_LEN && ctx->key_len != CRYPTO_192_KEY_LEN &&
        ctx->key_len != CRYPTO_256_KEY_LEN, SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");
    drv_err_chk_return(ctx->mode != CRYPTO_SYMC_WORK_MODE_CMAC && ctx->mode != CRYPTO_SYMC_WORK_MODE_CBC_MAC,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "mode[0x%x] invalid\n", ctx->mode);
    drv_err_chk_return(ctx->symc_alg != CRYPTO_SYMC_ALG_AES && ctx->symc_alg != CRYPTO_SYMC_ALG_SM4,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "symc_alg invalid\n");

#if !defined(CONFIG_SYMC_SUPPORT_SM4_CMAC)
    drv_err_chk_return(ctx->symc_alg == CRYPTO_SYMC_ALG_SM4 && ctx->mode == CRYPTO_SYMC_WORK_MODE_CMAC,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "sm4-cmac invalid\n");
#endif
    return CRYPTO_SUCCESS;
}

static int inner_symc_mac_common_init(drv_mac_ctx_t *mac_ctx, crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    int ret;

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    /* key_len check. */
    drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");
        
    /* symc_alg check. */
    drv_err_chk_return(symc_alg != CRYPTO_SYMC_ALG_AES && symc_alg != CRYPTO_SYMC_ALG_SM4,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "alg is invalid\n");

    (void)memset_s(mac_ctx, sizeof(drv_mac_ctx_t), 0, sizeof(drv_mac_ctx_t));

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        ret = memcpy_s(mac_ctx->key, sizeof(mac_ctx->key), key, key_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }
 
    mac_ctx->key_len = key_len;
    mac_ctx->keyslot_handle = keyslot_handle;
    mac_ctx->symc_alg = symc_alg;
    mac_ctx->mode = mode;
    return 0;
}

int drv_symc_mac_ctx_init(void *ctx, uint32_t ctx_size, crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    drv_mac_ctx_t *mac_ctx = (drv_mac_ctx_t *)ctx;

    /* pointer check. */
    symc_null_ptr_chk(ctx);

    drv_err_chk_return(ctx_size < sizeof(drv_mac_ctx_t),
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "ctx_size[%u] invalid\n", ctx_size);
    return inner_symc_mac_common_init(mac_ctx, symc_alg, mode, key, key_len, keyslot_handle);
}

int drv_symc_mac_start(void **ctx, crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle)
{
    int ret;
    drv_mac_ctx_t *mac_ctx = NULL;

    /* pointer check. */
    symc_null_ptr_chk(ctx);
    mac_ctx = crypto_malloc_coherent(sizeof(drv_mac_ctx_t));
    drv_err_chk_return(mac_ctx == NULL, ERROR_MALLOC, "malloc failed\n");

    ret = inner_symc_mac_common_init(mac_ctx, symc_alg, mode, key, key_len, keyslot_handle);
    if (ret != 0) {
        crypto_free_coherent(mac_ctx);
        return ret;
    }

    *ctx = mac_ctx;
    return 0;
}

int drv_symc_mac_update(void *ctx, const unsigned char *data, unsigned int data_len)
{
    int ret;
    drv_mac_ctx_t *mac_ctx = (drv_mac_ctx_t *)ctx;
    uint32_t left = data_len;
    uint32_t processed_len = 0;
    uint32_t processing_len;
    uint32_t tail_len;

    symc_null_ptr_chk(ctx);
    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }
    symc_null_ptr_chk(data);

    ret = inner_drv_mac_ctx_chk(mac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    tail_len = mac_ctx->tail_len;
    if (((tail_len + data_len) % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) == 0) {
        left = left - 1;
    }

    processing_len = crypto_min(DRV_SYMC_MAC_BUF_LEN - tail_len, left);
    if (processing_len != 0) {
        ret = memcpy_s(mac_ctx->buf + tail_len, DRV_SYMC_MAC_BUF_LEN - tail_len, data, processing_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }

    mac_ctx->tail_len += processing_len;
    processed_len += processing_len;
    left -= processing_len;

    if (mac_ctx->tail_len == DRV_SYMC_MAC_BUF_LEN) {
        ret = hal_cipher_symc_cbc_mac(mac_ctx->symc_alg, crypto_get_phys_addr(mac_ctx->buf), DRV_SYMC_MAC_BUF_LEN,
            mac_ctx->mac, mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
        mac_ctx->tail_len = 0;
    }

    /* process blocks. */
#if defined(CONFIG_SPACC_NON_ALIGNED_ADDR_SUPPORT)
    processing_len = left - left % DRV_SYMC_MAC_BUF_LEN;
    ret = hal_cipher_symc_cbc_mac(mac_ctx->symc_alg, crypto_get_phys_addr(data + processed_len), processing_len,
        mac_ctx->mac, mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
    left -= processing_len;
    processed_len += processing_len;
#else
    while (left >= DRV_SYMC_MAC_BUF_LEN) {
        processing_len = DRV_SYMC_MAC_BUF_LEN;
        ret = memcpy_s(mac_ctx->buf, DRV_SYMC_MAC_BUF_LEN, data + processed_len, processing_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

        ret = hal_cipher_symc_cbc_mac(mac_ctx->symc_alg, crypto_get_phys_addr(mac_ctx->buf), DRV_SYMC_MAC_BUF_LEN,
            mac_ctx->mac, mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
        left -= DRV_SYMC_MAC_BUF_LEN;
        processed_len += DRV_SYMC_MAC_BUF_LEN;
    }
#endif
    if (left != 0) {
        ret = memcpy_s(mac_ctx->buf, DRV_SYMC_MAC_BUF_LEN, data + processed_len, left);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }
    mac_ctx->tail_len += left;
    if (((tail_len + data_len) % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) == 0) {
        mac_ctx->buf[mac_ctx->tail_len] = data[data_len - 1];
        mac_ctx->tail_len++;
    }

    return 0;
}

static void inner_block_left_shift_one_bit(uint8_t block[CRYPTO_AES_BLOCK_SIZE_IN_BYTES])
{
    uint8_t carry = 0;
    uint8_t next_carry;
    uint32_t i;

    for (i = CRYPTO_AES_BLOCK_SIZE_IN_BYTES - 1; i > 0; i--) {
        next_carry = (block[i] >> 7) & 0x01; // 7: right-shift 7 bits
        block[i] = (block[i] << 1) | carry;
        carry = next_carry;
    }
    block[i] = (block[i] << 1) | carry;
}

#define CMAC_RB                 0x87
#define BYTE_HIGHEST_BIT_MASK   0x80

static int32_t inner_symc_cmac_zero_data_len_proces(drv_mac_ctx_t *mac_ctx, unsigned char mac[16])
{
    int32_t ret;
    uint8_t K2[CRYPTO_AES_BLOCK_SIZE_IN_BYTES]; // store zero_block, L, K1, K2
    uint32_t alg = CRYPTO_SYMC_AES_ECB_ALG;

    if (mac_ctx->symc_alg == CRYPTO_SYMC_ALG_SM4) {
        alg = CRYPTO_SYMC_SM4_ECB_ALG;
    }
    /* Step 1. Compute L = AES-ECB-Encrypt(zero_block). */
    (void)memset_s(K2, sizeof(K2), 0, sizeof(K2));
    ret = drv_cipher_symc_crypt(alg, K2, K2, sizeof(K2), NULL,
        mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle, true);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "compute L failed\n");

    /* Step 2. Compute K1.
     * if highest_bit(L) == 1:
     *     K1 = L << 1
     *     K1 = L[0:block_size-1] || L[block_size-1] xor Rb
     * else:
     *     K1 = L << 1
     */
    if ((K2[0] & BYTE_HIGHEST_BIT_MASK) != 0) {
        inner_block_left_shift_one_bit(K2); // K1 = L << 1
        K2[CRYPTO_AES_BLOCK_SIZE_IN_BYTES - 1] ^= CMAC_RB;    // K1 = L[0:block_size-1] || L[block_size-1] xor Rb
    } else {
        inner_block_left_shift_one_bit(K2); // K1 = L << 1
    }

    /* Step 3. Compute K2.
     * if highest_bit(K1) == 1:
     *     K2 = K1 << 1
     *     K2 = K1[0:block_size-1] || K1[block_size-1] xor Rb
     * else:
     *     K2 = K1 << 1
     */
    if ((K2[0] & BYTE_HIGHEST_BIT_MASK) != 0) {
        inner_block_left_shift_one_bit(K2); // K2 = K1 << 1
        K2[CRYPTO_AES_BLOCK_SIZE_IN_BYTES - 1] ^= CMAC_RB;    // K2 = K1[0:block_size-1] || K1[block_size-1] xor Rb
    } else {
        inner_block_left_shift_one_bit(K2); // K2 = K1 << 1
    }

    /* Step 4. Compute mac.
     * final_block = 0x80 || 0x00 * 15.
     * mac = AES-ECB-Encrypt(final_block xor K2).
     * notice: final_blcok xor K2 = K2[0] xor 0x80 || K2[1:block_size]
    */
    K2[0] ^= 0x80;  // 0x80: the highest bit is 1.
    ret = drv_cipher_symc_crypt(alg, K2, mac, sizeof(K2), NULL,
        mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle, true);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "compute mac failed\n");

    ret = CRYPTO_SUCCESS;
exit_clean:
    (void)memset_s(K2, sizeof(K2), 0, sizeof(K2));
    return ret;
}

int drv_symc_mac_finish(void *ctx, unsigned char mac[16])
{
    int ret;
    drv_mac_ctx_t *mac_ctx = (drv_mac_ctx_t *)ctx;
    unsigned int tail_len;
    unsigned int processing_len;

    symc_null_ptr_chk(ctx);
    symc_null_ptr_chk(mac);
    ret = inner_drv_mac_ctx_chk(mac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    if ((mac_ctx->tail_len == 0) && (mac_ctx->mode == CRYPTO_SYMC_WORK_MODE_CMAC)) {
        /* For CMAC, data_len is zero. */
        return inner_symc_cmac_zero_data_len_proces(mac_ctx, mac);
    }

    tail_len = mac_ctx->tail_len;
    if (mac_ctx->tail_len != 0) {
        processing_len = tail_len;
        if (mac_ctx->mode == CRYPTO_SYMC_WORK_MODE_CBC_MAC) {
            if (processing_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES != 0) {
                processing_len += CRYPTO_AES_BLOCK_SIZE_IN_BYTES - tail_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
                (void)memset_s(mac_ctx->buf + tail_len, processing_len - tail_len, 0, processing_len - tail_len);
            }
            ret = hal_cipher_symc_cbc_mac(mac_ctx->symc_alg, crypto_get_phys_addr(mac_ctx->buf),
                processing_len, mac_ctx->mac, mac_ctx->key, mac_ctx->key_len, mac_ctx->keyslot_handle);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
        } else {
            ret = hal_cipher_symc_cmac(mac_ctx->symc_alg, crypto_get_phys_addr(mac_ctx->buf),
                processing_len, mac_ctx->mac, mac_ctx->key, mac_ctx->key_len,
                mac_ctx->keyslot_handle);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");
        }
    }
    ret = memcpy_s(mac, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, mac_ctx->mac, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    return 0;
}

int drv_symc_mac_destroy(void *ctx)
{
    symc_null_ptr_chk(ctx);
    (void)memset_s(ctx, sizeof(drv_mac_ctx_t), 0, sizeof(drv_mac_ctx_t));
    crypto_free_coherent(ctx);
    return 0;
}

int drv_symc_mac_compute(crypto_symc_alg symc_alg, crypto_symc_work_mode mode,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle,
    const unsigned char *data, unsigned int data_len, unsigned char mac[16])
{
    int ret;
    void *mac_ctx = NULL;

    ret = drv_symc_mac_start(&mac_ctx, symc_alg, mode, key, key_len, keyslot_handle);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_mac_start failed\n");

    ret = drv_symc_mac_update(mac_ctx, data, data_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_destroy, "drv_symc_mac_update failed\n");

    ret = drv_symc_mac_finish(mac_ctx, mac);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_destroy, "drv_symc_mac_finish failed\n");

exit_destroy:
    (void)drv_symc_mac_destroy(mac_ctx);
    return ret;
}