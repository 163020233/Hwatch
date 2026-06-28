#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_manager_headers")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/communication/
    ${CMAKE_CURRENT_SOURCE_DIR}/powermgr/
    ${CMAKE_CURRENT_SOURCE_DIR}/devicemgr/
    ${CMAKE_CURRENT_SOURCE_DIR}/sysmgr/
    ${CMAKE_CURRENT_SOURCE_DIR}/inputmgr/
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcemgr/
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/communication/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/devicemgr/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/inputmgr/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/sourcemgr/" "*.h")
build_component()
