#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. 
#===============================================================================
set(COMPONENT_NAME "hal_sema")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_sema.c

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
