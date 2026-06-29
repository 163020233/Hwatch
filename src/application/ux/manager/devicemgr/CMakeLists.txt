#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_device_mgr")

if("DEMO_WATCH" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_device_manager_bs.c
    )
else()
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_device_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_device_property.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_api_device.c
    )
endif()

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_logic_manager.c
    )
endif()

if("CONFIG_SUPPORT_BS_SLE_L3" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_device_manager_obj.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
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