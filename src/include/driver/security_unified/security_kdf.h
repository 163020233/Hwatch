/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Provides key derivation function header file.
 *
 * Create: 2025-10-27
 */

#ifndef SECURITY_KDF_H
#define SECURITY_KDF_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/**
 * @defgroup security_kdf Kdf
 * @ingroup  drivers_driver_security_unified
 * @{
 */

/**
 * @if Eng
 * @brief Hash algorithm type
 * @note  SHA1 and SHA224 are not secure, and we advise not to use them.
 * @else
 * @brief hash算法类型
 * @note  SHA1和SHA224不安全，不建议使用
 * @endif
 */
typedef enum {
    UAPI_DRV_CIPHER_HASH_TYPE_SHA1 = 0xf690a0,
    UAPI_DRV_CIPHER_HASH_TYPE_SHA224 = 0x10690e0,
    UAPI_DRV_CIPHER_HASH_TYPE_SHA256 = 0x1169100,
    UAPI_DRV_CIPHER_HASH_TYPE_SHA384 = 0x127a180,
    UAPI_DRV_CIPHER_HASH_TYPE_SHA512 = 0x137a200,
    UAPI_DRV_CIPHER_HASH_TYPE_SM3 = 0x2169100,

    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA1 = 0x10f690a0,
    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA224 = 0x110690e0,
    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA256 = 0x11169100,
    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA384 = 0x1127a180,
    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA512 = 0x1137a200,
    UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SM3 = 0x12169100,

    UAPI_DRV_CIPHER_HASH_TYPE_INVALID = 0xffffffff,
} uapi_drv_cipher_hash_type_t;

/**
 * @if Eng
 * @brief Parameter structure of PBKDF2
 * @else
 * @brief PBKDF2算法的参数结构体
 * @endif
 */
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
    uint16_t count;         /*!< @if Iterations of PBKDF2.
                                 @else   PBKDF2的迭代次数。 @endif */
} uapi_drv_cipher_kdf_pbkdf2_param_t;

/**
 * @if Eng
 * @brief Parameter structure of key extract
 * @else
 * @brief 密钥提取的参数结构体
 * @endif
 */
typedef struct {
    uapi_drv_cipher_hash_type_t hmac_type;    /*!< @if Type of the HMAC algorithm for extracting keys.
                                                   @else   提取密钥的HMAC算法类型。 @endif */
    uint8_t *salt;    /*!< @if Extract the salt value of the key.
                           @else   提取密钥的盐值。 @endif */
    uint32_t salt_length;    /*!< @if Extract the salt length of the key.
                                  @else   提取密钥的盐值长度。 @endif */
    uint8_t *ikm;           /*!< @if Extract the input key material of the key.
                                 @else   提取密钥的原始密钥材料。 @endif */
    uint32_t ikm_length;    /*!< @if Length of the input key material of the extracted key.
                                 @else   提取密钥的原始密钥材料长度。 @endif */
} uapi_drv_cipher_hkdf_extract_t;

/**
 * @if Eng
 * @brief Parameter structure of key extend
 * @else
 * @brief 密钥拓展的参数结构体
 * @endif
 */
typedef struct {
    uapi_drv_cipher_hash_type_t hmac_type;    /*!< @if HMAC algorithm type of the extended key.
                                                   @else   拓展密钥的HMAC算法类型。 @endif */
    uint8_t *prk;    /*!< @if Pseudo-random key of the extended key.
                          @else   拓展密钥的伪随机密钥。 @endif */
    uint32_t prk_length;    /*!< @if Pseudo-random key length of the extended key.
                                 @else   拓展密钥的伪随机密钥长度。 @endif */
    uint8_t *info;    /*!< @if Information about the extended key.
                           @else   拓展密钥的信息。 @endif */
    uint32_t info_length;   /*!< @if Length of extended key information.
                                 @else   拓展密钥的信息长度。 @endif */
} uapi_drv_cipher_hkdf_expand_t;

/**
 * @if Eng
 * @brief Parameter structure of HKDF
 * @else
 * @brief HKDF参数结构体
 * @endif
 */
typedef struct {
    uapi_drv_cipher_hash_type_t hmac_type;    /*!< @if HMAC algorithm type of the extended key.
                                                   @else   提起、拓展密钥的HMAC算法类型。 @endif */
    uint8_t *salt;    /*!< @if Extract the salt value of the key.
                           @else   提取密钥的盐值。 @endif */
    uint32_t salt_length;    /*!< @if Extract the salt length of the key.
                                  @else   提取密钥的盐值长度。 @endif */
    uint8_t *ikm;    /*!< @if Extract the input key material of the key.
                          @else   提取密钥的原始密钥材料。 @endif */
    uint32_t ikm_length;    /*!< @if Length of the input key material of the extracted key.
                                 @else   提取密钥的原始密钥材料长度。 @endif */
    uint8_t *info;    /*!< @if Information about the extended key.
                           @else   拓展密钥的信息。 @endif */
    uint32_t info_length;    /*!< @if Length of extended key information.
                                  @else   拓展密钥的信息长度。 @endif */
} uapi_drv_cipher_hkdf_t;

/**
 * @if Eng
 * @brief  Use the PBKDF2 algorithm to derive keys.
 * @param  [in]    param Parameter structure of PBKDF2.
 * @param  [in]    out Output key buffer.
 * @param  [in]    out_len Length of the output key buffer.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  PBKDF2算法派生秘钥。
 * @param  [in]    param PBKDF2算法的参数结构体。
 * @param  [in]    out 输出秘钥缓冲区。
 * @param  [in]    out_len 输出秘钥缓冲区长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_pbkdf2(const uapi_drv_cipher_kdf_pbkdf2_param_t *param, uint8_t *out, const uint32_t out_len);

/**
 * @if Eng
 * @brief  Extracting the key.
 * @param  [in]    extract_param Key extraction parameter structure.
 * @param  [in]    prk Pseudo-random key with extended key.
 * @param  [in]    prk_length Extended pseudo-random key length.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  提取密钥。
 * @param  [in]     extract_param 密钥提取的参数结构体。
 * @param  [out]    prk 拓展密钥的伪随机密钥。
 * @param  [out]    prk_length 拓展密钥的伪随机密钥长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hkdf_extract(uapi_drv_cipher_hkdf_extract_t *extract_param,
    uint8_t *prk, uint32_t *prk_length);

/**
 * @if Eng
 * @brief  Extended Key.
 * @param  [in]    expand_param Key extension parameter structure.
 * @param  [in]    okm Output key material.
 * @param  [in]    okm_length Length of the output key material.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  拓展密钥。
 * @param  [in]     expand_param 密钥拓展的参数结构体。
 * @param  [out]    okm 输出密钥材料。
 * @param  [out]    okm_length 输出密钥材料长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hkdf_expand(const uapi_drv_cipher_hkdf_expand_t *expand_param,
    uint8_t *okm, uint32_t okm_length);

/**
 * @if Eng
 * @brief  HKDF, including key extraction and key extension.
 * @param  [in]    hkdf_param HKDF parameter structure.
 * @param  [in]    okm Output key material.
 * @param  [in]    okm_length Length of the output key material.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HKDF，包括提取密钥和拓展密钥两步。
 * @param  [in]     hkdf_param HKDF的参数结构体。
 * @param  [out]    okm 输出密钥材料。
 * @param  [out]    okm_length 输出密钥材料长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_drv_cipher_hkdf(uapi_drv_cipher_hkdf_t *hkdf_param, uint8_t *okm, uint32_t okm_length);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* SECURITY_KDF_H */