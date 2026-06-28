#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)

# UX内部使用头文件列表
set(UX_INTERNAL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/activity_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/bg_activity/bt
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/bg_activity/common
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/bg_activity/sle
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/earphone_activity
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/protocol_activity
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/usb_activity
    ${CMAKE_CURRENT_SOURCE_DIR}/activity/player_activity
    ${CMAKE_CURRENT_SOURCE_DIR}/customer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/at
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/at/test
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/at/intf
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/event
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/fsm/fsm_box
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/fsm/fsm_master
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/fsm/fsm_pairing
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/include
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/task
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter/include
    ${CMAKE_CURRENT_SOURCE_DIR}/adapter/service_adapter
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/cmd
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/devicemgr
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/inputmgr/
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/live_mic_rx/
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/live_mic_tx/
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/protocol/
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/protocol/commu/
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/protocol/service/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/manager
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/core/volume
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/effect/noise_reduction
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/include/effect
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/include/service
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/audio/service/recorder
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/bg/ble
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/bg/bt
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/bg/sle
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/bg/spp
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/box
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/button
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/common
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/display/graphic/include
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/display/graphic/service
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/display/touch/include
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/display/touch/service
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/factory
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/factory/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/factory/fac_tlv
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/commu/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/fitting/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/service/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/statistic/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/include
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/led
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/protocol/service
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/protocol/service/haid_dev
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/timer
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/tlv/
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/tone/config
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/tone/service
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/usb
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/sensor
    ${CMAKE_CURRENT_SOURCE_DIR}/solution/haid/media/
)
if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/manager/earphone)
endif()

if("DEMO_WATCH" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/activity/watch_bg_activity)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/solution/bg/bt)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/manager/communication)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/at)
endif()

if("CONFIG_SUPPORT_HFP_HF" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/bg/hfp/include)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/audio/sco)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/bg/bt)
endif()

if("CONFIG_SUPPORT_AVRCP" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/bg/avrcp)
endif()

if("CONFIG_SUPPORT_PBAP" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/bg/pbap)
endif()

if("CONFIG_SUPPORT_A2DP_SINK" IN_LIST DEFINES OR "CONFIG_SUPPORT_A2DP_SRC" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/audio/a2dp)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/bg/a2dp)
endif()

if (${CHIP} MATCHES "3322")
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/watch/common)
endif()

if("DEMO_WATCH" IN_LIST DEFINES)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/adapter/power/watch)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/adapter/bg/watch)
else()
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/adapter/power/melody)
    list(APPEND UX_INTERNAL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/adapter/bg/melody)
endif()

if (${CHIP} STREQUAL "socmn1")
    add_subdirectory_if_exist(${CHIP})
else()
    add_subdirectory_if_exist(interfaces)
    add_subdirectory_if_exist(activity)
    add_subdirectory_if_exist(customer)
    add_subdirectory_if_exist(delivery)
    add_subdirectory_if_exist(framework)
    add_subdirectory_if_exist(cli)
    add_subdirectory_if_exist(adapter)
    add_subdirectory_if_exist(manager)
    add_subdirectory_if_exist(solution)
endif()

if ("DEMO_WATCH" IN_LIST DEFINES)
    add_subdirectory_if_exist(watch)
endif()