/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal hash romable. \n
 *
 * History: \n
 * 2025-03-07, Create file. \n
 */
#include "hal_hash_romable.h"
#include "hal_hash.h"
#include "hal_spacc_reg.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_HAL_CODE_SPACC_HAL_HASH_ROMABLE

#define REE_CPU_MASK        0x1
#define TEE_CPU_MASK        0x2

#define CPU_MASK            0xF
#define CPU_MASK_BITS       4

#define TEE_IN_HASH_CHN_CTRL    0x80000500
#define REE_IN_HASH_CHN_CTRL    0x80000A00

#define CRYPTO_HASH_MAX_CHN_NUM     2

ROM_SHARE_BSS hal_hash_entry_in_t *g_hash_entry_in[CRYPTO_HASH_MAX_CHN_NUM];
ROM_SHARE_BSS uint32_t g_hash_idx_in[CRYPTO_HASH_MAX_CHN_NUM];
ROM_SHARE_BSS hal_hash_func_table_t g_hash_table;

int32_t hal_hash_chn_init_romable(uint32_t chn_num, hal_hash_entry_in_t entry_in[CRYPTO_HASH_MAX_DEPTH])
{
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    uint32_t cpu_mask = TEE_CPU_MASK;
    uint32_t in_hash_chn_ctrl_val = TEE_IN_HASH_CHN_CTRL;
    uint32_t used;

    if (cpu_type == CRYPTO_CPU_TYPE_ACPU) {
        cpu_mask = REE_CPU_MASK;
        in_hash_chn_ctrl_val = REE_IN_HASH_CHN_CTRL;
    }

    used = spacc_reg_read(SPACC_HASH_CHN_LOCK);
    /* Step 0. Check if channel has benn locked. */
#if defined(CONFIG_MULTI_THREAD_SUPPORT)    /* fix 3322 multi-thread problem only, remove this macro later. */
    if (CHN_WHO_USED_GET(used, chn_num) == cpu_mask) {
        return CRYPTO_SUCCESS;
    }
#endif
    /* Step 1. Try to lock channel. */
    CHN_WHO_USED_SET(used, chn_num, cpu_mask);
    spacc_reg_write(SPACC_HASH_CHN_LOCK, used);

    /* Step 2. Check if lock success. */
    used = spacc_reg_read(SPACC_HASH_CHN_LOCK);
    if (CHN_WHO_USED_GET(used, chn_num) != cpu_mask) {
        drv_set_err(ERROR_CHN_BUSY, "direct error return");
        return ERROR_CHN_BUSY;
    }

    /* Step 3. Configure IN_HASH_CHN_CTRL. */
    spacc_reg_write(IN_HASH_CHN_CTRL(chn_num), in_hash_chn_ctrl_val);

    /* Step 4. Configure Node Registers. */
    spacc_reg_write(IN_HASH_CHN_NODE_START_ADDR_L(chn_num), (uintptr_t)entry_in);
    spacc_reg_write(IN_HASH_CHN_NODE_START_ADDR_H(chn_num), 0);
    spacc_reg_write(IN_HASH_CHN_NODE_LENGTH(chn_num), CRYPTO_HASH_MAX_DEPTH);

    /* Step 5. Save Node Address. */
    g_hash_entry_in[chn_num - 1] = entry_in;
    g_hash_idx_in[chn_num - 1] = spacc_reg_read(IN_HASH_CHN_NODE_RD_POINT(chn_num));

    return CRYPTO_SUCCESS;
}

void hal_hash_chn_deinit_romable(uint32_t chn_num)
{
    uint32_t used;

    used = spacc_reg_read(SPACC_HASH_CHN_LOCK);
    used &= ~(CPU_MASK << (chn_num * CPU_MASK_BITS));
    spacc_reg_write(SPACC_HASH_CHN_LOCK, used);
}

int32_t hal_hash_compute_romable(uint32_t hash_alg,
    uint64_t src, uint32_t len,
    const uint32_t *state_in, uint32_t *state_out, uint32_t state_len)
{
    int32_t ret;
    uint32_t chn_num;
    uint32_t i;
    hal_hash_entry_in_t *entry_in;
    uint32_t idx;

    if (len == 0) {
        return CRYPTO_SUCCESS;
    }

    /* Step 1. Try alloc hash channel. */
    ret = hal_hash_try_alloc_chn_romable(&chn_num);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ERROR_CHN_BUSY);

    idx = chn_num - 1;
    /* Step 2. Configure IN_HASH_CHN_KEY_CTRL and state_in. */
    if (state_in != NULL) {
        spacc_reg_write(IN_HASH_CHN_KEY_CTRL(chn_num), hash_alg);
        for (i = 0; i < state_len / CRYPTO_WORD_WIDTH; i++) {
            spacc_reg_write(CHANN_HASH_STATE_VAL_ADDR(chn_num), i);
            spacc_reg_write(CHANN_HASH_STATE_VAL(chn_num), state_in[i]);
        }
    }

    /* Step 3. Configure input node info. */
    entry_in = &g_hash_entry_in[idx][g_hash_idx_in[idx]];
    entry_in->hash_first_node = 1;
    entry_in->hash_last_node = 1;
    entry_in->hash_start_addr_l = (uint32_t)src;
    entry_in->hash_alg_length = len;
    g_hash_idx_in[idx] = (g_hash_idx_in[idx] + 1) % CRYPTO_HASH_MAX_DEPTH;

    /* Step 4. Cache operation. */
    crypto_cache_flush((uintptr_t)entry_in, sizeof(hal_hash_entry_in_t));
    crypto_cache_flush(src, len);

    /* Step 5. Start compute. */
    spacc_reg_write(IN_HASH_CHN_NODE_WR_POINT(chn_num), g_hash_idx_in[idx]);

    /* Step 6. Wait compute finish. */
    ret = hal_hash_wait_done_romable(chn_num);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, exit_free_chn);

    /* Step 7. Update state_out. */
    if (state_out != NULL) {
        for (i = 0; i < state_len / CRYPTO_WORD_WIDTH; i++) {
            spacc_reg_write(CHANN_HASH_STATE_VAL_ADDR(chn_num), i);
            state_out[i] = spacc_reg_read(CHANN_HASH_STATE_VAL(chn_num));
        }
    }

    ret = CRYPTO_SUCCESS;
exit_free_chn:
    hal_hash_free_chn_romable(chn_num);
    return ret;
}

int32_t hal_hash_try_alloc_chn_romable(uint32_t *chn_num)
{
    if (g_hash_table.try_alloc_chn_func != NULL) {
        return g_hash_table.try_alloc_chn_func(chn_num);
    }
    *chn_num = 1;
    return CRYPTO_SUCCESS;
}

void hal_hash_free_chn_romable(uint32_t chn_num)
{
    if (g_hash_table.free_chn_func != NULL) {
        g_hash_table.free_chn_func(chn_num);
    }
}

#ifndef CRYPTO_HASH_CALC_WAIT_TIMEOUT
#define CRYPTO_HASH_CALC_WAIT_TIMEOUT    (2000 * 1000)  /* max wait time 2s for 2M data. */
#endif
int32_t hal_hash_wait_done_romable(uint32_t chn_num)
{
    uint32_t i;
    if (g_hash_table.wait_done_func != NULL) {
        return g_hash_table.wait_done_func(chn_num);
    }

    for (i = 0; i < CRYPTO_HASH_CALC_WAIT_TIMEOUT; i++) {
        if (hal_hash_done_try_romable(chn_num)) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CRYPTO_HASH_CALC_WAIT_TIMEOUT) {
        hal_hash_debug_romable(chn_num);
        drv_set_err(ERROR_HASH_CALC_TIMEOUT, "direct error return");
        return ERROR_HASH_CALC_TIMEOUT;
    }
    return CRYPTO_SUCCESS;
}

bool hal_hash_done_try_romable(uint32_t chn_num)
{
    uint32_t raw;

    raw = spacc_reg_read(HASH_CHANN_RAW_INT);
    raw &= (0x01 << chn_num);
    if (raw != 0) {
        spacc_reg_write(HASH_CHANN_RAW_INT, raw);
        return true;
    }
    return false;
}

void hal_hash_debug_romable(uint32_t chn_num)
{
    if (g_hash_table.debug_func != NULL) {
        g_hash_table.debug_func(chn_num);
    }
}

void hal_hash_register_romable(const hal_hash_func_table_t *func_table)
{
    (void)memcpy_s(&g_hash_table, sizeof(hal_hash_func_table_t), func_table, sizeof(hal_hash_func_table_t));
}

int hal_cipher_hash_compute(crypto_hash_type hash_type,
    unsigned long long data_phys_addr, unsigned int data_len, unsigned int *state, unsigned int state_len)
{
    uint32_t alg = HAL_SHA256_ALG;
    if (hash_type == CRYPTO_HASH_TYPE_SHA1) {
        alg = HAL_SHA1_ALG;
    } else if (hash_type == CRYPTO_HASH_TYPE_SHA224) {
        alg = HAL_SHA224_ALG;
    } else if (hash_type == CRYPTO_HASH_TYPE_SHA384) {
        alg = HAL_SHA384_ALG;
    } else if (hash_type == CRYPTO_HASH_TYPE_SHA512) {
        alg = HAL_SHA512_ALG;
    } else if (hash_type == CRYPTO_HASH_TYPE_SM3) {
        alg = HAL_SM3_ALG;
    }

    return hal_hash_compute_romable(alg, data_phys_addr, data_len, state, state, state_len);
}