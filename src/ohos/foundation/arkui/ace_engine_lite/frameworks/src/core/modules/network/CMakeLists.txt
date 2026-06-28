#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_network")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativeui/bluetooth/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network/connection_manage/
    ${ROOT_DIR}/application/ux/adapter/bg/watch
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network/network_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network/network_listener.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network/connection_manage/connection_manage.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
    -Wno-sometimes-uninitialized
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

