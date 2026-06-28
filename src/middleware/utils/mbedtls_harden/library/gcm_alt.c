/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt gcm implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_GCM_ALT)

#include "mbedtls/gcm.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/constant_time.h"
#include "securec.h"

#include "mbedtls_perf_utils.h"

void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    (void)memset_s(ctx, sizeof(mbedtls_gcm_context), 0, sizeof(mbedtls_gcm_context));
    ctx->keyslot_handle = UAPI_DRV_INVALID_KEY_SLOT;
    MBEDTLS_PERF_GCM_END1(MBEDTLS_PERF_GCM_TYPE_INIT);
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
    mbedtls_cipher_id_t cipher,
    const unsigned char *key,
    unsigned int keybits)
{
    int ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL || key == NULL) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (keybits != 128 && keybits != 192 && keybits != 256) {   // 128,192,256: key bits
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (cipher != MBEDTLS_CIPHER_ID_AES) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    ctx->key_len = keybits / 8; // 8: bits in byte

#if defined(MBEDTLS_ALT_KEYSLOT_SUPPORT)
    ret = uapi_drv_keyslot_setup(&(ctx->keyslot_handle), UAPI_DRV_KEYSLOT_TYPE_MCIPHER);
    if (ret == ERRCODE_SUCC) {  /* use keyslot. */
        ret = uapi_drv_keyslot_set_clear_key(ctx->keyslot_handle, UAPI_DRV_KLAD_ENGINE_AES,
            key, ctx->key_len);
        if (ret != ERRCODE_SUCC) {
            (void)uapi_drv_keyslot_teardown(ctx->keyslot_handle);
            ctx->keyslot_handle = UAPI_DRV_INVALID_KEY_SLOT;
        }
    }
#endif
    if (ctx->keyslot_handle == UAPI_DRV_INVALID_KEY_SLOT) {
        ret = memcpy_s(ctx->key, sizeof(ctx->key), key, ctx->key_len);
        if (ret != EOK) {
            return MBEDTLS_ERR_GCM_BAD_INPUT;
        }
    }

    MBEDTLS_PERF_GCM_END1(MBEDTLS_PERF_GCM_TYPE_SETKEY);
    return 0;
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
    int mode,
    const unsigned char *iv, size_t iv_len)
{
    int ret;
    bool is_encrypt;
    MBEDTLS_PERF_START();
    if (ctx == NULL || iv == NULL) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (ctx->gcm_ctx != 0) {
        uapi_drv_cipher_symc_gcm_teardown(ctx->gcm_ctx);
        ctx->gcm_ctx = 0;
    }
    /* IV is limited to 2^64 bits, so 2^61 bytes */
    /* IV is not allowed to be zero length */
    if (iv_len == 0 || ((uint64_t)iv_len >> 61) != 0) {  // 0, 61: iv len limit
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    if (mode != MBEDTLS_GCM_ENCRYPT && mode != MBEDTLS_GCM_DECRYPT) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (mode == MBEDTLS_GCM_ENCRYPT) {
        is_encrypt = true;
    } else {
        is_encrypt = false;
    }

    ret = uapi_drv_cipher_symc_gcm_setup(&(ctx->gcm_ctx), UAPI_DRV_CIPHER_SYMC_ALG_AES,
        ctx->key, ctx->key_len, ctx->keyslot_handle, iv, iv_len, is_encrypt);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("gcm setup failed, ret is 0x%x\n", ret);
        return ret;
    }

    MBEDTLS_PERF_GCM_END1(MBEDTLS_PERF_GCM_TYPE_STARTS);
    return 0;
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx,
                          const unsigned char *add, size_t add_len)
{
    int ret;

    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_gcm_update_ad(ctx->gcm_ctx, add, add_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("gcm update ad failed, ret is 0x%x\n", ret);
        return ret;
    }

    MBEDTLS_PERF_GCM_END2(MBEDTLS_PERF_GCM_TYPE_UPDATE_AD, add_len);
    return 0;
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                       const unsigned char *input, size_t input_length,
                       unsigned char *output, size_t output_size,
                       size_t *output_length)
{
    int ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL || output_length == NULL) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    if (output_size < input_length) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_gcm_update(ctx->gcm_ctx, input, output, input_length);
    if (ret != 0) {
        mbedtls_printf("gcm update failed, ret is 0x%x\n", ret);
        return ret;
    }

    *output_length = input_length;
    MBEDTLS_PERF_GCM_END2(MBEDTLS_PERF_GCM_TYPE_UPDATE, input_length);
    return 0;
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                       unsigned char *output, size_t output_size,
                       size_t *output_length,
                       unsigned char *tag, size_t tag_len)
{
    int ret;

    MBEDTLS_PERF_START();
    (void)output;
    (void)output_size;

    if (ctx == NULL || output_length == NULL) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (tag_len > 16 || tag_len < 4) {  // 4,16: tag len limit
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_gcm_finish(ctx->gcm_ctx, tag, tag_len);
    if (ret != 0) {
        mbedtls_printf("gcm finish failed, ret is 0x%x\n", ret);
        goto exit_teardown;
    }
    *output_length = 0;

    ret = 0;
exit_teardown:
    (void)uapi_drv_cipher_symc_gcm_teardown(ctx->gcm_ctx);
    ctx->gcm_ctx = 0;
    MBEDTLS_PERF_GCM_END1(MBEDTLS_PERF_GCM_TYPE_FINISH);
    return ret;
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                              int mode,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t tag_len,
                              unsigned char *tag)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t olen;

    if ((ret = mbedtls_gcm_starts(ctx, mode, iv, iv_len)) != 0) {
        return ret;
    }

    if ((ret = mbedtls_gcm_update_ad(ctx, add, add_len)) != 0) {
        return ret;
    }

    if ((ret = mbedtls_gcm_update(ctx, input, length,
                                  output, length, &olen)) != 0) {
        return ret;
    }

    if ((ret = mbedtls_gcm_finish(ctx, NULL, 0, &olen, tag, tag_len)) != 0) {
        return ret;
    }

    return 0;
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                             size_t length,
                             const unsigned char *iv,
                             size_t iv_len,
                             const unsigned char *add,
                             size_t add_len,
                             const unsigned char *tag,
                             size_t tag_len,
                             const unsigned char *input,
                             unsigned char *output)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char check_tag[16];
    int diff;

    if ((ret = mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_DECRYPT, length,
                                         iv, iv_len, add, add_len,
                                         input, output, tag_len, check_tag)) != 0) {
        return ret;
    }

    /* Check tag in "constant-time" */
    diff = mbedtls_ct_memcmp(tag, check_tag, tag_len);
    if (diff != 0) {
        mbedtls_platform_zeroize(output, length);
        return MBEDTLS_ERR_GCM_AUTH_FAILED;
    }

    return 0;
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    if (ctx->gcm_ctx != 0) {
        (void)uapi_drv_cipher_symc_gcm_teardown(ctx->gcm_ctx);
        ctx->gcm_ctx = 0;
    }
#if defined(MBEDTLS_ALT_KEYSLOT_SUPPORT)
    if (ctx->keyslot_handle != UAPI_DRV_INVALID_KEY_SLOT) {
        (void)uapi_drv_keyslot_teardown(ctx->keyslot_handle);
        ctx->keyslot_handle = UAPI_DRV_INVALID_KEY_SLOT;
    }
#endif
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_gcm_context));
    MBEDTLS_PERF_GCM_END1(MBEDTLS_PERF_GCM_TYPE_FREE);
}

#endif /* MBEDTLS_GCM_ALT */