#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "gesture")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gesture_algorithm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/slide_window.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data_segmentation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/imu_preprocess.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data_preprocess.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ai_preprocess.c
    ${CMAKE_CURRENT_SOURCE_DIR}/supp_functions.c
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gesture_common_def.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gesture_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mcu/ai_model.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gesture_filter.c
)

if("SUPPORT_SPORT_HEALTH_TEST" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/gesture_main.c
    )
endif()
 
if("SPORTS_HEALTH_NPU" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/npu/gesture_model.c
        ${CMAKE_CURRENT_SOURCE_DIR}/npu/utils.c
    )
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mcu/ai_model.c
    )
endif()
 

set(PUBLIC_HEADER

)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mcu
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/application/wearable/service/common
)

if("SPORTS_HEALTH_NPU" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/npu
        ${ROOT_DIR}/include/middleware/services/ai/acl
        ${ROOT_DIR}/include/middleware/services/ai/acl/driver
    )
else()
    list(APPEND PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/mcu
    )
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-strict-prototypes
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "*.h")

build_component()
