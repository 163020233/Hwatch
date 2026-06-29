#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)
set(OHOS_TEST_CASE_DIR ${OHOS_DIR}/foundation/arkui/ui_lite/test/uitest)

set(COMPONENT_NAME "uikit_test")

file(GLOB_RECURSE socmn2_test_case_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/socmn2_test_case/*.cpp)

file(GLOB_RECURSE open_source_file
${OHOS_TEST_CASE_DIR}/test_animator/ui_test_animator.cpp
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

if("${CHIP}" STREQUAL "socmn2")
set(src_file
    ${CMAKE_CURRENT_SOURCE_DIR}/sample_ui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uitest/framework/src/test_case_list_adapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uitest/framework/src/ui_test_app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uitest/framework/src/ui_test_group.cpp
    ${socmn2_test_case_file}
)
else()
    file(GLOB_RECURSE src_file ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${open_source_file}
    )
    if(NOT "JS_ENABLE" IN_LIST DEFINES)
        file(GLOB files_to_remove "${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_transition/*.cpp")
        list(REMOVE_ITEM src_file ${files_to_remove})
    endif()
    list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/components/test_bitmap_font.cpp)
    list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_bitmap_font/ui_test_bitmap_font.cpp)
    list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_font/ui_test_font_global.cpp)
    list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_image/ui_test_image_packer.cpp)
    list(REMOVE_ITEM src_file ${CMAKE_CURRENT_SOURCE_DIR}/uitest/components/ui_test_cross/ui_test_cross.cpp)
    list(REMOVE_ITEM src_file ${socmn2_test_case_file})
endif()

set(COMPONENT_SRC
    ${src_file}
)
set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/sample
)

set(PRIVATE_HEADER
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/sample/uitest
    ${UIKIT_ROOT_DIR}/sample/uitest/components
    ${UIKIT_ROOT_DIR}/sample/uitest/framework/include
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${UIKIT_ROOT_DIR}/proprietary/include/service
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${PROJECT_ROOT_DIR}/hal/input/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/third_party/bounds_checking_function/include
    ${OHOS_DIR}/third_party/qrcodegen/cpp
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager
    ${ROOTDIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${OHOS_DIR}/foundation/multimedia/player/interfaces/kits
    ${OHOS_DIR}/foundation/multimedia/utils/include
    ${ROOTDIR}/application/wearable/service/power_manager/include
    ${OHOS_TEST_CASE_DIR}
)

if("JS_ENABLE" IN_LIST DEFINES)
set(PRIVATE_HEADER
    ${PRIVATE_HEADER}
    ${ROOTDIR}/application/wearable/nativeabilityfwk/include
)
endif()

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-writable-strings
    -Wno-reorder-ctor
    -Wno-tautological-pointer-compare
    -Werror=return-type
)
if("${CHIP}" STREQUAL "socmn2")
    list(APPEND COMPONENT_CCFLAGS -Wno-macro-redefined)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
