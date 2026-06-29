#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "voice_cloud_LLM")

# The sample is disabled by default. It only builds (and links, via config.py
# ram_component) when CONFIG_VOICE_CLOUD_LLM_ENABLE is defined for the target.
if(NOT "CONFIG_VOICE_CLOUD_LLM_ENABLE" IN_LIST DEFINES)
    return()
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${ROOT_DIR}/application/wearable/service/intelligent_voice/include
    ${ROOT_DIR}/middleware/services/media/foundation/service/intelligent_voice_service/asr_engine/include
    ${ROOT_DIR}/middleware/services/media/foundation/service/audio_manger_service/include
    ${ROOT_DIR}/middleware/services/media/foundation/audio_capturer/interfaces/kits
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager/cpp
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/websocket/include
    ${OHOS_DIR}/third_party/cJSON
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_ws_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_audio_capture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_audio_player.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/voice_cloud_llm_at.c
)

set(COMPONENT_PUBLIC_CCFLAGS)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(WHOLE_LINK true)

set(PUBLIC_DEFINES)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()
