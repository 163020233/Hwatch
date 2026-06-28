#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_http")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/http/http_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/http/http_module_client.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/http/http_module_callback.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
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
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
   false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

build_component()
 