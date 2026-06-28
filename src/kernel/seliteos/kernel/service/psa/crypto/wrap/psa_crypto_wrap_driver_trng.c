/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap trng code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "trng.h"

psa_status_t psa_driver_wrapper_generate_trng(uint8_t *data, uint32_t length)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    ret = uapi_drv_cipher_trng_get_random_bytes(data, length);
    return psa_get_wrap_errcode(ret);
}
