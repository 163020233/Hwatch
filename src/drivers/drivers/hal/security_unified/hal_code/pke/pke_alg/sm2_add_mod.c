/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke sm2 add mod implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_SM2_ADD_MOD

int hal_pke_alg_sm2_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c)
{
    int ret = CRYPTO_FAILURE;
    unsigned int klen;
    unsigned int work_len;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    hal_pke_alg_ecc_init_check();

    ecc_curve = hal_pke_alg_ecc_get_curve();

    klen = ecc_curve->ksize;
    work_len = klen / ALIGNED_TO_WORK_LEN_IN_BYTE;

    /* Step 1: set data into DRAM. */
    hal_pke_set_ram_romable(ecc_addr_r, a->data, a->length);
    hal_pke_set_ram_romable(ecc_addr_s, b->data, b->length);
    hal_pke_set_ram_romable(ecc_addr_m, ecc_curve->n, klen);

    /* Step 2: start calculate. */
    ret = inner_batch_instr_process(&instr_sm2_verify_t_3, work_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* Step 3: get data out of DRAM. */
    hal_pke_get_ram_romable(ecc_addr_t, c->data, c->length);

    return CRYPTO_SUCCESS;
}