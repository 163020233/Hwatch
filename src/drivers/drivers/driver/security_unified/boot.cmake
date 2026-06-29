#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha256_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hash_compute.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/fapc/drv_fapc.c

    # km
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_keyslot.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km_simple.c
)

set(PUBLIC_DEFINES
    # need to change to related to kconfig.
    -DCRYPTO_ERROR_ENV=6    # NOOS
    -DPKE_SM2_SUPPORT
    -DCONFIG_HASH_SUPPORT_SHA256
    -DCONFIG_PKE_ECC_SUPPORT
    -DCONFIG_PKE_ECC_ECDSA_VERIFY_SUPPORT
)

# ROM API
if(DEFINED CONFIG_FLASH_BOOT_SUPPORT_ROM_API)
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_dsa_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_sm2_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_priv.c
    )
endif()