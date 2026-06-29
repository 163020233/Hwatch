#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "module_ActsDfxFuncTest")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/test/xts/acts/hiviewdfx_lite/hilog_hal/src
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/frameworks/mini
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/test/xts/tools/lite/hctest/include
    ${OHOS_DIR}/third_party/unity/src
    ${OHOS_DIR}/kernel/liteos_m/kal
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/test/xts/acts/hiviewdfx_lite/hilog_hal/src/dfx_hilog.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    UNITY_INCLUDE_CONFIG_H
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

