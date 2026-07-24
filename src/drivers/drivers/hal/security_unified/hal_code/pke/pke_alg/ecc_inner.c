/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc inner implement.
 *
 * Create: 2025-01-07
*/
#include "pke_alg_inner.h"

#include "hal_pke_romable.h"
#include "hal_pke_alg.h"
#include "crypto_drv_common.h"
#include "crypto_pke_common.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_INNER

#define MAX_ECC_LENGTH        72

#define JAC_TO_AFF_INSTR_NUM    6
int inner_jac_to_aff_cal(const rom_lib *batch_instr, const unsigned int batch_instr_num,
    const drv_pke_data *mod_p)
{
    int ret;
    const unsigned char *mod_p_2 = NULL;
    unsigned int loop_num = 0;
    const rom_lib *batch_instr_loop[] = {&batch_instr[1], &batch_instr[2], NULL, NULL};
    crypto_drv_func_enter();

    crypto_unused(batch_instr_num);

    mod_p_2 = hal_pke_alg_ecc_get_p_minus_2();

    /* 1. set necessary data into DRAM, which should have been set by former process. */

    /* 2. start transform */
    /* 2.1 preprocess, initialize the data, use batch process step. */
    ret = inner_batch_instr_process_opt(&batch_instr[0]);
    drv_err_chk_func_return(inner_batch_instr_process_opt, ret);

    /* 2.2 expand (p-2)'s bit length to 2n, and calculate from high bit */
    loop_num = mod_p->length * CRYPTO_BITS_IN_BYTE;

    ret = hal_pke_loop_batch_compute_romable(loop_num, mod_p_2, NULL, mod_p->length, batch_instr_loop);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    /* 2.3 postprocess, demontgomery and complete reduction */
    ret = inner_batch_instr_process_opt(&batch_instr[3]); /* 3: instr index. */
    drv_err_chk_func_return(inner_batch_instr_process_opt, ret);

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}

/* use when the process only have ecc related data address. */
int inner_ecfp_montgomery_data_aff(const drv_pke_ecc_point *in, const drv_pke_data *mod_p,
    const drv_pke_ecc_point *out)
{
    int ret;
    crypto_drv_func_enter();

    /* P = P * rrp mod p to Montgomery data, point mul_mud */
    /* 1. set data to DRAM */
    hal_pke_set_ram_romable(ecc_addr_px, in->x, in->length);
    hal_pke_set_ram_romable(ecc_addr_py, in->y, in->length);
    hal_pke_set_ram_romable(ecc_addr_m, mod_p->data, mod_p->length);

    /* 2. start calculate */
    ret = inner_batch_instr_process_opt(&instr_ecfp_mont_p_2);
    drv_err_chk_func_return(inner_batch_instr_process_opt, ret);

    /* 3. get result from DRAM */
    if (out != NULL && out->x != NULL && out->y != NULL) {
        hal_pke_get_ram_romable(ecc_addr_px, out->x, out->length);
        hal_pke_get_ram_romable(ecc_addr_py, out->y, out->length);
    }

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}

int inner_ecfp_demontgomery_data_aff(const drv_pke_ecc_point *r)
{
    int ret;
    crypto_drv_func_enter();

    /* Step 1: start calculate. */
    ret = inner_batch_instr_process_opt(&instr_ecfp_demont_c_6);
    drv_err_chk_func_return(inner_batch_instr_process_opt, ret);

    /* Step 2: get data from PKE DRAM */
    if (r != NULL && r->x != NULL && r->y != NULL) {
        hal_pke_get_ram_romable(ecc_addr_cx, r->x, r->length);
        hal_pke_get_ram_romable(ecc_addr_cy, r->y, r->length);
    }

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}

int inner_update_modulus(const unsigned char *n, const unsigned int n_len, const unsigned int *mont_param)
{
    int ret;

    /* 1. set new modulus and const_0(by initial parameters set) into PKE DRAM. */
    hal_pke_set_ram_romable(ecc_addr_n, n, n_len);

    /* 2. start calculate. */
    ret = inner_batch_instr_process_opt(&instr_ecfp_prime_n_1);
    drv_err_chk_func_return(inner_batch_instr_process_opt, ret);

    /* 5. get new montgomery parameters and set it into register. */
    hal_pke_set_mont_para_romable(mont_param);

    return CRYPTO_SUCCESS;
}

#define JAC_TO_AFF_INSTR_NUM    6
int inner_ecfp_jac_to_aff(const drv_pke_data *mod_p)
{
    int ret;

    /* Step 1: prepare instructions to use */
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {instr_ecfp_j2a_pre_5, instr_ecfp_j2a_exp_one_bit_00,
        instr_ecfp_j2a_exp_one_bit_01, instr_ecfp_j2a_post_4};

    /* Step 2: start transfer calculation. */
    ret = inner_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, mod_p);
    drv_err_chk_func_return(inner_jac_to_aff_cal, ret);

    return ret;
}

/* (ecc_addr_px, ecc_addr_py) -> (ecc_addr_cx, ecc_addr_cy, ecc_addr_cz) */
int inner_ecfp_aff_to_jac(const drv_pke_ecc_point *in, const drv_pke_data *mod_p, pke_ecc_point_jac *out)
{
    int ret;
    crypto_drv_func_enter();

    /* Step 1: set input data into DRAM. */
    if (in != NULL && in->x != NULL && in->y != NULL) {
        hal_pke_set_ram_romable(ecc_addr_px, in->x, in->length);
        hal_pke_set_ram_romable(ecc_addr_py, in->y, in->length);
    }
    /* need to reset modulur. */
    hal_pke_set_ram_romable(ecc_addr_m, mod_p->data, mod_p->length);

    /* Step 2: start calculate. */
    ret = inner_batch_instr_process_opt(&instr_ecfp_cpy_p2c_3);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");

    /* Step 3: get result from DRAM. */
    /* no need to get result out, only a common process. */
    if (out != NULL && out->x != NULL && out->y != NULL && out->z != NULL) {
        hal_pke_get_ram_romable(ecc_addr_cx, out->x, out->length);
        hal_pke_get_ram_romable(ecc_addr_cy, out->y, out->length);
        hal_pke_get_ram_romable(ecc_addr_cz, out->z, out->length);
    }

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}