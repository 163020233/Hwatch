/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc compute x implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

/* Only used for hal_pke_alg_ecc_compute_x. */
static int point_multi_mul_sim(const drv_pke_data *u1, const drv_pke_data *u2)
{
    int ret;
    int i = 0;
    int j = 0;
    unsigned char u1_bit = 0;
    unsigned char u2_bit = 0;
    unsigned char bit_2 = 0;
    unsigned int cnt = 0;
    unsigned int loop_num = 0;
    bool first_flag = false;
    const rom_lib *rom_lib_first_list[4] = {    // 4: select by bit_2
        NULL, &instr_ecfp_cpy_p2c_3, &instr_ecfp_cpy_g2c_3, &instr_ecfp_cpy_a2c_3
    };
    const rom_lib *batch_instr_loop[4] = {    // 4: select by bit_2
        &instr_ecfp_mul_c_double_22, &instr_ecfp_mul_p_22_18, &instr_ecfp_mul_g_22_18, &instr_ecfp_mul_jj_22_23
    };

    /*
     * 1) u1->data and u2->data are two byte arrays, their length u1->length and u2->length is the sample.
     * 2) For each bit in u1->data and u2->data, get bit_2 = u1_bit | u2_bit in order.
     * 3) If there is one bit_2 is not zero, then all the next bit_2(contain this time) will call
        inner_batch_instr_process_opt(), the first param depends on bit_2's value.
     */
    for (i = 0; i < (int)u1->length; i++) {
        if (i >= (int)u1->length || i >= (int)u2->length) {
            break;
        }
        for (j = CRYPTO_BITS_IN_BYTE - 1; j >= 0; j--) {
            u1_bit = (u1->data[i] >> j) & 1;
            u2_bit = (u2->data[i] >> j) & 1;
            bit_2 = ((u1_bit << 1) | u2_bit);       // bit_2 = u1_bit | u2_bit
            if (bit_2 == 0) {
                cnt++;
            } else {
                ret = inner_batch_instr_process_opt(rom_lib_first_list[bit_2]);
                crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");
                first_flag = true;
                break;
            }
        }
        if (first_flag) {
            break;
        }
    }

    loop_num = u1->length * CRYPTO_BITS_IN_BYTE - cnt - 1;
    ret = hal_pke_loop_batch_compute_romable(loop_num, u2->data, u1->data, crypto_min(u1->length, u2->length),
        batch_instr_loop);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_ecc_compute_x(const drv_pke_data *u1, const drv_pke_data *u2,
    const drv_pke_ecc_point *q, const drv_pke_data *x)
{
    int ret;
    drv_pke_data pp;
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();
    hal_pke_alg_ecc_init_check();

    pp.data = (unsigned char *)ecc_curve->p;
    pp.length = ecc_curve->ksize;

    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* Step 1: montgomery the input point. */
    /* Here q_jac_x and q_jac_y will set to px and py. */
    ret = inner_ecfp_montgomery_data_aff(q, &pp, NULL);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_montgomery_data_aff failed\n");

    /* Step 2: A = P + Q, from affine to jacobin. */
    hal_pke_set_ram_romable(ecc_addr_gx, init_param->g_jac_x, ecc_curve->ksize);
    hal_pke_set_ram_romable(ecc_addr_gy, init_param->g_jac_y, ecc_curve->ksize);

    /* point add A = G + Q. */
    ret = inner_batch_instr_process_opt(&instr_ecfp_add_ja_verify_18);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");

    /* Step 3: simultaneous calculate multi-multiplication. */
    ret = point_multi_mul_sim(u1, u2);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "point_multi_mul_sim failed\n");

    /* Step 4: trans Jacobin to Affine. */
    ret = inner_ecfp_jac_to_aff(&pp);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_jac_to_aff failed\n");

    /* Step 5: demontgomery, just process, not get data out of the DRAM. */
    ret = inner_ecfp_demontgomery_data_aff(NULL);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_demontgomery_data_aff failed\n");

    /* Step 6: get result out from PKE data RAM. only need to get r.x for compare. */
    hal_pke_get_ram_romable(ecc_addr_cx, x->data, x->length);

    return ret;
}