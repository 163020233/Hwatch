#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
if(EXISTS ${ROOT_DIR}/drivers/chips/${CHIP}/porting/mbedtls/mbedtls.cmake)
    include(${ROOT_DIR}/drivers/chips/${CHIP}/porting/mbedtls/mbedtls.cmake)
else()
    if(${CHIP} STREQUAL "brandy")
        if(EXISTS ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/harden/build/conncet/mbedtls.cmake)
            include(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/harden/build/conncet/mbedtls.cmake)
        endif()
    else()
        if(EXISTS ${ROOT_DIR}/drivers/drivers/driver/security_unified/mbedtls_harden_adapt/mbedtls_alt/mbedtls.cmake)
            include(${ROOT_DIR}/drivers/drivers/driver/security_unified/mbedtls_harden_adapt/mbedtls_alt/mbedtls.cmake)
        endif()
    endif()
endif()

if(mbedtls IN_LIST TARGET_COMPONENT)
    install_sdk(${ROOT_DIR}/open_source/mbedtls "*")
endif()

include(${CMAKE_DIR}/open_source/mbedtls_v3.6.0.cmake)
include(${CMAKE_DIR}/open_source/mbedtls_v3.6.5.cmake)