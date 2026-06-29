#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_can")

if(DEFINED CONFIG_CAN_USE_V2_DRIVER)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/v2/hal_can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v2/hal_can_fd_v330.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v2/hal_can_fd_v330_regs_op.c
)
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/v1/hal_can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v1/hal_can_fd_v330.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v1/hal_can_fd_v330_regs_op.c
)
endif()

set(PRIVATE_HEADER
)

if(DEFINED CONFIG_CAN_USE_V2_DRIVER)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v2
)
else()
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/v1
)
endif()


set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()