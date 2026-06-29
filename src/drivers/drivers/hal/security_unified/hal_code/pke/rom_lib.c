/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: romlib instruction functions content.
 *
 * Create: 2022-08-16
 */

#include "crypto_drv_common.h"
#include "crypto_common_def.h"
#include "rom_lib.h"

/*
 * preprocess, initialize the data, include copy and generate tmpera variable.
 * [fp_mul, fp_mul, fp_mul, fp_sub, fp_add]
 * [415359168, 1119903936, 1220567568, 2730659090, 2730640657]
 */
const rom_lib instr_ecfp_j2a_pre_5 = {
    .instr_addr = 143 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 5,
};

/* ************************************************* ecc instr start*********************************** */

/* mul mod while e_bin[i:i+2] == '00'. */
const rom_lib instr_ecfp_j2a_exp_00 = {
    .instr_addr = 148 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* mul mod while e_bin[i:i+2] == '01'. */
const rom_lib instr_ecfp_j2a_exp_01 = {
    .instr_addr = 148 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* mul mod while e_bin[i:i+2] == '10'. */
const rom_lib instr_ecfp_j2a_exp_10 = {
    .instr_addr = 151 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* mul mod while e_bin[i:i+2] == '11'. */
const rom_lib instr_ecfp_j2a_exp_11 = {
    .instr_addr = 154 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

const rom_lib instr_ecfp_j2a_exp_one_bit_00 = {
    .instr_addr = 148 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

const rom_lib instr_ecfp_j2a_exp_one_bit_01 = {
    .instr_addr = 149 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* postprocess, include the mul mod. */
const rom_lib instr_ecfp_j2a_post_4 = {
    .instr_addr = 157 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

/* demontgomery affine point, demont and fully sub mod. */
const rom_lib instr_ecfp_demont_c_6 = {
    .instr_addr = 165 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* demont and fully sub mod for z. */
const rom_lib instr_ecfp_demont_cz_3 = {
    .instr_addr = 171 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* montgomery point data. */
const rom_lib instr_ecfp_mont_p_2 = {
    .instr_addr = 163 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* affine coordinate system to jacobin coordinate system. */
const rom_lib instr_ecfp_cpy_p2c_3 = {
    .instr_addr = 177 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* copy the negative point of P(px,py) to G(gx,gy). */
const rom_lib instr_ecfp_cpy_np2g_2 = {
    .instr_addr = 180 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* double point and point add algorithm while ki is 1. */
const rom_lib instr_ecfp_mul_p_22_18 = {
    .instr_addr = 0 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 40,
};

/* double point and point add algorithm while ki is -1. */
const rom_lib instr_ecfp_mul_g_22_18 = {
    .instr_addr = 40 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 40,
};

/* double point. */
const rom_lib instr_ecfp_mul_c_double_22 = {
    .instr_addr = 0 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 22,
};

/* copy new modulus into address 0. */
const rom_lib instr_ecfp_prime_n_1 = {
    .instr_addr = 197 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

/* copy new modulus into address 0. */
const rom_lib instr_ecfn_sign_r_2 = {
    .instr_addr = 198 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* the former 5 process for inv calculate to get XXX. */
const rom_lib instr_ecfn_inv_pre_5 = {
    .instr_addr = 200 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 5,
};

const rom_lib instr_ecfn_inv_exp_00 = {
    .instr_addr = 205 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

const rom_lib instr_ecfn_inv_exp_01 = {
    .instr_addr = 205 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

const rom_lib instr_ecfn_inv_exp_10 = {
    .instr_addr = 208 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ecc_addr_s * ecc_addr_t1. */
const rom_lib instr_ecfn_inv_exp_11 = {
    .instr_addr = 211 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ecc_addr_s * ecc_addr_k. */
const rom_lib instr_ecfn_inv_exp_one_bit_00 = {
    .instr_addr = 205 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

/* multiplication modulus calculate ecc_addr_s * ecc_addr_t0. */
const rom_lib instr_ecfn_inv_exp_one_bit_01 = {
    .instr_addr = 206 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* calculate the s for ecdsa signature. */
const rom_lib instr_ecfn_sign_s_12 = {
    .instr_addr = 214 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 12,
};

/* calculate the u1 and u2 for ecdsa verify. */
const rom_lib instr_ecfn_verify_u_10 = {
    .instr_addr = 226 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 10,
};

/* calculate the A = P + Q and set data into ecc_addr_ax, ecc_addr_ay, ecc_addr_az. */
const rom_lib instr_ecfp_add_ja_verify_18 = {
    .instr_addr = 125 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 18,
};

/* affine coordinate system to jacobin coordinate system. */
const rom_lib instr_ecfp_cpy_a2c_3 = {
    .instr_addr = 182 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* affine coordinate system to jacobin coordinate system. */
const rom_lib instr_ecfp_cpy_g2c_3 = {
    .instr_addr = 174 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* jacobin double point and jac + jac calculation. */
const rom_lib instr_ecfp_mul_jj_22_23 = {
    .instr_addr = 80 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 45,
};

/* jacobin double point and jac + jac calculation. */
const rom_lib instr_ecfn_verify_v_4 = {
    .instr_addr = 236 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

/* ************************************************* ecc instr end*********************************** */

/* ************************************************* sm2 instr start*********************************** */
/* ecc ecfp modular addition. */
const rom_lib instr_sm2_sign_r_1 = {
    .instr_addr = 240 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

/* data montgomery and calculate the (1+da) & (k - r*da). */
const rom_lib instr_sm2_sign_s_pre_6 = {
    .instr_addr = 241 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* calculate the (1+da)^(-1) * (k - r*da) and demontgomery the data. */
const rom_lib instr_sm2_sign_s_post_6 = {
    .instr_addr = 247 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* calculate the (1+da)^(-1) * (k - r*da) and demontgomery the data. */
const rom_lib instr_sm2_verify_t_3 = {
    .instr_addr = 253 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};
/* ************************************************* sm2 instr end*********************************** */

/* ************************************************* curve instr start*********************************** */
/* point add and times point calculation while k_bin[i] == 0. */
const rom_lib instr_curve25519_ladder_0 = {
    .instr_addr = 332 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 18,
};

/* point add and times point calculation while k_bin[i] == 1. */
const rom_lib instr_curve25519_ladder_1 = {
    .instr_addr = 350 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 18,
};

/* x coordinate montgomerized and data copy. */
const rom_lib instr_curve25519_pre_ladder = {
    .instr_addr = 368 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 5,
};

/* data initialize: data copy. */
const rom_lib instr_curve25519_xz2x_pre = {
    .instr_addr = 373 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 5,
};

/* twice square calculate curve_addr_t0 * curve_addr_t0. */
const rom_lib instr_curve25519_xz2x_exp_00 = {
    .instr_addr = 378 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* multiplication modulus calculate curve_addr_t0 * curve_addr_t1. */
const rom_lib instr_curve25519_xz2x_exp_01 = {
    .instr_addr = 378 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate curve_addr_t0 * curve_addr_t2. */
const rom_lib instr_curve25519_xz2x_exp_10 = {
    .instr_addr = 381 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate curve_addr_t0 * curve_addr_t3. */
const rom_lib instr_curve25519_xz2x_exp_11 = {
    .instr_addr = 384 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

const rom_lib instr_curve25519_xz2x_exp_one_bit_00 = {
    .instr_addr = 378 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

const rom_lib instr_curve25519_xz2x_exp_one_bit_01 = {
    .instr_addr = 379 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* demontgomery and twice modular reduction, result in curve_addr_x2. */
const rom_lib instr_curve25519_xz2x_post = {
    .instr_addr = 387 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};
/* ************************************************* curve instr end*********************************** */

/* ************************************************* ed instr start*********************************** */
/* mul_naf: times point calculation while k_bin[i] == 0. */
const rom_lib instr_ed25519_mul_loop_0 = {
    .instr_addr = 391 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 14,
};

/* mul_naf: point add and times point calculation while k_bin[i] == 1. */
const rom_lib instr_ed25519_mul_a_loop_1 = {
    .instr_addr = 391 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 33,
};

/* mul_naf: point add and times point calculation while k_bin[i] == -1. */
const rom_lib instr_ed25519_mul_p_loop_1 = {
    .instr_addr = 424 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 33,
};

/* double point and point add while si == 0 && hi == 1. */
const rom_lib instr_ed25519_mul_g_loop_1 = {
    .instr_addr = 457 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 33,
};

/* data initialize: data montgomerized and data copy. */
const rom_lib instr_ed25519_mul_a_pre = {
    .instr_addr = 490 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 12,
};

/* data initialize: data copy. */
const rom_lib instr_ed_ecfp_xyz2xy_pre = {
    .instr_addr = 502 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 5,
};

/* twice square calculate ed_addr_t0 * ed_addr_t0. */
const rom_lib instr_ed_ecfp_xyz2xy_exp_00 = {
    .instr_addr = 507 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* multiplication modulus calculate ed_addr_t0 * ed_addr_t1. */
const rom_lib instr_ed_ecfp_xyz2xy_exp_01 = {
    .instr_addr = 507 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ed_addr_t0 * ed_addr_t2. */
const rom_lib instr_ed_ecfp_xyz2xy_exp_10 = {
    .instr_addr = 510 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ed_addr_t0 * ed_addr_t3. */
const rom_lib instr_ed_ecfp_xyz2xy_exp_11 = {
    .instr_addr = 513 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

const rom_lib instr_ed_ecfp_xyz2xy_exp_one_bit_00 = {
    .instr_addr = 507 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

const rom_lib instr_ed_ecfp_xyz2xy_exp_one_bit_01 = {
    .instr_addr = 508 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* demontgomery and twice modular reduction. */
const rom_lib instr_ed_ecfp_xyz2xy_post = {
    .instr_addr = 516 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 8,
};

/* data preparation, include data montgomery and data copy. */
const rom_lib instr_ed25519_mul_sub_pre = {
    .instr_addr = 524 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 33,
};

/* modulo operation for ed25519/ed448. */
const rom_lib instr_ed25519_calc_h = {
    .instr_addr = 566 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 7,
};

/* calculate the result s of signature. */
const rom_lib instr_ed25519_calc_s = {
    .instr_addr = 573 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 8,
};

/* data initialize: data montgomerized and data copy. */
const rom_lib instr_ed25519_recover_x_pre = {
    .instr_addr = 581 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 15,
};

/* twice square calculate ed_addr_t4 * ed_addr_t4. */
const rom_lib instr_ed25519_recover_x_exp_00 = {
    .instr_addr = 596 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* multiplication modulus calculate ed_addr_t4 * ed_addr_t1. */
const rom_lib instr_ed25519_recover_x_exp_01 = {
    .instr_addr = 596 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ed_addr_t4 * ed_addr_t2. */
const rom_lib instr_ed25519_recover_x_exp_10 = {
    .instr_addr = 599 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate ed_addr_t4 * ed_addr_t3. */
const rom_lib instr_ed25519_recover_x_exp_11 = {
    .instr_addr = 602 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

const rom_lib instr_ed25519_recover_x_exp_one_bit_00 = {
    .instr_addr = 596 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

const rom_lib instr_ed25519_recover_x_exp_one_bit_01 = {
    .instr_addr = 597 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* demontgomery and twice modular reduction, result in ed_addr_tx0 and ed_addr_tx1. */
const rom_lib instr_ed25519_recover_x_post = {
    .instr_addr = 605 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 22,
};

/* data copy: ed_addr_tx1 = ed_addr_m - ed_addr_tx0. */
const rom_lib instr_ed25519_cpy_tx0_to_tx1 = {
    .instr_addr = 627 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

/* copy and move: ed_addr_tx0 = ed_addr_tx1, ed_addr_tx1 = ed_addr_m - ed_addr_tx1. */
const rom_lib instr_ed25519_cpy_tx1_to_tx0 = {
    .instr_addr = 628 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* copy data from ed_addr_tx0, ed_addr_ty to ed_addr_gx, ed_addr_gy. */
const rom_lib instr_ed25519_cpy_t2g = {
    .instr_addr = 630 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* copy data from ed_addr_tx0, ed_addr_ty to ed_addr_rx, ed_addr_ry. */
const rom_lib instr_ed25519_cpy_t2r = {
    .instr_addr = 632 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* ed_addr_cx(cy) = ed_addr_cx(cy) - ed_addr_rx(ry) * ed_addr_cz. */
const rom_lib instr_ed25519_c_eq_r = {
    .instr_addr = 634 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 12,
};
/* ************************************************* ed instr end*********************************** */

/* ************************************************* rsa instr start*********************************** */
/* data montgomery. */
const rom_lib instr_rsa_exp_pre_6 = {
    .instr_addr = 268 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* twice square calculate rsa_addr_s * rsa_addr_s. */
const rom_lib instr_rsa_exp_00 = {
    .instr_addr = 274 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* multiplication modulus calculate rsa_addr_s * rsa_addr_a. */
const rom_lib instr_rsa_exp_01 = {
    .instr_addr = 274 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate rsa_addr_s * rsa_addr_t0. */
const rom_lib instr_rsa_exp_10 = {
    .instr_addr = 277 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* multiplication modulus calculate rsa_addr_s * rsa_addr_t1. */
const rom_lib instr_rsa_exp_11 = {
    .instr_addr = 280 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* demontgomery and twice modular reduction. */
const rom_lib instr_rsa_exp_post_3 = {
    .instr_addr = 283 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};

/* big data regular multiplication. */
const rom_lib instr_rsa_regular_mul = {
    .instr_addr = 286 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

/* pre 17 times add_mod for rrn calculate. */
const rom_lib instr_rsa_rrn_add = {
    .instr_addr = 287 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 17,
};

/* pre 17 times add_mod for rrn calculate. */
const rom_lib instr_rsa_rrn_add_3072 = {
    .instr_addr = 287 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

/* pre 17 times add_mod for rrn calculate. */
const rom_lib instr_rsa_rrn_add_320 = {
    .instr_addr = 287 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* multiplication modular for square calculate. */
const rom_lib instr_rsa_rrn_mul = {
    .instr_addr = 304 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 1,
};

const rom_lib instr_rsa_mod = {
    .instr_addr = 305 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 7,
};

/* demont and fully sub mod for z. */
const rom_lib instr_rsa_mod_add = {
    .instr_addr = 322 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* demont and fully sub mod for z. */
const rom_lib instr_rsa_mod_sub = {
    .instr_addr = 324 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 2,
};

/* montgomery multiplication modular with twice modular reduction. */
const rom_lib instr_rsa_mod_mul = {
    .instr_addr = 326 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

/* *********************************************** ecc blind old instr start********************************* */
const rom_lib instr_ecfp_pm_blind_0 = {
    .instr_addr = 851 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 45,
};

const rom_lib instr_ecfp_pm_blind_1 = {
    .instr_addr = 896 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 45,
};

const rom_lib instr_ecfp_pm_blind_2 = {
    .instr_addr = 941 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 45,
};

const rom_lib instr_ecfp_pm_blind_3 = {
    .instr_addr = 986 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 45,
};
/* *********************************************** ecc blind old instr end********************************* */

/* *********************************************** ecc blind new instr start********************************* */
const rom_lib instr_ecfp_blind_gen_point_r = {
    .instr_addr = 1108 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 29,
};

const rom_lib instr_ecfp_blind_gen_point_t_new = {
    .instr_addr = 1137 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 92,
};

const rom_lib instr_ecfp_blind_mix_point_t_0_new = {
    .instr_addr = 1229 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 19,
};

const rom_lib instr_ecfp_blind_mix_point_t_1_new = {
    .instr_addr = 1248 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 22,
};

const rom_lib instr_ecfp_blind_mix_point_t_2_new = {
    .instr_addr = 1270 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 22,
};

const rom_lib instr_ecfp_blind_mix_point_t_3_new = {
    .instr_addr = 1292 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 22,
};

const rom_lib instr_ecfp_blind_init_point_q_0 = {
    .instr_addr = 1314 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

const rom_lib instr_ecfp_blind_init_point_q_1 = {
    .instr_addr = 1318 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

const rom_lib instr_ecfp_blind_init_point_q_2 = {
    .instr_addr = 1322 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

const rom_lib instr_ecfp_blind_init_point_q_3 = {
    .instr_addr = 1326 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

const rom_lib instr_ecfn_inv_sec_pre_new = {
    .instr_addr = 1330 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 9,
};

const rom_lib instr_ecfn_sign_s_rand_new = {
    .instr_addr = 1339 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 17,
};

const rom_lib instr_sm2_sign_s_pre_rand_new = {
    .instr_addr = 1356 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 12,
};

const rom_lib instr_sm2_sign_s_post_rand_new = {
    .instr_addr = 1368 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 6,
};

const rom_lib instr_ecfp_blind_key_t = {
    .instr_addr = 1374 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 4,
};

const rom_lib instr_ecfp_blind_key_s = {
    .instr_addr = 1378 * CRYPTO_WORD_WIDTH + PKE_ROM_LIB_START_ADDR,
    .instr_num = 3,
};
/* *********************************************** ecc blind instr end********************************* */

#ifdef DEBUG_IN_RAM
const td_u32 g_instr_rom[] = {1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1220561040, 1321224768, 1220616528, 1321286016, 1220616242, 1321218674, 314646672, 1421943360, 1220616864, 1421955120, 113326704, 113319985, 213922368, 1220629153, 113319986, 1421955122, 1421955696, 213922466, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1220561040, 1321224768, 1220616624, 1321286112, 1220616242, 1321218674, 314646672, 1421943360, 1220616864, 1421955120, 113326704, 113319985, 213922368, 1220629153, 113319986, 1421955122, 1421955696, 213922466, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220579616, 1321279536, 1421936832, 1220616480, 1220616288, 1119946896, 1119946992, 1421949602, 314609808, 314591904, 1522619040, 1421955792, 1119953426, 1321286352, 113314320, 113332274, 113326130, 113326130, 1220616864, 1321212530, 1321286160, 213983858, 1220684880, 1321347648, 1220616528, 1321286016, 1220616626, 1321267826, 616637520, 1421943360, 1220616864, 1421955504, 415316592, 415310017, 515961408, 1220629153, 415310018, 1421955266, 1421955696, 515930786, 1119947282, 1220561425, 1321280064, 1421943408, 1120021009, 1119947280, 1119947280, 1119947328, 1119947280, 1119947280, 1119947376, 1119947280, 1119947280, 1119947424, 1220610576, 113319984, 1220616720, 213983328, 918608736, 1019278176, 717269856, 817939296, 113399856, 214063200, 113246258, 113246258, 213909602, 213909602, 314726544, 314572946, 314572946, 113406385, 214069729, 314715361, 113406289, 214069633, 314715361, 918712657, 1019265026, 113406145, 214069489, 314610913, 1119928704, 1120033296, 1220585808, 1220586048, 1321249776, 1220622913, 1220678209, 1220696640, 1119953426, 1119879698, 1119879698, 12743473, 12743569, 2831328578, 2831162689, 415261632, 1119903936, 1220567568, 2730659090, 2730640657, 2730659088, 2730659088, 2730657984, 2730659088, 2730659088, 2730658320, 2730659088, 2730659088, 2730658368, 2831279424, 516022512, 2931942768, 516071664, 516077809, 2730523920, 2730646800, 2831310144, 2831273282, 2831273282, 2730609938, 2730609938, 2931942768, 1120003392, 113413488, 214075920, 113399856, 214063200, 113246258, 113246258, 213909602, 213909602, 113406001, 113246258, 113246258, 214081586, 2831162737, 415359168, 1120002288, 2931942720, 2931887472, 2931996866, 415389937, 2730671376, 2730646800, 2831273282, 2831273282, 2730609938, 2730609938, 113413441, 113246258, 113246258, 113424433, 113246258, 113246258, 214081586, 1120002576, 1220665920, 1321329264, 1220622912, 1119953425, 1120033296, 1119879698, 1119879698, 2168587266, 2168554497, 2168586752, 557875712, 3242230016, 3779102208, 2168587264, 2168587264, 2168586496, 2168587264, 2168587264, 2168587776, 2168587264, 2168587264, 2168588032, 2168587008, 2168456194, 2168456194, 2168686080, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779393, 1094779392, 3779297792, 3779297792, 3242394112, 558073345, 557875968, 557842690, 557842690, 1363231362, 1363231746, 3242279938, 3242264064, 1900086144, 2168571392, 2168554496, 2168456194, 2168456194, 2168587777, 2168686081, 2168456194, 2168686082, 2168456194, 3242264064, 3779135232, 2168686080, 2168554496, 2168456194, 2168456194, 409477153, 476602560, 543694882, 610828544, 677941474, 745029729, 812138594, 409493888, 543719776, 208175297, 208154720, 745046210, 812167520, 275272064, 73961696, 141075040, 141062209, 141041984, 409485409, 476602560, 543703138, 610828544, 677941474, 745021473, 812130338, 409493888, 543719776, 73957569, 73928736, 745046210, 812167520, 141054336, 208179424, 275292768, 275280001, 275267904, 342421664, 141041730, 74006593, 208162881, 275333185, 409493698, 476586177, 543715552, 610824448, 409551041, 409493696, 409493696, 409493728, 409493696, 409493696, 409493760, 409493696, 409493696, 409493792, 73949216, 73929408, 73924642, 73924642, 1216876576, 1283989568, 1351102560, 1351172737, 1418277441, 1485316129, 1485398720, 1485398690, 1216950850, 1284055681, 76100288, 143217216, 277435072, 210313824, 1216876610, 1284001986, 1216946784, 1283985473, 1351110849, 1284059776, 1351164514, 1418273377, 1216905344, 1216947040, 1216946753, 1284010080, 1284059745, 1485382242, 1216946785, 76112512, 143217216, 277435008, 210313920, 1216876576, 1283989568, 1351102560, 1351172737, 1418277441, 1485316129, 1485398720, 1485398690, 1216950850, 1284055681, 76100288, 143217216, 277435072, 210313824, 1216876610, 1284018498, 1216946784, 1283985473, 1351127361, 1284059776, 1351164514, 1418273377, 1216921728, 1216947040, 1216946753, 1284026464, 1284059745, 1485382242, 1216946785, 76112512, 143217216, 277435008, 210313920, 1216876576, 1283989568, 1351102560, 1351172737, 1418277441, 1485316129, 1485398720, 1485398690, 1216950850, 1284055681, 76100288, 143217216, 277435072, 210313824, 1216876610, 1284035010, 1216946784, 1283985473, 1351143873, 1284059776, 1351164514, 1418273377, 1216938112, 1216947040, 1216946753, 1284042848, 1284059745, 1485382242, 1216946785, 76112512, 143217216, 277435008, 210313920, 344555680, 411664576, 478802817, 545808544, 76043233, 143156193, 210269153, 277382113, 612914146, 680027105, 747140065, 814253026, 1216946754, 1283994177, 1351168608, 1418277504, 1216987713, 1216946752, 1216946752, 1216946784, 1216946752, 1216946752, 1216946816, 1216946752, 1216946752, 1216946848, 76095520, 76026816, 76021794, 76021794, 143204416, 143139776, 143130690, 143130690, 612991264, 680100160, 747238273, 814260512, 881426848, 948535744, 1015673729, 1082712480, 881382370, 1082721250, 1216909634, 1284035010, 1216946784, 1284018497, 1351143873, 1284059776, 1351164514, 1418273377, 1216938368, 1216947040, 1216946753, 1284043104, 1284059745, 1485382242, 1216946785, 344547968, 411652672, 545870464, 478749376, 76025890, 143246305, 210355169, 277364866, 9040609, 9040673, 1284059968, 1284059968, 1216947008, 2223583809, 2223641536, 2223506466, 2223506466, 1284059968, 1284059968, 1216947008, 2290692673, 2290754496, 2290615362, 2290615362, 2290721856, 2357830752, 2223612960, 2424972384, 2424968321, 2424956032, 2424833154, 2424833154, 2760475936, 2492110112, 2559161504, 2492056738, 2559165633, 1217029312, 1216947392, 1284055616, 1284060352, 1216947360, 1284060320, 1351168608, 1418277504, 1485398722, 1485423297, 1485398720, 1485398720, 1485398624, 1485398720, 1485398720, 1485398656, 1485398720, 1485398720, 1485398688, 1485398592, 1418289856, 1418286272, 1217024674, 2626319586, 2626249953, 2760545216, 1216947136, 2626319296, 1216873026, 1216873026, 2626159842, 2626159842, 1284133537, 1351189504, 2693350080, 1284060096, 2693432256, 1283981922, 1283981922, 2693268738, 2693268738, 2693427202, 2626286849, 2693431298, 881456353, 948565281, 2961831137, 3028940065, 2961802624, 2961716608, 3028911520, 3028825504, 76202018, 143315010, 76144672, 143253568, 76021794, 76021794, 143130690, 143130690, 717336912, 818000256, 1120064928, 415304016, 515967360, 515931552, 616748257, 1220567232, 1321236720, 1421906208, 1522612416, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314609904, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220579616, 1321279536, 1421936832, 1220616480, 1220616288, 1119946896, 1119946992, 1421949602, 3233845392, 3234005664, 1522619040, 1421955792, 1119953426, 1321286352, 3032549904, 3032569250, 3032563106, 3032563106, 1220616864, 1321390706, 1321286160, 3133219442, 1120077312, 1220561040, 1321280928, 1421936688, 1220616336, 1220617680, 1119948288, 1119946848, 1421949602, 3535818240, 3536013984, 1522619040, 1421955792, 1119953426, 1321286352, 3334539792, 3334559282, 3334553138, 3334553138, 1220616864, 1321409138, 1321286160, 3435209330, 1120095888, 1220579616, 1321281072, 1421936832, 1220616480, 1220617824, 1119948432, 1119946992, 1421949602, 3837826704, 3838022304, 1522619040, 1421955792, 1119953426, 1321286352, 3636529680, 3636549314, 3636543170, 3636543170, 1220616864, 1321427570, 1321286160, 3737199218, 1120114464, 1220561040, 1321281216, 1421936688, 1220616336, 1220617968, 1119948576, 1119946848, 1421949602, 2931838752, 2931997344, 1522619040, 1421955792, 1119953426, 1321286352, 2730560016, 2730579218, 2730573074, 2730573074, 1220616864, 1321372274, 1321286160, 2831229554, 415440097, 516109537, 616778977, 415421665, 516091105, 616760545, 415458529, 516127969, 616797409, 415403233, 516072673, 616742113, 415421665, 516091105, 616760545, 415403233, 516072673, 616742113, 113468641, 214138081, 314807521, 415421665, 516091105, 616760545, 3032685793, 3133355233, 3234024673, 3334497505, 3435166945, 3535836385, 415403233, 516072673, 616742113, 2730714337, 2831383777, 2932053217, 3636487393, 3737156833, 3837826273, 113413345, 214082785, 314752225, 415458529, 516127969, 616797409, 3636647137, 3737316577, 3837986017, 3032507617, 3133177057, 3233846497, 415403233, 516072673, 616742113, 2730695905, 2831365345, 2932034785, 3334497505, 3435166945, 3535836385, 113431777, 214101217, 314770657, 415403233, 516072673, 616742113, 2730677473, 2831346913, 2932016353, 3032507617, 3133177057, 3233846497, 415440097, 516109537, 616778977, 3334694113, 3435363553, 3536032993, 3636487393, 3737156833, 3837826273, 113450209, 214119649, 314789089, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220740608, 1321279536, 1421938080, 1220617728, 1220616288, 1119946896, 1119948240, 1421949602, 314769552, 314591904, 1522619040, 1421955792, 1119953426, 1321286352, 113314320, 113332274, 113326130, 113326130, 1220616864, 1321212530, 1321286160, 213983858, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220759184, 1321279536, 1421938224, 1220617872, 1220616288, 1119946896, 1119948384, 1421949602, 314787984, 314591904, 1522619040, 1421955792, 1119953426, 1321286352, 113314320, 113332274, 113326130, 113326130, 1220616864, 1321212530, 1321286160, 213983858, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220777760, 1321279536, 1421938368, 1220618016, 1220616288, 1119946896, 1119948528, 1421949602, 314806416, 314591904, 1522619040, 1421955792, 1119953426, 1321286352, 113314320, 113332274, 113326130, 113326130, 1220616864, 1321212530, 1321286160, 213983858, 1220548656, 1321218144, 1421887632, 1522612272, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314591328, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220722032, 1321279536, 1421937936, 1220617584, 1220616288, 1119946896, 1119948096, 1421949602, 314751120, 314591904, 1522619040, 1421955792, 1119953426, 1321286352, 113314320, 113332274, 113326130, 113326130, 1220616864, 1321212530, 1321286160, 213983858, 415261632, 415260816, 1119903936, 1220567568, 2730659090, 2730640657, 2831328192, 515931072, 2931997632, 515930256, 515931456, 2931996816, 515931505, 2730658032, 2730658992, 2831328432, 2831273282, 2831273282, 2730609938, 2730609938, 415261632, 1120002288, 2931942720, 1119946896, 2931997200, 415260816, 2931996866, 415389937, 2730659184, 2730646800, 2831273282, 2831273282, 2730609938, 2730609938, 3242296834, 3779168002, 2168686080, 557875712, 3242328320, 3779200512, 558040576, 1094911744, 2168587266, 2168488961, 557875712, 3779199232, 3242329856, 1094944512, 558073344, 2168587266, 2168521729, 557875712, 557973760, 1094844672, 3242230016, 3779101184, 2168587266, 2168652801, 557875712, 1094844672, 557974016, 3779133952, 3242262784, 2168587266, 2168685569, 2168587264, 2168587776, 2168587264, 2168588032, 2168587264, 2168586496, 2168587264, 2168586752, 2168587008, 2168456194, 2168456194, 2168456194, 717336912, 818000256, 1120064928, 415304016, 515967360, 515931552, 616748257, 1220567232, 1321236720, 1421906208, 1522612416, 1522625233, 1522625233, 1421955744, 1421956080, 1421943457, 1421943457, 1421943457, 1220629152, 1220635202, 113338946, 314609904, 314591377, 1522539218, 1522625184, 1321286257, 1321286256, 1321286257, 213990098, 1119897744, 1220579616, 1321279536, 1421936832, 1220616480, 1220616288, 1119946896, 1119946992, 1421949602, 3233845392, 3234005664, 1522619040, 1421955792, 1119953426, 1321286352, 3032549904, 3032569250, 3032563106, 3032563106, 1220616864, 1321390706, 1321286160, 3133219442, 1120077312, 1220561040, 1321280928, 1421936688, 1220616336, 1220617680, 1119948288, 1119946848, 1421949602, 3535818240, 3536013984, 1522619040, 1421955792, 1119953426, 1321286352, 3334539792, 3334559282, 3334553138, 3334553138, 1220616864, 1321409138, 1321286160, 3435209330, 1120095888, 1220579616, 1321281072, 1421936832, 1220616480, 1220617824, 1119948432, 1119946992, 1421949602, 3837826704, 3838022304, 1522619040, 1421955792, 1119953426, 1321286352, 3636529680, 3636549314, 3636543170, 3636543170, 1220616864, 1321427570, 1321286160, 3737199218, 1120114464, 1220561040, 1321281216, 1421936688, 1220616336, 1220617968, 1119948576, 1119946848, 1421949602, 2931838752, 2931997344, 1522619040, 1421955792, 1119953426, 1321286352, 2730560016, 2730579218, 2730573074, 2730573074, 1220616864, 1321372274, 1321286160, 2831229554, 1119947282, 415439377, 516108817, 616778257, 415420945, 516090385, 616759825, 415457809, 516127249, 616796689, 415402513, 516071953, 616741393, 415420945, 516090385, 616759825, 415402513, 516071953, 616741393, 1119947282, 415420945, 516090385, 616759825, 3032685073, 3133354513, 3234023953, 3334496785, 3435166225, 3535835665, 415402513, 516071953, 616741393, 2730713617, 2831383057, 2932052497, 3636486673, 3737156113, 3837825553, 113412625, 214082065, 314751505, 1119947282, 415457809, 516127249, 616796689, 3636646417, 3737315857, 3837985297, 3032506897, 3133176337, 3233845777, 415402513, 516071953, 616741393, 2730695185, 2831364625, 2932034065, 3334496785, 3435166225, 3535835665, 113431057, 214100497, 314769937, 1119947282, 415402513, 516071953, 616741393, 2730676753, 2831346193, 2932015633, 3032506897, 3133176337, 3233845777, 415439377, 516108817, 616778257, 3334693393, 3435362833, 3536032273, 3636486673, 3737156113, 3837825553, 113449489, 214118929, 314788369, 1119947282, 113431057, 214100497, 314769937, 1119947282, 113449489, 214118929, 314788369, 1119947282, 113467921, 214137361, 314806801, 1119947282, 113412625, 214082065, 314751505, 415261632, 1119898560, 415261200, 1220610576, 415310018, 1119903936, 1220567568, 2730659090, 2730640657, 2831328192, 515931072, 2931997632, 1119898560, 515930640, 1220610576, 515973362, 515931456, 2931997200, 515931505, 2730658032, 2730658992, 2831328432, 2831273282, 2831273282, 2730609938, 2730609938, 1119910848, 415261632, 2931991488, 1220561856, 1321280064, 1119953424, 1119959570, 1120057872, 2931892416, 2931899762, 2931887474, 415389937, 2730659184, 2730646800, 2831273282, 2831273282, 2730609938, 2730609938, 1421893728, 1421906593, 1421918882, 1421943457, 2730516576, 2730530066, 2730542353};
#endif
