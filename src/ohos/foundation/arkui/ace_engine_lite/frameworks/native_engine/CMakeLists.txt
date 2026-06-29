#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_native_engine")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/third_party/freetype/include
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/native_engine/async/js_async_work.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/native_engine/async/message_queue_utils.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/native_engine/jsi/jsi.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
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

