#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "remote_msg")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include/phonemenu
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/remote_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/phone_contacts.cpp
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
)

build_component()


