/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: attestation API header. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#ifndef __INITIAL_ATTESTATION_H__
#define __INITIAL_ATTESTATION_H__

#include "psa/psa_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSA_INITIAL_ATTEST_API_VERSION_MAJOR 1
#define PSA_INITIAL_ATTEST_API_VERSION_MINOR 0

#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 (32u)

typedef enum psa_initial_attest_alg {
    PSA_INITIAL_ATTEST_SIGN_ALGORITHM_ES256 = 1,
    PSA_INITIAL_ATTEST_SIGN_ALGORITHM_ES384 = 2,
    PSA_INITIAL_ATTEST_SIGN_ALGORITHM_ES512 = 3
} psa_initial_attest_alg_t;

/**
 * @if Eng Generate ecc key pair
 * @brief
 * @param  [in]  alg         ECC algrithem of key.
 * @param  [out] public_key  Public key buffer.
 * @param  [out] key_size    Public key buffer size.
 * @param  [in]  key_length  Public key out buffer size.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  生成ECC密钥对
 * @param  [in]  alg         密钥对应算法.
 * @param  [out] public_key  公钥的数据缓存区
 * @param  [out] key_size    公钥的数据缓存区大小
 * @param  [in]  key_length  公钥输出的实际大小
 * @retval 错误码参考 @ref psa_status_t.
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */

psa_status_t psa_initial_attest_generate_key(uint32_t alg, uint8_t *public_key, size_t key_size, size_t *key_length);

/**
 * @if Eng  Get initial attestation token
 * @brief
 * param [in]     challenge_buf     Challenge buffer for attestation.
 * param [in]     challenge_size    Challenge buffer size.
 * param [out]    token_buf         Token buffer.
 * param [in]     token_buf_size    Token buffer size.
 * param [out]    token_size        Token buffer out size.
 * @retval PSA_SUCCESS Success
 * @retval Other Failure. For details, see @ref psa_status_t.
 * @else
 * @brief  获取初始安全证明标识符
 * param [in]     challenge_buf     安全证明的挑战数据缓存区
 * param [in]     challenge_size    安全证明的挑战数据缓存区的大小
 * param [out]    token_buf         标识符数据缓存区
 * param [in]     token_buf_size    标识符数据缓存区大小
 * param [out]    token_size        标识符实际输出大小
 * @retval PSA_SUCCESS 成功
 * @retval Other 失败， 参考 @ref psa_status_t.
 * @endif
 */
psa_status_t psa_initial_attest_get_token(const void *challenge_buf, size_t challenge_size, void *token_buf,
                                          size_t token_buf_size, size_t *token_size);

#ifdef __cplusplus
}
#endif

#endif /* __INITIAL_ATTESTATION__ */
