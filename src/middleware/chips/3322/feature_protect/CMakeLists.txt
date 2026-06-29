#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "feature_protect")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/feature_protect.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
true
)

build_component()
