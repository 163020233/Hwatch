/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drv symc gcm simple implement.
 *
 * Create: 2024-07-30
*/

#include "drv_symc.h"
#include "hal_symc.h"
#include "hal_keyslot.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_DRV_CODE_SYMC_DRV_SYMC_GCM

#if defined(CONFIG_DRV_SYMC_DEBUG_ENABLE)
#define crypto_drv_symc_gcm_log         crypto_print
#define crypto_drv_symc_gcm_dump        crypto_dump_data
#else
#define crypto_drv_symc_gcm_log(...)
#define crypto_drv_symc_gcm_dump(...)
#endif

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    drv_err_chk_return((ptr) == TD_NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

static void inner_iv_plus_one(uint8_t *iv, uint32_t iv_len)
{
    uint32_t value = CRYPTO_BYTE_MAX + 1;
    int32_t i = (int32_t)iv_len - 1;

    while (value > CRYPTO_BYTE_MAX && i >= 0) {
        value = iv[i] + 1;
        iv[i] = value % (CRYPTO_BYTE_MAX + 1);
        iv--;
    }
}

static int32_t inner_gcm_compute_j0(crypto_symc_alg symc_alg, const unsigned char *iv, unsigned int iv_len,
    unsigned char j0[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_handle)
{
    int32_t ret;
    uint8_t iv_padding[CRYPTO_AES_BLOCK_SIZE_IN_BYTES * 2];
    uint8_t iv_padding_len = sizeof(iv_padding);
    uint32_t iv_len_in_bit = iv_len * CRYPTO_BITS_IN_BYTE;
    void *ghash_ctx = NULL;
    crypto_hal_func_enter();

    iv_len_in_bit = crypto_cpu_to_be32(iv_len_in_bit);

    (void)memset_s(j0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, 0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    /* If the IV length is 12 bytes, J0 = IV || 00000000 00000000 00000000 00000001 */
    if (iv_len == CRYPTO_GCM_SPECIAL_IV_BYTES) { // for 96 bits, J0 = IV || 00000001
        ret = memcpy_s(j0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, iv, iv_len);
        drv_err_chk_return(ret != EOK, (ERROR_MEMCPY_S), "memcpy_s failed\n");
        j0[CRYPTO_AES_BLOCK_SIZE_IN_BYTES - 1] = 0x01;  // j0[15] = 0x01
    } else { /* If the IV length is not 12 bytes, J0 = GHASH{IV || [00..00] || len(IV)} */
        if (iv_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES == 0) {
            iv_padding_len = iv_padding_len - CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        } else {
            iv_padding_len = iv_padding_len - iv_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
        (void)memset_s(iv_padding, sizeof(iv_padding), 0, sizeof(iv_padding));
        ret = memcpy_s(iv_padding + iv_padding_len - sizeof(unsigned int), sizeof(unsigned int),
            &iv_len_in_bit, sizeof(unsigned int));
        drv_err_chk_return(ret != EOK, (ERROR_MEMCPY_S), "memcpy_s failed\n");

        ret = drv_symc_ghash_start(&ghash_ctx, symc_alg, key, key_len, keyslot_handle);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "ghash start failed\n");

        ret = drv_symc_ghash_update(ghash_ctx, iv, iv_len);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_ghash_destroy, "ghash update failed\n");

        ret = drv_symc_ghash_update(ghash_ctx, iv_padding, iv_padding_len);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_ghash_destroy, "ghash update failed\n");

        ret = drv_symc_ghash_finish(ghash_ctx, j0);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_ghash_destroy, "ghash update failed\n");
    }

exit_ghash_destroy:
    if (ghash_ctx != NULL) {
        drv_symc_ghash_destroy(ghash_ctx);
    }
    crypto_hal_func_exit();
    return ret;
}

static void inner_gcm_format_clen(unsigned int aad_len, unsigned int data_len, unsigned char gcm_clen[16])
{
    unsigned int aad_len_in_bit = 0;
    unsigned int data_len_in_bit = 0;

    aad_len_in_bit = crypto_cpu_to_be32(aad_len * CRYPTO_BITS_IN_BYTE);
    data_len_in_bit = crypto_cpu_to_be32(data_len * CRYPTO_BITS_IN_BYTE);

    (td_void)memset_s(gcm_clen, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, 0x00, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    (td_void)memcpy_s(gcm_clen + 8 - sizeof(unsigned int),        /* 8: bit15~8 for aad_len. */
        sizeof(unsigned int), &aad_len_in_bit, sizeof(unsigned int));
    (td_void)memcpy_s(gcm_clen + 16 - sizeof(unsigned int), sizeof(unsigned int),   /* 16: bit7~0 for data_len. */
        &data_len_in_bit, sizeof(unsigned int));
}

static int32_t inner_gcm_crypt(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *input, uint8_t *output, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len, bool is_encrypt)
{
    int ret;
    void *gcm_ctx = NULL;

    ret = drv_cipher_symc_gcm_setup(&gcm_ctx, symc_alg, key, key_len, keyslot_handle, iv, iv_len, is_encrypt);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_gcm_setup failed\n");

    ret = drv_cipher_symc_gcm_update_ad(gcm_ctx, aad, aad_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_gcm_update_ad failed\n");

    ret = drv_cipher_symc_gcm_update(gcm_ctx, input, output, data_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_gcm_update failed\n");

    ret = drv_cipher_symc_gcm_finish(gcm_ctx, tag, tag_len);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_teardown, "drv_cipher_symc_gcm_finish failed\n");

exit_teardown:
    (void)drv_cipher_symc_gcm_teardown(gcm_ctx);
    return ret;
}

int32_t drv_cipher_symc_gcm_encrypt(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len)
{
    int ret;

    ret = inner_gcm_crypt(symc_alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len, plain_text, cipher_text,
        data_len, tag, tag_len, true);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "crypt failed\n");

    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(cipher_text, data_len, 0, data_len);
    return ret;
}

int32_t drv_cipher_symc_gcm_decrypt_verify(crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len)
{
    int ret;
    unsigned int chk_sum = 0;
    unsigned int i;
    unsigned char out_tag[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];

    symc_null_ptr_chk(tag);

    ret = inner_gcm_crypt(symc_alg, key, key_len, keyslot_handle, iv, iv_len, aad, aad_len, cipher_text, plain_text,
        data_len, out_tag, tag_len, false);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_clean, "crypt failed\n");

    for (i = 0; i < tag_len; i++) {
        chk_sum += tag[i] ^ out_tag[i];
    }
    if (chk_sum != 0) {
        drv_set_err(ERROR_UNEXPECTED, "logged error");
        ret = SYMC_COMPAT_ERRNO(ERROR_SYMC_AEAD_TAG_INVALID);
        goto error_clean;
    }

    return CRYPTO_SUCCESS;
error_clean:
    (void)memset_s(plain_text, data_len, 0, data_len);
    return ret;
}

typedef struct {
    crypto_symc_alg symc_alg;
    uint8_t key[CRYPTO_256_KEY_LEN];
    uint8_t j0[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t gcm_iv[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t gcm_ghash[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t tag[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint8_t stream_block[CRYPTO_AES_BLOCK_SIZE_IN_BYTES];
    uint32_t keyslot_handle;
    uint32_t key_len;
    uint32_t aad_len;
    uint32_t data_len;
    void *ghash_ctx;
    void *ctr_ctx;
    uint8_t nc_off;
    bool is_local_keyslot;
    bool is_encrypt;
    bool aad_is_finish;
} drv_symc_gcm_ctx;

int32_t drv_cipher_symc_gcm_setup(void **ctx, crypto_symc_alg symc_alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len, bool is_encrypt)
{
    int ret;
    drv_symc_gcm_ctx *gcm_ctx = NULL;

    /* pointer check. */
    symc_null_ptr_chk(ctx);
    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    if (iv_len != 0) {
        symc_null_ptr_chk(iv);
    }
    /* symc_alg check. */
    drv_err_chk_return(symc_alg != CRYPTO_SYMC_ALG_AES && symc_alg != CRYPTO_SYMC_ALG_SM4,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "symc_alg is invalid\n");
    /* key_len check. */
    drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");

    gcm_ctx = crypto_malloc_coherent(sizeof(drv_symc_gcm_ctx));
    drv_err_chk_return(gcm_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(gcm_ctx, sizeof(drv_symc_gcm_ctx), 0, sizeof(drv_symc_gcm_ctx));

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        ret = memcpy_s(gcm_ctx->key, sizeof(gcm_ctx->key), key, key_len);
        drv_err_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }

    gcm_ctx->keyslot_handle = keyslot_handle;
    gcm_ctx->is_encrypt = is_encrypt;
    gcm_ctx->key_len = key_len;
    gcm_ctx->symc_alg = symc_alg;

    /* compute j0. */
    ret = inner_gcm_compute_j0(symc_alg, iv, iv_len, gcm_ctx->j0, gcm_ctx->key, gcm_ctx->key_len,
        gcm_ctx->keyslot_handle);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_free, "compute j0 failed\n");
    crypto_drv_symc_gcm_dump("j0", gcm_ctx->j0, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    (void)memcpy_s(gcm_ctx->gcm_iv, sizeof(gcm_ctx->gcm_iv), gcm_ctx->j0, sizeof(gcm_ctx->j0));
    inner_iv_plus_one(gcm_ctx->gcm_iv, sizeof(gcm_ctx->gcm_iv));
    crypto_drv_symc_gcm_dump("gcm_iv", gcm_ctx->gcm_iv, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    /* ghash start. */
    ret = drv_symc_ghash_start(&(gcm_ctx->ghash_ctx), symc_alg, gcm_ctx->key, gcm_ctx->key_len,
        gcm_ctx->keyslot_handle);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_free, "drv_symc_ghash_start failed\n");

    /* ctr setup. */
    ret = drv_cipher_symc_setup(&(gcm_ctx->ctr_ctx),
        crypto_compat_symc_alg(symc_alg, CRYPTO_SYMC_WORK_MODE_GCM, CRYPTO_SYMC_BIT_WIDTH_128BIT),
        gcm_ctx->key, gcm_ctx->key_len, gcm_ctx->keyslot_handle, is_encrypt);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_ghash_destroy, "drv_cipher_symc_setup failed\n");

    *ctx = gcm_ctx;
    return CRYPTO_SUCCESS;

error_ghash_destroy:
    (void)drv_symc_ghash_destroy(gcm_ctx->ghash_ctx);
error_free:
    (void)memset_s(gcm_ctx, sizeof(drv_symc_gcm_ctx), 0, sizeof(drv_symc_gcm_ctx));
    crypto_free_coherent(gcm_ctx);
    return ret;
}

int32_t drv_cipher_symc_gcm_update_ad(void *ctx, const uint8_t *aad, uint32_t aad_len)
{
    int ret;
    drv_symc_gcm_ctx *gcm_ctx = (drv_symc_gcm_ctx *)ctx;

    symc_null_ptr_chk(gcm_ctx);
    if (aad_len != 0) {
        symc_null_ptr_chk(aad);
    }

    /* process aad. */
    crypto_drv_symc_gcm_dump("aad", aad, aad_len);
    ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, aad, aad_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");

    gcm_ctx->aad_len += aad_len;
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    int ret;
    drv_symc_gcm_ctx *gcm_ctx = (drv_symc_gcm_ctx *)ctx;
    uint8_t padding[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };
    uint32_t aad_padding_len;

    symc_null_ptr_chk(gcm_ctx);
    if (data_len != 0) {
        symc_null_ptr_chk(src);
        symc_null_ptr_chk(dst);
    }

    if (gcm_ctx->aad_is_finish == false) {
        aad_padding_len = CRYPTO_AES_BLOCK_SIZE_IN_BYTES - gcm_ctx->aad_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        crypto_drv_symc_gcm_dump("aad_padding", padding, aad_padding_len);
        if ((aad_padding_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) != 0) {
            ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, padding, aad_padding_len);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");
        }
        gcm_ctx->aad_is_finish = true;
    }
    if (gcm_ctx->is_encrypt == false) {
        ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, src, data_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");
    }

    ret = drv_cipher_symc_ctr_update(gcm_ctx->ctr_ctx, src, dst, data_len, gcm_ctx->gcm_iv, &gcm_ctx->nc_off,
        gcm_ctx->stream_block);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_ctr_update failed\n");

    if (gcm_ctx->is_encrypt == true) {
        ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, dst, data_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");
    }

    gcm_ctx->data_len += data_len;
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_finish(void *ctx, uint8_t *tag, uint32_t tag_len)
{
    int ret;
    drv_symc_gcm_ctx *gcm_ctx = (drv_symc_gcm_ctx *)ctx;
    unsigned char gcm_clen[CRYPTO_IV_LEN_IN_BYTES];
    uint8_t padding[CRYPTO_AES_BLOCK_SIZE_IN_BYTES] = { 0 };
    uint32_t padding_len;

    symc_null_ptr_chk(gcm_ctx);
    symc_null_ptr_chk(tag);
    /* Tag length for GCM should be 1~16.. */
    drv_err_chk_return(tag_len == 0 || tag_len > CRYPTO_AES_MAX_TAG_SIZE, SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM),
        "tag_len must be 1~16 for GCM.\n");

    if ((gcm_ctx->data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) == 0) {
        padding_len = 0;
    } else {
        padding_len = CRYPTO_AES_BLOCK_SIZE_IN_BYTES - gcm_ctx->data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    }

    crypto_drv_symc_gcm_dump("p_padding", padding, padding_len);
    ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, padding, padding_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");

    inner_gcm_format_clen(gcm_ctx->aad_len, gcm_ctx->data_len, gcm_clen);

    crypto_drv_symc_gcm_dump("gcm_clen", gcm_clen, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    ret = drv_symc_ghash_update(gcm_ctx->ghash_ctx, gcm_clen, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_update failed\n");

    ret = drv_symc_ghash_finish(gcm_ctx->ghash_ctx, gcm_ctx->gcm_ghash);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_symc_ghash_finish failed\n");
    crypto_drv_symc_gcm_dump("gcm_ghash", gcm_ctx->gcm_ghash, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);

    ret = hal_cipher_symc_crypt(gcm_ctx->symc_alg, CRYPTO_SYMC_WORK_MODE_GCM, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        crypto_get_phys_addr(gcm_ctx->gcm_ghash), crypto_get_phys_addr(gcm_ctx->tag), CRYPTO_AES_BLOCK_SIZE_IN_BYTES,
        gcm_ctx->j0, gcm_ctx->key, gcm_ctx->key_len, gcm_ctx->keyslot_handle, 0);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_crypt failed\n");

    ret = memcpy_s(tag, tag_len, gcm_ctx->tag, tag_len);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    crypto_drv_symc_gcm_dump("out_tag", tag, tag_len);

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_gcm_teardown(void *ctx)
{
    drv_symc_gcm_ctx *gcm_ctx = (drv_symc_gcm_ctx *)ctx;

    symc_null_ptr_chk(gcm_ctx);
    (void)drv_symc_ghash_destroy(gcm_ctx->ghash_ctx);
    (void)drv_cipher_symc_teardown(gcm_ctx->ctr_ctx);

    (void)memset_s(gcm_ctx, sizeof(drv_symc_gcm_ctx), 0, sizeof(drv_symc_gcm_ctx));
    crypto_free_coherent(gcm_ctx);
    return CRYPTO_SUCCESS;
}