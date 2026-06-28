#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "abilityms")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/inner_api/bundlemgr_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/include/util
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/ability_lite/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/registry
    ${OHOS_DIR}/ohos_adapt/hals/memory
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/frameworks
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/third_party/qrcodegen/cpp
    ${OHOS_DIR}/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/resource
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/animation
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/directive
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/router
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/wrapper
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/cache
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/utils
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/native_engine
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/native_engine/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/module_manager
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/include
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/include/jerryscript-ext
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/include
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api
    ${OHOS_DIR}/third_party/jerryscript/jerry-core
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jrt
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/lit
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/commonlibrary/utils_lite/timer_task/include
    ${OHOS_DIR}/third_party/freetype/include
    ${OHOS_DIR}/base/global/resource_management_lite/interfaces/inner_api/include
    ${OHOS_DIR}/base/global/i18n_lite/interfaces/kits/i18n/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/syscap
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/foundation/multimedia/media_utils_lite/interfaces/kits
    ${OHOS_DIR}/base/sensors/sensor_lite/interfaces/kits/native/include
    ${OHOS_DIR}/base/location/location_lite/interfaces/kits/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/baidumap
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/include
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_list.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_mgr_service_slite.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_record.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_record_manager.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_record_observer_manager.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_thread.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/ability_thread_loader.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/bms_helper.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/js_ability_thread.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/native_ability_thread.cpp
    ${OHOS_DIR}/foundation/ability/ability_lite/services/abilitymgr_lite/src/slite/slite_ability_loader.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-format-extra-args
    -Wno-missing-field-initializers
    -Wno-mismatched-tags
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
    -std=c++14
    -Wno-c++17-extensions
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    # 6K for litewearable launcher.
    AMS_TASK_STACK_SIZE=6144
    ABILITY_LIST_CAPACITY=16
    TASK_STACK_SIZE=24576
    NATIVE_TASK_STACK_SIZE=4096
    _MINI_BMS_
    SHARED_TASK_STACK_SIZE=4608
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

