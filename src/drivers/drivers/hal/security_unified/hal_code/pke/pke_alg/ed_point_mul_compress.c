/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ed point mul compress implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"
#define ED25519_KEY_LEN     32

#define POINT_NAF_INSTR_NUM     3
static int inner_ed_ecfp_mul_naf_cal(unsigned int work_len, const drv_pke_data *k)
{
    int ret;
    rom_lib batch_instr_block[POINT_NAF_INSTR_NUM] = {
        instr_ed25519_mul_a_loop_1, instr_ed25519_mul_p_loop_1, instr_ed25519_mul_loop_0
    };

    /* Step 1: data montgomerize and data copy. the point and the initial parameters should have been set. */
    ret = inner_batch_instr_process(&instr_ed25519_mul_a_pre, work_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* Step 2: NAF point multiplication. */
    ret = inner_point_mul_naf(batch_instr_block, POINT_NAF_INSTR_NUM, k);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_point_mul_naf failed\n");

    return ret;
}

#define JAC_TO_AFF_INSTR_NUM    6
static int inner_ed_ecfp_xyz_to_xy(const drv_pke_data *mod_p)
{
    int ret;
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {
        instr_ed_ecfp_xyz2xy_pre, instr_ed_ecfp_xyz2xy_exp_one_bit_00, instr_ed_ecfp_xyz2xy_exp_one_bit_01,
        instr_ed_ecfp_xyz2xy_post
    };

    ret = inner_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, mod_p);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_jac_to_aff_cal failed\n");

    return ret;
}

int hal_pke_alg_ed_base_point_mul_compress(const drv_pke_data *p, const drv_pke_data *compress_p)
{
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();
    drv_pke_ecc_point g_point = {
        .x = (unsigned char *)ecc_curve->gx,
        .y = (unsigned char *)ecc_curve->gy,
        .length = ecc_curve->ksize
    };

    return hal_pke_alg_ed_point_mul_compress(p, &g_point, compress_p);
}

int hal_pke_alg_ed_point_mul_compress(const drv_pke_data *p, const drv_pke_ecc_point *point,
    const drv_pke_data *compress_p)
{
    int ret;
    unsigned int klen = hal_pke_alg_ecc_get_klen();
    unsigned int work_len = klen / ALIGNED_TO_WORK_LEN_IN_BYTE;
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();
    const drv_pke_data p_data = {
        .data = (unsigned char *)ecc_curve->p,
        .length = klen
    };
    unsigned char pG_point_x[ED25519_KEY_LEN];
    unsigned char pG_point_y[ED25519_KEY_LEN];
    drv_pke_ecc_point pG_point = {
        .x = pG_point_x,
        .y = pG_point_y,
        .length = klen
    };

    hal_pke_set_ram_romable(ed_addr_ax, point->x, klen);
    hal_pke_set_ram_romable(ed_addr_ay, point->y, klen);
    hal_pke_set_ram_romable(ed_addr_m, ecc_curve->p, klen);

    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    ret = inner_ed_ecfp_mul_naf_cal(work_len, p);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ed_ecfp_mul_naf_cal failed\n");

    ret = inner_ed_ecfp_xyz_to_xy(&p_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ed_ecfp_xyz_to_xy failed\n");

    hal_pke_get_ram_romable(ed_addr_cx, pG_point.x, pG_point.length);
    hal_pke_get_ram_romable(ed_addr_cy, pG_point.y, pG_point.length);

    crypto_pke_common_little_big_endian_trans(compress_p->data, pG_point.y, pG_point.length);

    compress_p->data[ED25519_KEY_LEN - 1] |=
        (pG_point.x[ED25519_KEY_LEN - 1] & 0x01) << 7;  /* 7: get least significant bit. */

    return CRYPTO_SUCCESS;
}