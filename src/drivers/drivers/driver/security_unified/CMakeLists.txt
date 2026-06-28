#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "drv_security_unified")

if ((("flashboot" IN_LIST APPLICATION) OR ("ssb" IN_LIST APPLICATION)) OR ("loaderboot" IN_LIST APPLICATION))
    set(CRYPTO_BUILD_ENV "boot")
    if("CONFIG_SECURITY_CMAKE_REPLACE" IN_LIST DEFINES)
        include(${ROOT_DIR}/drivers/chips/${CHIP}/porting/security_unified/drv_boot.cmake)
    else()
        include(${CMAKE_CURRENT_SOURCE_DIR}/boot.cmake)
    endif()
elseif("seliteos" IN_LIST APPLICATION)
    set(CRYPTO_BUILD_ENV "seliteos")
    if("CONFIG_SECURITY_CMAKE_REPLACE" IN_LIST DEFINES)
        include(${ROOT_DIR}/drivers/chips/${CHIP}/porting/security_unified/drv_seliteos.cmake)
    else()
        include(${CMAKE_CURRENT_SOURCE_DIR}/seliteos.cmake)
    endif()
elseif(("BUILD_APPLICATION_STANDARD" IN_LIST DEFINES) OR ("TEST_SUITE" IN_LIST DEFINES))
    set(CRYPTO_BUILD_ENV "liteos")
    if("CONFIG_SECURITY_CMAKE_REPLACE" IN_LIST DEFINES)
        include(${ROOT_DIR}/drivers/chips/${CHIP}/porting/security_unified/drv_os.cmake)
    else()
        include(${CMAKE_CURRENT_SOURCE_DIR}/os.cmake)
    endif()
elseif(("UT_TEST" IN_LIST DEFINES) OR ("FUZZ_TEST" IN_LIST DEFINES))
    set(CRYPTO_BUILD_ENV "ut")
    include(${CMAKE_CURRENT_SOURCE_DIR}/ut.cmake)
endif()

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb
    ${CMAKE_CURRENT_SOURCE_DIR}/romable
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/pke
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code
    ${CMAKE_CURRENT_SOURCE_DIR}/kapi/include
    ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/${CRYPTO_BUILD_ENV}
    ${ROOT_DIR}/drivers/drivers/hal/security_unified/crypto_osal/${CRYPTO_BUILD_ENV}
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/drv_include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/priv_include
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer

    # fix testsuite_brandy_app_yaffs compile error, need modify config.py later.
    ${ROOT_DIR}/include/driver/security_unified

    # fix sdk_ws63_haier compile error for wpa-supplicant.
    ${CMAKE_CURRENT_SOURCE_DIR}/export_header
)

if("seliteos" IN_LIST APPLICATION)
    list(APPEND PRIVATE_HEADER "${ROOT_DIR}/kernel/seliteos/kernel/include")
    list(APPEND PRIVATE_DEFINES
        -DLOSCFG_BOX    # support syscall
        -DSYSCALLS_H
        -DLOSCFG_ARCH_RISCV32
    )
endif()


# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Werror=cast-align
    ${LOCAL_CCFLAGS}
)

# check whether it's the  GCC compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    cmake_policy(PUSH)
    cmake_policy(SET CMP0057 NEW) # allow the use of CMAKE_CXX_COMPILER_VERSION
    # check whether the gcc version >= 9.0
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0)
        message(STATUS "GCC version is 9.0 or higher, enabling -Werror=address-of-packed-member")
        list(APPEND COMPONENT_CCFLAGS -Werror=address-of-packed-member)
    endif()
    # check whether the gcc version >= 11.1
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11.1)
        message(STATUS "GCC version is 11.1 or higher, enabling -Werror=array-parameter")
        list(APPEND COMPONENT_CCFLAGS -Werror=array-parameter)
    endif()
    cmake_policy(POP)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

list(FILTER SOURCES EXCLUDE REGEX "drv_fapc.c")
list(FILTER SOURCES EXCLUDE REGEX "service_layer/cipher.c")
list(FILTER SOURCES EXCLUDE REGEX "service_layer/km.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_pbkdf2_hard.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_pbkdf2.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_hkdf.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_hash_simple.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_hash.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_pke_cal.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_pke_ecc_cal.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_pke.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_symc.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_trng.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_init.c")
list(FILTER SOURCES EXCLUDE REGEX "kapi_km.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_hash.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_trng.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_mac_hard.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/crypto_drv_common.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_km/drv_keyslot.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_km/drv_klad.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_ccm.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_crc.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_gcm.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_ghash.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc_mac.c")
list(FILTER SOURCES EXCLUDE REGEX "drv_cipher/drv_code/drv_symc.c")
list(FILTER SOURCES EXCLUDE REGEX "rom_drv_api/hal_rom_table.c")
list(FILTER SOURCES EXCLUDE REGEX "romable/hash_romable.c")
list(FILTER SOURCES EXCLUDE REGEX "crypto_osal_lib.c")

list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_pbkdf2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/km/drv_keyslot.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/trng/drv_trng.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_ccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_crc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_gcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_ghash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_mac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/fapc/drv_fapc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_old_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hkdf_simple.c
)

if("seliteos" IN_LIST APPLICATION)
    list(FILTER SOURCES EXCLUDE REGEX "hkdf_simple.c")
    if(${CONFIG_SUPPORT_TA} STREQUAL "n")
        list(APPEND SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hkdf_simple.c
        )
    endif()
endif()

if("UT_TEST" IN_LIST DEFINES)
    list(FILTER SOURCES EXCLUDE REGEX "security_old_api.c")
endif()

build_component()
