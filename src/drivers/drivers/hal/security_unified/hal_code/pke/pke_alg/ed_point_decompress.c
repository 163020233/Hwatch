/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ed point decompress implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_POINT_DECOMPRESS

#define ED25519_KEY_LEN     32
static int inner_recovery_x(const unsigned char *y, const unsigned char *p, unsigned char *x, unsigned char sign,
    unsigned int byte_len)
{
    int ret;
    unsigned char t0[DRV_PKE_LEN_256];
    unsigned char t1[DRV_PKE_LEN_256];
    const unsigned char *mod_p_5 = crypto_curve_get_ed25519_p_minus_5_div_8();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();
    /* get work_len */
    unsigned int work_len = byte_len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    const rom_lib *batch_instr_loop[4] = {      // 4: select by bit_2
        &instr_ed25519_recover_x_exp_one_bit_00, &instr_ed25519_recover_x_exp_one_bit_01, NULL, NULL};
    uint32_t loop_num;
    drv_pke_data mod_p_5_data;

    /* 1. set necessary data into DRAM, which should have been set by former process. */
    hal_pke_set_ram_romable(ed_addr_ty, y, byte_len);
    hal_pke_set_ram_romable(ed_addr_m, p, byte_len);

    /* Step 1.2 set montgomery parameters into register. */
    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* 2. start recovery x from y by curve equation. x^2 = (y^2 - 1) / (d*y^2 + 1) (mod p) */
    /* 2.1 preprocess, initialize the data, use batch process step. */
    ret = inner_batch_instr_process(&instr_ed25519_recover_x_pre, work_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* calculate by binary data, from high address to low address. */
    mod_p_5_data.data = (unsigned char *)mod_p_5;
    mod_p_5_data.length = DRV_PKE_LEN_256;
    loop_num = DRV_PKE_LEN_256 * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length(&mod_p_5_data);
    ret = hal_pke_loop_batch_compute_romable(loop_num, mod_p_5, NULL, DRV_PKE_LEN_256, batch_instr_loop);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_loop_batch_instr_process failed\n");

    /* 2.2 postprocess, demontgomery and complete reduction */
    ret = inner_batch_instr_process(&instr_ed25519_recover_x_post, work_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* 3. get result from DRAM. */
    hal_pke_get_ram_romable(ed_addr_t0, t0, byte_len);
    hal_pke_get_ram_romable(ed_addr_t1, t1, byte_len);
    /* u = (y^2 - 1), y = (d*y^2 + 1), there are three cases. */
    if (inner_drv_is_zero(t0, byte_len)) {
        /* if v * x^2 = u (mod p), x is a square root. */
        hal_pke_get_ram_romable(ed_addr_tx0, x, byte_len);
        ret = inner_batch_instr_process(&instr_ed25519_cpy_tx0_to_tx1, work_len);
    } else if (inner_drv_is_zero(t1, byte_len)) {
        /* If v x^2 = -u (mod p), set x <-- x * 2^((p-1)/4), which is a square root. */
        hal_pke_get_ram_romable(ed_addr_tx1, x, byte_len);
        ret = inner_batch_instr_process(&instr_ed25519_cpy_tx1_to_tx0, work_len);
    } else {
        /* Otherwise, no square root exists for modulo p, and decoding fails. */
        drv_set_err(CRYPTO_FAILURE, "direct error return");
        return CRYPTO_FAILURE;
    }
    drv_err_chk_func_return(inner_batch_instr_process, ret);
    /* Finally, use the x_0 bit to select the right square root. */
    if (inner_drv_is_zero(x, byte_len) && sign == 0x01) {
        /* If x = 0, and x_0 = 1, decoding fails. */
        drv_set_err(CRYPTO_FAILURE, "direct error return");
        return CRYPTO_FAILURE;
    } else if ((x[byte_len - 1] & 0x01) != sign) {
        /* if x_0 != x mod 2, set x <-- p - x. */
        ret = inner_batch_instr_process(&instr_ed25519_cpy_tx1_to_tx0, work_len);
        drv_err_chk_func_return(inner_batch_instr_process, ret);
        hal_pke_get_ram_romable(ed_addr_tx0, x, byte_len);
    }

    return CRYPTO_SUCCESS;
}

int hal_pke_alg_ecc_point_decompress(const drv_pke_data *p, const drv_pke_ecc_point *point_decompress)
{
    int ret;
    unsigned char sign = 0; /* store the symbol to select the right x coordinate result. */
    const drv_pke_ecc_curve *ecc_curve = hal_pke_alg_ecc_get_curve();
    const unsigned char *ed_p = ecc_curve->p;

    crypto_pke_common_little_big_endian_trans(point_decompress->y, p->data, p->length);

    /* Bit 255 of this number is the least significant bit of the x-coordinate and denote this value x_0 */
    sign = point_decompress->y[0] >> 7; /* 7: get least significant bit. */
    /* The y-coordinate is recovered simply by clearing the bit 255. */
    point_decompress->y[0] &= 0x7F;
    /* If the resulting value pkey->y is >= p, decoding fails. */
    drv_err_chk_return(memcmp(point_decompress->y, ed_p, ED25519_KEY_LEN) >= 0, ERROR_INVALID_PARAM,
        "pkey->y must smaller than p\n");

    ret = inner_recovery_x(point_decompress->y, ed_p, point_decompress->x, sign, ED25519_KEY_LEN);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_recovery_x failed\n");

    return CRYPTO_SUCCESS;
}