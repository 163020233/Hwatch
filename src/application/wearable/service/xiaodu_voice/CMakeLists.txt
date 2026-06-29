#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "xiaodu_voice")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${OHOS_DIR}/foundation/multimedia/audio_player/interfaces/kits
    ${OHOS_DIR}/foundation/multimedia/audiomanager/interfaces/kits/audiomanager
    ${OHOS_DIR}/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
    ${OHOS_DIR}/foundation/multimedia/player/interfaces/kits/
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/xiaodu_voice_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/xiaodu_voice.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-missing-field-initializers
    -Wno-writable-strings
)

set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()


