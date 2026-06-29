/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: cipher driver pke header. \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#ifndef DRV_PKE_H
#define DRV_PKE_H

#include "crypto_pke_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 drv_cipher_pke_init(void);

td_s32 drv_cipher_pke_deinit(void);

int32_t drv_cipher_pke_get_ecc_param(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_curve **ecc_curve);

td_void drv_cipher_pke_init_ecc_param(const pke_default_parameters *ecc_params, const td_u32 *ecc_num);

td_s32 drv_cipher_pke_rsa_gen_key(const drv_pke_data *input_e, drv_pke_rsa_priv_key *output_priv_key);

td_s32 drv_cipher_pke_rsa_sign(const drv_pke_rsa_priv_key *priv_key, const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    const drv_pke_data *sign);

td_s32 drv_cipher_pke_rsa_verify(const drv_pke_rsa_pub_key *pub_key, const drv_pke_rsa_scheme scheme,
    const drv_pke_hash_type hash_type, const drv_pke_data *input_hash, const drv_pke_data *sign);

td_s32 drv_cipher_pke_rsa_public_encrypt(const drv_pke_rsa_scheme scheme, const drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);

td_s32 drv_cipher_pke_rsa_private_decrypt(const drv_pke_rsa_scheme scheme, const drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);

td_s32 drv_cipher_pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);

td_s32 drv_cipher_pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key);

td_s32 drv_cipher_pke_ecdsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_ecc_gen_key_sec(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);

td_s32 drv_cipher_pke_ecc_gen_ecdh_key_sec(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key);

td_s32 drv_cipher_pke_ecdsa_sign_sec(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_ecdsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_ecdsa_verify_s(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig, const drv_pke_data *v);

td_s32 drv_cipher_pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_sm2_dsa_hash(const drv_pke_data *sm2_id, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, drv_pke_data *hash);

td_s32 drv_cipher_pke_sm2_public_encrypt(const drv_pke_ecc_point *pub_key, const drv_pke_data *plain_text,
    const drv_pke_data *cipher_text);

td_s32 drv_cipher_pke_sm2_private_decrypt(const drv_pke_data *priv_key, const drv_pke_data *cipher_text,
    const drv_pke_data *plain_text);

td_s32 drv_cipher_pke_sm2_sign(const drv_pke_data *priv_key, const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_sm2_verify(const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

td_s32 drv_cipher_pke_dh_gen_key(const drv_pke_data *g_data, const drv_pke_data *mod_n,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_priv_key, const drv_pke_data *output_pub_key);

td_s32 drv_cipher_pke_dh_compute_key(const drv_pke_data *mod_n, const drv_pke_data  *input_priv_key,
    const drv_pke_data  *input_pub_key, const drv_pke_data  *output_shared_key);

td_s32 drv_cipher_pke_ed_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);

td_s32 drv_cipher_pke_curve_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);

td_s32 drv_cipher_pke_curve_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key);

td_s32 drv_cipher_pke_ec_elgamal_encrypt(drv_pke_ecc_curve_type curve_type, const drv_pke_data *plain_text,
    const drv_pke_ecc_point *pub_key, drv_pke_ecc_point *c1, drv_pke_data *c2);

td_s32 drv_cipher_pke_ec_elgamal_decrypt(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_ecc_point *c1, const drv_pke_data *c2, drv_pke_data *output);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
