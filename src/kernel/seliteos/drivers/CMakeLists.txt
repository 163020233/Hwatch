#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "drvbox_uart")

get_filename_component(SELITEOS_TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
MESSAGE(STATUS "SELITEOS_TOP_DIR: " ${SELITEOS_TOP_DIR})

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/uart/drv_uart.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/uart
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
