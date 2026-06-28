/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: psa crypto wrap common code. \n
 *
 * History: \n
 * 2025-03-14, Create file. \n
 */

#include "psa_crypto_wrap.h"

#include "psa/psa_crypto.h"
#include "psa_crypto_common.h"
#include "unified_cipher_pke.h"

/* key_data_length 和 bits的chechk会在import时根据ken_length确定bits时确定，这里所有就不用重复检查了 */

psa_status_t psa_get_rsa_private_key(psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_rsa_priv_key_t *private_key)
{
    psa_unused(data_length);

    private_key->n = (uint8_t *)(uintptr_t)data;
    private_key->n_len = bits / PSA_BYTES_IN_BIT;

    private_key->e = (uint8_t *)((uintptr_t)data + private_key->n_len);
    private_key->e_len = PSA_CRYPTO_RSA_PUBLIC_KEY_E_LENGTH;

    private_key->d = (uint8_t *)((uintptr_t)data + private_key->n_len + private_key->e_len);
    private_key->d_len = bits / PSA_BYTES_IN_BIT;
    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_get_rsa_public_key(psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_rsa_pub_key_t *public_key)
{
    psa_unused(data_length);

    public_key->len = bits / PSA_BYTES_IN_BIT;
    public_key->n = (uint8_t *)(uintptr_t)data;
    public_key->e = (uint8_t *)((uintptr_t)data + public_key->len);
    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_get_ecc_private_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data,
    uint32_t data_length, uapi_drv_cipher_pke_data_t *private_key)
{
    uint32_t real_key_len = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(bits);

    psa_crypto_chk_return(!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type), PSA_ERROR_INVALID_ARGUMENT, "not ecc key pair\n");
    psa_crypto_chk_return(real_key_len != data_length, PSA_ERROR_INVALID_ARGUMENT, "private key data_length invalid\n");

    private_key->data = (uint8_t *)(uintptr_t)data;
    private_key->length = real_key_len;
    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_get_ecc_public_key(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data, uint32_t data_length,
    uapi_drv_cipher_pke_ecc_point_t *public_key)
{
    uint32_t real_key_len = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(bits);

    psa_crypto_chk_return(!PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type), PSA_ERROR_INVALID_ARGUMENT, "not ecc public key\n");
    psa_crypto_chk_return(real_key_len != (data_length / 2), PSA_ERROR_INVALID_ARGUMENT,    /* 2: x + y */
        "public key data_length invalid\n");

    public_key->x = (uint8_t *)(uintptr_t)data;
    public_key->y = (uint8_t *)((uintptr_t)data + real_key_len);
    public_key->length = real_key_len;
    return PSA_INTERNAL_SUCCESS;
}

psa_status_t psa_get_ecc_sign(psa_key_type_t type, psa_key_bits_t bits, const uint8_t *data,
    uint32_t data_length, uapi_drv_cipher_pke_ecc_sig_t *sign)
{
    uint32_t real_key_len = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(bits);

    psa_crypto_chk_return(data_length < real_key_len * 2, PSA_ERROR_INVALID_ARGUMENT,    /* 2: r + s */
        "sign data_length invalid\n");

    psa_unused(type);

    sign->r = (uint8_t *)data;
    sign->s = (uint8_t *)((uintptr_t)data + real_key_len);
    sign->length = real_key_len;
    return PSA_INTERNAL_SUCCESS;
}

void psa_get_ecc_c1_and_c2(psa_key_bits_t bits, uint8_t *data, uapi_drv_cipher_pke_ecc_point_t *c1,
    uapi_drv_cipher_pke_data_t *c2)
{
    uint32_t real_key_len = PSA_CRYPTO_BIT_ALIGNED_IN_BYTE(bits);

    c1->x = (uint8_t *)data;
    c1->y = (uint8_t *)data + real_key_len;
    c1->length = real_key_len;
    c2->data = (uint8_t *)data + real_key_len * 2; /* 2: double real_key_len */
    c2->length = real_key_len;
}

/* only for uapi_xxx return, can not use for inner function */
int psa_get_wrap_errcode(errcode_t errcode)
{
    switch (errcode) {
        case ERRCODE_SUCC:
            return PSA_INTERNAL_SUCCESS;
        case ERRCODE_SECURITY_SYMC_AEAD_TAG_INVALID:
            return PSA_ERROR_INVALID_SIGNATURE;
        case ERRCODE_SECURITY_PKE_RSA_CRYPTO_BUFFER_TOO_SMALL:
            return PSA_ERROR_BUFFER_TOO_SMALL;
        case ERRCODE_SECURITY_PKE_RSA_VERIFY_V15_CHECK:
        case ERRCODE_SECURITY_PKE_RSA_VERIFY_PSS_CHECK:
            return PSA_ERROR_INVALID_SIGNATURE;
        case ERRCODE_NV_KEY_NOT_FOUND:
            return PSA_ERROR_DOES_NOT_EXIST;
        case ERRCODE_NV_INVALID_PARAM:
        case ERRCODE_INVALID_PARAM:
            return PSA_ERROR_INVALID_ARGUMENT;
        case ERRCODE_NV_INVALID_KEY_HEADER:
            return PSA_ERROR_DATA_CORRUPT;
        case ERRCODE_NV_ILLEGAL_OPERATION:
        case ERRCODE_NV_TRYING_TO_MODIFY_A_PERMANENT_KEY:
            return PSA_ERROR_NOT_PERMITTED;
        case ERRCODE_NV_NO_ENOUGH_SPACE:
            return PSA_ERROR_INSUFFICIENT_STORAGE;
        case ERRCODE_MALLOC:
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        default:
            return PSA_ERROR_GENERIC_ERROR;
    }
}
