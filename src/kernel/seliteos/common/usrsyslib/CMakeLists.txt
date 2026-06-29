#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "seliteos_usr")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ta_communication/ta_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ta_communication/ta_exit.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/kernel/seliteos/kernel/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    -DLOSCFG_BOX
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

add_subdirectory_if_exist(sec_storage_adapter)

build_component()
