/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestCylinder
 * Author: Hisi Graphic Team
 * Created: 2025-7
 */

#ifndef UI_TEST_CYLINDER_H
#define UI_TEST_CYLINDER_H

#include "ui_test.h"
#include "components/ui_3d_cylinder.h"

namespace OHOS {
class UITestCylinder : public UITest, public UIView::OnDragListener {
public:
    UITestCylinder() {}
    ~UITestCylinder() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

private:
    UIViewGroup* container_ = nullptr;
    UI3DCylinder* cylinder_ = nullptr;
};
}
#endif