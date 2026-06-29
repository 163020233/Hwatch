#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(pm_dev)
add_subdirectory_if_exist(pm_fsm)
add_subdirectory_if_exist(pm_sleep)
add_subdirectory_if_exist(pm_veto)
add_subdirectory_if_exist(pm_dfx)

set(COMPONENT_NAME "pm_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_porting.c
)

set(WHOLE_LINK
    true
)

build_component()

set(COMPONENT_NAME "pm_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_clocks/pm_clocks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_pmu/pm_pmu_porting.c

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
