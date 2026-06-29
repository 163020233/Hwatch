/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha512 implemention.
 *
*/

#include "library/common.h"

#if defined(MBEDTLS_SHA512_C) || defined(MBEDTLS_SHA384_C)
#include "mbedtls/sha512.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls_hash_func.h"
#include "security_hash_pri.h"

#include "security_hash.h"

#include "mbedtls_perf_utils.h"
#include <securec.h>

#if defined(MBEDTLS_SHA512_ALT)

void mbedtls_sha512_init(mbedtls_sha512_context *ctx)
{
    MBEDTLS_PERF_START();
    (void)memset_s(ctx, sizeof(mbedtls_sha512_context), 0, sizeof(mbedtls_sha512_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_INIT);
}

void mbedtls_sha512_free(mbedtls_sha512_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    (void)memset_s(ctx, sizeof(mbedtls_sha512_context), 0, sizeof(mbedtls_sha512_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FREE);
}

void mbedtls_sha512_clone(mbedtls_sha512_context *dst, const mbedtls_sha512_context *src)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (dst == NULL || src == NULL) {
        return;
    }
    if (dst->hash_handle == 0) {
        ret = uapi_drv_cipher_sha512_start(&dst->hash_handle);
        if (ret != ERRCODE_SUCC) {
            mbedtls_printf("uapi_drv_cipher_sha512_start failed, ret=0x%x\n", ret);
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
    dst->is384 = src->is384;
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_CLONE);
}

int mbedtls_sha512_starts(mbedtls_sha512_context *ctx, int is384)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    /**
    * Input parameter validation based on compile-time configurations
    *
    * The is384 parameter determines which SHA-2 algorithm to use:
    * - 0: SHA-512 (512-bit output)
    * - 1: SHA-384 (384-bit output)
    *
    * This validation ensures that:
    * 1. The is384 value is valid according to enabled features
    * 2. Only supported algorithms are allowed based on build configuration
    * 3. Invalid combinations are rejected early to prevent runtime errors
    */

#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_SHA384_C)
    /*
     * Both SHA-384 and SHA-512 are enabled
     * Only values 0 and 1 are acceptable for is384 parameter
     */
    if (is384 != 0 && is384 != 1) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#elif defined(MBEDTLS_SHA512_C)
    /*
     * Only SHA-512 is enabled (SHA-384 disabled)
     * is384 must be 0, any other value is invalid
     */
    if (is384 != 0) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#else /* defined MBEDTLS_SHA384_C only */
    /*
     * Only SHA-384 is enabled (SHA-512 disabled)
     * is384 must be 1 (non-zero), value 0 is invalid
     */
    if (is384 == 0) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
#endif
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }

    ctx->is384 = is384;
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    if (is384 == 0) {
        ret = uapi_drv_cipher_sha512_start(&(ctx->hash_handle));
    } else {
        ret = uapi_drv_cipher_sha384_start(&(ctx->hash_handle));
    }
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha512_starts failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_START);
    return 0;
}

int mbedtls_sha512_update(mbedtls_sha512_context *ctx, const unsigned char *input, size_t ilen)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }

    if (ctx->is384 == 0) {
        ret = uapi_drv_cipher_sha512_update(ctx->hash_handle, input, ilen);
    } else {
        ret = uapi_drv_cipher_sha384_update(ctx->hash_handle, input, ilen);
    }

    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha512_update failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END2(ilen);
    return ret;
}

int mbedtls_sha512_finish(mbedtls_sha512_context *ctx, unsigned char *output)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA512_BAD_INPUT_DATA;
    }
    if (ctx->is384 == 0) {
        ret = uapi_drv_cipher_sha512_finish(ctx->hash_handle, output);
    } else {
        ret = uapi_drv_cipher_sha384_finish(ctx->hash_handle, output);
    }

    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha512_finish failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FINISH);
    return ret;
}

#endif /* MBEDTLS_sha512_ALT */

#endif /* MBEDTLS_sha512_C */