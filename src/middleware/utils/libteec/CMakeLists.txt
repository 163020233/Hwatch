#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "teec_component")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ree_tee_com.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ree_tee_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/remote_call_func_table.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/drivers/boards/${CHIP}_${BOARD}/linker/seliteos
    ${ROOT_DIR}/drivers/chips/${CHIP}/board/linker/seliteos
)

if(NOT EXISTS ${ROOT_DIR}/kernel/seliteos/target/${CHIP}/ioctl/chips_porting_ioctl.h)
    list(APPEND PUBLIC_HEADER ${ROOT_DIR}/kernel/seliteos/target/common/ioctl)
else()
    list(APPEND PUBLIC_HEADER ${ROOT_DIR}/kernel/seliteos/target/${CHIP}/ioctl)
endif()

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/base/communication
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
add_subdirectory_if_exist(libteec)
