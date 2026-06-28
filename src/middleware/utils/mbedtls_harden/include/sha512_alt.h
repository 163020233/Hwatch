/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha512 header.
 *
*/

#ifndef SHA512_ALT_H
#define SHA512_ALT_H

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha512_context {
    uintptr_t hash_handle;
    int       is384;
} mbedtls_sha512_context;

#ifdef __cplusplus
}
#endif

#endif /* sha512_alt.h */