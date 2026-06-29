/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 * Create: 2023-05-10
*/

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#if defined(MBEDTLS_SHA256_ALT)
#include "mbedtls_harden_struct.h"
#endif /* MBEDTLS_SHA256_ALT */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha256_context
{
#if defined(MBEDTLS_SHA256_ALT)
    mbedtls_alt_hash_clone_ctx clone_ctx;
#elif defined(MBEDTLS_DIAL_SHA256_ALT)
    uint32_t hash_handle;
#endif /* MBEDTLS_DIAL_SHA256_ALT */
    unsigned int result_size;
}
mbedtls_sha256_context;

#ifdef __cplusplus
}
#endif

#endif /* sha256_alt.h */