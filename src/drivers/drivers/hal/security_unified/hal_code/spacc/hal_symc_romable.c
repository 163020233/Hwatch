/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal symc romable. \n
 *
 * History: \n
 * 2025-03-07, Create file. \n
 */
#include "hal_symc_romable.h"
#include "hal_spacc_reg.h"
#include "hal_keyslot_romable.h"

#include "crypto_drv_common.h"

#define REE_CPU_MASK        0x1
#define TEE_CPU_MASK        0x2

#define CPU_MASK            0xF
#define CPU_MASK_BITS       4

#define TEE_IN_SYM_CHN_CTRL 0x80005500
#define REE_IN_SYM_CHN_CTRL 0x8000AA00

#define CRYPTO_SYMC_MAX_CHN_NUM     2

ROM_SHARE_BSS hal_symc_entry_in_t *g_symc_entry_in[CRYPTO_SYMC_MAX_CHN_NUM];
ROM_SHARE_BSS hal_symc_entry_out_t *g_symc_entry_out[CRYPTO_SYMC_MAX_CHN_NUM];
ROM_SHARE_BSS uint32_t g_symc_idx_in[CRYPTO_SYMC_MAX_CHN_NUM];
ROM_SHARE_BSS uint32_t g_symc_idx_out[CRYPTO_SYMC_MAX_CHN_NUM];

ROM_SHARE_BSS hal_symc_func_table_t g_symc_table;

int32_t hal_symc_chn_init_romable(uint32_t chn_num, hal_symc_entry_in_t entry_in[CRYPTO_SYMC_MAX_DEPTH],
    hal_symc_entry_out_t entry_out[CRYPTO_SYMC_MAX_DEPTH])
{
    crypto_cpu_type cpu_type = crypto_get_cpu_type();
    uint32_t cpu_mask = TEE_CPU_MASK;
    uint32_t in_sym_chn_ctrl_val = TEE_IN_SYM_CHN_CTRL;
    uint32_t used;
    uint32_t idx = chn_num - 1;

    if (cpu_type == CRYPTO_CPU_TYPE_ACPU) {
        cpu_mask = REE_CPU_MASK;
        in_sym_chn_ctrl_val = REE_IN_SYM_CHN_CTRL;
    }

    used = spacc_reg_read(SPACC_SYM_CHN_LOCK);
    /* Step 0. Check if channel has benn locked. */
#if defined(CONFIG_MULTI_THREAD_SUPPORT)    /* fix 3322 multi-thread problem only, remove this macro later. */
    if (CHN_WHO_USED_GET(used, chn_num) == cpu_mask) {
        return CRYPTO_SUCCESS;
    }
#endif

    /* Step 1. Try to lock channel. */
    CHN_WHO_USED_SET(used, chn_num, cpu_mask);
    spacc_reg_write(SPACC_SYM_CHN_LOCK, used);

    /* Step 2. Check if lock success. */
    used = spacc_reg_read(SPACC_SYM_CHN_LOCK);
    if (CHN_WHO_USED_GET(used, chn_num) != cpu_mask) {
        return ERROR_CHN_BUSY;
    }

    /* Step 3. Configure IN_SYM_CHN_CTRL. */
    spacc_reg_write(IN_SYM_CHN_CTRL(chn_num), in_sym_chn_ctrl_val);

    /* Step 4. Configure Node Registers. */
    spacc_reg_write(IN_SYM_CHN_NODE_START_ADDR_L(chn_num), (uintptr_t)entry_in);
    spacc_reg_write(IN_SYM_CHN_NODE_START_ADDR_H(chn_num), 0);
    spacc_reg_write(IN_SYM_CHN_NODE_LENGTH(chn_num), CRYPTO_SYMC_MAX_DEPTH);

    spacc_reg_write(OUT_SYM_CHN_NODE_START_ADDR_L(chn_num), (uintptr_t)entry_out);
    spacc_reg_write(OUT_SYM_CHN_NODE_START_ADDR_H(chn_num), 0);
    spacc_reg_write(OUT_SYM_CHN_NODE_LENGTH(chn_num), CRYPTO_SYMC_MAX_DEPTH);

    /* Step 5. Save Node Address. */
    g_symc_entry_in[idx] = entry_in;
    g_symc_entry_out[idx] = entry_out;
    g_symc_idx_in[idx] = spacc_reg_read(IN_SYM_CHN_NODE_RD_POINT(chn_num));
    g_symc_idx_out[idx] = spacc_reg_read(OUT_SYM_CHN_NODE_RD_POINT(chn_num));

    return CRYPTO_SUCCESS;
}

void hal_symc_chn_deinit_romable(uint32_t chn_num)
{
    uint32_t used;

    used = spacc_reg_read(SPACC_SYM_CHN_LOCK);
    used &= ~(CPU_MASK << (chn_num * CPU_MASK_BITS));
    spacc_reg_write(SPACC_SYM_CHN_LOCK, used);
}

#define INVALID_KEYSLOT     0xFFFFFFFF

/* Parse key_len in bytes to register value.
** 16 --> 0x00
** 24 --> 0x01
** 32 --> 0x10
 */
#define key_len_parse(key_len)      ((key_len) / 8 - 1)

int32_t hal_symc_crypt_romable(uint32_t symc_alg,
    uint64_t src, uint64_t dst, uint32_t len, uint8_t iv[16],
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_chn, bool is_encrypt)
{
    int32_t ret;
    uint32_t chn_num;
    uint32_t local_keyslot_chn = keyslot_chn;
    in_sym_chn_key_ctrl key_ctrl;
    bool is_noout = (dst == 0 ? true : false);
    hal_symc_entry_in_t *entry_in;
    hal_symc_entry_out_t *entry_out;
    uint32_t i;
    uint32_t kl_key_cfg_val = AES_KL_KEY_CFG_VAL;
    bool is_dma = (hal_get_symc_alg(symc_alg) == HAL_DMA_ALG) ? true : false;
    bool is_crc = (hal_get_symc_alg(symc_alg) == HAL_CRC_ALG) ? true : false;
    uint32_t idx;

    if (len == 0) {
        return CRYPTO_SUCCESS;
    }

    /* Step 1. Try to alloc one free chn. */
    ret = hal_symc_try_alloc_chn_romable(&chn_num);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ERROR_CHN_BUSY);

    idx = chn_num - 1;

    if ((keyslot_chn == INVALID_KEYSLOT) && (is_dma == false) && (is_crc == false)) {
        /* Step 2. Send clear key to mcipher keyslot. */
        crypto_unused(key);
        crypto_unused(key_len);
        local_keyslot_chn = chn_num;
        if (hal_get_symc_alg(symc_alg) == HAL_SM4_ALG ||
            hal_get_symc_alg(symc_alg) == HAL_SM4_GHASH_ALG) {
            kl_key_cfg_val = SM4_KL_KEY_CFG_VAL;
        }

        ret = hal_keyslot_mcipher_lock_romable(local_keyslot_chn);
        crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free_chn);

        ret = hal_keyslot_mcipher_send_clear_key_romable(local_keyslot_chn, key, key_len, kl_key_cfg_val);
        crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_keyslot_unlock);
    }

    /* Step 3. Configure IN_SYM_CHN_KEY_CTRL and IN_SYM_OUT_CTRL. */
    if (is_dma) {
        spacc_reg_write(IN_SYM_CHN_KEY_CTRL(chn_num), 0);
        spacc_reg_write(IN_SYM_OUT_CTRL(chn_num), 1);    // 1: enable dma copy.
    } else if (is_crc) {
        spacc_reg_write(IN_SYM_CHN_KEY_CTRL(chn_num), symc_alg);    /* set alg & mod. */
        spacc_reg_write(IN_SYM_OUT_CTRL(chn_num), 0);    // 0: disable dma copy.
    } else {
        key_ctrl.u32 = symc_alg;
        key_ctrl.bits.sym_key_chn_id = local_keyslot_chn;
        key_ctrl.bits.sym_alg_decrypt = (is_encrypt) ? 0 : 1;
        key_ctrl.bits.sym_alg_key_len = key_len_parse(key_len);
        spacc_reg_write(IN_SYM_CHN_KEY_CTRL(chn_num), key_ctrl.u32);
        spacc_reg_write(IN_SYM_OUT_CTRL(chn_num), 0);    // 0: disable dma copy.
    }

    /* Step 4. Update iv. */
    entry_in = &g_symc_entry_in[idx][g_symc_idx_in[idx]];
    if (iv != NULL) {
        ret = memcpy_s(entry_in->iv, sizeof(entry_in->iv), iv, CRYPTO_AES_IV_SIZE);
        crypto_chk_goto_with_ret(ret, ret != EOK, exit_keyslot_unlock, ERROR_MEMCPY_S);
    }

    /* Step 5. Configure input node info. */
    entry_in->sym_first_node = 1;
    entry_in->sym_last_node = 1;
    entry_in->sym_alg_length = len;
    entry_in->sym_start_addr = (uint32_t)src;
    g_symc_idx_in[idx] = (g_symc_idx_in[idx] + 1) % CRYPTO_SYMC_MAX_DEPTH;

    /* Step 6. Configure output node info. */
    if (is_noout == false) {
        entry_out = &g_symc_entry_out[idx][g_symc_idx_out[idx]];
        entry_out->sym_alg_length = len;
        entry_out->sym_start_addr = (uint32_t)dst;
        g_symc_idx_out[idx] = (g_symc_idx_out[idx] + 1) % CRYPTO_SYMC_MAX_DEPTH;
        crypto_cache_flush((uintptr_t)entry_out, sizeof(hal_symc_entry_out_t));
    }

    /* Step 7. Cache operation. */
    crypto_cache_flush((uintptr_t)entry_in, sizeof(hal_symc_entry_in_t));
    crypto_cache_flush((uintptr_t)entry_in->sym_start_addr, len);

    /* Step 8. Start compute. */
    if (is_noout == false) {
        crypto_cache_flush((uintptr_t)entry_out->sym_start_addr, len);
        spacc_reg_write(OUT_SYM_CHN_NODE_WR_POINT(chn_num), g_symc_idx_out[idx]);
    }
    spacc_reg_write(IN_SYM_CHN_NODE_WR_POINT(chn_num), g_symc_idx_in[idx]);

    /* Step 9. Wait compute finish. */
    ret = hal_symc_wait_done_romable(chn_num, is_noout);
    if (is_noout == false) {
        crypto_cache_inv((uintptr_t)entry_out->sym_start_addr, len);
    }
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_keyslot_unlock);

    /* Step 10. Update iv. */
    if (iv != NULL) {
        for (i = 0; i < CRYPTO_AES_IV_SIZE_IN_WORD; i++) {
            entry_in->iv[i] = spacc_reg_read(CHANN_CIPHER_IVOUT(chn_num) + i * CRYPTO_WORD_WIDTH);
        }
        ret = memcpy_s(iv, CRYPTO_AES_IV_SIZE, entry_in->iv, sizeof(entry_in->iv));
        crypto_chk_goto_with_ret(ret, ret != EOK, exit_keyslot_unlock, ERROR_MEMCPY_S);
    }

    ret = CRYPTO_SUCCESS;

exit_keyslot_unlock:
    if ((keyslot_chn == INVALID_KEYSLOT) && (is_dma == false) && (is_crc == false)) {
        hal_keyslot_mcipher_unlock_romable(local_keyslot_chn);
    }
exit_free_chn:
    /* Step 11. Free channel. */
    hal_symc_free_chn_romable(chn_num);
    return ret;
}

int32_t hal_symc_dma_romable(uint64_t src, uint64_t dst, uint32_t len)
{
    return hal_symc_crypt_romable(hal_compat_symc_alg(HAL_DMA_ALG, 0), src, dst, len, NULL, NULL, 0, 0, true);
}

int32_t hal_symc_try_alloc_chn_romable(uint32_t *chn_num)
{
    if (g_symc_table.try_alloc_chn_func != NULL) {
        return g_symc_table.try_alloc_chn_func(chn_num);
    }
    *chn_num = 1;       // 1: default channel.
    return CRYPTO_SUCCESS;
}

void hal_symc_free_chn_romable(uint32_t chn_num)
{
    if (g_symc_table.free_chn_func != NULL) {
        g_symc_table.free_chn_func(chn_num);
    }
}

#ifndef CRYPTO_SYMC_CALC_WAIT_TIMEOUT
#define CRYPTO_SYMC_CALC_WAIT_TIMEOUT    (1000 * 1000)  /* max wait time 1s for 2M data. */
#endif
int32_t hal_symc_wait_done_romable(uint32_t chn_num, bool is_noout)
{
    uint32_t i;
    if (g_symc_table.wait_done_func != NULL) {
        return g_symc_table.wait_done_func(chn_num, is_noout);
    }

    for (i = 0; i < CRYPTO_SYMC_CALC_WAIT_TIMEOUT; i++) {
        if (hal_symc_done_try_romable(chn_num, is_noout)) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CRYPTO_SYMC_CALC_WAIT_TIMEOUT) {
        hal_symc_debug_romable(chn_num);
        return ERROR_SYMC_CALC_TIMEOUT;
    }
    return CRYPTO_SUCCESS;
}

bool hal_symc_done_try_romable(uint32_t chn_num, bool is_noout)
{
    uint32_t raw;
    uint32_t int_addr = OUT_SYM_CHAN_RAW_LAST_NODE_INT;
    if (is_noout) {
        int_addr = SYM_CHANN_RAW_INT;
    }
    raw = spacc_reg_read(int_addr);
    raw &= (1 << chn_num);
    if (raw != 0) {
        spacc_reg_write(int_addr, raw);
        return true;
    }
    return false;
}

void hal_symc_debug_romable(uint32_t chn_num)
{
    if (g_symc_table.debug_func != NULL) {
        g_symc_table.debug_func(chn_num);
    }
}

void hal_symc_register_romable(const hal_symc_func_table_t *func_table)
{
    (void)memcpy_s(&g_symc_table, sizeof(hal_symc_func_table_t), func_table, sizeof(hal_symc_func_table_t));
}