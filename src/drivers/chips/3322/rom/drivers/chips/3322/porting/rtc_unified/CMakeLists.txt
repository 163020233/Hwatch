#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "rtc_unified_port_rom")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rtc_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    CONFIG_RTC_USING_V150
    CONFIG_RTC_MAX_NUM=1
    CONFIG_RTC_MAX_RTCS_NUM=8
    CONFIG_RTC_0_WIDTH_64=0
)

list(APPEND PUBLIC_DEFINES CONFIG_RTC_CLOCK_VALUE=32768)

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
