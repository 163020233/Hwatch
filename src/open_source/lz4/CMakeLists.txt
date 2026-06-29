#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "freebsd_lz4")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lz4.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
    _STANDALONE
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