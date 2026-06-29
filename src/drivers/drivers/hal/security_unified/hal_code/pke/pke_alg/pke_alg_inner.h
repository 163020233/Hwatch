/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pke_alg inner header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-28, Create file. \n
 */

#ifndef PKE_ALG_INNER_H
#define PKE_ALG_INNER_H

#include "crypto_pke_struct.h"
#include "hal_pke_ram_addr.h"

#define PKE_COMPAT_ERRNO(err_code)      DRV_COMPAT_ERRNO(ERROR_MODULE_PKE, err_code)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*
 *
 * Used in:
 *      ecc_common.c
 *      rsa_exp_mod.c
 */
int inner_pke_alg_resume(void);
void inner_pke_alg_suspend(void);

/*
 *
 * Used in:
 *      ecc_common_sec.c
 */
int inner_pke_alg_resume_sec(void);
void inner_pke_alg_suspend_sec(void);

/*
 *
 * Used in:
 *      ecc_common.c
 *      ecc_common_sec.c
 */
void inner_set_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve);

/*
 *
 * Used in:
 *      ecc_compute_x.c
 *      ecc_point_mul.c
 */
int inner_ecfp_montgomery_data_aff(const drv_pke_ecc_point *in, const drv_pke_data *mod_p,
    const drv_pke_ecc_point *out);

/*
 *
 * Used in:
 *      ecc_point_mul.c
 */
int inner_ecfp_montgomery_pre(const drv_pke_ecc_point *in, const drv_pke_data *mod_p);

/*
 *
 * Used in:
 *      ecc_compute_x.c
 *      ecc_point_mul.c
 */
int inner_ecfp_demontgomery_data_aff(const drv_pke_ecc_point *r);

/*
 *
 * Used in:
 *      ecc_compute_x.c
 *      ecc_point_mul.c
 */
int inner_ecfp_jac_to_aff(const drv_pke_data *mod_p);

/* * struct of ecc point */
typedef struct {
    unsigned char *x;   /* X coordinates of the point in Jacobian coordinate system. */
    unsigned char *y;   /* Y coordinates of the point in Jacobian coordinate system. */
    unsigned char *z;   /* Z coordinates of the point in Jacobian coordinate system. */
    unsigned int length;
} pke_ecc_point_jac;
int inner_ecfp_aff_to_jac(const drv_pke_ecc_point *in, const drv_pke_data *mod_p, pke_ecc_point_jac *out);

/*
 *
 * Used in:
 *      ecc_check_dot_on_curve.c
 *      ecc_point_mul.c
 */
int inner_ecfp_mul_naf_cal(unsigned int work_len, const drv_pke_data *k);

/*
 *
 * Used in:
 *      ecc_compute_s.c
 *      ecc_compute_u1_and_u2.c
 *      ecc_compute_x.c
 *      ecc_inv_mod.c
 *      sm2_add_mod.c
 *      sm2_compute_s.c
 *      rsa_exp_mod.c
 */
int inner_batch_instr_process(const rom_lib *batch_instr, unsigned int work_len);
int inner_batch_instr_process_opt(const rom_lib *batch_instr);

/*
 *
 * Used in:
 *      ecc_compute_s_sec.c
 *      ecc_inv_mod_sec.c
 *      ecc_point_mul_sec.c
 *      pke_alg_inner_sec.c
 */
int inner_batch_instr_process_sec(const rom_lib *batch_instr, unsigned int work_len);

/*
 *
 * Used in:
 *      ecc_point_mul.c
 *      sm2_compute_s.c
 */
int inner_update_modulus(const unsigned char *n, const unsigned int n_len, const unsigned int *mont_param);

/*
 *
 * Used in:
 *      rsa_exp_mod.c
 */
int inner_update_rsa_modulus(const unsigned char *n, const unsigned int n_len);

/*
 * Used in:
 *      ecc_mul_naf_inner.c
 *      ed_point_mul_compress.c
 *
 */
int inner_point_mul_naf(const rom_lib *batch_instr, const unsigned int batch_instr_num,
    const drv_pke_data *k);

/*
 * Used in:
 *      ecc_inner.c
 *      ed_point_mul_compress.c
 *
 */
int inner_jac_to_aff_cal(const rom_lib *batch_instr, const unsigned int batch_instr_num,
    const drv_pke_data *mod_p);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif