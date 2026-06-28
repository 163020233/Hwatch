/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crypto hash common.
 *
 * Create: 2025-05-27
*/
#ifndef CRYPTO_HASH_COMMON_H
#define CRYPTO_HASH_COMMON_H

#include "crypto_hash_struct.h"
#include "crypto_drv_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef hash_null_ptr_chk
#define hash_null_ptr_chk(ptr)   \
    crypto_chk_return((ptr) == NULL, ERROR_PARAM_IS_NULL, #ptr" is NULL\n")
#endif

int32_t crypto_hash_common_type_check(crypto_hash_type hash_type);

int32_t crypto_hash_common_get_state(crypto_hash_type hash_type, uint32_t *state, uint32_t *state_size);

const uint8_t *crypto_hash_common_get_state_iv(crypto_hash_type hash_type, uint32_t *iv_size);

void crypto_hash_common_length_add(uint32_t length[2], uint32_t addition);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
