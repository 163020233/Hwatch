/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal pke ecc point mul op implement.
 *
 * Create: 2025-06-23
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
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

int32_t hal_pke_alg_point_mul_add(const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int ret;
    drv_pke_data pp;
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();

    pp.data = (unsigned char *)ecc_curve->p;
    pp.length = ecc_curve->ksize;

    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* Step 1: montgomery the input point. */
    if (p == NULL) {    // use base point G as point P.
        hal_pke_set_ram_romable(ecc_addr_gx, init_param->g_jac_x, ecc_curve->ksize);
        hal_pke_set_ram_romable(ecc_addr_gy, init_param->g_jac_y, ecc_curve->ksize);
    } else {
        /* The r is reuse, for same addr support, before this cal every input data should have been set into ram. */
        ret = inner_ecfp_montgomery_data_aff(p, &pp, r);
        crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_montgomery_data_aff failed\n");

        hal_pke_set_ram_romable(ecc_addr_gx, r->x, ecc_curve->ksize);
        hal_pke_set_ram_romable(ecc_addr_gy, r->y, ecc_curve->ksize);
    }

    /* Here q_jac_x and q_jac_y will set to px and py. */
    ret = inner_ecfp_montgomery_data_aff(q, &pp, NULL);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_montgomery_data_aff failed\n");

    /* Step 2: A = P + Q, from affine to jacobin. */
    /* point add A = G + Q. */
    ret = inner_batch_instr_process_opt(&instr_ecfp_add_ja_verify_18);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");

    /* Step 3: simultaneous calculate multi-multiplication. */
    ret = point_multi_mul_sim(a, b);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "point_multi_mul_sim failed\n");

    /* Step 4: trans Jacobin to Affine. */
    ret = inner_ecfp_jac_to_aff(&pp);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_jac_to_aff failed\n");

    /* Step 5: demontgomery, just process, not get data out of the DRAM. */
    ret = inner_ecfp_demontgomery_data_aff(NULL);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_demontgomery_data_aff failed\n");

    /* Step 6: get result out from PKE data RAM. */
    hal_pke_get_ram_romable(ecc_addr_cx, r->x, r->length);
    hal_pke_get_ram_romable(ecc_addr_cy, r->y, r->length);

    return ret;
}

/* r = a * P(x, y) - b * Q(x, y) means r = a * P(x, y) + b * Q(x, p - y). */
int32_t hal_pke_alg_point_mul_sub(const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r)
{
    int ret;
    uint8_t q_y_inv[MAX_ECC_SIZE];
    drv_pke_data p_data;
    drv_pke_data q_y_data;
    drv_pke_data q_y_inv_data;
    drv_pke_ecc_point q_inv_point;
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();

    p_data.data = (uint8_t *)ecc_curve->p;
    p_data.length = ecc_curve->ksize;
    q_y_data.data = q->y;
    q_y_data.length = ecc_curve->ksize;
    q_y_inv_data.data = q_y_inv;
    q_y_inv_data.length = ecc_curve->ksize;

    /* Step 1. Compute q_y_inv = p - q_y. */
    ret = crypto_pke_common_normal_sub(&p_data, &q_y_data, &q_y_inv_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_pke_common_normal_sub failed\n");

    /* Step 2. Compute r = a * P(x, y) + b * Q_inv(x, y). */
    q_inv_point.x = q->x;
    q_inv_point.y = q_y_inv_data.data;
    q_inv_point.length = q->length;
    ret = hal_pke_alg_point_mul_add(a, b, p, &q_inv_point, r);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_alg_point_mul_add failed\n");

    return CRYPTO_SUCCESS;
}