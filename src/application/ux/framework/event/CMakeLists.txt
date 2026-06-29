#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_event_socmn2")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_fwk_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_fwk_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_fwk_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_fwk_sync_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_fwk_sync_event.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_EVENT
    USE_UX_FWK_API
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
