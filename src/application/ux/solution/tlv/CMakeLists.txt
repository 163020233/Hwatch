#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "tlv")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tlv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/conn_tlv_api.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/solution/bg/spp
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -fstack-protector-strong
    -Werror=all
    -Wconversion
    -Werror=incompatible-pointer-types
    -Werror=unused-parameter
    -Werror=enum-conversion
    -Werror=sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "conn_tlv_err_code.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "conn_tlv_api.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "conn_tlv_spec.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "tlv.h")

build_component()
