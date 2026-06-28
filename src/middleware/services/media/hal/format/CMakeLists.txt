#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "format_hw")
set(COMPONENT_DIR ${ROOT_DIR}/middleware/services/media/component/hmf_demuxer)
if (EXISTS ${COMPONENT_DIR})
    add_subdirectory(plugins/hmf_demuxer)
if(NOT "CONFIG_SUPPORT_MEDIA_MEMORY_MINI" IN_LIST DEFINES AND "${CHIP}" MATCHES "brandy|3322")
    add_subdirectory(plugins/protocol_https)
    add_subdirectory(plugins/mjpeg_demuxer)
    add_subdirectory(plugins/ffmpeg_demuxer)
endif()
else()
    if("plugin_demuxer_hmf" IN_LIST RAM_COMPONENT)
        set(NEWDEFINES ${NEWDEFINES} "ENABLE_PLUGIN_DEMUXER_HMF")
    endif()
endif()
add_subdirectory(plugins/raw_demuxer)
add_subdirectory(plugins/wav_muxer)
add_subdirectory_if_exist(plugins/buffer_raw_demuxer)
add_subdirectory_if_exist(plugins/common/dtcf)
add_subdirectory_if_exist(src/decrypt)
# use build env and option camke
include(../../build/build_env.cmake)
include(../../build/build_option.cmake)

option(ENABLE_BUFFER_MANAGER "buffer manager" OFF)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/src/demuxer_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_plugins_manager/src/demuxer_plugins_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_common/src/format_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_manager/src/muxer_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_plugins_manager/src/muxer_plugins_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins_manager_common/src/plugins_manager_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol_plugins_manager/src/protocol_plugins_manager.c
)

if(ENABLE_BUFFER_MANAGER)
    #set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/src/demuxer_buffer.c)
    #set(CDEFINES ${CDEFINES} "ENABLE_BUFFER_MANAGER")
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/demuxer_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/format_common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/muxer_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins_manager_common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol_plugins_manager/include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/hmf_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/raw_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/wav_muxer
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/ffmpeg_demuxer
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/buffer_raw_demuxer
    ${OHOS_DIR}/drivers/peripheral/format/interfaces/include
)

 set(CDEFINES ${CDEFINES} "ENABLE_DEMUXER" "-DENABLE_MUXER" "ENABLE_PROTOCOL_PLUGIN")
 set(PRIVATE_DEFINES
     ${CDEFINES}
 )

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

set(AUTO_ADD_PATH
    false
)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/media/${BOARD}_${APPLICATION}/${OS}/${COMPONENT_NAME})

build_component()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/plugins/hmf_demuxer/CMakeLists.txt "*.txt")