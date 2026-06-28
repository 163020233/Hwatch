/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestSphere
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */
#ifdef _WIN32
#include <cmath>
#endif
#include "ui_test_sphere.h"
#include "common/image_cache_manager.h"

namespace OHOS {
constexpr uint8_t SPHERE_MODEL_MAX = 3;

static const char* g_sphereModelPath[SPHERE_MODEL_MAX] = {
    RES_PATH"sphere_model_150.bin",
    RES_PATH"sphere_model_160.bin",
    RES_PATH"sphere_model_233.bin"
};

void UITestSphere::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    ImageInfo* texture = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"earth.bin");
    if (texture == nullptr) {
        GRAPHIC_LOGE("Load texture failed!");
        return;
    }

    if (sphere_ == nullptr) {
        sphere_ = new UI3DSphere();
    }
    sphere_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    sphere_->SetTouchable(true);
    sphere_->SetDraggable(true);
    sphere_->SetOnDragListener(this);
    sphere_->SetTexture(texture);
    sphere_->SetModel(g_sphereModelPath[modelIndex_]);
    container_->Add(sphere_);

    if (switchBtn_ == nullptr) {
        switchBtn_ = new UILabelButton();
    }
    switchBtn_->SetPosition(203, 400, 60, 50); // 203, 400, 60, 50: coordinate
    switchBtn_->SetOnClickListener(this);
    switchBtn_->SetText("Switch");
    container_->Add(switchBtn_);
}

bool UITestSphere::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool UITestSphere::OnDrag(UIView& view, const DragEvent& event)
{
    if (sphere_ == nullptr) {
        return false;
    }

    float yOffset = event.GetCurrentPos().y - event.GetPreLastPoint().y;
    if (yOffset != 0) {
        xAngle += yOffset;
        sphere_->RotateAroundXAxis(xAngle);
    }

    float xOffset = event.GetCurrentPos().x - event.GetPreLastPoint().x;
    if (xOffset != 0) {
        yAngle += xOffset;
        sphere_->RotateAroundYAxis(yAngle);
    }

    if (yOffset != 0 && xOffset != 0) {
        float zOffset = sqrt(yOffset * yOffset + xOffset * xOffset);
        zAngle += zOffset;
        sphere_->RotateAroundZAxis(zAngle);
    }
    return true;
}

bool UITestSphere::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}

bool UITestSphere::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == switchBtn_) {
        modelIndex_ = modelIndex_ + 1;
        if (modelIndex_ >= SPHERE_MODEL_MAX) {
            modelIndex_ = 0;
        }
        sphere_->SetModel(g_sphereModelPath[modelIndex_]);
        printf("Switch to model: %s\n", g_sphereModelPath[modelIndex_]);
    }
    return true;
}

void UITestSphere::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    sphere_ = nullptr;
    switchBtn_ = nullptr;
    modelIndex_ = 0;
    xAngle = 0.0;
    yAngle = 0.0f;
    zAngle = 0.0f;
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"earth.bin");
}

const UIView* UITestSphere::GetTestView()
{
    return container_;
}
}