/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls entropy alt implement.
 *
*/
#include "entropy_poll.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"

#include "trng.h"

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    errcode_t errcode;
    (void)data;
    if (output == NULL || olen == NULL) {
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }

    errcode = uapi_drv_cipher_trng_get_random_bytes(output, len);
    if (errcode != ERRCODE_SUCC) {
        mbedtls_printf("uapi_drv_cipher_trng_get_random_bytes failed, errcode is 0x%x\n", errcode);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    *olen = len;
    return 0;
}
