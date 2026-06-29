/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal symc romable header. \n
 *
 * History: \n
 * 2025-03-07, Create file. \n
 */
#ifndef HAL_SYMC_ROMABLE_H
#define HAL_SYMC_ROMABLE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CRYPTO_SYMC_MAX_DEPTH		    2
#define CRYPTO_AES_IV_SIZE_IN_WORD      4

/* ! spacc symc int32_t entry struct which is defined by hardware, you can't change it */
typedef struct {
    uint32_t sym_first_node : 1;         /* !<  Indicates whether the node is the first node */
    uint32_t sym_last_node : 1;          /* !<  Indicates whether the node is the last node */
    uint32_t rev1 : 7;                   /* !<  reserve */
    uint32_t odd_even : 1;               /* !<  Indicates whether the key is an odd key or even key */
    uint32_t rev2 : 22;                  /* !<  reserve */
    uint32_t sym_alg_length;             /* !<  symc data length */
    uint32_t sym_start_addr;             /* !<  symc start low addr */
    uint32_t sym_start_high;             /* !<  symc start high addr */
    uint32_t iv[CRYPTO_AES_IV_SIZE_IN_WORD]; /* !<  symc IV */
} hal_symc_entry_in_t;
/* ! spacc symc out entry struct which is defined by hardware, you can't change it */
typedef struct {
    uint32_t rev1;           /* !<  reserve */
    uint32_t sym_alg_length; /* !<  syma data length */
    uint32_t sym_start_addr; /* !<  syma start low addr */
    uint32_t sym_start_high; /* !<  syma start high addr */
} hal_symc_entry_out_t;


int32_t hal_symc_chn_init_romable(uint32_t chn_num, hal_symc_entry_in_t entry_in[CRYPTO_SYMC_MAX_DEPTH],
    hal_symc_entry_out_t entry_out[CRYPTO_SYMC_MAX_DEPTH]);

void hal_symc_chn_deinit_romable(uint32_t chn_num);

int32_t hal_symc_dma_romable(uint64_t src, uint64_t dst, uint32_t len);

enum {
    HAL_ECB_MODE            = 0x1,
    HAL_CBC_MODE            = 0x3,
    HAL_CTR_MODE            = 0x6,
    HAL_OFB_MODE            = 0x7,
    HAL_CFB128_MODE         = 0x8,
    HAL_CFB8_MODE           = 0x48,
    HAL_CFB1_MODE           = 0x88,
    HAL_GCTR_MODE           = 0xB,
    HAL_CMAC_MODE           = 0xC,
    HAL_CBC_NOOUT_MODE      = 0xD,
    HAL_GCTR_NOOUT_MODE     = 0xE,
};

enum {
    HAL_AES_ALG             = 0x2,
    HAL_SM4_ALG             = 0x5,
    HAL_AES_GHASH_ALG       = 0x6,
    HAL_SM4_GHASH_ALG       = 0x9,
    HAL_CRC_ALG             = 0xC,
    HAL_DMA_ALG             = 0xF,
};

#define hal_compat_symc_alg(alg, mode)  \
    (((alg) << 16) | ((mode) << 20))

#define hal_get_symc_alg(symc_alg)  \
    (((symc_alg) >> 16) & 0xF)

int32_t hal_symc_crypt_romable(uint32_t symc_alg,
    uint64_t src, uint64_t dst, uint32_t len, uint8_t iv[16],
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_chn, bool is_encrypt);

int32_t hal_symc_try_alloc_chn_romable(uint32_t *chn_num);

void hal_symc_free_chn_romable(uint32_t chn_num);

int32_t hal_symc_wait_done_romable(uint32_t chn_num, bool is_noout);

bool hal_symc_done_try_romable(uint32_t chn_num, bool is_noout);

void hal_symc_debug_romable(uint32_t chn_num);

void hal_symc_debug_default(uint32_t chn_num);

typedef int32_t (*symc_try_alloc_chn_func)(uint32_t *chn_num);
typedef void (*symc_free_chn_func)(uint32_t chn_num);
typedef int32_t (*symc_wait_done_func)(uint32_t chn_num, bool is_noout);
typedef void (*symc_debug_func)(uint32_t chn_num);

typedef struct {
    symc_try_alloc_chn_func     try_alloc_chn_func;
    symc_free_chn_func          free_chn_func;
    symc_wait_done_func         wait_done_func;
    symc_debug_func             debug_func;
} hal_symc_func_table_t;

void hal_symc_register_romable(const hal_symc_func_table_t *func_table);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
