/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt cmac implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_CMAC_C) && defined(MBEDTLS_CMAC_ALT)

#include "mbedtls/cmac.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "drv_symc.h"
#include "securec.h"
#include "security_symc_pri.h"
#include "mbedtls_perf_utils.h"

#ifndef BITS_OF_BYTE
#define BITS_OF_BYTE    8
#endif

int mbedtls_cipher_cmac_starts(mbedtls_cipher_context_t *ctx,
                               const unsigned char *key, size_t keybits)
{
    errcode_t ret;
    mbedtls_cipher_type_t type;
    MBEDTLS_PERF_START();

    if (ctx == NULL || ctx->cipher_info == NULL || key == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    type = mbedtls_cipher_info_get_type(ctx->cipher_info);
    if (type != MBEDTLS_CIPHER_AES_128_ECB &&
        type != MBEDTLS_CIPHER_AES_192_ECB &&
        type != MBEDTLS_CIPHER_AES_256_ECB) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    if (ctx->cmac_ctx != 0) {
        (void)uapi_drv_cipher_cmac_ctx_deinit((uintptr_t)(ctx->cmac_ctx->mac_handle_buf),
            sizeof(ctx->cmac_ctx->mac_handle_buf));
        mbedtls_free(ctx->cmac_ctx);
        ctx->cmac_ctx = 0;
    }

    ctx->cmac_ctx = mbedtls_calloc(1, sizeof(mbedtls_cmac_context_t));
    if (ctx->cmac_ctx == NULL) {
        mbedtls_printf("mbedtls_cipher_cmac_starts malloc failed.\n");
        return MBEDTLS_ERR_CIPHER_ALLOC_FAILED;
    }

    ret = uapi_drv_cipher_cmac_ctx_init((uintptr_t)(ctx->cmac_ctx->mac_handle_buf),
                                        sizeof(ctx->cmac_ctx->mac_handle_buf),
                                        UAPI_DRV_CIPHER_SYMC_ALG_AES,
                                        key, keybits / BITS_OF_BYTE, UAPI_DRV_INVALID_KEY_SLOT);
    if (ret != ERRCODE_SUCC) {
        mbedtls_platform_zeroize(ctx->cmac_ctx, sizeof(mbedtls_cmac_context_t));
        mbedtls_free(ctx->cmac_ctx);
        ctx->cmac_ctx = 0;
        mbedtls_printf("mbedtls_cipher_cmac_starts start failed, ret is 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_CMAC_END1(MBEDTLS_PERF_CMAC_TYPE_STARTS);
    return 0;
}

int mbedtls_cipher_cmac_update(mbedtls_cipher_context_t *ctx,
                               const unsigned char *input, size_t ilen)
{
    errcode_t ret;
    MBEDTLS_PERF_START();

    if (ctx == NULL || ctx->cmac_ctx == 0) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    if (input == NULL && ilen != 0) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }
    ret = uapi_drv_cipher_cmac_update((uintptr_t)(ctx->cmac_ctx->mac_handle_buf), input, ilen);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_cipher_cmac_update failed, ret is 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_CMAC_END2(ilen);
    return ret;
}

int mbedtls_cipher_cmac_finish(mbedtls_cipher_context_t *ctx,
                               unsigned char *output)
{
    errcode_t ret;
    MBEDTLS_PERF_START();

    if (ctx == NULL || ctx->cmac_ctx == NULL || output == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }
    ret = uapi_drv_cipher_cmac_finish((uintptr_t)(ctx->cmac_ctx->mac_handle_buf), output);
    (void)uapi_drv_cipher_cmac_ctx_deinit((uintptr_t)(ctx->cmac_ctx->mac_handle_buf),
        sizeof(ctx->cmac_ctx->mac_handle_buf));
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_cipher_cmac_finish failed, ret is 0x%x\n", ret);
        ret = MBEDTLS_ERR_ERROR_GENERIC_ERROR;
        return ret;
    }

    MBEDTLS_PERF_CMAC_END1(MBEDTLS_PERF_CMAC_TYPE_FINISH);
    return 0;
}

int mbedtls_cipher_cmac_reset(mbedtls_cipher_context_t *ctx)
{
    drv_mac_ctx_t *mac_ctx;
    MBEDTLS_PERF_START();

    if (ctx == NULL || ctx->cipher_info == NULL || ctx->cmac_ctx == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    mac_ctx = (drv_mac_ctx_t *)(ctx->cmac_ctx->mac_handle_buf);

    mbedtls_platform_zeroize(mac_ctx->mac, sizeof(mac_ctx->mac));
    mbedtls_platform_zeroize(mac_ctx->buf, sizeof(mac_ctx->buf));
    mac_ctx->tail_len = 0;

    MBEDTLS_PERF_CMAC_END1(MBEDTLS_PERF_CMAC_TYPE_RESET);
    return 0;
}
int mbedtls_cipher_cmac(const mbedtls_cipher_info_t *cipher_info,
                        const unsigned char *key, size_t keylen,
                        const unsigned char *input, size_t ilen,
                        unsigned char *output)
{
    errcode_t ret;
    mbedtls_cipher_type_t type;
    MBEDTLS_PERF_START();

    if (cipher_info == NULL || key == NULL || output == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    if (input == NULL && ilen != 0) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }
    type = mbedtls_cipher_info_get_type(cipher_info);
    if (type != MBEDTLS_CIPHER_AES_128_ECB &&
        type != MBEDTLS_CIPHER_AES_192_ECB &&
        type != MBEDTLS_CIPHER_AES_256_ECB) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    ret = uapi_drv_cipher_cmac(UAPI_DRV_CIPHER_SYMC_ALG_AES, key, keylen / BITS_OF_BYTE,
        UAPI_DRV_INVALID_KEY_SLOT, input, ilen, output);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_cipher_cmac failed, ret is 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    MBEDTLS_PERF_CMAC_END2(ilen);
    return 0;
}

#if defined(MBEDTLS_AES_C)
int mbedtls_aes_cmac_prf_128(const unsigned char *key, size_t key_len,
                             const unsigned char *input, size_t in_len,
                             unsigned char output[16])
{
    errcode_t ret;
    const mbedtls_cipher_info_t *cipher_info;
    unsigned char zero_key[MBEDTLS_AES_BLOCK_SIZE];

    if (key == NULL || input == NULL || output == NULL) {
        return MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
    }

    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        ret = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
        goto exit;
    }

    if (key_len == MBEDTLS_AES_BLOCK_SIZE) {
        /* Use key as is */
        ret = uapi_drv_cipher_cmac(UAPI_DRV_CIPHER_SYMC_ALG_AES, key, key_len, UAPI_DRV_INVALID_KEY_SLOT,
            input, in_len, output);
    } else {
        memset_s(zero_key, MBEDTLS_AES_BLOCK_SIZE, 0, MBEDTLS_AES_BLOCK_SIZE);

        ret = uapi_drv_cipher_cmac(UAPI_DRV_CIPHER_SYMC_ALG_AES, zero_key, sizeof(zero_key), UAPI_DRV_INVALID_KEY_SLOT,
            key, key_len, zero_key);
        if (ret != ERRCODE_SUCC) {
            goto exit;
        }
        ret = uapi_drv_cipher_cmac(UAPI_DRV_CIPHER_SYMC_ALG_AES, zero_key, sizeof(zero_key), UAPI_DRV_INVALID_KEY_SLOT,
            input, in_len, output);
    }

exit:
    mbedtls_platform_zeroize(zero_key, sizeof(zero_key));
    return ret;
}
#endif /* MBEDTLS_AES_C */

#endif /* MBEDTLS_CMAC_ALT */