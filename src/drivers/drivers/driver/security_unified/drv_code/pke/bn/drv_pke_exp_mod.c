/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke cal implement.
 *
 * Create: 2025-01-07
*/

#include "drv_pke_cal.h"
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_BN_DRV_PKE_EXP_MOD

int drv_cipher_pke_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out)
{
    int ret;
    pke_null_ptr_chk(n);
    pke_null_ptr_chk(n->data);
    pke_null_ptr_chk(k);
    pke_null_ptr_chk(k->data);
    pke_null_ptr_chk(in);
    pke_null_ptr_chk(in->data);
    pke_null_ptr_chk(out);
    pke_null_ptr_chk(out->data);
    drv_err_chk_return(n->length > DRV_PKE_LEN_4096, ERROR_INVALID_PARAM, "n_len is invalid\n");
    drv_err_chk_return(in->length > DRV_PKE_LEN_4096, ERROR_INVALID_PARAM, "in_len is invalid\n");
    drv_err_chk_return(out->length < n->length, ERROR_INVALID_PARAM, "out_len is too small\n");

    ret = inner_modular_chk(n, true);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret);

    ret = hal_pke_alg_rsa_exp_mod(n, k, in, out);

    return ret;
}