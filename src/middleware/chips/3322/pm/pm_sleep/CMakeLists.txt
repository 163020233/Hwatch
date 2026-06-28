#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_sleep_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}/pm_sleep_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}/pm_cpu_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}/lowpower.S
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

build_component()
