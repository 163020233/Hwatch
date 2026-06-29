/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: wearable Test case
 * Author: Hisi Graphic Team
 * Created: 2025-07
 */
#ifndef GARPHIC_UI_WEARABLE_TEST_TABLE_H
#define GARPHIC_UI_WEARABLE_TEST_TABLE_H

#include "ui_test.h"
#if ENABLE_BITMAP_FONT
#include "ui_test_bitmap_font/ui_test_bitmap_font.h"
#endif
#include "ui_test_draw_rect/ui_test_draw_rect.h"
#include "test_image/ui_test_image.h"
#include "ui_test_image_fps/ui_test_image_fps.h"
#include "ui_test_key/ui_test_key.h"
#include "ui_test_list_ext/ui_test_list_ext.h"
#if (ENABLE_FONT_VECTOR_GLOBAL && ENABLE_FONT_GLOBAL_TEST)
#include "ui_test_font/ui_test_font_global.h"
#endif
#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
#include "ui_test_image/ui_test_image_packer.h"
#endif
#ifndef _WIN32
#include "ui_test_video_play/ui_test_video_play.h"
#endif
#include "ui_test_capture/ui_test_capture.h"
#if ENABLE_SOFT_KEYBOARD
#include "ui_test_keyboard/ui_test_keyboard.h"
#endif
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
#include "ui_test_draw/ui_test_canvas_ext.h"
#include "ui_test_countdown/ui_test_countdown.h"
#include "ui_test_running_ball/ui_test_running_ball.h"
#ifndef _WIN32
#include "ui_test_svg/ui_test_svg.h"
#endif
#endif
#if CAPABILITY_HW_JPEG_DECODE
#include "ui_test_image/ui_test_jpeg.h"
#endif
#if ENABLE_JPEG_AND_PNG
#include "ui_test_image/ui_test_png.h"
#endif
#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
#include "ui_test_font/ui_test_multi_font.h"
#endif
#include "ui_test_icosahedron/ui_test_icosahedron.h"
#include "ui_test_clock_dial/ui_test_clock_dial.h"
#include "ui_test_compass/ui_test_compass.h"
#include "ui_test_sweepclock/ui_test_sweepclock.h"
#include "ui_test_text_fps/ui_test_text_fps.h"
#include "ui_test_tiger/ui_test_tiger.h"
#include "ui_test_hexagons_list/ui_test_hexagons_list.h"
#include "ui_test_waterfall_list/ui_test_waterfall_list.h"
#include "ui_test_list_fps/ui_test_list_fps.h"
#include "ui_test_card_switch/ui_test_card_switch.h"
#include "ui_test_message/ui_test_message.h"
#include "ui_test_breath/ui_test_breath.h"
#include "ui_test_cube_rotate/ui_test_cube_rotate.h"
#include "ui_test_map/ui_test_map.h"
#include "ui_test_coverflow/ui_test_coverflow.h"
#include "ui_test_coverflow/ui_test_coverflow2.h"
#include "ui_test_kaleidoscope/ui_test_kaleidoscope.h"
#include "ui_test_roller_view/ui_test_roller_view.h"
#include "ui_test_transform_list/ui_test_scale_list.h"
#include "ui_test_particle/ui_test_particle.h"
#include "ui_test_lottie/ui_test_trim_path.h"
#include "ui_test_lottie/ui_test_lottie.h"
#include "ui_test_colorize/ui_test_colorize.h"
#include "ui_test_hsv/ui_test_hsv.h"
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
#include "ui_test_light/ui_test_light.h"
#endif
#include "ui_test_rain/ui_test_rain.h"
#include "ui_test_3d/ui_test_mesh.h"
#include "ui_test_3d/ui_test_cylinder.h"
#include "ui_test_3d/ui_test_sphere.h"
#include "ui_test_float_box/ui_test_float_box.h"
#include "ui_test_foldable/ui_test_foldable.h"
#include "ui_test_view_alignment/ui_test_external_alignment.h"
#include "ui_test_view_alignment/ui_test_internal_alignment.h"
#include "ui_test_view_alignment/ui_test_dynamic_alignment.h"
#include "ui_test_simple_list/ui_test_simple_list.h"
#include "ui_test_simple_list/ui_test_simple_reach_list.h"
#include "ui_test_simple_list/ui_test_simple_transform_list.h"
#include "ui_test_video_play/ui_test_video_ayuv.h"
#include "ui_test_shine_on_click/ui_test_shine_on_click.h"
#include "ui_test_chart/ui_test_scatter_plot.h"
#include "ui_test_draw/ui_test_draw.h"
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#include "ui_test_rotate_injector/ui_test_rotate_injector.h"
#endif
#include "ui_test_cross/ui_test_cross.h"
#include "ui_test_emoji/ui_test_emoji.h"
#if ENABLE_VECTOR_FONT
#include "test_font/ui_test_font.h"
#endif
#include "test_render/ui_test_render.h"
#include "test_ui_analog_clock/ui_test_analog_clock.h"
#include "test_chart/ui_test_chart_polyline.h"
#include "test_chart/ui_test_chart_pillar.h"
#include "test_arc_label/ui_test_arc_label.h"
#include "test_ui_list/ui_test_ui_list.h"
#include "test_qrcode/ui_test_qrcode.h"
#include "test_slider/ui_test_slider.h"
#include "test_texture_mapper/ui_test_texture_mapper.h"
#include "test_button/ui_test_button.h"
#include "test_picker/ui_test_ui_picker.h"
#include "test_progress_bar/ui_test_box_progress.h"
#include "test_progress_bar/ui_test_circle_progress.h"
#include "test_ui_list/ui_test_ui_list.h"
#include "test_ui_list_view/ui_test_list_layout.h"
#include "test_ui_scroll_view/ui_test_ui_scroll_view.h"
#include "test_ui_swipe_view/ui_test_ui_swipe_view.h"
#include "test_canvas/ui_test_canvas.h"
#include "test_scroll_bar/ui_test_scroll_bar.h"
#include "test_input_event/ui_test_input_event.h"
#include "test_view_group/ui_test_view_group.h"
#include "test_animator/ui_test_animator.h"
#include "test_border_margin_padding/ui_test_border_margin_padding.h"
#include "test_digital_clock/ui_test_digital_clock.h"
#include "test_draw_line/ui_test_draw_line.h"
#include "test_event_injector/ui_test_event_injector.h"
#include "test_focus_manager/ui_test_focus_manager.h"
#include "test_image_animator/ui_test_image_animator.h"
#include "test_opacity/ui_test_opacity.h"
#include "test_rotate_input/ui_test_rotate_input.h"
#include "test_view_bounds/ui_test_view_bounds.h"
#include "test_view_percent/ui_test_view_percent.h"
#include "test_view_scale_rotate/ui_test_view_scale_rotate.h"
#include "test_view_bitmap/ui_test_view_bitmap.h"

namespace OHOS {
static void SetUpTestCaseToList(List<TestCaseInfo> *testCaseList)
{
    if (testCaseList == nullptr) {
        return;
    }
#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
    testCaseList->PushBack(TestCaseInfo{"光照", new UITestLight()});
#endif
    testCaseList->PushBack(TestCaseInfo{"3D Mesh", new UITestMesh()});
    testCaseList->PushBack(TestCaseInfo{"3D Cylinder", new UITestCylinder()});
    testCaseList->PushBack(TestCaseInfo{"3D Sphere", new UITestSphere()});
    testCaseList->PushBack(TestCaseInfo{"散点图", new UITestScatterPlot()});
    testCaseList->PushBack(TestCaseInfo{"悬浮框", new UITestFloatBox()});
    testCaseList->PushBack(TestCaseInfo{"折叠控件", new UITestFoldable()});
    testCaseList->PushBack(TestCaseInfo{"路径裁剪", new UITestTrimPath()});
    testCaseList->PushBack(TestCaseInfo{"Lottie", new UITestLottie()});
    testCaseList->PushBack(TestCaseInfo{"粒子效果", new UITestParticle()});
    testCaseList->PushBack(TestCaseInfo{"瀑布列表", new UITestWaterfallList()});
    testCaseList->PushBack(TestCaseInfo{"老虎头", new UITestTiger()});
#ifndef QSPI_DISPLAY
    testCaseList->PushBack(TestCaseInfo{"蜂窝表盘", new UITestHexagonsList()});
#endif
    testCaseList->PushBack(TestCaseInfo{"无变换滑动列表", new UITestListFPS()});
    testCaseList->PushBack(TestCaseInfo{"带缩放滑动列表", new UITestListExt()});
    testCaseList->PushBack(TestCaseInfo{"简易列表", new UITestSimpleList()});
    testCaseList->PushBack(TestCaseInfo{"简易触底检测列表", new UITestSimpleReachList()});
    testCaseList->PushBack(TestCaseInfo{"简易变换列表", new UITestSimpleTransformList()});
    testCaseList->PushBack(TestCaseInfo{"扫秒", new UITestSweepClock()});
    testCaseList->PushBack(TestCaseInfo{"指南针", new UITestCompass()});
    testCaseList->PushBack(TestCaseInfo{"全屏文字", new UITestTextFPS()});
    testCaseList->PushBack(TestCaseInfo{"立方体旋转", new UITestCubeRotate()});
    testCaseList->PushBack(TestCaseInfo{"卡片切换", new UITestCardSwitch()});
    testCaseList->PushBack(TestCaseInfo{"消息", new UITestMessage()});
    testCaseList->PushBack(TestCaseInfo{"呼吸效果", new UITestBreath()});
#ifndef QSPI_DISPLAY
    testCaseList->PushBack(TestCaseInfo{"地图", new UITestMap()});
#if !defined(_WIN32)
    testCaseList->PushBack(TestCaseInfo{"视频播放", new UITestVideoPlay()});
#endif
#if !defined(_WIN32)
    testCaseList->PushBack(TestCaseInfo{"万花筒", new UITestKaleidoscope()});
#endif
    testCaseList->PushBack(TestCaseInfo{"容器覆盖流", new UITestCoverflow2()});
    testCaseList->PushBack(TestCaseInfo{"覆盖流", new UITestCoverflow()});
#endif
    testCaseList->PushBack(TestCaseInfo{"滚筒", new UITestRollerView()});
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
    testCaseList->PushBack(TestCaseInfo{"倒计时", new UITestCountdown()});
    testCaseList->PushBack(TestCaseInfo{"动画曲线", new UITestRunningBall()});
    testCaseList->PushBack(TestCaseInfo{"扩展画布", new UITestCanvasExt()});
#ifndef _WIN32
    testCaseList->PushBack(TestCaseInfo{"绘制SVG", new UITestSVG()});
#endif
#endif
    testCaseList->PushBack(TestCaseInfo{"足球效果", new UITestIcosahedron()});
#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
    testCaseList->PushBack(TestCaseInfo{"多语言混排", new UITestMultiFont()});
#endif
#if ENABLE_SOFT_KEYBOARD
    testCaseList->PushBack(TestCaseInfo{"软键盘", new UITestKeyboard()});
#endif
#if CAPABILITY_HW_JPEG_DECODE
    testCaseList->PushBack(TestCaseInfo{"JPEG显示", new UITestJPEG()});
#endif
#if ENABLE_JPEG_AND_PNG
    testCaseList->PushBack(TestCaseInfo{"PNG显示", new UITestPNG()});
#endif
    testCaseList->PushBack(TestCaseInfo{"弧形文字", new UITestArcLabel()});
    testCaseList->PushBack(TestCaseInfo{"截图", new UITestCapture()});
    testCaseList->PushBack(TestCaseInfo{"全屏高斯模糊", new UITestKey()});
#if ENABLE_BITMAP_FONT
    testCaseList->PushBack(TestCaseInfo{"点阵文字", new UITestBitmapFont()});
#endif
#if (ENABLE_FONT_VECTOR_GLOBAL && ENABLE_FONT_GLOBAL_TEST)
    testCaseList->PushBack(TestCaseInfo{"多语言切换", new UITestFontGlobal()});
#endif
    testCaseList->PushBack(TestCaseInfo{"图片显示", new UITestImage()});
    testCaseList->PushBack(TestCaseInfo{"矩形绘制", new UITestDrawRect()});
#if (ENABLE_IMAGE_PACKER && ENABLE_IMAGE_PACKER_TEST)
    testCaseList->PushBack(TestCaseInfo{"图片打包", new UITestImageBin()});
#endif
    testCaseList->PushBack(TestCaseInfo{"图片动画", new UITestImageFPS()});
    testCaseList->PushBack(TestCaseInfo{"切换动效", new UITestClockDial()});
    testCaseList->PushBack(TestCaseInfo{"颜色侵染", new UITestColorize()});
    testCaseList->PushBack(TestCaseInfo{"HSV换色", new UITestHSV()});
    testCaseList->PushBack(TestCaseInfo{"雨滴效果", new UITestRain()});
    testCaseList->PushBack(TestCaseInfo{"外部对齐", new UITestExternalAlignment()});
    testCaseList->PushBack(TestCaseInfo{"内部对齐", new UITestInternalAlignment()});
    testCaseList->PushBack(TestCaseInfo{"动态对齐", new UITestDynamicAlignment()});
    testCaseList->PushBack(TestCaseInfo{"透明视频", new UITestVideoAYUV()});
    testCaseList->PushBack(TestCaseInfo{"按键发光", new UITestShineOnClick()});
    testCaseList->PushBack(TestCaseInfo{"基础画布", new UITestDraw()});
    testCaseList->PushBack(TestCaseInfo{"原生画布", new UITestCanvas()});
    testCaseList->PushBack(TestCaseInfo{"图片旋转缩放动画", new UITestTextureMapper()});
#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
    testCaseList->PushBack(TestCaseInfo{"模拟表冠", new UITestRotateInjector()});
#endif
    testCaseList->PushBack(TestCaseInfo{"二维码", new UITestQrcode()});
    testCaseList->PushBack(TestCaseInfo{"折线图", new UITestChartPolyline()});
    testCaseList->PushBack(TestCaseInfo{"柱状图", new UITestChartPillar()});
#if ENABLE_VECTOR_FONT
    testCaseList->PushBack(TestCaseInfo{"矢量文字", new UITestFont()});
#endif
    testCaseList->PushBack(TestCaseInfo{"滑动条", new UITestSlider()});
    testCaseList->PushBack(TestCaseInfo{"透明度", new UITestOpacity()});
    testCaseList->PushBack(TestCaseInfo{"播放图片动画", new UITestImageAnimator()});
    testCaseList->PushBack(TestCaseInfo{"焦点管理", new UITestFocusManager()});
    testCaseList->PushBack(TestCaseInfo{"绘制线条", new UITestDrawLine()});
    testCaseList->PushBack(TestCaseInfo{"数字时钟", new UITestDigitalClock()});
    testCaseList->PushBack(TestCaseInfo{"边界间隔", new UITestBorderMarginPadding()});
    testCaseList->PushBack(TestCaseInfo{"动效", new UITestAnimator()});
    testCaseList->PushBack(TestCaseInfo{"视图组", new UITestViewGroup()});
    testCaseList->PushBack(TestCaseInfo{"事件测试", new UITestInputEvent()});
    testCaseList->PushBack(TestCaseInfo{"滚动条", new UITestScrollBar()});
    testCaseList->PushBack(TestCaseInfo{"Swipe视图", new UITestUISwipeView()});
    testCaseList->PushBack(TestCaseInfo{"滑动视图", new UITestUIScrollView()});
    testCaseList->PushBack(TestCaseInfo{"列表布局", new UITestListLayout()});
    testCaseList->PushBack(TestCaseInfo{"列表", new UITestUIList()});
    testCaseList->PushBack(TestCaseInfo{"按钮", new UITestButton()});
    testCaseList->PushBack(TestCaseInfo{"选择", new UITestUIPicker()});
    testCaseList->PushBack(TestCaseInfo{"圆形进度条", new UITestCircleProgress()});
    testCaseList->PushBack(TestCaseInfo{"盒状进度条", new UITestBoxProgress()});
    testCaseList->PushBack(TestCaseInfo{"事件模拟", new UITestEventInjector()});
    testCaseList->PushBack(TestCaseInfo{"视图比例", new UITestViewPercent()});
    testCaseList->PushBack(TestCaseInfo{"视图边界", new UITestViewBounds()});
    testCaseList->PushBack(TestCaseInfo{"模拟时钟", new UITestAnalogClock()});
    testCaseList->PushBack(TestCaseInfo{"旋转输入", new UITestRotateInput()});
    testCaseList->PushBack(TestCaseInfo{"渲染", new UITestRender()});
}
}
#endif