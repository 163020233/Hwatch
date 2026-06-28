/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.\n
 * Description: Implementation of the PSA Pake API. \n
 *
 * This file must be at the end of psa_crypto.h.
 */
#ifndef PSA_PAKE_H
#define PSA_PAKE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* PSA_ALG_SPAKE2P. */
#define PSA_ALG_SPAKE2P_HMAC(hash_alg) \
    ((psa_algorithm_t) (0x0a000400 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_JPAKE(hash_alg) \
    ((psa_algorithm_t) (0x0a000100 | ((hash_alg) & 0x000000ff)))

#define PSA_ALG_IS_PAKE(alg) \
    (((alg) & 0x7f000000) == 0x0a000000)

#define PSA_ALG_IS_SPAKE2P_HMAC(alg) \
    (((alg) & ~0x000000ff) == 0x0a000400)

#define PSA_ALG_IS_JPAKE(alg) \
    (((alg) & ~0x000000ff) == 0x0a000100)

/* PAKE_TYPE. */
typedef uint8_t psa_pake_primitive_type_t;
#define PSA_PAKE_PRIMITIVE_INVALID  ((psa_pake_primitive_type_t)0)
#define PSA_PAKE_PRIMITIVE_TYPE_ECC ((psa_pake_primitive_type_t)0x01)
#define PSA_PAKE_PRIMITIVE_TYPE_DH ((psa_pake_primitive_type_t)0x02)

/* PAKE_FAMILY. */
typedef uint8_t psa_pake_family_t;

/*
 * |-- PAKE_TYPE(8-bit) --|-- PAKE_FAMILY(8-bit)|-- PAKE_BITS(16-bit) --|
 */
typedef uint32_t psa_pake_primitive_t;
#define PSA_PAKE_PRIMITIVE(pake_type, pake_family, pake_bits)       \
    ((((pake_type) & 0xFF) << 24) | (((pake_family) & 0xFF) << 16) | ((pake_bits) & 0xFFFF))

#define PSA_PAKE_PRIMITIVE_GET_TYPE(pake_primitive)    ((pake_primitive) >> 24 & 0xFF)
#define PSA_PAKE_PRIMITIVE_GET_FAMILY(pake_primitive)    ((pake_primitive) >> 16 & 0xFF)
#define PSA_PAKE_PRIMITIVE_GET_BITS(pake_primitive)    ((psa_key_bits_t)((pake_primitive) & 0xFFFF))
#define PSA_PAKE_CONFIRMED_KEY  0
#define PSA_PAKE_UNCONFIRMED_KEY  1

/* pake cipher suite. */
typedef struct {
    psa_algorithm_t alg;
    psa_pake_primitive_t primitive;
    uint32_t key_confirmation;
} psa_pake_cipher_suite_t;

/* pake operation. */
typedef uint8_t psa_pake_role_t;
#define PSA_PAKE_ROLE_CLIENT        ((psa_pake_role_t)0x11)
#define PSA_PAKE_ROLE_SERVER        ((psa_pake_role_t)0x12)

typedef uint8_t psa_pake_step_t;
#define PSA_PAKE_STEP_KEY_SHARE     ((psa_pake_step_t)0x01)
#define PSA_PAKE_STEP_ZK_PUBLIC     ((psa_pake_step_t)0x02)
#define PSA_PAKE_STEP_ZK_PROOF      ((psa_pake_step_t)0x03)
#define PSA_PAKE_STEP_CONFIRM       ((psa_pake_step_t)0x04)

#define PSA_PAKE_ID_MAX_LEN 16

typedef struct {
    psa_key_id_t password_key;
    psa_pake_cipher_suite_t cipher_suite;
    uint8_t user_id[PSA_PAKE_ID_MAX_LEN];
    uint8_t peer_id[PSA_PAKE_ID_MAX_LEN];   /* 计算ZKP时需要 */
    uint8_t user_id_len;
    uint8_t peer_id_len;
    psa_pake_role_t role;
    uint8_t op_id;
} psa_pake_operation_t;

/* cipher_suite. */
#define PSA_PAKE_CIPHER_SUITE_INIT      psa_pake_cipher_suite_init()

/**
 * @if Eng
 * @brief  Pake cipher suite init.
 * @else
 * @brief  Pake 加密套件初始化。
 * @endif
 */
static inline psa_pake_cipher_suite_t psa_pake_cipher_suite_init(void)
{
    psa_pake_cipher_suite_t cipher_suite = { 0 };
    return cipher_suite;
}

/**
 * @if Eng
 * @brief  Pake cipher suite get algorithm.
 * @param  [in]   cipher_suite cipher suite.
 * @else
 * @brief  获取 pake 加密套件算法。
 * @param  [in]   cipher_suite 加密套件。
 * @endif
 */
static inline psa_algorithm_t psa_pake_cs_get_algorithm(const psa_pake_cipher_suite_t *cipher_suite)
{
    if (cipher_suite == NULL) {
        return PSA_ALG_NONE;
    }
    return cipher_suite->alg;
}

/**
 * @if Eng
 * @brief  Pake cipher suite set algorithm.
 * @param  [in]   cipher_suite cipher suite.
 * @param  [in]   alg algorithm.
 * @else
 * @brief  设置 pake 加密套件算法。
 * @param  [in]   cipher_suite 加密套件。
 * @param  [in]   alg 算法。
 * @endif
 */
static inline void psa_pake_cs_set_algorithm(psa_pake_cipher_suite_t *cipher_suite,
    psa_algorithm_t alg)
{
    if (cipher_suite == NULL) {
        return;
    }
    cipher_suite->alg = alg;
}

/**
 * @if Eng
 * @brief  Pake cipher suite get primitive.
 * @param  [in]   cipher_suite cipher suite.
 * @else
 * @brief  获取 pake 加密套件的算法属性。
 * @param  [in]   cipher_suite 加密套件。
 * @endif
 */
static inline psa_pake_primitive_t psa_pake_cs_get_primitive(const psa_pake_cipher_suite_t *cipher_suite)
{
    if (cipher_suite == NULL) {
        return PSA_PAKE_PRIMITIVE_INVALID;
    }
    return cipher_suite->primitive;
}

/**
 * @if Eng
 * @brief  Pake cipher suite set primitive.
 * @param  [in]   cipher_suite cipher suite.
 * @param  [in]   primitive primitive.
 * @else
 * @brief  设置 pake 加密套件的算法属性。
 * @param  [in]   cipher_suite 加密套件。
 * @param  [in]   primitive 算法属性。
 * @endif
 */
static inline void psa_pake_cs_set_primitive(psa_pake_cipher_suite_t *cipher_suite,
    psa_pake_primitive_t primitive)
{
    if (cipher_suite == NULL) {
        return;
    }
    cipher_suite->primitive = primitive;
}

/**
 * @if Eng
 * @brief  Pake cipher suite set key_confirmation.
 * @param  [in]   cipher_suite cipher suite.
 * @param  [in]   key_confirmation key_confirmation.
 * @else
 * @brief  设置 pake 加密套件的密钥确认标志。
 * @param  [in]   cipher_suite 加密套件。
 * @param  [in]   key_confirmation 密钥确认标志。
 * @endif
 */
static inline void psa_pake_cs_set_key_confirmation(psa_pake_cipher_suite_t *cipher_suite,
    uint32_t key_confirmation)
{
    if (cipher_suite == NULL) {
        return;
    }
    cipher_suite->key_confirmation = key_confirmation;
}

/**
 * @if Eng
 * @brief  Pake operation init.
 * @else
 * @brief  Pake 操作初始化。
 * @endif
 */
psa_pake_operation_t psa_pake_operation_init(void);

/**
 * @if Eng
 * @brief  Pake operation setup password and cipher suite.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   password_key password key id.
 * @param  [in]   cipher_suite cipher suite.
 * @else
 * @brief  Pake 操作设置密码和加密套件。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   password_key password 密钥ID.
 * @param  [in]   cipher_suite 加密套件.
 * @endif
 */
psa_status_t psa_pake_setup(psa_pake_operation_t *operation, psa_key_id_t password_key,
    const psa_pake_cipher_suite_t *cipher_suite);

/**
 * @if Eng
 * @brief  Pake operation set role.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   role role.
 * @else
 * @brief  Pake 操作设置角色。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   role 角色。
 * @endif
 */
psa_status_t psa_pake_set_role(psa_pake_operation_t *operation, psa_pake_role_t role);

/**
 * @if Eng
 * @brief  Set the user ID for a password-authenticated key exchange.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   user_id The user ID to authenticate with.
 * @param  [in]   user_id_len Size of the user_id buffer in bytes.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t
 * @else
 * @brief  为口令认证密钥交换设置用户 ID。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   user_id 待验证的用户身份标识。
 * @param  [in]   user_id_len user_id 缓冲区的字节长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_pake_set_user(psa_pake_operation_t *operation, const uint8_t *user_id, size_t user_id_len);

/**
 * @if Eng
 * @brief  Set the peer ID for a password-authenticated key exchange.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   peer_id The peer’s ID to authenticate.
 * @param  [in]   peer_id_len Size of the peer_id buffer in bytes.
 * @retval PSA_SUCCESS Success.
 * @retval Other        Failure. For details, see @ref psa_status_t
 * @else
 * @brief  为口令认证密钥交换设置对端 ID。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   peer_id 待验证的对端身份标识。
 * @param  [in]   peer_id_len peer_id 缓冲区的字节长度。
 * @retval PSA_SUCCESS 成功。
 * @retval Other        失败，参考 @ref psa_status_t 。
 * @endif
 */
psa_status_t psa_pake_set_peer(psa_pake_operation_t *operation, const uint8_t *peer_id, size_t peer_id_len);

/**
 * @if Eng
 * @brief  Pake operation compute output data.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   step step.
 * @param  [in]   output output data.
 * @param  [in]   output_size output data capacity in byte.
 * @param  [out]  output_length output data real length in byte.
 * @else
 * @brief  Pake 操作计算输出数据。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   step 步骤.
 * @param  [in]   output 输出数据.
 * @param  [in]   output_size 输出数据的buf容量，单位是byte。
 * @param  [out]  output_length 输出数据的实际长度，单位是byte。
 * @endif
 */
psa_status_t psa_pake_output(psa_pake_operation_t *operation, psa_pake_step_t step,
    uint8_t *output, size_t output_size, size_t *output_length);

/**
 * @if Eng
 * @brief  Pake operation set input data.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   step step.
 * @param  [in]   input input data.
 * @param  [in]   input_length input data length in byte.
 * @else
 * @brief  Pake 操作设置输入数据。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   step 步骤.
 * @param  [in]   input 输入数据.
 * @param  [in]   input_length 输出数据的大小，单位是byte。
 * @endif
 */
psa_status_t psa_pake_input(psa_pake_operation_t *operation, psa_pake_step_t step,
    const uint8_t *input, size_t input_length);

/**
 * @if Eng
 * @brief  Pake operation get shared key.
 * @param  [in]   operation pake operation handle.
 * @param  [in]   attributes key attributes.
 * @param  [out]  key shared key id.
 * @else
 * @brief  Pake 操作获取共享密钥。
 * @param  [in]   operation pake 操作句柄。
 * @param  [in]   attributes 密钥属性。
 * @param  [out]  key 共享密钥ID。
 * @endif
 */
psa_status_t psa_pake_get_shared_key(psa_pake_operation_t *operation,
    const psa_key_attributes_t *attributes, psa_key_id_t *key);

/**
 * @if Eng
 * @brief  Pake abort operation.
 * @param  [in]   operation pake operation handle.
 * @else
 * @brief  Pake 终止操作。
 * @param  [in]   operation pake 操作句柄。
 * @endif
 */
psa_status_t psa_pake_abort(psa_pake_operation_t *operation);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
