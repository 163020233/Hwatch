#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "amap_controller")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/awk_map_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gralloc_c_api.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/application/wearable/service/amap_service/amap_os_adapter
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
