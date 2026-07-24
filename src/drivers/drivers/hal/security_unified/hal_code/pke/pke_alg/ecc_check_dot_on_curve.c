/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc check dot on curve implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#include "crypto_pke_common.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_CHECK_DOT_ON_CURVE

static int inner_ecfp_demontgomery_data_jac_z(const drv_pke_data *z, unsigned int work_len)
{
    int ret;
    crypto_drv_func_enter();

    /* Step 1: start calculate. */
    ret = inner_batch_instr_process(&instr_ecfp_demont_cz_3, work_len);
    drv_err_chk_func_return(inner_batch_instr_process, ret);

    /* Step 2: get data from PKE DRAM */
    if (z != NULL && z->data != NULL) {
        hal_pke_get_ram_romable(ecc_addr_cz, z->data, z->length);
    }

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}

static int inner_ecc_ecfp_point_valid_standard(const drv_pke_ecc_curve *curve, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve)
{
    int ret;
    unsigned char z[DRV_PKE_LEN_576] = {0};
    drv_pke_data check_z = { .data = z, .length = curve->ksize };
    drv_pke_data mod_p = { .data = (unsigned char *)curve->p, .length = curve->ksize };
    drv_pke_data kk = { .data = (unsigned char *)curve->n, .length = curve->ksize };
    const pke_ecc_init_param *init_param = NULL;
    crypto_drv_func_enter();

    init_param = hal_pke_alg_ecc_get_init_param();

    /* Step 0: set montgomery param into register. */
    ret = inner_update_modulus(mod_p.data, mod_p.length, init_param->mont_param_p);
    drv_err_chk_func_return(inner_update_modulus, ret);

    /* Step 1: montgomerize the pub_key point */
    ret = inner_ecfp_montgomery_data_aff(pub_key, &mod_p, NULL);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_ecfp_montgomery_data_aff failed\n");

    /* Step 2: trans Affine coordinate system to Jacobin coordinate system */
    ret = inner_ecfp_aff_to_jac(NULL, &mod_p, NULL);
    drv_err_chk_func_return(inner_ecfp_aff_to_jac, ret);

    /* Step 3: calculate [n] * P */
    ret = inner_ecfp_mul_naf_cal(curve->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE, &kk);
    drv_err_chk_func_return(inner_ecfp_mul_naf_cal, ret);

    /* Step 4: no need to trans result data from jacobin coordinate system to affine coordinate system. */

    /* Step 5: demontgomery data of point.z. */
    ret = inner_ecfp_demontgomery_data_jac_z(&check_z, curve->ksize / ALIGNED_TO_WORK_LEN_IN_BYTE);
    drv_err_chk_func_return(inner_ecfp_demontgomery_data_jac_z, ret);

    *is_on_curve = TD_FALSE;
    if (inner_drv_is_zero(check_z.data, check_z.length)) {
        *is_on_curve = TD_TRUE;
    };

    crypto_drv_func_exit();
    return CRYPTO_SUCCESS;
}

int hal_pke_alg_ecc_check_dot_on_curve(const drv_pke_ecc_point *point, td_bool *is_on_curve)
{
    const drv_pke_ecc_curve *curve = NULL;
    hal_pke_alg_ecc_init_check();

    curve = hal_pke_alg_ecc_get_curve();
    drv_err_chk_return(curve == NULL, CRYPTO_FAILURE, "hal_pke_alg_ecc_get_curve failed\n");

    return inner_ecc_ecfp_point_valid_standard(curve, point, is_on_curve);
}