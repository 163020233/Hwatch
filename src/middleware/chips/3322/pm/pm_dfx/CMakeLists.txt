#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_dfx_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_dfx_porting.c
)

set(WHOLE_LINK
    true
)

build_component()
