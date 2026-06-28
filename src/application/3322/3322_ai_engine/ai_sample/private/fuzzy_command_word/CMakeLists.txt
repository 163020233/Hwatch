#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_ai_engine_fuzzy_command_word_demo_private")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fuzzy_command_word.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model.c
)
 
set(PUBLIC_HEADER
)
 
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../public/fuzzy_command_word/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../fuzzy_command_word/inc/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../fuzzy_command_word/inc/npu
    ${ROOT_DIR}/include/middleware/services/ai/acl/driver
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
    3322_ai_engine_fuzzy_command_word
)
 
build_component()