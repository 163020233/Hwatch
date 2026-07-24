/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: drv pke rsa v15 implement.
 *
 * Create: 2025-01-07
*/

/* According to RFC2313. */
#include "drv_pke_priv.h"

#define FILE_ID FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_V15

#define RSA_PADLEN_1                1
#define RSA_PADLEN_3                3
#define RSA_PADLEN_11               11

/* EM = 00 || 01 || PS || 00 || T */
int pkcs1_v15_sign(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    int ret;
    rsa_pkcs1_hash_info hash_info = {0};
    unsigned char *p = NULL;
    unsigned int idx = 0;

    ret = pkcs1_get_hash(hash_type, NULL, &hash_info);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x01;

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        p[idx++] = CRYPTO_BYTE_MAX;
    }

    p[idx++] = 0x00;

    /* T */
    ret = memcpy_s(&p[idx], pack->em_len - idx, hash_info.asn1_data, hash_info.asn1_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    idx += hash_info.asn1_len;
    ret = memcpy_s(&p[idx], pack->em_len - idx, pack->hash, hash_info.hash_len);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);

    return ret;
}

/* check EM = 00 || 01 || PS || 00 || T */
int pkcs1_v15_verify(const drv_pke_hash_type hash_type, const rsa_pkcs1_pack *pack)
{
    int ret;
    rsa_pkcs1_hash_info hash_info = {0};
    unsigned char *p = NULL;
    unsigned int idx = 0;

    ret = pkcs1_get_hash(hash_type, NULL, &hash_info);
    drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "pkcs1_get_hash failed, ret is 0x%x\n", ret);

    p = pack->em;
    if (p[idx++] != 0x00) {
        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }
    if (p[idx++] != 0x01) {
        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* PS */
    while (idx < (pack->em_len - hash_info.hash_len - hash_info.asn1_len - 1)) {
        if (p[idx++] != CRYPTO_BYTE_MAX) {
            drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
            return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
        }
    }
    if (p[idx++] != 0x00) {
        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    /* T */
    if (memcmp(&p[idx], hash_info.asn1_data, hash_info.asn1_len) != 0) {

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    idx += hash_info.asn1_len;
    if (memcmp(&p[idx], pack->hash, hash_info.hash_len) != 0) {

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_VERIFY_V15_CHECK);
    }

    return CRYPTO_SUCCESS;
}

/* EM = 00 || 02 || PS || 00 || M */
int pkcs1_v15_encrypt(const rsa_pkcs1_pack *pack)
{
    int ret;
    unsigned char *p = NULL;
    unsigned int idx = 0;
    unsigned int ps_len = pack->em_len - pack->data_len - RSA_PADLEN_3;

    p = pack->em;
    p[idx++] = 0x00;
    p[idx++] = 0x02; // 0x02 see comment above func

    /* PS */
    ret = inner_get_random_with_nonzero_octets(&p[idx], ps_len);
    if (ret != CRYPTO_SUCCESS) {
        drv_set_err(ERROR_UNEXPECTED, "logged error");
        return ret;
    }
    idx += ps_len;

    p[idx++] = 0x00;
    /* M */
    if (pack->data_len != 0) {
        ret = memcpy_s(&p[idx], pack->em_len - idx, pack->data, pack->data_len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, ret, "memcpy_s failed, ret is 0x%x\n", ret);
    }

    return CRYPTO_SUCCESS;
}

/* check EM = 00 || 02 || PS || 00 || M */
int pkcs1_v15_decrypt(rsa_pkcs1_pack *pack)
{
    int ret;
    unsigned char *p = NULL;
    unsigned int idx = 0;
    unsigned int len = 0;
    /* Hint-1.1 Here pack->em_len >= 128 guaranteed by
     * drv_cipher_pke_rsa_private_decrypt->pke_rsa_decrypt->rsa_decrypt->pkcs1_v15_decrypt. */

    p = pack->em;
    if (p[idx++] != 0x00) {

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    if (p[idx++] != 0x02) { // 0x02 see comment above func

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    /* PS */
    while ((idx < (pack->em_len - RSA_PADLEN_1)) && p[idx] != 0x00) {
        idx++;
    }

    if (p[idx++] != 0x00) {

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    len = pack->em_len - idx;
    /* Result-1.1 According to Hint-1.1, pack->em_len > RSA_PADLEN_11(11),
     * so expression 'pack->em_len - RSA_PADLEN_11' will never reverse. */
    if (len > pack->em_len - RSA_PADLEN_11) {

        drv_set_err(PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK), "direct error return");
        return PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_V15_CHECK);
    }

    /* M */
    if (len != 0) {
        ret = memcpy_s(pack->data, pack->data_len, &p[idx], len);
        drv_err_chk_return(ret != CRYPTO_SUCCESS, PKE_COMPAT_ERRNO(ERROR_PKE_RSA_CRYPTO_BUFFER_TOO_SMALL),
            "memcpy_s failed, ret is 0x%x\n", ret);
    }
    pack->data_len = len; // record length of plain text for return

    return CRYPTO_SUCCESS;
}