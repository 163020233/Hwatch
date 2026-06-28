#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "amap_os_adapter")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_fs_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_log_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_mem_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_net_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_thread_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/amap_http_response_callback.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/osal/schedule
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
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
