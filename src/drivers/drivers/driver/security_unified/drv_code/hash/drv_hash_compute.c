/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: drv hash compute. \n
 *
 * History: \n
 * 2025-05-27, Create file. \n
 */
#include "drv_hash.h"
#include "hal_hash.h"

#include "crypto_hash_common.h"

#define FILE_ID FILE_ID_DRV_CODE_HASH_DRV_HASH_COMPUTE

CRYPTO_STATIC int32_t inner_drv_hash_ctx_chk(const drv_hash_ctx *ctx)
{
    const uint8_t *state_iv = NULL;
    uint32_t iv_size = 0;
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)  // cause ws53/ws63 smoke test failed, fix it later.
    drv_err_chk_return((ctx->check_word ^ (uintptr_t)ctx) != 0, ERROR_INVALID_PARAM, "invalid hash_ctx\n");
#endif
    drv_err_chk_return(ctx->tail_len >= CONFIG_DRV_HASH_BUF_LEN, ERROR_INVALID_PARAM,
        "invalid tail_len\n");

    state_iv = crypto_hash_common_get_state_iv(ctx->hash_type, &iv_size);
    drv_err_chk_return(state_iv == NULL, ERROR_INVALID_PARAM, "invalid hash_type\n");
    drv_err_chk_return(ctx->state_size != iv_size, ERROR_INVALID_PARAM, "invalid state_size\n");

    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute_setup(void **ctx)
{
    drv_hash_ctx *hash_ctx = NULL;

    hash_null_ptr_chk(ctx);
    hash_ctx = crypto_malloc_coherent(sizeof(drv_hash_ctx));
    drv_err_chk_return(hash_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(hash_ctx, sizeof(drv_hash_ctx), 0, sizeof(drv_hash_ctx));
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    hash_ctx->check_word = ((uintptr_t)hash_ctx);
#endif
    *ctx = hash_ctx;
    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute_start(void *ctx, crypto_hash_type hash_type)
{
    int ret;
    drv_hash_ctx *hash_ctx = (drv_hash_ctx *)ctx;

    hash_null_ptr_chk(ctx);
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    drv_err_chk_return((hash_ctx->check_word ^ (uintptr_t)ctx) != 0, ERROR_INVALID_PARAM, "invalid hash_ctx\n");
#endif
    hash_ctx->state_size = (uint32_t)sizeof(hash_ctx->state);
    ret = crypto_hash_common_get_state(hash_type, hash_ctx->state, &(hash_ctx->state_size));
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_hash_common_get_state failed\n");

    hash_ctx->hash_type = hash_type;
    hash_ctx->tail_len = 0;
    hash_ctx->length[0] = 0;
    hash_ctx->length[1] = 0;

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_hash_compute_clone(void *dst_ctx, const void *src_ctx)
{
    int32_t ret;
    const drv_hash_ctx *hash_src_ctx = (const drv_hash_ctx *)src_ctx;
    drv_hash_ctx *hash_dst_ctx = (drv_hash_ctx *)dst_ctx;

    hash_null_ptr_chk(dst_ctx);
    hash_null_ptr_chk(src_ctx);
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    drv_err_chk_return((hash_dst_ctx->check_word ^ (uintptr_t)dst_ctx) != 0, ERROR_INVALID_PARAM, "invalid dst_ctx\n");
    drv_err_chk_return((hash_src_ctx->check_word ^ (uintptr_t)src_ctx) != 0, ERROR_INVALID_PARAM, "invalid src_ctx\n");
#endif
    ret = memcpy_s(hash_dst_ctx, sizeof(drv_hash_ctx), hash_src_ctx, sizeof(drv_hash_ctx));
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    hash_dst_ctx->check_word = (uintptr_t)dst_ctx;
#endif
    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute_update(void *ctx, const unsigned char *data, unsigned int data_len)
{
    int ret;
    drv_hash_ctx *hash_ctx = (drv_hash_ctx *)ctx;
    unsigned int block_size;
    unsigned int left = data_len;
    unsigned int processing_len;
    unsigned int processed_len = 0;
    unsigned long long phys_addr = crypto_get_phys_addr(data);

    hash_null_ptr_chk(ctx);
    if (data_len != 0) {
        hash_null_ptr_chk(data);
    }
    ret = inner_drv_hash_ctx_chk(hash_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }
    block_size = crypto_hash_get_block_size(hash_ctx->hash_type) / CRYPTO_BITS_IN_BYTE;

    if (hash_ctx->tail_len == 0 && crypto_addr_align_check(phys_addr) == TD_TRUE &&
        data_len >= CONFIG_DRV_HASH_BUF_LEN) {
        processing_len = data_len - data_len % block_size;
        ret = hal_cipher_hash_compute(hash_ctx->hash_type, phys_addr,
            processing_len, hash_ctx->state, hash_ctx->state_size);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");

        processed_len += processing_len;
        left -= processing_len;

        ret = memcpy_s(hash_ctx->buf, CONFIG_DRV_HASH_BUF_LEN, data + processed_len, left);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

        hash_ctx->tail_len = left;

        crypto_hash_common_length_add(hash_ctx->length, data_len * CRYPTO_BITS_IN_BYTE);
        return CRYPTO_SUCCESS;
    }

    if ((hash_ctx->tail_len + data_len) < CONFIG_DRV_HASH_BUF_LEN) {
        ret = memcpy_s(hash_ctx->buf + hash_ctx->tail_len, CONFIG_DRV_HASH_BUF_LEN - hash_ctx->tail_len,
            data, data_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
        hash_ctx->tail_len = hash_ctx->tail_len + data_len;

        crypto_hash_common_length_add(hash_ctx->length, data_len * CRYPTO_BITS_IN_BYTE);
        return CRYPTO_SUCCESS;
    }

    processing_len = CONFIG_DRV_HASH_BUF_LEN - hash_ctx->tail_len;
    ret = memcpy_s(hash_ctx->buf + hash_ctx->tail_len, CONFIG_DRV_HASH_BUF_LEN - hash_ctx->tail_len,
        data, processing_len);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = hal_cipher_hash_compute(hash_ctx->hash_type, crypto_get_phys_addr(hash_ctx->buf),
        CONFIG_DRV_HASH_BUF_LEN, hash_ctx->state, hash_ctx->state_size);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");
    left -= processing_len;
    processed_len += processing_len;

#if defined(CONFIG_SPACC_NON_ALIGNED_ADDR_SUPPORT)
    processing_len = left - left % CONFIG_DRV_HASH_BUF_LEN;

    ret = hal_cipher_hash_compute(hash_ctx->hash_type, crypto_get_phys_addr(data + processed_len),
        processing_len, hash_ctx->state, hash_ctx->state_size);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");

    left -= processing_len;
    processed_len += processing_len;
#else
    while (left >= CONFIG_DRV_HASH_BUF_LEN) {
        processing_len = CONFIG_DRV_HASH_BUF_LEN;

        ret = memcpy_s(hash_ctx->buf, CONFIG_DRV_HASH_BUF_LEN, data + processed_len, processing_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

        ret = hal_cipher_hash_compute(hash_ctx->hash_type, crypto_get_phys_addr(hash_ctx->buf),
            processing_len, hash_ctx->state, hash_ctx->state_size);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");

        left -= processing_len;
        processed_len += processing_len;
    }
#endif
    if (left != 0) {
        ret = memcpy_s(hash_ctx->buf, CONFIG_DRV_HASH_BUF_LEN, data + processed_len, left);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }
    hash_ctx->tail_len = left;

    crypto_hash_common_length_add(hash_ctx->length, data_len * CRYPTO_BITS_IN_BYTE);
    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute_finish(void *ctx, unsigned char *out_hash, unsigned int out_hash_len)
{
    int ret;
    drv_hash_ctx *hash_ctx = (drv_hash_ctx *)ctx;
    unsigned int max_message_len;
    unsigned int block_size;
    unsigned int tail_len;
    unsigned int processing_len;

    hash_null_ptr_chk(ctx);
    hash_null_ptr_chk(out_hash);
    ret = inner_drv_hash_ctx_chk(hash_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    max_message_len = crypto_hash_get_message_len(hash_ctx->hash_type) / CRYPTO_BITS_IN_BYTE;
    block_size = crypto_hash_get_block_size(hash_ctx->hash_type) / CRYPTO_BITS_IN_BYTE;
    tail_len = hash_ctx->tail_len;
    /* Hint-1.1 Here tail_len <= CONFIG_DRV_HASH_BUF_LEN guaranteed by inner_drv_hash_ctx_chk,
     * so processing_len <= CONFIG_DRV_HASH_BUF_LEN. */
    processing_len = tail_len + (block_size - tail_len % block_size);

    if (tail_len % block_size + 1 + max_message_len > block_size) {
        /* Hint-1.2 Here block_size <= CRYPTO_HASH_BLOCK_SIZE_MAX,
         * so processing_len <= CONFIG_DRV_HASH_BUF_LEN + CRYPTO_HASH_BLOCK_SIZE_MAX
        */
        processing_len += block_size;
    }
    hash_ctx->buf[tail_len] = 0x80;
    hash_ctx->length[1] = crypto_cpu_to_be32(hash_ctx->length[1]);
    hash_ctx->length[0] = crypto_cpu_to_be32(hash_ctx->length[0]);
    /* Result-1.1 According to Hint-1.1 and Hint-1.2, processing_len <= sizeof(hash_ctx->buf),
     * so memory 'hash_ctx->buf' overflow will not happen. */
    (void)memset_s(hash_ctx->buf + tail_len + 1, processing_len - 1 - tail_len, 0, processing_len - 1 - tail_len);

    /* Result-1.2 According to Hint-1.1 and Hint-1.2, processing_len <= sizeof(hash_ctx->buf),
     * so memory 'hash_ctx->buf + processing_len - sizeof(hash_ctx->length)'
     * has enough space to store hash_ctx->length.  */
    ret = memcpy_s(hash_ctx->buf + processing_len - sizeof(hash_ctx->length), sizeof(hash_ctx->length),
        hash_ctx->length, sizeof(hash_ctx->length));
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = hal_cipher_hash_compute(hash_ctx->hash_type, crypto_get_phys_addr(hash_ctx->buf),
        processing_len, hash_ctx->state, hash_ctx->state_size);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");

    ret = memcpy_s(out_hash, out_hash_len, hash_ctx->state, out_hash_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_hash_compute failed\n");

    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute_teardown(void *ctx)
{
    drv_hash_ctx *hash_ctx = (drv_hash_ctx *)ctx;
    hash_null_ptr_chk(hash_ctx);
#if defined(CONFIG_HASH_ADDR_CHECK_ENABLE)
    drv_err_chk_return((hash_ctx->check_word ^ (uintptr_t)ctx) != 0, ERROR_INVALID_PARAM, "invalid hash_ctx\n");
#endif
    (void)memset_s(hash_ctx, sizeof(drv_hash_ctx), 0, sizeof(drv_hash_ctx));
    crypto_free_coherent(ctx);

    return CRYPTO_SUCCESS;
}

int drv_cipher_hash_compute(crypto_hash_type hash_type, const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len)
{
    int ret;
    void *hash_ctx = NULL;

    ret = drv_cipher_hash_compute_setup(&hash_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_hash_compute_setup failed\n");

    ret = drv_cipher_hash_compute_start(hash_ctx, hash_type);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_hash_compute_start failed\n");

    ret = drv_cipher_hash_compute_update(hash_ctx, buf, buf_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_hash_compute_update failed\n");

    ret = drv_cipher_hash_compute_finish(hash_ctx, out_hash, out_hash_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_hash_compute_finish failed\n");

    ret = CRYPTO_SUCCESS;
exit_teardown:
    (void)drv_cipher_hash_compute_teardown(hash_ctx);
    return ret;
}
