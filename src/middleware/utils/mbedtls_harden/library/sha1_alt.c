/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha256 implemention.
 *
*/

#include "library/common.h"

#if defined(MBEDTLS_SHA1_C)
#include "mbedtls/sha1.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls_hash_func.h"
#include "security_hash.h"
#include "security_hash_pri.h"

#include "mbedtls_perf_utils.h"

#include <securec.h>

#if defined(MBEDTLS_SHA1_ALT)

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    MBEDTLS_PERF_START();
    (void)memset_s(ctx, sizeof(mbedtls_sha1_context), 0, sizeof(mbedtls_sha1_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_INIT);
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return;
    }
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    (void)memset_s(ctx, sizeof(mbedtls_sha1_context), 0, sizeof(mbedtls_sha1_context));
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FREE);
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (dst == NULL || src == NULL) {
        return;
    }
    if (dst->hash_handle == 0) {
        ret = uapi_drv_cipher_sha1_start(&dst->hash_handle);
        if (ret != ERRCODE_SUCC) {
            mbedtls_printf("uapi_drv_cipher_sha1_start failed, ret=0x%x\n", ret);
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
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_CLONE);
}

int mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA1_BAD_INPUT_DATA;
    }
    if (ctx->hash_handle != 0) {
        uapi_drv_cipher_sha_destroy(ctx->hash_handle);
        ctx->hash_handle = 0;
    }
    ret = uapi_drv_cipher_sha1_start(&(ctx->hash_handle));
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha1_starts failed, ret = 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_START);
    return 0;
}

int mbedtls_sha1_update(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA1_BAD_INPUT_DATA;
    }
    ret = uapi_drv_cipher_sha1_update(ctx->hash_handle, input, ilen);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha1_update failed, ret = 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END2(ilen);
    return 0;
}

#define MBEDTLS_SHA1_DIGEST_SIZE 20
int mbedtls_sha1_finish(mbedtls_sha1_context *ctx, unsigned char output[MBEDTLS_SHA1_DIGEST_SIZE])
{
    errcode_t ret;
    MBEDTLS_PERF_START();
    if (ctx == NULL) {
        return MBEDTLS_ERR_SHA1_BAD_INPUT_DATA;
    }
    ret = uapi_drv_cipher_sha1_finish(ctx->hash_handle, output);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_sha1_finish failed, ret = 0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    MBEDTLS_PERF_SHA_END1(MBEDTLS_PERF_SHA_TYPE_FINISH);
    return 0;
}

#endif /* MBEDTLS_SHA1_ALT */

#endif /* MBEDTLS_SHA1_C */