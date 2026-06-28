/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides hash driver pri header \n

 */
#ifndef SECURITY_HASH_PRI_H
#define SECURITY_HASH_PRI_H

#include "cipher.h"
#include "security_hash.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uapi_drv_cipher_hash_type_t hash_type;  /*!< @if The type of hash algorithm used by PBKDF2.
                                                 @else   PBKDF2使用的hash算法类型。 @endif */
    uint8_t *password;      /*!< @if Password for PBKDF2.
                                 @else   PBKDF2的输入口令。 @endif */
    uint32_t plen;          /*!< @if Password length of PBKDF2.
                                 @else   PBKDF2的输入口令长度。 @endif */
    uint8_t *salt;          /*!< @if Salt of PBKDF2.
                                 @else   PBKDF2的盐值。 @endif */
    uint32_t slen;          /*!< @if Salt of PBKDF2.
                                 @else   PBKDF2的盐值长度。 @endif */
    uint32_t count;         /*!< @if Iterations of PBKDF2.
                                 @else   PBKDF2的迭代次数。 @endif */
} uapi_drv_cipher_kdf_pbkdf2_ex_param_t;

errcode_t uapi_drv_cipher_pbkdf2_ex(const uapi_drv_cipher_kdf_pbkdf2_ex_param_t *param,
    uint8_t *out, const uint32_t out_len);

errcode_t uapi_drv_cipher_hash_compute_clone(hash_handle_t dst_handle, hash_handle_t src_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif