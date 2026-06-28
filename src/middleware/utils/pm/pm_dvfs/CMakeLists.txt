#===============================================================================
# @brief    cmake make file for pm_dvfs
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "pm_dvfs")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_dvfs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_dvfs_governor.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_dvfs.h
)

set(WHOLE_LINK
    true
)

build_component()
