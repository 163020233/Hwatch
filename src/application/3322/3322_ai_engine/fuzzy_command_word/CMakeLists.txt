#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_ai_engine_fuzzy_command_word")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/asr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/audio_vqe_common_basicop_vec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/audio_vqe_common_basicop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/ctc_prefix_beamsearch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/fbank.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/logSumExp_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/map_dict.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/sys_dsp_fix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/window_coeff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/nlu_slot_convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/nlu_postproc_tab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio/nlu_slot_postproc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npu/asr_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npu/ctc_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npu/nlu_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/npu/asr_ctc_model.c
    #${CMAKE_CURRENT_SOURCE_DIR}/fuzz_demo.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/npu
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-strict-prototypes
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(COMPONENT_REQUIRES
    3322_ai_engine_common_lib
)

build_component()

