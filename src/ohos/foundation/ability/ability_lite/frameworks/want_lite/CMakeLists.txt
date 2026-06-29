#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "want")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/want_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite/include
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/want_lite/src/want.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-non-c-typedef-for-linkage
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

