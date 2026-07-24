/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides pke driver source \n
 *
 * History: \n
 * 2023-03-22, Create file. \n
 */
#include "unified_cipher_pke.h"
#include "security_pke_pri.h"
#include "drv_pke.h"
#include "drv_pke_cal.h"
#include "hal_pke_func.h"

#include "sl_common.h"
#include "crypto_common_macro.h"
#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_SERVICE_LAYER_PKE

errcode_t uapi_drv_cipher_pke_ecc_gen_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key,
    const uapi_drv_cipher_pke_ecc_point_t *output_pub_key)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_GEN_KEY
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecc_gen_key((drv_pke_ecc_curve_type)curve_type, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_priv_key, (const drv_pke_ecc_point *)output_pub_key);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(input_priv_key);
    crypto_unused(output_priv_key);
    crypto_unused(output_pub_key);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_GEN_KEY */
}

errcode_t uapi_drv_cipher_pke_ecc_gen_key_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key,
    const uapi_drv_cipher_pke_ecc_point_t *output_pub_key)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_GEN_KEY
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecc_gen_key_sec((drv_pke_ecc_curve_type)curve_type, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_priv_key, (const drv_pke_ecc_point *)output_pub_key);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_enhance_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(input_priv_key);
    crypto_unused(output_priv_key);
    crypto_unused(output_pub_key);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_GEN_KEY */
}

errcode_t uapi_drv_cipher_pke_ecdsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_SIGN
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecdsa_sign((drv_pke_ecc_curve_type)curve_type, (const drv_pke_data *)priv_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(priv_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_SIGN */
}

errcode_t uapi_drv_cipher_pke_ecdsa_sign_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_SIGN
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecdsa_sign_sec((drv_pke_ecc_curve_type)curve_type, (const drv_pke_data *)priv_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_enhance_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(priv_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_SIGN */
}

errcode_t uapi_drv_cipher_pke_ecdsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_ecdsa_verify((drv_pke_ecc_curve_type)curve_type, (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_ecdsa_verify_s(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig,
    const uapi_drv_cipher_pke_data_t *v)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecdsa_verify_s((drv_pke_ecc_curve_type)curve_type, (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig, (const drv_pke_data *)v);
    crypto_service_postprocess(PKE_MODULE_ID);
    hal_trng_close_clk();
    return crypto_sl_common_get_errcode(ret);
}

#ifdef CONFIG_PKE_SUPPORT_EDWARD
errcode_t uapi_drv_cipher_pke_eddsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    int32_t ret = drv_cipher_pke_eddsa_sign((drv_pke_ecc_curve_type)curve_type, (const drv_pke_data *)priv_key,
        (const drv_pke_msg *)msg, (const drv_pke_ecc_sig *)sig);
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_eddsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    int32_t ret = drv_cipher_pke_eddsa_verify((drv_pke_ecc_curve_type)curve_type, (const drv_pke_ecc_point *)pub_key,
        (const drv_pke_msg *)msg, (const drv_pke_ecc_sig *)sig);
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_pke_eddsa_sign(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    crypto_unused(curve_type);
    crypto_unused(priv_key);
    crypto_unused(msg);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_eddsa_verify(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
    crypto_unused(curve_type);
    crypto_unused(pub_key);
    crypto_unused(msg);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_shared_key)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_ECDH
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecc_gen_ecdh_key((drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)input_pub_key, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_shared_key);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(input_pub_key);
    crypto_unused(input_priv_key);
    crypto_unused(output_shared_key);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_ECDH */
}

errcode_t uapi_drv_cipher_pke_ecc_gen_ecdh_key_sec(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *input_pub_key,
    const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_shared_key)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_ECDH
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_ecc_gen_ecdh_key_sec((drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)input_pub_key, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)output_shared_key);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_enhance_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(input_pub_key);
    crypto_unused(input_priv_key);
    crypto_unused(output_shared_key);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif /* CONFIG_PKE_SUPPORT_ECC_ECDH */
}

errcode_t uapi_drv_cipher_pke_check_dot_on_curve(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key, bool *is_on_curve)
{
#if defined(CONFIG_PKE_ECC_CHECK_DOT_ON_CURVE_SUPPORT)
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_check_dot_on_curve((drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)pub_key, (uint8_t *)is_on_curve);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(pub_key);
    crypto_unused(is_on_curve);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_pke_sm2_dsa_hash(const uapi_drv_cipher_pke_data_t *sm2_id,
    const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_msg_t *msg,
    uapi_drv_cipher_pke_data_t *hash)
{
#if defined(CONFIG_PKE_SUPPORT_SM2_SIGN) || defined(CONFIG_PKE_SUPPORT_SM2_VERIFY)
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    int32_t ret = drv_cipher_pke_sm2_dsa_hash((const drv_pke_data *)sm2_id,
        (const drv_pke_ecc_point *)pub_key, (const drv_pke_msg *)msg, (drv_pke_data *)hash);
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(sm2_id);
    crypto_unused(pub_key);
    crypto_unused(msg);
    crypto_unused(hash);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_pke_sm2_sign(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
#if defined(CONFIG_PKE_SUPPORT_SM2_SIGN)
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_sm2_sign((const drv_pke_data *)priv_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(priv_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

errcode_t uapi_drv_cipher_pke_sm2_verify(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *hash,
    const uapi_drv_cipher_pke_ecc_sig_t *sig)
{
#if defined(CONFIG_PKE_SUPPORT_SM2_VERIFY)
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_sm2_verify((const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)hash, (const drv_pke_ecc_sig *)sig);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(pub_key);
    crypto_unused(hash);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}

#ifdef CONFIG_PKE_SUPPORT_SM2_CRYPTO
errcode_t uapi_drv_cipher_pke_sm2_public_encrypt(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *plain_text,
    const uapi_drv_cipher_pke_data_t *cipher_text)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_sm2_public_encrypt((const drv_pke_ecc_point *)pub_key,
        (const drv_pke_data *)plain_text, (const drv_pke_data *)cipher_text);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_sm2_private_decrypt(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *cipher_text,
    const uapi_drv_cipher_pke_data_t *plain_text)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    int32_t ret = drv_cipher_pke_sm2_private_decrypt((const drv_pke_data *)priv_key,
        (const drv_pke_data *)cipher_text, (const drv_pke_data *)plain_text);
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_pke_sm2_public_encrypt(const uapi_drv_cipher_pke_ecc_point_t *pub_key,
    const uapi_drv_cipher_pke_data_t *plain_text,
    const uapi_drv_cipher_pke_data_t *cipher_text)
{
    crypto_unused(pub_key);
    crypto_unused(plain_text);
    crypto_unused(cipher_text);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_sm2_private_decrypt(const uapi_drv_cipher_pke_data_t *priv_key,
    const uapi_drv_cipher_pke_data_t *cipher_text,
    const uapi_drv_cipher_pke_data_t *plain_text)
{
    crypto_unused(priv_key);
    crypto_unused(cipher_text);
    crypto_unused(plain_text);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

#ifdef CONFIG_PKE_SUPPORT_RSA
errcode_t uapi_drv_cipher_pke_rsa_sign(const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_data_t *input_hash,
    uapi_drv_cipher_pke_data_t *sign)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_rsa_sign((const drv_pke_rsa_priv_key *)priv_key, (drv_pke_rsa_scheme)scheme,
        (drv_pke_hash_type)hash_type, (const drv_pke_data *)input_hash, (drv_pke_data *)sign);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_verify(const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    uapi_drv_cipher_pke_data_t *input_hash,
    const uapi_drv_cipher_pke_data_t *sig)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_rsa_verify((const drv_pke_rsa_pub_key *)pub_key, (drv_pke_rsa_scheme)scheme,
        (drv_pke_hash_type)hash_type, (drv_pke_data *)input_hash, (const drv_pke_data *)sig);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_public_encrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_rsa_public_encrypt((drv_pke_rsa_scheme)scheme, (drv_pke_hash_type)hash_type,
        (const drv_pke_rsa_pub_key *)pub_key, (const drv_pke_data *)input, (const drv_pke_data *)label,
        (drv_pke_data *)output);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_rsa_private_decrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output)
{
    crypto_service_preprocess(PKE_HASH_MODULE_ID);
    int32_t ret = drv_cipher_pke_rsa_private_decrypt((drv_pke_rsa_scheme)scheme, (drv_pke_hash_type)hash_type,
        (const drv_pke_rsa_priv_key *)priv_key, (const drv_pke_data *)input, (const drv_pke_data *)label,
        (drv_pke_data *)output);
    crypto_service_postprocess(PKE_HASH_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_dh_gen_key(const uapi_drv_cipher_pke_data_t *g_data,
    const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_data_t *output_pub_key)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    hal_trng_open_clk();
    int32_t ret = drv_cipher_pke_dh_gen_key((const drv_pke_data *)g_data, (const drv_pke_data *)mod_n,
        (const drv_pke_data *)input_priv_key, (const drv_pke_data *)output_priv_key,
        (const drv_pke_data *)output_pub_key);
    hal_trng_close_clk();
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_dh_compute_key(const uapi_drv_cipher_pke_data_t *mod_n,
    const uapi_drv_cipher_pke_data_t  *input_priv_key, const uapi_drv_cipher_pke_data_t  *input_pub_key,
    const uapi_drv_cipher_pke_data_t  *output_shared_key)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_dh_compute_key((const drv_pke_data *)mod_n, (const drv_pke_data *)input_priv_key,
        (const drv_pke_data *)input_pub_key, (const drv_pke_data *)output_shared_key);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_pke_rsa_sign(const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_data_t *input_hash,
    uapi_drv_cipher_pke_data_t *sign)
{
    crypto_unused(priv_key);
    crypto_unused(scheme);
    crypto_unused(hash_type);
    crypto_unused(input_hash);
    crypto_unused(sign);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_rsa_verify(const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    uapi_drv_cipher_pke_data_t *input_hash,
    const uapi_drv_cipher_pke_data_t *sig)
{
    crypto_unused(pub_key);
    crypto_unused(scheme);
    crypto_unused(hash_type);
    crypto_unused(input_hash);
    crypto_unused(sig);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_rsa_public_encrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_pub_key_t *pub_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output)
{
    crypto_unused(scheme);
    crypto_unused(hash_type);
    crypto_unused(pub_key);
    crypto_unused(input);
    crypto_unused(label);
    crypto_unused(output);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_rsa_private_decrypt(uapi_drv_cipher_pke_rsa_scheme_t scheme,
    uapi_drv_cipher_pke_hash_type_t hash_type,
    const uapi_drv_cipher_pke_rsa_priv_key_t *priv_key,
    const uapi_drv_cipher_pke_data_t *input,
    const uapi_drv_cipher_pke_data_t *label,
    uapi_drv_cipher_pke_data_t *output)
{
    crypto_unused(scheme);
    crypto_unused(hash_type);
    crypto_unused(priv_key);
    crypto_unused(input);
    crypto_unused(label);
    crypto_unused(output);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_dh_gen_key(const uapi_drv_cipher_pke_data_t *g_data,
    const uapi_drv_cipher_pke_data_t *mod_n, const uapi_drv_cipher_pke_data_t *input_priv_key,
    const uapi_drv_cipher_pke_data_t *output_priv_key, const uapi_drv_cipher_pke_data_t *output_pub_key)
{
    crypto_unused(g_data);
    crypto_unused(mod_n);
    crypto_unused(input_priv_key);
    crypto_unused(output_priv_key);
    crypto_unused(output_pub_key);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_dh_compute_key(const uapi_drv_cipher_pke_data_t *mod_n,
    const uapi_drv_cipher_pke_data_t  *input_priv_key, const uapi_drv_cipher_pke_data_t  *input_pub_key,
    const uapi_drv_cipher_pke_data_t  *output_shared_key)
{
    crypto_unused(mod_n);
    crypto_unused(input_priv_key);
    crypto_unused(input_pub_key);
    crypto_unused(output_shared_key);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

errcode_t uapi_drv_cipher_pke_add_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_add_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_sub_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_sub_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *p, const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_mul_mod((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)p,
        (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_inv_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_inv_mod((const drv_pke_data *)a, (const drv_pke_data *)p, (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mod(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *p,
    const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_mod((const drv_pke_data *)a, (const drv_pke_data *)p, (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul(const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_data_t *c)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_mul((const drv_pke_data *)a, (const drv_pke_data *)b, (const drv_pke_data *)c);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_exp_mod(const uapi_drv_cipher_pke_data_t *n, const uapi_drv_cipher_pke_data_t *k,
    const uapi_drv_cipher_pke_data_t *in, const uapi_drv_cipher_pke_data_t *out)
{
    int32_t ret = TD_SUCCESS;
    crypto_service_preprocess(PKE_MODULE_ID);
    ret = drv_cipher_pke_exp_mod((const drv_pke_data *)n, (const drv_pke_data *)k, (const drv_pke_data *)in,
        (const drv_pke_data *)out);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul_dot(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *k, const uapi_drv_cipher_pke_ecc_point_t *p,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_mul_dot(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_data *)k,
        (const drv_pke_ecc_point *)p,
        (const drv_pke_ecc_point *)r);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

#ifdef CONFIG_PKE_SUPPORT_ECC_ADD_DOT
errcode_t uapi_drv_cipher_pke_mul_dot_add(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_mul_dot_add(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_data *)a,
        (const drv_pke_data *)b,
        (const drv_pke_ecc_point *)p,
        (const drv_pke_ecc_point *)q,
        (const drv_pke_ecc_point *)r);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_add_dot(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_add_dot(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_ecc_point *)p,
        (const drv_pke_ecc_point *)q,
        (const drv_pke_ecc_point *)r);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}

errcode_t uapi_drv_cipher_pke_mul_dot_sub(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_service_preprocess(PKE_MODULE_ID);
    int32_t ret = drv_cipher_pke_mul_dot_sub(
        (drv_pke_ecc_curve_type)curve_type,
        (const drv_pke_data *)a,
        (const drv_pke_data *)b,
        (const drv_pke_ecc_point *)p,
        (const drv_pke_ecc_point *)q,
        (const drv_pke_ecc_point *)r);
    crypto_service_postprocess(PKE_MODULE_ID);
    return crypto_sl_common_get_errcode(ret);
}
#else
errcode_t uapi_drv_cipher_pke_mul_dot_add(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_unused(curve_type);
    crypto_unused(a);
    crypto_unused(b);
    crypto_unused(p);
    crypto_unused(q);
    crypto_unused(r);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_add_dot(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_unused(curve_type);
    crypto_unused(p);
    crypto_unused(q);
    crypto_unused(r);
    return ERRCODE_SECURITY_UNSUPPORT;
}

errcode_t uapi_drv_cipher_pke_mul_dot_sub(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_data_t *a, const uapi_drv_cipher_pke_data_t *b,
    const uapi_drv_cipher_pke_ecc_point_t *p, const uapi_drv_cipher_pke_ecc_point_t *q,
    const uapi_drv_cipher_pke_ecc_point_t *r)
{
    crypto_unused(curve_type);
    crypto_unused(a);
    crypto_unused(b);
    crypto_unused(p);
    crypto_unused(q);
    crypto_unused(r);
    return ERRCODE_SECURITY_UNSUPPORT;
}
#endif

errcode_t uapi_drv_cipher_pke_get_ecc_param(uapi_drv_cipher_pke_ecc_curve_type_t curve_type,
    const uapi_drv_cipher_pke_ecc_param **ecc_param)
{
#ifdef CONFIG_PKE_SUPPORT_ECC_GET_PARAM
    int32_t ret = CRYPTO_SUCCESS;
    ret = drv_cipher_pke_get_ecc_param((drv_pke_ecc_curve_type)curve_type, (const drv_pke_ecc_curve **)ecc_param);
    return crypto_sl_common_get_errcode(ret);
#else
    crypto_unused(curve_type);
    crypto_unused(ecc_param);
    return ERRCODE_SECURITY_UNSUPPORT;
#endif
}