#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "bs_service_include")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/bs_service
    ${CMAKE_CURRENT_SOURCE_DIR}/sle/sle_service
)

build_component()
