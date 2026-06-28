#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sha256_simple.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
)

set(PUBLIC_DEFINES

)

if(NOT "CONFIG_SECURITY_ROM_API_SUPPORT" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/ecc/drv_pke_ecdsa_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke/drv_pke_priv.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hash_compute.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_dma_copy.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_keyslot.c
    )
    list(APPEND PUBLIC_DEFINES
        # need to change to related to kconfig.
        -DCRYPTO_ERROR_ENV=6    # NOOS
        -DCONFIG_PKE_ECC_SUPPORT
        -DCONFIG_PKE_ECC_ECDSA_VERIFY_SUPPORT
        -DCONFIG_PKE_SUPPORT_ECC_BP256R
        -DCONFIG_HKM_SUPPORT
    )
endif()