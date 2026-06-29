/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap ffdh code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "ffdh_param.h"
#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "unified_cipher_pke.h"

psa_status_t psa_driver_wrapper_ffdh(psa_key_bits_t bits, const uint8_t *private_key,
    uint32_t private_key_length, const uint8_t *public_key, uint32_t public_key_length, uint8_t *shared_key,
    uint32_t shared_key_length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;
    uapi_drv_cipher_pke_data_t mod_n;
    uapi_drv_cipher_pke_data_t input_priv_key = {.length = private_key_length, .data = (uint8_t *)private_key};
    uapi_drv_cipher_pke_data_t input_pub_key = {.length = public_key_length, .data = (uint8_t *)public_key};
    uapi_drv_cipher_pke_data_t output_shared_key = {.length = shared_key_length, .data = shared_key};

    switch (bits) {
#ifdef CONFIG_FFDH_2048_SUPPORT
        case PSA_CRYPTO_RSA_2048_BIT:
            mod_n.length = PSA_CRYPTO_RSA_2048_BIT / PSA_BYTES_IN_BIT;
            mod_n.data = (uint8_t *)g_ffdh_2048.p;
            break;
#endif
#ifdef CONFIG_FFDH_3072_SUPPORT
        case PSA_CRYPTO_RSA_3072_BIT:
            mod_n.length = PSA_CRYPTO_RSA_3072_BIT / PSA_BYTES_IN_BIT;
            mod_n.data = (uint8_t *)g_ffdh_3072.p;
            break;
#endif
#ifdef CONFIG_FFDH_4096_SUPPORT
        case PSA_CRYPTO_RSA_4096_BIT:
            mod_n.length = PSA_CRYPTO_RSA_4096_BIT / PSA_BYTES_IN_BIT;
            mod_n.data = (uint8_t *)g_ffdh_4096.p;
            break;
#endif
        default:
            return PSA_ERROR_INVALID_ARGUMENT;
    }

    ret = uapi_drv_cipher_pke_dh_compute_key(&mod_n, &input_priv_key, &input_pub_key, &output_shared_key);
    return psa_get_wrap_errcode(ret);
}