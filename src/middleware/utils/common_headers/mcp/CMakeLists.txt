#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "common_headers")
set(MCP_PLAT_NAME "wear")

if(${CHIP} MATCHES "sw22")
set(MCP_CHIP_NAME "1A22")
elseif(${CHIP} MATCHES "3322")
set(MCP_CHIP_NAME "3322")
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/viu
    ${CMAKE_CURRENT_SOURCE_DIR}/external/isp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/isp/alg
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cis
    ${CMAKE_CURRENT_SOURCE_DIR}/external/mipi_rx
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ppu
    ${CMAKE_CURRENT_SOURCE_DIR}/external/base
    ${CMAKE_CURRENT_SOURCE_DIR}/external/pipeline
    ${CMAKE_CURRENT_SOURCE_DIR}/external/fsd
    ${CMAKE_CURRENT_SOURCE_DIR}/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/base
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/base/hal/${MCP_CHIP_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/platform/${MCP_PLAT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/cis
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/mipi_rx
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/ppu
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/isp
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/viu
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/vgs
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/dis
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/fsd
)

set(MCP_COMMON_HEADERS ${PUBLIC_HEADER} PARENT_SCOPE)