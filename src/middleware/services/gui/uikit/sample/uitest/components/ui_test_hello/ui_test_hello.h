/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: hello
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef UI_TEST_HELLO_H
#define UI_TEST_HELLO_H

#include <random>
#include <string>
#include <ctime>
#include "animator/animator.h"
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "components/ui_lite_surface_view.h"
#include "ui_test.h"
#include "video_player_service.h"

namespace OHOS {
class SpeedAnimatorCallback : public AnimatorCallback {
public:
    explicit SpeedAnimatorCallback(UILabel* label) : label_(label) {}
    ~SpeedAnimatorCallback() {}

    virtual void Callback(UIView* view)
    {
        if (label_ != nullptr) {
            srand(time(NULL));
            uint8_t speed = static_cast<uint8_t>(rand() % 50); // 50: speed max value
            std::string speedString = std::to_string(speed);
            speedString += "KM/h";
            label_->SetText(speedString.c_str());
            label_->Invalidate();
        }

        if (img_ != nullptr) {
            x = x > 800 ? 0 : x + 4; // 800: max width 4: step
            img_->Translate({x, 0});
            img_->Invalidate();
        }
    }

    void SetImageObj(UIImageView* image)
    {
        img_ = image;
    }

private:
    UILabel* label_ = nullptr;
    UIImageView* img_ = nullptr;
    int16_t x = 0;
};

class UITestHello : public UITest {
public:
    UITestHello() {}
    ~UITestHello() {}
    void SetUp() override;
    void TearDown() override;

    const UIView* GetTestView() override;
    void CreateVideo();
    void DestoryVideo();

private:
    void CheckVideoPath();
    UIImageView* backgroudImage_ = nullptr;
    UILabel* speedLabel_ = nullptr;
    Animator* animator_ = nullptr;
    SpeedAnimatorCallback* callback_ = nullptr;
    UIScrollView* container_ = nullptr;
    UILiteSurfaceView *surfaceView_ = nullptr;
    ColorType colorKey_;
    VideoPlayerService *videoPlay_ = nullptr;
    UILabel *noResLabel_ = nullptr;
    UIImageView* smallImg_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_BUTTON_H
