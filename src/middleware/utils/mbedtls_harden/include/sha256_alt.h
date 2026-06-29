/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha256 header.
 *
*/

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha256_context {
    uintptr_t hash_handle;
    int       is224;
} mbedtls_sha256_context;

#ifdef __cplusplus
}
#endif

#endif /* sha256_alt.h */