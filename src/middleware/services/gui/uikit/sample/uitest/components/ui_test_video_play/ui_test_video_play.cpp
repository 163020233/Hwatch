/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "securec.h"
#include "sys/time.h"
#include "ui_test_video_play.h"
#include "components/root_view.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif

namespace OHOS {
static constexpr int16_t VIDEO_WIDTH = 466;
static constexpr int16_t VIDEO_HEIGHT = 466;
static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;
static constexpr int16_t BUFFER_QUEUE_SIZE = 3;
static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
static constexpr int16_t USEC_TO_MILLI = 1000;
static constexpr int16_t TIME_CONVERT = 60; // sec to min, and min to hor
static constexpr int16_t TEXT_BUFFER_SIEZ = 20;
static constexpr int16_t TEXT_FONT_SIEZ = 40;
static constexpr int16_t RECT_SIZE_CNT = 3;
static int16_t RECT_SIZE_IND = 0;
static Rect RECT_SIZES[] = {
    {33, 83, 400, 300},
    {83, 33, 300, 400},
    {83, 83, 300, 300},
};

#ifdef SUPPORT_OHOSFWK
static VideoPlayerService *g_videoPlay = nullptr;
static bool g_lastPlayed = false;
static void ScreenStatusNotify(ScreenStatus status)
{
    if (g_videoPlay == nullptr) {
        return;
    }

    bool temp = g_videoPlay->IsPlaying();
    if (status == SCREEN_STATUS_OFF && g_videoPlay->IsPlaying()) {
        g_videoPlay->Pause();
    } else if (status == SCREEN_STATUS_ON) {
        if (g_lastPlayed && g_videoPlay->IsPaused()) {
            g_videoPlay->Resume();
        }
    }
    g_lastPlayed = temp;
}
#endif

static void GetCount(char* buf, int len)
{
    static int count = 0;

    sprintf_s(buf, len, "count: %d", count);
    count++;
    count = count % 1000; // 1000: count times
}

void VideoAnimatorCallback::Callback(UIView *view)
{
    (void)view;
    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetCount(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    RootView::GetInstance()->Invalidate();
}

void UITestVideoPlay::SetUp()
{
    container_ = new UIScrollView();
    container_->SetThrowDrag(true);
    container_->Resize(VIDEO_WIDTH * 2, VIDEO_HEIGHT * 2); // 2 double,2 double
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    InitViews();
    CreateSurfaceView(0);
    AddToContainer();
}

void UITestVideoPlay::InitViews()
{
    label_ = new UILabel();
    char filePath[PATH_MAX];
    if (realpath("/user/video/panda.mp4", filePath) == nullptr) {
        label_->SetPosition(60, 227, 330, 70); // 60, 227, 330, 70: Left, right, width, height.
        label_->SetText("please upload video resource.");
        label_->SetStyle(STYLE_BACKGROUND_COLOR, 0xff505050);
        label_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        label_->SetStyle(STYLE_BORDER_RADIUS, 20); // 20: Radius for label border.
        label_->SetStyle(STYLE_BACKGROUND_OPA, 150); // 150: Opacity for background.
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 25); // 25: Font size.
        label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        GRAPHIC_LOGE("no video resource\n");
        return;
    }

    char buf[TEXT_BUFFER_SIEZ] = {0};
    GetCount(buf, TEXT_BUFFER_SIEZ);
    label_->SetText(buf);
    label_->SetPosition(60, 50, 330, 70); // 60, 50, 330, 70: Left, right, width, height.
    label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Silver().full);
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_FONT_SIEZ); // 25: Font size.
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);

    callBack_ = new VideoAnimatorCallback(label_);
    animator_ = new Animator(callBack_, label_, 0, true);
    animator_->Start();

    button1_ = new UILabelButton();
    button1_->SetPosition(100, 100, 120, 60); // 100: x, 100: y, 120: width, 60: height
    button1_->SetText("Play");
    button1_->SetTextColor(Color::Silver());
    button1_->SetOnClickListener(this);

    button2_ = new UILabelButton();
    button2_->SetPosition(260, 100, 120, 60); // 260: x, 100: y, 120: width, 60: height
    button2_->SetText("Stop");
    button2_->SetTextColor(Color::Silver());
    button2_->SetOnClickListener(this);

#if ENABLE_VIDEO_RGB
    buttonResizeMode_ = new UILabelButton();
    buttonResizeMode_->SetPosition(50, 350, 100, 40); // 50: x, 350: y, 100: width, 40: height
    buttonResizeMode_->SetText("ResizeMode");
    buttonResizeMode_->SetTextColor(Color::Silver());
    buttonResizeMode_->SetOnClickListener(this);

    buttonRotate_ = new UILabelButton();
    buttonRotate_->SetPosition(170, 350, 80, 40); // 170: x, 350: y, 80: width, 40: height
    buttonRotate_->SetText("Rotate");
    buttonRotate_->SetTextColor(Color::Silver());
    buttonRotate_->SetOnClickListener(this);

    buttonResize_ = new UILabelButton();
    buttonResize_->SetPosition(260, 350, 80, 40); // 260: x, 350: y, 80: width, 40: height
    buttonResize_->SetText("NextRect");
    buttonResize_->SetTextColor(Color::Silver());
    buttonResize_->SetOnClickListener(this);
#endif
}

void UITestVideoPlay::TearDown()
{
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = nullptr;
    deregisterNotifyScreenStatus(ScreenStatusNotify);
#endif
    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    if (videoPlay_ != nullptr) {
        videoPlay_->Stop();
        delete videoPlay_;
        videoPlay_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
    label_ = nullptr;
    button1_ = nullptr;
    button2_ = nullptr;
#if ENABLE_VIDEO_RGB
    buttonRotate_ = nullptr;
    buttonResizeMode_ = nullptr;
    buttonResize_ = nullptr;
#endif
    surfaceView_ = nullptr;
}

const UIView* UITestVideoPlay::GetTestView()
{
    return container_;
}

bool UITestVideoPlay::OnClick(UIView& view, const ClickEvent& event)
{
    int32_t ret = 0;
    if (videoPlay_ == nullptr) {
        GRAPHIC_LOGE("videoPlay_ is nullptr!\n");
        return false;
    }
    if (&view == button1_) {
        if (!videoPlay_->IsPlaying()) {
            ret = videoPlay_->Play();
            return true;
        }
        if (videoPlay_->IsPaused()) {
            ret = videoPlay_->Resume();
            if (ret == 0) {
                button1_->SetText("Pause");
            }
        } else {
            ret = videoPlay_->Pause();
            if (ret == 0) {
                button1_->SetText("Play");
            }
        }
    } else if (&view == button2_) {
        if (!videoPlay_->IsPlaying()) {
            GRAPHIC_LOGE("video is not played\n");
            return false;
        }
        videoPlay_->Stop();
#if ENABLE_VIDEO_RGB
    } else if (&view == buttonResizeMode_) {
        int ind = ((int)resizeMode_+ 1) % (int)UILiteSurfaceView::VideoResizeMode::MODE_MAX;
        resizeMode_= static_cast<UILiteSurfaceView::VideoResizeMode>(ind);
        surfaceView_->SetVideoResizeMode(resizeMode_);
    } else if (&view == buttonResize_) {
        RECT_SIZE_IND = (RECT_SIZE_IND + 1) % RECT_SIZE_CNT;
        videoPlay_->Stop();
        container_->RemoveAll();
        CreateSurfaceView(RECT_SIZE_IND);
        AddToContainer();
    } else if (&view == buttonRotate_) {
        rotation_ = (UILiteSurfaceView::VideoRotation)(((int)rotation_ + 1) % 4); // 4 : type count
        surfaceView_->SetRotation(rotation_);
#endif
    }
    return true;
}

void UITestVideoPlay::AddToContainer()
{
    container_->Add(surfaceView_);
    container_->Add(label_);
    container_->Add(button1_);
    container_->Add(button2_);
#if ENABLE_VIDEO_RGB
    container_->Add(buttonRotate_);
    container_->Add(buttonResizeMode_);
    container_->Add(buttonResize_);
#endif
}

void UITestVideoPlay::CreateSurfaceView(int index)
{
    if (index > RECT_SIZE_CNT) {
        index = 0;
    }
    if (surfaceView_ != nullptr) {
        delete surfaceView_;
        surfaceView_ = nullptr;
    }
    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        GRAPHIC_LOGE("surfaceView is nullptr\n");
        return;
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetViewId("sur");
    surfaceView_->SetPosition(RECT_SIZES[index].GetX(), RECT_SIZES[index].GetY(),
        RECT_SIZES[index].GetRight(), RECT_SIZES[index].GetBottom());
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
#if !ENABLE_VIDEO_RGB
    surfaceView_->SetSurfaceColorkey(colorKey_);
#else
    surfaceView_->SetVideoResizeMode(resizeMode_);
    surfaceView_->SetRotation(rotation_);
#endif
    Surface *surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(454, 454); // 454:source width, 454: source height
    surface->SetQueueSize(BUFFER_QUEUE_SIZE);
    surface->SetFormat(PIXEL_FMT_YCBCR_420_SP);

    // jpeg decoder output buffer size calculation formula
    int32_t ySize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) * VIDEO_HEIGHT;
    int32_t uvSize = ALIGN_BYTE(VIDEO_WIDTH, JPEG_WIDTH_BYTE_ALIGNMENT) *
        ALIGN_BYTE(VIDEO_HEIGHT, JPEG_HEIGHT_BYTE_ALIGNMENT) / JPEG_HEIGHT_DIVISOR;
    int32_t bufferSize = ySize + uvSize;
    surface->SetSize(bufferSize);
    if (videoPlay_ == nullptr) {
        std::string uri("/user/video/panda.mp4");
        videoPlay_ = new VideoPlayerService(uri, false);
        if (videoPlay_ == nullptr) {
            GRAPHIC_LOGE("videoPlay_ is nullptr\n");
            return;
        }
        videoPlay_->SetUILabelButton(button1_);
#ifdef SUPPORT_OHOSFWK
        g_videoPlay = videoPlay_;
        registerNotifyScreenStatus(ScreenStatusNotify);
#endif
    }
    videoPlay_->SetSurface(surface);
}
}
