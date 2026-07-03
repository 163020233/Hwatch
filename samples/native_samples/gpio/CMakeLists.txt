#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "gpio_sample")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gpio.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/src/include/driver
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_DITING_EXAMPLE_GPIO
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