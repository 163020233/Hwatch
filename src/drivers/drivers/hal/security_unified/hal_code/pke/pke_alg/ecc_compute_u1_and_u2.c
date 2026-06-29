/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc compute u1 and u2 implement.
 *
 * Create: 2025-01-07
*/

#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

int hal_pke_alg_ecc_compute_u1_and_u2(const drv_pke_data *w, const drv_pke_data *e, const drv_pke_data *r,
    const drv_pke_data *u1, const drv_pke_data *u2)
{
    int ret;
    unsigned int klen;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    hal_pke_alg_ecc_init_check();

    klen = hal_pke_alg_ecc_get_klen();
    ecc_curve = hal_pke_alg_ecc_get_curve();

    /* Step 1: set data into PKE DRAM. the ecc_addr_rrn and ecc_addr_const_1 is set in initial parameters. */
    hal_pke_set_ram_romable(ecc_addr_e, e->data, e->length);
    hal_pke_set_ram_romable(ecc_addr_r, r->data, r->length);
    hal_pke_set_ram_romable(ecc_addr_s, w->data, w->length);
    hal_pke_set_ram_romable(ecc_addr_m, ecc_curve->n, klen);

    /* Step 2: start calculate. */
    ret = inner_batch_instr_process_opt(&instr_ecfn_verify_u_10);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process_opt failed\n");

    /* Step 3: get result out of PKE DRAM */
    hal_pke_get_ram_romable(ecc_addr_u1, u1->data, u1->length);
    hal_pke_get_ram_romable(ecc_addr_u2, u2->data, u2->length);

    return ret;
}