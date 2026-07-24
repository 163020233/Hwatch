/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: driver file id definitions for security_unified module.
 */
#ifndef DRV_FILE_ID_H
#define DRV_FILE_ID_H

/* driver layer file IDs (1-64) */
#define FILE_ID_DRV_CODE_FAPC_DRV_FAPC                                           1
#define FILE_ID_DRV_CODE_HASH_DRV_HASH_COMPUTE                                   2
#define FILE_ID_DRV_CODE_HASH_DRV_HKDF                                           3
#define FILE_ID_DRV_CODE_HASH_DRV_HMAC_COMPUTE                                   4
#define FILE_ID_DRV_CODE_HASH_DRV_PBKDF2_SOFT                                    5
#define FILE_ID_DRV_CODE_KM_DRV_KEYSLOT                                          6
#define FILE_ID_DRV_CODE_KM_DRV_PBKDF2                                           7
#define FILE_ID_DRV_CODE_PKE_BN_DRV_PKE_CALC                                     8
#define FILE_ID_DRV_CODE_PKE_BN_DRV_PKE_EXP_MOD                                  9
#define FILE_ID_DRV_CODE_PKE_DRV_PKE_API                                         10
#define FILE_ID_DRV_CODE_PKE_DRV_PKE_DH                                          11
#define FILE_ID_DRV_CODE_PKE_DRV_PKE_PRIV                                        12
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_CURVE_ECDH                              13
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_CURVE_GEN_KEY                           14
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_CHECK_DOT_ON_CURVE                15
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_ECDH                              16
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_GEN_KEY                           17
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_SIGN                              18
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ECDSA_VERIFY                            19
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_ED_GEN_KEY                              20
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_EDDSA_SIGN                              21
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_EDDSA_VERIFY                            22
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_DSA_HASH                            23
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_PRIV_DEC                            24
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_PUB_ENC                             25
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_SIGN                                26
#define FILE_ID_DRV_CODE_PKE_ECC_DRV_PKE_SM2_VERIFY                              27
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_PRIV_DEC                            28
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_PUB_ENC                             29
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_SIGN                                30
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_V15                                 31
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_V21                                 32
#define FILE_ID_DRV_CODE_PKE_RSA_DRV_PKE_RSA_VERIFY                              33
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC                                           34
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_CCM                                       35
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_CRC                                       36
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_DMA_COPY                                  37
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_GCM                                       38
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_GHASH                                     39
#define FILE_ID_DRV_CODE_SYMC_DRV_SYMC_MAC                                       40
#define FILE_ID_DRV_CODE_TRNG_DRV_TRNG                                           41
#define FILE_ID_SERVICE_LAYER_CIPHER_SIMPLE                                      42
#define FILE_ID_SERVICE_LAYER_CRC_SIMPLE                                         43
#define FILE_ID_SERVICE_LAYER_HKDF_SIMPLE                                        44
#define FILE_ID_SERVICE_LAYER_HMAC_SHA1_SIMPLE                                   45
#define FILE_ID_SERVICE_LAYER_HMAC_SHA224_SIMPLE                                 46
#define FILE_ID_SERVICE_LAYER_HMAC_SHA256_SIMPLE                                 47
#define FILE_ID_SERVICE_LAYER_HMAC_SHA384_SIMPLE                                 48
#define FILE_ID_SERVICE_LAYER_HMAC_SHA512_SIMPLE                                 49
#define FILE_ID_SERVICE_LAYER_HMAC_SM3_SIMPLE                                    50
#define FILE_ID_SERVICE_LAYER_IPC_ECDH_CLAC                                      51
#define FILE_ID_SERVICE_LAYER_KM_SIMPLE                                          52
#define FILE_ID_SERVICE_LAYER_MAC_SIMPLE                                         53
#define FILE_ID_SERVICE_LAYER_PKE                                                54
#define FILE_ID_SERVICE_LAYER_SECURITY_INIT                                      55
#define FILE_ID_SERVICE_LAYER_SECURITY_OLD_API                                   56
#define FILE_ID_SERVICE_LAYER_SHA1_SIMPLE                                        57
#define FILE_ID_SERVICE_LAYER_SHA224_SIMPLE                                      58
#define FILE_ID_SERVICE_LAYER_SHA256_SIMPLE                                      59
#define FILE_ID_SERVICE_LAYER_SHA384_SIMPLE                                      60
#define FILE_ID_SERVICE_LAYER_SHA512_SIMPLE                                      61
#define FILE_ID_SERVICE_LAYER_SL_COMMON                                          62
#define FILE_ID_SERVICE_LAYER_SM3_SIMPLE                                         63
#define FILE_ID_SERVICE_LAYER_TRNG                                               64

/* HAL layer file IDs (65-125) */
#define FILE_ID_COMMON_CRYPTO_DRV_COMMON                                         65
#define FILE_ID_COMMON_CRYPTO_HASH_COMMON                                        66
#define FILE_ID_COMMON_CRYPTO_PKE_COMMON                                         67
#define FILE_ID_COMMON_CRYPTO_REGISTER                                           68
#define FILE_ID_COMMON_CURVE_PARAM_BRAINPOOL_256R1                               69
#define FILE_ID_COMMON_CURVE_PARAM_BRAINPOOL_384R1                               70
#define FILE_ID_COMMON_CURVE_PARAM_BRAINPOOL_512R1                               71
#define FILE_ID_COMMON_CURVE_PARAM_CURVE25519                                    72
#define FILE_ID_COMMON_CURVE_PARAM_CURVE448                                      73
#define FILE_ID_COMMON_CURVE_PARAM_ED25519                                       74
#define FILE_ID_COMMON_CURVE_PARAM_NIST_P192R                                    75
#define FILE_ID_COMMON_CURVE_PARAM_NIST_P224R                                    76
#define FILE_ID_COMMON_CURVE_PARAM_NIST_P256R                                    77
#define FILE_ID_COMMON_CURVE_PARAM_NIST_P384R                                    78
#define FILE_ID_COMMON_CURVE_PARAM_NIST_P521R                                    79
#define FILE_ID_COMMON_CURVE_PARAM_SM2                                           80
#define FILE_ID_COMMON_ECC_CURVE_PARAM                                           81
#define FILE_ID_CRYPTO_OSAL_BOOT_CRYPTO_OSAL_LIB                                 82
#define FILE_ID_CRYPTO_OSAL_LITEOS_CRYPTO_OSAL_LIB                               83
#define FILE_ID_CRYPTO_OSAL_SELITEOS_CRYPTO_OSAL_LIB                             84
#define FILE_ID_HAL_CODE_COMMON_HAL_COMMON                                       85
#define FILE_ID_HAL_CODE_FAPC_HAL_FAPC                                           86
#define FILE_ID_HAL_CODE_KM_HAL_KDF_ROMABLE                                      87
#define FILE_ID_HAL_CODE_KM_HAL_KEYSLOT                                          88
#define FILE_ID_HAL_CODE_KM_HAL_KEYSLOT_ROMABLE                                  89
#define FILE_ID_HAL_CODE_PKE_HAL_PKE                                             90
#define FILE_ID_HAL_CODE_PKE_HAL_PKE_ROMABLE                                     91
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_CURVE_POINT_MUL                             92
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_CHECK_DOT_ON_CURVE                      93
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_COMMON                                  94
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_COMPUTE_S                               95
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_COMPUTE_U1_AND_U2                       96
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_COMPUTE_X                               97
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_INNER                                   98
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_INV_MOD                                 99
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_MUL_NAF_INNER                           100
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ECC_POINT_MUL                               101
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_COMPUTE_S                                102
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_MUL_SUB                                  103
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_POINT_DECOMPRESS                         104
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_POINT_MUL_COMPRESS                       105
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_ED_SHA512_MODQ                              106
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_PKE_ALG_INNER                               107
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_POINT_MUL_OP                                108
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_RSA_COMMON_MOD                              109
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_RSA_EXP_MOD                                 110
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_RSA_INNER                                   111
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_SM2_ADD_MOD                                 112
#define FILE_ID_HAL_CODE_PKE_PKE_ALG_SM2_COMPUTE_S                               113
#define FILE_ID_HAL_CODE_PKE_ROM_LIB                                             114
#define FILE_ID_HAL_CODE_PKE_ENHANCE_HAL_PKE_SEC                                 115
#define FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_ECC_COMMON_SEC                      116
#define FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_ECC_COMPUTE_S_SEC                   117
#define FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_ECC_INV_MOD_SEC                     118
#define FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_ECC_POINT_MUL_SEC                   119
#define FILE_ID_HAL_CODE_PKE_ENHANCE_PKE_ALG_PKE_ALG_INNER_SEC                   120
#define FILE_ID_HAL_CODE_SPACC_HAL_HASH                                          121
#define FILE_ID_HAL_CODE_SPACC_HAL_HASH_ROMABLE                                  122
#define FILE_ID_HAL_CODE_SPACC_HAL_SYMC                                          123
#define FILE_ID_HAL_CODE_SPACC_HAL_SYMC_ROMABLE                                  124
#define FILE_ID_HAL_CODE_TRNG_HAL_TRNG_ROMABLE                                   125

#endif
