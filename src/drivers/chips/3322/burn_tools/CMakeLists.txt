#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "burn_tools_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/burn_ctrl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/seboot_channel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data_convertor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ymodem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
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
