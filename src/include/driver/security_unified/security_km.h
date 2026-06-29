/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: km header file
 *
 * Create: 2025-06-11
 */

#ifndef SECURITY_KM_H
#define SECURITY_KM_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/**
 * @defgroup security_km KM
 * @ingroup  drivers_driver_security_unified
 * @{
 */

#define UAPI_DRV_INVALID_KEY_SLOT    0xFFFFFFFF

/**
 * @if Eng
 * @brief  Keyslot hardware ket type selection.
 * @else
 * @brief  keyslot 硬件密钥类型选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_BOOT_REE = 0,
    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_BOOT_HISI,
    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_BOOT_TEE,

    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_SEC_STORE,

    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_ODRK_REE,
    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_ODRK_TEE,

    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE,
    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_TEE,

    UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_INVALID  = 0xFFFFFFFF
} uapi_drv_keyslot_hard_key_type_t;

/**
 * @if Eng
 * @brief  Keyslot type selection.
 * @else
 * @brief  keyslot 类型选择。
 * @endif
 */
typedef enum {
    UAPI_DRV_KEYSLOT_TYPE_MCIPHER = 0,
    UAPI_DRV_KEYSLOT_TYPE_HMAC,
    UAPI_DRV_KEYSLOT_TYPE_FLASH,
} uapi_drv_keyslot_type_t;

/**
 * @if Eng
 * @brief  The klad target module's algorithm engine, determining the algorithm supported by the sent key.
 * @else
 * @brief  klad 目标模块算法引擎，决定送出的 key 支持哪个算法。
 * @endif
 */
typedef enum {
    UAPI_DRV_KLAD_ENGINE_AES = 0x20,
    UAPI_DRV_KLAD_ENGINE_LAE = 0x40,
    UAPI_DRV_KLAD_ENGINE_SM4 = 0x50,
    UAPI_DRV_KLAD_ENGINE_TDES = 0x70,
    UAPI_DRV_KLAD_ENGINE_SHA1_HMAC = 0xA0,
    UAPI_DRV_KLAD_ENGINE_SHA2_HMAC = 0xA1,
    UAPI_DRV_KLAD_ENGINE_SM3_HMAC = 0xA2,
    UAPI_DRV_KLAD_ENGINE_MAX,
    UAPI_DRV_KLAD_ENGINE_INVALID = 0xffffffff,
} uapi_drv_klad_engine_t;

/**
 * @if Eng
 * @brief  Create one keyslot handle.
 * @param  [out]   keyslot_handle Points to the created keyslot handle.
 * @param  [in]   keyslot_type keyslot type. see @ref uapi_drv_keyslot_type_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  创建一个 keyslot 句柄。
 * @param  [out]   keyslot_handle 指向创建的 keyslot 句柄。
 * @param  [in]   keyslot_type keyslot 类型。 参考 @ref uapi_drv_keyslot_type_t.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_setup(uint32_t *keyslot_handle, uapi_drv_keyslot_type_t keyslot_type);

/**
 * @if Eng
 * @brief  Destroy one keyslot handle.
 * @param  [in]   keyslot_handle Keyslot handle to be destroyed.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  销毁一个 keyslot 句柄。
 * @param  [in]   keyslot_handle 待销毁的 keyslot 句柄。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_teardown(uint32_t keyslot_handle);

/**
 * @if Eng
 * @brief  Set the clear key to the specified keyslot handle.
 * @param  [in]   keyslot_handle The specified keyslot handle.
 * @param  [in]   klad_engine Specified algorithm. see @ref uapi_drv_klad_engine_t.
 * @param  [in]   key Clear key.
 * @param  [in]   key_len The length of clear key in byte. If the key is sent to MCIPHER/FLASH,
 *                  the key_len must be 16, 24, or 32;
 *                  If the key is sent to HMAC, the key_len must be less than or equal to 128.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  向指定的 keyslot 句柄设置明文密钥。
 * @param  [in]   keyslot_handle 指定的 keyslot 句柄。
 * @param  [in]   klad_engine 指定的算法。参考 @ref uapi_drv_klad_engine_t.
 * @param  [in]   key 明文密钥.
 * @param  [in]   key_len 明文密钥的长度，单位是 byte。若送密钥给MCIPHER/FLASH，则key_len要求为16/24/32;
  *                 若送密钥给HMAC，则key_len要求小于等于128。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_set_clear_key(uint32_t keyslot_handle, uapi_drv_klad_engine_t klad_engine,
    const uint8_t *key, uint32_t key_len);

/**
 * @if Eng
 * @brief  Set the hardware key to the specified keyslot handle.
 * @param  [in]   keyslot_handle The specified keyslot handle.
 * @param  [in]   hard_key_type Selected hardware key type. see @ref uapi_drv_keyslot_hard_key_type_t.
 * @param  [in]   klad_engine Specified algorithm. see @ref uapi_drv_klad_engine_t.
 * @param  [in]   salt Salt value, used to derive the working key.
 * @param  [in]   salt_len The length of salt in byte.
 * @param  [in]   key_len The length of working key in byte. The key_len must be 16, 24, or 32;
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  向指定的 keyslot 句柄设置硬件密钥。
 * @param  [in]   keyslot_handle 指定的 keyslot 句柄。
 * @param  [in]   hard_key_type 选择的硬件密钥类型。 参考 @ref uapi_drv_keyslot_hard_key_type_t.
 * @param  [in]   klad_engine 指定的算法。参考 @ref uapi_drv_klad_engine_t.
 * @param  [in]   salt 盐值，用于派生工作密钥。
 * @param  [in]   salt_len 盐值的长度，单位是 byte。
 * @param  [in]   key_len 派生的工作密钥长度，单位是 byte。key_len要求为16/24/32。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_keyslot_set_hard_key(uint32_t keyslot_handle,
    uapi_drv_keyslot_hard_key_type_t hard_key_type, uapi_drv_klad_engine_t klad_engine,
    const uint8_t *salt, uint32_t salt_len, uint32_t key_len);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* SECURITY_CRC_H */