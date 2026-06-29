#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_bluetooth_port")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth_sub_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/latm_simple_muxer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_a2dp_uplink/src/bt_a2dp_uplink.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_sco/src/bt_sco.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nearlink_dongle_downlink/src/nearlink_downlink.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nearlink_dongle_uplink/src/nearlink_uplink.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_a2dp_downlink/src/bt_a2dp_downlink.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_a2dp_downlink/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_a2dp_uplink/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bt_sco/include
    ${CMAKE_CURRENT_SOURCE_DIR}/nearlink_dongle_downlink/include
    ${CMAKE_CURRENT_SOURCE_DIR}/nearlink_dongle_uplink/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin_include
    ${OHOS_DIR}/drivers/peripheral/audio/interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
    ${ROOT_DIR}/application/audio/
    ${ROOT_DIR}/drivers/drivers/driver/audio/source/include/
    ${ROOT_DIR}/drivers/drivers/driver/audio/source/core/common/include/
    ${ROOT_DIR}/application/audio/arch/3322/
)

if("${CHIP}" MATCHES "3322")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/watch/common/
        ${ROOT_DIR}/application/ux/interfaces/framework/
    )
endif()

set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT" "MINI_MEMORY_SUPPORT")
if("bth_sample" IN_LIST RAM_COMPONENT)
    list(APPEND CDEFINES "BT_HAL_SUPPORT")
endif()
if("${CHIP}" MATCHES "socmn1|socmn2")
    set(CDEFINES ${CDEFINES} "MELODY_AUDIO_SUPPORT" "SUPPORT_MUTIL_SOUND")
endif()
set(PRIVATE_DEFINES
    ${CDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)


set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()
