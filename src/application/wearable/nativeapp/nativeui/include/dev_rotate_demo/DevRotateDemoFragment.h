/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoFragment
 * Author: Hisi Graphic Team
 * Created: 2026-02
 */

#ifndef DEV_ROTATE_DEMO_FRAGMENT_H
#define DEV_ROTATE_DEMO_FRAGMENT_H
#include "components/ui_fragment.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "RotationManager.h"

namespace OHOS {
class DevRotateDemoFragment : public UIFragment, public UIView::OnClickListener {
public:
    DevRotateDemoFragment(ColorType txtColor, ColorType bgColor, uint16_t curPage, uint16_t dstPage);
    ~DevRotateDemoFragment();
    void OnRotationChanged(AppRotationType rot);
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    DevRotateDemoFragment(const DevRotateDemoFragment &) = delete;
    DevRotateDemoFragment &operator=(const DevRotateDemoFragment &) = delete;
    DevRotateDemoFragment(DevRotateDemoFragment &&) = delete;
    DevRotateDemoFragment &operator=(DevRotateDemoFragment &&) = delete;
    bool InitViews();
    void DeinitViews();
    void InitText(const char *text, ColorType txtColor, ColorType bColor);
    void NormalLayout();
    void RotateLayout();
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

    UILabel *title_ = nullptr;
    UILabelButton *exitBtn_ = nullptr;
    UILabelButton *transNoAni_ = nullptr;
    UILabelButton *transAni_ = nullptr;
    UILabelButton *transBack_ = nullptr;
    UILabelButton *rotate0_ = nullptr;
    UILabelButton *rotate90_ = nullptr;
    ColorType color_;
    ColorType bgColor_;
    uint16_t curPage_ = 0;
    uint16_t dstPage_ = 0;
};
} // namespace OHOS
#endif // DEV_ROTATE_DEMO_FRAGMENT_H
