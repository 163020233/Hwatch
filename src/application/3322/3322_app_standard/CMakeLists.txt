#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_app_standard")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/chip_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/board_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_pri_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.S
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
    ${CMAKE_SOURCE_DIR}/middleware/chips/3322/update/include
    ${CMAKE_SOURCE_DIR}/middleware/chips/3322/partition/include
    ${CMAKE_SOURCE_DIR}/middleware/utils/nv/nv_storage_app
    ${CMAKE_SOURCE_DIR}/middleware/utils/nv/nv_storage_lib
    ${CMAKE_SOURCE_DIR}/include/middleware/utils
    ${CMAKE_SOURCE_DIR}/drivers/drivers/driver/audio/include
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
true
)

set(BUILD_AS_OBJ
false
)

set(MAIN_COMPONENT
true
)
install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/../build_3322_fpga.py" "../build_3322_fpga.py")
build_component()
