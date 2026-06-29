/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drv symc ghash simple implement.
 *
 * Create: 2024-07-30
*/

#include "drv_symc.h"
#include "hal_symc.h"

#include "crypto_drv_common.h"

#if defined(CONFIG_DRV_SYMC_DEBUG_ENABLE)
#define crypto_drv_symc_ghash_dump        crypto_dump_data
#else
#define crypto_drv_symc_ghash_dump(...)
#endif

#define DRV_SYMC_GHASH_BUF_LEN      256

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

typedef struct {
    unsigned char ghash[16];
    unsigned char buf[DRV_SYMC_GHASH_BUF_LEN];
    unsigned int tail_len;
    unsigned char key[32];
    unsigned int key_len;
    unsigned int keyslot_handle;
    crypto_symc_alg symc_alg;
} drv_ghash_ctx_t;

int drv_symc_ghash_start(void **ctx, crypto_symc_alg symc_alg,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle)
{
    int ret;
    drv_ghash_ctx_t *ghash_ctx = NULL;

    symc_null_ptr_chk(ctx);
    /* pointer check. */
    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    /* key_len check. */
    crypto_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");

    ghash_ctx = crypto_malloc_coherent(sizeof(drv_ghash_ctx_t));
    crypto_chk_return(ghash_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(ghash_ctx, sizeof(drv_ghash_ctx_t), 0, sizeof(drv_ghash_ctx_t));

    if (keyslot_handle == 0xFFFFFFFF) {
        ret = memcpy_s(ghash_ctx->key, sizeof(ghash_ctx->key), key, key_len);
        crypto_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }
    ghash_ctx->key_len = key_len;
    ghash_ctx->keyslot_handle = keyslot_handle;
    ghash_ctx->symc_alg = symc_alg;

    *ctx = ghash_ctx;
    return 0;
error_free:
    crypto_free_coherent(ghash_ctx);
    return ret;
}

int drv_symc_ghash_update(void *ctx, const unsigned char *data, unsigned int data_len)
{
    int ret;
    drv_ghash_ctx_t *ghash_ctx = (drv_ghash_ctx_t *)ctx;
    uint32_t left = data_len;
    uint32_t processed_len = 0;
    uint32_t processing_len;
    uint32_t tail_len;

    symc_null_ptr_chk(ctx);
    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }
    symc_null_ptr_chk(data);
    tail_len = ghash_ctx->tail_len;

    crypto_drv_symc_ghash_dump("ghash before", ghash_ctx->ghash, sizeof(ghash_ctx->ghash));
    crypto_drv_symc_ghash_dump("ghash data", data, data_len);

    processing_len = crypto_min(DRV_SYMC_GHASH_BUF_LEN - tail_len, data_len);
    ret = memcpy_s(ghash_ctx->buf + tail_len, DRV_SYMC_GHASH_BUF_LEN - tail_len, data, processing_len);
    crypto_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ghash_ctx->tail_len += processing_len;
    processed_len += processing_len;
    left -= processing_len;

    if (ghash_ctx->tail_len == DRV_SYMC_GHASH_BUF_LEN) {
        ret = hal_cipher_symc_gcm_ghash(ghash_ctx->symc_alg, crypto_get_phys_addr(ghash_ctx->buf),
            DRV_SYMC_GHASH_BUF_LEN, ghash_ctx->ghash, ghash_ctx->key, ghash_ctx->key_len, ghash_ctx->keyslot_handle);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_gcm_ghash failed\n");
        ghash_ctx->tail_len = 0;
    }

#if defined(CONFIG_SPACC_NON_ALIGNED_ADDR_SUPPORT)
    processing_len = left - left % DRV_SYMC_GHASH_BUF_LEN;
    ret = hal_cipher_symc_gcm_ghash(ghash_ctx->symc_alg, crypto_get_phys_addr(data + processed_len),
        processing_len, ghash_ctx->ghash, ghash_ctx->key, ghash_ctx->key_len, ghash_ctx->keyslot_handle);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_gcm_ghash failed\n");

    left -= processing_len;
    processed_len += processing_len;
#else
    while (left >= DRV_SYMC_GHASH_BUF_LEN) {
        processing_len = DRV_SYMC_GHASH_BUF_LEN;
        ret = memcpy_s(ghash_ctx->buf, DRV_SYMC_GHASH_BUF_LEN, data + processed_len, processing_len);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ERROR_MEMCPY_S, "memcpy_s failed\n");

        ret = hal_cipher_symc_gcm_ghash(ghash_ctx->symc_alg, crypto_get_phys_addr(ghash_ctx->buf),
            DRV_SYMC_GHASH_BUF_LEN, ghash_ctx->ghash, ghash_ctx->key, ghash_ctx->key_len, ghash_ctx->keyslot_handle);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_gcm_ghash failed\n");

        left -= processing_len;
        processed_len += processing_len;
    }
#endif
    if (left != 0) {
        (void)memcpy_s(ghash_ctx->buf, DRV_SYMC_GHASH_BUF_LEN, data + processed_len, left);
    }
    ghash_ctx->tail_len += left;

    crypto_drv_symc_ghash_dump("ghash after", ghash_ctx->ghash, sizeof(ghash_ctx->ghash));

    return 0;
}

int drv_symc_ghash_finish(void *ctx, unsigned char ghash[16])
{
    int ret;
    drv_ghash_ctx_t *ghash_ctx = (drv_ghash_ctx_t *)ctx;
    unsigned int tail_len;
    unsigned int processing_len;

    symc_null_ptr_chk(ctx);
    symc_null_ptr_chk(ghash);

    tail_len = ghash_ctx->tail_len;

    if (tail_len != 0) {
        processing_len = tail_len;
        if (tail_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES != 0) {
            processing_len += CRYPTO_AES_BLOCK_SIZE_IN_BYTES - tail_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
            (void)memset_s(ghash_ctx->buf + tail_len, processing_len - tail_len, 0, processing_len - tail_len);
        }

        ret = hal_cipher_symc_gcm_ghash(ghash_ctx->symc_alg, crypto_get_phys_addr(ghash_ctx->buf), processing_len,
            ghash_ctx->ghash, ghash_ctx->key, ghash_ctx->key_len, ghash_ctx->keyslot_handle);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_gcm_ghash failed\n");
    }
    ret = memcpy_s(ghash, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, ghash_ctx->ghash, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    return 0;
}

int drv_symc_ghash_destroy(void *ctx)
{
    (void)memset_s(ctx, sizeof(drv_ghash_ctx_t), 0, sizeof(drv_ghash_ctx_t));
    crypto_free_coherent(ctx);
    return 0;
}