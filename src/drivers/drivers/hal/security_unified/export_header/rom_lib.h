/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: romlib instruction functions.
 *
 * Create: 2022-08-15
*/

#ifndef ROM_LIB_H
#define ROM_LIB_H

#include "crypto_pke_struct.h"
/************************************************** ecc instr start************************************/

extern const rom_lib instr_ecfp_j2a_pre_5;

extern const rom_lib instr_ecfp_j2a_exp_00;

extern const rom_lib instr_ecfp_j2a_exp_01;

extern const rom_lib instr_ecfp_j2a_exp_10;

extern const rom_lib instr_ecfp_j2a_exp_11;

extern const rom_lib instr_ecfp_j2a_post_4;

extern const rom_lib instr_ecfp_demont_c_6;

extern const rom_lib instr_ecfp_demont_cz_3;

extern const rom_lib instr_ecfp_mont_p_2;

extern const rom_lib instr_ecfp_cpy_p2c_3;

extern const rom_lib instr_ecfp_cpy_np2g_2;

extern const rom_lib instr_ecfp_mul_p_22_18;

extern const rom_lib instr_ecfp_mul_g_22_18;

extern const rom_lib instr_ecfp_mul_c_double_22;

extern const rom_lib instr_ecfp_prime_n_1;

extern const rom_lib instr_ecfn_inv_pre_5;

extern const rom_lib instr_ecfn_inv_exp_00;

extern const rom_lib instr_ecfn_inv_exp_01;

extern const rom_lib instr_ecfn_inv_exp_10;

extern const rom_lib instr_ecfn_inv_exp_11;

extern const rom_lib instr_ecfn_sign_s_12;

extern const rom_lib instr_ecfn_verify_u_10;

extern const rom_lib instr_ecfp_add_ja_verify_18;

extern const rom_lib instr_ecfp_cpy_a2c_3;

extern const rom_lib instr_ecfp_cpy_g2c_3;

extern const rom_lib instr_ecfp_mul_jj_22_23;

extern const rom_lib instr_ecfn_verify_v_4;

/************************************************** ecc instr end************************************/

/************************************************** sm2 instr start************************************/
extern const rom_lib instr_sm2_sign_r_1;

extern const rom_lib instr_sm2_sign_s_pre_6;

extern const rom_lib instr_sm2_sign_s_post_6;

extern const rom_lib instr_sm2_verify_t_3;
/************************************************** sm2 instr end************************************/

/************************************************** curve instr start************************************/
extern const rom_lib instr_curve25519_pre_ladder;

extern const rom_lib instr_curve25519_ladder_0;

extern const rom_lib instr_curve25519_ladder_1;

extern const rom_lib instr_curve25519_xz2x_pre;

extern const rom_lib instr_curve25519_xz2x_exp_00;

extern const rom_lib instr_curve25519_xz2x_exp_01;

extern const rom_lib instr_curve25519_xz2x_exp_10;

extern const rom_lib instr_curve25519_xz2x_exp_11;

extern const rom_lib instr_curve25519_xz2x_post;
/************************************************** curve instr end************************************/

/************************************************** ed instr start************************************/
extern const rom_lib instr_ed25519_mul_loop_0;

extern const rom_lib instr_ed25519_mul_a_loop_1;

extern const rom_lib instr_ed25519_mul_p_loop_1;

extern const rom_lib instr_ed25519_mul_g_loop_1;

extern const rom_lib instr_ed25519_mul_a_pre;

extern const rom_lib instr_ed_ecfp_xyz2xy_pre;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_00;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_01;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_10;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_11;

extern const rom_lib instr_ed_ecfp_xyz2xy_post;

extern const rom_lib instr_ed25519_mul_sub_pre;

extern const rom_lib instr_ed25519_calc_h;

extern const rom_lib instr_ed25519_calc_s;

extern const rom_lib instr_ed25519_recover_x_pre;

extern const rom_lib instr_ed25519_recover_x_exp_00;

extern const rom_lib instr_ed25519_recover_x_exp_01;

extern const rom_lib instr_ed25519_recover_x_exp_10;

extern const rom_lib instr_ed25519_recover_x_exp_11;

extern const rom_lib instr_ed25519_recover_x_post;

extern const rom_lib instr_ed25519_cpy_tx0_to_tx1;

extern const rom_lib instr_ed25519_cpy_tx1_to_tx0;

extern const rom_lib instr_ed25519_cpy_t2g;

extern const rom_lib instr_ed25519_cpy_t2r;

extern const rom_lib instr_ed25519_c_eq_r;
/************************************************** ed instr end************************************/

/************************************************** rsa instr start************************************/
extern const rom_lib instr_rsa_exp_pre_6;

extern const rom_lib instr_rsa_exp_00;

extern const rom_lib instr_rsa_exp_01;

extern const rom_lib instr_rsa_exp_10;

extern const rom_lib instr_rsa_exp_11;

extern const rom_lib instr_rsa_exp_post_3;

extern const rom_lib instr_rsa_regular_mul;

extern const rom_lib instr_rsa_rrn_add;

extern const rom_lib instr_rsa_rrn_add_3072;

extern const rom_lib instr_rsa_rrn_add_320;

extern const rom_lib instr_rsa_rrn_mul;

extern const rom_lib instr_rsa_mod;

extern const rom_lib instr_rsa_mod_add;

extern const rom_lib instr_rsa_mod_sub;

extern const rom_lib instr_rsa_mod_mul;
/************************************************** rsa instr end************************************/

extern const td_u32 g_instr_rom[];

#endif