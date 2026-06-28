#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_ai_engine_common_lib")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/path_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fp16_convert.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${ROOT_DIR}/include/middleware/services/ai
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
    -Wno-strict-prototypes
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/include/ "*.h")
build_component()

