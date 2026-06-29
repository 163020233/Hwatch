#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_veto_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_veto_porting.c
)

set(PUBLIC_DEFINES
    CONFIG_PM_SLEEP_RECORD_USE_COUNT=1
)

set(WHOLE_LINK
    true
)

build_component()
