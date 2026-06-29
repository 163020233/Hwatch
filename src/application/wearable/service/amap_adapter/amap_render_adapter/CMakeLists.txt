#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "amap_render_adapter")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/awk_mal_render_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mal_awk_convert.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/kernel/liteos/liteos_v207.1.0_b014/Huawei_LiteOS/open_source/rt-thread/include
    ${ROOT_DIR}/application/wearable/service/amap_service/amap_os_adapter
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

build_component()
