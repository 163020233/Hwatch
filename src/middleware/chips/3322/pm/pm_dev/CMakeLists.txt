#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "pm_dev_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_dev_porting.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/drivers/drivers/driver/lcd/include/
    ${ROOT_DIR}/middleware/services/gpu/drv/include/
    ${ROOT_DIR}/middleware/services/gpu/drv/gmmu/include/
)

set(WHOLE_LINK
    true
)

build_component()
