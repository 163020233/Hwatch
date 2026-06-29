#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "irmalloc_3322")

if(NOT ${OS} STREQUAL "liteos")
if(NOT ${CHIP} STREQUAL "ws63" AND NOT ${CHIP} STREQUAL "ws53")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/chip_mem/chip_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/chip_mem/irmalloc.c
)
endif()
endif()
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/chip_mem
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../middleware/utils/dfx/panic/public
    ${CMAKE_CURRENT_SOURCE_DIR}/../non_os
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../drivers/drivers/hal/sectors
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../bootloader/bootrom_3322/boot/commonboot/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/common" "irmalloc.h")

build_component()
