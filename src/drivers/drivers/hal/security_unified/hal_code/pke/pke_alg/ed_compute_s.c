/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ed compute_s implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_COMPUTE_S

#define ED25519_KEY_LEN     32

/* s = (r + h * a) % q. */
int hal_pke_alg_ed_compute_s(const drv_pke_data *r_data, const drv_pke_data *h_data,
    const drv_pke_data *a_data, const drv_pke_data *s_data)
{
    int ret;
    unsigned int klen = hal_pke_alg_ecc_get_klen();
    unsigned int work_len = klen / ALIGNED_TO_WORK_LEN_IN_BYTE;
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();
    const drv_pke_data q_data = {
        .data = (unsigned char *)hal_pke_alg_ecc_get_n(),
        .length = klen
    };
    unsigned char s_tmp[ED25519_KEY_LEN];

    hal_pke_set_ram_romable(ed_addr_sk, a_data->data, a_data->length);
    hal_pke_set_ram_romable(ed_addr_h, h_data->data, h_data->length);
    hal_pke_set_ram_romable(ed_addr_r, r_data->data, r_data->length);
    hal_pke_set_ram_romable(ed_addr_m, q_data.data, q_data.length);

    hal_pke_set_mont_para_romable(init_param->mont_param_n);

    ret = inner_batch_instr_process(&instr_ed25519_calc_s, work_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    hal_pke_get_ram_romable(ed_addr_s, s_tmp, s_data->length);

    crypto_pke_common_little_big_endian_trans(s_data->data, s_tmp, s_data->length);

    return CRYPTO_SUCCESS;
}