/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc compute s implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"
#include "drv_pke_priv.h"

static int ecc_ecfn_sign_s_sec(const drv_pke_data *k_inv, const drv_pke_data *e, const drv_pke_data *d,
    const drv_pke_data *r, const drv_pke_data *n, const drv_pke_data *s, const drv_pke_data *rand)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned int work_len;
    unsigned char d_add[MAX_ECC_SIZE];
    drv_pke_data d_add_data = {
        .data = d_add,
        .length = rand->length
    };
    crypto_drv_func_enter();

    work_len = n->length / ALIGNED_TO_WORK_LEN_IN_BYTE;

    ret = crypto_pke_common_add_mod(d, rand, &d_add_data, n);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret);

    /* Step 1: set data into PKE DRAM. the ecc_addr_rrn and ecc_addr_const_1 have been set in intial parameters. */
    hal_pke_set_ram_romable(ecc_addr_r, r->data, r->length);
    hal_pke_set_ram_romable(ecc_addr_e, e->data, e->length);
    hal_pke_set_ram_romable(ecc_addr_d, d_add_data.data, d_add_data.length);
    random_delay();
    hal_pke_set_ram_romable(ecc_addr_s, k_inv->data, k_inv->length);
    random_delay();
    hal_pke_set_ram_romable(ecc_addr_n, n->data, n->length);

    /* Step 2. start calculate. */
    ret = inner_batch_instr_process_sec(&instr_ecfn_sign_s_rand_new, work_len);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret);

    /* Step 3: get data from PKE DRAM */
    hal_pke_get_ram_romable(ecc_addr_s, s->data, s->length);

    crypto_drv_func_exit();
    return CRYPTO_ENHANCE_SUCCESS;
}

/* s = k^-1(e + dr) mod n */
int hal_pke_alg_ecc_compute_s_sec(const drv_pke_data *k, const drv_pke_data *e, const drv_pke_data *d,
    const drv_pke_data *r, const drv_pke_data *s)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned char *buf = NULL;
    unsigned int buf_len = MAX_ECC_SIZE * 2;    // 2: for k_inv/rand
    drv_pke_data k_inv_data = {
        .length = k->length
    };
    drv_pke_data rand_data = {
        .length = k->length
    };
    drv_pke_data n = {0};
    const drv_pke_ecc_curve *ecc_curve = NULL;
    hal_pke_alg_ecc_init_check();

    buf = crypto_malloc_coherent(buf_len);
    crypto_chk_return(buf == NULL, ERROR_MALLOC, "malloc failed\n");

    k_inv_data.data = buf;
    rand_data.data = k_inv_data.data + MAX_ECC_SIZE;

    ecc_curve = hal_pke_alg_ecc_get_curve();
    n.data = (unsigned char *)ecc_curve->n;
    n.length = ecc_curve->ksize;

    ret = inner_get_random_sec(rand_data.data, rand_data.length);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, exit_free, "inner_get_random failed\n");
    if (inner_drv_is_zero(rand_data.data, rand_data.length)) {
        ret = CRYPTO_ENHANCE_FAILURE;
        goto exit_free;
    }

    /* Step 1. Compute k_inv. */
    ret = hal_pke_alg_ecc_inv_mod_sec(k, &k_inv_data, &rand_data);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, exit_free, "hal_pke_alg_ecc_inv_mod_sec failed\n");

    /* Step 2. Compute s = k_inv * (e + d * r) mod n. */
    ret = ecc_ecfn_sign_s_sec(&k_inv_data, e, d, r, &n, s, &rand_data);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, exit_free, "ecc_ecfn_sign_s_sec failed\n");

exit_free:
    (void)memset_s(buf, buf_len, 0, buf_len);
    crypto_free_coherent(buf);
    return ret;
}