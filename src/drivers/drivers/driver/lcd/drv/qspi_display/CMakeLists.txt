#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "qspi_display")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi_display_dfx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi_display_hal.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../hal/dma
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Werror=out-of-line-declaration"
    "-Werror=uninitialized"
    "-Werror=int-conversion"
    "-Werror=enum-conversion"
    "-Werror=long-long"
    "-Werror=unused-variable"
    "-Werror=unused-value"
    "-Werror=unused-function"
    "-Werror=unused-but-set-variable"
    "-Werror=sign-compare"
    "-Werror=unused-parameter"
    "-Werror=tautological-compare"
    "-Werror=bad-function-cast"
    "-Werror=incompatible-pointer-types"
    "-Werror=implicit-function-declaration"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
