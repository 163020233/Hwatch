/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap hash code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "security_hash.h"
#include "security_hmac.h"
#include "km.h"
#ifdef SECURITY_ENHANCEMENT
#include "trng.h"
#include "security_enhancement.h"
#endif

psa_status_t psa_driver_wrapper_cipher_hash_compute(psa_algorithm_t alg, const uint8_t *input, uint32_t input_length,
    uint8_t *hash)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    if (alg == PSA_ALG_SHA_256) {
        ret = uapi_drv_cipher_sha256(input, input_length, hash);
    } else {
        ret = uapi_drv_cipher_sm3(input, input_length, hash);
    }
    return psa_get_wrap_errcode(ret);
}

#ifdef SECURITY_ENHANCEMENT
#define MAX_FALSE_COMPUTE_COUNT 3

static psa_status_t hmac_sha256_loop_compute(uint32_t times, uint8_t *false_key, uint32_t single_key_len,
    const uint8_t *input, uint32_t data_len, uint8_t output[32])
{
    uint32_t i;
    errcode_t ret = ERRCODE_FAIL;

    for (i = 0; i < times; i++) {
        tee_random_delay();
        ret = uapi_drv_cipher_hmac_sha256(input, data_len, false_key + i * single_key_len, single_key_len,
            UAPI_DRV_INVALID_KEY_SLOT, output);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_hmac_sha256 error!\n");
    }
    return psa_get_wrap_errcode(ret);
}

static psa_status_t hmac_sha256_random_compute(const uint8_t *input, uint32_t data_len, uint8_t *key_data,
    uint32_t key_len, uint8_t mac[32])
{
    volatile_sec psa_status_t psa_ret = PSA_INTERNAL_FAILURE;
    errcode_t ret = ERRCODE_FAIL;
    uint8_t times[2]; /* 2: two bytes random data */
    uint8_t times1;
    uint8_t times2;
    uint8_t *false_key = NULL;
    uint8_t false_mac[32]; /* 32: max mac length */

    ret = uapi_drv_cipher_trng_get_random_bytes(times, sizeof(times));
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_trng_get_random_bytes error!\n");

    times1 = times[0] % MAX_FALSE_COMPUTE_COUNT + 1;
    times2 = times[1] % MAX_FALSE_COMPUTE_COUNT + 1;
    false_key = psa_crypto_malloc(key_len * (times1 + times2));
    psa_crypto_chk_return(false_key == NULL, PSA_ERROR_INSUFFICIENT_MEMORY, "psa_crypto_malloc error!\n");

    ret = uapi_drv_cipher_trng_get_random_bytes(false_key, key_len * (times1 + times2));
    psa_crypto_chk_goto_with_ret(psa_ret, psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, free,
        psa_get_wrap_errcode(ret), "uapi_drv_cipher_trng_get_random_bytes error!\n");

    /* false key, loop times1 */
    psa_ret = hmac_sha256_loop_compute(times1, false_key, key_len, input, data_len, false_mac);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret,
        "hmac_sha256_loop_compute error!\n");

    /* real key */
    psa_ret = hmac_sha256_loop_compute(1, key_data, key_len, input, data_len, mac);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret,
        "hmac_sha256_loop_compute error!\n");

    /* false key, loop times2 */
    psa_ret = hmac_sha256_loop_compute(times2, false_key + key_len * times1, key_len, input, data_len, false_mac);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret,
        "hmac_sha256_loop_compute error!\n");

free:
    psa_crypto_free(false_key);
    return psa_ret;
}
#endif

psa_status_t psa_driver_wrapper_hmac(psa_algorithm_t alg, uint8_t *key_data, uint32_t key_len,
    const uint8_t *input, uint32_t data_len, uint8_t mac[32])
{
    volatile_sec psa_status_t psa_ret = PSA_INTERNAL_FAILURE;
    errcode_t ret = ERRCODE_FAIL;

    if (alg == PSA_ALG_HMAC(PSA_ALG_SHA_256)) {
#ifdef SECURITY_ENHANCEMENT
        psa_ret = hmac_sha256_random_compute(input, data_len, key_data, key_len, mac);
        psa_crypto_chk_return(psa_ret != PSA_INTERNAL_SUCCESS, psa_ret, "uapi_drv_cipher_hmac_sm3 error!\n");
#else
        ret = uapi_drv_cipher_hmac_sha256(input, data_len, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, mac);
        psa_ret = psa_get_wrap_errcode(ret);
        psa_crypto_chk_return(psa_ret != PSA_INTERNAL_SUCCESS, psa_ret, "uapi_drv_cipher_hmac_sha256 error!\n");
#endif
    } else {    /* PSA_ALG_HMAC(PSA_ALG_SM3) */
        ret = uapi_drv_cipher_hmac_sm3(input, data_len, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, mac);
        psa_ret = psa_get_wrap_errcode(ret);
        psa_crypto_chk_return(psa_ret != PSA_INTERNAL_SUCCESS, psa_ret, "uapi_drv_cipher_hmac_sm3 error!\n");
    }

    return psa_ret;
}
