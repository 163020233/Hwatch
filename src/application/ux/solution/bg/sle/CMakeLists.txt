#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_bg_sle")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle.c
)

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES  OR "MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_livemic_pairing_service.c
    )
endif()

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES  OR "MELODY_DEMO_HA" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_terminal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_terminal_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_event_term.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_weak.c
    )
endif()

if("MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_terminal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_terminal_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_event_term.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_weak.c
    )
endif()

if("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_grant.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_wrap.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_weak.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_grant_adapt.c
    )
endif()

set(PUBLIC_HEADER
    ${ROOT_DIR}/middleware/chips/${CHIP}/product/live_mic
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/application/ux/solution/bg/database
    ${ROOT_DIR}/protocol/bt/host/include/bs_service
    ${ROOT_DIR}/protocol/bt/host/include/sle/sdm
    ${ROOT_DIR}/protocol/bt/host/include/sle/sle_service
    ${ROOT_DIR}/protocol/bt/host/include/sle/sle_audio
)

set(PRIVATE_DEFINES
    CONFIG_SUPPORT_SLE_BASED_BT
)

set(PUBLIC_DEFINES
    SUPPORT_UX_AT_CMD
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
