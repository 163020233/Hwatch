/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc point mul implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_drv_common.h"
#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "hal_pke_func.h"
#include "pke_alg_inner.h"
#include "drv_pke_priv.h"

#define MAX_ECC_LENGTH        72
#define PKE_MONT_LEN_IN_WORD 4
#define ALIGNED_IN_16_BYTE 16
#define RAND_T_LEN 8

int hal_pke_alg_ecc_base_point_mul_sec(const drv_pke_data *k, const drv_pke_ecc_point *r)
{
    drv_pke_ecc_point base;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    hal_pke_alg_ecc_init_check();

    ecc_curve = hal_pke_alg_ecc_get_curve();

    base.x = (unsigned char *)ecc_curve->gx;
    base.y = (unsigned char *)ecc_curve->gy;
    base.length = ecc_curve->ksize;

    return hal_pke_alg_ecc_point_mul_sec(k, &base, r);
}

static int inner_hal_pke_alg_compute_t_s_sec(const drv_pke_data *k, const drv_pke_ecc_curve *ecc_curve,
    drv_pke_data *t_data, drv_pke_data *s_data, unsigned int *out_max_bit_len)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned int klen = ecc_curve->ksize;
    unsigned int work_len;
    unsigned int max_bit_len; /* calculate from t_data */
    unsigned int rand_s_bit_len; /* calculate from max_bit_len */
    unsigned int rand_s_byte_len;
    unsigned char rand_t[RAND_T_LEN];
    unsigned char *rand_s;
    unsigned int mont_param_const_1[PKE_MONT_LEN_IN_WORD] = {0, 0, 0, 1};

    /* STEP0: init process */
    work_len = ((klen + RAND_T_LEN + ALIGNED_IN_16_BYTE - 1) / ALIGNED_IN_16_BYTE) * ALIGNED_IN_16_BYTE * 2; /* 2: ts */
    hal_pke_set_work_len_romable(work_len);
    hal_pke_set_mont_para_romable(mont_param_const_1);

    /* STEP1.1: get rand_t (1, 2**(rand_bit_len-1)) */
    ret = inner_get_random_sec(rand_t, RAND_T_LEN);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_get_random failed\n");

    /* STEP1.2: r_t = r_t | 2**(rand_bit_len-1) */
    random_delay();
    rand_t[0] |= 0x80; /* 0x80: make sure highest bit is 1 */
    random_delay();

    /* STEP1.3: set data to pke ram */
    hal_pke_set_ram_word_all_romable(ecc_addr_m, work_len, 0xffffffff);     /* all 0xff */
    hal_pke_set_ram_word_romable(ecc_addr_t1, work_len, -1, 0x1);  /* const_1  */
    hal_pke_set_ram_romable(ecc_addr_cy, ecc_curve->n, klen);
    random_delay();
    hal_pke_set_ram_romable(ecc_addr_ax, rand_t, RAND_T_LEN);
    random_delay();
    hal_pke_set_ram_romable(ecc_addr_az, k->data, k->length);
    random_delay();
    hal_pke_set_ram_word_all_romable(ecc_addr_py, work_len, 0); /* rand_num: const_0 */

    /* STEP1.4: run instr to calculate t */
    ret = inner_batch_instr_process_sec(&instr_ecfp_blind_key_t, work_len);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* STEP1.5: get final t */
    hal_pke_get_ram_romable(ecc_addr_t3, t_data->data, t_data->length);
    max_bit_len = t_data->length * CRYPTO_BITS_IN_BYTE - crypto_pke_common_get_high_0_bit_length_sec(t_data) - 1;
    crypto_pke_common_clear_highest_bit_sec(t_data);

    /* STEP2.1: get rand_s_bit_len from t effective len */
    if (ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_FIPS_P521R) {
        rand_s_bit_len = max_bit_len - 521;  /* 521: curve real key length */
    } else {
        rand_s_bit_len = max_bit_len - klen * CRYPTO_BITS_IN_BYTE;
    }
    rand_s_byte_len = (rand_s_bit_len + CRYPTO_BITS_IN_BYTE - 1) / CRYPTO_BITS_IN_BYTE;

    /* STEP2.2: get rand_s (1, 2**(max_bit_len - bit_len_old - 1)) */
    rand_s = crypto_malloc_coherent(rand_s_byte_len);
    crypto_chk_return(rand_s == NULL, ERROR_MALLOC, "crypto_malloc rand_s failed\n");

    ret = inner_get_random_sec(rand_s, rand_s_byte_len);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, free_s, "inner_get_random failed\n");

    /* STEP2.3: r_s = r_s | 2**(max_bit_len - bit_len_old - 1) */
    /* make sure highest bit is 1, higher bit is 0 */
    crypto_pke_common_set_bit_to_1_and_clear_higher_bit(&rand_s[0],
        7 - (rand_s_byte_len * CRYPTO_BITS_IN_BYTE - rand_s_bit_len));  /* 7: highest bit position value */

    /* STEP2.4: set data to pke ram, most data has been already setted during t process */
    hal_pke_set_ram_not_aligned(ecc_addr_ax, rand_s, rand_s_byte_len, work_len);

    /* STEP2.5: run instr to calculate s */
    ret = inner_batch_instr_process_sec(&instr_ecfp_blind_key_s, work_len);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, free_s, "inner_batch_instr_process failed\n");

    /* STEP2.6: get final s */
    hal_pke_get_ram_romable(ecc_addr_s, s_data->data, s_data->length);

    *out_max_bit_len = max_bit_len;
free_s:
    crypto_free_coherent(rand_s);
    return ret;
}

static int inner_gen_random_point_sec(unsigned int klen)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned int loop_num;
    unsigned char rand_z[MAX_ECC_LENGTH];
    unsigned char rand_k[RAND_T_LEN];
    unsigned int z_len = klen;
    unsigned int k_len = RAND_T_LEN;
    const rom_lib *batch_instr_loop[] = {&instr_ecfp_mul_c_double_22, &instr_ecfp_mul_jj_22_23, NULL, NULL};

    ret = inner_get_random_sec(rand_z, z_len);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_get_random failed\n");

    hal_pke_set_ram_romable(ecc_addr_pz, rand_z, z_len);

    ret = inner_batch_instr_process_sec(&instr_ecfp_blind_gen_point_r, klen);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    ret = inner_get_random_sec(rand_k, k_len);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_get_random failed\n");
    random_delay();
    rand_k[0] |= 0x80; /* make sure highest bit is 1 */
    random_delay();

    loop_num = k_len * CRYPTO_BITS_IN_BYTE;
    ret = hal_pke_loop_batch_compute_sec(loop_num, rand_k, NULL, k_len, batch_instr_loop);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");

    ret = inner_batch_instr_process_sec(&instr_ecfp_blind_gen_point_t_new, klen);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    return ret;
}

static int inner_point_mul_calculate_sec(const drv_pke_data t_data, const drv_pke_data s_data, unsigned int klen,
    unsigned int max_bit_len)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned int i;
    unsigned int tmp_r;
    unsigned int init_flag;
    unsigned int loop_num;
    unsigned char t_data_mask;
    unsigned char s_data_mask;
    const rom_lib mix_point[] = {instr_ecfp_blind_mix_point_t_0_new, instr_ecfp_blind_mix_point_t_1_new,
        instr_ecfp_blind_mix_point_t_2_new, instr_ecfp_blind_mix_point_t_3_new};
    const rom_lib init_point[] = {instr_ecfp_blind_init_point_q_0, instr_ecfp_blind_init_point_q_1,
        instr_ecfp_blind_init_point_q_2, instr_ecfp_blind_init_point_q_3};
    const rom_lib *batch_instr_loop[] = {&instr_ecfp_pm_blind_0, &instr_ecfp_pm_blind_1,
        &instr_ecfp_pm_blind_2, &instr_ecfp_pm_blind_3};

    /* 混淆随机点 */
    ret = inner_get_random_sec((unsigned char *)&tmp_r, sizeof(tmp_r));
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_get_random failed\n");
    random_delay();
    tmp_r %= 4;    /* 4: tmpr is rand 0 1 2 3 */
    random_delay();

    ret = inner_batch_instr_process_sec(&mix_point[tmp_r], klen);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* 产生初始点 */
    init_flag = \
        ((uint32_t)crypto_pke_common_get_bit_from_low_to_high(t_data.data, t_data.length, max_bit_len - 1) << 1) +
        (uint32_t)crypto_pke_common_get_bit_from_low_to_high(s_data.data, s_data.length, max_bit_len - 1);
    init_flag ^= tmp_r;
    ret = inner_batch_instr_process_sec(&init_point[init_flag], klen);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "inner_batch_instr_process failed\n");

    /* 循环计算 */
    s_data_mask = ((tmp_r & 0x1) == 0) ? 0 : 0xff;
    t_data_mask = (((tmp_r >> 1) & 0x1) == 0) ? 0 : 0xff;
    for (i = 0; i < s_data.length && i < t_data.length; i++) {
        s_data.data[i] ^= s_data_mask;
        t_data.data[i] ^= t_data_mask;
    }
    crypto_chk_return(i != s_data.length && i != t_data.length, CRYPTO_ENHANCE_FAILURE, "for loop is not ended\n");

    loop_num = max_bit_len - 1;
    ret = hal_pke_loop_batch_compute_sec(loop_num, s_data.data, t_data.data, s_data.length, batch_instr_loop);
    crypto_chk_return(ret != CRYPTO_ENHANCE_SUCCESS, ret, "hal_pke_loop_batch_compute_romable failed\n");
    return ret;
}

int hal_pke_alg_ecc_point_mul_sec(const drv_pke_data *k, const drv_pke_ecc_point *p, const drv_pke_ecc_point *r)
{
    volatile int ret = CRYPTO_ENHANCE_FAILURE;
    unsigned int klen;
    unsigned int max_bit_len = 0;
    unsigned char *buffer;
    unsigned int max_ts_len;
    drv_pke_data t_data;
    drv_pke_data s_data;
    drv_pke_data mod_p;
    const drv_pke_ecc_curve *ecc_curve = NULL;
    const pke_ecc_init_param *init_param = NULL;
    hal_pke_alg_ecc_init_check();

    ecc_curve = hal_pke_alg_ecc_get_curve();
    init_param = hal_pke_alg_ecc_get_init_param();
    klen = ecc_curve->ksize;
    mod_p.data = (unsigned char *)ecc_curve->p;
    mod_p.length = klen;

    /* Step 1. compute t and s, work_len change */
    max_ts_len = klen + RAND_T_LEN;

    buffer = crypto_malloc_coherent(max_ts_len * 2);    /* 2: t and s */
    crypto_chk_return(buffer == NULL, ERROR_MALLOC, "crypto_malloc failed\n");

    t_data.data = buffer;
    t_data.length = max_ts_len;
    s_data.data = buffer + max_ts_len;
    s_data.length = max_ts_len;
    ret = inner_hal_pke_alg_compute_t_s_sec(k, ecc_curve, &t_data, &s_data, &max_bit_len);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, free, "inner_update_modulus failed\n");

    /* Step 2: set p/point/mont_param into register, work_len recovery klen. */
    hal_pke_set_work_len_romable(klen);
    hal_pke_set_ram_romable(ecc_addr_m, ecc_curve->p, klen);
    hal_pke_set_ram_romable(ecc_addr_px, p->x, p->length);
    hal_pke_set_ram_romable(ecc_addr_py, p->y, p->length);
    hal_pke_set_mont_para_romable(init_param->mont_param_p);

    /* Step 3: generate random point */
    ret = inner_gen_random_point_sec(klen);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, free, "inner_gen_random_point failed\n");

    /* Step 4: 混淆随机点，产生初始点，循环运算 */
    ret = inner_point_mul_calculate_sec(t_data, s_data, klen, max_bit_len);
    crypto_chk_goto(ret != CRYPTO_ENHANCE_SUCCESS, free, "inner_point_mul_calculate failed\n");

    /* Step 5: trans result data from jacobin coordinate system to affine coordinate system. */
    ret = inner_ecfp_jac_to_aff(&mod_p);
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, free, "inner_ecfp_jac_to_aff failed\n");
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, free, "inner_ecfp_jac_to_aff failed\n");
    ret = CRYPTO_ENHANCE_SUCCESS;

    /* Step 6. demontgomery data. */
    ret = inner_ecfp_demontgomery_data_aff(r);
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, free, "inner_ecfp_demontgomery_data_aff failed\n");
    random_delay();
    crypto_chk_goto(ret != CRYPTO_SUCCESS, free, "inner_ecfp_demontgomery_data_aff failed\n");
    ret = CRYPTO_ENHANCE_SUCCESS;

free:
    crypto_free_coherent(buffer);
    return ret;
}