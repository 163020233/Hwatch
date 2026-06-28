/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides cipher driver pke utils. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-28, Create file. \n
 */
#ifndef CRYPTO_PKE_COMMON_H
#define CRYPTO_PKE_COMMON_H

#include "crypto_pke_struct.h"
#include "crypto_drv_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void hal_pke_alg_init_ecc_param(const pke_default_parameters *ecc_params, uint32_t ecc_num);

const drv_pke_ecc_curve *get_ecc_curve(drv_pke_ecc_curve_type curve_type);

const pke_ecc_init_param *get_ecc_init_param(drv_pke_ecc_curve_type curve_type);

int32_t get_ecc_param(drv_pke_ecc_curve_type curve_type,
    const drv_pke_ecc_curve **ecc_curve, const pke_ecc_init_param **ecc_init_param);

void crypto_curve_param_init(void);

void crypto_curve_param_deinit(void);

int32_t crypto_pke_common_secret_expand(const uint8_t *secret, uint8_t *a,
    uint8_t *prefix, uint32_t len);

void crypto_pke_common_little_big_endian_trans(uint8_t *output_endian, const uint8_t *input_endian,
    uint32_t byte_len);

const uint8_t *crypto_curve_get_ed25519_p_minus_5_div_8(void);

int32_t crypto_pke_common_bin2naf(const uint8_t *k, const uint32_t k_len, int32_t *k_naf, uint32_t *k_naf_len);

#define MAX_NAF_LEN     128
int32_t crypto_pke_common_bin2naf_fast(const uint8_t *x, uint8_t *positive_bits, uint8_t *negative_bits,
    const uint32_t len, uint32_t *carry);

bool inner_drv_is_zero(const uint8_t *val, uint32_t length);

uint32_t inner_drv_is_zero_sec(const uint8_t *val, uint32_t length);

/*
 * c = a + b.
 * required: len(a) = len(b) = len, len(c) = len + 1.
 */
void crypto_pke_common_normal_add(const uint8_t *a, const uint8_t *b, uint8_t *c,
    uint32_t len);

#define PKE_MONT_LEN_IN_BYTE        16
void crypto_pke_common_get_mont_param(const uint8_t p[PKE_MONT_LEN_IN_BYTE],
    uint8_t mont_param[PKE_MONT_LEN_IN_BYTE]);

void crypto_pke_common_bn_add_one(uint8_t *data, uint32_t len);

int32_t crypto_pke_rsa_hash_len_check(uint32_t klen, drv_pke_hash_type hash_type);

void crypto_pke_common_bn_sub_one(uint8_t *data, uint32_t len);

/*
 * Return 0 if a == b.
 * Return > 0 if a > b.
 * Return < 0 if a < b.
 */
int32_t crypto_pke_common_data_cmp(const drv_pke_data *a, const drv_pke_data *b);

int32_t crypto_pke_common_add_mod(const drv_pke_data *a, const drv_pke_data *b, drv_pke_data *c,
    const drv_pke_data *n);

/* a >= b - 1, a.length = c.length, a.length >= b.length */
int32_t crypto_pke_common_normal_sub(const drv_pke_data *a, const drv_pke_data *b, const drv_pke_data *c);

bool crypto_pke_common_get_bit_from_low_to_high(const uint8_t *in, uint32_t size, uint32_t pos);

bool crypto_pke_common_get_bit_from_high_to_low(const uint8_t *bn, uint32_t index);

uint32_t crypto_pke_common_get_high_0_bit_length(const drv_pke_data *a);

/* len(a) = len(b) */
uint32_t crypto_pke_common_get_two_bn_high_0_bit_length(const drv_pke_data *a, const drv_pke_data *b);

void crypto_pke_common_clear_highest_bit(drv_pke_data *a);

void crypto_pke_common_set_bit_to_1_and_clear_higher_bit(uint8_t *byte, uint32_t pos);

uint32_t crypto_pke_common_get_high_0_bit_length_sec(const drv_pke_data *a);

void crypto_pke_common_clear_highest_bit_sec(drv_pke_data *a);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif