#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "3322_clocks")

if ((("flashboot" IN_LIST APPLICATION) OR ("ssb" IN_LIST APPLICATION)) OR ("loaderboot" IN_LIST APPLICATION))
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clocks_ccrg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_clocks_ccrg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_clocks_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clock_calibration.c
)
elseif("bt_core" IN_LIST CORE)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clock_calibration.c
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clocks_ccrg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clocks_switch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clocks_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clocks_switch_trap.s
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/clock_calibration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_clocks_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/hal_clocks_ccrg.c
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
