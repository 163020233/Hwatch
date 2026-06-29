#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pmu_3322")

if ((("flashboot" IN_LIST APPLICATION) OR ("ssb" IN_LIST APPLICATION)) OR ("loaderboot" IN_LIST APPLICATION))
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_cmu.c
)
elseif("bt_core" IN_LIST CORE)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_ldo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_buck.c
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_buck.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_cmu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_ldo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_perp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_sub_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_sys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_ulp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_err_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pmu_alarm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_pmu_peripheral.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_pmu_cmu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_pmu_ldo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_pmu_interrupt.c
)
endif()

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_CLOCKS
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
