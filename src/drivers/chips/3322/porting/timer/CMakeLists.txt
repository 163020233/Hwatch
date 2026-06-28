#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "timer_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/timer_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PUBLIC_DEFINES
    DEFAULT_TIMER=2
    TEST_TIMER_INDEX=1
)

set(COMPONENT_CCFLAGS
    -Wno-incompatible-function-pointer-types
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
