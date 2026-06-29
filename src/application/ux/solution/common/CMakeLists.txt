#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_common")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_config_common.c
)

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_config_tws.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_secure.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_state_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_multiple_device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_state_tws.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/protocol/bt/host/include/sle/sle_service
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

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/  "ux_event_define.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/  "ux_config_common.h")

build_component()
