#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ipc_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}/ipc_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/ipc_test.c
)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/${CORE}
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
)
set(PUBLIC_DEFINES
    IPC_NEW
)
set(WHOLE_LINK
    true
)
set(MAIN_COMPONENT
    false
)
build_component()
