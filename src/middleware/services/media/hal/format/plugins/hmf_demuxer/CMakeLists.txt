#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "plugin_demuxer_hmf")
# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hmf_demuxer_plugin.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../component/hmf_demuxer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../component/hmf_demuxer/src/common/utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../component/hmf_demuxer/src/protocol_file
)

if("CONFIG_SUPPORT_MEDIA_MEMORY_MINI" IN_LIST DEFINES OR "${CHIP}" MATCHES "socmn2|socmn1")
    set(CDEFINES ${CDEFINES} "HMF_MINI_MEDIA_ENABLE" "MEMORY_MINI")
else()
    if(${CHIP} MATCHES "brandy|3322")
        set(CDEFINES ${CDEFINES} "HMF_MINI_MEDIA_ENABLE" "HMF_VIDEO_MEDIA_ENABLE" "HMF_DECRYPT_DATA_ENABLE")
    endif()
endif()
set(PRIVATE_DEFINES
    ${CDEFINES}
)
set(NEWDEFINES "ENABLE_PLUGIN_DEMUXER_HMF")
set(PUBLIC_DEFINES
    ${NEWDEFINES}
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

if("CONFIG_SUPPORT_MEDIA_MEMORY_MINI" IN_LIST DEFINES OR "${CHIP}" MATCHES "socmn2|socmn1")
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/mini)
elseif("SUPPORT_OHOSFWK" IN_LIST DEFINES)
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/js)
else()
    set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION}/${OS}/normal)
endif()

build_component()