/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoFragment
 * Create: 2026-02-12
 */

#include "switch_slice_demo/SwitchSliceDemoFragment.h"
#include "switch_slice_demo/SwitchSliceDemoPresenter.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "components/root_view.h"

namespace OHOS {
SwitchSliceDemoFragment::SwitchSliceDemoFragment(ColorType txtColor, ColorType bgColor, uint16_t curPage, uint16_t brother1Page, uint16_t brother2Page)
    : color_(txtColor), bgColor_(bgColor), curPage_(curPage), brother1Page_(brother1Page), brother2Page_(brother2Page)
{}

SwitchSliceDemoFragment::~SwitchSliceDemoFragment()
{
    OnDestroyView();
}

bool SwitchSliceDemoFragment::OnClick(UIView &view, const ClickEvent &event)
{
    uint16_t nextSlice = curSlice_ + 1;
    if (&view == backAppBtn_) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (&view == backPrevSliceBtn_) {
        NativeAbility::GetInstance().BackToPreSlice();
    } else if (&view == backPrePageBtn_) {
        NativeAbility::GetInstance().BackToPrevSlicePage();
    } else if (&view == switchSliceBtn1_) {
        NativeAbility::GetInstance().SwitchSliceWithCache(nextSlice_, gPageInvalid, TransitionType::TRANSITION_TRANSPARENT_GRADIENT, true);
    } else if (&view == switchSliceBtn2_) {
        NativeAbility::GetInstance().SwitchSliceWithCache(nextSlice_, gPageInvalid, TransitionType::TRANSITION_INVALID, true);
    } else if (&view == switchSliceBtn3_) {
        NativeAbility::GetInstance().SwitchSliceWithData(nextSlice_, nullptr, 0, ExtralConfig{gPageInvalid,
                                                         TransitionType::TRANSITION_TRANSPARENT_GRADIENT, true, false});
    } else if (&view == switchSliceBtn4_) {
        NativeAbility::GetInstance().SwitchSliceWithData(nextSlice_, nullptr, 0, ExtralConfig{gPageInvalid, TransitionType::TRANSITION_INVALID, true, false});
    } else if (&view == switchSliceBtn5_) {
        NativeAbility::GetInstance().SwitchSliceWithCache(nextSlice_, gPageInvalid, TransitionType::TRANSITION_TRANSPARENT_GRADIENT, false);
    } else if (&view == switchSliceBtn6_) {
        NativeAbility::GetInstance().SwitchSliceWithCache(nextSlice_, gPageInvalid, TransitionType::TRANSITION_INVALID, false);
    } else if (&view == switchSliceBtn7_) {
        NativeAbility::GetInstance().SwitchSliceWithData(nextSlice_, nullptr, 0, ExtralConfig{gPageInvalid,
                                                         TransitionType::TRANSITION_TRANSPARENT_GRADIENT, false, false});
    } else if (&view == switchSliceBtn8_) {
        NativeAbility::GetInstance().SwitchSliceWithData(nextSlice_, nullptr, 0, ExtralConfig{gPageInvalid, TransitionType::TRANSITION_INVALID, false, false});
    } else if (&view == switchBrother1Page1_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother1Page_, TransitionType::TRANSITION_COVER_RIGHT, true, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother1Page2_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother1Page_, TransitionType::TRANSITION_INVALID, true, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother1Page3_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother1Page_, TransitionType::TRANSITION_COVER_RIGHT, false, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother1Page4_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother1Page_, TransitionType::TRANSITION_INVALID, false, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother2Page1_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother2Page_, TransitionType::TRANSITION_COVER_RIGHT, true, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother2Page2_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother2Page_, TransitionType::TRANSITION_INVALID, true, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother2Page3_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother2Page_, TransitionType::TRANSITION_COVER_RIGHT, false, &curSlice_, sizeof(curSlice_));
    } else if (&view == switchBrother2Page4_) {
        NativeAbility::GetInstance().SwitchPageInSlice(brother2Page_, TransitionType::TRANSITION_INVALID, false, &curSlice_, sizeof(curSlice_));
    } else if (&view == titleState_) {
        titleState_->SetText("State : ON");
        titleState_->Invalidate();
    } else {
        GRAPHIC_LOGE("view is invalid!");
        return false;
    }
    return true;
}

bool SwitchSliceDemoFragment::InitViews()
{
    fragmentView_.RemoveAll();
    fragmentView_.Resize(RootView::GetInstance()->GetWidth(), RootView::GetInstance()->GetHeight());
    if (!InitBackViews()) {
        GRAPHIC_LOGE("init back views failed!");
        return false;
    }
    if (!InitSwitchSliceViews()) {
        GRAPHIC_LOGE("init switch slice failed!");
        return false;
    }
    if (!InitSwitchPageViews()) {
        GRAPHIC_LOGE("init switch page failed!");
        return false;
    }
    if (!InitStateViews()) {
        GRAPHIC_LOGE("init switch page failed!");
        return false;
    }
    InitBackViewsText();
    InitSwitchSliceViewsText();
    InitSwitchPageViewsText();
    InitStateText();
    BackViewsLayout();
    SwitchSliceViewsLayout();
    SwitchPageViewsLayout();
    StateViewsLayout();
    fragmentView_.Invalidate();
    return true;
}

bool SwitchSliceDemoFragment::InitBackViews()
{
    title_ = new (std::nothrow) UILabel;
    if (title_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    backAppBtn_ = new (std::nothrow) UILabelButton;
    if (backAppBtn_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    backPrevSliceBtn_ = new (std::nothrow) UILabelButton;
    if (backPrevSliceBtn_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    backPrePageBtn_ = new (std::nothrow) UILabelButton;
    if (backPrePageBtn_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }
    return true;
}

bool SwitchSliceDemoFragment::InitSwitchSliceViews()
{
    title2_ = new (std::nothrow) UILabel;
    if (title2_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn1_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn1_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn2_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn2_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn3_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn3_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn4_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn4_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn5_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn5_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn6_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn6_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn7_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn7_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchSliceBtn8_ = new (std::nothrow) UILabelButton;
    if (switchSliceBtn8_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }
    return true;
}

bool SwitchSliceDemoFragment::InitSwitchPageViews()
{
    title3_ = new (std::nothrow) UILabel;
    if (title3_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother1Page1_ = new (std::nothrow) UILabelButton;
    if (switchBrother1Page1_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother1Page2_ = new (std::nothrow) UILabelButton;
    if (switchBrother1Page2_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother1Page3_ = new (std::nothrow) UILabelButton;
    if (switchBrother1Page3_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother1Page4_ = new (std::nothrow) UILabelButton;
    if (switchBrother1Page4_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    title4_ = new (std::nothrow) UILabel;
    if (title4_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother2Page1_ = new (std::nothrow) UILabelButton;
    if (switchBrother2Page1_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother2Page2_ = new (std::nothrow) UILabelButton;
    if (switchBrother2Page2_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother2Page3_ = new (std::nothrow) UILabelButton;
    if (switchBrother2Page3_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }

    switchBrother2Page4_ = new (std::nothrow) UILabelButton;
    if (switchBrother2Page4_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }
    return true;
}

bool SwitchSliceDemoFragment::InitStateViews()
{
    titleState_ = new (std::nothrow) UILabel;
    if (titleState_ == nullptr) {
        GRAPHIC_LOGE("failed to alloc view");
        return false;
    }
    return true;
}

void SwitchSliceDemoFragment::DeinitViews()
{
    fragmentView_.RemoveAll();
    DeinitBackViews();
    DeinitSwitchSliceViews();
    DeinitSwitchPageViews();
    DeinitStateViews();
    return;
}

void SwitchSliceDemoFragment::DeinitBackViews()
{
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (backAppBtn_ != nullptr) {
        delete backAppBtn_;
        backAppBtn_ = nullptr;
    }
    if (backPrevSliceBtn_ != nullptr) {
        delete backPrevSliceBtn_;
        backPrevSliceBtn_ = nullptr;
    }
    if (backPrePageBtn_ != nullptr) {
        delete backPrePageBtn_;
        backPrePageBtn_ = nullptr;
    }
    return;
}

void SwitchSliceDemoFragment::DeinitSwitchSliceViews()
{
    if (title2_ != nullptr) {
        delete title2_;
        title2_ = nullptr;
    }
    if (switchSliceBtn1_ != nullptr) {
        delete switchSliceBtn1_;
        switchSliceBtn1_ = nullptr;
    }
    if (switchSliceBtn2_ != nullptr) {
        delete switchSliceBtn2_;
        switchSliceBtn2_ = nullptr;
    }
    if (switchSliceBtn3_ != nullptr) {
        delete switchSliceBtn3_;
        switchSliceBtn3_ = nullptr;
    }
    if (switchSliceBtn4_ != nullptr) {
        delete switchSliceBtn4_;
        switchSliceBtn4_ = nullptr;
    }
    if (switchSliceBtn5_ != nullptr) {
        delete switchSliceBtn5_;
        switchSliceBtn5_ = nullptr;
    }
    if (switchSliceBtn6_ != nullptr) {
        delete switchSliceBtn6_;
        switchSliceBtn6_ = nullptr;
    }
    if (switchSliceBtn7_ != nullptr) {
        delete switchSliceBtn7_;
        switchSliceBtn7_ = nullptr;
    }
    if (switchSliceBtn8_ != nullptr) {
        delete switchSliceBtn8_;
        switchSliceBtn8_ = nullptr;
    }
    return;
}

void SwitchSliceDemoFragment::DeinitSwitchPageViews()
{
    if (title3_ != nullptr) {
        delete title3_;
        title3_ = nullptr;
    }
    if (switchBrother1Page1_ != nullptr) {
        delete switchBrother1Page1_;
        switchBrother1Page1_ = nullptr;
    }
    if (switchBrother1Page2_ != nullptr) {
        delete switchBrother1Page2_;
        switchBrother1Page2_ = nullptr;
    }
    if (switchBrother1Page3_ != nullptr) {
        delete switchBrother1Page3_;
        switchBrother1Page3_ = nullptr;
    }
    if (switchBrother1Page4_ != nullptr) {
        delete switchBrother1Page4_;
        switchBrother1Page4_ = nullptr;
    }
    if (title4_ != nullptr) {
        delete title4_;
        title4_ = nullptr;
    }
    if (switchBrother2Page1_ != nullptr) {
        delete switchBrother2Page1_;
        switchBrother2Page1_ = nullptr;
    }
    if (switchBrother2Page2_ != nullptr) {
        delete switchBrother2Page2_;
        switchBrother2Page2_ = nullptr;
    }
    if (switchBrother2Page3_ != nullptr) {
        delete switchBrother2Page3_;
        switchBrother2Page3_ = nullptr;
    }
    if (switchBrother2Page4_ != nullptr) {
        delete switchBrother2Page4_;
        switchBrother2Page4_ = nullptr;
    }
    return;
}

void SwitchSliceDemoFragment::DeinitStateViews()
{
    if (titleState_ != nullptr) {
        delete titleState_;
        titleState_ = nullptr;
    }
}

void SwitchSliceDemoFragment::InitBackViewsText()
{
    constexpr size_t len = 128;
    char str[len];
    if (snprintf_s(str, len, len, "Slice %u Page %u.", (curSlice_ - VIEW_SWITCH_SLICE_DEMOA), curPage_) < 0) {
        GRAPHIC_LOGE("encoding error");
        return;
    }

    title_->SetText(str);
    title_->SetTextColor(color_);
    title_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);

    backAppBtn_->SetText("Back App");
    backAppBtn_->SetOnClickListener(this);
    backAppBtn_->SetTextColor(color_);
    backAppBtn_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);

    backPrevSliceBtn_->SetText("Back PreSlice");
    backPrevSliceBtn_->SetOnClickListener(this);
    backPrevSliceBtn_->SetTextColor(color_);
    backPrevSliceBtn_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);

    backPrePageBtn_->SetText("Back PrePage");
    backPrePageBtn_->SetOnClickListener(this);
    backPrePageBtn_->SetTextColor(color_);
    backPrePageBtn_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    return;
}

void SwitchSliceDemoFragment::InitSwitchSliceViewsText()
{
    title2_->SetText("Switch slice, btn format(back : cache : ani)");
    title2_->SetTextColor(color_);
    title2_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    title2_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 1 cache : 1 ani : 1
    switchSliceBtn1_->SetText("1:1:1");
    switchSliceBtn1_->SetOnClickListener(this);
    switchSliceBtn1_->SetTextColor(color_);
    switchSliceBtn1_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 1 cache : 1 ani : 0
    switchSliceBtn2_->SetText("1:1:0");
    switchSliceBtn2_->SetOnClickListener(this);
    switchSliceBtn2_->SetTextColor(color_);
    switchSliceBtn2_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 1 cache : 0 ani : 1
    switchSliceBtn3_->SetText("1:0:1");
    switchSliceBtn3_->SetOnClickListener(this);
    switchSliceBtn3_->SetTextColor(color_);
    switchSliceBtn3_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 1 cache : 0 ani : 0
    switchSliceBtn4_->SetText("1:0:0");
    switchSliceBtn4_->SetOnClickListener(this);
    switchSliceBtn4_->SetTextColor(color_);
    switchSliceBtn4_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 0 cache : 1 ani : 1
    switchSliceBtn5_->SetText("0:1:1");
    switchSliceBtn5_->SetOnClickListener(this);
    switchSliceBtn5_->SetTextColor(color_);
    switchSliceBtn5_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 0 cache : 1 ani : 0
    switchSliceBtn6_->SetText("0:1:0");
    switchSliceBtn6_->SetOnClickListener(this);
    switchSliceBtn6_->SetTextColor(color_);
    switchSliceBtn6_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 0 cache : 0 ani : 1
    switchSliceBtn7_->SetText("0:0:1");
    switchSliceBtn7_->SetOnClickListener(this);
    switchSliceBtn7_->SetTextColor(color_);
    switchSliceBtn7_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back : 0 cache : 0 ani : 0
    switchSliceBtn8_->SetText("0:0:0");
    switchSliceBtn8_->SetOnClickListener(this);
    switchSliceBtn8_->SetTextColor(color_);
    switchSliceBtn8_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    return;
}

void SwitchSliceDemoFragment::InitSwitchPageViewsText()
{
    constexpr size_t len = 128;
    char str[len];
    if (snprintf_s(str, len, len, "Switch page %u, btn format(back and cache : ani).", brother1Page_) < 0) {
        GRAPHIC_LOGE("encoding error");
        return;
    }
    char str2[len];
    if (snprintf_s(str2, len, len, "Switch page %u, btn format(back and cache : ani).", brother2Page_) < 0) {
        GRAPHIC_LOGE("encoding error");
        return;
    }

    title3_->SetText(str);
    title3_->SetTextColor(color_);
    title3_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    title3_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 1 ani : 1
    switchBrother1Page1_->SetText("1:1");
    switchBrother1Page1_->SetOnClickListener(this);
    switchBrother1Page1_->SetTextColor(color_);
    switchBrother1Page1_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 1 ani : 0
    switchBrother1Page2_->SetText("1:0");
    switchBrother1Page2_->SetOnClickListener(this);
    switchBrother1Page2_->SetTextColor(color_);
    switchBrother1Page2_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 0 ani : 1
    switchBrother1Page3_->SetText("0:1");
    switchBrother1Page3_->SetOnClickListener(this);
    switchBrother1Page3_->SetTextColor(color_);
    switchBrother1Page3_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 0 ani : 0
    switchBrother1Page4_->SetText("0:0");
    switchBrother1Page4_->SetOnClickListener(this);
    switchBrother1Page4_->SetTextColor(color_);
    switchBrother1Page4_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);

    title4_->SetText(str2);
    title4_->SetTextColor(color_);
    title4_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    title4_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 1 ani : 1
    switchBrother2Page1_->SetText("1:1");
    switchBrother2Page1_->SetOnClickListener(this);
    switchBrother2Page1_->SetTextColor(color_);
    switchBrother2Page1_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 1 ani : 0
    switchBrother2Page2_->SetText("1:0");
    switchBrother2Page2_->SetOnClickListener(this);
    switchBrother2Page2_->SetTextColor(color_);
    switchBrother2Page2_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 0 ani : 1
    switchBrother2Page3_->SetText("0:1");
    switchBrother2Page3_->SetOnClickListener(this);
    switchBrother2Page3_->SetTextColor(color_);
    switchBrother2Page3_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    // back and cache : 0 ani : 0
    switchBrother2Page4_->SetText("0:0");
    switchBrother2Page4_->SetOnClickListener(this);
    switchBrother2Page4_->SetTextColor(color_);
    switchBrother2Page4_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
    return;
}

void SwitchSliceDemoFragment::InitStateText()
{
    titleState_->SetText("State : OFF");
    titleState_->SetTouchable(true);
    titleState_->SetOnClickListener(this);
    titleState_->SetTextColor(color_);
    titleState_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    titleState_->SetStyle(STYLE_BACKGROUND_COLOR, bgColor_.full);
}

void SwitchSliceDemoFragment::BackViewsLayout()
{
    UIView *const views[] = {title_, backAppBtn_, backPrevSliceBtn_, backPrePageBtn_};
    constexpr int n = sizeof(views) / sizeof(views[0]);
    constexpr float x = 0.05;
    constexpr float y = 0.05;
    constexpr float w = 0.9 / 3;
    constexpr float h = 0.1;
    fragmentView_.Add(views[0]);
    views[0]->SetPositionPercent(x, y, w * 3, h);
    for (size_t i = 1; i < sizeof(views) / sizeof(views[0]); ++i) {
        fragmentView_.Add(views[i]);
        views[i]->SetPositionPercent(x + (i - 1) * w, y + h, w, h);
    }
    return;
}

void SwitchSliceDemoFragment::SwitchSliceViewsLayout()
{
    UIView *const views[] = {title2_, switchSliceBtn1_, switchSliceBtn2_, switchSliceBtn3_, switchSliceBtn4_,
                             switchSliceBtn5_, switchSliceBtn6_, switchSliceBtn7_, switchSliceBtn8_};
    constexpr int n = sizeof(views) / sizeof(views[0]);
    constexpr float x = 0.05;
    constexpr float y = 0.05 + 0.2;
    constexpr float w = 0.9 / 4;
    constexpr float h = 0.1;
    fragmentView_.Add(views[0]);
    views[0]->SetPositionPercent(x, y, w * 4, h * 0.5);
    for (size_t i = 1; i < sizeof(views) / sizeof(views[0]); ++i) {
        fragmentView_.Add(views[i]);
        float itemX = x + w * ((i - 1) % 4);
        float itemY = y + h * ((i - 1) / 4 + 0.5);
        views[i]->SetPositionPercent(itemX, itemY, w, h);
    }
    return;
}

void SwitchSliceDemoFragment::SwitchPageViewsLayout()
{
    UIView *const views[] = {title3_, switchBrother1Page1_, switchBrother1Page2_, switchBrother1Page3_, switchBrother1Page4_};
    constexpr int n = sizeof(views) / sizeof(views[0]);
    constexpr float x = 0.05;
    constexpr float y = 0.05 + 0.45;
    constexpr float w = 0.9 / 4;
    constexpr float h = 0.1;
    fragmentView_.Add(views[0]);
    views[0]->SetPositionPercent(x, y, w * 4, h * 0.5);
    for (size_t i = 1; i < sizeof(views) / sizeof(views[0]); ++i) {
        fragmentView_.Add(views[i]);
        float itemX = x + w * (i - 1);
        float itemY = y + h * 0.5;
        views[i]->SetPositionPercent(itemX, itemY, w, h);
    }

    UIView *const views2[] = {title4_, switchBrother2Page1_, switchBrother2Page2_, switchBrother2Page3_, switchBrother2Page4_};
    fragmentView_.Add(views2[0]);
    views2[0]->SetPositionPercent(x, y + h * 1.5, w * 4, h * 0.5);
    for (size_t i = 1; i < sizeof(views2) / sizeof(views2[0]); ++i) {
        fragmentView_.Add(views2[i]);
        float itemX = x + w * (i - 1);
        float itemY = y + h * 2;
        views2[i]->SetPositionPercent(itemX, itemY, w, h);
    }
    return;
}

void SwitchSliceDemoFragment::StateViewsLayout()
{
    constexpr float x = 0.05;
    constexpr float y = 0.05 + 0.75;
    constexpr float w = 0.9;
    constexpr float h = 0.1;
    fragmentView_.Add(titleState_);
    titleState_->SetPositionPercent(x, y, w, h);
}

void SwitchSliceDemoFragment::OnCreateView(void *data)
{
    curSlice_ = NativeAbility::GetInstance().GetCurSliceId();
    SetNextSlice(curSlice_);
    if (!InitViews()) {
        DeinitViews();
        GRAPHIC_LOGE("failed init views");
        return;
    }
    return;
}

void SwitchSliceDemoFragment::OnDestroyView()
{
    DeinitViews();
    color_.full = 0;
    bgColor_.full = 0;
    curPage_ = 0;
    curSlice_ = 0;
    nextSlice_ = 0;
    brother1Page_ = 0;
    brother2Page_ = 0;
    return;
}

void SwitchSliceDemoFragment::SetNextSlice(uint16_t curSlice)
{
    switch (curSlice) {
        case VIEW_SWITCH_SLICE_DEMOA :
            nextSlice_ = VIEW_SWITCH_SLICE_DEMOB;
            break;
        case VIEW_SWITCH_SLICE_DEMOB :
            nextSlice_ = VIEW_SWITCH_SLICE_DEMOC;
            break;
        case VIEW_SWITCH_SLICE_DEMOC :
            nextSlice_ = VIEW_SWITCH_SLICE_DEMOD;
            break;
        case VIEW_SWITCH_SLICE_DEMOD :
            nextSlice_ = VIEW_SWITCH_SLICE_DEMOA;
            break;
        default:
            GRAPHIC_LOGE("unkonwn view id %u!", curSlice);
            break;
    }
}
}  // namespace OHOS
