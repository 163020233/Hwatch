#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_watch_bg_activity")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/watch_bg_response_bg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/watch_bg_response_master.c
    ${CMAKE_CURRENT_SOURCE_DIR}/watch_bg_response_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bg_gap_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/watch_bg_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bg_dfx_service.c
)

if("CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bg_hfp_hf_service.c
    )
endif()

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bg_a2dp_sink_service.c
    )
endif()

if("CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bg_a2dp_src_service.c
    )
endif()

if("CONFIG_SUPPORT_AVRCP" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bg_avrcp_service.c
    )
endif()

if("CONFIG_SUPPORT_PBAP" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bg_pbap_service.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/watch/bg/gap
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_AT_CMD
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
    false
)

build_component()
