#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "native_samples")

set(SOURCES
)

set(PUBLIC_HEADER ${CMAKE_SOURCE_DIR}/middleware/utils/app_init
)
if("CONFIG_ENABLE_GPIO_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(gpio)
endif()
if("CONFIG_ENABLE_PWM_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(pwm)
endif()
if("CONFIG_ENABLE_TIMER_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(timer)
endif()
if("CONFIG_ENABLE_WATCHDOG_SAMPLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(watchdog)
endif()
if("CONFIG_VOICE_CLOUD_LLM_ENABLE" IN_LIST DEFINES)
    add_subdirectory_if_exist(voice_cloud_LLM)
endif()

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