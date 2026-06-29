/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver trng header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_TRNG_H
#define DRV_TRNG_H

#include "crypto_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int32_t drv_cipher_trng_get_random(uint32_t *randnum);

int32_t drv_cipher_trng_get_multi_random(uint32_t size, uint8_t *randnum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif