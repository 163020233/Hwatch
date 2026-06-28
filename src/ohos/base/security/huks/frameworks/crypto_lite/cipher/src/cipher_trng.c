/*
 * Copyright (c) 2026-2030 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ohos begin
// secure random
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "cipher.h"
#include "cipher_log.h"
#include "securec.h"

#define RANDOM_ERROR_SUCCESS 0
#define RANDOM_ERROR_INVALID_PARAM -1
#define RANDOM_ERROR_NOT_INIT -2
#define RANDOM_ERROR_GENERATION_FAILED -3
#define RANDOM_ERROR_MEMORY -4

// 每个线程或请求的独立上下文
typedef struct {
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    int32_t initialized;
} SecureRandomContext;

// 创建并初始化一个新的随机数上下文（线程安全基础）
SecureRandomHandle SecureRandomCreate(void)
{
    SecureRandomContext *ctx = (SecureRandomContext *)malloc(sizeof(SecureRandomContext));
    if (ctx == NULL) {
        return NULL;
    }

    mbedtls_entropy_init(&ctx->entropy);
    mbedtls_ctr_drbg_init(&ctx->ctr_drbg);

    const char *pers = "openharmony_secure_random";
    int32_t ret = mbedtls_ctr_drbg_seed(
        &ctx->ctr_drbg, mbedtls_entropy_func, &ctx->entropy, (const unsigned char *)pers, strlen(pers));
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctx->ctr_drbg);
        mbedtls_entropy_free(&ctx->entropy);
        free(ctx);
        ctx = NULL;
        return NULL;
    }

    ctx->initialized = 1;
    return (SecureRandomHandle)ctx;
}

// 生成随机字节
int32_t SecureRandomGenerateBytes(SecureRandomHandle handle, unsigned char *output, size_t outputLen)
{
    if (handle == NULL || output == NULL || outputLen == 0 || outputLen > RANDOM_MAX_BYTES) {
        return RANDOM_ERROR_INVALID_PARAM;
    }

    SecureRandomContext *ctx = (SecureRandomContext *)handle;
    if (ctx->initialized != 1) {
        return RANDOM_ERROR_NOT_INIT;
    }

    return mbedtls_ctr_drbg_random(&ctx->ctr_drbg, output, outputLen);
}

// 释放随机数上下文
void SecureRandomDestroy(SecureRandomHandle handle)
{
    if (handle != NULL) {
        SecureRandomContext *ctx = (SecureRandomContext *)handle;
        mbedtls_ctr_drbg_free(&ctx->ctr_drbg);
        mbedtls_entropy_free(&ctx->entropy);
        free(ctx);
        ctx = NULL;
    }
}

// 生成指定范围的随机数
int32_t SecureRandomGenerateNumber(SecureRandomHandle handle, int32_t min, int32_t max, int32_t *result)
{
    if (handle == NULL || result == NULL || min >= max) {
        return RANDOM_ERROR_INVALID_PARAM;
    }

    unsigned char randomBytes[4];
    int32_t ret = SecureRandomGenerateBytes(handle, randomBytes, sizeof(randomBytes));
    if (ret != 0) {
        return ret;
    }

    // 将4字节转换为32位整数
    uint32_t randomValue = 0;
    for (int32_t i = 0; i < 4; i++) {
        randomValue = (randomValue << 8) | randomBytes[i];
    }

    // 映射到指定范围
    int32_t range = max - min + 1;
    *result = min + (randomValue % range);

    return RANDOM_ERROR_SUCCESS;
}
// ohos end