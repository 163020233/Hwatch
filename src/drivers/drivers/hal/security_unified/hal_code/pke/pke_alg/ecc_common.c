/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke ecc common implement.
 *
 * Create: 2025-01-07
*/
#include "hal_pke_alg.h"

#include "crypto_pke_common.h"
#include "hal_pke_romable.h"
#include "pke_alg_inner.h"

#define FILE_ID FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_COMMON

/* ECC Curve Parameters. */
ROM_SHARE_BSS const pke_default_parameters *g_ecc_params;
ROM_SHARE_BSS uint32_t g_ecc_num = 0;
ROM_SHARE_BSS const drv_pke_ecc_curve *g_ecc_curve = NULL;
ROM_SHARE_BSS const pke_ecc_init_param *g_init_param = NULL;
void hal_pke_alg_init_ecc_param(const pke_default_parameters *ecc_params, uint32_t ecc_num)
{
    g_ecc_params = ecc_params;
    g_ecc_num = ecc_num;
}

void inner_set_param(const pke_ecc_init_param *init_param, const drv_pke_ecc_curve *ecc_curve)
{
    uint32_t ksize = ecc_curve->ksize;
    uint32_t idx = 0;
    uint8_t ecc_addr_list[] = {
        ecc_addr_n, ecc_addr_p, ecc_addr_rrn, ecc_addr_rrp,
        ecc_addr_mont_a, ecc_addr_mont_b, ecc_addr_mont_1_p, ecc_addr_mont_1_n,
        ecc_addr_const_0, ecc_addr_const_1
    };
    uint8_t ed_addr_list[] = {
        ed_addr_n, ed_addr_p, ed_addr_rrn, ed_addr_rrp,
        ed_addr_mont_d, ed_addr_sqrt_m1, ed_addr_mont_1_p, ed_addr_mont_1_n,
        ed_addr_const_0, ed_addr_const_1
    };
    uint8_t curve_addr_list[] = {
        curve_addr_n, curve_addr_p, curve_addr_rrn, curve_addr_rrp,
        curve_addr_mont_a24, curve_addr_mont_1_p, curve_addr_mont_1_n,
        curve_addr_const_0, curve_addr_const_1,
        0   /* will never be visited, used to align to ecc_addr_list and ed_addr_list. */
    };

    uint8_t *addr_list = ecc_addr_list;
    if (ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC8032) {
        addr_list = ed_addr_list;
    } else if (ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC7748 || ecc_curve->ecc_type == DRV_PKE_ECC_TYPE_RFC7748_448) {
        addr_list = curve_addr_list;
    }

    hal_pke_set_ram_romable(addr_list[idx++], ecc_curve->n, ksize);
    hal_pke_set_ram_romable(addr_list[idx++], ecc_curve->p, ksize);
    hal_pke_set_ram_romable(addr_list[idx++], init_param->rrn, ksize);
    hal_pke_set_ram_romable(addr_list[idx++], init_param->rrp, ksize);

    hal_pke_set_ram_romable(addr_list[idx++], init_param->mont_a, ksize);
    if (init_param->mont_b != NULL) {
        hal_pke_set_ram_romable(addr_list[idx++], init_param->mont_b, ksize);
    }

    hal_pke_set_ram_romable(addr_list[idx++], init_param->mont_1_p, ksize);
    hal_pke_set_ram_romable(addr_list[idx++], init_param->mont_1_n, ksize);

    /* set zero. */
    hal_pke_set_ram_word_romable(addr_list[idx++], ksize, 0, 0);
    /* set const_1. */
    hal_pke_set_ram_word_romable(addr_list[idx++], ksize, -1, 0x1);
}

int hal_pke_alg_ecc_init(drv_pke_ecc_curve_type curve_type)
{
    int ret;

    ret = inner_pke_alg_resume();
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_pke_alg_resume failed\n");

    ret = hal_pke_alg_ecc_get_param(curve_type, &g_ecc_curve, &g_init_param);
    drv_err_chk_goto(ret != CRYPTO_SUCCESS, error_unlock, "hal_pke_alg_ecc_get_param failed\n");
    drv_err_chk_goto_with_ret(ret, g_ecc_curve == NULL, error_unlock, CRYPTO_FAILURE,
        "hal_pke_alg_ecc_get_param for ecc_curve failed\n");
    drv_err_chk_goto_with_ret(ret, g_init_param == NULL, error_unlock, CRYPTO_FAILURE,
        "hal_pke_alg_ecc_get_param for init_param failed\n");

    if (curve_type == DRV_PKE_ECC_TYPE_FIPS_P521R) {
        hal_pke_set_work_len_romable(DRV_PKE_LEN_640);    /* 128 bit aligned */
    } else {
        hal_pke_set_work_len_romable(g_ecc_curve->ksize);
    }

    inner_set_param(g_init_param, g_ecc_curve);

    return ret;

error_unlock:
    inner_pke_alg_suspend();
    g_ecc_curve = NULL;
    g_init_param = NULL;
    return ret;
}

void hal_pke_alg_ecc_deinit(void)
{
    if (g_ecc_curve == NULL) {
        return;
    }
    g_ecc_curve = NULL;
    g_init_param = NULL;
    inner_pke_alg_suspend();
}

bool hal_pke_alg_ecc_is_init(void)
{
    if (g_ecc_curve == NULL) {
        return false;
    }
    return true;
}

const unsigned char *hal_pke_alg_ecc_get_n(void)
{
    return g_ecc_curve->n;
}

const unsigned char *hal_pke_alg_ecc_get_p_minus_2(void)
{
    return g_init_param->p_minus_2;
}

unsigned int hal_pke_alg_ecc_get_klen(void)
{
    return g_ecc_curve->ksize;
}

const drv_pke_ecc_curve *hal_pke_alg_ecc_get_curve(void)
{
    return g_ecc_curve;
}

const pke_ecc_init_param *hal_pke_alg_ecc_get_init_param(void)
{
    return g_init_param;
}

int __attribute__((weak)) hal_pke_alg_ecc_get_param(drv_pke_ecc_curve_type curve_type,
    const drv_pke_ecc_curve **ecc_curve, const pke_ecc_init_param **ecc_init_param)
{
    uint32_t i;
    const pke_default_parameters *param = NULL;
    drv_err_chk_return(g_ecc_params == NULL, CRYPTO_FAILURE, "ecc_param not register!\n");

    for (i = 0; i < g_ecc_num; i++) {
        param = &g_ecc_params[i];
        if (param->curve_param->ecc_type == curve_type) {
            *ecc_curve = param->curve_param;
            if (ecc_init_param != NULL) {
                *ecc_init_param = param->default_param;
            }
            return CRYPTO_SUCCESS;
        }
    }
    *ecc_curve = NULL;
    if (ecc_init_param != NULL) {
        *ecc_init_param = NULL;
    }

    drv_set_err(ERROR_UNSUPPORT, "direct error return");
    return ERROR_UNSUPPORT;
}