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

#include "crypto_drv_common.h"
#include "crypto_pke_common.h"

#define TEE_LOCK_CODE   0xA5
#define REE_LOCK_CODE   0xAA

ROM_SHARE_BSS hal_pke_func_table_t g_pke_table;
ROM_SHARE_BSS uint32_t g_work_len;

int32_t hal_pke_lock_romable(void)
{
    /* Step 1. Lock pke. */
    if (g_pke_table.lock != NULL) {
        return g_pke_table.lock();
    }

    pke_reg_write(PKE_LOCK_CTRL, 0x1);
    return hal_pke_lock_wait_done_romable();
}

void hal_pke_unlock_romable(void)
{
    /* Step 1. Cleana pke dram. */
    if (g_pke_table.unlock != NULL) {
        return g_pke_table.unlock();
    }
    pke_reg_write(PKE_DRAM_CLR, 0x1);
    (void)hal_pke_wait_done_romable();

    /* Step 2. Unock pke. */
    pke_reg_write(PKE_LOCK_CTRL, 0x3);
}

#ifndef CONFIG_PKE_LOCK_TIMEOUT_US
#define CONFIG_PKE_LOCK_TIMEOUT_US      6000
#endif

int32_t hal_pke_lock_wait_done_romable(void)
{
    unsigned int i;
    unsigned int lock_code = TEE_LOCK_CODE;
    pke_lock_status lock_status;
    pke_busy busy;

    if (g_pke_table.lock_wait_done_func != NULL) {
        return g_pke_table.lock_wait_done_func();
    }

    if (crypto_get_cpu_type() == CRYPTO_CPU_TYPE_ACPU) {
        lock_code = REE_LOCK_CODE;
    }
    for (i = 0; i < CONFIG_PKE_LOCK_TIMEOUT_US; i++) {
        lock_status.u32 = pke_reg_read(PKE_LOCK_STATUS);
        busy.u32 = pke_reg_read(PKE_BUSY);

        if ((lock_status.bits.pke_lock_stat == lock_code) && (busy.bits.pke_busy == 0)) {
            break;
        }
        crypto_udelay(1);
    }
    if (i >= CONFIG_PKE_LOCK_TIMEOUT_US) {
        hal_pke_debug_romable();
        return ERROR_PKE_LOCK_TIMEOUT;
    }
    return CRYPTO_SUCCESS;
}

void hal_pke_set_mont_para_romable(const uint32_t mont_param[4])
{
    pke_reg_write(PKE_MONT_PARA0, mont_param[3]); /* 3: third word */
    pke_reg_write(PKE_MONT_PARA1, mont_param[2]); /* 2: second word */
#if defined(CONFIG_PKE_MONT_PARAM_128_BIT_SUPPORT)
    pke_reg_write(PKE_MONT_PARA2, mont_param[1]);
    pke_reg_write(PKE_MONT_PARA3, mont_param[0]);
#endif
}

#define PKE_WORK_LEN_DIV        8
void hal_pke_set_work_len_romable(uint32_t work_len)
{
    pke_reg_write(PKE_WORK_LEN, work_len / PKE_WORK_LEN_DIV);
    g_work_len = work_len;
}

void hal_pke_set_ram_romable(uint32_t ram_section, const uint8_t *data, uint32_t data_len)
{
    uint32_t i, j;
    uint32_t val;
    uint32_t random_num;
    uint32_t start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;

    if ((data_len % CRYPTO_WORD_WIDTH) != 0) {
        crypto_log_err("data_len must be align to 4-byte\n");
        return;
    }

    random_num = pke_reg_read(PKE_DRAM_MASK);

    for (i = 0, j = data_len; i < data_len; i += CRYPTO_WORD_WIDTH, j -= CRYPTO_WORD_WIDTH) {
        val = data[j - 1];
        val |= data[j - 2] << 8;    /* 2, 8: shift 8 bits */
        val |= data[j - 3] << 16;   /* 3, 16: shift 16 bits */
        val |= data[j - 4] << 24;   /* 4, 24: shift 24 bits */
        val ^= random_num;
        pke_reg_write(start_dram_addr + i, val);
    }

    for (; i < g_work_len; i += CRYPTO_WORD_WIDTH) {
        pke_reg_write(start_dram_addr + i, random_num);
    }
}

void hal_pke_set_ram_word_all_romable(uint32_t ram_section, uint32_t data_len, uint32_t word)
{
    uint32_t i;
    uint32_t random_num;
    uint32_t start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;

    crypto_unused(data_len);

    random_num = pke_reg_read(PKE_DRAM_MASK);

    for (i = g_work_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        pke_reg_write(start_dram_addr, word ^ random_num) ;
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
}

void hal_pke_set_ram_word_romable(uint32_t ram_section, uint32_t data_len, int32_t word_pos, uint32_t word)
{
    uint32_t i;
    uint32_t random_num;
    uint32_t start_dram_addr = (uint32_t)PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;
    uint32_t word_addr;
    uint32_t local_word_pos;

    crypto_unused(data_len);

    if (word_pos == -1) {
        local_word_pos = (uint32_t)(g_work_len / CRYPTO_WORD_WIDTH - 1);
    } else {
        local_word_pos = (uint32_t)word_pos;
    }

    word_addr = (uint32_t)PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH +
        g_work_len - (uint32_t)(local_word_pos + 1) * CRYPTO_WORD_WIDTH;
    random_num = pke_reg_read(PKE_DRAM_MASK);

    for (i = g_work_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        pke_reg_write(start_dram_addr, random_num);
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
    pke_reg_write(word_addr, word ^ random_num);
}

void hal_pke_get_ram_romable(uint32_t ram_section, uint8_t *data, uint32_t data_len)
{
    uint32_t i;
    uint32_t val;
    uint32_t random_num;
    uint32_t start_dram_addr = PKE_DRAM_BASE + ram_section * PKE_DRAM_BLOCK_LENGTH;

    random_num = pke_reg_read(PKE_DRAM_MASK);

    for (i = data_len; i >= CRYPTO_WORD_WIDTH; i -= CRYPTO_WORD_WIDTH) {
        val = pke_reg_read(start_dram_addr);
        val ^= random_num;
        data[i - 4] = (val >> 24) & 0xFF; /* 4, 24: shift 24 bits */
        data[i - 3] = (val >> 16) & 0xFF; /* 3, 16: shift 16 bits */
        data[i - 2] = (val >> 8) & 0xFF;  /* 2, 8: offset 2, shift 8 bits */
        data[i - 1] = (val) & 0xFF;       /* 1: offset 1 */
        start_dram_addr += CRYPTO_WORD_WIDTH;
    }
}

int32_t hal_pke_batch_compute_romable(uint32_t instr_addr, uint32_t instr_num)
{
    if (g_pke_table.batch_trace_func != NULL) {
        g_pke_table.batch_trace_func(instr_addr, instr_num);
    }

    pke_reg_write(PKE_INSTR_ADDR_LOW, instr_addr);
    pke_reg_write(PKE_INSTR_ADDR_HIG, 0);    /* compatible with old version IP */
    pke_reg_write(PKE_INSTR_LEN, instr_num * CRYPTO_WORD_WIDTH);
    pke_reg_write(PKE_START, 0x5AA);

    return hal_pke_wait_done_romable();
}

#if defined(CONFIG_PKE_LOOP_COMPUTE_SUPPORT)
#define SHIFT_16_BIT            16
#define PKE_LOOP_START_VALUE    0x5
#define PKE_LOOP_INSTR_NUM      4
static int32_t inner_fast_batch_compute_romable(uint32_t loop_num,
    const unsigned char *key0, const unsigned char *key1,
    unsigned int klen, const rom_lib *batch_instr[4])
{
    uint32_t i;
    /* set key0, key1 */
    hal_pke_set_ram_romable(ecc_addr_key0, key0, klen);
    if (key1 != NULL) {
        hal_pke_set_ram_romable(ecc_addr_key1, key1, klen);
    } else {
        hal_pke_set_ram_word_all_romable(ecc_addr_key1, klen, 0);
    }

    /* set batch_loop_instr0~3 */
    for (i = 0; i < PKE_LOOP_INSTR_NUM; i++) {
        if (batch_instr[i] != NULL) {
            pke_reg_write(PKE_LOOP_INSTR(i),
                (batch_instr[i]->instr_addr & 0xffff) |
                ((batch_instr[i]->instr_num * CRYPTO_WORD_WIDTH) << SHIFT_16_BIT));
        }
    }
    pke_reg_write(PKE_LOOP_CMD, PKE_LOOP_START_VALUE | (loop_num << SHIFT_16_BIT));

    return hal_pke_wait_done_romable();
}

#else
static int32_t inner_normal_batch_compute_romable(uint32_t loop_num, const unsigned char *key0,
    const unsigned char *key1, unsigned int klen, const rom_lib *batch_instr[4])
{
    int ret;
    uint32_t i = 0;
    bool key0_bit;
    bool key1_bit;
    unsigned char bit_2;

    if (loop_num > klen * CRYPTO_BITS_IN_BYTE) {
        return CRYPTO_FAILURE;
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
            ret = hal_pke_batch_compute_romable(batch_instr[bit_2]->instr_addr, batch_instr[bit_2]->instr_num);
            crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");
        }
    }
    return CRYPTO_SUCCESS;
}
#endif

/* key1 can be NULL, which means key1 is const_0 */
int32_t hal_pke_loop_batch_compute_romable(uint32_t loop_num,
    const unsigned char *key0, const unsigned char *key1,
    unsigned int klen, const rom_lib *batch_instr[4])
{
    if (loop_num == 0) {
        return CRYPTO_SUCCESS;
    }
#if defined(CONFIG_PKE_LOOP_COMPUTE_SUPPORT)
    return inner_fast_batch_compute_romable(loop_num, key0, key1, klen, batch_instr);
#else
    return inner_normal_batch_compute_romable(loop_num, key0, key1, klen, batch_instr);
#endif
}

int32_t hal_pke_single_compute_romable(uint32_t single_instr)
{
    pke_reg_write(PKE_INSTR0, single_instr);
    pke_reg_write(PKE_START, 0xAA5);

    return hal_pke_wait_done_romable();
}

#ifndef CONFIG_PKE_CALC_TIMEOUT_US
#define CONFIG_PKE_CALC_TIMEOUT_US      60000
#endif

int32_t hal_pke_wait_done_romable(void)
{
    int32_t ret = CRYPTO_SUCCESS;
    uint32_t i;
    pke_busy busy;
    pke_alarm_status result = {.u32 = PKE_NON_SPECIAL_VAL};

    if (g_pke_table.wait_done_func != NULL) {
        return g_pke_table.wait_done_func();
    }

    /* wait ready */
    for (i = 0; i < CONFIG_PKE_CALC_TIMEOUT_US; i++) {
        busy.u32 = pke_reg_read(PKE_BUSY);
        if (busy.bits.pke_busy == 0) {
            break;
        }
        crypto_udelay(1); /* 1us */
    }
    if (i >= CONFIG_PKE_CALC_TIMEOUT_US) {
        ret = ERROR_PKE_WAIT_DONE_TIMEOUT;
    }

    if (pke_reg_read(PKE_FAILURE_FLAG) != 0) {
        ret = ERROR_PKE_LOGIC;
    }
    /* robust error check. */
    result.u32 = pke_reg_read(PKE_ALARM_STATUS);
    if (result.bits.alarm_int == PKE_ALARM_STATUS_EFFECTIVE_CODE) {
        pke_reg_write(PKE_ALARM_STATUS, PKE_ALARM_STATUS_CLEAN_CODE);   // clear warn interrupt
        ret = ERROR_PKE_ROBUST_WARNING;
    }

    if (ret != CRYPTO_SUCCESS) {
        hal_pke_debug_romable();
    }
    return ret;
}

void hal_pke_debug_romable(void)
{
    if (g_pke_table.debug_func != NULL) {
        g_pke_table.debug_func();
    }
}

void hal_pke_register_romable(const hal_pke_func_table_t *func_table)
{
    (void)memcpy_s(&g_pke_table, sizeof(hal_pke_func_table_t), func_table, sizeof(hal_pke_func_table_t));
}