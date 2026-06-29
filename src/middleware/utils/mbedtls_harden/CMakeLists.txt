#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "mbedtls_harden_ex")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha1_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha256_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/sha512_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/aes_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/pbkdf2_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/bignum_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/hkdf_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ccm_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/gcm_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/cmac_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecp_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecdsa_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/ecdh_alt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/mbedtls_hash_func.c
    ${CMAKE_CURRENT_SOURCE_DIR}/library/mbedtls_ecp_func.c

    ${CMAKE_CURRENT_SOURCE_DIR}/library/mbedtls_perf_utils.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if("mbedtls_v3.6.0" IN_LIST RAM_COMPONENT)
    set(MBEDTLS_VERSION "mbedtls_v3.6.0")
endif()

if("mbedtls_v3.6.5" IN_LIST RAM_COMPONENT)
    set(MBEDTLS_VERSION "mbedtls_v3.6.5")
endif()

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/mbedtls/${MBEDTLS_VERSION}
    ${ROOT_DIR}/open_source/mbedtls/${MBEDTLS_VERSION}/library
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/export_header
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/drv_cipher/drv_include
    ${ROOT_DIR}/drivers/drivers/hal/security_unified/hal_cipher/include/common_include
)
 
set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    MBEDTLS_HARDEN_OPEN
    # MBEDTLS_PERF_UTILS_OPEN
)
 
# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)
 
set(COMPONENT_CCFLAGS
)
 
set(WHOLE_LINK
    true
)
 
set(MAIN_COMPONENT
    false
)

if(("UT_TEST" IN_LIST DEFINES) OR ("FUZZ_TEST" IN_LIST DEFINES))
    list(APPEND PUBLIC_DEFINES
        MBEDTLS_PERF_UTILS_OPEN
    )
endif()

build_component()