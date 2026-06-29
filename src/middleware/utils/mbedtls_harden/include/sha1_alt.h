/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt sha1 header.
 *
*/

#ifndef SHA1_ALT_H
#define SHA1_ALT_H
#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha1_context {
    uintptr_t hash_handle;
} mbedtls_sha1_context;

#ifdef __cplusplus
}
#endif

#endif /* sha1_alt.h */