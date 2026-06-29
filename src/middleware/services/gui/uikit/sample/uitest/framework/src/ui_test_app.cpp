/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Test application
 * Create: 2025-09-20
 */

#include "ui_test_app.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "dfx/event_injector.h"
#include "ui_test.h"
#include "ui_test_group.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif
#include "gfx_utils/style.h"

namespace OHOS {
UITestApp* UITestApp::GetInstance()
{
    static UITestApp instance;
    return &instance;
}

void UITestApp::Start()
{
    UIViewGroup* rootView = RootView::GetInstance();
    rootView->SetPosition(0, 0);
    rootView->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rootView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    rootView->RemoveAll();
    Init();
}

void UITestApp::Init()
{
    if ((mainList_ == nullptr) && (adapter_ == nullptr)) {
        mainList_ = new(std::nothrow) UIList(UIList::VERTICAL);
        if (mainList_ == nullptr) {
            GRAPHIC_LOGE("UITestApp::new mainList_ failed.");
            return;
        }
        mainList_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        mainList_->SetThrowDrag(true);
        mainList_->SetReboundSize(100); // 100: rebound size
        mainList_->SetElastic(true);
        adapter_ = new(std::nothrow) TestCaseListAdapter();
        if (adapter_ == nullptr) {
            GRAPHIC_LOGE("UITestApp::new adapter_ failed.");
            return;
        }
        UITestGroup::GetInstance()->SetUpTestCase();
        mainList_->SetAdapter(adapter_);
    }
    UIViewGroup* rootView = RootView::GetInstance();
    rootView->Add(mainList_);
    rootView->Invalidate();
}

UITestApp::~UITestApp()
{
    if (mainList_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete mainList_;
        mainList_ = nullptr;
    }
}
} // namespace OHOS
