/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestSphere
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_TEST_SPHERE_H
#define UI_TEST_SPHERE_H

#include "ui_test.h"
#include "components/ui_3d_sphere.h"
#include "components/ui_label_button.h"

namespace OHOS {
class UITestSphere : public UITest, public UIView::OnDragListener, public UIView::OnClickListener {
public:
    UITestSphere() {}
    ~UITestSphere() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIViewGroup* container_ = nullptr;
    UI3DSphere* sphere_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;
    float xAngle = 0.0f;
    float yAngle = 0.0f;
    float zAngle = 0.0f;
    uint8_t modelIndex_ = 0;
};
}
#endif