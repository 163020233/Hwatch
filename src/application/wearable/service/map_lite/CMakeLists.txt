#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "map_lite")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer_cache_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lite_task_runner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map_controller_proxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map_render_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/map_utils.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
)

set(PRIVATE_HEADER
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-error=unused-function
    -std=c++11
)

set(WHOLE_LINK
    true
)

build_component()
