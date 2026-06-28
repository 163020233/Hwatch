#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_spp")

set(SOURCES)

if("CONFIG_SUPPORT_UX_SPP" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_spp_service.c
    )
endif()

if("DEMO_WATCH" IN_LIST DEFINES)
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_spp_tws.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_spp_service.c
    )
endif()

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
)

set(PRIVATE_DEFINES
   RUN_WITHOUT_BOX
)

set(PUBLIC_DEFINES
   CONFIG_SYSTEM_MU0101
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
