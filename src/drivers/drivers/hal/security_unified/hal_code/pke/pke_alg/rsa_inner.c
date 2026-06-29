/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke rsa inner implement.
 *
 * Create: 2025-01-07
*/
#include "pke_alg_inner.h"

#include "hal_pke_romable.h"
#include "hal_pke_romable.h"

#include "crypto_pke_common.h"

#define PKE_MONT_BIT_LEN        128
#define PKE_MONT_LEN_IN_BYTE    16
#define PKE_MONT_LEN_IN_WORD    4
#define ALIGNED_TO_WORK_LEN_IN_BYTE 8

typedef struct {
    unsigned int aligned_len;
    const rom_lib *batch_instr;
    unsigned int loop_number;
} rrn_loop_table;

#define DRV_PKE_CAL_LEN_1536    192
#define DRV_PKE_CAL_LEN_320     40
#define LOOP_NUMBER_4096    8
#define LOOP_NUMBER_3072    10
#define LOOP_NUMBER_2048    7
#define LOOP_NUMBER_1536    9
#define LOOP_NUMBER_1024    6
#define LOOP_NUMBER_192     6
#define LOOP_NUMBER_256     4
#define LOOP_NUMBER_320     6
#define LOOP_NUMBER_384     7
#define LOOP_NUMBER_512     5

#if defined(CONFIG_ROMABLE_API_SUPPORT)
static const rrn_loop_table g_rrn_loop_table[] = {
    {DRV_PKE_LEN_3072, &instr_rsa_rrn_add_3072, LOOP_NUMBER_3072},
};
#else
static const rrn_loop_table g_rrn_loop_table[] = {
    {DRV_PKE_LEN_3072, &instr_rsa_rrn_add_3072, LOOP_NUMBER_3072},
    {DRV_PKE_CAL_LEN_1536, &instr_rsa_rrn_add_3072, LOOP_NUMBER_1536},
    {DRV_PKE_LEN_192, &instr_rsa_rrn_add_3072, LOOP_NUMBER_192},
    {DRV_PKE_LEN_256, &instr_rsa_rrn_add, LOOP_NUMBER_256},
    {DRV_PKE_CAL_LEN_320, &instr_rsa_rrn_add_320, LOOP_NUMBER_320},
    {DRV_PKE_LEN_384, &instr_rsa_rrn_add_3072, LOOP_NUMBER_384},
    {DRV_PKE_LEN_512, &instr_rsa_rrn_add, LOOP_NUMBER_512},
    {DRV_PKE_LEN_1024, &instr_rsa_rrn_add, LOOP_NUMBER_1024},
    {DRV_PKE_LEN_2048, &instr_rsa_rrn_add, LOOP_NUMBER_2048},
    {DRV_PKE_LEN_4096, &instr_rsa_rrn_add, LOOP_NUMBER_4096},
};
#endif

static int inner_get_loop_number(unsigned int aligned_len, unsigned int *loop_number)
{
    int ret;
    unsigned int i = 0;
    unsigned int work_len = aligned_len / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step3: prepare data for loop calculate. */
    for (; i < crypto_array_size(g_rrn_loop_table); i++) {
        if (aligned_len == g_rrn_loop_table[i].aligned_len) {
            ret = inner_batch_instr_process(g_rrn_loop_table[i].batch_instr, work_len);
            crypto_chk_func_return(inner_batch_instr_process, ret);
            *loop_number = g_rrn_loop_table[i].loop_number;
            return CRYPTO_SUCCESS;
        }
    }
    crypto_log_err("invalid modulur\n");
    return PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM);
}

static int inner_rsa_rrn(const unsigned char *n, const unsigned int key_size)
{
    int ret;
    unsigned int cnt = 0;
    unsigned int i = 0;

    /* Step 1: set data to PKE DRAM. */
    hal_pke_set_ram_romable(rsa_addr_n, n, key_size);

    /* Step 2: set data initial data rr = 2^(bit_len - 1). */
    hal_pke_set_ram_word_romable(rsa_addr_rr, key_size, 0, 0x80000000);  // 0x80000000: highest bit is 1

    /* Step3: prepare data for loop calculate. */
    ret = inner_get_loop_number(key_size, &cnt);
    crypto_chk_func_return(inner_get_loop_number, ret);

    /* Step 4.2: start calculate square multiplication modulur. */
    for (i = 0; i < cnt; i++) {
        ret = inner_batch_instr_process(&instr_rsa_rrn_mul, 0);
        crypto_chk_func_return(inner_batch_instr_process, ret);
    }
    return ret;
}

static void inner_set_mont_param(const uint8_t *p, uint32_t p_len)
{
    const unsigned char *p_last_bytes = &p[p_len - PKE_MONT_LEN_IN_BYTE];
    unsigned int mont_param_word[PKE_MONT_LEN_IN_WORD];

    crypto_pke_common_get_mont_param(p_last_bytes, (unsigned char *)mont_param_word);

    mont_param_word[0] = crypto_cpu_to_be32(mont_param_word[0]);
    mont_param_word[1] = crypto_cpu_to_be32(mont_param_word[1]);
    mont_param_word[2] = crypto_cpu_to_be32(mont_param_word[2]); /* 2: second word */
    mont_param_word[3] = crypto_cpu_to_be32(mont_param_word[3]); /* 3: third word */

    hal_pke_set_mont_para_romable(mont_param_word);
}

int inner_update_rsa_modulus(const unsigned char *n, const unsigned int n_len)
{
    /* 1. get new montgomery parameters and set it into register. */
    inner_set_mont_param(n, n_len);

    /* 2. calculate to get rrn value. */
    return inner_rsa_rrn(n, n_len);
}