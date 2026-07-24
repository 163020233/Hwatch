/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: symmetric algorithm implementation.
 *
 * Create: 2023-05-26
*/

#include "drv_symc.h"
#include "hal_symc.h"
#include "crypto_drv_common.h"

#include "hal_keyslot.h"

#define FILE_ID FILE_ID_DRV_CODE_SYMC_DRV_SYMC

#define SYMC_COMPAT_ERRNO(err_code)     DRV_COMPAT_ERRNO(ERROR_MODULE_SYMC, err_code)
#define symc_null_ptr_chk(ptr)   \
    drv_err_chk_return((ptr) == TD_NULL, SYMC_COMPAT_ERRNO(ERROR_PARAM_IS_NULL), #ptr" is NULL\n")

#define DRV_SYMC_BUF_LEN    256

static const uint32_t g_symc_alg_list[] = {
#if defined(CONFIG_SYMC_SUPPORT_ECB)
    CRYPTO_SYMC_AES_ECB_ALG, CRYPTO_SYMC_SM4_ECB_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CBC)
    CRYPTO_SYMC_AES_CBC_ALG, CRYPTO_SYMC_SM4_CBC_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CTR)
    CRYPTO_SYMC_AES_CTR_ALG, CRYPTO_SYMC_SM4_CTR_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_OFB)
    CRYPTO_SYMC_AES_OFB_ALG, CRYPTO_SYMC_SM4_OFB_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CFB1)
    CRYPTO_SYMC_AES_CFB1_ALG, CRYPTO_SYMC_SM4_CFB1_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CFB8)
    CRYPTO_SYMC_AES_CFB8_ALG, CRYPTO_SYMC_SM4_CFB8_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CFB128)
    CRYPTO_SYMC_AES_CFB128_ALG, CRYPTO_SYMC_SM4_CFB128_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_CCM)
    CRYPTO_SYMC_AES_CCM_ALG, CRYPTO_SYMC_SM4_CCM_ALG,
#endif
#if defined(CONFIG_SYMC_SUPPORT_GCM)
    CRYPTO_SYMC_AES_GCM_ALG, CRYPTO_SYMC_SM4_GCM_ALG,
#endif
};

static int32_t inner_symc_alg_chk(uint32_t alg)
{
    uint32_t i;
    for (i = 0; i < crypto_array_size(g_symc_alg_list); i++) {
        if (g_symc_alg_list[i] == alg) {
            return CRYPTO_SUCCESS;
        }
    }
    drv_set_err(ERROR_INVALID_PARAM, "direct error return");
    return ERROR_INVALID_PARAM;
}

int drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    int ret;
    unsigned char *aligned_buf = NULL;
    unsigned long long src_phys_addr = crypto_get_phys_addr(src);
    unsigned long long dst_phys_addr = crypto_get_phys_addr(dst);
    unsigned int left = data_len;
    unsigned int processed_len = 0;
    unsigned int processing_len;
    unsigned int is_decrypt = !is_encrypt;
    symc_null_ptr_chk(src);
    symc_null_ptr_chk(dst);
    if (crypto_get_symc_work_mode(alg) != CRYPTO_SYMC_WORK_MODE_ECB) {
        symc_null_ptr_chk(iv);
    }
    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    ret = inner_symc_alg_chk(alg);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid alg\n");

#if defined(CONFIG_SPACC_NON_ALIGNED_ADDR_SUPPORT)
    crypto_unused(aligned_buf);
    crypto_unused(left);
    crypto_unused(processed_len);
    crypto_unused(processing_len);

    ret = hal_cipher_symc_crypt(crypto_get_symc_alg(alg), crypto_get_symc_work_mode(alg),
        crypto_get_symc_bit_width(alg), src_phys_addr, dst_phys_addr,
        data_len, iv, key, key_len, keyslot_handle, is_decrypt);
    return ret;
#else
    /* process aligned address. */
    if (crypto_addr_align_check(src_phys_addr) == TD_TRUE &&
        crypto_addr_align_check(dst_phys_addr) == TD_TRUE) {
        return hal_cipher_symc_crypt(crypto_get_symc_alg(alg), crypto_get_symc_work_mode(alg),
            crypto_get_symc_bit_width(alg), src_phys_addr, dst_phys_addr,
            data_len, iv, key, key_len, keyslot_handle, is_decrypt);
    }
    /* process unaligned address. */
    aligned_buf = crypto_malloc_coherent(DRV_SYMC_BUF_LEN);
    drv_err_chk_return(aligned_buf == NULL, ERROR_MALLOC, "malloc failed\n");

    src_phys_addr = crypto_get_phys_addr(aligned_buf);
    dst_phys_addr = src_phys_addr;

    while (left > 0) {
        processing_len = crypto_min(left, DRV_SYMC_BUF_LEN);
        ret = memcpy_s(aligned_buf, DRV_SYMC_BUF_LEN, src + processed_len, processing_len);
        drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy failed\n");

        ret = hal_cipher_symc_crypt(crypto_get_symc_alg(alg), crypto_get_symc_work_mode(alg),
            crypto_get_symc_bit_width(alg), src_phys_addr, dst_phys_addr, processing_len,
            iv, key, key_len, keyslot_handle, is_decrypt);
        drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_cipher_symc_crypt failed\n");

        ret = memcpy_s(dst + processed_len, data_len - processed_len, aligned_buf, processing_len);
        drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free, ERROR_MEMCPY_S, "memcpy failed\n");

        left -= processing_len;
        processed_len += processing_len;
    }

    ret = CRYPTO_SUCCESS;
exit_free:
    (void)memset_s(aligned_buf, DRV_SYMC_BUF_LEN, 0, DRV_SYMC_BUF_LEN);
    crypto_free_coherent(aligned_buf);
    return ret;
#endif
}

typedef struct {
    uint8_t stream_block[16];   // for ctr/cfb/ofb
    uint8_t key[CRYPTO_256_KEY_LEN];
    uint32_t keyslot_handle;
    unsigned int key_len;
    uint32_t alg;
    bool is_encrypt;
} drv_symc_simple_ctx;

int32_t drv_cipher_symc_setup(void **ctx, uint32_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = NULL;

    symc_null_ptr_chk(ctx);
    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        symc_null_ptr_chk(key);
    }
    /* key_len check. */
    drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        SYMC_COMPAT_ERRNO(ERROR_INVALID_PARAM), "key_len is invalid\n");
    ret = inner_symc_alg_chk(alg);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid alg\n");

    symc_ctx = crypto_malloc_coherent(sizeof(drv_symc_simple_ctx));
    drv_err_chk_return(symc_ctx == NULL, ERROR_MALLOC, "malloc failed\n");
    (void)memset_s(symc_ctx, sizeof(drv_symc_simple_ctx), 0, sizeof(drv_symc_simple_ctx));

    if (keyslot_handle == CRYPTO_INVALID_KEYSLOT_HANDLE) {
        ret = memcpy_s(symc_ctx->key, sizeof(symc_ctx->key), key, key_len);
        drv_err_chk_goto_with_ret(ret, ret != EOK, error_free, ERROR_MEMCPY_S, "memcpy_s failed\n");
    }

    symc_ctx->keyslot_handle = keyslot_handle;
    symc_ctx->is_encrypt = is_encrypt;
    symc_ctx->key_len = key_len;
    symc_ctx->alg = alg;

    *ctx = symc_ctx;
    return CRYPTO_SUCCESS;

error_free:
    (void)memset_s(symc_ctx, sizeof(drv_symc_simple_ctx), 0, sizeof(drv_symc_simple_ctx));
    crypto_free_coherent(symc_ctx);
    *ctx = NULL;
    return ret;
}

int32_t drv_cipher_symc_ecb_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len)
{
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(symc_ctx);
    drv_err_chk_return((data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) != 0, ERROR_SYMC_LEN_NOT_ALIGNED);

    return drv_cipher_symc_crypt(symc_ctx->alg, src, dst, data_len, NULL, symc_ctx->key, symc_ctx->key_len,
        symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
}

int32_t drv_cipher_symc_cbc_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len, uint8_t iv[16])
{
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(symc_ctx);
    drv_err_chk_return((data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) != 0, ERROR_SYMC_LEN_NOT_ALIGNED);

    return drv_cipher_symc_crypt(symc_ctx->alg, src, dst, data_len, iv, symc_ctx->key, symc_ctx->key_len,
        symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
}

int32_t drv_cipher_symc_ctr_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t nonce_counter[16], uint8_t *nc_off, uint8_t stream_block[16])
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;
    unsigned int left = data_len;
    unsigned int processed_len = 0;
    unsigned int processing_len;
    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }

    symc_null_ptr_chk(symc_ctx);
    symc_null_ptr_chk(nc_off);
    symc_null_ptr_chk(stream_block);
    symc_null_ptr_chk(src);
    symc_null_ptr_chk(dst);

#if defined(CONFIG_DRV_SYMC_CTR_DEBUG_ENABLE)
    crypto_dump_data("src", src, data_len);
    crypto_dump_data("nonce_counter", nonce_counter, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_dump_data("stream_block", stream_block, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_print("nc_off is %u\n", *nc_off);
#endif
    /* process last block. */
    if (*nc_off != 0) {
        for (; *nc_off < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && left > 0; (*nc_off)++, left--) {
            dst[processed_len] = src[processed_len] ^ stream_block[*nc_off];
            processed_len++;
        }
        *nc_off = *nc_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    }

    /* process aligned data. */
    processing_len = left - left % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    ret = drv_cipher_symc_crypt(symc_ctx->alg, src + processed_len, dst + processed_len, processing_len,
        nonce_counter, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");
    left -= processing_len;
    processed_len += processing_len;

    /* process tail. */
    if (left != 0) {
        (void)memset_s(symc_ctx->stream_block, sizeof(symc_ctx->stream_block), 0, sizeof(symc_ctx->stream_block));
        ret = drv_cipher_symc_crypt(symc_ctx->alg, symc_ctx->stream_block, symc_ctx->stream_block,
            sizeof(symc_ctx->stream_block), nonce_counter, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle,
            symc_ctx->is_encrypt);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");

        (void)memcpy_s(stream_block, CRYPTO_AES_BLOCK_SIZE_IN_BYTES,
            symc_ctx->stream_block, sizeof(symc_ctx->stream_block));

        for (; *nc_off < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && left > 0; (*nc_off)++, left--) {
            dst[processed_len] = src[processed_len] ^ stream_block[*nc_off];
            processed_len++;
        }
        *nc_off = *nc_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        (void)memset_s(symc_ctx->stream_block, sizeof(symc_ctx->stream_block), 0, sizeof(symc_ctx->stream_block));
    }
#if defined(CONFIG_DRV_SYMC_CTR_DEBUG_ENABLE)
    crypto_dump_data("dst", dst, data_len);
    crypto_dump_data("nonce_counter after", nonce_counter, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_dump_data("stream_block after", stream_block, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
    crypto_print("nc_off after is %u\n", *nc_off);
#endif
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_ofb_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;
    unsigned int left = data_len;
    unsigned int processed_len = 0;
    unsigned int processing_len;
    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }

    symc_null_ptr_chk(symc_ctx);
    symc_null_ptr_chk(iv);
    symc_null_ptr_chk(iv_off);
    symc_null_ptr_chk(src);
    symc_null_ptr_chk(dst);

#if defined(CONFIG_DRV_SYMC_OFB_DEBUG_ENABLE)
    crypto_dump_data("src", src, data_len);
    crypto_dump_data("iv", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off is %u\n", *iv_off);
#endif
    if (*iv_off != 0) {
        for (; *iv_off < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && left > 0; (*iv_off)++, left--) {
            dst[processed_len] = src[processed_len] ^ iv[*iv_off];
            processed_len++;
        }
        *iv_off = *iv_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    }

    if (left != 0) {
        processing_len = left;
        ret = drv_cipher_symc_crypt(symc_ctx->alg, src + processed_len, dst + processed_len, processing_len,
            iv, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");
    
        left -= processing_len;
        processed_len += processing_len;
    
        *iv_off = processing_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
    }

#if defined(CONFIG_DRV_SYMC_OFB_DEBUG_ENABLE)
    crypto_dump_data("dst", dst, data_len);
    crypto_dump_data("iv after", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off after is %u\n", *iv_off);
#endif
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_cfb128_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;
    unsigned int left = data_len;
    unsigned int processed_len = 0;
    unsigned int processing_len;
    if (data_len == 0) {
        return CRYPTO_SUCCESS;
    }

    symc_null_ptr_chk(symc_ctx);
    symc_null_ptr_chk(iv);
    symc_null_ptr_chk(iv_off);
    symc_null_ptr_chk(src);
    symc_null_ptr_chk(dst);

#if defined(CONFIG_DRV_SYMC_CFB128_DEBUG_ENABLE)
    crypto_dump_data("src", src, data_len);
    crypto_dump_data("iv", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off is %u\n", *iv_off);
#endif

    if (symc_ctx->is_encrypt) {
        if (*iv_off != 0) {
            for (; *iv_off < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && left > 0; (*iv_off)++, left--, processed_len++) {
                dst[processed_len] = src[processed_len] ^ iv[*iv_off];
                iv[*iv_off] = dst[processed_len];
            }
            *iv_off = *iv_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
        if (left != 0) {
            ret = drv_cipher_symc_crypt(symc_ctx->alg, src + processed_len, dst + processed_len, left,
                iv, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");
            *iv_off = left % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
    } else {
        /* Decrypt. */
        if (*iv_off != 0) {
            for (; *iv_off < CRYPTO_AES_BLOCK_SIZE_IN_BYTES && left > 0; (*iv_off)++, left--, processed_len++) {
                dst[processed_len] = src[processed_len] ^ iv[*iv_off];
                iv[*iv_off] = src[processed_len];
            }
            *iv_off = *iv_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
        if (left == 0) {
            return CRYPTO_SUCCESS;
        }

        /* processed aligned length. */
        (void)memcpy_s(symc_ctx->stream_block, sizeof(symc_ctx->stream_block), iv, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
        processing_len = left - left % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        if (processing_len != 0) {
            ret = drv_cipher_symc_crypt(symc_ctx->alg, src + processed_len, dst + processed_len, processing_len,
                iv, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");

            left -= processing_len;
            processed_len += processing_len;
            (void)memcpy_s(symc_ctx->stream_block, sizeof(symc_ctx->stream_block),
                &src[processed_len - CRYPTO_AES_BLOCK_SIZE_IN_BYTES], CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
        }
        if (left != 0) {
            /* ECB Mode. */
            ret = drv_cipher_symc_crypt(
                crypto_compat_symc_alg(crypto_get_symc_alg(symc_ctx->alg), CRYPTO_SYMC_WORK_MODE_ECB,
                CRYPTO_SYMC_BIT_WIDTH_128BIT), symc_ctx->stream_block, symc_ctx->stream_block,
                CRYPTO_AES_BLOCK_SIZE_IN_BYTES, NULL, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, true);
            drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");

            (void)memcpy_s(iv, CRYPTO_AES_BLOCK_SIZE_IN_BYTES, symc_ctx->stream_block, CRYPTO_AES_BLOCK_SIZE_IN_BYTES);
            for (; *iv_off < left; (*iv_off)++) {
                dst[processed_len + *iv_off] = src[processed_len + *iv_off] ^ iv[*iv_off];
                iv[*iv_off] = src[processed_len + *iv_off];
            }
            *iv_off = *iv_off % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
    }

#if defined(CONFIG_DRV_SYMC_CFB128_DEBUG_ENABLE)
    crypto_dump_data("dst", dst, data_len);
    crypto_dump_data("iv after", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off after is %u\n", *iv_off);
#endif
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_cfb8_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(symc_ctx);
    symc_null_ptr_chk(iv);
    symc_null_ptr_chk(iv_off);

#if defined(CONFIG_DRV_SYMC_CFB8_DEBUG_ENABLE)
    crypto_dump_data("src", src, data_len);
    crypto_dump_data("iv", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off is %u\n", *iv_off);
#endif
    ret = drv_cipher_symc_crypt(symc_ctx->alg, src, dst, data_len,
        iv, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");

    *iv_off = (*iv_off + data_len) % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
#if defined(CONFIG_DRV_SYMC_CFB8_DEBUG_ENABLE)
    crypto_dump_data("dst", dst, data_len);
    crypto_dump_data("iv after", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off after is %u\n", *iv_off);
#endif
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_cfb1_update(void *ctx, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], uint8_t *iv_off)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(symc_ctx);
    symc_null_ptr_chk(iv);
    symc_null_ptr_chk(iv_off);

#if defined(CONFIG_DRV_SYMC_CFB1_DEBUG_ENABLE)
    crypto_dump_data("src", src, data_len);
    crypto_dump_data("iv", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off is %u\n", *iv_off);
#endif
    ret = drv_cipher_symc_crypt(symc_ctx->alg, src, dst, data_len,
        iv, symc_ctx->key, symc_ctx->key_len, symc_ctx->keyslot_handle, symc_ctx->is_encrypt);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "drv_cipher_symc_crypt failed\n");

    *iv_off = (*iv_off + data_len) % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
#if defined(CONFIG_DRV_SYMC_CFB1_DEBUG_ENABLE)
    crypto_dump_data("dst", dst, data_len);
    crypto_dump_data("iv after", iv, CRYPTO_IV_LEN_IN_BYTES);
    crypto_print("iv_off after is %u\n", *iv_off);
#endif
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_teardown(void *ctx)
{
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(symc_ctx);
    (void)memset_s(symc_ctx, sizeof(drv_symc_simple_ctx), 0, sizeof(drv_symc_simple_ctx));
    crypto_free_coherent(symc_ctx);
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_set_key(void *ctx, const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;
    symc_null_ptr_chk(ctx);
    symc_null_ptr_chk(key);
    drv_err_chk_return(key_len != CRYPTO_128_KEY_LEN && key_len != CRYPTO_192_KEY_LEN && key_len != CRYPTO_256_KEY_LEN,
        ERROR_INVALID_PARAM, "key_len is invalid\n");

    ret = memcpy_s(symc_ctx->key, sizeof(symc_ctx->key), key, key_len);
    drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");

    symc_ctx->keyslot_handle = keyslot_handle;
    symc_ctx->key_len = key_len;

    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_set_crypt_mode(void *ctx, bool is_encrypt)
{
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;
    symc_null_ptr_chk(ctx);
    symc_ctx->is_encrypt = is_encrypt;
    return CRYPTO_SUCCESS;
}

int32_t drv_cipher_symc_set_alg(void *ctx, uint32_t alg)
{
    int ret;
    drv_symc_simple_ctx *symc_ctx = (drv_symc_simple_ctx *)ctx;

    symc_null_ptr_chk(ctx);
    ret = inner_symc_alg_chk(alg);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "invalid alg\n");
    symc_ctx->alg = alg;
    return CRYPTO_SUCCESS;
}