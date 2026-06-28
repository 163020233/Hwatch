/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides common utils for security_unified service layer. \n
 *
 * History: \n
 * 2023-03-14, Create file. \n
 */
#ifndef SL_COMMON_H
#define SL_COMMON_H

#include <stdint.h>
#include "errcode.h"
#include "security_hmac.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Convert security-driven error codes to standard error codes.
 * @param  [in]  from_errcode Error codes returned by the security driver interface.
 * @retval Converted error code. For details, see @ref errcode_t
 * @else
 * @brief  将安全驱动的错误码转换成标准错误码
 * @param  [in]  from_errcode 安全驱动接口返回的错误码
 * @retval 转换后的错误码，参考 @ref errcode_t
 * @endif
 */
errcode_t crypto_sl_common_get_errcode(int32_t from_errcode);

errcode_t crypto_sl_enhance_get_errcode(int32_t from_errcode);

errcode_t crypto_klad_set_clear_key(uint32_t keyslot_handle, uint32_t alg_type, const uint8_t *key, uint32_t key_len);

errcode_t crypto_keyslot_destroy(uint32_t keyslot_handle);

errcode_t crypto_hmac_start(hmac_handle_t *ctx, uint32_t hash_type,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

errcode_t crypto_hmac_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

errcode_t crypto_hmac_finish(hmac_handle_t ctx, uint8_t *out, uint32_t out_len);

errcode_t crypto_hmac_destroy(hmac_handle_t ctx);

errcode_t crypto_hmac(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    uint8_t *out, uint32_t out_len, uint32_t hash_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif