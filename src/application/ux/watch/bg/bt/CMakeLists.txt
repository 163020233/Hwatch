#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_watch_bg_bt")

return_if_not_build()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_bt_basic_event.c
)

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES OR 
   "CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES OR
   "CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_api_bt_audio.c
    )
endif()

if("CONFIG_SUPPORT_AVRCP" IN_LIST DEFINES OR
   "CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_api_bt_control.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/solution/bg/bt
    ${ROOT_DIR}/application/ux/solution/include
)

set(PRIVATE_DEFINES
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