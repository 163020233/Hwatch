/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal trng romable header. \n
 *
 * History: \n
 * 2025-06-12, Create file. \n
 */
#ifndef HAL_TRNG_ROMABLE_H
#define HAL_TRNG_ROMABLE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void (*trng_debug_func)(void);

typedef struct {
    trng_debug_func             debug_func;
} hal_trng_func_table_t;

int32_t hal_trng_get_random_romable(uint32_t *randnum);

void hal_trng_debug_romable(void);

void hal_trng_register_romable(const hal_trng_func_table_t *func_table);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif