#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_cmd")

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_bs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_audio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_mic_rx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_mic_tx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_system.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_usb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_recovery.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_utils.c
)
else()
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_bs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_bt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_manager.c
)
endif()
if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_tws.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_bt.c
    )
endif()

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_RX" IN_LIST DEFINES 
    OR "MELODY_DEMO_BMIC_TX" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
        list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_power.c
    )
endif()

if("MELODY_DEMO_HA" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_haid.c
    )
endif()
if("CONFIG_EXT_MIC_SUPPORT" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_cmd_ext_mic.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${ROOT_DIR}/application/ux/solution/bg/database
    ${ROOT_DIR}/application/ux/activity/power_activity
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
