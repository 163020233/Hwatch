/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoFragment
 * Author: Hisi Graphic Team
 * Created: 2026-02
 */

#ifndef SWITCH_SLICE_DEMO_FRAGMENT_H
#define SWITCH_SLICE_DEMO_FRAGMENT_H
#include "components/ui_fragment.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
class SwitchSliceDemoFragment : public UIFragment, public UIView::OnClickListener {
public:
    SwitchSliceDemoFragment(ColorType txtColor, ColorType bgColor, uint16_t curPage, uint16_t brother1Page, uint16_t brother2Page);
    ~SwitchSliceDemoFragment();
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    SwitchSliceDemoFragment(const SwitchSliceDemoFragment &) = delete;
    SwitchSliceDemoFragment &operator=(const SwitchSliceDemoFragment &) = delete;
    SwitchSliceDemoFragment(SwitchSliceDemoFragment &&) = delete;
    SwitchSliceDemoFragment &operator=(SwitchSliceDemoFragment &&) = delete;
    bool InitViews();
    bool InitBackViews();
    bool InitSwitchSliceViews();
    bool InitSwitchPageViews();
    bool InitStateViews();
    void InitBackViewsText();
    void InitSwitchSliceViewsText();
    void InitSwitchPageViewsText();
    void InitStateText();
    void DeinitViews();
    void DeinitBackViews();
    void DeinitSwitchSliceViews();
    void DeinitSwitchPageViews();
    void DeinitStateViews();
    void BackViewsLayout();
    void SwitchSliceViewsLayout();
    void SwitchPageViewsLayout();
    void StateViewsLayout();
    void OnCreateView(void* data) override;
    void OnDestroyView() override;
    void SetNextSlice(uint16_t curSlice);

    UILabel *title_ = nullptr;
    UILabel *titleState_ = nullptr;
    UILabelButton *backAppBtn_ = nullptr;
    UILabelButton *backPrevSliceBtn_ = nullptr;
    UILabelButton *backPrePageBtn_ = nullptr;
    UILabel *title2_ = nullptr;
    // back : 1 cache : 1 ani : 1
    UILabelButton *switchSliceBtn1_ = nullptr;
    // back : 1 cache : 1 ani : 0
    UILabelButton *switchSliceBtn2_ = nullptr;
    // back : 1 cache : 0 ani : 1
    UILabelButton *switchSliceBtn3_ = nullptr;
    // back : 1 cache : 0 ani : 0
    UILabelButton *switchSliceBtn4_ = nullptr;
    // back : 0 cache : 1 ani : 1
    UILabelButton *switchSliceBtn5_ = nullptr;
    // back : 0 cache : 1 ani : 0
    UILabelButton *switchSliceBtn6_ = nullptr;
    // back : 0 cache : 0 ani : 0
    UILabelButton *switchSliceBtn7_ = nullptr;
    // back : 0 cache : 0 ani : 0
    UILabelButton *switchSliceBtn8_ = nullptr;
    UILabel *title3_ = nullptr;
    // back and cache : 1 ani : 1
    UILabelButton *switchBrother1Page1_ = nullptr;
    // back and cache : 1 ani : 0
    UILabelButton *switchBrother1Page2_ = nullptr;
    // back and cache : 0 ani : 1
    UILabelButton *switchBrother1Page3_ = nullptr;
    // back and cache : 0 ani : 0
    UILabelButton *switchBrother1Page4_ = nullptr;
    UILabel *title4_ = nullptr;
    // back and cache : 1 ani : 1
    UILabelButton *switchBrother2Page1_ = nullptr;
    // back and cache : 1 ani : 0
    UILabelButton *switchBrother2Page2_ = nullptr;
    // back and cache : 0 ani : 1
    UILabelButton *switchBrother2Page3_ = nullptr;
    // back and cache : 0 ani : 0
    UILabelButton *switchBrother2Page4_ = nullptr;
    ColorType color_;
    ColorType bgColor_;
    uint16_t curSlice_ = 0;
    uint16_t nextSlice_ = 0;
    uint16_t curPage_ = 0;
    uint16_t brother1Page_ = 0;
    uint16_t brother2Page_ = 0;
};
} // namespace OHOS
#endif // SWITCH_SLICE_DEMO_FRAGMENT_H
