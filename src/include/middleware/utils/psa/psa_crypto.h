/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.\n
 * Description: Implementation of the PSA Crypto API. \n
 *
 * History: \n
 * 2025-06-21, Create file. \n
 */

#ifndef PSA_CRYPTO_H
#define PSA_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "psa/psa_error.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief  Definition of key type.
 */
typedef uint16_t psa_key_type_t;

/**
 * @brief  Definition of key bits.
 */
typedef uint16_t psa_key_bits_t;

/**
 * @brief  Definition of key lifetime.
 */
typedef uint32_t psa_key_lifetime_t;

/**
 * @brief  Definition of key id.
 */
typedef uint32_t psa_key_id_t;

/**
 * @brief  Definition of key usage.
 */
typedef uint32_t psa_key_usage_t;

/**
 * @brief  Definition of key algorithm.
 */
typedef uint32_t psa_algorithm_t;

/**
 * @brief  Definition of key persistence.
 */
typedef uint8_t  psa_key_persistence_t;

/**
 * @brief  Definition of key location.
 */
typedef uint32_t psa_key_location_t;

/**
 * @brief  Definition of ecc family.
 */
typedef uint8_t psa_ecc_family_t;

/**
 * @brief  Definition of dh family.
 */
typedef uint8_t psa_dh_family_t;

/**
 * @brief  Definition of derivation step.
 */
typedef uint16_t psa_key_derivation_step_t;

/**
 * @brief  Definition of key policy.
 */
typedef struct {
    psa_key_usage_t usage;
    psa_algorithm_t alg;
} psa_key_policy_t;

/**
 * @brief  Definition of key attribute.
 */
typedef struct {
    psa_key_type_t type;
    psa_key_bits_t bits;
    psa_key_lifetime_t lifetime;
    psa_key_policy_t policy;
    psa_key_id_t key;
} psa_key_attributes_t;

/**
 * @brief  Definition of hkdf key derivation.
 */
typedef struct {
    uint8_t *salt;
    size_t salt_length;
    uint8_t *info;
    size_t info_length;
    bool is_ikm_key_id;
    psa_key_id_t ikm_key;
    uint8_t *ikm;
    size_t ikm_length;
} psa_hkdf_key_derivation_t;

/**
 * @brief  Definition of pbkdf2 key derivation.
 */
typedef struct {
    bool is_password_key_id;
    psa_key_id_t password_key;
    uint8_t *password;
    size_t password_length;
    bool is_salt_key_id;
    psa_key_id_t salt_key;
    uint8_t *salt;
    size_t salt_length;
    uint64_t count;
} psa_pbkdf2_key_derivation_t;

/**
 * @brief  Definition of derivation key operation.
 */
typedef struct {
    bool is_set_up;
    psa_algorithm_t alg;
    size_t capacity;
    psa_hkdf_key_derivation_t hkdf;
    psa_pbkdf2_key_derivation_t pbkdf2;
} psa_key_derivation_operation_t;

/**
 * @brief  Definition of cipher operation.
 */
typedef struct {
    psa_key_id_t key;
    psa_algorithm_t alg;
} psa_cipher_operation_t;

/**
 * @if Eng
 * @brief  The major version of this implementation of the PSA Crypto API.
 * @else
 * @brief  PSA Crypto API实现的主要版本。
 * @endif
 */
#define PSA_CRYPTO_API_VERSION_MAJOR 1

/**
 * @if Eng
 * @brief  The minor version of this implementation of the PSA Crypto API.
 * @else
 * @brief  PSA Crypto API实现的较小版本。
 * @endif
 */
#define PSA_CRYPTO_API_VERSION_MINOR 0

/**
 * @if Eng
 * @brief  Key type Initial value.
 * @else
 * @brief  密钥类型初始化值。
 * @endif
 */
#define PSA_KEY_TYPE_NONE ((psa_key_type_t)0x0000)

/**
 * @if Eng
 * @brief  Key lifetime Initial value.
 * @else
 * @brief  密钥生命周期初始化值。
 * @endif
 */
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t)0x00000000)

/**
 * @if Eng
 * @brief  Key policy Initial value.
 * @else
 * @brief  密钥使用策略初始化值。
 * @endif
 */
#define PSA_KEY_POLICY_INIT { 0, 0 }

/**
 * @if Eng
 * @brief  Key ID Initial value.
 * @else
 * @brief  密钥ID初始化值。
 * @endif
 */
#define PSA_KEY_ID_INIT ((psa_key_id_t) 0)

/**
 * @if Eng
 * @brief  Key attributes Initial value.
 * @else
 * @brief  密钥属性初始化值。
 * @endif
 */
#define PSA_KEY_ATTRIBUTES_INIT            { PSA_KEY_TYPE_NONE, 0,            \
                                             PSA_KEY_LIFETIME_VOLATILE,       \
                                             PSA_KEY_POLICY_INIT,             \
                                             PSA_KEY_ID_INIT }
/**
 * @if Eng
 * @brief  Raw data key type, only for test.
 * @else
 * @brief  原始数据密钥类型, 测试使用。
 * @endif
 */
#define PSA_KEY_TYPE_RAW_DATA               ((psa_key_type_t) 0x1001)

/**
 * @if Eng
 * @brief  HMAC key type.
 * @else
 * @brief  HMAC密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_HMAC                   ((psa_key_type_t) 0x1100)

/**
 * @if Eng
 * @brief  Derivation key type.
 * @else
 * @brief  派生密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_DERIVE                 ((psa_key_type_t) 0x1200)

/**
 * @if Eng
 * @brief  A low-entropy secret for password hashing or key derivation.
 * @else
 * @brief  用于口令哈希或密钥派生的低熵秘密。
 * @endif
 */
#define PSA_KEY_TYPE_PASSWORD          ((psa_key_type_t) 0x1203)

/**
 * @if Eng
 * @brief  PASSWORD HASH key type.
 * @else
 * @brief  密码摘要密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_PASSWORD_HASH          ((psa_key_type_t) 0x1205)

/**
 * @if Eng
 * @brief  AES key type.
 * @else
 * @brief  AES密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_AES                    ((psa_key_type_t) 0x2400)

/**
 * @if Eng
 * @brief  SM4 key type.
 * @else
 * @brief  SM4密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_SM4                    ((psa_key_type_t) 0x2405)

/**
 * @if Eng
 * @brief  RSA public key type.
 * @else
 * @brief  RSA公钥密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY         ((psa_key_type_t) 0x4001)

/**
 * @if Eng
 * @brief  RSA private key type.
 * @else
 * @brief  RSA私钥密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_RSA_KEY_PAIR           ((psa_key_type_t) 0x7001)

/**
 * @if Eng
 * @brief  ECC public key base key type.
 * @else
 * @brief  ECC公钥基础密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE    ((psa_key_type_t) 0x4100)

/**
 * @if Eng
 * @brief  ECC private key base key type.
 * @else
 * @brief  ECC私钥基础密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR_BASE      ((psa_key_type_t) 0x7100)

/**
 * @if Eng
 * @brief  DH public key base key type.
 * @else
 * @brief  DH公钥基础密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY_BASE     ((psa_key_type_t) 0x4200)

/**
 * @if Eng
 * @brief  DH private key base key type.
 * @else
 * @brief  DH私钥基础密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_DH_KEY_PAIR_BASE       ((psa_key_type_t) 0x7200)

/**
 * @if Eng
 * @brief  NIST 256/384/521 curve name.
 * @else
 * @brief  NIST 256/384/521曲线名。
 * @endif
 */
#define PSA_ECC_FAMILY_SECP_R1              ((psa_ecc_family_t) 0x12)

/**
 * @if Eng
 * @brief  BRAINPOOL 256/384/521 curve name.
 * @else
 * @brief  BRAINPOOL 256/384/521曲线名。
 * @endif
 */
#define PSA_ECC_FAMILY_BRAINPOOL_P_R1       ((psa_ecc_family_t) 0x30)

/**
 * @if Eng
 * @brief  Montgomery curve name.
 * @else
 * @brief  Montgomery曲线名。
 * @endif
 */
#define PSA_ECC_FAMILY_MONTGOMERY           ((psa_ecc_family_t) 0x41)    /* curve 25519 */

/**
 * @if Eng
 * @brief  SM2 curve name.
 * @else
 * @brief  SM2曲线名。
 * @endif
 */
#define PSA_ECC_FAMILY_SM2                  ((psa_ecc_family_t) 0x51)    /* sm2 */

/**
 * @if Eng
 * @brief  RFC7919 dh family name.
 * @else
 * @brief  RFC7919 密钥交换族名。
 * @endif
 */
#define PSA_DH_FAMILY_RFC7919               ((psa_dh_family_t)  0x03)

/**
 * @if Eng
 * @brief  Maximum key bit.
 * @else
 * @brief  最大密钥比特。
 * @endif
 */
#define PSA_MAX_KEY_BITS                    ((psa_key_bits_t) 0x1000)

#define PSA_CRYPTO_RSA_2048_BIT 2048
#define PSA_CRYPTO_RSA_3072_BIT 3072
#define PSA_CRYPTO_RSA_4096_BIT 4096

/**
 * @if Eng
 * @brief  Local storage key location.
 * @else
 * @brief  本地存储密钥存储类型。
 * @endif
 */
#define PSA_KEY_LOCATION_LOCAL_STORAGE      ((psa_key_location_t) 0x000000)

/**
 * @if Eng
 * @brief  Volatile key.
 * @else
 * @brief  临时密钥。
 * @endif
 */
#define PSA_KEY_PERSISTENCE_VOLATILE        ((psa_key_persistence_t) 0x00)

/**
 * @if Eng
 * @brief  Persistent key.
 * @else
 * @brief  永久密钥。
 * @endif
 */
#define PSA_KEY_PERSISTENCE_DEFAULT         ((psa_key_persistence_t) 0x01)

/**
 * @if Eng
 * @brief  Encrypt key usage.
 * @else
 * @brief  加密密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_ENCRYPT               ((psa_key_usage_t) 0x00000100)

/**
 * @if Eng
 * @brief  Decrypt key usage.
 * @else
 * @brief  解密密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_DECRYPT               ((psa_key_usage_t) 0x00000200)

/**
 * @if Eng
 * @brief  Message input sign key usage.
 * @else
 * @brief  消息输入签名密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_SIGN_MESSAGE          ((psa_key_usage_t) 0x00000400)

/**
 * @if Eng
 * @brief  Message input verify key usage.
 * @else
 * @brief  消息输入验签密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_VERIFY_MESSAGE        ((psa_key_usage_t) 0x00000800)

/**
 * @if Eng
 * @brief  HASH input sign key usage.
 * @else
 * @brief  摘要输入签名密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_SIGN_HASH             ((psa_key_usage_t) 0x00001000)

/**
 * @if Eng
 * @brief  HASH input verify key usage.
 * @else
 * @brief  摘要输入验签密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_VERIFY_HASH           ((psa_key_usage_t) 0x00002000)

/**
 * @if Eng
 * @brief  Derivation key usage.
 * @else
 * @brief  派生密钥用途。
 * @endif
 */
#define PSA_KEY_USAGE_DERIVE                ((psa_key_usage_t) 0x00004000)

/**
 * @if Eng
 * @brief  Invalid algorithm.
 * @else
 * @brief  非法算法。
 * @endif
 */
#define PSA_ALG_NONE                        ((psa_algorithm_t)0)

/**
 * @if Eng
 * @brief  SHA256 algorithm.
 * @else
 * @brief  SHA256算法。
 * @endif
 */
#define PSA_ALG_SHA_256                     ((psa_algorithm_t) 0x02000009)

/**
 * @if Eng
 * @brief  SHA512 algorithm, only for test.
 * @else
 * @brief  SHA512算法，测试使用。
 * @endif
 */
#define PSA_ALG_SHA_512                     ((psa_algorithm_t) 0x0200000b)

/**
 * @if Eng
 * @brief  SM3 algorithm.
 * @else
 * @brief  SM3算法。
 * @endif
 */
#define PSA_ALG_SM3                         ((psa_algorithm_t) 0x02000014)

/**
 * @if Eng
 * @brief  HAMC algorithm.
 * @else
 * @brief  HAMC算法。
 * @endif
 */
#define PSA_ALG_HMAC_BASE                   ((psa_algorithm_t) 0x03800000)

/**
 * @if Eng
 * @brief  CBC_MAC algorithm.
 * @else
 * @brief  CBC_MAC算法。
 * @endif
 */
#define PSA_ALG_CBC_MAC                         ((psa_algorithm_t)0x03c00100)

/**
 * @if Eng
 * @brief  CMAC algorithm.
 * @else
 * @brief  CMAC算法。
 * @endif
 */
#define PSA_ALG_CMAC                        ((psa_algorithm_t) 0x03c00200)

/**
 * @if Eng
 * @brief  ECB algorithm.
 * @else
 * @brief  ECB算法。
 * @endif
 */
#define PSA_ALG_ECB_NO_PADDING              ((psa_algorithm_t) 0x04404400)

/**
 * @if Eng
 * @brief  CBC algorithm.
 * @else
 * @brief  CBC算法。
 * @endif
 */
#define PSA_ALG_CBC_NO_PADDING              ((psa_algorithm_t) 0x04404000)

/**
 * @if Eng
 * @brief  CBC PKCS7 padding algorithm.
 * @else
 * @brief  CBC PKCS7格式填充算法。
 * @endif
 */
#define PSA_ALG_CBC_PKCS7                   ((psa_algorithm_t) 0x04404100)

/**
 * @if Eng
 * @brief  CFB algorithm, only for test.
 * @else
 * @brief  CFB算法，测试使用。
 * @endif
 */
#define PSA_ALG_CFB                         ((psa_algorithm_t) 0x04c01100)

/**
 * @if Eng
 * @brief  CTR algorithm.
 * @else
 * @brief  CTR算法。
 * @endif
 */
#define PSA_ALG_CTR                         ((psa_algorithm_t) 0x04c01000)

/**
 * @if Eng
 * @brief  CCM algorithm.
 * @else
 * @brief  CCM算法。
 * @endif
 */
#define PSA_ALG_CCM                         ((psa_algorithm_t) 0x05500100)

/**
 * @if Eng
 * @brief  GCM algorithm.
 * @else
 * @brief  GCM算法。
 * @endif
 */
#define PSA_ALG_GCM                         ((psa_algorithm_t) 0x05500200)

/**
 * @if Eng
 * @brief  RSA PKCS1V15 sign algorithm.
 * @else
 * @brief  RSA PKCS1V15填充签名验签算法。
 * @endif
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN_BASE      ((psa_algorithm_t) 0x06000200)

/**
 * @if Eng
 * @brief  RSA PKCS1V21 sign algorithm.
 * @else
 * @brief  RSA PKCS1V21填充签名验签算法。
 * @endif
 */
#define PSA_ALG_RSA_PSS_BASE                ((psa_algorithm_t) 0x06000300)

/**
 * @if Eng
 * @brief  RSA PKCS1V21 any salt input sign algorithm, only for test.
 * @else
 * @brief  RSA PKCS1V21任意盐值输入签名验签算法，测试使用。
 * @endif
 */
#define PSA_ALG_RSA_PSS_ANY_SALT_BASE       ((psa_algorithm_t) 0x06001300)

/**
 * @if Eng
 * @brief  ECDSA sign algorithm.
 * @else
 * @brief  ECDSA签名验签算法。
 * @endif
 */
#define PSA_ALG_ECDSA_BASE                  ((psa_algorithm_t) 0x06000600)

/**
 * @if Eng
 * @brief  ECDSA any hash input sign algorithm, only for test.
 * @else
 * @brief  ECDSA任意摘要输入签名验签算法， 测试使用。
 * @endif
 */
#define PSA_ALG_ECDSA_ANY                   PSA_ALG_ECDSA_BASE

/**
 * @if Eng
 * @brief  RSA PKCS1V15 crypto algorithm.
 * @else
 * @brief  RSA PKCS1V15填充格式加密解密算法。
 * @endif
 */
#define PSA_ALG_RSA_PKCS1V15_CRYPT          ((psa_algorithm_t) 0x07000200)

/**
 * @if Eng
 * @brief  RSA PKCS1V21 crypto algorithm.
 * @else
 * @brief  RSA PKCS1V21填充格式加密解密算法。
 * @endif
 */
#define PSA_ALG_RSA_OAEP_BASE               ((psa_algorithm_t) 0x07000300)

/**
 * @if Eng
 * @brief  ECC crypto algorithm.
 * @else
 * @brief  ECC加解密算法
 * @endif
 */
#define PSA_ALG_EC_ELGAMAL                  ((psa_algorithm_t) 0x07000700)

/**
 * @if Eng
 * @brief  HKDF algorithm.
 * @else
 * @brief  HKDF算法。
 * @endif
 */
#define PSA_ALG_HKDF_BASE                   ((psa_algorithm_t) 0x08000100)

#define PSA_ALG_PBKDF2_HMAC_BASE            ((psa_algorithm_t) 0x08800100)

/**
 * @if Eng
 * @brief  ECDH key agreement algorithm.
 * @else
 * @brief  ECDH秘钥协商算法。
 * @endif
 */
#define PSA_ALG_ECDH                        ((psa_algorithm_t) 0x09020000)

/**
 * @if Eng
 * @brief  DH key agreement algorithm.
 * @else
 * @brief  DH秘钥协商算法。
 * @endif
 */
#define PSA_ALG_FFDH                        ((psa_algorithm_t) 0x09010000)

/**
 * @if Eng
 * @brief  ECC curve name mask.
 * @else
 * @brief  ECC曲线名掩码。
 * @endif
 */
#define PSA_KEY_TYPE_ECC_CURVE_MASK                 ((psa_key_type_t) 0x00ff)

/**
 * @if Eng
 * @brief  DH group name mask.
 * @else
 * @brief  DH族名掩码。
 * @endif
 */
#define PSA_KEY_TYPE_DH_GROUP_MASK                  ((psa_key_type_t) 0x00ff)

/**
 * @if Eng
 * @brief  HASH algorithm mask.
 * @else
 * @brief  摘要算法掩码。
 * @endif
 */
#define PSA_ALG_HASH_MASK                       ((psa_algorithm_t) 0x000000ff)

/**
 * @if Eng
 * @brief  AEAD algorithm mask.
 * @else
 * @brief  关联数据加密算法掩码。
 * @endif
 */
#define PSA_ALG_AEAD_ALG_MASK                   ((psa_algorithm_t) 0x00000300)

/**
 * @if Eng
 * @brief  AEAD tag length mask.
 * @else
 * @brief  关联数据加密标签长度掩码。
 * @endif
 */
#define PSA_ALG_AEAD_TAG_LENGTH_MASK            ((psa_algorithm_t) 0x003f0000)

/**
 * @if Eng
 * @brief  algorithm category mask.
 * @else
 * @brief  算法类别掩码。
 * @endif
 */
#define PSA_ALG_CATEGORY_MASK                   ((psa_algorithm_t) 0x7f000000)

/**
 * @if Eng
 * @brief  HASH algorithm category.
 * @else
 * @brief  摘要算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_HASH                   ((psa_algorithm_t) 0x02000000)

/**
 * @if Eng
 * @brief  MAC algorithm category.
 * @else
 * @brief  MAC算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_MAC                    ((psa_algorithm_t) 0x03000000)

/**
 * @if Eng
 * @brief  MAC algorithm category.
 * @else
 * @brief  报文认证码算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_CIPHER                 ((psa_algorithm_t) 0x04000000)

/**
 * @if Eng
 * @brief  AEAD algorithm category.
 * @else
 * @brief  附加关联数据加密算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_AEAD                   ((psa_algorithm_t) 0x05000000)

/**
 * @if Eng
 * @brief  Sign algorithm category.
 * @else
 * @brief  签名验签算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_SIGN                   ((psa_algorithm_t) 0x06000000)

/**
 * @if Eng
 * @brief  Asymmetric encryption category.
 * @else
 * @brief  非对称加密算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION  ((psa_algorithm_t) 0x07000000)

/**
 * @if Eng
 * @brief  Key agreement category.
 * @else
 * @brief  密钥交换算法类别。
 * @endif
 */
#define PSA_ALG_CATEGORY_KEY_AGREEMENT          ((psa_algorithm_t) 0x09000000)

/**
 * @if Eng
 * @brief  Key type mask.
 * @else
 * @brief  密钥类型掩码。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_MASK                  ((psa_key_type_t)0x7000)

/**
 * @if Eng
 * @brief  Raw type mask.
 * @else
 * @brief  原始密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_RAW                   ((psa_key_type_t) 0x1000)

/**
 * @if Eng
 * @brief  Symmetric key type.
 * @else
 * @brief  对称密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_SYMMETRIC             ((psa_key_type_t)0x2000)

/**
 * @if Eng
 * @brief  Public key type.
 * @else
 * @brief  公钥密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY            ((psa_key_type_t)0x4000)

/**
 * @if Eng
 * @brief  Private key type.
 * @else
 * @brief  私钥密钥类型。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_KEY_PAIR              ((psa_key_type_t)0x7000)

/**
 * @if Eng
 * @brief  Private key flag category.
 * @else
 * @brief  私钥标志类型。
 * @endif
 */
#define PSA_KEY_TYPE_CATEGORY_FLAG_PAIR             ((psa_key_type_t)0x3000)

/**
 * @if Eng
 * @brief  AEAD tag length offset.
 * @else
 * @brief  AEAD标签长度偏移。
 * @endif
 */
#define PSA_AEAD_TAG_LENGTH_OFFSET 16

/**
 * @if Eng
 * @brief  AEAD at least this length flag.
 * @else
 * @brief  AEAD最小标签长度标志。
 * @endif
 */
#define PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG  ((psa_algorithm_t)0x00008000)

/**
 * @if Eng
 * @brief  MAC length mask.
 * @else
 * @brief  MAC长度掩码。
 * @endif
 */
#define PSA_ALG_MAC_TRUNCATION_MASK             ((psa_algorithm_t)0x003f0000)

/**
 * @if Eng
 * @brief  MAC length offset.
 * @else
 * @brief  MAC长度偏移。
 * @endif
 */
#define PSA_MAC_TRUNCATION_OFFSET 16

/**
 * @if Eng
 * @brief  MAC at least this length flag.
 * @else
 * @brief  MAC最小长度标签。
 * @endif
 */
#define PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG   ((psa_algorithm_t)0x00008000)

/**
 * @if Eng
 * @brief  Deterministic ECDSA flag.
 * @else
 * @brief  确定ECDSA算法标签。
 * @endif
 */
#define PSA_ALG_ECDSA_DETERMINISTIC_FLAG        ((psa_algorithm_t)0x00000100)

/**
 * @if Eng
 * @brief  Key derivation operation initial value.
 * @else
 * @brief  密钥派生操作初始化值。
 * @endif
 */
#define PSA_KEY_DERIVATION_OPERATION_INIT { 0, 0, 0, { 0 }, { 0 } }

/**
 * @if Eng
 * @brief  Key derivation secret parameter input, ikm for HKDF.
 * @else
 * @brief  密钥派生秘密参数输入，即HKDF中的ikm参数。
 * @endif
 */
#define PSA_KEY_DERIVATION_INPUT_SECRET     ((psa_key_derivation_step_t) 0x0101)

/**
 * @if Eng
 * @brief  Key derivation password parameter input.
 * @else
 * @brief  密钥派生密码参数输入。
 * @endif
 */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD   ((psa_key_derivation_step_t) 0x0102)

/**
 * @if Eng
 * @brief  Key derivation salt parameter input.
 * @else
 * @brief  密钥派生盐值参数输入。
 * @endif
 */
#define PSA_KEY_DERIVATION_INPUT_SALT       ((psa_key_derivation_step_t) 0x0202)

/**
 * @if Eng
 * @brief  Key derivation info parameter input.
 * @else
 * @brief  密钥派生消息参数输入。
 * @endif
 */
#define PSA_KEY_DERIVATION_INPUT_INFO       ((psa_key_derivation_step_t) 0x0203)

/**
 * @if Eng
 * @brief  Key derivation cost parameter input, count for PBKDF2.
 * @else
 * @brief  密钥派生成本参数输入，即PBKDF2中count参数。
 * @endif
 */
#define PSA_KEY_DERIVATION_INPUT_COST       ((psa_key_derivation_step_t) 0x0205)

/**
 * @if Eng
 * @brief  ECC private key type.
 * @param  ECC curve name. see @ref psa_ecc_family_t
 * @else
 * @brief  ECC私钥类型。
 * @param  ECC曲线名。参考 @ref psa_ecc_family_t
 * @endif
 */
#define PSA_KEY_TYPE_ECC_KEY_PAIR(curve)    (PSA_KEY_TYPE_ECC_KEY_PAIR_BASE | (curve))

/**
 * @if Eng
 * @brief  ECC public key type.
 * @param  ECC curve name. see @ref psa_ecc_family_t
 * @else
 * @brief  ECC公钥类型。
 * @param  ECC曲线名。参考 @ref psa_ecc_family_t
 * @endif
 */
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve)    (PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE | (curve))

/**
 * @if Eng
 * @brief  DH private key type.
 * @param  DH group name. see @ref psa_dh_family_t
 * @else
 * @brief  DH私钥类型。
 * @param  DH族名。参考 @ref psa_dh_family_t
 * @endif
 */
#define PSA_KEY_TYPE_DH_KEY_PAIR(group)    (PSA_KEY_TYPE_DH_KEY_PAIR_BASE | (group))

/**
 * @if Eng
 * @brief  DH public key type.
 * @param  DH group name. see @ref psa_dh_family_t
 * @else
 * @brief  DH公钥类型。
 * @param  DH族名。参考 @ref psa_dh_family_t
 * @endif
 */
#define PSA_KEY_TYPE_DH_PUBLIC_KEY(group)    (PSA_KEY_TYPE_DH_PUBLIC_KEY_BASE | (group))

/**
 * @if Eng
 * @brief  DH public key type.
 * @param  DH group name. see @ref psa_dh_family_t
 * @else
 * @brief  DH公钥类型。
 * @param  DH族名。参考 @ref psa_dh_family_t
 * @endif
 */
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type)    ((type) & ~PSA_KEY_TYPE_CATEGORY_FLAG_PAIR)

/**
 * @if Eng
 * @brief  Whether a key type is an elliptic curve key (pair or public-only).
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥类型是否是ECC。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_ECC(type)    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) &                        \
      ~PSA_KEY_TYPE_ECC_CURVE_MASK) == PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE)

/**
 * @if Eng
 * @brief  Whether a key type is an elliptic curve key pair.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥类型是否是ECC私钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)                               \
    (((type) & ~PSA_KEY_TYPE_ECC_CURVE_MASK) ==                         \
     PSA_KEY_TYPE_ECC_KEY_PAIR_BASE)

/**
 * @if Eng
 * @brief  Whether a key type is an elliptic curve public key.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥类型是否是ECC公钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type)                            \
    (((type) & ~PSA_KEY_TYPE_ECC_CURVE_MASK) ==                         \
     PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE)

/**
 * @if Eng
 * @brief  Whether a key type is an RSA public key.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥类型是否是RSA公钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_PUBLIC_KEY(type)                                \
    (((type) & PSA_KEY_TYPE_CATEGORY_MASK) == PSA_KEY_TYPE_CATEGORY_PUBLIC_KEY)

/**
 * @if Eng
 * @brief  Extract the curve from an elliptic curve key type.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  从密钥类型中获取曲线名。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_ECC_GET_FAMILY(type)                        \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_ECC(type) ?             \
                         ((type) & PSA_KEY_TYPE_ECC_CURVE_MASK) : \
                         0))
/**
 * @if Eng
 * @brief  Whether a key type is a unstructured key.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥是否是未封装密钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_UNSTRUCTURED(type) \
    (((type) & PSA_KEY_TYPE_CATEGORY_MASK) == PSA_KEY_TYPE_CATEGORY_RAW || \
     ((type) & PSA_KEY_TYPE_CATEGORY_MASK) == PSA_KEY_TYPE_CATEGORY_SYMMETRIC)

/**
 * @if Eng
 * @brief  Whether a key type is a RSA key.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断密钥是否是RSA密钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_RSA(type)                                       \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == PSA_KEY_TYPE_RSA_PUBLIC_KEY)

/**
 * @if Eng
 * @brief  Whether a key type is a Diffie-Hellman key (pair or public-only).
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断秘钥类型是否是DH。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_DH(type)                                        \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) &                        \
      ~PSA_KEY_TYPE_DH_GROUP_MASK) == PSA_KEY_TYPE_DH_PUBLIC_KEY_BASE)

/**
 * @if Eng
 * @brief  Whether a key type is a Diffie-Hellman key pair.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断秘钥类型是否是DH私钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_DH_KEY_PAIR(type)                               \
    (((type) & ~PSA_KEY_TYPE_DH_GROUP_MASK) ==                         \
     PSA_KEY_TYPE_DH_KEY_PAIR_BASE)
/**
 * @if Eng
 * @brief  Whether a key type is a Diffie-Hellman public key.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  判断秘钥类型是否是DH公钥。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_IS_DH_PUBLIC_KEY(type)                            \
    (((type) & ~PSA_KEY_TYPE_DH_GROUP_MASK) ==                         \
     PSA_KEY_TYPE_DH_PUBLIC_KEY_BASE)

/**
 * @if Eng
 * @brief  Extract the group from a Diffie-Hellman key type.
 * @param  key type. see @ref psa_key_type_t
 * @else
 * @brief  从密钥类型中获取DH族名。
 * @param  密钥类型。参考 @ref psa_key_type_t
 * @endif
 */
#define PSA_KEY_TYPE_DH_GET_FAMILY(type)                        \
    ((psa_dh_family_t) (PSA_KEY_TYPE_IS_DH(type) ?              \
                        ((type) & PSA_KEY_TYPE_DH_GROUP_MASK) :  \
                        0))

/**
 * @if Eng
 * @brief  Construct a lifetime from a persistence level and a location.
 * @param  persistence. see @ref psa_key_persistence_t
 * @param  location. see @ref psa_key_location_t
 * @else
 * @brief  密钥生命周期组合。
 * @param  生命周期。参考 @ref psa_key_persistence_t
 * @param  存储位置。参考 @ref psa_key_location_t
 * @endif
 */
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location)  ((location) << 8 | (persistence))

/**
 * @if Eng
 * @brief  Extract the persistence from lifetime.
 * @param  lifetime. see @ref psa_key_lifetime_t
 * @else
 * @brief  从生命周期中获取存储时效。
 * @param  生命周期。参考 @ref psa_key_lifetime_t
 * @endif
 */
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime)      \
    ((psa_key_persistence_t) ((lifetime) & 0x000000ff))

/**
 * @if Eng
 * @brief  Extract the location from lifetime.
 * @param  lifetime. see @ref psa_key_lifetime_t
 * @else
 * @brief  从生命周期中获取存储位置。
 * @param  生命周期。参考 @ref psa_key_lifetime_t
 * @endif
 */
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime)      \
    ((psa_key_location_t) ((lifetime) >> 8))

/**
 * @if Eng
 * @brief  Whether a key is volatile.
 * @param  lifetime. see @ref psa_key_lifetime_t
 * @else
 * @brief  判断密钥是否是临时密钥。
 * @param  生命周期。参考 @ref psa_key_lifetime_t
 * @endif
 */
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)  \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == \
     PSA_KEY_PERSISTENCE_VOLATILE)

/**
 * @if Eng
 * @brief  HMAC algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的HMAC算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_HMAC(hash_alg)                                  \
    (PSA_ALG_HMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  RSA PKCS1V15 sign algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的RSA PKCS1V15 签名算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_RSA_PKCS1V15_SIGN(hash_alg)                     \
    (PSA_ALG_RSA_PKCS1V15_SIGN_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  RSA PKCS1V21 sign algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的RSA PKCS1V21 签名算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_RSA_PSS(hash_alg)                               \
    (PSA_ALG_RSA_PSS_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  ECDSA algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的ECDSA签名算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_ECDSA(hash_alg)                                 \
    (PSA_ALG_ECDSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  RSA OAEP algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的RSA OAEP算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_RSA_OAEP(hash_alg)                              \
    (PSA_ALG_RSA_OAEP_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  HKDF algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的HKDF算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_HKDF(hash_alg)                                  \
    (PSA_ALG_HKDF_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  Key agreement algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的密钥协商算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_KEY_AGREEMENT(ka_alg, kdf_alg)  \
    ((ka_alg) | (kdf_alg))

/**
 * @if Eng
 * @brief  RSA PSS any salt algorithm with hash algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  由摘要算法组合的RSA PSS 任意盐值算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_RSA_PSS_ANY_SALT(hash_alg)                      \
    (PSA_ALG_RSA_PSS_ANY_SALT_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

#define PSA_ALG_PBKDF2_HMAC(hash_alg)                                  \
    (PSA_ALG_PBKDF2_HMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  Extract hash from HMAC key algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从HMAC算法获取摘要算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_HMAC_GET_HASH(hmac_alg)                             \
    (PSA_ALG_CATEGORY_HASH | ((hmac_alg) & PSA_ALG_HASH_MASK))

/**
 * @if Eng
 * @brief  Extract hash length from HASH algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从HASH算法获取摘要长度。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_HASH_LENGTH(alg)                                        \
    (                                                               \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_256 ? 32 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SM3 ? 32 :        \
        0)

/**
 * @if Eng
 * @brief  Extract hash algorithm from combination HASH algorithm.
 * @param  hash_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从组合HASH算法获取纯HASH算法。
 * @param  摘要算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_GET_HASH(alg) \
        (((alg) & 0x000000ff) == 0 ? ((psa_algorithm_t)0) : 0x02000000 | ((alg) & 0x000000ff))

/**
 * @if Eng
 * @brief  Whether a key algorithm is HASH.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是HASH。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_HASH(alg)                                            \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_HASH)

/**
 * @if Eng
 * @brief  Whether a key algorithm is MAC.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是MAC。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_MAC(alg)                                             \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_MAC)

/**
 * @if Eng
 * @brief  Whether a key algorithm is crypto.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是加解密。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_CIPHER(alg)                                          \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_CIPHER)

/**
 * @if Eng
 * @brief  Whether a key algorithm is AEAD.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是附加关联数据加解密。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_AEAD(alg)                                            \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_AEAD)

/**
 * @if Eng
 * @brief  Whether a key algorithm is sign.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是签名。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_SIGN(alg)                                            \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_SIGN)

/**
 * @if Eng
 * @brief  Whether a key algorithm is AEAD.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是附加关联数据加解密。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg)                   \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PSS_BASE)

/**
 * @if Eng
 * @brief  Whether a key algorithm is RSA PSS any salt.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是RSA PSS任意盐值输入。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_RSA_PSS_ANY_SALT(alg)                                \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PSS_ANY_SALT_BASE)

/**
 * @if Eng
 * @brief  Whether a key algorithm is RSA PSS.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是RSA PSS。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_RSA_PSS(alg)                                 \
    (PSA_ALG_IS_RSA_PSS_STANDARD_SALT(alg) ||                   \
     PSA_ALG_IS_RSA_PSS_ANY_SALT(alg))

/**
 * @if Eng
 * @brief  Whether a key algorithm is RSA PKCS1V15 SIGN.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是RSA PKCS1V15签名。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)                               \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_RSA_PKCS1V15_SIGN_BASE)

#define PSA_ALG_IS_RSA_OAEP(alg)                                \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_RSA_OAEP_BASE)
/**
 * @if Eng
 * @brief  Whether a key algorithm is RSA PKCS1V15 SIGN.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是RSA PKCS1V15签名。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_ECDSA(alg)                                           \
    (((alg) & ~PSA_ALG_HASH_MASK & ~PSA_ALG_ECDSA_DETERMINISTIC_FLAG) ==  \
     PSA_ALG_ECDSA_BASE)

/**
 * @if Eng
 * @brief  Whether a key algorithm is HASH sign.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是HASH签名。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_SIGN_HASH(alg)                                       \
    (PSA_ALG_IS_RSA_PSS(alg) || PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||    \
     PSA_ALG_IS_ECDSA(alg))

/**
 * @if Eng
 * @brief  Whether a key algorithm is asymmetric encryption.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是非对称加解密。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg)                           \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION)

/**
 * @if Eng
 * @brief  Whether a key algorithm is key agreement.
 * @param  alg. see @ref psa_algorithm_t
 * @else
 * @brief  判断算法是否是密钥协商。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_IS_KEY_AGREEMENT(alg)                                   \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_KEY_AGREEMENT)

#define PSA_ALG_IS_HKDF(alg)                            \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_HKDF_BASE)

#define PSA_ALG_IS_PBKDF2_HMAC(alg)                                    \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_PBKDF2_HMAC_BASE)

#define PSA_ALG_IS_HKDF(alg)                            \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_HKDF_BASE)

#define PSA_ALG_IS_KEY_DERIVATION(alg)                                    \
    (PSA_ALG_IS_PBKDF2_HMAC(alg) || PSA_ALG_IS_HKDF(alg))

/**
 * @if Eng
 * @brief  AEAD algorithm with tag length.
 * @param  aead_alg. see @ref psa_algorithm_t
 * @param  tag_length.
 * @else
 * @brief  有标签长度的AEAD算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  标签长度。
 * @endif
 */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length)           \
    (((aead_alg) & ~(PSA_ALG_AEAD_TAG_LENGTH_MASK |                     \
                     PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG)) |         \
     ((tag_length) << PSA_AEAD_TAG_LENGTH_OFFSET &                      \
      PSA_ALG_AEAD_TAG_LENGTH_MASK))

/**
 * @if Eng
 * @brief  AEAD algorithm with minimum tag length.
 * @param  aead_alg. see @ref psa_algorithm_t
 * @param  min_tag_length.
 * @else
 * @brief  有最小标签长度的AEAD算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  标签长度。
 * @endif
 */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    (PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, min_tag_length) |            \
      PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG)

/**
 * @if Eng
 * @brief  Extract tag length from AEAD algorithm.
 * @param  aead_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从AEAD算法获取标签长度。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_ALG_AEAD_GET_TAG_LENGTH(aead_alg)                           \
    (((aead_alg) & PSA_ALG_AEAD_TAG_LENGTH_MASK) >>                     \
      PSA_AEAD_TAG_LENGTH_OFFSET)

/**
 * @if Eng
 * @brief  MAC algorithm with mac length.
 * @param  mac_alg. see @ref psa_algorithm_t
 * @param  mac_length.
 * @else
 * @brief  有认证码长度的MAC算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  认证码长度。
 * @endif
 */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length)              \
    (((mac_alg) & ~(PSA_ALG_MAC_TRUNCATION_MASK |               \
                    PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG)) |   \
     ((mac_length) << PSA_MAC_TRUNCATION_OFFSET & PSA_ALG_MAC_TRUNCATION_MASK))

/**
 * @if Eng
 * @brief  Extract mac length from MAC algorithm.
 * @param  aead_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从MAC算法获取认证码长度。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_MAC_TRUNCATED_LENGTH(mac_alg)                               \
    (((mac_alg) & PSA_ALG_MAC_TRUNCATION_MASK) >> PSA_MAC_TRUNCATION_OFFSET)

/**
 * @if Eng
 * @brief  MAC algorithm with minimum mac length.
 * @param  mac_alg. see @ref psa_algorithm_t
 * @param  min_mac_length.
 * @else
 * @brief  有最小MAC长度的MAC算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  认证码长度。
 * @endif
 */
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length)   \
    ( PSA_ALG_TRUNCATED_MAC(mac_alg, min_mac_length) |              \
      PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG)

/**
 * @if Eng
 * @brief  Extract pure AEAD algorithm from combination AEAD algorithm.
 * @param  aead_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从组合AEAD算法中获取纯AEAD算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  认证码长度。
 * @endif
 */
#define PSA_ALG_AEAD_GET_ALG(aead_alg)              (0x05500000 | ((aead_alg) & PSA_ALG_AEAD_ALG_MASK))

/**
 * @if Eng
 * @brief  Extract pure MAC algorithm from combination MAC algorithm.
 * @param  mac_alg. see @ref psa_algorithm_t
 * @else
 * @brief  从组合MAC算法中获取纯MAC算法。
 * @param  算法。参考 @ref psa_algorithm_t
 * @param  认证码长度。
 * @endif
 */
#define PSA_ALG_MAC_GET_ALG(mac_alg)    ((mac_alg) & 0x03c003ff)

/**
 * @if Eng
 * @brief  Maximum plaintext length.
 * @param  input_length.
 * @else
 * @brief  获取最大明文长度。
 * @param  算法。参考 @ref psa_algorithm_t
 * @endif
 */
#define PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE(input_length)    (input_length)

/**
 * @if Eng
 * @brief  PSA crypto init.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA crypto初始化。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_crypto_init(void);

/**
 * @if Eng
 * @brief  Set key type.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   type Key type.
 * @else
 * @brief  设置密钥类型。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   type 密钥类型。
 * @endif
 */
void psa_set_key_type(psa_key_attributes_t *attributes, psa_key_type_t type);

/**
 * @if Eng
 * @brief  Set key algorithm.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   alg Key algorithm.
 * @else
 * @brief  设置密钥算法。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   alg 密钥算法。
 * @endif
 */
void psa_set_key_algorithm(psa_key_attributes_t *attributes, psa_algorithm_t alg);

/**
 * @if Eng
 * @brief  Set key bits.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   bits Key bits.
 * @else
 * @brief  设置密钥比特长度。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   bits 密钥比特长度。
 * @endif
 */
void psa_set_key_bits(psa_key_attributes_t *attributes, size_t bits);

/**
 * @if Eng
 * @brief  Set key lifetime.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   bits Key lifetime.
 * @else
 * @brief  设置密钥生命周期。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   lifetime 密钥生命周期。
 * @endif
 */
void psa_set_key_lifetime(psa_key_attributes_t *attributes, psa_key_lifetime_t lifetime);

/**
 * @if Eng
 * @brief  Set key id.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   bits Key id.
 * @else
 * @brief  设置密钥标识符。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   id 密钥标识符。
 * @endif
 */
void psa_set_key_id(psa_key_attributes_t *attributes, psa_key_id_t key);

/**
 * @if Eng
 * @brief  Set key usage.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   bits Key usage.
 * @else
 * @brief  设置密钥用途。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   usage 密钥用途。
 * @endif
 */
void psa_set_key_usage_flags(psa_key_attributes_t *attributes, psa_key_usage_t usage_flags);

/**
 * @if Eng
 * @brief  Clear key attributes.
 * @param  [in]   attributes Key attributes.
 * @else
 * @brief  设置密钥属性。
 * @param  [in]   attributes 密钥属性。
 * @endif
 */
void psa_reset_key_attributes(psa_key_attributes_t *attributes);

/**
 * @if Eng
 * @brief  Get key attributes.
 * @param  [in]   key Key id.
 * @param  [out]  attributes Key attributes.
 * @else
 * @brief  获取密钥属性。
 * @param  [in]   key 密钥标识符。
 * @param  [out]  attributes 密钥属性。
 * @endif
 */
psa_status_t psa_get_key_attributes(psa_key_id_t key, psa_key_attributes_t *attributes);

/**
 * @if Eng
 * @brief  Import a key in binary format.
 * @param  [in]   attributes Key attributes.
 * @param  [in]   data Key data.
 * @param  [in]   data_length Key data length.
 * @param  [out]  key Key ID.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  以二进制格式导入密钥。
 * @param  [in]   attributes 密钥属性。
 * @param  [in]   data 密钥数据。
 * @param  [in]   data_length 密钥数据长度。
 * @param  [out]  key 密钥标识符。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_import_key(const psa_key_attributes_t *attributes, const uint8_t *data, size_t data_length,
    psa_key_id_t *key);

/**
 * @if Eng
 * @brief  Generate a key.
 * @param  [in]   attributes Key attributes.
 * @param  [out]  key Key ID.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  生成密钥。
 * @param  [in]   attributes 密钥属性。
 * @param  [out]  key 密钥标识符。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_generate_key(const psa_key_attributes_t *attributes, psa_key_id_t *key);

/**
 * @if Eng
 * @brief  Export public key.
 * @param  [in]   key Key ID.
 * @param  [out]  data Key data.
 * @param  [in]   data_size Key data buffer size.
 * @param  [out]  data_length Output key length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  导出私钥对应的公钥。
 * @param  [in]   key 密钥标识符.
 * @param  [out]  data 密钥数据.
 * @param  [in]   data_size 密钥缓冲区长度.
 * @param  [out]  data_length 输出密钥长度.
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_export_public_key(psa_key_id_t key, uint8_t *data, size_t data_size, size_t *data_length);

/**
 * @if Eng
 * @brief  Verify a key with crc32.
 * @param  [in]   key Key ID.
 * @param  [in]   crc32 CRC32 value of key.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  校验密钥的crc32。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   crc32 密钥的CRC32值。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_verify_key(psa_key_id_t key, uint32_t crc32);

/**
 * @if Eng
 * @brief  Destroy a key.
 * @param  [in]   key Key ID.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  销毁密钥。
 * @param  [in]   key 密钥标识符。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_destroy_key(psa_key_id_t key);

/**
 * @if Eng
 * @brief  Generate random bytes.
 * @param  [out]  output Random data.
 * @param  [in]   output_size Random data byte size.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  生成随机数。
 * @param  [out]  output 随机数。
 * @param  [in]   output_size 随机数字节长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_generate_random(uint8_t *output, size_t output_size);

/**
 * @if Eng
 * @brief  Set the IV for encryption; if not called, a random IV will be generated internally..
 * @param  [in]   operation Set IV operation。
 * @param  [in]   iv IV data。
 * @param  [in]   iv_length IV data byte size。
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  设置加密时的IV，不调用则内部生成随机IV。
 * @param  [in]   operation 设置IV操作。
 * @param  [in]   iv 随机数字节长度。
 * @param  [in]   iv_length 随机数字节长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation, const uint8_t *iv, size_t iv_length);

/**
 * @if Eng
 * @brief  Encrypt a message using a symmetric cipher.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Plain text.
 * @param  [in]   input_length Plain text length.
 * @param  [out]  output Cipher text.
 * @param  [in]   output_size Cipher text buffer length.
 * @param  [out]  output_length Output Cipher text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息进行对称加密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 明文。
 * @param  [in]   input_length 明文长度。
 * @param  [out]  output 密文。
 * @param  [in]   output_size 密文缓冲区长度。
 * @param  [out]  output_length 实际输出密文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_cipher_encrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  Decrypt a message using a symmetric cipher.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Cipher text.
 * @param  [in]   input_length Cipher text length.
 * @param  [out]  output Plain text.
 * @param  [in]   output_size Plain text buffer length.
 * @param  [out]  output_length Output Plain text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息进行对称解密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 密文。
 * @param  [in]   input_length 密文长度。
 * @param  [out]  output 明文。
 * @param  [in]   output_size 明文缓冲区长度。
 * @param  [out]  output_length 实际输出明文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_cipher_decrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  Process an authenticated encryption operation.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   nonce IV.
 * @param  [in]   nonce_length IV length.
 * @param  [in]   additional_data Additional data.
 * @param  [in]   additional_data_length Additional data length.
 * @param  [in]   plaintext Plain text.
 * @param  [in]   plaintext_length Plain text length.
 * @param  [out]  ciphertext Cipher text.
 * @param  [in]   ciphertext_size Cipher text buffer length.
 * @param  [out]  ciphertext_length Output cipher text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息使用附加关联数据进行加密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   nonce 随机数。
 * @param  [in]   nonce_length 随机数长度。
 * @param  [in]   additional_data 关联数据。
 * @param  [in]   additional_data_length 关联数据长度。
 * @param  [in]   plaintext 明文。
 * @param  [in]   plaintext_length 明文长度。
 * @param  [out]  ciphertext 密文。
 * @param  [in]   ciphertext_size 密文缓冲区长度。
 * @param  [out]  ciphertext_length 实际输出密文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_aead_encrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length, const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

/**
 * @if Eng
 * @brief  Process an authenticated decryption operation.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   nonce IV.
 * @param  [in]   nonce_length IV length.
 * @param  [in]   additional_data Additional data.
 * @param  [in]   additional_data_length Additional data length.
 * @param  [in]   ciphertext Cipher text.
 * @param  [in]   ciphertext_length Cipher text length.
 * @param  [out]  plaintext Plain text.
 * @param  [in]   plaintext_size Plain text buffer length.
 * @param  [out]  plaintext_length Output plain text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息使用附加关联数据进行解密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   nonce 随机数。
 * @param  [in]   nonce_length 随机数长度。
 * @param  [in]   additional_data 关联数据。
 * @param  [in]   additional_data_length 关联数据长度。
 * @param  [in]   ciphertext 密文。
 * @param  [in]   ciphertext_length 密文长度。
 * @param  [out]  plaintext 明文。
 * @param  [in]   plaintext_size 明文缓冲区长度。
 * @param  [out]  plaintext_length 实际输出明文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_aead_decrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length, const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length);

/**
 * @if Eng
 * @brief  Calculate the hash (digest) of a message.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Input text.
 * @param  [in]   input_length Input text length.
 * @param  [out]  hash hash text.
 * @param  [in]   hash_size Hash text buffer length.
 * @param  [out]  hash_length Output hash text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息计算摘要。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 输入消息。
 * @param  [in]   input_length 输入消息长度。
 * @param  [out]  hash 摘要。
 * @param  [in]   hash_size 摘要缓冲区长度。
 * @param  [out]  hash_length 实际输出摘要长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_hash_compute(psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *hash,
    size_t hash_size, size_t *hash_length);

/**
 * @if Eng
 * @brief  Calculate the MAC (message authentication code) of a message.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Input text.
 * @param  [in]   input_length Input text length.
 * @param  [out]  mac MAC text.
 * @param  [in]   mac_size MAC text buffer length.
 * @param  [out]  mac_length Output MAC text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段消息计算消息认证码。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 输入消息。
 * @param  [in]   input_length 输入消息长度。
 * @param  [out]  mac 消息认证码。
 * @param  [in]   mac_size 消息认证码缓冲区长度。
 * @param  [out]  mac_length 实际输出消息认证码长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_mac_compute(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length,
    uint8_t *mac, size_t mac_size, size_t *mac_length);

/**
 * @if Eng
 * @brief  Perform a key agreement and return the shared secret.
 * @param  [in]   private_key Input private key.
 * @param  [in]   peer_key Input public key.
 * @param  [in]   peer_key_length Input public key length.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   attributes Key attributes.
 * @param  [out]  key Key ID.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  密钥交换并返回共享密钥。
 * @param  [in]   private_key 输入私钥。
 * @param  [in]   peer_key Input 输入公钥。
 * @param  [in]   peer_key_length Input 输入公钥长度。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   attributes 密钥属性。
 * @param  [out]  key 密钥标识符。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_agreement(psa_key_id_t private_key, const uint8_t *peer_key, size_t peer_key_length,
    psa_algorithm_t alg, const psa_key_attributes_t *attributes, psa_key_id_t *key);

/**
 * @if Eng
 * @brief  Perform a key agreement and return the binary shared secret.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   private_key Input private key.
 * @param  [in]   peer_key Input public key.
 * @param  [in]   peer_key_length Input public key length.
 * @param  [out]  output Shared Key binary.
 * @param  [in]   output_size Key binary buffer size.
 * @param  [out]  output_length Key binary length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  密钥交换并返回二进制共享密钥。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   private_key 输入私钥。
 * @param  [in]   peer_key Input 输入公钥。
 * @param  [in]   peer_key_length Input 输入公钥长度。
 * @param  [out]  output 二进制共享密钥。
 * @param  [in]   output_size 共享密钥缓冲区长度。
 * @param  [out]  output_length 共享密钥长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_raw_key_agreement(psa_algorithm_t alg, psa_key_id_t private_key, const uint8_t *peer_key,
    size_t peer_key_length, uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  Sign a hash with a private key.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   hash Input hash.
 * @param  [in]   hash_length Input hash length.
 * @param  [out]  signature Output signature.
 * @param  [in]   signature_size Output signature buffer length.
 * @param  [out]  signature_length Output signature length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段摘要使用私钥进行签名。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   hash 输入摘要。
 * @param  [in]   hash_length 输入摘要长度。
 * @param  [out]  signature 输出签名。
 * @param  [in]   signature_size 输出签名缓冲区长度。
 * @param  [out]  signature_length 输出签名长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_sign_hash(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length);

/**
 * @if Eng
 * @brief  Verify a hash with a public key.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   hash Input hash.
 * @param  [in]   hash_length Input hash length.
 * @param  [in]   signature Input signature.
 * @param  [in]   signature_length Input signature length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段摘要使用公钥进行验签。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   hash 输入摘要。
 * @param  [in]   hash_length 输入摘要长度。
 * @param  [in]   signature 输入签名。
 * @param  [in]   signature_length 输入签名长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_verify_hash(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length);

/**
 * @if Eng
 * @brief  Encrypt a message with a public key.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Plain text.
 * @param  [in]   input_length Plain text length.
 * @param  [in]   salt Label.
 * @param  [in]   salt_length Label length.
 * @param  [out]  output Cipher text.
 * @param  [in]   output_size Cipher text buffer size.
 * @param  [out]  output_length Cipher text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段信息使用公钥进行加密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 输入明文。
 * @param  [in]   input_length 输入明文长度。
 * @param  [in]   salt 标签。
 * @param  [in]   salt_length 标签长度。
 * @param  [out]  output 输出密文。
 * @param  [in]   output_size 输出密文缓冲区长度。
 * @param  [out]  output_length 实际输出密文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_asymmetric_encrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length,
    const uint8_t *salt, size_t salt_length, uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  Decrypt a message with a private key.
 * @param  [in]   key Key ID.
 * @param  [in]   alg Key algorithm.
 * @param  [in]   input Cipher text.
 * @param  [in]   input_length Cipher text length.
 * @param  [in]   salt Label.
 * @param  [in]   salt_length Label length.
 * @param  [out]  output Plain text.
 * @param  [in]   output_size Plain text buffer size.
 * @param  [out]  output_length Plain text length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  对一段信息使用私钥进行解密。
 * @param  [in]   key 密钥标识符。
 * @param  [in]   alg 密钥算法。
 * @param  [in]   input 输入密文。
 * @param  [in]   input_length 输入密文长度。
 * @param  [in]   salt 标签。
 * @param  [in]   salt_length 标签长度。
 * @param  [out]  output 输出明文。
 * @param  [in]   output_size 输出明文缓冲区长度。
 * @param  [out]  output_length 实际输出明文长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_asymmetric_decrypt(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length,
    const uint8_t *salt, size_t salt_length, uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  PSA key derivation operation struct init.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生操作结构体初始化。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_key_derivation_operation_t psa_key_derivation_operation_init(void);

/**
 * @if Eng
 * @brief  PSA key derivation init.
 * @param  [in]   operation Key derivation operation.
 * @param  [in]   alg Key derivation algorithm.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生初始化。
 * @param  [in]   operation 密钥派生操作。
 * @param  [in]   alg 密钥派生算法。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_setup(psa_key_derivation_operation_t *operation, psa_algorithm_t alg);

/**
 * @if Eng
 * @brief  PSA key derivation deinit.
 * @param  [in]   operation Key derivation operation.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生初始化。
 * @param  [in]   operation 密钥派生操作。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_abort(psa_key_derivation_operation_t *operation);

/**
 * @if Eng
 * @brief  PSA key derivation set key capacity.
 * @param  [in]   operation Key derivation operation.
 * @param  [in]   capacity Key derivation key capacity.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  设置PSA 密钥派生结果容量。
 * @param  [in]   operation 密钥派生操作。
 * @param  [in]   capacity 密钥派生结果容量。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_set_capacity(psa_key_derivation_operation_t *operation, size_t capacity);

/**
 * @if Eng
 * @brief  PSA key derivation get key capacity.
 * @param  [in]   operation Key derivation operation.
 * @param  [out]  capacity Key derivation key capacity.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  获取PSA 密钥派生结果容量。
 * @param  [in]   operation 密钥派生操作。
 * @param  [out]  capacity 密钥派生结果容量。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t *operation, size_t *capacity);

/**
 * @if Eng
 * @brief  PSA key derivation parameters input.
 * @param  [in]   operation Key derivation operation.
 * @param  [in]   step Key derivation step.
 * @param  [in]   data Key derivation parameter buffer.
 * @param  [in]   data_length Key derivation parameter length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生参数输入。
 * @param  [in]   operation 密钥派生操作。
 * @param  [in]   step 密钥派生步骤。
 * @param  [in]   data 密钥派生参数。
 * @param  [in]   data_length 密钥派生参数长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_input_bytes(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step,
    const uint8_t *data, size_t data_length);

/**
 * @if Eng
 * @brief  PSA key derivation parameters input by key.
 * @param  [in]   operation Key derivation operation.
 * @param  [in]   step Key derivation step.
 * @param  [in]   key Key ID.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生参数以KEY形式输入。
 * @param  [in]   operation 密钥派生操作。
 * @param  [in]   step 密钥派生步骤。
 * @param  [in]   key 密钥标识符。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_input_key(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step,
    psa_key_id_t key);

/**
 * @if Eng
 * @brief  PSA key derivation parameters input by integer.
 * @param  [in]   operation Key derivation operation.
 * @param  [in]   step Key derivation step.
 * @param  [in]   value 64 bit unsigned int.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  PSA 密钥派生参数以整数形式输入。
 * @param  [in]   operation 密钥派生操作。
 * @param  [in]   step 密钥派生步骤。
 * @param  [in]   value 64位无符号整数。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_input_integer(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step,
    uint64_t value);

/**
 * @if Eng
 * @brief  PSA key derivation key output.
 * @param  [in]   operation Key derivation operation.
 * @param  [out]  output Key derivation key output.
 * @param  [in]   output_length Key derivation key length.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  获取PSA 密钥派生结果。
 * @param  [in]   operation 密钥派生操作。
 * @param  [out]  output 密钥派生结果。
 * @param  [in]   output_length 要获取的密钥派生结果长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_key_derivation_output_bytes(psa_key_derivation_operation_t *operation, uint8_t *output,
    size_t output_length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#include "psa/psa_pake.h"

#endif
