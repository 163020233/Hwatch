/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke curve point mul implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_CURVE_POINT_MUL

int hal_pke_alg_curve_base_point_mul(const drv_pke_data *d, const drv_pke_ecc_point *r)
{
    drv_pke_ecc_point base;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    hal_pke_alg_ecc_init_check();

    ecc_curve = hal_pke_alg_ecc_get_curve();

    base.x = (unsigned char *)ecc_curve->gx;
    base.y = (unsigned char *)ecc_curve->gy;
    base.length = ecc_curve->ksize;

    return hal_pke_alg_curve_point_mul(d, &base, r);
}

#define MAX_CURVE_KLEN      56

CRYPTO_STATIC void inner_decode_priv_key(unsigned char *output_key, const unsigned char *input_key, unsigned int klen)
{
    crypto_pke_common_little_big_endian_trans(output_key, input_key, klen);
    if (klen == DRV_PKE_LEN_256) {
        /* curve25519 decode private key. */
        output_key[klen - 1] = output_key[klen - 1] & 0xF8; /* & 248 */
        output_key[0] = output_key[0] & 0x7F;   /* & 127 */
        output_key[0] = output_key[0] | 0x40;   /* | 64 */
    } else {
        /* curve448 decode private key. */
        output_key[klen - 1] = output_key[klen - 1] & 0xFC; /* & 252 */
        output_key[klen - 56] = output_key[0] | 0x80;   /* 56: skip 7 bytes, 0x80: | 128 */
    }
}

CRYPTO_STATIC void inner_decode_point_x(unsigned char *output_x, const unsigned char *input_x, unsigned int len)
{
    crypto_pke_common_little_big_endian_trans(output_x, input_x, len);
    if (len == DRV_PKE_LEN_256) {
        /* curve25519 need further processing. */
        output_x[0] &= (1 << (CRYPTO_BITS_IN_BYTE - 1)) - 1;
    }
}

CRYPTO_STATIC int inner_curve_ecfp_mont_ladder(const unsigned char *k, const unsigned char *x,
    const unsigned char *mod_p, unsigned int len)
{
    int ret;
    unsigned int work_len = len / ALIGNED_TO_WORK_LEN_IN_BYTE;
    uint32_t loop_num;
    drv_pke_data k_data;
    const rom_lib *batch_instr_loop[] = {&instr_curve25519_ladder_0, &instr_curve25519_ladder_1, NULL, NULL};

    /* Step 1: set calculation data into PKE DRAM. */
    hal_pke_set_ram_romable(curve_addr_x1, x, len);
    hal_pke_set_ram_romable(curve_addr_m, mod_p, len);

    /* Step 2: pre process for curve montgomery ladder calculation. */
    ret = inner_batch_instr_process(&instr_curve25519_pre_ladder, work_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* Step 3: point add + times point calculation to calculate the point multiplication. */
    k_data.data = (unsigned char *)k;
    k_data.length = len;
    loop_num = len * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length(&k_data);

    ret = hal_pke_loop_batch_compute_romable(loop_num, k, NULL, len, batch_instr_loop);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    return ret;
}
#define JAC_TO_AFF_INSTR_NUM    6
CRYPTO_STATIC int inner_curve_ecfp_xz_to_x(const unsigned char *mod_p, unsigned int len)
{
    drv_pke_data mod_p_data = {
        .data = (unsigned char *)mod_p,
        .length = len
    };

    /* Step 1: prepare instructions to use */
    rom_lib batch_instr_block[JAC_TO_AFF_INSTR_NUM] = {instr_curve25519_xz2x_pre, instr_curve25519_xz2x_exp_one_bit_00,
        instr_curve25519_xz2x_exp_one_bit_01, instr_curve25519_xz2x_post};

    /* Step 2: start transfer calculation. */
    return inner_jac_to_aff_cal(batch_instr_block, JAC_TO_AFF_INSTR_NUM, &mod_p_data);
}

CRYPTO_STATIC int inner_curve_ecfp_mul(const unsigned char *decode_d, const unsigned char *decode_px,
    unsigned char *rx, unsigned int len)
{
    int ret;
    const drv_pke_ecc_curve *curve = hal_pke_alg_ecc_get_curve();
    const pke_ecc_init_param *init_param = hal_pke_alg_ecc_get_init_param();

    /* Step 1: set montgomery parameters into register. */
    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* Step 2: Montgomery Ladder to calculate the point multiplication for curve25519/curve448. */
    ret = inner_curve_ecfp_mont_ladder(decode_d, decode_px, curve->p, curve->ksize);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_curve_ecfp_mont_ladder failed\n");

    /* Step 3: Jacobin to Affine coordinate system tranform. */
    ret = inner_curve_ecfp_xz_to_x(curve->p, curve->ksize);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_curve_ecfp_xz_to_x failed\n");

    /* Step 4: get data result from PKE DRAM. */
    hal_pke_get_ram_romable(curve_addr_x2, rx, len);

    return CRYPTO_SUCCESS;
}

CRYPTO_STATIC void inner_encode_point_x(unsigned char *output_x, const unsigned char *input_x,
    unsigned int len)
{
    crypto_pke_common_little_big_endian_trans(output_x, input_x, len);
}

int hal_pke_alg_curve_point_mul(const drv_pke_data *d, const drv_pke_ecc_point *p, const drv_pke_ecc_point *r)
{
    int ret;
    unsigned char decode_d[MAX_CURVE_KLEN];
    unsigned char decode_px[MAX_CURVE_KLEN];
    unsigned char local_rx[MAX_CURVE_KLEN];
    unsigned int klen = hal_pke_alg_ecc_get_klen();

    inner_decode_priv_key(decode_d, d->data, d->length);

    inner_decode_point_x(decode_px, p->x, p->length);

    ret = inner_curve_ecfp_mul(decode_d, decode_px, local_rx, klen);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_curve_ecfp_mul failed\n");

    if (inner_drv_is_zero(local_rx, klen)) {

        drv_set_err(CRYPTO_FAILURE, "direct error return");
        return CRYPTO_FAILURE;
    }

    inner_encode_point_x(r->x, local_rx, klen);

    return CRYPTO_SUCCESS;
}