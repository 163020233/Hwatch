#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(nandflash)

set(COMPONENT_NAME "flash_boot_3322")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/upgrade.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tee_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/version
)

set(PRIVATE_HEADER
    ${CMAKE_SOURCE_DIR}/middleware/utils/update/inner_include
    ${CMAKE_SOURCE_DIR}/middleware/chips/3322/update/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
    -Wno-unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)


build_component()
