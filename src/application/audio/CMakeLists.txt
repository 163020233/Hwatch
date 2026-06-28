#[[
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
Description: CMake audio sample module.
Author: audio
Create: 2022-06-05
]]

add_subdirectory_if_exist(vendor)
#===================================================================================
set(COMPONENT_NAME "audio_sample")
if(NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    return()
endif()
#===================================================================================
if("${CHIP}" STREQUAL "hi2132")
set(SAP_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/drivers/driver/audio/riscv_codec/framework)
else()
set(SAP_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/drivers/driver/audio/source)
endif()
#===================================================================================
get_filename_component(SAP_TOP_DIR ${SAP_TOP_DIR} ABSOLUTE)
set(FUZZ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3322/3322_ai_engine/fuzzy_command_word)
get_filename_component(FUZZ_DIR ${FUZZ_DIR} ABSOLUTE)
set(TTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3322/3322_ai_engine/tts)
get_filename_component(TTS_DIR ${TTS_DIR} ABSOLUTE)
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_config.cmake)
#===================================================================================
set(MODULE_NAME "dsp")
set(AUTO_DEF_FILE_ID TRUE)
#===================================================================================
set(COMPONENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ai/sample_ai.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ai/sample_ai_aenc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ao/sample_ao.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ao/sample_aef.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_decode/sample_adec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_decode/sample_decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_encode/sample_encode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_encode/sample_cast_aenc_play.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_lpwk/sample_lpwk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_gui_aef/sample_gui_aef.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_sea/sample_sea.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_sea/sample_sea_only.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_phone/sample_phone_apps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_dump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_proc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/sample_rms.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thread/athread_cmsis.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio_fops.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_audio_weak.c
)

if("CONFIG_NPU_LIB" IN_LIST DEFINES)
if("${CFG_SAP_ASR_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_asr/sample_asr.c
)
endif()

if("${CFG_SAP_TTS_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_tts/sample_tts.c
)
endif()
endif()

if("${CFG_SAP_EFFECT_ADJUSTMENT_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_effect/sample_effect.c
)
endif()

if("${CFG_SAP_HAID_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_haid/sample_haid.c
)
endif()

if("${CFG_SAP_SMART_WATCH_PRODUCT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_asr/sample_vad_feed.c
)
endif()

if("${CFG_SAP_DPM_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm/dpm_fir_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm/sample_dpm.c
)
endif()

if("${CFG_SAP_LIVE_MIC_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_bmic/sample_bmic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_bmic/sample_bmic_rx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_bmic/sample_bmic_tx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_bmic/sample_bmic_rec.c
)
endif()

if("${CFG_SAP_TESTCASE_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_test/sample_test_int.c
)
endif()

if("${CHIP}" STREQUAL "brandy")
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/sample_dpm "*")
endif()

if("${CFG_SAP_ANC_SUPPORT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_aha/sample_aha.c
)
endif()

if("${CFG_SAP_DONGLE_PRODUCT}" STREQUAL "y")
set(COMPONENT_SRC ${COMPONENT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_sle_music/sample_sle_music.c
)
endif()
#===================================================================================
set(COMPONENT_INC
    ${SAP_TOP_DIR}/include
    ${SAP_TOP_DIR}/core/common/include
    ${SAP_TOP_DIR}/core/osal/liteos
    ${SAP_TOP_DIR}/core/arch/${CFG_SAP_CHIP_TYPE}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_phone
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_decode
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/${CFG_SAP_CHIP_TYPE}
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_asr
)
#===================================================================================
if("${CHIP}" STREQUAL "hi2132")
set(DIAG_INC
    ${ROOT_DIR}/middleware/chips/${CHIP}/dfx/include
    ${ROOT_DIR}/middleware/chips/${CHIP}/dfx
)
set(COMPONENT_INC ${COMPONENT_INC} ${DIAG_INC} ${SAP_UAPI_DIR})
list(REMOVE_ITEM COMPONENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/sample_gui_aef/sample_gui_aef.c")
endif()
#===================================================================================
set(COMPONENT_DEF
    "SAP_PROC_SUPPORT"
)

if("CONFIG_NPU_LIB" IN_LIST DEFINES)
if("${CFG_SAP_ASR_SUPPORT}" STREQUAL "y")
set(COMPONENT_INC ${COMPONENT_INC}
    ${FUZZ_DIR}/inc/audio
    ${FUZZ_DIR}/inc/npu
    ${FUZZ_DIR}/../common/inc
    ${ROOT_DIR}/include/middleware/services/ai/acl/driver
)
set(COMPONENT_AUDIO_CCFLAGS "-Wno-strict-prototypes")
endif()

if("${CFG_SAP_TTS_SUPPORT}" STREQUAL "y")
set(COMPONENT_INC ${COMPONENT_INC}
    ${TTS_DIR}/inc/npu
    ${TTS_DIR}/../common/inc
)
endif()
endif()
#===================================================================================
include(${SAP_TOP_DIR}/build/cmake/audio_common.cmake)
install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/sample_dsp_overlay "*")
