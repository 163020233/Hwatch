#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "sample_component")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/component.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PUBLIC_HEADER
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

set(BUILD_AS_OBJ
false
)

set(APP_TARGETS
"diting-community"
)

set(MAIN_COMPONENT
true
)
build_component()
