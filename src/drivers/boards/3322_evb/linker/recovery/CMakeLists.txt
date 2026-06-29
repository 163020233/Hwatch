#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_recovery_lds")

if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${CORE}.prelds" CACHE INTERNAL "" FORCE)
set(LDS_HEADER ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/board_config
    ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/memory_config/include
    ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/memory_config/norflash/${WATCH_TYPE}
    ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/memory_config/psram/${WATCH_TYPE}
    ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/product
    ${ROOT_DIR}/drivers/chips/${CHIP}/include/${CORE}
    ${ROOT_DIR}/middleware/utils/common_headers
    CACHE INTERNAL "" FORCE)
build_component()
