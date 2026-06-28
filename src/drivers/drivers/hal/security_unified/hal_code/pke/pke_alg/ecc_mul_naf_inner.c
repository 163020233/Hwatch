/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc mul naf inner implement.
 *
 * Create: 2025-01-07
*/
#include "pke_alg_inner.h"

#include "hal_pke_romable.h"
#include "hal_pke_alg.h"
#include "crypto_pke_common.h"
#include "pke_alg_inner.h"

int inner_point_mul_naf(const rom_lib *batch_instr, const unsigned int batch_instr_num,
    const drv_pke_data *k)
{
    int ret;
    uint8_t *buf = NULL;
    uint32_t buf_len = MAX_NAF_LEN * 2; // 2: for positive/negative
    uint8_t *k_naf_positive_bits = NULL;
    uint8_t *k_naf_negative_bits = NULL;
    drv_pke_data positive_data;
    unsigned int carry = 0;
    const rom_lib *batch_instr_loop[] = {&batch_instr[2], &batch_instr[0], &batch_instr[1], &batch_instr[2]};
    uint32_t loop_num = k->length * CRYPTO_BITS_IN_BYTE;    /* loop_num if carry != 0. */

    buf = crypto_malloc_coherent(buf_len);
    crypto_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    k_naf_positive_bits = buf;
    k_naf_negative_bits = k_naf_positive_bits + MAX_NAF_LEN;
    /* Step 1: convert k to non-adjacent-form. */
    ret = crypto_pke_common_bin2naf_fast(k->data, k_naf_positive_bits, k_naf_negative_bits, k->length, &carry);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "crypto_pke_common_bin2naf_fast failed\n");

    crypto_unused(batch_instr_num);
    /* before calculate naf_point_mul, you need to set mont_a to ecc_addr_mont_a first.
       And keep ecc->p in the modulur. */
    /* Step 2: calculate point multiplication by double point and point plus. k_naf couldn't be infinity point. */
    /* instr_ecfp_mul_p_22_18[0,40], instr_ecfp_mul_g_22_18[40,40], instr_ecfp_mul_c_double_22[0,22] */

    /* skip prefix zeros.  */
    if (carry == 0) {
        positive_data.data = k_naf_positive_bits;
        positive_data.length = k->length;
        loop_num = k->length * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length(&positive_data) - 1;
    }

    ret = hal_pke_loop_batch_compute_romable(loop_num, k_naf_positive_bits, k_naf_negative_bits, k->length,
        batch_instr_loop);
    crypto_chk_goto(ret != CRYPTO_SUCCESS, exit_free, "hal_pke_loop_batch_compute_romable failed\n");
exit_free:
    (void)memset_s(buf, buf_len, 0, buf_len);
    crypto_free_coherent(buf);
    return ret;
}

#define POINT_NAF_INSTR_NUM     3
int inner_ecfp_mul_naf_cal(unsigned int work_len, const drv_pke_data *k)
{
    volatile int ret;
    rom_lib batch_instr_block[POINT_NAF_INSTR_NUM] = {instr_ecfp_mul_p_22_18, instr_ecfp_mul_g_22_18,
        instr_ecfp_mul_c_double_22};

    crypto_drv_func_enter();

    /* WARN: from here, there should be no other process which is not ecc, for the result in DRAM wasn't got out, other
    process will destroy data in the DRAM. */
    /* 3. copy negative point data */
    ret = inner_batch_instr_process(&instr_ecfp_cpy_np2g_2, work_len);
    crypto_chk_func_return(inner_batch_instr_process, ret);

    /* 4. NAF point multiplication. */
    ret = inner_point_mul_naf(batch_instr_block, POINT_NAF_INSTR_NUM, k);
    crypto_chk_func_return(inner_point_mul_naf, ret);

    crypto_drv_func_exit();
    return ret;
}