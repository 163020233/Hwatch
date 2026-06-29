#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "calendar_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/calendar_porting.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/drivers/drivers/driver/ipc
)

set(PRIVATE_DEFINES
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
