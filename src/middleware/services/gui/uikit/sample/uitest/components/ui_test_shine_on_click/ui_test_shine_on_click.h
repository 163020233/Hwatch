/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ShineOnClickView
 * Create: 2025-11-18
 */

#ifndef UI_TEST_SHINE_ON_CLICK_H
#define UI_TEST_SHINE_ON_CLICK_H

#include "ui_test.h"
#include "shine_on_click_view.h"

namespace OHOS {
class UITestShineOnClick : public UITest {
public:
    UITestShineOnClick() {}
    ~UITestShineOnClick() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView *GetTestView() override;

private:
    ShineOnClickView *shineOnClickView_ = nullptr;
};
}
#endif // UI_TEST_SHINE_ON_CLICK_H