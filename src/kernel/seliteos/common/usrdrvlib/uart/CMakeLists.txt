#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "usrdrv_uart")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/udrv_uart.c

    ${ROOT_DIR}/kernel/seliteos/kernel/base/print/los_printf.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/include
    ${ROOT_DIR}/kernel/seliteos/kernel/base/print
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/seliteos/drivers/uart
)

set(PRIVATE_DEFINES
    -DLOSCFG_BOX    # support syscall
    -DSYSCALLS_H
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
