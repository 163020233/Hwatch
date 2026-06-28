/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PauseReasonFragment
 * Create: 2026-03-18
 */

#include "pause_reason_test/PauseReasonFragment.h"
#include "notification_manager.h"
#include "NativeAbility.h"

namespace OHOS {
namespace {
class ClickNotify;
ClickNotify *g_click = nullptr;
UILabelButton *g_label = nullptr;
UIViewGroup *g_group = nullptr;

class ClickNotify : public UIView::OnClickListener {
public:
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        NotificationManager::GetInstance()->StopNotify();
        return true;
    }
};

void DestroyNotify()
{
    if (g_group != nullptr) {
        g_group->RemoveAll();
        delete g_group;
        g_group = nullptr;
    }

    if (g_label != nullptr) {
        delete g_label;
        g_label = nullptr;
    }

    if (g_click != nullptr) {
        delete g_click;
        g_click = nullptr;
    }
}

UIViewGroup *CreateNotify()
{
    g_label = new (std::nothrow) UILabelButton;
    g_group = new (std::nothrow) UIViewGroup;
    g_click = new (std::nothrow) ClickNotify;

    if (g_label == nullptr || g_group == nullptr || g_click == nullptr) {
        delete g_label;
        g_label = nullptr;
        delete g_group;
        g_group = nullptr;
        delete g_click;
        g_click = nullptr;
        return nullptr;
    }

    g_group->SetPosition(0, 0, RootView::GetInstance()->GetWidth(), RootView::GetInstance()->GetHeight());
    g_label->SetText("弹窗示例");
    g_label->SetOnClickListener(g_click);
    g_group->Add(g_label);
    const float x = 0.2;
    const float y = x;
    const float w = 0.6;
    const float h = w;
    g_label->SetPositionPercent(x, y, w, h);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(DestroyNotify);
    return g_group;
}
}
PauseReasonFragment::PauseReasonFragment(ColorType color, uint16_t dstPage) : color_(color), dstPage_(dstPage) {}

PauseReasonFragment::~PauseReasonFragment()
{
    OnDestroyView();
}

bool PauseReasonFragment::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == btnExit_) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (&view == btnPageSwitch_) {
        NativeAbility::GetInstance().SwitchPageInSlice(dstPage_, TransitionType::TRANSITION_INVALID, true);
    } else if (&view == btnPageSwitchWithAni_) {
        NativeAbility::GetInstance().SwitchPageInSlice(dstPage_, TransitionType::TRANSITION_COVER_RIGHT, true);
    } else if (&view == btnNotify_) {
        (void)NotificationManager::GetInstance()->ShowNotify(CreateNotify, 0, true);
    } else if (&view == btnBack_) {
        NativeAbility::GetInstance().BackToPrevSlicePage(TransitionType::TRANSITION_SLIDE_OUT_RIGHT);
    } else {
        GRAPHIC_LOGE("click error!");
        return false;
    }
    return true;
}

void PauseReasonFragment::OnCreateView(void* data)
{
    if (!InitViews()) {
        DeinitViews();
        return;
    }

    Layout();
}

void PauseReasonFragment::OnDestroyView()
{
    DeinitViews();
}

bool PauseReasonFragment::InitViews()
{
    if (btnExit_ != nullptr) {
        DeinitViews();
    }

    btnExit_ = new (std::nothrow) UILabelButton;
    if (btnExit_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc btnExit!");
        return false;
    }

    btnPageSwitch_ = new (std::nothrow) UILabelButton;
    if (btnPageSwitch_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc btnPageSwitch!");
        return false;
    }

    btnPageSwitchWithAni_ = new (std::nothrow) UILabelButton;
    if (btnPageSwitchWithAni_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc btnPageSwitchWithAni!");
        return false;
    }

    btnNotify_ = new (std::nothrow) UILabelButton;
    if (btnNotify_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc btnNotify!");
        return false;
    }

    btnBack_ = new (std::nothrow) UILabelButton;
    if (btnBack_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc btnBack!");
        return false;
    }
    return true;
}

void PauseReasonFragment::DeinitViews()
{
    fragmentView_.RemoveAll();
    if (btnExit_ != nullptr) {
        delete btnExit_;
        btnExit_ = nullptr;
    }
    if (btnPageSwitch_ != nullptr) {
        delete btnPageSwitch_;
        btnPageSwitch_ = nullptr;
    }
    if (btnPageSwitchWithAni_ != nullptr) {
        delete btnPageSwitchWithAni_;
        btnPageSwitchWithAni_ = nullptr;
    }
    if (btnNotify_ != nullptr) {
        delete btnNotify_;
        btnNotify_ = nullptr;
    }
    if (btnBack_ != nullptr) {
        delete btnBack_;
        btnBack_ = nullptr;
    }
}

void PauseReasonFragment::Layout()
{
    if (btnExit_ == nullptr) {
        GRAPHIC_LOGE("views has not been init!");
        return;
    }
    fragmentView_.RemoveAll();
    fragmentView_.Resize(RootView::GetInstance()->GetWidth(), RootView::GetInstance()->GetHeight());

    InitText(*btnExit_, "退出");
    InitText(*btnPageSwitch_, "页面切换");
    InitText(*btnPageSwitchWithAni_, "页面切换+动效");
    InitText(*btnNotify_, "弹窗");
    InitText(*btnBack_, "返回到前一页");

    const float g = 0.01;
    const float w = 0.8;
    const float x0 = 0.1;
    const float y0 = g;
    UILabelButton *const views[] = {btnExit_, btnPageSwitch_, btnPageSwitchWithAni_, btnNotify_, btnBack_};
    const uint16_t n = sizeof(views) / sizeof(views[0]);
    const float h = (1 - (float(n) + 1.0) * g) / float(n);
    for (uint16_t i = 0; i < n; ++i) {
        fragmentView_.Add(views[i]);
        views[i]->SetPositionPercent(x0, y0 + i * (h + g), w, h);
    }
    fragmentView_.Invalidate();
}

void PauseReasonFragment::InitText(UILabelButton &btn, const char *txt)
{
    btn.SetText(txt);
    btn.SetOnClickListener(this);
    btn.SetStyle(STYLE_BACKGROUND_COLOR, color_.full);
}
} // namespace OHOS
