#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_broadcast")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_broadcast_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_broadcast.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/osal
    ${ROOT_DIR}/kernel/osal/adapt/include
    ${UX_INTERNAL_HEADERS}
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

build_component()