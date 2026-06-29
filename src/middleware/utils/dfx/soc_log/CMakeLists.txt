#===============================================================================
# @brief    cmake make file
# Copyright (c) 2021 HiSilicon (Shanghai) Technologies Co., Ltd.
#===============================================================================

set(COMPONENT_NAME "soc_log")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/soc_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/soc_log_uart_instance.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include/" "*.h")

build_component()