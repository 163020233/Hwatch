/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestVideoAYUV
 * Author: Hisi Graphic Team
 * Created: 2025-11
 */

#include "securec.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
#include "ui_test_video_ayuv.h"

namespace OHOS {
static constexpr const char* VIDEO_PATH = "/user/video/ayuv.mp4";
static constexpr int16_t VIDEO_WIDTH = 466;
static constexpr int16_t VIDEO_HEIGHT = 466;

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

bool UITestVideoAYUV::InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text)
{
    button = new UILabelButton();
    if (button == nullptr) {
        printf("new button failed!\n");
        return false;
    }
    button->SetPosition(x, y);
    button->Resize(80, 40); // 80: width, 40: height
    button->SetText(text);
    button->SetTextColor(Color::Silver());
    button->SetOnClickListener(this);
    container_->Add(button);
    return true;
}

void UITestVideoAYUV::SetUp()
{
    container_ = new UIScrollView();
    if (container_ == nullptr) {
        printf("new container_ failed!\n");
        return;
    }
    container_->SetThrowDrag(true);
    container_->Resize(VIDEO_WIDTH, VIDEO_HEIGHT);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    char filePath[PATH_MAX];
    if (realpath(VIDEO_PATH, filePath) == nullptr) {
        printf("no video resource: %s\n", VIDEO_PATH);
        return;
    }

    surfaceView_ = new UILiteSurfaceView();
    if (surfaceView_ == nullptr) {
        printf("surfaceView is nullptr\n");
        return;
    }
    surfaceView_->SetViewId("sur");
    surfaceView_->SetPosition(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    surfaceView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(surfaceView_);

    Surface *surface = surfaceView_->GetSurface();
    if (surface == nullptr) {
        printf("surface is nullptr!\n");
        return;
    }
    if (videoPlay_ == nullptr) {
        std::string uri(VIDEO_PATH);
        videoPlay_ = new VideoPlayerService(uri, false);
        if (videoPlay_ == nullptr) {
            printf("videoPlay_ is nullptr\n");
            return;
        }
        videoPlay_->SetSurface(surface);
        videoPlay_->SetUILabelButton(button1_);
#ifdef SUPPORT_OHOSFWK
        g_videoPlay = videoPlay_;
        registerNotifyScreenStatus(ScreenStatusNotify);
#endif
    }

    // 130: x, 360: y; 240: x, 360: y
    if (!InitButton(button1_, 130, 360, "Play") || !InitButton(button2_, 240, 360, "Stop")) {
        return;
    }
}

void UITestVideoAYUV::TearDown()
{
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = nullptr;
    deregisterNotifyScreenStatus(ScreenStatusNotify);
#endif

    if (videoPlay_ != nullptr) {
        videoPlay_->Stop();
        delete videoPlay_;
        videoPlay_ = nullptr;
    }

    UIViewGroup* group = dynamic_cast<UIViewGroup*>(container_);
    UIViewGroup::RemoveAndDeleteAllRecursively(group);
    container_ = nullptr;
    button1_ = nullptr;
    button2_ = nullptr;
    surfaceView_ = nullptr;
}

const UIView* UITestVideoAYUV::GetTestView()
{
    return container_;
}

bool UITestVideoAYUV::OnClick(UIView& view, const ClickEvent& event)
{
    int32_t ret = 0;
    if (videoPlay_ == nullptr) {
        printf("videoPlay_ is nullptr!\n");
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
            printf("video is not played\n");
            return false;
        }
        videoPlay_->Stop();
    }
    return true;
}
}
