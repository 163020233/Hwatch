/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides trng driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */

#include "trng.h"
#include "drv_trng.h"

#include "sl_common.h"
#include "crypto_drv_common.h"

errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum)
{
    crypto_service_preprocess(TRNG_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_trng_get_random(randnum);
    hal_trng_close_clk();
    crypto_service_postprocess(TRNG_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size)
{
    crypto_service_preprocess(TRNG_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_trng_get_multi_random(size, randnum);
    hal_trng_close_clk();
    crypto_service_postprocess(TRNG_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}