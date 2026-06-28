#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "auto_ota_msg")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/middleware/utils/
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/auto_ota_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/msg_center_ota.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-writable-strings
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


