/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: hello
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#include "ui_test_hello.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "components/root_view.h"
#ifdef SUPPORT_OHOSFWK
#include "screennotify/ui_screennotify.h"
#endif
namespace OHOS {
namespace {
    static constexpr int16_t VIDEO_WIDTH = 454;
    static constexpr int16_t VIDEO_HEIGHT = 454;
    static constexpr int16_t JPEG_WIDTH_BYTE_ALIGNMENT = 128;
    static constexpr int16_t STRIDE_ALIGNMENT_VALUE = 128;
    static constexpr int16_t BUFFER_QUEUE_SIZE = 3;
    static constexpr int16_t JPEG_HEIGHT_BYTE_ALIGNMENT = 16;
    static constexpr int16_t JPEG_HEIGHT_DIVISOR = 2;
}

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

void UITestHello::SetUp()
{
    RootView::GetInstance()->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestHello::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    backgroudImage_ = nullptr;
    smallImg_ = nullptr;
    speedLabel_ = nullptr;

    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }

    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    DestoryVideo();
}

const UIView* UITestHello::GetTestView()
{
    if (backgroudImage_ == nullptr) {
        backgroudImage_ = new UIImageView();
        backgroudImage_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        ImageInfo* imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"background800_480.bin");
        if (imgInfo == nullptr) {
            GRAPHIC_LOGE("LoadSingleRes is failed\n");
            return nullptr;
        }
        backgroudImage_->SetSrc(imgInfo);
        container_->Add(backgroudImage_);
    }

    if (speedLabel_ == nullptr) {
        speedLabel_ = new UILabel;
        speedLabel_->SetPosition(40, 200, 240, 80); // 40: x, 200: y, 240: w, 80: h
        speedLabel_->SetText("00KM/h");
        speedLabel_->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
        speedLabel_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        speedLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 50); // 50: Font size.
        speedLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        container_->Add(speedLabel_);
    }

    if (smallImg_ == nullptr) {
        smallImg_ = new UIImageView();
        smallImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
        smallImg_->SetPosition(0, 180); // 180:y
        ImageInfo* imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"haha.bin");
        if (imgInfo == nullptr) {
            GRAPHIC_LOGE("LoadSingleRes is failed\n");
            return nullptr;
        }
        smallImg_->SetSrc(imgInfo);
        container_->Add(smallImg_);
    }

    callback_ = new SpeedAnimatorCallback(speedLabel_);
    callback_->SetImageObj(smallImg_);
    animator_ = new Animator(callback_, speedLabel_, 100, true); // 100:period
    animator_->Start();
    CreateVideo();
    return container_;
}

void UITestHello::CheckVideoPath()
{
    if (noResLabel_ == nullptr) {
        noResLabel_ = new UILabel();
        if (noResLabel_ == nullptr) {
            GRAPHIC_LOGE("label is nullptr\n");
            return;
        }
    }
    noResLabel_->SetPosition(60, 227, 330, 70); // 60, 227, 330, 70: Left, right, width, height.
    noResLabel_->SetText("please upload video resource.");
    noResLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    noResLabel_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    noResLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 25); // 25: Font size.
    noResLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(noResLabel_);
    GRAPHIC_LOGE("no video resource\n");
    return;
}

void UITestHello::CreateVideo()
{
    char filePath[PATH_MAX];
    if (realpath("/user/video/panda.mp4", filePath) == nullptr) {
        CheckVideoPath();
        return;
    }

    if (surfaceView_ == nullptr) {
        surfaceView_ = new UILiteSurfaceView();
        if (surfaceView_ == nullptr) {
            GRAPHIC_LOGE("surfaceView is nullptr\n");
            return;
        }
    }
    colorKey_.full = 0xff202020;
    surfaceView_->SetViewId("sur");
    surfaceView_->SetPosition(273, 13, VIDEO_WIDTH, VIDEO_HEIGHT); // 273:x 13:y
    surfaceView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    surfaceView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
#if !ENABLE_VIDEO_RGB
    surfaceView_->SetSurfaceColorkey(colorKey_);
#endif
    container_->Add(surfaceView_);

    Surface *surface = surfaceView_->GetSurface();
    surface->SetStrideAlignment(STRIDE_ALIGNMENT_VALUE);
    surface->SetWidthAndHeight(VIDEO_WIDTH, VIDEO_HEIGHT);
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
        videoPlay_->SetSurface(surface);
#ifdef SUPPORT_OHOSFWK
        g_videoPlay = videoPlay_;
        registerNotifyScreenStatus(ScreenStatusNotify);
#endif
        videoPlay_->Play();
    }
}

void UITestHello::DestoryVideo()
{
#ifdef SUPPORT_OHOSFWK
    g_videoPlay = nullptr;
    deregisterNotifyScreenStatus(ScreenStatusNotify);
#endif
    if (noResLabel_ != nullptr) {
        delete noResLabel_;
        noResLabel_ = nullptr;
    }

    if (videoPlay_ != nullptr) {
        videoPlay_->Stop();
        delete videoPlay_;
        videoPlay_ = nullptr;
    }
}
}  // namespace OHOS
