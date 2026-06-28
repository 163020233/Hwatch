#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(SOURCES
    # SYMC
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_ccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_mac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_gcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_ghash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_crc.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hmac_compute.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_dma_copy.c

    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/trng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c

    # simple API
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/mac_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hkdf_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/crc_simple.c

    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha1_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha224_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha256_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha384_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha512_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sm3_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sha1_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sha224_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sha256_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sha384_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sha512_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hmac_sm3_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km_simple.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/bn/drv_pke_calc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/bn/drv_pke_exp_mod.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_curve_ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_curve_gen_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_check_dot_on_curve.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_gen_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_sign.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_dh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_priv.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_keyslot.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_pbkdf2_soft.c
)

set(PUBLIC_DEFINES
# -DCRYPTO_CCM_TRACE_ENABLE   # Debug for ccm trace
# -DCRYPTO_DRV_FUNC_TRACE_ENABLE  # Debug for drv func trace
# -DCONFIG_CRYPTO_PERF_STATISTICS
    -DCONFIG_PKE_SUPPORT_SM2
)

if(DEFINED CONFIG_IPC_CLAC_ECDH)
list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/service_layer/ipc_ecdh_clac.c")
endif()

if(NOT "CONFIG_SECURITY_ROM_API_SUPPORT" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hash_compute.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/trng/drv_trng.c
    )
endif()

# add old km for test only
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_km.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_keyslot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_klad.c
)

list(APPEND LOCAL_CCFLAGS
    -include ${ROOT_DIR}/drivers/chips/3322/porting/security_unified/security_unified_config.h
)