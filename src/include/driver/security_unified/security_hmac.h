/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Hmac simple API header file.
 *
 * Create: 2024-07-04
*/

#ifndef SECURITY_HMAC_H
#define SECURITY_HMAC_H

#include "errcode.h"
#include "security_km.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup security_unified_hmac Hmac
 * @ingroup  drivers_driver_security_unified
 * @{
 */

typedef uintptr_t hmac_handle_t;

/* HASH(Single-Part). */
/**
 * @if Eng
 * @brief  Hmac sha1 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SHA1 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha1(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[20]);

/**
 * @if Eng
 * @brief  Hmac sha224 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SHA224 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha224(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[28]);

/**
 * @if Eng
 * @brief  Hmac sha256 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SHA256 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha256(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[32]);

/**
 * @if Eng
 * @brief  Hmac sha384 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SHA384 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha384(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[48]);

/**
 * @if Eng
 * @brief  Hmac sha512 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SHA512 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha512(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[64]);

/**
 * @if Eng
 * @brief  Hmac sm3 calculate. Single-Part API.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  SM3 Hmac 摘要计算。 单流程接口。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @param  [out]  out Hmac 摘要计算结果
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sm3(const uint8_t *buf, uint32_t len,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, uint8_t out[32]);

/* HASH(Multi-Part) */
/**
 * @if Eng
 * @brief  Create Sha1 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 Sha1 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha1_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SHA1-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SHA1-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha1_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SHA1-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SHA1-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha1_finish(hmac_handle_t ctx, uint8_t out[20]);

/**
 * @if Eng
 * @brief  Create Sha224 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 Sha224 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha224_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SHA224-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SHA224-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha224_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SHA224-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SHA224-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha224_finish(hmac_handle_t ctx, uint8_t out[28]);

/**
 * @if Eng
 * @brief  Create Sha256 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 Sha256 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha256_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SHA256-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SHA256-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha256_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SHA256-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SHA256-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha256_finish(hmac_handle_t ctx, uint8_t out[32]);

/**
 * @if Eng
 * @brief  Create Sha384 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 Sha384 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha384_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SHA384-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SHA384-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha384_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SHA384-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SHA384-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha384_finish(hmac_handle_t ctx, uint8_t out[48]);

/**
 * @if Eng
 * @brief  Create Sha512 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 Sha512 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha512_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SHA512-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SHA512-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha512_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SHA512-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SHA512-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sha512_finish(hmac_handle_t ctx, uint8_t out[64]);

/**
 * @if Eng
 * @brief  Create SM3 Hmac handle.
 * @param  [out]  ctx Hmac handle.
 * @param  [in]   key Key buffer.
 * @param  [in]   key_len Key length.
 * @param  [in]   keyslot_handle Keyslot handle. If not need please set as UAPI_DRV_INVALID_KEY_SLOT.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建 SM3 Hmac 句柄。
 * @param  [out]  ctx Hmac 句柄。
 * @param  [in]   key 密钥。
 * @param  [in]   key_len 密钥长度。
 * @param  [in]   keyslot_handle Keyslot 句柄，如果不需要传入，请传入 UAPI_DRV_INVALID_KEY_SLOT。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sm3_start(hmac_handle_t *ctx,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Update SM3-Hmac message. Support multi-calling.
 * @param  [in]   ctx Hmac handale.
 * @param  [in]   buf Message.
 * @param  [in]   len Message length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新 SM3-Hmac 消息内容。支持多次调用。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [in]   buf 消息。
 * @param  [in]   len 消息长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sm3_update(hmac_handle_t ctx, const uint8_t *buf, uint32_t len);

/**
 * @if Eng
 * @brief  Finish SM3-Hmac hash calculation and get hash result.
 * @param  [in]   ctx Hmac handle.
 * @param  [out]  out Hmac result.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  结束 SM3-Hmac hash 计算，并获取摘要结果。
 * @param  [in]   ctx Hmac 句柄。
 * @param  [out]  out 摘要结果.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_sm3_finish(hmac_handle_t ctx, uint8_t out[32]);

/**
 * @if Eng
 * @brief  Destroy Hmac handle.
 * @param  [in]   ctx Hmac handle.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  销毁 Hmac 句柄。
 * @param  [in]   ctx Hmac 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hmac_destroy(hmac_handle_t ctx);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* SECURITY_HMAC_H */