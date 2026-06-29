#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(activity_manager)

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
add_subdirectory_if_exist(bg_activity)
add_subdirectory_if_exist(audio_activity)
add_subdirectory_if_exist(player_activity)
add_subdirectory_if_exist(usb_activity)
add_subdirectory_if_exist(protocol_activity)

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES)
    add_subdirectory_if_exist(live_mic_rx_activity)
endif()

if("MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
    add_subdirectory_if_exist(live_mic_tx_activity)
endif()

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    add_subdirectory_if_exist(earphone_activity)
endif()

if("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    add_subdirectory_if_exist(system_activity)
    add_subdirectory_if_exist(dongle_activity)
endif()

if(NOT ("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES))
    add_subdirectory_if_exist(charger_activity)
endif()

else()
add_subdirectory_if_exist(watch_bg_activity)
endif()

add_subdirectory_if_exist(power_activity)
