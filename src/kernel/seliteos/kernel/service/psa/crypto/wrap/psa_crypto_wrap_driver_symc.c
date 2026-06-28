/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap symc code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "security_symc.h"
#include "security_mac.h"

const psa_crypto_table_item_pro g_cipher_alg_work_mode[] = {
    {PSA_KEY_TYPE_AES, PSA_ALG_ECB_NO_PADDING, UAPI_DRV_SYMC_AES_ECB_ALG},
    {PSA_KEY_TYPE_AES, PSA_ALG_CBC_NO_PADDING, UAPI_DRV_SYMC_AES_CBC_ALG},
    {PSA_KEY_TYPE_AES, PSA_ALG_CTR, UAPI_DRV_SYMC_AES_CTR_ALG},
    {PSA_KEY_TYPE_SM4, PSA_ALG_ECB_NO_PADDING, UAPI_DRV_SYMC_SM4_ECB_ALG},
    {PSA_KEY_TYPE_SM4, PSA_ALG_CBC_NO_PADDING, UAPI_DRV_SYMC_SM4_CBC_ALG},
    {PSA_KEY_TYPE_SM4, PSA_ALG_CTR, UAPI_DRV_SYMC_SM4_CTR_ALG},
};

#ifdef SECURITY_ENHANCEMENT
#include "trng.h"
#include "security_enhancement.h"

#define MAX_FALSE_COMPUTE_COUNT 3

static psa_status_t aes_loop_compute(uint32_t times, uint32_t combine_alg, const uint8_t *input, uint8_t *output,
    uint32_t data_len, uint8_t iv[16], const uint8_t *key, uint32_t single_key_len, bool is_encrypt)
{
    uint32_t i;
    errcode_t ret = ERRCODE_FAIL;
    uint8_t tmp_iv[16];  // 16: iv length

    (void)memcpy_s(tmp_iv, sizeof(tmp_iv), iv, 16); // 16: iv length
    for (i = 0; i < times; i++) {
        tee_random_delay();
        ret = uapi_drv_cipher_symc_crypt(combine_alg, input, output, data_len, tmp_iv, key + i * single_key_len,
            single_key_len, UAPI_DRV_INVALID_KEY_SLOT, is_encrypt);
        psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
            "uapi_drv_cipher_symc_crypt error!\n");
    }

    return psa_get_wrap_errcode(ret);
}

static psa_status_t aes_random_compute(uint32_t combine_alg, const uint8_t *input, uint8_t *output, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, bool is_encrypt)
{
    psa_status_t psa_ret = PSA_INTERNAL_FAILURE;
    errcode_t ret = ERRCODE_FAIL;
    uint32_t i;
    uint8_t *false_key = NULL;
    uint8_t times[2]; /* 2: two bytes random data */
    uint8_t times1;
    uint8_t times2;

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

    psa_ret = aes_loop_compute(times1, combine_alg, input, output, data_len, iv, false_key, key_len, is_encrypt);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret, "aes_loop_compute error!\n");

    psa_ret = aes_loop_compute(1, combine_alg, input, output, data_len, iv, key, key_len, is_encrypt);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret, "aes_loop_compute error!\n");

    psa_ret = aes_loop_compute(times2, combine_alg, input, (uint8_t *)input, data_len, iv, false_key + key_len * times1,
        key_len, is_encrypt);
    psa_crypto_chk_goto_with_ret(psa_ret, psa_ret != PSA_INTERNAL_SUCCESS, free, psa_ret, "aes_loop_compute error!\n");

free:
    psa_crypto_free(false_key);
    return psa_ret;
}
#endif

psa_status_t psa_driver_wrapper_cipher_crypto(psa_key_type_t type, psa_algorithm_t alg, uint8_t *key_data,
    uint32_t key_len, const uint8_t *input, uint8_t *output, uint32_t data_len, uint8_t iv[16], bool is_encrypt)
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;
    uint32_t combine_alg;
    uint8_t local_iv[PSA_CIPHER_IV_LEN];

    ret = memcpy_s(local_iv, PSA_CIPHER_IV_LEN, iv, PSA_CIPHER_IV_LEN);
    psa_crypto_chk_return(ret != EOK, ret, "memcpy_s error!\n");

    ret = psa_crypto_get_value_by_2index(g_cipher_alg_work_mode, psa_crypto_array_size(g_cipher_alg_work_mode),
        type, alg, &combine_alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_index error!\n");

#ifdef SECURITY_ENHANCEMENT
    ret = aes_random_compute(combine_alg, input, output, data_len, local_iv, key_data, key_len, is_encrypt);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "aes_random_compute error!\n");
#else
    ret = uapi_drv_cipher_symc_crypt(combine_alg, input, output, data_len, local_iv, key_data, key_len,
        UAPI_DRV_INVALID_KEY_SLOT, is_encrypt);
    psa_crypto_chk_return(psa_get_wrap_errcode(ret) != PSA_INTERNAL_SUCCESS, psa_get_wrap_errcode(ret),
        "uapi_drv_cipher_symc_crypt error!\n");
#endif

    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_driver_wrapper_cmac(uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t data_len,
    uint8_t mac[16])
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;

    ret = uapi_drv_cipher_cmac(UAPI_DRV_CIPHER_SYMC_ALG_AES, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, input,
        data_len, mac);
    return psa_get_wrap_errcode(ret);
}

psa_status_t psa_driver_wrapper_cbc_mac(uint8_t *key_data, uint32_t key_len, const uint8_t *input, uint32_t data_len,
    uint8_t mac[16])
{
    volatile_sec int32_t ret = PSA_INTERNAL_FAILURE;

    ret = uapi_drv_cipher_cbc_mac(UAPI_DRV_CIPHER_SYMC_ALG_AES, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, input,
        data_len, mac);
    return psa_get_wrap_errcode(ret);
}

const psa_crypto_table_item g_cipher_alg[] = {
    {PSA_KEY_TYPE_AES, UAPI_DRV_CIPHER_SYMC_ALG_AES},
    {PSA_KEY_TYPE_SM4, UAPI_DRV_CIPHER_SYMC_ALG_SM4},
};

psa_status_t psa_driver_wrapper_aead_crypto(psa_key_type_t type, psa_algorithm_t alg, uint8_t *key_data,
    uint32_t key_len, const uint8_t *nonce, uint32_t nonce_length, const uint8_t *additional_data,
    uint32_t additional_data_length, const uint8_t *input, uint8_t *output, uint32_t data_length,
    uint8_t *tag, uint32_t tag_length, bool is_encrypt)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_symc_alg_t drv_alg;

    ret = psa_crypto_get_value_by_index(g_cipher_alg, psa_crypto_array_size(g_cipher_alg), type, &drv_alg);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_index error!\n");

    if (is_encrypt) {
        if (alg == PSA_ALG_CCM) {
            ret = uapi_drv_cipher_symc_ccm_encrypt(drv_alg, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, nonce,
                nonce_length, additional_data, additional_data_length, input, output, data_length, tag, tag_length);
        } else {    /* PSA_ALG_GCM */
            ret = uapi_drv_cipher_symc_gcm_encrypt(drv_alg, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, nonce,
                nonce_length, additional_data, additional_data_length, input, output, data_length, tag, tag_length);
        }
    } else {        /* decrypt */
        if (alg == PSA_ALG_CCM) {
            ret = uapi_drv_cipher_symc_ccm_decrypt_verify(drv_alg, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, nonce,
                nonce_length, additional_data, additional_data_length, input, output, data_length, tag, tag_length);
        } else {    /* PSA_ALG_GCM */
            ret = uapi_drv_cipher_symc_gcm_decrypt_verify(drv_alg, key_data, key_len, UAPI_DRV_INVALID_KEY_SLOT, nonce,
                nonce_length, additional_data, additional_data_length, input, output, data_length, tag, tag_length);
        }
    }
    return psa_get_wrap_errcode(ret);
}