#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "nv_porting")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_crypto.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nv_file.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/middleware/chips/${CHIP}/nv/nv_config/${NV_CFG}/include
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
    false
)
build_component()
