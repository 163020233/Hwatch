#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_rtc_unified_rom")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_rtc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_rtc_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_rtc_v150_op.c
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

build_component()
