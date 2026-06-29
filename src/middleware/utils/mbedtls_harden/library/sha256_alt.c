/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha256 implemention.
 *
*/

#include "library/common.h"

#if defined(MBEDTLS_SHA256_C) || defined(MBEDTLS_SHA224_C)
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls_hash_func.h"
#include "security_hash.h"
#include "security_hash_pri.h"

#include "mbedtls_perf_utils.h"

#include <securec.h>

#if defined(MBEDTLS_SHA256_ALT)

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    MBEDTLS_PERF_START();
    (void)memset_s(ctx, sizeof(mbedtls_sha256_context), 0, sizeof(mbedtls_sha256_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_INIT);
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    (void)memset_s(ctx, sizeof(mbedtls_sha256_context), 0, sizeof(mbedtls_sha256_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FREE);
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (dst == NULL || src == NULL) {
        return;
    }
    if (dst->hash_handle == 0) {
        ret = uapi_drv_cipher_sha256_start(&dst->hash_handle);
        if (ret != ERRCODE_SUCC) {
            mbedtls_printf("uapi_drv_cipher_sha256_start failed, ret=0x%x\n", ret);
            return;
        }
    }
    ret = uapi_drv_cipher_hash_compute_clone(dst->hash_handle, src->hash_handle);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("uapi_drv_cipher_hash_compute_clone failed, ret=0x%x\n", ret);
        (void)uapi_drv_cipher_sha_destroy(dst->hash_handle);
        dst->hash_handle = 0;
        return;
    }

    dst->is224 = src->is224;
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_CLONE);
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    /*
     * Input parameter validation based on compile-time configurations
     *
     * The is224 parameter determines which SHA-2 algorithm to use:
     * - 0: SHA-256 (256-bit output)
     * - 1: SHA-224 (224-bit output)
     *
     * This validation ensures that:
     * 1. The is224 value is valid according to enabled features
     * 2. Only supported algorithms are allowed based on build configuration
     * 3. Invalid combinations are rejected early to prevent runtime errors
     */
#if defined(MBEDTLS_SHA224_C) && defined(MBEDTLS_SHA256_C)
    if (is224 != 0 && is224 != 1) {
    /*
     * Both SHA-224 and SHA-256 are enabled
     * Only values 0 and 1 are acceptable for is224 parameter
     */
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#elif defined(MBEDTLS_SHA256_C)
    /*
     * Only SHA-256 is enabled (SHA-224 disabled)
     * is224 must be 0, any other value is invalid
     */
    if (is224 != 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#else /* defined MBEDTLS_SHA224_C only */
    /*
     * Only SHA-224 is enabled (SHA-256 disabled)
     * is224 must be 1 (non-zero), value 0 is invalid
     */
    if (is224 == 0) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
#endif
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }

    ctx->is224 = is224;
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    if (is224 == 0) {
        ret = uapi_drv_cipher_sha256_start(&(ctx->hash_handle));
    } else {
        ret = uapi_drv_cipher_sha224_start(&(ctx->hash_handle));
    }

    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha256_starts failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_START);
    return 0;
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }
    if (ctx->is224 == 0) {
        ret = uapi_drv_cipher_sha256_update(ctx->hash_handle, input, ilen);
    } else {
        ret = uapi_drv_cipher_sha224_update(ctx->hash_handle, input, ilen);
    }

    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha256_update failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END2(ilen);
    return 0;
}

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA256_BAD_INPUT_DATA;
    }

    if (ctx->is224 == 0) {
        ret = uapi_drv_cipher_sha256_finish(ctx->hash_handle, output);
    } else {
        ret = uapi_drv_cipher_sha224_finish(ctx->hash_handle, output);
    }

    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha256_finish failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FINISH);
    return 0;
}

#endif /* MBEDTLS_SHA256_ALT */

#endif /* MBEDTLS_SHA256_C */