#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/entropy_poll_alt.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if("mbedtls_v3.6.0" IN_LIST RAM_COMPONENT)
    set(MBEDTLS_VERSION "mbedtls_v3.6.0")
endif()

if("mbedtls_v3.6.5" IN_LIST RAM_COMPONENT)
    set(MBEDTLS_VERSION "mbedtls_v3.6.5")
endif()

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/mbedtls/${MBEDTLS_VERSION}/library
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    MBEDTLS_CONFIG_FILE="${MBEDTLS_VERSION}_config_3322.h"
    MBEDTLS_USER_CONFIG_FILE="${MBEDTLS_VERSION}_user_config_3322.h"
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

build_component()