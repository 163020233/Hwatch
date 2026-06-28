/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal hash romable header. \n
 *
 * History: \n
 * 2025-03-07, Create file. \n
 */
#ifndef HAL_HASH_ROMABLE_H
#define HAL_HASH_ROMABLE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CRYPTO_HASH_MAX_DEPTH       2

typedef struct {
    /* word0. */
    uint32_t hash_first_node : 1;
    uint32_t hash_last_node : 1;
    uint32_t reserved_0 : 30;       // reserve
    /* word1. */
    uint32_t hash_alg_length;       // hash message length
    /* word2. */
    uint32_t hash_start_addr_l;     // hash message address
    /* word3. */
    uint32_t hash_start_addr_h : 4; // hash message address
    uint32_t reserved_1 : 28;       // reserve
} hal_hash_entry_in_t;

int32_t hal_hash_chn_init_romable(uint32_t chn_num, hal_hash_entry_in_t entry_in[CRYPTO_HASH_MAX_DEPTH]);

void hal_hash_chn_deinit_romable(uint32_t chn_num);

enum {
    HAL_SHA1_ALG        = 0xA0000,
    HAL_SHA224_ALG      = 0xB0000,
    HAL_SHA256_ALG      = 0x1B0000,
    HAL_SHA384_ALG      = 0x2B0000,
    HAL_SHA512_ALG      = 0x3B0000,
    HAL_SM3_ALG         = 0xC0000,
};

int32_t hal_hash_compute_romable(uint32_t hash_alg,
    uint64_t src, uint32_t len,
    const uint32_t *state_in, uint32_t *state_out, uint32_t state_len);

int32_t hal_hash_try_alloc_chn_romable(uint32_t *chn_num);

void hal_hash_free_chn_romable(uint32_t chn_num);

int32_t hal_hash_wait_done_romable(uint32_t chn_num);

bool hal_hash_done_try_romable(uint32_t chn_num);

void hal_hash_debug_romable(uint32_t chn_num);

typedef int32_t (*hash_try_alloc_chn_func)(uint32_t *chn_num);
typedef void (*hash_free_chn_func)(uint32_t chn_num);
typedef int32_t(*hash_wait_done_func)(uint32_t chn_num);
typedef void (*hash_debug_func)(uint32_t chn_num);

typedef struct {
    hash_try_alloc_chn_func     try_alloc_chn_func;
    hash_free_chn_func          free_chn_func;
    hash_wait_done_func         wait_done_func;
    hash_debug_func             debug_func;
} hal_hash_func_table_t;

void hal_hash_register_romable(const hal_hash_func_table_t *func_table);

void hal_hash_debug_default(uint32_t chn_num);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
