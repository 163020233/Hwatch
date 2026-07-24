/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal pke not romable code. \n
 *
 * History: \n
 * 2025-03-07, Create file. \n
 */

#include "hal_pke_func.h"
#include "hal_pke_reg.h"

#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_HAL_PKE

void hal_pke_set_ram_not_aligned(uint32_t ram_section, const uint8_t *data, uint32_t data_len, uint32_t work_len)
{
    td_u32 i = 0;
    td_u32 j = 0;
    td_u32 val = 0;
    td_u32 random_num = 0;
    td_u32 write_len = 0;

    random_num = pke_reg_read(PKE_DRAM_MASK);

    /* set data_len to ram */
    td_u32 start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;
    for (i = data_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        val = 0;
        val = data[i - 4] << 24; /* i - 4 index shift 24 bits */
        val |= data[i - 3] << 16; /* i - 3 index shift 16 bits */
        val |= data[i - 2] << 8; /* i - 2 index shift 8 bits */
        val |= data[i - 1];
        val ^= random_num;
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }

    /* padding with 0x00 for not aligned with word */
    if (i != 0) {
        val = 0;
        for (j = 0; j < i; j++) {
            val |= data[j] << (CRYPTO_BITS_IN_BYTE * (i - j - 1));
        }
        val ^= random_num;
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    write_len = (data_len + CRYPTO_WORD_WIDTH - 1) / CRYPTO_WORD_WIDTH * CRYPTO_WORD_WIDTH;

    /* 3. padding with 0x00 for not aligned with work_len */
    for (; write_len < work_len; write_len += CRYPTO_WORD_WIDTH) {
        val = 0x0;
        val ^= random_num;
        pke_reg_write(start_dram_addr, val);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    return;
}

void hal_pke_debug_default(void)
{
    crypto_print("PKE_WORK_LEN: 0x%x\n", pke_reg_read(PKE_WORK_LEN));
    crypto_print("PKE_MONT_PARA0: 0x%x\n", pke_reg_read(PKE_MONT_PARA0));
    crypto_print("PKE_MONT_PARA1: 0x%x\n", pke_reg_read(PKE_MONT_PARA1));
#if defined(CONFIG_PKE_MONT_PARAM_128_BIT_SUPPORT)
    crypto_print("PKE_MONT_PARA2: 0x%x\n", pke_reg_read(PKE_MONT_PARA2));
    crypto_print("PKE_MONT_PARA3: 0x%x\n", pke_reg_read(PKE_MONT_PARA3));
#endif
    crypto_print("PKE_INSTR_ADDR_LOW: 0x%x\n", pke_reg_read(PKE_INSTR_ADDR_LOW));
    crypto_print("PKE_INSTR_LEN: 0x%x\n", pke_reg_read(PKE_INSTR_LEN));
    crypto_print("PKE_MONT_PARA0: 0x%x\n", pke_reg_read(PKE_MONT_PARA0));
    crypto_print("PKE_MONT_PARA1: 0x%x\n", pke_reg_read(PKE_MONT_PARA1));
    crypto_print("PKE_CK_GT_EN: 0x%x\n", pke_reg_read(PKE_CK_GT_EN));
    crypto_print("PKE_FAILURE_FLAG: 0x%x\n", pke_reg_read(PKE_FAILURE_FLAG));
    crypto_print("PKE_LOCK_STATUS: 0x%x\n", pke_reg_read(PKE_LOCK_STATUS));
}