#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_lite")

set(PUBLIC_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/cache
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/utils
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/resource
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/device/soc/hisilicon/common/hardware/litewearable_base/mem_hal
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components


    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/third_party/qrcodegen/cpp
    ${OHOS_DIR}/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/resource
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/ace_ext
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
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/include
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/animation/transition_impl.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/ace_lock.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/async_task_manager.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/dft_impl.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/dfx_assist.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/event_util.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/js_debugger_config.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/js_fwk_common.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/key_parser.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/lazy_load_manager.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/lazy_load_watcher.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/locale_util.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/number_parser.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/product_adapter.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/string_util.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/system_info.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/base/time_util.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/analog_clock_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/camera_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/canvas_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/chart_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/circle_progress_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/clock_hand_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/component_utils.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/div_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/event_listener.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/horizon_progress_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/image_animator_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/image_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/input_button_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/input_checkbox_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/input_edittext_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/input_radio_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/list_adapter.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/list_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/marquee_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/panel_view.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/picker_view_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/qrcode_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/scroll_layer.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/slider_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/stack_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/swiper_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/switch_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/tab_bar_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/tab_content_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/tabs_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/text_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/video_component.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/components/video_view.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/ace_event_error_code.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/fatal_handler.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_ability.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_ability_impl.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_app_context.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_app_environment.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_framework_raw.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_profiler.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/js_timer_list.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/dialog/js_dialog.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/directive/descriptor_utils.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/directive/directive_watcher_callback.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/resource/video_muted_image_res.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/resource/video_play_image_res.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/router/js_page_state.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/router/js_page_state_machine.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/router/js_router.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/app_style.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/app_style_item.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/app_style_list.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/app_style_manager.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/app_style_sheet.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/condition_arbitrator.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/link_queue.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/stylemgr/link_stack.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/wrapper/js.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/platform_adapter.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/context/slite_ace_ability.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/router_module.cpp
     ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/timer_module.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
    -Wno-unused-private-field
    -Wno-reorder-ctor
    -Wno-sometimes-uninitialized
    -Wno-sign-compare
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
    -std=c++14
)

set(PUBLIC_DEFINES
    FEATURE_COMPONENT_TEXT_SPANNABLE=0
)

set(PRIVATE_DEFINES
    ENABLED_DELETE_MODULE_IMAGE=1
    JS_PROFILER=1
    ABILITY_WINDOW_SUPPORT
    ENABLE_CANVAS_EXTEND
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "ace_lite_adapt")

set(PUBLIC_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/cache
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/utils
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/resource
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/device/soc/hisilicon/common/hardware/litewearable_base/mem_hal
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/src/utils/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/service
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/components
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/middleware/services/gui/uikit/surface/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/layout
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/innerkits/common
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/font
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/dfx
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/frameworks
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include/components


    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include/slite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite/slite
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/third_party/qrcodegen/cpp
    ${OHOS_DIR}/foundation/communication/ipc/interfaces/innerkits/c/ipc/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/want_lite
    ${OHOS_DIR}/foundation/ability/ability_lite/frameworks/abilitymgr_lite/include
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/inner_api/abilitymgr_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/interfaces/kits/bundle_lite
    ${OHOS_DIR}/foundation/bundlemanager/bundle_framework_lite/utils/bundle_lite
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/async
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/jsi
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/context
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/base
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/include/resource
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/ace_ext
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
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${ROOT_DIR}/application/wearable/nativeapp/nativelauncher/include
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/include
    ${OHOS_DIR}/base/global/resource_management_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits/hilog
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/innerkits
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/app_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/dialog_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/app_data_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/cjson_parser.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/console_log_impl.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/console_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/date_time_format_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/image_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/intl_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/localization_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/number_format_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/preset_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/profiler_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/render_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/require_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/syscap_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/presets/version_module.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/sample_module.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
    -Wno-unused-private-field
    -Wno-reorder-ctor
    -Wno-sometimes-uninitialized
    -Wno-sign-compare
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
    -std=c++14
)

set(PUBLIC_DEFINES
    FEATURE_COMPONENT_TEXT_SPANNABLE=0
)

set(PRIVATE_DEFINES
    ENABLED_DELETE_MODULE_IMAGE=1
    JS_PROFILER=1
    ABILITY_WINDOW_SUPPORT
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "websocket")
set(MODULE_NAME "ohos")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/websocket/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/websocket/internal
    ${OHOS_DIR}/third_party/lwip/lwip_v2.1.3/src/include
    ${OHOS_DIR}/third_party/lwip/lwip_adapter/liteos/src/include
)

if("SUPPORT_LWIP" IN_LIST DEFINES)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_priv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/modules/netstack/websocket/src/net_websocket_thread.c
)
endif()

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

set(MAIN_COMPONENT
   false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    WEBSOCKET_SSL_ENABLE
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/interfaces" "*")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include" "*")

build_component()