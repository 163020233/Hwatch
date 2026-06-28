/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal pke romable. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */

#include "hal_pke_romable.h"
#include "hal_pke_reg.h"
#include "hal_trng_romable.h"

#include "crypto_drv_common.h"
#include "crypto_pke_common.h"

#ifndef CONFIG_PKE_CALC_TIMEOUT_US
#define CONFIG_PKE_CALC_TIMEOUT_US      60000
#endif

int32_t hal_pke_wait_done_sec(void)
{
    uint32_t i;
    pke_busy busy;
    pke_alarm_status result = {.u32 = PKE_NON_SPECIAL_VAL};

    /* wait ready */
    for (i = 0; i < CONFIG_PKE_CALC_TIMEOUT_US; i++) {
        busy.u32 = pke_reg_read(PKE_BUSY);
        if (busy.bits.pke_busy == 0) {
            break;
        }
        crypto_udelay(1); /* 1us */
    }
    if (i >= CONFIG_PKE_CALC_TIMEOUT_US) {
        hal_pke_debug_romable();
        return ERROR_PKE_WAIT_DONE_TIMEOUT;
    }

    if (pke_reg_read(PKE_FAILURE_FLAG) != 0) {
        hal_pke_debug_romable();
        return ERROR_PKE_LOGIC;
    }
    /* robust error check. */
    result.u32 = pke_reg_read(PKE_ALARM_STATUS);
    if (result.bits.alarm_int == PKE_ALARM_STATUS_EFFECTIVE_CODE) {
        pke_reg_write(PKE_ALARM_STATUS, PKE_ALARM_STATUS_CLEAN_CODE);   // clear warn interrupt
        hal_pke_debug_romable();
        return ERROR_PKE_ROBUST_WARNING;
    }

    return CRYPTO_ENHANCE_SUCCESS;
}

int32_t hal_pke_batch_compute_sec(uint32_t instr_addr, uint32_t instr_num)
{
    pke_reg_write(PKE_INSTR_ADDR_LOW, instr_addr);
    pke_reg_write(PKE_INSTR_ADDR_HIG, 0);    /* compatible with old version IP */
    pke_reg_write(PKE_INSTR_LEN, instr_num * CRYPTO_WORD_WIDTH);
    pke_reg_write(PKE_START, 0x5AA);

    return hal_pke_wait_done_sec();
}

/* key1 can be NULL, which means key1 is const_0 */
int32_t hal_pke_loop_batch_compute_sec(uint32_t loop_num,
    const unsigned char *key0, const unsigned char *key1,
    unsigned int klen, const rom_lib *batch_instr[4])
{
    volatile int32_t ret = CRYPTO_ENHANCE_FAILURE;
    uint32_t i = 0;
    bool key0_bit;
    bool key1_bit;
    unsigned char bit_2;

    if (loop_num > klen * CRYPTO_BITS_IN_BYTE) {
        return CRYPTO_ENHANCE_FAILURE;
    }

    for (i = 0; i < klen * CRYPTO_BITS_IN_BYTE; i++) {
        if (i < (klen * CRYPTO_BITS_IN_BYTE - loop_num)) {
            continue;
        }

        key0_bit = crypto_pke_common_get_bit_from_high_to_low(key0, i);
        if (key1 != NULL) {
            key1_bit = crypto_pke_common_get_bit_from_high_to_low(key1, i);
        } else {
            key1_bit = 0;
        }
        bit_2 = (((uint8_t)key1_bit << 1) | (uint8_t)key0_bit);

        if (batch_instr[bit_2] != NULL) {
            ret = hal_pke_batch_compute_sec(batch_instr[bit_2]->instr_addr, batch_instr[bit_2]->instr_num);
            crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");
        }
    }
    crypto_chk_return(i != klen * CRYPTO_BITS_IN_BYTE, CRYPTO_ENHANCE_FAILURE, "for loop is not ended\n");

    return CRYPTO_ENHANCE_SUCCESS;
}

void hal_pke_enable_noise(void)
{
    pke_reg_write(PKE_NOISE_EN, 0x2);
}

void hal_pke_disable_noise(void)
{
    pke_reg_write(PKE_NOISE_EN, 0);
}

int32_t hal_pke_set_dram_mask(void)
{
    int32_t ret = CRYPTO_ENHANCE_FAILURE;
    uint32_t mask = 0xE69BC3A7; /* 0xE69BC3A7: default dram mask */

    ret = hal_trng_get_random_romable(&mask);
    if (ret != CRYPTO_SUCCESS) {
        crypto_log_err("hal_trng_get_random_romable error!\n");
        return ret;
    }
    pke_reg_write(PKE_DRAM_MASK, mask);
    return CRYPTO_ENHANCE_SUCCESS;
}

void hal_pke_clean_dram_mask(void)
{
    pke_reg_write(PKE_DRAM_MASK, 0xE69BC3A7);
}