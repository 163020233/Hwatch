#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_socket")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/socket
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/third_party/lwip/lwip_v2.1.3/src/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/socket/socket_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/socket/socket_module_client.cpp
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
 