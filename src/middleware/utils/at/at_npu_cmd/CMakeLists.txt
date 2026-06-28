#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if(NOT "NPU_AT_USE_IPC" IN_LIST DEFINES)
if(NOT "CONFIG_NPU_LIB" IN_LIST DEFINES)
if(NOT "CONFIG_NPU_DEBUG" IN_LIST DEFINES)
    return()
endif()
endif()
endif()

set(COMPONENT_NAME "npu_at")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/at/at_npu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/at_npu_cmd_register.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/at
    ${ROOT_DIR}/middleware/utils/at/at
    ${ROOT_DIR}/middleware/utils/at/at/include
    ${ROOT_DIR}/middleware/utils/at/at/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    AT_COMMAND_NPU
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