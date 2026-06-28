/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ShineOnClickView
 * Create: 2025-11-18
 */


#include "ui_test_shine_on_click/ui_test_shine_on_click.h"
#include "components/ui_view.h"

namespace OHOS {
void UITestShineOnClick::SetUp()
{
    if (shineOnClickView_ == nullptr) {
        shineOnClickView_ = new ShineOnClickView();
        shineOnClickView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        shineOnClickView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        shineOnClickView_->InitView();
    }
}

void UITestShineOnClick::TearDown()
{
    delete shineOnClickView_;
    shineOnClickView_ = nullptr;
}

const UIView *UITestShineOnClick::GetTestView()
{
    return shineOnClickView_;
}
}
