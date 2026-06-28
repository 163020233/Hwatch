/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoFragment
 * Create: 2026-02-12
 */

#include "dev_rotate_demo/DevRotateDemoFragment.h"
#include "dev_rotate_demo/DevRotateDemoPresenter.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "RotationManager.h"
#include "components/root_view.h"

namespace OHOS {
DevRotateDemoFragment::DevRotateDemoFragment(ColorType txtColor, ColorType bgColor, uint16_t curPage, uint16_t dstPage)
    : color_(txtColor), bgColor_(bgColor), curPage_(curPage), dstPage_(dstPage)
{}

DevRotateDemoFragment::~DevRotateDemoFragment()
{
    OnDestroyView();
}

void DevRotateDemoFragment::OnRotationChanged(AppRotationType rot)
{
    if (rot != AppRotationType::ROTATION_0) {
        RotateLayout();
    } else {
        NormalLayout();
    }
}

bool DevRotateDemoFragment::OnClick(UIView &view, const ClickEvent &event)
{
    if (title_ == nullptr) {
        GRAPHIC_LOGE("views are null");
        return false;
    }

    if (&view == exitBtn_) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (&view == transNoAni_) {
        NativeAbility::GetInstance().SwitchPageInSlice(dstPage_);
    } else if (&view == transAni_) {
        NativeAbility::GetInstance().SwitchPageInSlice(dstPage_, TransitionType::TRANSITION_SLIDE_OUT_RIGHT);
    } else if (&view == transBack_) {
        NativeAbility::GetInstance().SwitchPageInSlice(dstPage_, TransitionType::TRANSITION_INVALID, true);
    } else if (&view == rotate0_) {
        RotationManager::GetInstance().SetDisplayRotation(AppRotationType::ROTATION_0);
    } else if (&view == rotate90_) {
        RotationManager::GetInstance().SetDisplayRotation(AppRotationType::ROTATION_90);
    } else {
        GRAPHIC_LOGE("view is invalid!");
        return false;
    }
    return true;
}

bool DevRotateDemoFragment::InitViews()
{
    title_ = new (std::nothrow) UILabel;
    if (title_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    exitBtn_ = new (std::nothrow) UILabelButton;
    if (exitBtn_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    transNoAni_ = new (std::nothrow) UILabelButton;
    if (transNoAni_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    transAni_ = new (std::nothrow) UILabelButton;
    if (transAni_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    transBack_ = new (std::nothrow) UILabelButton;
    if (transBack_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    rotate0_ = new (std::nothrow) UILabelButton;
    if (rotate0_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    rotate90_ = new (std::nothrow) UILabelButton;
    if (rotate90_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }
    return true;
}

void DevRotateDemoFragment::DeinitViews()
{
    fragmentView_.RemoveAll();
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (exitBtn_ != nullptr) {
        delete exitBtn_;
        exitBtn_ = nullptr;
    }
    if (transNoAni_ != nullptr) {
        delete transNoAni_;
        transNoAni_ = nullptr;
    }
    if (transAni_ != nullptr) {
        delete transAni_;
        transAni_ = nullptr;
    }
    if (transBack_ != nullptr) {
        delete transBack_;
        transBack_ = nullptr;
    }
    if (rotate0_ != nullptr) {
        delete rotate0_;
        rotate0_ = nullptr;
    }
    if (rotate90_ != nullptr) {
        delete rotate90_;
        rotate90_ = nullptr;
    }
}

void DevRotateDemoFragment::InitText(const char *text, ColorType txtColor, ColorType bColor)
{
    if (title_ == nullptr) {
        GRAPHIC_LOGE("views are null!");
        return;
    }

    title_->SetText(text);
    title_->SetTextColor(txtColor);
    title_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    exitBtn_->SetText("退出");
    exitBtn_->SetOnClickListener(this);
    exitBtn_->SetTextColor(txtColor);
    exitBtn_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    transNoAni_->SetText("不带动效切换");
    transNoAni_->SetOnClickListener(this);
    transNoAni_->SetTextColor(txtColor);
    transNoAni_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    transAni_->SetText("带动效切换");
    transAni_->SetOnClickListener(this);
    transAni_->SetTextColor(txtColor);
    transAni_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    transBack_->SetText("跟手切换");
    transBack_->SetOnClickListener(this);
    transBack_->SetTextColor(txtColor);
    transBack_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    rotate0_->SetText("旋转0");
    rotate0_->SetOnClickListener(this);
    rotate0_->SetTextColor(txtColor);
    rotate0_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);

    rotate90_->SetText("旋转90");
    rotate90_->SetOnClickListener(this);
    rotate90_->SetTextColor(txtColor);
    rotate90_->SetStyle(STYLE_BACKGROUND_COLOR, bColor.full);
}

void DevRotateDemoFragment::NormalLayout()
{
    if (title_ == nullptr) {
        GRAPHIC_LOGE("views are null!");
        return;
    }

    fragmentView_.RemoveAll();
    fragmentView_.Resize(RootView::GetInstance()->GetWidth(), RootView::GetInstance()->GetHeight());
    constexpr size_t len = 128;
    char str[len];
    if (snprintf_s(str, len, len, "page %u normal angle.", curPage_) < 0) {
        GRAPHIC_LOGE("encoding error");
        return;
    }
    InitText(str, color_, bgColor_);

    UIView *const views[] = {title_, exitBtn_, transNoAni_, transAni_, transBack_, rotate0_, rotate90_};
    constexpr int n = sizeof(views) / sizeof(views[0]);
    constexpr float x = 0.05;
    constexpr float y = 0.05;
    constexpr float w = 0.9;
    constexpr float h = 0.9 / n;
    for (size_t i = 0; i < sizeof(views) / sizeof(views[0]); ++i) {
        fragmentView_.Add(views[i]);
        views[i]->SetPositionPercent(x, y + i * h, w, h);
    }
    fragmentView_.Invalidate();
}

void DevRotateDemoFragment::RotateLayout()
{
    if (title_ == nullptr) {
        GRAPHIC_LOGE("container is null!");
        return;
    }

    fragmentView_.RemoveAll();
    fragmentView_.Resize(RootView::GetInstance()->GetWidth(), RootView::GetInstance()->GetHeight());

    constexpr size_t len = 128;
    char str[len];
    if (snprintf_s(str, len, len, "page %u rotate angle.", curPage_) < 0) {
        GRAPHIC_LOGE("encoding error");
        return;
    }
    InitText(str, color_, bgColor_);
    UIView *const views[] = {title_, exitBtn_, transNoAni_, transAni_, transBack_, rotate0_, rotate90_};
    for (size_t i = 0; i < sizeof(views) / sizeof(views[0]); ++i) {
        fragmentView_.Add(views[i]);
    }

    constexpr int n = sizeof(views) / sizeof(views[0]);
    constexpr int m = 1 + n / 2;  // 2
    constexpr float x = 0.05;
    constexpr float y = 0.05;
    constexpr float w = 0.45;
    constexpr float h = 0.9 / m;
    views[0]->SetPositionPercent(x, y, w * 2, h);  // 2
    for (int i = 1; i < n; i = i + 2) {            // 2: step
        int a = (i + 1) / 2;                       // 2: half
        views[i]->SetPositionPercent(x, y + a * h, w, h);
        if (i + 1 >= n) {
            break;
        }
        views[i + 1]->SetPositionPercent(x + w, y + a * h, w, h);
    }
    fragmentView_.Invalidate();
}

void DevRotateDemoFragment::OnCreateView(void *data)
{
    if (!InitViews()) {
        DeinitViews();
        GRAPHIC_LOGE("failed init views");
        return;
    }

    OnRotationChanged(RotationManager::GetInstance().GetCurContentRotation());
}

void DevRotateDemoFragment::OnDestroyView()
{
    DeinitViews();
    color_.full = 0;
    bgColor_.full = 0;
    curPage_ = 0;
    dstPage_ = 0;
}
}  // namespace OHOS
