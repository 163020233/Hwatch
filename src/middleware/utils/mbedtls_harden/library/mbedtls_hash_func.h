/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
*/

#ifndef MBEDTLS_HASH_FUNC_H
#define MBEDTLS_HASH_FUNC_H
#include "mbedtls/md.h"
#include "md_wrap.h"
#include "security_kdf.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    mbedtls_md_type_t md_type;
    uapi_drv_cipher_hash_type_t hash_type;
} md_type_info;

void mbedtls_alt_get_hash_info(mbedtls_md_type_t md_type, uapi_drv_cipher_hash_type_t *hash_type);

#ifdef __cplusplus
}
#endif
#endif