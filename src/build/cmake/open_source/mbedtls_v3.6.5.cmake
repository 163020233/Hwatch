#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls_v3.6.5")

set(SOURCES)

aux_source_directory(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/library SOURCES)

# only unix and Windows support net_sockets, liteos should adapt lwip in porting.
list(FILTER SOURCES EXCLUDE REGEX "net_sockets.c")
list(FILTER SOURCES EXCLUDE REGEX "ecp_alt.c")

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/include
)

set(PUBLIC_DEFINES
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.5/library
)

set(PRIVATE_DEFINES
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

if("${ARCH}" STREQUAL "riscv31")
    list (APPEND COMPONENT_CCFLAGS
        -Wno-error=jump-misses-init
    )
endif()
build_component()
