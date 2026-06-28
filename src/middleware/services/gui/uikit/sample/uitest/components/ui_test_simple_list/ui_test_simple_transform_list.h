/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-07
 */

#ifndef UI_TEST_SIMPLE_TRANSFORM_LIST_H
#define UI_TEST_SIMPLE_TRANSFORM_LIST_H

#include "components/ui_slider.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_animator.h"
#include "components/ui_label_ext.h"
#include "ui_test.h"

namespace OHOS {
class UITestSimpleTransformList : public UITest, UIView::OnClickListener, ListScrollListener {
public:
    UITestSimpleTransformList() : list_(nullptr) {}
    ~UITestSimpleTransformList() override {}
    UITestSimpleTransformList(const UITestSimpleTransformList &) = delete;
    UITestSimpleTransformList &operator=(const UITestSimpleTransformList &) = delete;
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    void OnItemSelected(int16_t index, UIView* view) override;

private:
    void LoadImg();
    void ReleaseImg();
    UISimpleTransformList* list_;
    List<Contents*> data_;
    UIView* lastSelectedView_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_SIMPLE_TRANSFORM_LIST_H

