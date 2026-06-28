#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "audio_primary_port")

# use build env and option camke
include(../../../../../build/build_env.cmake)
include(../../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/primary_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/primary_sub_port_in.c
    ${CMAKE_CURRENT_SOURCE_DIR}/primary_sub_port_out.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin_include
    ${OHOS_DIR}/drivers/peripheral/audio/interfaces/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${ROOT_DIR}/drivers/drivers/driver/audio/include
)

set(CDEFINES ${CDEFINES} "AUDIO_SDK_SUPPORT" "AUDIO_SEA_SUPPORT")

if(${CHIP} MATCHES "socmn1|socmn2")
    set(CDEFINES ${CDEFINES} "AUDIO_HAID_SUPPORT" "MELODY_AUDIO_SUPPORT" "SUPPORT_MUTIL_SOUND")
elseif (${CHIP} MATCHES "brandy|3322")
    set(CDEFINES ${CDEFINES} "MINI_MEMORY_SUPPORT" "SUPPORT_AUDIO_RECOGNITION" "SUPPORT_SEA_AHE_PARAM")
endif()
if(${CHIP} MATCHES "socmn2")
if("MELODY_DEMO_BMIC_TX" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_RX" IN_LIST DEFINES OR
   "MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_DONGLE" IN_LIST DEFINES OR
   "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES) 
   list(REMOVE_ITEM CDEFINES "AUDIO_HAID_SUPPORT")
endif()
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
