/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: drvbox call define for SELiteOS.
 */

#ifndef DRVBOX_CALLS_DEF_H
#define DRVBOX_CALLS_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef LITEOS_DRVBOX_CALL_DEF
#error LITEOS_DRVBOX_CALL_DEF not defined
#endif

#define SECURITY_UNIFIED_BASE 0

/* HASH API */
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sha256, SECURITY_UNIFIED_BASE + 0)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sha256_start, SECURITY_UNIFIED_BASE + 1)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sha256_update, SECURITY_UNIFIED_BASE + 2)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sha256_finish, SECURITY_UNIFIED_BASE + 3)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sm3, SECURITY_UNIFIED_BASE + 4)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sm3_start, SECURITY_UNIFIED_BASE + 5)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sm3_update, SECURITY_UNIFIED_BASE + 6)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sm3_finish, SECURITY_UNIFIED_BASE + 7)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_sha_destroy, SECURITY_UNIFIED_BASE + 8)

/* TRNG API */
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_trng_get_random, SECURITY_UNIFIED_BASE + 9)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_trng_get_random_bytes, SECURITY_UNIFIED_BASE + 10)

/* PKE. */
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_ecc_gen_key, SECURITY_UNIFIED_BASE + 11)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_ecdsa_sign, SECURITY_UNIFIED_BASE + 12)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_ecdsa_verify, SECURITY_UNIFIED_BASE + 13)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_ecc_gen_ecdh_key, SECURITY_UNIFIED_BASE + 14)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_sm2_dsa_hash, SECURITY_UNIFIED_BASE + 15)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_sm2_public_encrypt, SECURITY_UNIFIED_BASE + 16)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_sm2_private_decrypt, SECURITY_UNIFIED_BASE + 17)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_sm2_sign, SECURITY_UNIFIED_BASE + 18)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_sm2_verify, SECURITY_UNIFIED_BASE + 19)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_rsa_sign, SECURITY_UNIFIED_BASE + 20)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_rsa_verify, SECURITY_UNIFIED_BASE + 21)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_rsa_public_encrypt, SECURITY_UNIFIED_BASE + 22)
LITEOS_DRVBOX_CALL_DEF(uapi_drv_cipher_pke_rsa_private_decrypt, SECURITY_UNIFIED_BASE + 23)

#undef LITEOS_DRVBOX_CALL_DEF

#ifdef LITEOS_DRVBOX_FUNC_DEF
/* HASH API */
errcode_t drvbox_uapi_drv_cipher_sha256(const uint8_t *buf, uint32_t len, uint8_t out[32]);
errcode_t drvbox_uapi_drv_cipher_sha256_start(hash_handle_t *ctx);
errcode_t drvbox_uapi_drv_cipher_sha256_update(hash_handle_t ctx, const uint8_t *buf, uint32_t len);
errcode_t drvbox_uapi_drv_cipher_sha256_finish(hash_handle_t ctx, uint8_t out[32]);
errcode_t drvbox_uapi_drv_cipher_sm3(const uint8_t *buf, uint32_t len, uint8_t out[32]);
errcode_t drvbox_uapi_drv_cipher_sm3_start(hash_handle_t *ctx);
errcode_t drvbox_uapi_drv_cipher_sm3_update(hash_handle_t ctx, const uint8_t *buf, uint32_t len);
errcode_t drvbox_uapi_drv_cipher_sm3_finish(hash_handle_t ctx, uint8_t out[32]);
errcode_t drvbox_uapi_drv_cipher_sha_destroy(hash_handle_t ctx);

/* SYMC API */
errcode_t drvbox_uapi_drv_cipher_symc_crypt(uint32_t alg, const uint8_t *src, uint8_t *dst, uint32_t data_len,
    uint8_t iv[16], const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle, bool is_encrypt);
errcode_t drvbox_uapi_drv_cipher_symc_gcm_encrypt(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *plain_text, uint8_t *cipher_text, uint32_t data_len,
    uint8_t *tag, uint32_t tag_len);
errcode_t drvbox_uapi_drv_cipher_symc_gcm_decrypt_verify(uapi_drv_cipher_symc_alg_t alg,
    const uint8_t *key, uint32_t key_len, uint32_t keyslot_handle,
    const uint8_t *iv, uint32_t iv_len,
    const uint8_t *aad, uint32_t aad_len,
    const uint8_t *cipher_text, uint8_t *plain_text, uint32_t data_len,
    const uint8_t *tag, uint32_t tag_len);

/* TRNG API */
errcode_t drvbox_uapi_drv_cipher_trng_get_random(uint32_t *randnum);
errcode_t drvbox_uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size);

/* PKE API */
errcode_t drvbox_uapi_drv_cipher_pke_ecc_gen_key(
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_ecc_point_t *output_pub_key);
errcode_t drvbox_uapi_drv_cipher_pke_ecdsa_sign(
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash, const uapi_drv_cipher_pke_ecc_sig_t *sig);
errcode_t drvbox_uapi_drv_cipher_pke_ecdsa_verify(
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash, const uapi_drv_cipher_pke_ecc_sig_t *sig);
errcode_t drvbox_uapi_drv_cipher_pke_ecc_gen_ecdh_key(
    uapi_drv_cipher_pke_ecc_curve_type_t curve_type, const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key, const uapi_drv_cipher_pke_data_t *output_shared_key);
errcode_t drvbox_uapi_drv_cipher_pke_sm2_dsa_hash(
    const uapi_drv_cipher_pke_data_t *sm2_id, const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg, uapi_drv_cipher_pke_data_t *hash);
errcode_t drvbox_uapi_drv_cipher_pke_sm2_public_encrypt(
    const uapi_drv_cipher_pke_ecc_point_t *pub_key, const uapi_drv_cipher_pke_data_t *plain_text,
    const uapi_drv_cipher_pke_data_t *cipher_text);
errcode_t drvbox_uapi_drv_cipher_pke_sm2_private_decrypt(
    const uapi_drv_cipher_pke_data_t *priv_key, const uapi_drv_cipher_pke_data_t *cipher_text,
    const uapi_drv_cipher_pke_data_t *plain_text);
errcode_t drvbox_uapi_drv_cipher_pke_sm2_sign(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);
errcode_t drvbox_uapi_drv_cipher_pke_sm2_verify(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig);
errcode_t drvbox_uapi_drv_cipher_pke_rsa_sign(
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key, uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type, const uapi_drv_cipher_pke_data_t *input_hash,
    uapi_drv_cipher_pke_data_t *sign);
errcode_t drvbox_uapi_drv_cipher_pke_rsa_verify(
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key, uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type, uapi_drv_cipher_pke_data_t *input_hash,
    const uapi_drv_cipher_pke_data_t *sig);
errcode_t drvbox_uapi_drv_cipher_pke_rsa_public_encrypt(
    uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key, const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label, uapi_drv_cipher_pke_data_t *output);
errcode_t drvbox_uapi_drv_cipher_pke_rsa_private_decrypt(
    uapi_drv_cipher_pke_rsa_scheme_t scheme, uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key, const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label, const uapi_drv_cipher_pke_data_t *output);

#endif

#undef LITEOS_DRVBOX_FUNC_DEF

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DRVBOX_CALLS_DEF_H */
