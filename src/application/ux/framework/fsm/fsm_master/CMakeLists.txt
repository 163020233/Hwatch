#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "fsm_master_socmn2")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm_master.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_FSM_MASTER
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
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
