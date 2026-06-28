#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "3322_ai_engine_tts_demo_private")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tts_model.c
)
 
set(PUBLIC_HEADER
)
 
set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../public/tts/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tts/inc/npu
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
    3322_ai_engine_tts
)
 
build_component()