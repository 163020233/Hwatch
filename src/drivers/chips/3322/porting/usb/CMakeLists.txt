#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_usb_device")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PUBLIC_DEFINES
)

set(WHOLE_LINK
    true
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
build_component()
