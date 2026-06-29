#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_port_common")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin_common.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin_include
    ${CMAKE_CURRENT_SOURCE_DIR}/../primary
    ${CMAKE_CURRENT_SOURCE_DIR}/../bluetooth
    ${CMAKE_CURRENT_SOURCE_DIR}/../modem
    ${CMAKE_CURRENT_SOURCE_DIR}/../usb
    ${OHOS_DIR}/drivers/peripheral/audio/interfaces/include
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
)

if(${CHIP} MATCHES "socmn1|socmn2")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/middleware/utils/nv/nv_storage_lib
        ${ROOT_DIR}/application/dut/command/plt
        ${ROOT_DIR}/drivers/chips/sw22/porting/patch/
    )
elseif("${CHIP}" STREQUAL "brandy")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/middleware/chips/brandy/nv/nv_config/include
    )
endif()

set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT" "AUDIO_SEA_SUPPORT" "SUPPORT_NV")

if (${CHIP} MATCHES "brandy|3322" AND NOT "MEMORY_MINI" IN_LIST DEFINES)
    set(CDEFINES ${CDEFINES} "NOT_SUPPORT_MULTI_AENC" "SUPPORT_AUDIO_EFFECT_PARAM" "SUPPORT_SEA_AHE_PARAM")
endif()

if("audio_modem_port" IN_LIST RAM_COMPONENT)
    set(CDEFINES ${CDEFINES} "SUPPORT_MODEM")
endif()

if(${CHIP} MATCHES "socmn1")
    set(CDEFINES ${CDEFINES} "AUDIO_HAID_SUPPORT" "MELODY_AUDIO_SUPPORT" "SUPPORT_UX_CONFIG" "SUPPORT_MUTIL_SOUND"
        "SUPPORT_VOLUME_CALIBRATION")
elseif(${CHIP} MATCHES "socmn2")
    set(CDEFINES ${CDEFINES} "AUDIO_HAID_SUPPORT" "MELODY_AUDIO_SUPPORT"
        "SUPPORT_MUTIL_SOUND" "SUPPORT_ADC0_AI_PORT" "SUPPORT_FEATURE_CONFIG")
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

set(AUTO_ADD_PATH
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()
