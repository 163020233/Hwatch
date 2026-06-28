#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "intelligent_voice")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_voice_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_app_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_music_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_timer_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_device_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_alarm_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intelligent_transform.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-missing-field-initializers
)

set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
    OHOS_DEBUG
)

build_component()


