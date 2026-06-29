#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "heart_rate")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ppg_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ppg_math_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ppg_preprocessing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ppg_memory_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ppg_heart_variable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fft.c
)


if("SUPPORT_SPORT_HEALTH_TEST" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ppg_main.c
    )
endif()

if("SPORTS_HEALTH_NPU" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/npu/heart_rate_model.c
        ${CMAKE_CURRENT_SOURCE_DIR}/npu/utils.c
    )
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mcu/ai_model.c
    )
endif()

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
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


set(PUBLIC_HEADER
)


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
