#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "osal_3322")

if(NOT DEFINED OS)
    return()
endif()

if(${OS} STREQUAL "non-os")
    set(src_sub nonos)
endif()

if(${OS} STREQUAL "non-os")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_interrupt.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}
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
