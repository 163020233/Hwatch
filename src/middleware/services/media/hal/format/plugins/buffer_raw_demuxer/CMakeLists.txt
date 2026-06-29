#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "buffer_raw_demuxer")

# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)

if(NOT "${OS_TYPE}" STREQUAL "Linux")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_raw_demuxer.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
)


set(PRIVATE_DEFINES
)

set(CDEFINES ${CDEFINES} "CMSIS_SUPPORT" "ENABLE_PLUGIN_BUFFER_RAW_DEMUXER")
set(PUBLIC_DEFINES
    ${CDEFINES}
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

else()
    add_definitions("-DENABLE_PLUGIN_MUXER_WAV")
    set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../..")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../build/build_env.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../build/build_option.cmake)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/wav_muxer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/wav.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/dtcf_manager/dtcf_manager.c
    )
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../${HAL_RELEASE_PATH}/format/libs)
    add_library("${COMPONENT_NAME}_linux" STATIC ${SOURCES})
    target_include_directories("${COMPONENT_NAME}_linux" PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/dtcf_manager
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/dtcf/include
        ${PROJECT_ROOT_DIR}/include
    )
    target_include_directories("${COMPONENT_NAME}_linux" PUBLIC
        ${SECUREC_INCLUDE}
        ${PROJECT_ROOT_DIR}/middleware/services/media/hal/common/include
        ${PROJECT_ROOT_DIR}/middleware/utils/dfx/dfx_os/dfx_include
    )
endif()