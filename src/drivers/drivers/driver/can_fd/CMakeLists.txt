#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "can")

if(DEFINED CONFIG_CAN_USE_V2_DRIVER)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/can_v2.c
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/can.c
)
endif()

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()
