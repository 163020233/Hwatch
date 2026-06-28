#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
get_property(_cfbb_nativeapp_project_included GLOBAL PROPERTY CFBB_NATIVEAPP_PROJECT_INCLUDED)
if("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}"
        AND NOT _cfbb_nativeapp_project_included)
    set_property(GLOBAL PROPERTY CFBB_NATIVEAPP_PROJECT_INCLUDED TRUE)
    cmake_minimum_required(VERSION 3.14.1)

    # 包含封装的 build_project.cmake
    include("${ROOT_DIR}/build/cmake/build_project.cmake")
    # 项目构建
    cfbb_build_project()
else()
get_property(_cfbb_nativeapp_included GLOBAL PROPERTY CFBB_NATIVEAPP_COMPONENT_INCLUDED)
if(_cfbb_nativeapp_included)
    message(STATUS "component already included, skipping to avoid recursion")
    return()
endif()
set_property(GLOBAL PROPERTY CFBB_NATIVEAPP_COMPONENT_INCLUDED TRUE)
set(COMPONENT_NAME "nativelauncher")
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)
set(NativeApp ${CMAKE_CURRENT_SOURCE_DIR})
set(UIKIT_ROOT_DIR ${ROOT_DIR}/middleware/services/gui/uikit)
set(UI_SAMPLE_ROOT ${ROOT_DIR}/middleware/services/gui/uikit/sample/uitest)
set(WEARABLE_SRV_DIR ${ROOT_DIR}/application/wearable/service/)
set(BTS_DIR ${ROOT_DIR}/include/middleware/services/bts)
set(OHOS_TEST_CASE_DIR ${OHOS_DIR}/foundation/arkui/ui_lite/test/uitest)

if("SUPPORT_ALIPAY_HSC32I1" IN_LIST DEFINES)
    set(ALIPAY_SDK_DIR ${ROOT_DIR}/vendor/alipay_sdk/rv70_llvm_HSC32I1)
elseif("SUPPORT_ALIPAY_SEC" IN_LIST DEFINES)
    set(ALIPAY_SDK_DIR ${ROOT_DIR}/vendor/alipay_sdk/alipay_sec_sdk_v3.0.0/lib/rv70_llvm)
endif()

set(PUBLIC_HEADER
    ${NativeApp}/nativelauncher/include
    ${NativeApp}/nativeui/include
    ${NativeApp}/nativeui/include/phonemenu
    ${NativeApp}/nativeui/applist/include
    ${NativeApp}/nativeui/uiservice/include
    ${NativeApp}/nativeui/bluetooth/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/application/wearable/ohos_deps
    ${NativeApp}/nativeui/uisample/uitest/framework/include
    ${WEARABLE_SRV_DIR}/smart_voice/smart_voice_service/interfaces/innerkits/native_c/include
    ${WEARABLE_SRV_DIR}/smart_voice/voice_engine_hal/interfaces/innerkits/native_c/include
    ${WEARABLE_SRV_DIR}/msg_center/include
    ${WEARABLE_SRV_DIR}/device_msg
    ${WEARABLE_SRV_DIR}/dial_market

    ${UI_SAMPLE_ROOT}/components
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/include/components
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits/events
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits/gfx_utils
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits/components
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${BTS_DIR}/common
    ${BTS_DIR}/ble
    ${BTS_DIR}/br
    ${ROOT_DIR}/middleware/chips/3322/nv/nv_config/nv_default/include
    ${ROOT_DIR}/drivers/boards/3322_evb/product
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/application/wearable/nativeabilityfwk/include
    ${ROOT_DIR}/middleware/chips/3322/include/shared
    ${NativeApp}/nativeui/include/uisample
    ${NativeApp}/nativeui/include/applist
    ${ROOT_DIR}/drivers/chips/3322/include/shared
    ${ROOT_DIR}/include
    ${ROOT_DIR}/protocol/bt/host/bt/sdk/L2/common

    ${OHOS_DIR}/foundation/multimedia/player/interfaces/kits/
    ${OHOS_DIR}/third_party/qrcodegen/cpp
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/foundation/distributedschedule/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/foundation/multimedia/audio_player/interfaces/kits
    ${OHOS_DIR}/foundation/multimedia/audiomanager/interfaces/kits/audiomanager
    ${OHOS_DIR}/foundation/multimedia/audiomanager/interfaces/kits/audiocommon
    ${OHOS_DIR}/foundation/multimedia/player/interfaces/kits/
    ${OHOS_DIR}/foundation/multimedia/utils/include
    ${OHOS_DIR}/utils/native/lite/include
    ${OHOS_DIR}/utils/native/lite/kv_store/innerkits
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/distributedschedule/kv_store/interfaces/inner_api/kv_store/include
    ${OHOS_DIR}/drivers/peripheral/base
    ${OHOS_DIR}/device/soc/hisilicon/common/hardware/litewearable_base/mem_hal
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/animator
    ${OHOS_DIR}/foundation/ability/ability_lite/interfaces/kits/ability_lite/slite
    ${OHOS_DIR}/drivers/peripheral/format/interfaces/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/samgr/adapter/posix
    ${OHOS_DIR}/third_party/lwip/lwip_adapter/liteos/src/include/public
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/calendarManager
    ${OHOS_DIR}/base/global/resource_management_lite/interfaces/inner_api/include
    ${ROOT_DIR}/application/ux/adapter/bg/watch
    ${NativeApp}/nativeui/include/uiservice
    ${OHOS_TEST_CASE_DIR}
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network/connection_manage
    ${ALIPAY_SDK_DIR}/share/common/inc
    ${ROOT_DIR}/application/wearable/service/amap_service/amap_controller
    ${ROOT_DIR}/middleware/services/gui/uikit/proprietary/include
    ${ROOT_DIR}/ohos/foundation/arkui/ace_engine_lite/frameworks/src/core/ace_ext/map/map_service/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/src/core/map_fwk/map_client/include
    ${ROOT_DIR}/middleware/services/gui/uikit/ui/interfaces/kits/common
    ${OHOS_DIR}/third_party/zlib
)

if("SUPPORT_ALIPAY_HSC32I1" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ALIPAY_SDK_DIR}/app/nfc_offline_pay/inc
        ${ALIPAY_SDK_DIR}/app/scan_pay/inc
    )
endif()

file(GLOB_RECURSE SOURCES "${NativeApp}/nativeui/*.cpp")
file(GLOB_RECURSE UI_SAMPLE_SRC "${UI_SAMPLE_ROOT}/components/*.cpp")
file(GLOB_RECURSE UI_SAMPLE_SOCMN2 "${UI_SAMPLE_ROOT}/components/socmn2_test_case/*.cpp")
file(GLOB_RECURSE UI_SAMPLE_OPENSOURCE ${OHOS_TEST_CASE_DIR}/test_animator/ui_test_animator.cpp
    ${OHOS_TEST_CASE_DIR}/test_arc_label/ui_test_arc_label.cpp
    ${OHOS_TEST_CASE_DIR}/test_border_margin_padding/ui_test_border_margin_padding.cpp
    ${OHOS_TEST_CASE_DIR}/test_button/ui_test_button.cpp
    ${OHOS_TEST_CASE_DIR}/test_canvas/ui_test_canvas.cpp
    ${OHOS_TEST_CASE_DIR}/test_chart/ui_test_chart_pillar.cpp
    ${OHOS_TEST_CASE_DIR}/test_chart/ui_test_chart_polyline.cpp
    ${OHOS_TEST_CASE_DIR}/test_digital_clock/ui_test_digital_clock.cpp
    ${OHOS_TEST_CASE_DIR}/test_draw_line/ui_test_draw_line.cpp
    ${OHOS_TEST_CASE_DIR}/test_event_injector/ui_test_event_injector_view.cpp
    ${OHOS_TEST_CASE_DIR}/test_focus_manager/ui_test_focus_manager.cpp
    ${OHOS_TEST_CASE_DIR}/test_image_animator/ui_test_image_animator.cpp
    ${OHOS_TEST_CASE_DIR}/test_input_event/ui_test_input_event.cpp
    ${OHOS_TEST_CASE_DIR}/test_opacity/ui_test_opacity.cpp
    ${OHOS_TEST_CASE_DIR}/test_picker/ui_test_ui_picker.cpp
    ${OHOS_TEST_CASE_DIR}/test_progress_bar/ui_test_box_progress.cpp
    ${OHOS_TEST_CASE_DIR}/test_progress_bar/ui_test_circle_progress.cpp
    ${OHOS_TEST_CASE_DIR}/test_qrcode/ui_test_qrcode.cpp
    ${OHOS_TEST_CASE_DIR}/test_rotate_input/ui_test_rotate_input.cpp
    ${OHOS_TEST_CASE_DIR}/test_scroll_bar/ui_test_scroll_bar.cpp
    ${OHOS_TEST_CASE_DIR}/test_slider/ui_test_slider.cpp
    ${OHOS_TEST_CASE_DIR}/test_texture_mapper/ui_test_texture_mapper.cpp
    ${OHOS_TEST_CASE_DIR}/test_ui_list/ui_test_ui_list.cpp
    ${OHOS_TEST_CASE_DIR}/test_ui_list_view/ui_test_list_layout.cpp
    ${OHOS_TEST_CASE_DIR}/test_ui_scroll_view/ui_test_ui_scroll_view.cpp
    ${OHOS_TEST_CASE_DIR}/test_ui_swipe_view/ui_test_ui_swipe_view.cpp
    ${OHOS_TEST_CASE_DIR}/test_view_bitmap/ui_test_view_bitmap.cpp
    ${OHOS_TEST_CASE_DIR}/test_view_bounds/ui_test_view_bounds.cpp
    ${OHOS_TEST_CASE_DIR}/test_view_group/ui_test_view_group.cpp
    ${OHOS_TEST_CASE_DIR}/test_view_percent/ui_test_view_percent.cpp
    ${OHOS_TEST_CASE_DIR}/test_view_scale_rotate/ui_test_view_scale_rotate.cpp
    ${OHOS_TEST_CASE_DIR}/test_ui_analog_clock/ui_test_analog_clock.cpp
    ${OHOS_TEST_CASE_DIR}/test_render/ui_test_render.cpp
    ${OHOS_TEST_CASE_DIR}/test_font/ui_test_font.cpp
)

list(REMOVE_ITEM UI_SAMPLE_SRC
    ${UI_SAMPLE_ROOT}/components/ui_test_bitmap_font/ui_test_bitmap_font.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ui_test_clock_dial.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ScreenModel.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/PullUpDownView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/MainClockView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/HeartRateView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/CompassView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/CompassControlView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_clock_dial/ActivityWeekView.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_compass/ui_test_compass.cpp
    ${UI_SAMPLE_ROOT}/components/ui_test_dump_dom_tree/ui_test_dump_dom.cpp
    ${UI_SAMPLE_SOCMN2}
)

if(NOT "CONFIG_NATIVEAPP_TEST" IN_LIST DEFINES)
    file(GLOB FILES_TO_REMOVE_MONKEY "${CMAKE_CURRENT_SOURCE_DIR}/nativeui/monkey/src/*.cpp")
    file(GLOB FILES_TO_REMOVE_EMPTY "${CMAKE_CURRENT_SOURCE_DIR}/nativeui/emptypage/src/*.cpp")
    list(APPEND FILES_TO_REMOVE ${FILES_TO_REMOVE_MONKEY} ${FILES_TO_REMOVE_EMPTY})
    list(REMOVE_ITEM SOURCES ${FILES_TO_REMOVE})
endif()

set(SOURCES
    ${SOURCES}
    ${NativeApp}/nativelauncher/src/NativeLauncher.cpp
    ${NativeApp}/nativelauncher/src/NativeRegisterManager.cpp
    ${NativeApp}/nativelauncher/src/UICardPageFactory.cpp
    ${NativeApp}/nativelauncher/src/UIWatchDialFactory.cpp
    ${UI_SAMPLE_SRC}
    ${UI_SAMPLE_OPENSOURCE}
)

list(REMOVE_ITEM SOURCES
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParser.cpp
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParserManager.cpp
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParserV3.cpp
    ${NativeApp}/nativeui/main/src/dial/parser/DialBinParserV3_1.cpp
)

# Please remove the pages that are not used in XTS.
if("XTS_SUPPORT" IN_LIST DEFINES)
    list(REMOVE_ITEM SOURCES
        ${NativeApp}/nativeui/alarm/src/AlarmPresenter.cpp
        ${NativeApp}/nativeui/alipay/src/AlipayPresenter.cpp
        ${NativeApp}/nativeui/blood/src/BloodPresenter.cpp
        ${NativeApp}/nativeui/breathexercise/src/BreathPresenter.cpp
        ${NativeApp}/nativeui/camera/src/CameraPresenter.cpp
        ${NativeApp}/nativeui/compass/src/MainCompassPresenter.cpp
        ${NativeApp}/nativeui/date/src/DatePresenter.cpp
        ${NativeApp}/nativeui/emptypage/src/EmptyPresenter.cpp
        ${NativeApp}/nativeui/findphone/src/FindPhonePresenter.cpp
        ${NativeApp}/nativeui/flashlight/src/FlashLightPresenter.cpp
        ${NativeApp}/nativeui/heartrate/src/HeartRatePresenter.cpp
        ${NativeApp}/nativeui/monkey/src/MonkeyPresenter.cpp
        ${NativeApp}/nativeui/offlinevoice/src/VocassistPresenter.cpp
        ${NativeApp}/nativeui/phonemenu/src/PhoneMenuPresenter.cpp
        ${NativeApp}/nativeui/sleep/src/SleepPresenter.cpp
        ${NativeApp}/nativeui/sport/src/SportCountdownPage.cpp
        ${NativeApp}/nativeui/sport/src/SportPresenter.cpp
        ${NativeApp}/nativeui/stopwatch/src/StopwatchPresenter.cpp
        ${NativeApp}/nativeui/timers/src/TimerPresenter.cpp
        ${NativeApp}/nativeui/weather/src/WeatherPresenter.cpp
        ${NativeApp}/nativeui/settings/src/SettingPresenter.cpp
        ${NativeApp}/nativeui/uisample/src/UISamplePresenter.cpp
        ${NativeApp}/nativeui/settings/src/SettingPresenter.cpp
    )
endif()

if(NOT "SUPPORT_ALIPAY_SEC" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/alipay/src/AlipayMainView.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayModel.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayPresenter.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayBind.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayExitBind.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayBindRes.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayBindAni.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayPayCode.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayBarCode.cpp

                         ${NativeApp}/nativeui/alipay/src/AlipaySetting.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayHelp.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayUnbindFinish.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayUnbind.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayRefreshBindCode.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListAdapter.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListView.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayList/AlipayListItemView.cpp

                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayGetTransList.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransTips.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransCode.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransEnable.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransRes.cpp

                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListAdapter.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListView.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransListItemView.cpp
                         ${NativeApp}/nativeui/alipay/src/AlipayTrans/AlipayTransAni.cpp
)
endif()

if(NOT "SUPPORT_ALIPAY_HSC32I1" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayHsc32I1Model.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayHsc32I1Presenter.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayBarcodePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayBindExitPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayBindingPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayBindPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayBindResultPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayHelpPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayMenuItemView.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayMenuPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayPayFailedPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayPaySuccessPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayQrcodePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipaySettingPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayStartPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapAmountPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapConfirmPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapFinishPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapInputPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapPayingPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapPayPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapResultPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransCardItemView.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransEnablePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransGuidePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransListPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransQrcodePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransRefreshPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransResultPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransTipsPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayUnbindFinishPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayUnbindPage.cpp
)

endif()

if(NOT "SUPPORT_ALIPAY_TAP" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapAmountPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapConfirmPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapFinishPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapInputPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapPayingPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapPayPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTapResultPage.cpp
)
endif()

if(NOT "SUPPORT_ALIPAY_TRANS" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransCardItemView.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransEnablePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransGuidePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransListPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransQrcodePage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransRefreshPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransResultPage.cpp
                         ${NativeApp}/nativeui/alipay_hsc32i1/src/AlipayTransTipsPage.cpp
)
endif()

if(NOT "CONFIG_MAP_DEMO" IN_LIST DEFINES)
list(REMOVE_ITEM SOURCES ${NativeApp}/nativeui/map/MainStateHandler.cpp
                         ${NativeApp}/nativeui/map/MapDragZoomHandler.cpp
                         ${NativeApp}/nativeui/map/MapModel.cpp
                         ${NativeApp}/nativeui/map/MapPageContext.cpp
                         ${NativeApp}/nativeui/map/MapPage.cpp
                         ${NativeApp}/nativeui/map/MapPresenter.cpp
                         ${NativeApp}/nativeui/map/MapResultContentItemView.cpp
                         ${NativeApp}/nativeui/map/MapService.cpp
                         ${NativeApp}/nativeui/map/MapUtils.cpp
                         ${NativeApp}/nativeui/map/NaviRouteRenderer.cpp
                         ${NativeApp}/nativeui/map/NaviSimulator.cpp
                         ${NativeApp}/nativeui/map/NaviStateHandler.cpp
                         ${NativeApp}/nativeui/map/PoiStateHandler.cpp
                         ${NativeApp}/nativeui/map/RouteStateHandler.cpp
)
endif()

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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
    -Wno-missing-field-initializers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
    SUPPORT_NATIVE_UI
)

set(PRIVATE_DEFINES
)

build_component()

set(COMPONENT_NAME "uikit_fwk")
set(UIKIT_NATIVE_FWK ${ROOT_DIR}/application/wearable/nativeabilityfwk)
set(UIKIT_NATIVE_DIAL ${NativeApp}/nativeui/main)
set(COMPONENT_SRC
    ${UIKIT_NATIVE_FWK}/src/View.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePage.cpp
    ${UIKIT_NATIVE_FWK}/src/BackDragListener.cpp
    ${UIKIT_NATIVE_FWK}/src/PageTransitionMgr.cpp
    ${UIKIT_NATIVE_FWK}/src/TransitionManager.cpp
    ${UIKIT_NATIVE_FWK}/src/RecentManager.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePageManager.cpp
    ${UIKIT_NATIVE_FWK}/src/SlicePageFactory.cpp
    ${UIKIT_NATIVE_FWK}/src/TransitionRecord.cpp
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_NATIVE_FWK}/include
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/ace/ace_engine_lite/interfaces/base
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks/
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits/gfx_utils
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/middleware/services/media/foundation/player/interfaces/kits
    ${ROOT_DIR}/base/security/appverify/interfaces/innerkits/appverify/include
    ${ROOT_DIR}/application/ux/adapter/bg/watch
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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
    -Wno-missing-field-initializers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "wearable_dial")
set(UIKIT_NATIVE_FWK ${ROOT_DIR}/application/wearable/nativeabilityfwk)
set(UIKIT_NATIVE_DIAL ${NativeApp}/nativeui/main)
set(COMPONENT_SRC
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParser.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParserManager.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParserV3.cpp
    ${UIKIT_NATIVE_DIAL}/src/dial/parser/DialBinParserV3_1.cpp
)

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_NATIVE_DIAL}/include
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/foundation/ace/ace_engine_lite/interfaces/base
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks/
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits/gfx_utils
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${UIKIT_NATIVE_DIAL}/include/dial
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
    ${ROOT_DIR}/middleware/services/media/foundation/player/interfaces/kits
    ${ROOT_DIR}/base/security/appverify/interfaces/innerkits/appverify/include
    ${ROOT_DIR}/application/ux/adapter/bg/watch
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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
    -Wno-missing-field-initializers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
endif()
