/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ed mul sub implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define NEED_PAD_ZERO       1
#define NO_NEED_PAD_ZERO    0
#define ED25519_KEY_LEN     32
#define JAC_TO_AFF_INSTR_NUM    6

/*
 * Compute C = sB - hA, where sB = point_mul(s * G), hA = point_mul(h * A).
 * if point C is equal to R, return success.
 */
int hal_pke_alg_ed_mul_sub(const drv_pke_data *s_data, const drv_pke_data *h_data,
    const drv_pke_ecc_point *A_point, const drv_pke_ecc_point *R_point)
{
    int ret;
    unsigned int klen = hal_pke_alg_ecc_get_klen();
    const drv_pke_ecc_curve *ecc = hal_pke_alg_ecc_get_curve();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();
    unsigned int work_len = klen / ALIGNED_TO_WORK_LEN_IN_BYTE;
    unsigned char result[ED25519_KEY_LEN];
    uint32_t loop_num;
    const rom_lib *batch_instr_loop[4] = {   // 4: select by bit_2.
        &instr_ed25519_mul_loop_0, &instr_ed25519_mul_g_loop_1,
        &instr_ed25519_mul_p_loop_1, &instr_ed25519_mul_a_loop_1
    };

    /* Step 1.1: set data into DRAM */
    hal_pke_set_ram_romable(ed_addr_px, ecc->gx, klen);
    hal_pke_set_ram_romable(ed_addr_py, ecc->gy, klen);
    hal_pke_set_ram_romable(ed_addr_gx, A_point->x, A_point->length);
    hal_pke_set_ram_romable(ed_addr_gy, A_point->y, A_point->length);
    hal_pke_set_ram_romable(ed_addr_m, ecc->p, klen);

    /* Step 1.2 set montgomery parameters into register. */
    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* Step 2: data preparation, include data montgomery and data copy. */
    ret = inner_batch_instr_process(&instr_ed25519_mul_sub_pre, work_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* Step 4: start calculate. */
    loop_num = klen * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_two_bn_high_0_bit_length(s_data, h_data);

    ret =  hal_pke_loop_batch_compute_romable(loop_num, h_data->data, s_data->data, klen, batch_instr_loop);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    /* Step 5: Check whether C is equal to R. */
    hal_pke_set_ram_romable(ed_addr_rx, R_point->x, R_point->length);
    hal_pke_set_ram_romable(ed_addr_ry, R_point->y, R_point->length);

    hal_pke_get_ram_romable(ed_addr_cx, result, ED25519_KEY_LEN);
    crypto_chk_return(inner_drv_is_zero(result, ED25519_KEY_LEN), CRYPTO_FAILURE, "cx is zero\n");

    hal_pke_get_ram_romable(ed_addr_cy, result, ED25519_KEY_LEN);
    crypto_chk_return(inner_drv_is_zero(result, ED25519_KEY_LEN), CRYPTO_FAILURE, "cy is zero\n");

    return CRYPTO_SUCCESS;
}