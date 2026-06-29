/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: some functions for mbedtls hash.
 *
*/
#include "mbedtls_hash_func.h"

const md_type_info g_md_type_info_table[] = {
    { MBEDTLS_MD_SHA1,   UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA1,  },
    { MBEDTLS_MD_SHA224, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA224, },
    { MBEDTLS_MD_SHA256, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA256, },
    { MBEDTLS_MD_SHA384, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA384, },
    { MBEDTLS_MD_SHA512, UAPI_DRV_CIPHER_HASH_TYPE_HMAC_SHA512, },
    { MBEDTLS_MD_NONE,   UAPI_DRV_CIPHER_HASH_TYPE_INVALID,    },
};
void mbedtls_alt_get_hash_info(mbedtls_md_type_t md_type, uapi_drv_cipher_hash_type_t *hash_type)
{
    unsigned int i = 0;
    *hash_type = UAPI_DRV_CIPHER_HASH_TYPE_INVALID;
    for (i = 0; i < sizeof(g_md_type_info_table) / sizeof(g_md_type_info_table[0]); i++) {
        if (g_md_type_info_table[i].md_type == md_type) {
            *hash_type = g_md_type_info_table[i].hash_type;
            return;
        }
    }
    return;
}
