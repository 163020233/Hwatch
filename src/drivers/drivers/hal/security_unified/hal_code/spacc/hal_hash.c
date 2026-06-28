/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal hash. \n
 */
#include "hal_hash.h"
#include "hal_hash_romable.h"
#include "hal_spacc_reg.h"

#include "crypto_drv_common.h"

void hal_hash_debug_default(uint32_t chn_num)
{
    uint32_t i;
    hal_hash_entry_in_t *entry_in;
    crypto_unused(chn_num);
    crypto_unused(entry_in);

    crypto_print("SPACC_HASH_CHN_LOCK is 0x%x\n", spacc_reg_read(SPACC_HASH_CHN_LOCK));

    crypto_print("HASH_CHANN_RAW_INT is 0x%x\n", spacc_reg_read(HASH_CHANN_RAW_INT));
    crypto_print("HASH_CHANN_RAW_INT_EN is 0x%x\n", spacc_reg_read(HASH_CHANN_RAW_INT_EN));

    crypto_print("TEE_HASH_CALC_CTRL_CHECK_ERR is 0x%x\n", spacc_reg_read(TEE_HASH_CALC_CTRL_CHECK_ERR));
    crypto_print("TEE_HASH_CALC_CTRL_CHECK_ERR_STATUS is 0x%x\n", spacc_reg_read(TEE_HASH_CALC_CTRL_CHECK_ERR_STATUS));

    crypto_print("REE_HASH_CALC_CTRL_CHECK_ERR is 0x%x\n", spacc_reg_read(REE_HASH_CALC_CTRL_CHECK_ERR));
    crypto_print("REE_HASH_CALC_CTRL_CHECK_ERR_STATUS is 0x%x\n", spacc_reg_read(REE_HASH_CALC_CTRL_CHECK_ERR_STATUS));

    crypto_print("IN_HASH_CHN_CTRL is 0x%x\n", spacc_reg_read(IN_HASH_CHN_CTRL(chn_num)));
    crypto_print("IN_HASH_CHN_KEY_CTRL is 0x%x\n", spacc_reg_read(IN_HASH_CHN_KEY_CTRL(chn_num)));
    crypto_print("IN_HASH_CHN_NODE_LENGTH: 0x%x\n", spacc_reg_read(IN_HASH_CHN_NODE_LENGTH(chn_num)));
    crypto_print("IN_HASH_CHN_NODE_START_ADDR_L: 0x%x\n", spacc_reg_read(IN_HASH_CHN_NODE_START_ADDR_L(chn_num)));
    crypto_print("IN_HASH_CHN_NODE_START_ADDR_H: 0x%x\n", spacc_reg_read(IN_HASH_CHN_NODE_START_ADDR_H(chn_num)));

    crypto_print("IN_HASH_CHN_DATA_LEN: 0x%x\n", spacc_reg_read(IN_HASH_CHN_DATA_LEN(chn_num)));

    crypto_print("IN_HASH_CHN_NODE_WR_POINT: 0x%x\n", spacc_reg_read(IN_HASH_CHN_NODE_WR_POINT(chn_num)));
    crypto_print("IN_HASH_CHN_NODE_RD_POINT: 0x%x\n", spacc_reg_read(IN_HASH_CHN_NODE_RD_POINT(chn_num)));

    entry_in = (hal_hash_entry_in_t *)(uintptr_t)spacc_reg_read(IN_HASH_CHN_NODE_START_ADDR_L(chn_num));
    /* Print Entry_In. */
    for (i = 0; i < CRYPTO_HASH_MAX_DEPTH; i++) {
        crypto_print("Entry_IN[%u]:\n", i);
        crypto_print("hash_first_node is 0x%x\n", entry_in->hash_first_node);
        crypto_print("hash_last_node is 0x%x\n", entry_in->hash_last_node);
        crypto_print("hash_alg_length is 0x%x\n", entry_in->hash_alg_length);
        crypto_print("hash_start_addr_l is 0x%x\n", entry_in->hash_start_addr_l);
        crypto_print("hash_start_addr_h is 0x%x\n", entry_in->hash_start_addr_h);
    }

    /* Print last state */
    for (i = 0; i < CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD; i++) {
        spacc_reg_write(CHANN_HASH_STATE_VAL_ADDR(chn_num), i);
        crypto_print("CHANn_HASH_STATE_VAL[%d] is 0x%x\n", i, spacc_reg_read(CHANN_HASH_STATE_VAL(chn_num)));
    }
}

int32_t hal_cipher_hmac_compute(crypto_hash_type hash_type, uint64_t data_phys_addr, uint32_t data_len,
    uint32_t keyslot_chn, uint32_t *state, uint32_t state_len)
{
    in_hash_chn_key_ctrl key_ctrl;
    uint32_t alg = HAL_SHA256_ALG;
    if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SHA1) {
        alg = HAL_SHA1_ALG;
    } else if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SHA224) {
        alg = HAL_SHA224_ALG;
    } else if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SHA384) {
        alg = HAL_SHA384_ALG;
    } else if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SHA512) {
        alg = HAL_SHA512_ALG;
    } else if (crypto_hash_remove_hmac_flag(hash_type) == CRYPTO_HASH_TYPE_SM3) {
        alg = HAL_SM3_ALG;
    }

    key_ctrl.u32 = alg;
    key_ctrl.bits.hmac_vld = 1;
    key_ctrl.bits.hash_key_chn_id = keyslot_chn;
    return hal_hash_compute_romable(key_ctrl.u32, data_phys_addr, data_len, state, state, state_len);
}

bool hal_hash_done_try_special(uint32_t chn_num)
{
    uint32_t raw;

    raw = spacc_reg_read(HASH_CHANN_INT_SPECIAL);
    raw &= (0x01 << chn_num);
    if (raw != 0) {
        spacc_reg_write(HASH_CHANN_INT_SPECIAL, raw);
        return true;
    }
    return false;
}

void hal_hash_interrupt_enable(uint32_t chn_num, bool enable)
{
    uint32_t raw_int_en;

    raw_int_en = spacc_reg_read(HASH_CHANN_RAW_INT_EN);
    if (enable) {
        raw_int_en |= (0x1 << chn_num);
    } else {
        raw_int_en &= ~(1 << chn_num);
    }
    spacc_reg_write(HASH_CHANN_RAW_INT_EN, raw_int_en);
}