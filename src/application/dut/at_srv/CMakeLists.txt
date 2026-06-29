#===============================================================================
# @brief    cmake entry
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "at_srv")
set(MODULE_NAME "pf")
set(AUTO_DEF_FILE_ID TRUE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/app_at_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_at_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/app_at_srv.c
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

