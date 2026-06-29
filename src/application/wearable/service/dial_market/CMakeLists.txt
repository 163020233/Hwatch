#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "dial_market")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dial_market_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dial_market.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
)

set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()


