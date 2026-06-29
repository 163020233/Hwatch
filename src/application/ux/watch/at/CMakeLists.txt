#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_3322_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_dut.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_sn.c
)

if("CONFIG_SUPPORT_BR" IN_LIST DEFINES OR "CONFIG_SUPPORT_BLE" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_bt.c
    )
endif()

if("FT_SUPPORT_SN" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_sn.c
    )
endif()

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES OR "CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_music.c
    )
endif()

if("CONFIG_SUPPORT_PBAP" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_pbap.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/watch/bg/gap
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
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

build_component()
