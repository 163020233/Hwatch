#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================


set(COMPONENT_NAME "net_at_sw22")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_net_sw22_cmd_register.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_net_sw22.c
    ${CMAKE_CURRENT_SOURCE_DIR}/at/net_at_sw22_process.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/at
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/middleware/services/media/foundation/sample/common/include
    ${ROOT_DIR}/open_source/lwip/lwip_v2.2.0/src/include
    ${ROOT_DIR}/open_source/lwip/lwip_adapter/sw22/src/include/public
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_NET_SW22
    AT_COMMAND_HTTP
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
    true
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/at "*.h")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/src "*.h")
