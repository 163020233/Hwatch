#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_at")
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_system.c
)

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_manager.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_sle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_bs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_box.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_bt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_system_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_audio_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_sle_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/cust/ux_at_cust.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_recovery.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_audio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_led.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_dut.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_power.c
    )
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_call.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_api.c
    )
endif()

if("DEMO_WATCH" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_spp_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_ble_test.c
    )
endif()

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_power_test.c
    )
endif()

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_display_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_logic_id_test.c
    )
endif()

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_tws.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_spp_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_ble_test.c
        ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/ancmanager
    )
endif()

if("MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_player.c
    )
endif()

if("MELODY_DEMO_HA" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_at_haid.c
    )
endif()
if("ux_sensor" IN_LIST RAM_COMPONENT)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_sensor_test.c
    )
endif()
if("ux_jack" IN_LIST RAM_COMPONENT)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_jack_test.c
    )
endif()

if("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_usb_test.c
    )
endif()

if("MELODY_DEMO_BMIC_TX" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test/ux_at_usb_test.c
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
    -fstack-protector-strong
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
