#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "codec_plugin_vdec")
# use build env and option camke
include(../../../../build/build_env.cmake)
include(../../../../build/build_option.cmake)
if(${CHIP} MATCHES "3322")
  set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/video_decoder/native_vdec.c)
else()
  set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/video_decoder/native_vdec_jpeg.c)
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/video_decoder
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/pluginManager/codec_plugin_base
    ${ROOT_DIR}/middleware/utils/common_headers/drivers/gpu
    ${ROOT_DIR}/middleware/utils/common_headers/native
    ${OHOS_DIR}/drivers/peripheral/codec/interfaces/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/utils/common_headers/vdec/external
    ${ROOT_DIR}/middleware/utils/common_headers/vdec/internal
)

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