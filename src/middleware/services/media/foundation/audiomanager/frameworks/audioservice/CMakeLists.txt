#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

enable_language(C)
set(COMPONENT_NAME "audio_service")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src/device_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/devices_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/interrupt_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/policy_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/policy_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/source_handle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/stream_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/stream_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src/track_handle.c
)
if (${CHIP} MATCHES "brandy|3322")
    set(SOURCES_TEMP ${SOURCES})
    list(APPEND SOURCES_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src/hifi_clock_freq_manager.c)
    set(SOURCES
        ${SOURCES_TEMP}
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devicemanager/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/streammanager/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../audioutils/include
    ${OHOS_DIR}/drivers/peripheral/audio/interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/interfaces/kits
    ${ROOTDIR}/include/middleware/utils
    ${ROOTDIR}/middleware/utils/nv/nv_storage_lib

)

set(PRIVATE_HEADER_TEMP ${PRIVATE_HEADER})
if (${CHIP} MATCHES "brandy|3322")
    list(APPEND PRIVATE_HEADER_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/brandy)
elseif (${CHIP} MATCHES "socmn1|socmn2")
if("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/dongle)
endif()
    list(APPEND PRIVATE_HEADER_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/src/policymanager/melody)
endif()
if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES) 
    set(CDEFINES ${CDEFINES} "SUPPORT_MIX_VOLUME")
endif()
set(PRIVATE_HEADER
    ${PRIVATE_HEADER_TEMP}
)

set(CDEFINES ${CDEFINES} "SUPPORT_NV")
if (${CHIP} MATCHES "brandy|3322")
    set(CDEFINES ${CDEFINES} "SUPPORT_SET_DSP_CLOCK_FREQ" "SUPPORT_SEA_AHE_PARAM")
elseif (${CHIP} MATCHES "socmn1|socmn2")
    set(CDEFINES ${CDEFINES} "MELODY_AUDIO_SUPPORT")
    if ("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES)
        list(APPEND CDEFINES "SUPPORT_DOWN_LINK_GLOBAL_MUTE" "SUPPORT_CURRENT_STREAM_MIX_WEIGHT")
    else()
        list(APPEND CDEFINES "SUPPORT_UP_LINK_GLOBAL_MUTE")
    endif()
endif()
if (${CHIP} MATCHES "3322")
    set(CDEFINES ${CDEFINES} "SUPPORT_A2DPOUT_MUSIC")
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

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${COMPONENT_NAME})

build_component()