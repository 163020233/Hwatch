#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "esim_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_esim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_esim_cmd_register.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/service/esim_msg/
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
    true
)

build_component()