#===============================================================================
# @brief    ux_power service cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_jack")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_jack.c
)
 
set(PUBLIC_HEADER
    
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${UX_INTERNAL_HEADERS}
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
 
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "ux_power.h")