#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_adapter_bg")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter_bt_br.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
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
    -Wconversion
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
