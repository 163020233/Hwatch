/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt ccm implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_CCM_ALT)

#include "mbedtls/ccm.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "securec.h"

#include "mbedtls_perf_utils.h"

void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    (void)memset_s(ctx, sizeof(mbedtls_ccm_context), 0, sizeof(mbedtls_ccm_context));
    ctx->keyslot_handle = UAPI_DRV_INVALID_KEY_SLOT;
    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_INIT);
}

int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
    int ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL || key == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (keybits != 128 && keybits != 192 && keybits != 256) {   // 128,192,256: key bits
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (cipher != MBEDTLS_CIPHER_ID_AES) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
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
            return MBEDTLS_ERR_CCM_BAD_INPUT;
        }
    }

    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_SETKEY);
    return 0;
}

void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    if (ctx->ccm_ctx != 0) {
        (void)uapi_drv_cipher_symc_ccm_teardown(ctx->ccm_ctx);
        ctx->ccm_ctx = 0;
    }
#if defined(MBEDTLS_ALT_KEYSLOT_SUPPORT)
    if (ctx->keyslot_handle != UAPI_DRV_INVALID_KEY_SLOT) {
        (void)uapi_drv_keyslot_teardown(ctx->keyslot_handle);
        ctx->keyslot_handle = UAPI_DRV_INVALID_KEY_SLOT;
    }
#endif
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_ccm_context));
    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_FREE);
}

int mbedtls_ccm_starts(mbedtls_ccm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len)
{
    int ret;
    bool is_encrypt;
    MBEDTLS_PERF_START();
    if (ctx == NULL || iv == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (ctx->ccm_ctx != 0) {
        uapi_drv_cipher_symc_ccm_teardown(ctx->ccm_ctx);
        ctx->ccm_ctx = 0;
    }

    if (iv_len < 7 || iv_len > 13) {    // 7: iv_min_len, 13: iv_max_len
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
    if (mode != MBEDTLS_CCM_ENCRYPT && mode != MBEDTLS_CCM_DECRYPT) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (mode == MBEDTLS_CCM_ENCRYPT) {
        is_encrypt = true;
    } else {
        is_encrypt = false;
    }

    ret = uapi_drv_cipher_symc_ccm_setup(&(ctx->ccm_ctx), UAPI_DRV_CIPHER_SYMC_ALG_AES,
        ctx->key, ctx->key_len, ctx->keyslot_handle,
        iv, iv_len, is_encrypt);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("ccm setup failed, ret is 0x%x\n", ret);
        return ret;
    }

    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_STARTS);
    return 0;
}

int mbedtls_ccm_set_lengths(mbedtls_ccm_context *ctx,
                            size_t total_ad_len,
                            size_t plaintext_len,
                            size_t tag_len)
{
    int ret;

    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0) { // 2, 16: tag_len check.
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (total_ad_len >= 0xFF00) {   // 0xFF00: max ad_len
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_ccm_set_lengths(ctx->ccm_ctx, total_ad_len,
        plaintext_len, tag_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("ccm set lengths failed, ret is 0x%x\n", ret);
        return ret;
    }

    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_SET_LENGTHS);
    return 0;
}

int mbedtls_ccm_update_ad(mbedtls_ccm_context *ctx,
                          const unsigned char *add,
                          size_t add_len)
{
    int ret;

    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_ccm_update_ad(ctx->ccm_ctx, add, add_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("ccm update ad failed, ret is 0x%x\n", ret);
        return ret;
    }

    MBEDTLS_PERF_CCM_END2(MBEDTLS_PERF_CCM_TYPE_UPDATE_AD, add_len);
    return 0;
}

int mbedtls_ccm_update(mbedtls_ccm_context *ctx,
                       const unsigned char *input, size_t input_len,
                       unsigned char *output, size_t output_size,
                       size_t *output_len)
{
    int ret;

    MBEDTLS_PERF_START();
    if (ctx == NULL || output_len == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
    if (output_size < input_len) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_ccm_update(ctx->ccm_ctx, input, output, input_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("ccm update failed, ret is 0x%x\n", ret);
        return ret;
    }

    *output_len = input_len;
    MBEDTLS_PERF_CCM_END2(MBEDTLS_PERF_CCM_TYPE_UPDATE, input_len);
    return 0;
}

int mbedtls_ccm_finish(mbedtls_ccm_context *ctx,
                       unsigned char *tag, size_t tag_len)
{
    int ret;

    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    ret = uapi_drv_cipher_symc_ccm_finish(ctx->ccm_ctx, tag, tag_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("ccm finish failed, ret is 0x%x\n", ret);
    } else {
        ret = 0;
    }

    (void)uapi_drv_cipher_symc_ccm_teardown(ctx->ccm_ctx);
    ctx->ccm_ctx = 0;
    MBEDTLS_PERF_CCM_END1(MBEDTLS_PERF_CCM_TYPE_FINISH);
    return ret;
}

#endif