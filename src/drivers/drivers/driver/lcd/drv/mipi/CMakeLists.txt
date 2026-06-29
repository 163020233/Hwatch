#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mipi_tx")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mipi_tx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mipi_tx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mipi_tx_dfx.c
)

if("PRE_FPGA" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/mipi_tx_fpga.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    MIPI_TX_ERROR
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
