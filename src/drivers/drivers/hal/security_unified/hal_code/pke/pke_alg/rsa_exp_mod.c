/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke rsa exp mod implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "hal_pke_romable.h"

#include "pke_alg_inner.h"

#define MAX_OFFSET_OF_VALID_BIT 2

static int rsa_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out)
{
    int ret;
    unsigned int i = 0;
    int j = 0;
    const rom_lib *rom_lib_list[4] = {    // 4: select by bit_2
        &instr_rsa_exp_00, &instr_rsa_exp_01, &instr_rsa_exp_10, &instr_rsa_exp_11
    };
    unsigned char bit_2 = 0;
    bool start_flag = false;
    uint32_t work_len = crypto_max(n->length, in->length);

    crypto_drv_func_enter();

    hal_pke_set_work_len_romable(work_len);

    ret = inner_update_rsa_modulus(n->data, n->length);
    crypto_chk_func_return(inner_update_rsa_modulus, ret);

    /* step 1: data montgomery and copy. */
    /* 1. set data into DRAM. */
    hal_pke_set_ram_romable(rsa_addr_n, n->data, n->length);
    hal_pke_set_ram_romable(rsa_addr_a, in->data, in->length);

    /* set last word to 1. */
    hal_pke_set_ram_word_romable(rsa_addr_const_1, n->length, -1, 0x1);
    /* 2. start calculate. */
    ret = inner_batch_instr_process(&instr_rsa_exp_pre_6, 0);
    crypto_chk_func_return(inner_batch_instr_process, ret);

    /*
     * 1) k->data is one byte array, k->length is the byte number.
     * 2) For each byte in k->data, get bit2 = byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
     * 3) If there is one bit2 is not zero, then all the next bit2(contain this time) will call
        inner_batch_instr_process(), the first param depends on bit2's value.
     */
    for (i = 0; i < k->length; i++) {
        for (j = 3; j >= 0; j--) {                  // 3: get byte[7:6], byte[5:4], byte[3:2], byte[1:0] in order.
            bit_2 = (k->data[i] >> (j * 2)) & 0x3;   // 2, 0x3: get bit2
            if ((start_flag == false) && (bit_2 != 0)) {
                start_flag = true;
                ret = inner_batch_instr_process(rom_lib_list[bit_2], 0);
                crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");
            } else if (start_flag == true) {
                ret = inner_batch_instr_process(rom_lib_list[bit_2], 0);
                crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");
            }
        }
    }

    /* step 3: data demontgomery and twice modulus reduction. */
    ret = inner_batch_instr_process(&instr_rsa_exp_post_3, 0);
    crypto_chk_func_return(inner_batch_instr_process, ret);

    /* step 4: get data out. */
    (void)memset_s(out->data, out->length, 0x00, out->length);
    hal_pke_get_ram_romable(rsa_addr_s, out->data, out->length);

    crypto_drv_func_exit();
    return ret;
}

int32_t hal_pke_alg_rsa_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out)
{
    int32_t ret = inner_pke_alg_resume();
    crypto_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    ret = rsa_exp_mod(n, k, in, out);
    inner_pke_alg_suspend();

    return ret;
}