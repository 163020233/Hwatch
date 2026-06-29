#===============================================================================
# @brief    cmake make file
# Copyright (c) 2021 Huawei Technologies Co. Ltd
#===============================================================================
add_subdirectory_if_exist(commonboot)
set(COMPONENT_NAME "loaderboot_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_load/usb_load.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/version
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_load
)

set(PRIVATE_HEADER
 
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
    true
)

build_component()