#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nonos_3322")

set(PUBLIC_HEADER
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/non_os_utils.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inlcude
)

set(PRIVATE_DEFINES
    CONFIG_LIB_SUPPORT_COMPAT_LINUX
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
