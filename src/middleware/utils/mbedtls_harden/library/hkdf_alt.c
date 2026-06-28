/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls alt hkdf implemention.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_HKDF_C) && defined(MBEDTLS_HKDF_ALT)
#include "entropy_poll.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/private_access.h"
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/platform.h"
#include "security_kdf.h"
#include "mbedtls_hash_func.h"
#include "mbedtls/error.h"

int mbedtls_hkdf(const mbedtls_md_info_t *md, const unsigned char *salt,
                 size_t salt_len, const unsigned char *ikm, size_t ikm_len,
                 const unsigned char *info, size_t info_len,
                 unsigned char *okm, size_t okm_len)
{
    uapi_drv_cipher_hkdf_t hkdf_params;
    errcode_t ret;

    if (md == NULL) {
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_hash_info(md->type, &hkdf_params.hmac_type);
    hkdf_params.salt = (unsigned char *)salt;
    hkdf_params.salt_length = salt_len;
    hkdf_params.ikm = (unsigned char *)ikm;
    hkdf_params.ikm_length = ikm_len;
    hkdf_params.info = (unsigned char *)info;
    hkdf_params.info_length = info_len;

    ret = uapi_drv_cipher_hkdf(&hkdf_params, okm, okm_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_hkdf failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    return 0;
}
int mbedtls_hkdf_extract(const mbedtls_md_info_t *md,
                         const unsigned char *salt, size_t salt_len,
                         const unsigned char *ikm, size_t ikm_len,
                         unsigned char *prk)
{
    uapi_drv_cipher_hkdf_extract_t hkdf_extract_params;
    uint32_t prk_len;
    errcode_t ret;
    size_t hash_len;

    if (md == NULL) {
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    hash_len = mbedtls_md_get_size(md);
    if (hash_len == 0) {
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_hash_info(md->type, &hkdf_extract_params.hmac_type);
    hkdf_extract_params.ikm = (unsigned char *)ikm;
    hkdf_extract_params.ikm_length = ikm_len;
    hkdf_extract_params.salt = (unsigned char *)salt;
    hkdf_extract_params.salt_length = salt_len;

    ret = uapi_drv_cipher_hkdf_extract(&hkdf_extract_params, prk, &prk_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_hkdf_extract failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    return 0;
}

int mbedtls_hkdf_expand(const mbedtls_md_info_t *md, const unsigned char *prk,
                        size_t prk_len, const unsigned char *info,
                        size_t info_len, unsigned char *okm, size_t okm_len)
{
    size_t hash_len;
    size_t n;
    uapi_drv_cipher_hkdf_expand_t hkdf_expand_params;
    errcode_t ret;

    if (md == NULL) {
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    hash_len = mbedtls_md_get_size(md);
    if (prk_len < hash_len || hash_len == 0) {
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_hash_info(md->type, &hkdf_expand_params.hmac_type);

    n = okm_len / hash_len;

    if (okm_len % hash_len != 0) {
        n++;
    }

    /*
     * Per RFC 5869 Section 2.3, okm_len must not exceed
     * 255 times the hash length
    */
    if (n > 255) { // 255: refer above
        return MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
    }

    hkdf_expand_params.info = (unsigned char *)info;
    hkdf_expand_params.info_length = info_len;
    hkdf_expand_params.prk = (unsigned char *)prk;
    hkdf_expand_params.prk_length = prk_len;

    ret = uapi_drv_cipher_hkdf_expand(&hkdf_expand_params, okm, okm_len);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_hkdf_expand failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    return 0;
}
#endif
