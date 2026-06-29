/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialSphereView
 * Author:
 * Create: 2025-10-09
 */

#include "main/dial/DialSphereView.h"

namespace OHOS {
namespace {
constexpr uint8_t MODEL_HEADER_SIZE = 16;
} // anonymous namespace

class DialSphereView::DialSphereCallback : public AnimatorCallback {
public:
    DialSphereCallback(DialSphereView &sphere) : sphere_(sphere) {}
    ~DialSphereCallback() override {}

    void Callback(UIView* view) override
    {
        if (view == nullptr) {
            return;
        }

        tick_ = (tick_ + 1) % total_;
        sphere_.RotateAroundYAxis(float(tick_) / total_ * 360.0); // 360.0: total longitude
    }

private:
    DialSphereView &sphere_;
    int32_t tick_ = 0;
    const int32_t total_ = 500; // 500: 500 ticks, one tick is approximately 16.6ms.
}; // class DialSphereView::DialSphereCallback

DialSphereView::~DialSphereView()
{
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }

    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
}

void DialSphereView::Start()
{
    if (animator_ == nullptr) {
        callback_ = new DialSphereCallback(*this);
        if (callback_ == nullptr) {
            GRAPHIC_LOGE("failed to malloc callback when start dialsphere!");
            return;
        }
        animator_ = new Animator(callback_, this, 0, true);
        if (animator_ == nullptr) {
            delete callback_;
            callback_ = nullptr;
            GRAPHIC_LOGE("failed to malloc animator when start dialsphere!");
            return;
        }
        animator_->Start();
    } else {
        animator_->Start();
    }
}

void DialSphereView::Stop()
{
    if (animator_ != nullptr) {
        animator_->Stop();
    }
}

void DialSphereView::Pause()
{
    if (animator_ != nullptr) {
        animator_->Pause();
    }
}

void DialSphereView::Resume()
{
    if (animator_ != nullptr) {
        animator_->Resume();
    }
}

void DialSphereView::SetStartLongitude(float angle)
{
    startLongitude_ = angle;
}

void DialSphereView::SetDialSphereType(DialSphereType type)
{
    type_ = type;
}
} // namespace OHOS
