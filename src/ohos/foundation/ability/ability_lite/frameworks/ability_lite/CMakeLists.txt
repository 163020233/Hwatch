#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ability")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${OHOS_DIR}/third_party/freetype/include
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/src/slite/ability_saved_data.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/src/slite/lite_context.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/src/slite/slite_ability.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/src/ability_env.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/src/ability_env_impl.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-format-extra-args
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    OPENHARMONY_FONT_PATH
    ABILITY_WINDOW_SUPPORT
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

