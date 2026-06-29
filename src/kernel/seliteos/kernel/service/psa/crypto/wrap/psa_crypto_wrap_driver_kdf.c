/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap kdf code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "cipher.h"

psa_status_t psa_driver_wrapper_pbkdf2(psa_algorithm_t alg, const uint8_t *password, uint32_t password_length,
    const uint8_t *salt, uint32_t salt_length, uint16_t count, uint8_t *output, uint32_t output_length)
{
    psa_unused(alg);
    psa_unused(password);
    psa_unused(password_length);
    psa_unused(salt);
    psa_unused(salt_length);
    psa_unused(count);
    psa_unused(output);
    psa_unused(output_length);
    return PSA_ERROR_NOT_SUPPORTED;
}

static const psa_crypto_table_item g_kdf_hmac[] = {
    {PSA_ALG_SHA_256, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA256},
    {PSA_ALG_SM3, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SM3},
};

psa_status_t psa_driver_wrapper_hkdf(psa_algorithm_t alg, const uint8_t *salt, uint32_t salt_length,
    const uint8_t *ikm, uint32_t ikm_length, const uint8_t *info, uint32_t info_length, uint8_t *output,
    uint32_t output_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_hkdf_t param;

    ret = psa_crypto_get_value_by_index(g_kdf_hmac, psa_crypto_array_size(g_kdf_hmac), PSA_ALG_GET_HASH(alg),
        &param.hmac_type);
    psa_crypto_chk_return(ret != PSA_INTERNAL_SUCCESS, ret, "psa_crypto_get_value_by_index error!\n");

    param.salt = (uint8_t *)salt;
    param.salt_length = salt_length;
    param.ikm = (uint8_t *)ikm;
    param.ikm_length = ikm_length;
    param.info = (uint8_t *)info;
    param.info_length = info_length;

    ret = uapi_drv_cipher_hkdf(&param, output, output_length);
    return psa_get_wrap_errcode(ret);
}