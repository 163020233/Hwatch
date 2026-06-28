/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: big number API header file.
 *
 * Create: 2023-03-15
*/

#ifndef DRV_PKE_CAL_H
#define DRV_PKE_CAL_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/*
 * normal big number calculate function.
 */
td_s32 drv_cipher_pke_add_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c);

td_s32 drv_cipher_pke_sub_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c);

td_s32 drv_cipher_pke_mul_mod(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *p,
    const drv_pke_data *c);

td_s32 drv_cipher_pke_inv_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c);

td_s32 drv_cipher_pke_mod(const drv_pke_data *a, const drv_pke_data *p, const drv_pke_data *c);

td_s32 drv_cipher_pke_mul(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c);

td_s32 drv_cipher_pke_exp_mod(const drv_pke_data *n, const drv_pke_data *k, const drv_pke_data *in,
    const drv_pke_data *out);

/*
 * ecc big number calculate function.
 */
td_s32 drv_cipher_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    td_bool *is_on_curve);

td_s32 drv_cipher_pke_mul_dot(drv_pke_ecc_curve_type curve_type, const drv_pke_data *k, const drv_pke_ecc_point *p,
    const drv_pke_ecc_point *r);

int32_t drv_cipher_pke_mul_dot_add(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r);

int32_t drv_cipher_pke_add_dot(drv_pke_ecc_curve_type curve_type,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r);

int32_t drv_cipher_pke_mul_dot_sub(drv_pke_ecc_curve_type curve_type,
    const drv_pke_data *a, const drv_pke_data *b,
    const drv_pke_ecc_point *p, const drv_pke_ecc_point *q, const drv_pke_ecc_point *r);

int32_t drv_cipher_pke_get_ecc_param(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_curve **ecc_curve);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif  /* DRV_PKE_CAL_H */
