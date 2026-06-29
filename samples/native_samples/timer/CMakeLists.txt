#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "timer_sample")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/timer_demo.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/src/include/driver
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_DITING_EXAMPLE_TIMER
)

set(COMPONENT_CCFLAGS
    -Wincompatible-function-pointer-types
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/protocol/wifi/source/host/main/liteOS
    ${ROOT_DIR}/kernel/alios/alios_v3.3.0/components/debug/src
)

build_component()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*.h")