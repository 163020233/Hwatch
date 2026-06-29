#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "at_adapter")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at_adapter.c
)

set(PRIVATE_HEADER

)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_ADAPTER
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
