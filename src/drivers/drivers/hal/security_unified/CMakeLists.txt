#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_security_unified")

set(SOURCES

)

# add common
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/common/curve_param SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/common SOURCES)
# add hal_code/common
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/common SOURCES)
# add hal_code/fapc
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/fapc SOURCES)
# add hal_code/km
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/km SOURCES)
# add hal_code/pke
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/pke/pke_alg SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/pke SOURCES)
# add hal_code/pke_enhance
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/pke_enhance/pke_alg SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/pke_enhance SOURCES)
# add hal_code/spacc
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/spacc SOURCES)
# add hal_code/trng
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/hal_code/trng SOURCES)

set(PUBLIC_HEADER
    # Unreasonable export include, which will be deleted later.
    ${CMAKE_CURRENT_SOURCE_DIR}/export_header
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common_include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common_include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hal_include
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/include/drv_include

    ${CMAKE_CURRENT_SOURCE_DIR}/hal_code/pke/pke_alg

    # Unreasonable dependency, which will be deleted later.
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/drv_code/pke
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    SHELL:-include ${ROOT_DIR}/drivers/drivers/driver/security_unified/include/drv_include/drv_errno.h
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(("flashboot" IN_LIST APPLICATION) OR ("ssb" IN_LIST APPLICATION) OR ("loaderboot" IN_LIST APPLICATION))
    set(CONFIG_BUILD_ENV "boot")
    list(APPEND PRIVATE_DEFINES
        -DCRYPTO_ERROR_ENV=6        # NOOS
    )
elseif("seliteos" IN_LIST APPLICATION)
    set(CONFIG_BUILD_ENV "seliteos")
    list(APPEND PRIVATE_DEFINES
        -DCRYPTO_ERROR_ENV=5        # SELITEOS
    )
elseif("UT_TEST" IN_LIST DEFINES OR "FUZZ_TEST" IN_LIST DEFINES)
    set(CONFIG_BUILD_ENV "test")
    # move config to test.attr later
    list(APPEND PRIVATE_DEFINES
        -DDEBUG_IN_RAM              # PKE ROM IN RAM
        -DCONFIG_HKM_SUPPORT
    )
else()
    set(CONFIG_BUILD_ENV "liteos")
    list(APPEND PRIVATE_DEFINES
        -DCRYPTO_ERROR_ENV=4        # LITEOS
    )
endif()

# crypto_osal
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/${CONFIG_BUILD_ENV}/crypto_osal_lib.c
)

list(APPEND PUBLIC_HEADER
    # Unreasonable export include, chips porting depend on it, which will be deleted later.
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/${CONFIG_BUILD_ENV}
)

# add compile_flags from porting/*.cmake
set(SOURCES_BACKUP ${SOURCES})
set(PUBLIC_DEFINES_BACKUP ${PUBLIC_DEFINES})

set(CONFIG_BUILD_ENV_CMAKE_FILE
    ${ROOT_DIR}/drivers/chips/${CHIP}/porting/security_unified/hal_${CONFIG_BUILD_ENV}.cmake
)

if(EXISTS ${CONFIG_BUILD_ENV_CMAKE_FILE})
    include(${CONFIG_BUILD_ENV_CMAKE_FILE})
endif()

if(${CONFIG_BUILD_ENV} STREQUAL "liteos")
    set(CONFIG_FILE
        ${ROOT_DIR}/drivers/chips/${CHIP}/porting/security_unified/security_unified_config.h
    )
    if(EXISTS ${CONFIG_FILE})
        list(APPEND COMPONENT_CCFLAGS
            -include ${CONFIG_FILE}
        )
    endif()
endif()

set(SOURCES ${SOURCES_BACKUP})
set(PUBLIC_DEFINES ${PUBLIC_DEFINES} ${PUBLIC_DEFINES_BACKUP})
list(APPEND PRIVATE_DEFINES ${LOCAL_DEFINES})

build_component()
