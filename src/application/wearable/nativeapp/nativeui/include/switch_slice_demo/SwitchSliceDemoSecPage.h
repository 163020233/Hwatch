/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoSecPage
 * Create: 2026-02-12
 */
#ifndef SWITCH_SLICE_DEMO_SEC_PAGE_H
#define SWITCH_SLICE_DEMO_SEC_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "SwitchSliceDemoPresenter.h"
#include "SwitchSliceDemoFragment.h"

namespace OHOS {

class SwitchSliceDemoSecPage : public SlicePage<SwitchSliceDemoPresenter> {
public:
    SwitchSliceDemoSecPage();
    ~SwitchSliceDemoSecPage();
    void OnStart(void* data) override;

private:
    SwitchSliceDemoSecPage(const SwitchSliceDemoSecPage &) = delete;
    SwitchSliceDemoSecPage &operator=(const SwitchSliceDemoSecPage &) = delete;
    SwitchSliceDemoSecPage(SwitchSliceDemoSecPage &&) = delete;
    SwitchSliceDemoSecPage &operator=(SwitchSliceDemoSecPage &&) = delete;
    SwitchSliceDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* SWITCH_SLICE_DEMO_SEC_PAGE_H */
