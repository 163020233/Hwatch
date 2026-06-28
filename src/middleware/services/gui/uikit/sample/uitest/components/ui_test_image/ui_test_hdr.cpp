/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestHDR
 * Create: 2025-12
 */

#include "ui_test_hdr.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "power_display_service.h"
namespace OHOS {
void UITestHDR::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        positionX_ = 0;
        positionY_ = 0;
    }
}

void UITestHDR::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    if (defaultBrightness_ == -1) {
        return;
    }
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return;
    }
    svr->set_brightness(defaultBrightness_);
    defaultBrightness_ = -1;
}

const UIView* UITestHDR::GetTestView()
{
    SetBackgroundColor();
    TestHDR();
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return container_;
    }
    defaultBrightness_ = svr->get_brightness();
    svr->set_brightness(500); // 500: highlight brightness
    return container_;
}

void UITestHDR::SetBackgroundColor()
{
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
}

void UITestHDR::TestHDR()
{
    UIImageView* img = new UIImageView();
    img->SetAutoEnable(true);
    img->SetPosition(0, 0);
    img->SetSrc(RES_PATH"sun.jpg");
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
}
}
#endif