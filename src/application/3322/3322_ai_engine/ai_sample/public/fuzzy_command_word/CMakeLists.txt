#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_ai_engine_fuzzy_command_word_demo_public")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fuzz_demo.c
)
 
set(PUBLIC_HEADER
)
 
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
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
    3322_ai_engine_fuzzy_command_word_demo_private
)

install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/files/ "*")
build_component()