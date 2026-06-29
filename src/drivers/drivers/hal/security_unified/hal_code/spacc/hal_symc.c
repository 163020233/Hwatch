/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal symc. \n
 *
 * History: \n
 * 2025-05-21, Create file. \n
 */
#include "hal_symc.h"
#include "hal_symc_romable.h"
#include "hal_spacc_reg.h"
#include "crypto_drv_common.h"

static void hal_cipher_symc_print_entry_in(const hal_symc_entry_in_t *entry_in)
{
    crypto_unused(entry_in);
    crypto_print("The Details of Entry In:\n");
    crypto_print("sym_first_node is 0x%x\n", entry_in->sym_first_node);
    crypto_print("sym_last_node is 0x%x\n", entry_in->sym_last_node);
    crypto_print("odd_even is 0x%x\n", entry_in->odd_even);
    crypto_print("sym_alg_length is 0x%x\n", entry_in->sym_alg_length);
    crypto_print("sym_start_addr is 0x%x\n", entry_in->sym_start_addr);
    crypto_print("sym_start_high is 0x%x\n", entry_in->sym_start_high);
    crypto_dump_data("iv", (td_u8 *)entry_in->iv, sizeof(entry_in->iv));
}

static void hal_cipher_symc_print_entry_out(const hal_symc_entry_out_t *entry_out)
{
    crypto_unused(entry_out);
    crypto_print("The Details of Entry Out:\n");
    crypto_print("sym_alg_length is 0x%x\n", entry_out->sym_alg_length);
    crypto_print("sym_start_addr is 0x%x\n", entry_out->sym_start_addr);
    crypto_print("sym_start_high is 0x%x\n", entry_out->sym_start_high);
}

void hal_symc_debug_default(uint32_t chn_num)
{
    uint32_t i;
    hal_symc_entry_in_t *entry_in;
    hal_symc_entry_out_t *entry_out;

    crypto_unused(chn_num);
    crypto_unused(entry_in);
    crypto_unused(entry_out);

    crypto_print("==================== chn_num[%u] ====================\n", chn_num);
    crypto_print("SPACC_SYMC_CHN_LOCK is 0x%x\n", spacc_reg_read(SPACC_SYM_CHN_LOCK));
    crypto_print("OUT_SYM_CHAN_RAW_LAST_NODE_INT is 0x%x\n", spacc_reg_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT));
    crypto_print("OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN is 0x%x\n", spacc_reg_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN));
    crypto_print("SYM_CHANN_RAW_INT is 0x%x\n", spacc_reg_read(SYM_CHANN_RAW_INT));
    crypto_print("SYM_CHANN_RAW_INT_EN is 0x%x\n", spacc_reg_read(SYM_CHANN_RAW_INT_EN));
    crypto_print("TEE_SYM_CALC_CTRL_CHECK_ERR is 0x%x\n", spacc_reg_read(TEE_SYM_CALC_CTRL_CHECK_ERR));
    crypto_print("TEE_SYM_CALC_CTRL_CHECK_ERR_STATUS is 0x%x\n", spacc_reg_read(TEE_SYM_CALC_CTRL_CHECK_ERR_STATUS));
    crypto_print("REE_SYM_CALC_CTRL_CHECK_ERR is 0x%x\n", spacc_reg_read(REE_SYM_CALC_CTRL_CHECK_ERR));
    crypto_print("REE_SYM_CALC_CTRL_CHECK_ERR_STATUS is 0x%x\n", spacc_reg_read(REE_SYM_CALC_CTRL_CHECK_ERR_STATUS));

    crypto_print("IN_SYM_CHN_NODE_START_ADDR_L is 0x%x\n", spacc_reg_read(IN_SYM_CHN_NODE_START_ADDR_L(chn_num)));
    crypto_print("IN_SYM_CHN_NODE_START_ADDR_H is 0x%x\n", spacc_reg_read(IN_SYM_CHN_NODE_START_ADDR_H(chn_num)));
    crypto_print("IN_SYM_CHN_NODE_LENGTH is 0x%x\n", spacc_reg_read(IN_SYM_CHN_NODE_LENGTH(chn_num)));

    crypto_print("DBG_IN_SYM_CHN_RD_ADDR_H is 0x%x\n", spacc_reg_read(DBG_IN_SYM_CHN_RD_ADDR_H(chn_num)));
    crypto_print("DBG_IN_SYM_CHN_RD_ADDR_L is 0x%x\n", spacc_reg_read(DBG_IN_SYM_CHN_RD_ADDR_L(chn_num)));
    crypto_print("DBG_IN_SYM_CHN_DATA_LEN is 0x%x\n", spacc_reg_read(DBG_IN_SYM_CHN_DATA_LEN(chn_num)));

    crypto_print("OUT_SYM_CHN_NODE_START_ADDR_L is 0x%x\n", spacc_reg_read(OUT_SYM_CHN_NODE_START_ADDR_L(chn_num)));
    crypto_print("OUT_SYM_CHN_NODE_START_ADDR_H is 0x%x\n", spacc_reg_read(OUT_SYM_CHN_NODE_START_ADDR_H(chn_num)));
    crypto_print("OUT_SYM_CHN_NODE_LENGTH is 0x%x\n", spacc_reg_read(OUT_SYM_CHN_NODE_LENGTH(chn_num)));

    crypto_print("DBG_OUT_SYM_CHN_RD_ADDR_H is 0x%x\n", spacc_reg_read(DBG_OUT_SYM_CHN_RD_ADDR_H(chn_num)));
    crypto_print("DBG_OUT_SYM_CHN_RD_ADDR_L is 0x%x\n", spacc_reg_read(DBG_OUT_SYM_CHN_RD_ADDR_L(chn_num)));
    crypto_print("DBG_OUT_SYM_CHN_NODE_LEFT_BUF_LEN is 0x%x\n",
        spacc_reg_read(DBG_OUT_SYM_CHN_NODE_LEFT_BUF_LEN(chn_num)));

    crypto_print("IN_SYM_CHN_CTRL is 0x%x\n", spacc_reg_read(IN_SYM_CHN_CTRL(chn_num)));
    crypto_print("IN_SYM_OUT_CTRL is 0x%x\n", spacc_reg_read(IN_SYM_OUT_CTRL(chn_num)));
    crypto_print("IN_SYM_CHN_KEY_CTRL is 0x%x\n", spacc_reg_read(IN_SYM_CHN_KEY_CTRL(chn_num)));

    crypto_print("OUT_SYM_CHN_NODE_WR_POINT is 0x%x\n", spacc_reg_read(OUT_SYM_CHN_NODE_WR_POINT(chn_num)));
    crypto_print("OUT_SYM_CHN_NODE_RD_POINT is 0x%x\n", spacc_reg_read(OUT_SYM_CHN_NODE_RD_POINT(chn_num)));

    crypto_print("IN_SYM_CHN_NODE_WR_POINT is 0x%x\n", spacc_reg_read(IN_SYM_CHN_NODE_WR_POINT(chn_num)));
    crypto_print("IN_SYM_CHN_NODE_RD_POINT is 0x%x\n", spacc_reg_read(IN_SYM_CHN_NODE_RD_POINT(chn_num)));

    entry_in = (hal_symc_entry_in_t *)(uintptr_t)spacc_reg_read(IN_SYM_CHN_NODE_START_ADDR_L(chn_num));
    entry_out = (hal_symc_entry_out_t *)(uintptr_t)spacc_reg_read(OUT_SYM_CHN_NODE_START_ADDR_L(chn_num));

    for (i = 0; i < CRYPTO_SYMC_MAX_DEPTH; i++) {
        crypto_print("IDX_IN %u:\n", i);
        hal_cipher_symc_print_entry_in(&entry_in[i]);
    }
    for (i = 0; i < CRYPTO_SYMC_MAX_DEPTH; i++) {
        crypto_print("IDX_OUT %u:\n", i);
        hal_cipher_symc_print_entry_out(&entry_out[i]);
    }
}

int hal_cipher_symc_crypt(crypto_symc_alg symc_alg, crypto_symc_work_mode work_mode,
    crypto_symc_bit_width symc_bit_width, unsigned long long src_phys_addr, unsigned long long dst_phys_addr,
    unsigned int data_len, unsigned char iv[16], const unsigned char *key, unsigned int key_len,
    unsigned int keyslot_chn_num, td_bool is_decrypt)
{
    in_sym_chn_key_ctrl in_key_ctrl;
    in_key_ctrl.u32 = hal_compat_symc_alg(HAL_AES_ALG, HAL_CBC_MODE);

    crypto_unused(symc_bit_width);

    if (work_mode == CRYPTO_SYMC_WORK_MODE_ECB) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_ECB_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CTR) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CTR_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_OFB) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_OFB_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CFB) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CFB_VAL;
        in_key_ctrl.bits.sym_alg_data_width = SYMC_ALG_DATA_WIDTH_128BIT;
        if (symc_bit_width == CRYPTO_SYMC_BIT_WIDTH_1BIT) {
            in_key_ctrl.bits.sym_alg_data_width = SYMC_ALG_DATA_WIDTH_1BIT;
        } else if (symc_bit_width == CRYPTO_SYMC_BIT_WIDTH_8BIT) {
            in_key_ctrl.bits.sym_alg_data_width = SYMC_ALG_DATA_WIDTH_8BIT;
        }
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_GCM) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_GCTR_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CBC_MAC) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CBC_NOOUT_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CMAC) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CMAC_VAL;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CRC16) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CRC16_VAL;
        in_key_ctrl.bits.sym_alg_sel = HAL_CRC_ALG;
    } else if (work_mode == CRYPTO_SYMC_WORK_MODE_CRC32) {
        in_key_ctrl.bits.sym_alg_mode = SYMC_ALG_MODE_CRC32_VAL;
        in_key_ctrl.bits.sym_alg_sel = HAL_CRC_ALG;
    }

    if (symc_alg == CRYPTO_SYMC_ALG_SM4) {
        in_key_ctrl.bits.sym_alg_sel = SYMC_ALG_SM4_VAL;
    } else if (symc_alg == CRYPTO_SYMC_ALG_AES_GHASH) {
        in_key_ctrl.bits.sym_alg_sel = SYMC_ALG_GHASH_VAL;
        in_key_ctrl.bits.sym_alg_mode = 0;
    } else if (symc_alg == CRYPTO_SYMC_ALG_SM4_GHASH) {
        in_key_ctrl.bits.sym_alg_sel = SYMC_ALG_SM4_GHASH_VAL;
        in_key_ctrl.bits.sym_alg_mode = 0;
    }

    return hal_symc_crypt_romable(in_key_ctrl.u32, src_phys_addr, dst_phys_addr, data_len, iv,
        key, key_len, keyslot_chn_num, is_decrypt ? false : true);
}

void hal_spacc_interrupt_enable(bool enable)
{
    spacc_ie cfg_val = { 0 };

    cfg_val.u32 = spacc_reg_read(SPACC_IE);
    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_SCPU) {
        cfg_val.bits.spacc_ie_tee = enable;
    } else {
        cfg_val.bits.spacc_ie_ree = enable;
    }

    spacc_reg_write(SPACC_IE, cfg_val.u32);
}

void hal_symc_interrupt_enable(uint32_t chn_num, bool enable)
{
    uint32_t last_int_en;
    uint32_t raw_int_en;
    last_int_en = spacc_reg_read(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN);
    raw_int_en = spacc_reg_read(SYM_CHANN_RAW_INT_EN);
    if (enable) {
        last_int_en |= (0x1 << chn_num);
        raw_int_en |= (0x1 << chn_num);
    } else {
        last_int_en &= ~(1 << chn_num);
        raw_int_en &= ~(1 << chn_num);
    }

    spacc_reg_write(OUT_SYM_CHAN_RAW_LAST_NODE_INT_EN, last_int_en);
    spacc_reg_write(SYM_CHANN_RAW_INT_EN, raw_int_en);
}

bool hal_symc_done_try_special(uint32_t chn_num, bool is_noout)
{
    uint32_t raw;
    uint32_t int_addr = OUT_SYM_CHAN_LAST_NODE_INT_SPECIAL;
    if (is_noout) {
        int_addr = SYM_CHANN_INT_SPECIAL;
    }
    raw = spacc_reg_read(int_addr);
    raw &= (1 << chn_num);
    if (raw != 0) {
        spacc_reg_write(int_addr, raw);
        return true;
    }
    return false;
}

int hal_cipher_symc_gcm_ghash(crypto_symc_alg symc_alg, unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char ghash[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num)
{
    crypto_symc_alg ghash_alg = CRYPTO_SYMC_ALG_AES_GHASH;
    if (symc_alg == CRYPTO_SYMC_ALG_SM4) {
        ghash_alg = CRYPTO_SYMC_ALG_SM4_GHASH;
    }

    return hal_cipher_symc_crypt(ghash_alg, CRYPTO_SYMC_WORK_MODE_GCM, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        data_phys_addr, 0, data_len, ghash, key, key_len, keyslot_chn_num, TD_FALSE);
}

int hal_cipher_symc_cbc_mac(crypto_symc_alg symc_alg, unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char mac[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num)
{
    return hal_cipher_symc_crypt(symc_alg, CRYPTO_SYMC_WORK_MODE_CBC_MAC, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        data_phys_addr, 0, data_len, mac, key, key_len, keyslot_chn_num, TD_FALSE);
}

int hal_cipher_symc_cmac(crypto_symc_alg symc_alg,
    unsigned long long data_phys_addr, unsigned int data_len,
    unsigned char mac[16], const unsigned char *key, unsigned int key_len, unsigned int keyslot_chn_num)
{
    int ret;
    unsigned int tail_len;
    unsigned int aligned_len = 0;
    if (data_len > CRYPTO_AES_BLOCK_SIZE_IN_BYTES) {
        aligned_len = data_len - data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        if ((data_len % CRYPTO_AES_BLOCK_SIZE_IN_BYTES) == 0) {
            aligned_len -= CRYPTO_AES_BLOCK_SIZE_IN_BYTES;
        }
        ret = hal_cipher_symc_cbc_mac(symc_alg, data_phys_addr, aligned_len, mac, key, key_len, keyslot_chn_num);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_cipher_symc_cbc_mac failed\n");
    }
    tail_len = data_len - aligned_len;
    return hal_cipher_symc_crypt(symc_alg, CRYPTO_SYMC_WORK_MODE_CMAC, CRYPTO_SYMC_BIT_WIDTH_128BIT,
        data_phys_addr + aligned_len, crypto_get_phys_addr(mac), tail_len, mac, key, key_len, keyslot_chn_num,
        TD_FALSE);
}