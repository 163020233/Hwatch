#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "drivers_include_3322")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/shared/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/${CORE}/" "*.h")

build_component()
