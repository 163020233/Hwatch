/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie Test
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_lottie.h"
#include "common/image_cache_manager.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
constexpr uint8_t FILE_MAX = 8;
static const char* g_filePath[FILE_MAX] = {
    RES_PATH"2016_lottie.bin",
    RES_PATH"singing-contract.bin",
    RES_PATH"girl-cycling-in-autumn.bin",
    RES_PATH"loading.bin",
    RES_PATH"ripple_loading_animation.bin",
    RES_PATH"starts_transparent.bin",
    RES_PATH"fingerprint_success.bin",
    RES_PATH"intelia_logo_animation.bin",
};

static const uint8_t g_playCounts[] = {1, 3, 0};

void UITestLottie::SetUp()
{
    fileIndex_ = 0;
    playCountIndex_ = 0;

    if (container_ == nullptr) {
        container_ = new UIScrollView();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    UIView* view = new UIView();
    view->SetPosition(0, 0, 800, 800); // 800: width and height
    container_->Add(view);

    if (lottieView_ == nullptr) {
        lottieView_ = new UILottView();
        lottieView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        lottieView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        lottieView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        if (lottieView_->SetSrc(g_filePath[fileIndex_])) {
            lottieView_->Start();
        }
        fileIndex_++;
    }
    container_->Add(lottieView_);

    InitBtns();

    stopListener_ = new TestLottAnimStopListener();
    lottieView_->SetOnLottAnimStopListener(stopListener_);
    lottieView_->SetPlayCount(g_playCounts[playCountIndex_]);
}

void UITestLottie::InitBtns()
{
    startBtn_ = new UILabelButton();
    startBtn_->SetPosition(87, 380, 70, 30); // 87, 380, 70, 30: coordinate
    startBtn_->SetOnClickListener(this);
    startBtn_->SetText("Start");
    container_->Add(startBtn_);

    switchBtn_ = new UILabelButton();
    switchBtn_->SetPosition(157, 380, 70, 30); // 157, 380, 70, 30: coordinate
    switchBtn_->SetOnClickListener(this);
    switchBtn_->SetText("Switch");
    container_->Add(switchBtn_);

    repeatBtn_ = new UILabelButton();
    repeatBtn_->SetPosition(227, 380, 70, 30); // 227, 380, 70, 30: coordinate
    repeatBtn_->SetOnClickListener(this);
    repeatBtn_->SetText("Repeat");
    container_->Add(repeatBtn_);

    transformBtn_ = new UILabelButton();
    transformBtn_->SetPosition(297, 380, 80, 30); // 297, 380, 80, 30: coordinate
    transformBtn_->SetOnClickListener(this);
    transformBtn_->SetText("Transform");
    container_->Add(transformBtn_);

    playCountBtn_ = new UILabelButton();
    playCountBtn_->SetPosition(87, 420, 70, 30); // 87, 420, 70, 30: coordinate
    playCountBtn_->SetOnClickListener(this);
    playCountBtn_->SetText("Cnt");
    container_->Add(playCountBtn_);

    reverseBtn_ = new UILabelButton();
    reverseBtn_->SetPosition(157, 420, 70, 30); // 157, 420, 70, 30: coordinate
    reverseBtn_->SetOnClickListener(this);
    reverseBtn_->SetText("Reverse");
    container_->Add(reverseBtn_);

    frameInfoBtn_ = new UILabelButton();
    frameInfoBtn_->SetPosition(227, 420, 90, 30); // 227, 420, 90, 30: coordinate
    frameInfoBtn_->SetOnClickListener(this);
    frameInfoBtn_->SetText("FrameNo");
    container_->Add(frameInfoBtn_);
}

void UITestLottie::TearDown()
{
    DeleteChildren(container_);
    lottieView_ = nullptr;
    startBtn_ = nullptr;
    switchBtn_ = nullptr;
    repeatBtn_ = nullptr;
    transformBtn_ = nullptr;
    playCountBtn_ = nullptr;
    reverseBtn_ = nullptr;
    frameInfoBtn_ = nullptr;

    if (stopListener_ != nullptr) {
        delete stopListener_;
        stopListener_ = nullptr;
    }

    container_ = nullptr;
}

bool UITestLottie::OnClick(UIView& view, const ClickEvent& event)
{
    if (lottieView_ == nullptr) {
        return false;
    }
    if (&view == startBtn_) {
        lottieView_->Start();
    } else if (&view == switchBtn_) {
        lottieView_->SetSrc(g_filePath[fileIndex_]);
        fileIndex_++;
        if (fileIndex_ == FILE_MAX) {
            fileIndex_ = 0;
        }
        lottieView_->Start();
    } else if (&view == repeatBtn_) {
        bool repeat = !lottieView_->IsRepeat();
        lottieView_->SetRepeat(repeat);
        printf("SetRepeat to %d\n", repeat);
    } else if (&view == transformBtn_) {
        if (lottieView_->IsTransInvalid()) {
            Vector2<float> pivot = {lottieView_->GetWidth() / 2.0f, lottieView_->GetHeight() / 2.0f}; // 2: divider
            lottieView_->Rotate(90.0f, pivot);
            lottieView_->Scale(Vector2<float>{0.5f, 0.5f}, pivot);
            printf("Rotate 90 degree and scale by [0.5, 0.5]\n");
        } else {
            Matrix4<float> identity;
            lottieView_->GetTransformMap().SetMatrix(identity);
            lottieView_->Invalidate();
            printf("Reset transformation!\n");
        }
    } else if (&view == playCountBtn_) {
        playCountIndex_ = (playCountIndex_ + 1) % (sizeof(g_playCounts) / sizeof(g_playCounts[0]));
        lottieView_->SetPlayCount(g_playCounts[playCountIndex_]);
        printf("SetPlayCount to %d\n", g_playCounts[playCountIndex_]);
    } else if (&view == reverseBtn_) {
        bool reverse = !lottieView_->IsReverse();
        lottieView_->SetReverse(reverse);
        printf("SetReverse to %d\n", reverse);
    } else if (&view == frameInfoBtn_) {
        float currentFrame = lottieView_->GetCurrentFrame();
        printf("FrameNo: %.2f\n", currentFrame);
    }
    return true;
}

const UIView* UITestLottie::GetTestView()
{
    return container_;
}

void UITestLottie::TestLottAnimStopListener::OnLottAnimStop(UILottView& view)
{
    printf("Lott Anim Stop!\n");
}
}
