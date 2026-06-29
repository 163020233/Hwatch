#===============================================================================
# @brief    cmake make file
# Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd.
#===============================================================================

set(COMPONENT_NAME "log_file")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/log_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_file_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_file_flash.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
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
