#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

enable_language(CXX)

set(COMPONENT_NAME "js_audio_capturer")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/recording_module.cpp
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(OGG_INCLUDE ${ROOT_DIR}/open_source/libogg/libogg-1.3.5/include)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../utils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../interfaces/kits
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../frameworks/buffersource/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../frameworks/player_control/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../frameworks/player_control/decoder
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../frameworks/player_control/player
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../frameworks/player_control/sink
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../frameworks/player_control/source
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/audioservice/include
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/frameworks/audiostream/include
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/frameworks/audioutils/include
    ${OHOS_DIR}/drivers/peripheral/format/interfaces/include
    ${OHOS_DIR}/drivers/peripheral/codec/interfaces/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${ROOT_DIR}/application/wearable/service/xiaodu_voice
    ${OGG_INCLUDE}
    ${OGG_INCLUDE}/ogg/
)

set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "UNSUPPORT_AUDIO_DECODER" "SUPPORT_OGG_SPLIT_PAGE")
set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_FLAGS ${ccflags})
list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables)
set(COMPONENT_CCFLAGS
    ${COMPONENT_FLAGS}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/media/${BOARD}_${APPLICATION})

build_component()
