#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mbedtls_v3.6.0")

set(SOURCES)

aux_source_directory(${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library SOURCES)

# only unix and Windows support net_sockets, liteos should adapt lwip in porting.
list(FILTER SOURCES EXCLUDE REGEX "net_sockets.c")

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0
)

set(PUBLIC_DEFINES
)

set(PRIVATE_HEADER
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

if ("lwip" IN_LIST RAM_COMPONENT)
	list(APPEND PRIVATE_DEFINES
        WS_IOT_LWIP_C
    )
	list(APPEND COMPONENT_CCFLAGS
        -Wno-error=switch-default
        -Wno-error=unused-parameter
        -Wno-error=unused-variable
        -Wno-error=strict-prototypes
        -Wno-error=sign-compare
        -Wno-error=return-type
        -Wno-error=unused-value
        -Wno-error=comment
        -Wno-error=pointer-sign
        -Wno-error=attributes
        -Wno-error=uninitialized
        -Wno-error=unused-function
        -Wno-error=format
        -Wno-error=incompatible-pointer-types
        -Wno-macro-redefined
        -include ${ROOT_DIR}/drivers/chips/${CHIP}/porting/mbedtls/lwip_adapt.h
        -Wno-error
    )
	list(APPEND SOURCES
		${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/net_sockets.c
	)
endif()

build_component()
