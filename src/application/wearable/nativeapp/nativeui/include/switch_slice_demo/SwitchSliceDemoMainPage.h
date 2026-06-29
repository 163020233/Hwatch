/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoMainPage
 * Create: 2026-03-12
 */
#ifndef SWITCH_SLICE_DEMO_MAIN_PAGE_H
#define SWITCH_SLICE_DEMO_MAIN_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "SwitchSliceDemoPresenter.h"
#include "SwitchSliceDemoFragment.h"

namespace OHOS {

class SwitchSliceDemoMainPage : public SlicePage<SwitchSliceDemoPresenter> {
public:
    SwitchSliceDemoMainPage();
    ~SwitchSliceDemoMainPage();
    void OnStart(void* data) override;

private:
    SwitchSliceDemoMainPage(const SwitchSliceDemoMainPage &) = delete;
    SwitchSliceDemoMainPage &operator=(const SwitchSliceDemoMainPage &) = delete;
    SwitchSliceDemoMainPage(SwitchSliceDemoMainPage &&) = delete;
    SwitchSliceDemoMainPage &operator=(SwitchSliceDemoMainPage &&) = delete;
    SwitchSliceDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* SWITCH_SLICE_DEMO_MAIN_PAGE_H */
