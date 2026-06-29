/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: mbedtls harden adapt internal header file.
 * Create: 2023-05-10
*/

#ifndef SHA1_ALT_H
#define SHA1_ALT_H

#include "mbedtls_harden_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_sha1_context
{
    mbedtls_alt_hash_clone_ctx clone_ctx;
    unsigned int result_size;
}
mbedtls_sha1_context;


#ifdef __cplusplus
}
#endif

#endif /* sha1_alt.h */