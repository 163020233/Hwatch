/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke dh implement.
 *
 * Create: 2025-01-07
*/

#include "drv_pke.h"
#include "drv_pke_cal.h"
#include "drv_pke_priv.h"
#include "crypto_pke_common.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_DRV_PKE_DH

int drv_cipher_pke_dh_gen_key(const drv_pke_data *g_data, const drv_pke_data *mod_n,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_priv_key, const drv_pke_data *output_pub_key)
{
    int ret;
    /* check ptr. */
    pke_null_ptr_chk(g_data);
    pke_null_ptr_chk(g_data->data);
    pke_null_ptr_chk(mod_n);
    pke_null_ptr_chk(mod_n->data);
    pke_null_ptr_chk(output_priv_key);
    pke_null_ptr_chk(output_priv_key->data);
    pke_null_ptr_chk(output_pub_key);
    pke_null_ptr_chk(output_pub_key->data);
    drv_crypto_pke_check_param(input_priv_key != TD_NULL && input_priv_key->length != 0 &&
        input_priv_key->data == TD_NULL);
    drv_crypto_pke_check_param(mod_n->length != DRV_PKE_LEN_1024 && mod_n->length != DRV_PKE_LEN_2048 &&
        mod_n->length != DRV_PKE_LEN_3072 && mod_n->length != DRV_PKE_LEN_4096);
    /* if there is input private key, the key length should equal to the modulus length. */
    drv_crypto_pke_check_param(input_priv_key != TD_NULL && input_priv_key->length != 0 &&
        mod_n->length != input_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length < g_data->length);
    drv_crypto_pke_check_param(mod_n->length != output_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length != output_pub_key->length);
    /* the modulur couldn't be zero. */
    drv_crypto_pke_check_param(inner_drv_is_zero(mod_n->data, mod_n->length));

    /* Step 1: get private key. */
    if (input_priv_key != TD_NULL  && input_priv_key->data != TD_NULL && input_priv_key->length != 0) {
        ret = memcpy_s(output_priv_key->data, output_priv_key->length, input_priv_key->data,
            input_priv_key->length);
        drv_err_chk_return(ret != EOK, ERROR_MEMCPY_S, "memcpy_s failed\n");
    } else {
        /* genarate private key by random. */
        ret = inner_get_limit_random(output_priv_key->data, mod_n->data, mod_n->length);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "inner_get_limit_random failed\n");
    }
    /* step 2: get public key. */
    ret = drv_cipher_pke_exp_mod(mod_n, output_priv_key, g_data, output_pub_key);
    return ret;
}

CRYPTO_STATIC int pke_dh_check_pub_key(const drv_pke_data *mod_n, const drv_pke_data *input_pub_key)
{
    int ret;
    unsigned char *tmp = crypto_malloc_coherent(input_pub_key->length);
    drv_err_chk_return(tmp == TD_NULL, CRYPTO_FAILURE, "malloc failed\n");
    /* shouldn't be 0 */
    drv_err_chk_goto_with_ret(ret, inner_drv_is_zero(input_pub_key->data, input_pub_key->length),
        exit_free, PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be 1 */
    tmp[input_pub_key->length - 1] = 1;
    ret = memcmp(input_pub_key->data, tmp, input_pub_key->length);
    drv_err_chk_goto_with_ret(ret, ret == 0, exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be p */
    ret = memcmp(input_pub_key->data, mod_n->data, input_pub_key->length);
    drv_err_chk_goto_with_ret(ret, ret == 0, exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    /* shouldn't be p-1 */
    ret = memcpy_s(tmp, input_pub_key->length, mod_n->data, mod_n->length);
    drv_err_chk_goto_with_ret(ret, ret != EOK, exit_free,
        PKE_COMPAT_ERRNO(ERROR_MEMCPY_S), "memcpy_s failed\n");

    crypto_pke_common_bn_sub_one(tmp, input_pub_key->length);
    ret = memcmp(input_pub_key->data, tmp, input_pub_key->length);
    drv_err_chk_goto_with_ret(ret, ret == 0, exit_free,
        PKE_COMPAT_ERRNO(ERROR_INVALID_PARAM), "invalid public key\n");
    ret = TD_SUCCESS;

exit_free:
    crypto_free_coherent(tmp);
    return ret;
}

int drv_cipher_pke_dh_compute_key(const drv_pke_data *mod_n, const drv_pke_data *input_priv_key,
    const drv_pke_data *input_pub_key, const drv_pke_data *output_shared_key)
{
    int ret = CRYPTO_FAILURE;
    /* check ptr. */
    pke_null_ptr_chk(mod_n);
    pke_null_ptr_chk(mod_n->data);
    pke_null_ptr_chk(input_priv_key);
    pke_null_ptr_chk(input_priv_key->data);
    pke_null_ptr_chk(input_pub_key);
    pke_null_ptr_chk(input_pub_key->data);
    pke_null_ptr_chk(output_shared_key);
    pke_null_ptr_chk(output_shared_key->data);
    drv_crypto_pke_check_param(mod_n->length != DRV_PKE_LEN_1024 && mod_n->length != DRV_PKE_LEN_2048 &&
        mod_n->length != DRV_PKE_LEN_3072 && mod_n->length != DRV_PKE_LEN_4096);
    drv_crypto_pke_check_param(mod_n->length != input_priv_key->length);
    drv_crypto_pke_check_param(mod_n->length != input_pub_key->length);
    drv_crypto_pke_check_param(mod_n->length != output_shared_key->length);
    /* check whether public key is legal. */
    ret = pke_dh_check_pub_key(mod_n, input_pub_key);
    drv_err_chk_func_return(pke_dh_check_pub_key, ret);

    /* start calculate. */
    ret = drv_cipher_pke_exp_mod(mod_n, input_priv_key, input_pub_key, output_shared_key);
    return ret;
}