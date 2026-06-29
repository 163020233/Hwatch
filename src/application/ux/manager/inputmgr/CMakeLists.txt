#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_input_manager")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_input_manager.c
)

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_input_passthrough.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_AT_CMD
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/" "ux_input_manager.h")

build_component()
