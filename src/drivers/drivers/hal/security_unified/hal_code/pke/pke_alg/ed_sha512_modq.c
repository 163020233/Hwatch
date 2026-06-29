/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ed sha512 modq implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define MAX_HASH_SIZE       64
#define ED25519_KEY_LEN     32
int hal_pke_alg_ed_mod(const drv_pke_data *in, const drv_pke_data *p, const drv_pke_data *out)
{
    int ret;
    unsigned int work_len = p->length / ALIGNED_TO_WORK_LEN_IN_BYTE;
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();

    hal_pke_set_ram_romable(ed_addr_t1, in->data, ED25519_KEY_LEN);
    hal_pke_set_ram_romable(ed_addr_t0, in->data + ED25519_KEY_LEN, ED25519_KEY_LEN);
    hal_pke_set_ram_romable(ed_addr_m, p->data, p->length);

    hal_pke_set_mont_para_romable(init_param->mont_param_n);

    ret = inner_batch_instr_process(&instr_ed25519_calc_h, work_len);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    hal_pke_get_ram_romable(ed_addr_h, out->data, out->length);

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_ed_sha512_modq(const drv_pke_data *arr, unsigned int arr_len, const drv_pke_data *out)
{
    int ret;
    unsigned char h[MAX_HASH_SIZE];
    unsigned char h_reverse[MAX_HASH_SIZE];
    drv_pke_data hash_data = {
        .data = h,
        .length = MAX_HASH_SIZE
    };
    drv_pke_data p_data = {
        .data = (unsigned char *)hal_pke_alg_ecc_get_n(),
        .length = ED25519_KEY_LEN
    };

    ret = crypto_pke_common_calc_hash(arr, arr_len, DRV_PKE_HASH_TYPE_SHA512, &hash_data);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "crypto_pke_common_calc_hash failed\n");

    crypto_pke_common_little_big_endian_trans(h_reverse, h, MAX_HASH_SIZE);

    hash_data.data = h_reverse;
    ret = hal_pke_alg_ed_mod(&hash_data, &p_data, out);
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ed_mod failed\n");

    return CRYPTO_SUCCESS;
}