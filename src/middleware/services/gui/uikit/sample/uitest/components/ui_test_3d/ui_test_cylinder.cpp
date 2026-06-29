/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestCylinder
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#include "ui_test_cylinder.h"
#include "common/image_cache_manager.h"

namespace OHOS {
void UITestCylinder::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    ImageInfo* texture = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"calender_texture.bin");
    if (texture == nullptr) {
        GRAPHIC_LOGE("Load texture failed!");
        return;
    }

    if (cylinder_ == nullptr) {
        cylinder_ = new UI3DCylinder();
    }
    cylinder_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    cylinder_->SetTouchable(true);
    cylinder_->SetDraggable(true);
    cylinder_->SetOnDragListener(this);
    cylinder_->SetTexture(texture);
    cylinder_->SetModel(RES_PATH"cylinder_model.bin");
    cylinder_->SetStartY(0);
    container_->Add(cylinder_);
}

bool UITestCylinder::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool UITestCylinder::OnDrag(UIView& view, const DragEvent& event)
{
    if (cylinder_ == nullptr) {
        return false;
    }
    int16_t prevY = event.GetPreLastPoint().y;
    int16_t y = event.GetCurrentPos().y;
    int16_t yOffset = prevY - y;
    cylinder_->SetStartY(cylinder_->GetStartY() + yOffset);
    return true;
}

bool UITestCylinder::OnDragEnd(UIView& view, const DragEvent& event)
{
    if (cylinder_ == nullptr) {
        return false;
    }
    return true;
}

void UITestCylinder::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    cylinder_ = nullptr;
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"calender_texture.bin");
}

const UIView* UITestCylinder::GetTestView()
{
    return container_;
}
}