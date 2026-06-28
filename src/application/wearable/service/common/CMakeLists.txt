#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ai_common")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/data_loader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utf8_to_gbk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix_memory.c
)

if("SPORTS_HEALTH_NPU" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/fp32_to_fp16.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
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
