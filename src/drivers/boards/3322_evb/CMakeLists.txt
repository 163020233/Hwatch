#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "board_3322")

if(${CORE} STREQUAL "acore")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/memory_config/psram/${WATCH_TYPE}/memory_config.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/board_config
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_config/include
    ${CMAKE_CURRENT_SOURCE_DIR}/product
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_config/psram/${WATCH_TYPE}
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_config/norflash/${WATCH_TYPE}
)

set(WHOLE_LINK
    true
)

build_component()

add_subdirectory(linker)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/memory_config/psram "*")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/memory_config/norflash "*")