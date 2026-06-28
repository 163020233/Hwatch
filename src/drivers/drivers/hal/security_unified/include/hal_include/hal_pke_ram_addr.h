/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: hal pke ram addr header. \n
 *
 * History: \n
 * 2025-03-13, Create file. \n
 */
#ifndef HAL_PKE_RAM_ADDR_H
#define HAL_PKE_RAM_ADDR_H

#include "crypto_pke_struct.h"

/************************************************** ecc instr start************************************/

extern const rom_lib instr_ecfp_j2a_pre_5;

extern const rom_lib instr_ecfp_j2a_exp_00;

extern const rom_lib instr_ecfp_j2a_exp_01;

extern const rom_lib instr_ecfp_j2a_exp_10;

extern const rom_lib instr_ecfp_j2a_exp_11;

extern const rom_lib instr_ecfp_j2a_exp_one_bit_00;

extern const rom_lib instr_ecfp_j2a_exp_one_bit_01;

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

extern const rom_lib instr_ecfn_inv_exp_one_bit_00;

extern const rom_lib instr_ecfn_inv_exp_one_bit_01;

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

extern const rom_lib instr_curve25519_xz2x_exp_one_bit_00;

extern const rom_lib instr_curve25519_xz2x_exp_one_bit_01;

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

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_one_bit_00;

extern const rom_lib instr_ed_ecfp_xyz2xy_exp_one_bit_01;

extern const rom_lib instr_ed_ecfp_xyz2xy_post;

extern const rom_lib instr_ed25519_mul_sub_pre;

extern const rom_lib instr_ed25519_calc_h;

extern const rom_lib instr_ed25519_calc_s;

extern const rom_lib instr_ed25519_recover_x_pre;

extern const rom_lib instr_ed25519_recover_x_exp_00;

extern const rom_lib instr_ed25519_recover_x_exp_01;

extern const rom_lib instr_ed25519_recover_x_exp_10;

extern const rom_lib instr_ed25519_recover_x_exp_11;

extern const rom_lib instr_ed25519_recover_x_exp_one_bit_00;

extern const rom_lib instr_ed25519_recover_x_exp_one_bit_01;

extern const rom_lib instr_ed25519_recover_x_post;

extern const rom_lib instr_ed25519_cpy_tx0_to_tx1;

extern const rom_lib instr_ed25519_cpy_tx1_to_tx0;

extern const rom_lib instr_ed25519_cpy_t2g;

extern const rom_lib instr_ed25519_cpy_t2r;

extern const rom_lib instr_ed25519_c_eq_r;
/************************************************** ed instr end ************************************/

/************************************************** rsa instr start ************************************/
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
/************************************************** rsa instr end ************************************/

/************************************************ blind instr old start **********************************/
extern const rom_lib instr_ecfp_pm_blind_0;

extern const rom_lib instr_ecfp_pm_blind_1;

extern const rom_lib instr_ecfp_pm_blind_2;

extern const rom_lib instr_ecfp_pm_blind_3;
/************************************************ blind instr old end **********************************/

/************************************************ blind instr new start **********************************/
extern const rom_lib instr_ecfp_blind_gen_point_r;

extern const rom_lib instr_ecfp_blind_gen_point_t_new;

extern const rom_lib instr_ecfp_blind_mix_point_t_0_new;

extern const rom_lib instr_ecfp_blind_mix_point_t_1_new;

extern const rom_lib instr_ecfp_blind_mix_point_t_2_new;

extern const rom_lib instr_ecfp_blind_mix_point_t_3_new;

extern const rom_lib instr_ecfp_blind_init_point_q_0;

extern const rom_lib instr_ecfp_blind_init_point_q_1;

extern const rom_lib instr_ecfp_blind_init_point_q_2;

extern const rom_lib instr_ecfp_blind_init_point_q_3;

extern const rom_lib instr_ecfn_inv_sec_pre_new;

extern const rom_lib instr_ecfn_sign_s_rand_new;

extern const rom_lib instr_sm2_sign_s_pre_rand_new;

extern const rom_lib instr_sm2_sign_s_post_rand_new;

extern const rom_lib instr_ecfp_blind_key_t;

extern const rom_lib instr_ecfp_blind_key_s;

/************************************************ blind instr start **********************************/
#ifndef CONFIG_PKE_RAM_ADDR_DIV
#define CONFIG_PKE_RAM_ADDR_DIV 1
#endif
/* store modulus */
#define curve_addr_m 0
/* store results */
#define curve_addr_x2 2
#define curve_addr_z2 4
/* store point coordinate temporarily */
#define curve_addr_x3 6
#define curve_addr_z3 8
#define curve_addr_x1 10

/* store temporary variable */
#define curve_addr_t0 12
#define curve_addr_t1 14
#define curve_addr_t2 16
#define curve_addr_t3 18
#define curve_addr_t4 20
#define curve_addr_t5 22
#define curve_addr_t6 24
#define curve_addr_tp 26

/* store the const value */
#define curve_addr_p 28
#define curve_addr_rrp 30
#define curve_addr_n 32
#define curve_addr_rrn 34
#define curve_addr_mont_a 36
#define curve_addr_mont_a24 38
#define curve_addr_mont_1_p 40
#define curve_addr_mont_1_n 42
#define curve_addr_const_1 44
#define curve_addr_const_0 46


/* store modulus */
#define ecc_addr_m  (0 / CONFIG_PKE_RAM_ADDR_DIV)
/* store the result of point multiplication operation */
#define ecc_addr_cx (3 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_cy (6 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_cz (9 / CONFIG_PKE_RAM_ADDR_DIV)
/* Temporary point coordinate storage space */
#define ecc_addr_ax (12 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_ay (15 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_az (18 / CONFIG_PKE_RAM_ADDR_DIV)
/* Storage space for input point coordinates */
#define ecc_addr_px (21 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_py (24 / CONFIG_PKE_RAM_ADDR_DIV)
/* Storage space for input point coordinates */
#define ecc_addr_gx (27 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_gy (30 / CONFIG_PKE_RAM_ADDR_DIV)
/* Temporary variable storage space for operations */
#define ecc_addr_t0 (33 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t1 (36 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t2 (39 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t3 (42 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t4 (45 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_tp (48 / CONFIG_PKE_RAM_ADDR_DIV)

/* Storage space for constant values */
#define ecc_addr_p      (51 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_rrp    (54 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_n      (57 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_rrn    (60 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_mont_a (63 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_mont_b (66 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_mont_1_p (69 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_mont_1_n (72 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_const_1 (75 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_const_0 (78 / CONFIG_PKE_RAM_ADDR_DIV)

/* Storage space for digital signatures */
#define ecc_addr_s (81 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_r (84 / CONFIG_PKE_RAM_ADDR_DIV)
/* Storage space of the message hash value */
#define ecc_addr_e (87 / CONFIG_PKE_RAM_ADDR_DIV)

#define ecc_addr_pz (90 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_gz (93 / CONFIG_PKE_RAM_ADDR_DIV)

/* Storage space of intermediate variables during signature and verification */
#define ecc_addr_k (12 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_k = ecc_addr_ax */
#define ecc_addr_d (15 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_d = ecc_addr_ay */

#define ecc_addr_u1 (3 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_u1 = ecc_addr_cx */
#define ecc_addr_u2 (6 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_u2 = ecc_addr_cy */

#define ecc_addr_v (3 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_v = ecc_addr_cx */
#define ecc_addr_rx (3 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_rx = ecc_addr_R = ecc_addr_cx */
#define ecc_addr_t (3 / CONFIG_PKE_RAM_ADDR_DIV) /* ecc_addr_t = ecc_addr_cx */

#define ecc_addr_t00_x (90 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t00_y (93 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t00_z (96 / CONFIG_PKE_RAM_ADDR_DIV)

#define ecc_addr_t01_x (99  / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t01_y (102 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t01_z (105 / CONFIG_PKE_RAM_ADDR_DIV)

#define ecc_addr_t10_x (108 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t10_y (111 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t10_z (114 / CONFIG_PKE_RAM_ADDR_DIV)

#define ecc_addr_t11_x (81 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t11_y (84 / CONFIG_PKE_RAM_ADDR_DIV)
#define ecc_addr_t11_z (87 / CONFIG_PKE_RAM_ADDR_DIV)


/* store modulus */
#define ed_addr_m 0
/* store the result for point mulplication. */
#define ed_addr_cx 2
#define ed_addr_cy 4
#define ed_addr_cz 6
#define ed_addr_ct 8
/* store point coordinate for temporary */
#define ed_addr_ax 10
#define ed_addr_ay 12
#define ed_addr_az 14
#define ed_addr_at 16
/* store the input point */
#define ed_addr_px 18
#define ed_addr_py 20
#define ed_addr_pz 22
#define ed_addr_pt 24
/* store the input point */
#define ed_addr_gx 26
#define ed_addr_gy 28
#define ed_addr_gz 30
#define ed_addr_gt 32

/* store the tmeporary variable */
#define ed_addr_tp 34
#define ed_addr_t0 36
#define ed_addr_t1 38
#define ed_addr_t2 40
#define ed_addr_t3 42
#define ed_addr_t4 44

/* store the const value */
#define ed_addr_p 46
#define ed_addr_rrp 48
#define ed_addr_n 50
#define ed_addr_rrn 52
#define ed_addr_mont_d 54
#define ed_addr_mont_1_p 56
#define ed_addr_mont_1_n 58
#define ed_addr_const_1 60
#define ed_addr_const_0 62
#define ed_addr_sqrt_m1 64

/* store intermediate variable for sign */
#define ed_addr_r 66
#define ed_addr_h 68
/* store private key for sign */
#define ed_addr_sk 70
/* store result for signature */
#define ed_addr_s 72

/* store temporary variable for verify */
#define ed_addr_u 74
#define ed_addr_v 76

#define ed_addr_tx0 78
#define ed_addr_tx1 80
#define ed_addr_ty 82
#define ed_addr_bx 84
#define ed_addr_by 86
#define ed_addr_rx 88
#define ed_addr_ry 90

/* modulus storage space */
#define rsa_addr_n  0
/* storage space for base number ofr the modular exponentiation */
#define rsa_addr_a  16
/* storage sapce for montgomery parameter */
#define rsa_addr_rr  32
/* storage sapce for constant value 1 */
#define rsa_addr_const_1  48
/* storage sapce of result */
#define rsa_addr_s  64
/* storage sapce for intermediate variable of CRT mode */
#define rsa_addr_m2  40
#define rsa_addr_q_inv  56
/* storage sapce for common intermediate variable */
#define rsa_addr_tp  80
/* storage sapce for input data */
#define rsa_addr_t0  96
#define rsa_addr_t1  112

extern const uint32_t g_instr_rom[];

#if CONFIG_PKE_RAM_ADDR_DIV == 3
#define ecc_addr_key0 30
#define ecc_addr_key1 31
#else
#define ecc_addr_key0 122
#define ecc_addr_key1 125
#endif

#endif
