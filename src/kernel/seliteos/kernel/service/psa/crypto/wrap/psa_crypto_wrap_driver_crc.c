/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap crc code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "security_crc.h"

psa_status_t psa_driver_wrapper_driver_crc32(const uint8_t *data, uint32_t length, uint32_t *crc32)
{
    volatile_sec psa_status_t ret = PSA_INTERNAL_FAILURE;

    ret = uapi_drv_cipher_crc32_compute(data, length, crc32);
    return psa_get_wrap_errcode(ret);
}