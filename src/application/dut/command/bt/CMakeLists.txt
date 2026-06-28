#===============================================================================
# @brief    cmake entry
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

# bt facotry test cmd
set(COMPONENT_NAME "bt_atcmd")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_ft_atcmd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_cali_atcmd.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${ROOT_DIR}/middleware/chips/${CHIP}/product/tws
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
    -Werror=all
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    true
)

build_component()