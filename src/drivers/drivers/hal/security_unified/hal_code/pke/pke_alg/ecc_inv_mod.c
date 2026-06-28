/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc inv mod implement.
 *
 * Create: 2025-01-07
*/

#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

int hal_pke_alg_ecc_inv_mod(const drv_pke_data *a, const drv_pke_data *c)
{
    int ret;
    const unsigned char *n_2 = NULL;
    const rom_lib *batch_instr_loop[] = {&instr_ecfn_inv_exp_one_bit_00, &instr_ecfn_inv_exp_one_bit_01, NULL, NULL};
    unsigned int klen;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    const pke_ecc_init_param *init_param = NULL;
    hal_pke_alg_ecc_init_check();
    ecc_curve = hal_pke_alg_ecc_get_curve();
    init_param = hal_pke_alg_ecc_get_init_param();

    klen = ecc_curve->ksize;
    n_2 = init_param->n_minus_2;

    hal_pke_set_mont_para_romable(init_param->mont_param_n);

    /* Step 1. former 5 process. data montgomery and copy */
    /* 1.set data into DRAM, ecc_addr_rrn and ecc_addr_mont_1_n will be set in initial parameters. */
    hal_pke_set_ram_romable(ecc_addr_k, a->data, a->length);
    hal_pke_set_ram_romable(ecc_addr_m, ecc_curve->n, klen);

    /* 2. start calculate. */
    ret = inner_batch_instr_process_opt(&instr_ecfn_inv_pre_5);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");

    /* Step 2. start inv calculate. */
    ret = hal_pke_loop_batch_compute_romable(ecc_curve->key_valid_bit, n_2, NULL, klen, batch_instr_loop);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    /* get data out from DRAM ecc_addr_s, the result is montgomeried. */
    hal_pke_get_ram_romable(ecc_addr_s, c->data, c->length);
    return ret;
}