#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(service)

set(COMPONENT_NAME "ux_protocol")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_api_protocol.c
)

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/service/query_dev/query_dev_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/query_dev/query_dev_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/set_dev/set_dev_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/set_dev/set_dev_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/bt_dev/bt_dev_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/bt_dev/bt_dev_info.c
    )
endif()

if("MELODY_DEMO_HA" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/service/haid_dev/haid_dev_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/haid_dev/haid_dev_info.c
    )
endif()

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/service/mesh_dev/mesh_dev_handle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service/mesh_dev/mesh_dev_info.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/service/mesh_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/service/query_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/service/set_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/service/bt_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/service/haid_dev
    ${ROOT_DIR}/application/ux/solution/bg/spp
    ${ROOT_DIR}/application/ux/solution/include
    ${ROOT_DIR}/include/middleware/services/bts/ble
    ${ROOT_DIR}/include/middleware/services/bts/common
    ${ROOT_DIR}/application/dut/command/include
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/utils
    ${ROOT_DIR}/application/ux/solution/haid/media
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_PROTOCOL
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/service" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/service/mesh_dev" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/service/query_dev" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/service/set_dev" "*.h")
build_component()
