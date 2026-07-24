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
#include "hal_pke_func.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_ECC_INV_MOD_SEC

#define MAX_ECC_LENGTH        72

int hal_pke_alg_ecc_inv_mod_sec(const drv_pke_data *a, const drv_pke_data *c, const drv_pke_data *rand_data)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    const unsigned char *n_2 = NULL;
    unsigned int work_len = 0;
    unsigned int klen;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    const pke_ecc_init_param *init_param = NULL;
    unsigned char rand_mod[MAX_ECC_LENGTH];
    drv_pke_data rand_mod_data;
    drv_pke_data mod_n;
    const rom_lib *batch_instr_loop[] = {&instr_ecfn_inv_exp_one_bit_00, &instr_ecfn_inv_exp_one_bit_01, NULL, NULL};
    hal_pke_alg_ecc_init_check();
    ecc_curve = hal_pke_alg_ecc_get_curve();
    init_param = hal_pke_alg_ecc_get_init_param();

    klen = ecc_curve->ksize;
    n_2 = init_param->n_minus_2;
    work_len = klen;

    hal_pke_set_mont_para_romable(init_param->mont_param_n);

    /* Step 1. former 5 process. data montgomery and copy */
    /* 1.set data into DRAM, ecc_addr_rrn and ecc_addr_mont_1_n will be set in initial parameters. */
    rand_mod_data.data = rand_mod;
    rand_mod_data.length = klen;
    mod_n.data = (unsigned char *)ecc_curve->n;
    mod_n.length = klen;

    ret = crypto_pke_common_add_mod(a, rand_data, &rand_mod_data, &mod_n); /* rand_mod = k + rand mod n */
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_pke_common_add_mod failed\n");

    hal_pke_set_ram_romable(ecc_addr_m, ecc_curve->n, klen);
    hal_pke_set_ram_romable(ecc_addr_cz, rand_data->data, rand_data->length);
    hal_pke_set_ram_romable(ecc_addr_k, rand_mod_data.data, rand_mod_data.length);

    /* 2. start calculate. */
    ret = inner_batch_instr_process_sec(&instr_ecfn_inv_sec_pre_new, work_len);
    drv_err_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* Step 2. start inv calculate. */
    ret = hal_pke_loop_batch_compute_sec(ecc_curve->key_valid_bit, n_2, NULL, klen, batch_instr_loop);
    drv_err_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    /* get data out from DRAM ecc_addr_s, the result is montgomeried. */
    hal_pke_get_ram_romable(ecc_addr_s, c->data, c->length);
    return ret;
}