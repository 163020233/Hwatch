/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: drv hmac compute. \n
 *
 * History: \n
 * 2025-05-27, Create file. \n
 */
#include "drv_hash.h"
#include "hal_hash.h"

#include "crypto_hash_common.h"

#define FILE_ID FILE_ID_DRV_CODE_HASH_DRV_HMAC_COMPUTE

CRYPTO_STATIC int32_t inner_drv_hmac_ctx_chk(const drv_hmac_ctx *ctx)
{
    uint32_t block_size = crypto_hash_get_block_size(ctx->hash_type) / CRYPTO_BITS_IN_BYTE;

    drv_err_chk_return(ctx->block_size != block_size, ERROR_INVALID_PARAM,
        "invalid block_size\n");

    return crypto_hash_common_type_check(ctx->hash_type);
}

int drv_cipher_hmac_compute_setup(void **ctx)
{
    int ret;
    drv_hmac_ctx *hmac_ctx = NULL;

    hash_null_ptr_chk(ctx);

    hmac_ctx = crypto_malloc_coherent(sizeof(drv_hmac_ctx));
    drv_err_chk_return(hmac_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(hmac_ctx, sizeof(drv_hmac_ctx), 0, sizeof(drv_hmac_ctx));

    ret = drv_cipher_hash_compute_setup(&(hmac_ctx->hash_ctx));
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_free, "hash_setup failed\n");

    *ctx = hmac_ctx;
    return CRYPTO_SUCCESS;
error_free:
    crypto_free_coherent(hmac_ctx);
    *ctx = NULL;
    return ret;
}

int drv_cipher_hmac_compute_start(void *ctx, crypto_hash_type hash_type,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle)
{
    int ret;
    unsigned int i;
    unsigned char i_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX] = { 0 };
    drv_hmac_ctx *hmac_ctx = (drv_hmac_ctx *)ctx;
    unsigned int block_size = crypto_hash_get_block_size(hash_type) / CRYPTO_BITS_IN_BYTE;
    unsigned int result_size = crypto_hash_get_result_size(hash_type) / CRYPTO_BITS_IN_BYTE;

    hash_null_ptr_chk(ctx);
    hash_null_ptr_chk(key);
    drv_err_chk_return(block_size > CRYPTO_HASH_BLOCK_SIZE_MAX,
        ERROR_INVALID_PARAM, "invalid block_size!\n");
    drv_err_chk_return(result_size > CRYPTO_HASH_RESULT_SIZE_MAX,
        ERROR_INVALID_PARAM, "invalid result_size!\n");

    if (keyslot_handle != CRYPTO_INVALID_KEYSLOT_HANDLE) {

        drv_set_err(ERROR_UNSUPPORT, "direct error return");
        return ERROR_UNSUPPORT;
    }

    if (key_len <= block_size) {
        ret = memcpy_s(i_key_pad, sizeof(i_key_pad), key, key_len);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    } else {
        ret = drv_cipher_hash_compute(crypto_hash_remove_hmac_flag(hash_type), key, key_len,
            i_key_pad, result_size);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash compute failed\n");
    }
    for (i = 0; i < block_size; i++) {
        hmac_ctx->o_key_pad[i] = 0x5C ^ i_key_pad[i];   // 0x5c: algo requirements
        i_key_pad[i] = 0x36 ^ i_key_pad[i]; // 0x36: algo requirements
    }

    ret = drv_cipher_hash_compute_start(hmac_ctx->hash_ctx, crypto_hash_remove_hmac_flag(hash_type));
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_clean, "hash start failed\n");

    ret = drv_cipher_hash_compute_update(hmac_ctx->hash_ctx, i_key_pad, block_size);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_exit_clean, "hash update failed\n");

    hmac_ctx->block_size = block_size;
    hmac_ctx->hash_type = hash_type;
exit_clean:
    (void)memset_s(i_key_pad, sizeof(i_key_pad), 0, sizeof(i_key_pad));
    return ret;
error_exit_clean:
    (void)drv_cipher_hash_compute_teardown(hmac_ctx->hash_ctx);
    hmac_ctx->hash_ctx = NULL;
    (void)memset_s(i_key_pad, sizeof(i_key_pad), 0, sizeof(i_key_pad));
    return ret;
}

int drv_cipher_hmac_compute_update(void *ctx, const unsigned char *data, unsigned int data_len)
{
    int32_t ret;
    drv_hmac_ctx *hmac_ctx = (drv_hmac_ctx *)ctx;

    hash_null_ptr_chk(ctx);
    ret = inner_drv_hmac_ctx_chk(hmac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    return drv_cipher_hash_compute_update(hmac_ctx->hash_ctx, data, data_len);
}

int drv_cipher_hmac_compute_finish(void *ctx, unsigned char *out_hash, unsigned int out_hash_len)
{
    int ret;
    drv_hmac_ctx *hmac_ctx = (drv_hmac_ctx *)ctx;

    hash_null_ptr_chk(ctx);
    hash_null_ptr_chk(out_hash);
    ret = inner_drv_hmac_ctx_chk(hmac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = drv_cipher_hash_compute_finish(hmac_ctx->hash_ctx, out_hash, out_hash_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash finish failed\n");

    ret = drv_cipher_hash_compute_start(hmac_ctx->hash_ctx, crypto_hash_remove_hmac_flag(hmac_ctx->hash_type));
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash compute failed\n");

    ret = drv_cipher_hash_compute_update(hmac_ctx->hash_ctx, hmac_ctx->o_key_pad,
        hmac_ctx->block_size);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash update failed\n");

    ret = drv_cipher_hash_compute_update(hmac_ctx->hash_ctx, out_hash, out_hash_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash update failed\n");

    ret = drv_cipher_hash_compute_finish(hmac_ctx->hash_ctx, out_hash, out_hash_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hash finish failed\n");

    return CRYPTO_SUCCESS;
}

int drv_cipher_hmac_compute_teardown(void *ctx)
{
    drv_hmac_ctx *hmac_ctx = (drv_hmac_ctx *)ctx;
    hash_null_ptr_chk(ctx);

    (void)drv_cipher_hash_compute_teardown(hmac_ctx->hash_ctx);
    (void)memset_s(hmac_ctx, sizeof(drv_hmac_ctx), 0, sizeof(drv_hmac_ctx));
    crypto_free_coherent(hmac_ctx);

    return CRYPTO_SUCCESS;
}

static int inner_hmac_compute_without_keyslot(crypto_hash_type hash_type,
    const unsigned char *key, unsigned int key_len,
    const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len)
{
    void *hmac_ctx = NULL;
    int ret;

    ret = drv_cipher_hmac_compute_setup(&hmac_ctx);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hmac setup failed\n");

    ret = drv_cipher_hmac_compute_start(hmac_ctx, hash_type, key, key_len, CRYPTO_INVALID_KEYSLOT_HANDLE);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac start failed\n");

    ret = drv_cipher_hmac_compute_update(hmac_ctx, buf, buf_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac update failed\n");

    ret = drv_cipher_hmac_compute_finish(hmac_ctx, out_hash, out_hash_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "hmac finish failed\n");

exit_teardown:
    (void)drv_cipher_hmac_compute_teardown(hmac_ctx);
    return ret;
}

static int inner_hmac_compute_with_keyslot(crypto_hash_type hmac_type,
    unsigned int keyslot_handle,
    const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len)
{
    int ret;
    uint32_t block_size;
    uint32_t result_size;
    uint32_t max_message_len;
    uint32_t processing_len;
    uint32_t tail_len;
    uint32_t length_reverse;
    uint8_t *tmp_buf = NULL;
    uint32_t tmp_buf_len = buf_len + CRYPTO_HASH_BLOCK_SIZE_MAX * 2;    // 2: max 2 blocks.
    uint32_t state[CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD];
    uint32_t state_size = (uint32_t)sizeof(state);

    drv_err_chk_return(tmp_buf_len < buf_len, ERROR_INVALID_PARAM, "invalid buf_len\n");

    ret = crypto_hash_common_get_state(hmac_type, state, &state_size);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    tmp_buf = crypto_malloc_coherent(tmp_buf_len);
    drv_err_chk_return(tmp_buf == NULL, ERROR_MALLOC, "malloc failed\n");

    ret = memcpy_s(tmp_buf, tmp_buf_len, buf, buf_len);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    (void)memset_s(tmp_buf + buf_len, tmp_buf_len - buf_len, 0, tmp_buf_len - buf_len);
    tmp_buf[buf_len] = 0x80;

    block_size = crypto_hash_get_block_size(hmac_type) / CRYPTO_BITS_IN_BYTE;
    max_message_len = crypto_hash_get_message_len(hmac_type) / CRYPTO_BITS_IN_BYTE;
    result_size = crypto_hash_get_result_size(hmac_type) / CRYPTO_BITS_IN_BYTE;

    tail_len = buf_len % block_size;
    processing_len = buf_len + (block_size - tail_len);
    if (tail_len + 1 + max_message_len > block_size) {
        processing_len += block_size;
    }
    length_reverse = crypto_cpu_to_be32((buf_len + block_size) * CRYPTO_BITS_IN_BYTE);
    ret = memcpy_s(tmp_buf + processing_len - sizeof(length_reverse), sizeof(length_reverse),
        &length_reverse, sizeof(length_reverse));
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = hal_cipher_hmac_compute(hmac_type, crypto_get_phys_addr(tmp_buf), processing_len,
        keyslot_handle, state, state_size);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hmac compute failed\n");

    ret = memcpy_s(out_hash, out_hash_len, state, result_size);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy_s failed\n");

    ret = CRYPTO_SUCCESS;
exit_free:
    (void)memset_s(tmp_buf, tmp_buf_len, 0, tmp_buf_len);
    crypto_free_coherent(tmp_buf);
    return ret;
}

int drv_cipher_hmac_compute(crypto_hash_type hash_type,
    const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle,
    const unsigned char *buf, unsigned int buf_len,
    unsigned char *out_hash, unsigned int out_hash_len)
{
    if (buf_len != 0) {
        hash_null_ptr_chk(buf);
    }
    hash_null_ptr_chk(out_hash);

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        return inner_hmac_compute_without_keyslot(hash_type, key, key_len, buf, buf_len,
            out_hash, out_hash_len);
    }

    return inner_hmac_compute_with_keyslot(hash_type, keyslot_handle, buf, buf_len,
        out_hash, out_hash_len);
}