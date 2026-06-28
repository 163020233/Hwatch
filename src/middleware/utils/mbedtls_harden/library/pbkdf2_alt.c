/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
*/
#include "library/common.h"

#if defined(MBEDTLS_PKCS5_C) && defined(MBEDTLS_PBKDF2_HMAC_ALT)
#include "mbedtls/private_access.h"
#include "mbedtls/build_info.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/platform.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/error.h"
#include "mbedtls_hash_func.h"

#include "security_kdf.h"

#if MBEDTLS_VERSION_NUMBER == 0x03060500
int mbedtls_pkcs5_pbkdf2_hmac_ext(mbedtls_md_type_t md_alg,
                                  const unsigned char *password,
                                  size_t plen, const unsigned char *salt, size_t slen,
                                  unsigned int iteration_count,
                                  uint32_t key_length, unsigned char *output)
{
    uapi_drv_cipher_kdf_pbkdf2_param_t pbkdf2_params;
    errcode_t ret;

    mbedtls_alt_get_hash_info(md_alg, &pbkdf2_params.hash_type);
    if (pbkdf2_params.hash_type == UAPI_DRV_CIPHER_HASH_TYPE_INVALID) {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if (iteration_count > 0XFFFF) { // pbkdf2_params.count's type is uint16_t
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }
    pbkdf2_params.count = iteration_count;
    pbkdf2_params.password = (unsigned char *)password;
    pbkdf2_params.plen = plen;
    pbkdf2_params.salt = (unsigned char *)salt;
    pbkdf2_params.slen = slen;

    ret = uapi_drv_cipher_pbkdf2(&pbkdf2_params, output, key_length);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_pkcs5_pbkdf2_hmac_ext failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    return 0;
}
#endif

int mbedtls_pkcs5_pbkdf2_hmac(mbedtls_md_context_t *ctx, const unsigned char *password, size_t plen,
    const unsigned char *salt, size_t slen, unsigned int iteration_count,
    uint32_t key_length, unsigned char *output)
{
    uapi_drv_cipher_kdf_pbkdf2_param_t pbkdf2_params;
    errcode_t ret;

    if (ctx == NULL) {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if (ctx->md_info == NULL) {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    mbedtls_alt_get_hash_info(ctx->md_info->type, &pbkdf2_params.hash_type);
    if (pbkdf2_params.hash_type == UAPI_DRV_CIPHER_HASH_TYPE_INVALID) {
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }

    if (iteration_count > 0XFFFF) { // pbkdf2_params.count's type is uint16_t
        return MBEDTLS_ERR_MD_BAD_INPUT_DATA;
    }
    pbkdf2_params.count = iteration_count;
    pbkdf2_params.password = (unsigned char *)password;
    pbkdf2_params.plen = plen;
    pbkdf2_params.salt = (unsigned char *)salt;
    pbkdf2_params.slen = slen;

    ret = uapi_drv_cipher_pbkdf2(&pbkdf2_params, output, key_length);
    if (ret != ERRCODE_SUCC) {
        mbedtls_printf("mbedtls_pkcs5_pbkdf2_hmac_ext failed, ret=0x%x\n", ret);
        return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
    return 0;
}

#endif