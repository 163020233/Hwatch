#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(CRYPTO_BUILD_ENV "seliteos")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/seliteos/crypto_osal_lib.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_dma_copy.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_ccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_mac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_gcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_ghash.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hash_compute.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hmac_compute.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/trng/drv_trng.c

    # pke
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_curve_ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_curve_gen_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_check_dot_on_curve.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_gen_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_sign.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ed_gen_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_eddsa_sign.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_eddsa_verify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_dsa_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_priv_dec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_pub_enc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_sign.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_verify.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_priv_dec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_pub_enc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_sign.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_v15.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_v21.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/rsa/drv_pke_rsa_verify.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/bn/drv_pke_exp_mod.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_dh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_priv.c

    # km
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_keyslot.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hmac_compute.c
)

if(${CONFIG_SUPPORT_TA} STREQUAL "n")
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/trng.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_init.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher_simple.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha256_simple.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sm3_simple.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km_simple.c)
endif()

set(PUBLIC_DEFINES
    # need to change to related to kconfig.
    -DCRYPTO_ERROR_ENV=5    # SELITEOS
    -DCONFIG_CRYPTO_SELITEOS_SUPPORT
    -DCONFIG_DRV_HASH_BUF_LEN=128

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_SYMC
    -DCONFIG_SYMC_SUPPORT_ECB
    -DCONFIG_SYMC_SUPPORT_CBC
    -DCONFIG_SYMC_SUPPORT_CTR
    -DCONFIG_SYMC_SUPPORT_GCM
    -DCONFIG_SYMC_SUPPORT_CCM
    -DCONFIG_SYMC_SUPPORT_CMAC

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_HASH
    -DCONFIG_HASH_SUPPORT_SHA256              # for service_layer
    -DCONFIG_HASH_SUPPORT_HMAC_SHA256         # for service_layer
    -DCONFIG_HASH_SUPPORT_SM3                 # for service_layer
    -DCONFIG_HASH_SUPPORT_HMAC_SM3            # for service_layer

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_TRNG

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_KM
    -DCONFIG_KM_KEYSLOT_SOFT_SUPPORT
    -DCONFIG_HKM_SUPPORT

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_PKE
    -DCONFIG_PKE_ECC_SUPPORT
    -DCONFIG_PKE_SUPPORT_ECC_BP256R       # for ecc curve
    -DCONFIG_PKE_SUPPORT_ECC_FIPS_P256R   # for ecc curve
    -DCONFIG_PKE_SUPPORT_SM2              # for ecc curve
    -DCONFIG_PKE_SUPPORT_SM2_SIGN         #for service_layer
    -DCONFIG_PKE_SUPPORT_SM2_VERIFY       #for service_layer
    -DCONFIG_PKE_ECC_SM2_SIGN_SUPPORT     #for drv_code
    -DCONFIG_PKE_ECC_SM2_VERIFY_SUPPORT   #for drv_code
    -DCONFIG_PKE_SUPPORT_SM2_CRYPTO       #for service_layer
    -DCONFIG_PKE_ECC_SM2_PRIV_DEC_SUPPORT #for drv_code
    -DCONFIG_PKE_ECC_SM2_PUB_ENC_SUPPORT  #for drv_code
    -DCONFIG_PKE_ECC_SM2_DSA_HASH_SUPPORT #for drv_code
    -DCONFIG_PKE_SUPPORT_ECC_GEN_KEY      #for service_layer
    -DCONFIG_PKE_ECC_GEN_KEY_SUPPORT      #for drv_code
    -DCONFIG_PKE_SUPPORT_ECC_ECDH         #for service_layer
    -DCONFIG_PKE_ECC_ECDH_SUPPORT         #for drv_code
    -DCONFIG_PKE_ECC_ECDSA_SIGN_SUPPORT   #for drv_code
    -DCONFIG_PKE_ECC_ECDSA_VERIFY_SUPPORT #for drv_code
    -DCONFIG_PKE_ECC_CHECK_DOT_ON_CURVE_SUPPORT #for drv_code
    -DCONFIG_PKE_RSA_SUPPORT       #for drv_code
    -DCONFIG_PKE_SUPPORT_RSA       #for service_layer
    -DCONFIG_PKE_RSA_PRIV_DEC_SUPPORT
    -DCONFIG_PKE_RSA_PUB_ENC_SUPPORT
    -DCONFIG_PKE_RSA_SIGN_SUPPORT
    -DCONFIG_PKE_RSA_VERIFY_SUPPORT
    -DCONFIG_PKE_RSA_V15_SUPPORT
    -DCONFIG_PKE_RSA_V21_SUPPORT
    -DCONFIG_PKE_RSA_SHA256_SUPPORT
    -DCONFIG_PKE_RSA_SHA384_SUPPORT
    -DCONFIG_PKE_RSA_SHA512_SUPPORT

    -DCONFIG_PKE_SUPPORT_ECC_SIGN

    -DCONFIG_SECURITY_UNIFIED_SUPPORT_PBKDF2
    -DCONFIG_HASH_SUPPORT_HKDF
)

