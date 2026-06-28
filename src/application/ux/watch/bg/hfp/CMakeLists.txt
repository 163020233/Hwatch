#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_bt_hfp")

set(SOURCES
)

if ("DEMO_WATCH" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hfp_hal/ux_bt_hal_hfp_hf.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/hfp_hal/ux_bt_hal_sco.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_call_hf.c)
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/watch/bg/hfp/include
    ${ROOT_DIR}/application/ux/solution/include
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

set(COMPONENT_CCFLAGS
    -Werror=all
    -Wconversion
    -Werror=incompatible-pointer-types
    -Werror=unused-parameter
    -Werror=enum-conversion
    -Werror=sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
