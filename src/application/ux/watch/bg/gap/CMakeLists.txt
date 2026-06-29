#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_bt")

return_if_not_build()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_config.c
)

if("CONFIG_SUPPORT_BR" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_br.c
    )
endif()

if("CONFIG_SUPPORT_BLE" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_ble.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/adapter/bg/watch
    ${ROOT_DIR}/application/ux/watch/common
    ${ROOT_DIR}/application/ux/watch/bg/pbap
    ${ROOT_DIR}/application/ux/manager/include
    ${ROOT_DIR}/application/ux/solution/bg/ble
)

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES OR "CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/watch/audio/a2dp
        ${ROOT_DIR}/application/ux/watch/bg/a2dp
    )
endif()

if("CONFIG_SUPPORT_AVRCP" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/watch/bg/avrcp
    )
endif()

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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}"  "ux_bt_uapi.h")
build_component()
