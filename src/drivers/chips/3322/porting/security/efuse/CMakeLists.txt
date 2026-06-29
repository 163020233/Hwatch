#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "efuse_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/efuse_porting.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/base/communication/tesvec/include
)

set(WHOLE_LINK
    true
)

build_component()
