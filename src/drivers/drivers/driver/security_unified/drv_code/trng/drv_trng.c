/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: cipher driver trng. \n
 *
 * History: \n
 * 2025-06-12, Create file. \n
 */
#include "drv_trng.h"
#include "hal_trng_romable.h"

#include "crypto_drv_common.h"

int32_t drv_cipher_trng_get_random(uint32_t *randnum)
{
    crypto_chk_return(randnum == NULL, ERROR_PARAM_IS_NULL, "randnum is NULL\n");
    return hal_trng_get_random_romable(randnum);
}

int32_t drv_cipher_trng_get_multi_random(uint32_t size, uint8_t *randnum)
{
    int32_t ret;
    uint32_t cnt;
    uint32_t i;
    uint32_t randnum_once;
    uint32_t tail;

    crypto_chk_return(randnum == TD_NULL, ERROR_PARAM_IS_NULL, "randnum is NULL\n");

    cnt = size / CRYPTO_WORD_WIDTH;
    for (i = 0; i < cnt; i++) {
        ret = hal_trng_get_random_romable(&randnum_once);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

        ret = memcpy_s(randnum + i * CRYPTO_WORD_WIDTH, size - i * CRYPTO_WORD_WIDTH,
            &randnum_once, CRYPTO_WORD_WIDTH);
        crypto_chk_return(ret != EOK, ERROR_MEMCPY_S);
    }

    /* less then 4 byte */
    tail = size - cnt * CRYPTO_WORD_WIDTH;
    if (tail != 0) {
        ret = hal_trng_get_random_romable(&randnum_once);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

        ret = memcpy_s(randnum + cnt * CRYPTO_WORD_WIDTH, tail, &randnum_once, tail);
        crypto_chk_return(ret != EOK, ERROR_MEMCPY_S);
    }
    return CRYPTO_SUCCESS;
}
