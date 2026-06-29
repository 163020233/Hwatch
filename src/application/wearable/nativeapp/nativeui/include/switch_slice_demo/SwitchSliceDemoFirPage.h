/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: SwitchSliceDemoFirPage
 * Create: 2026-02-12
 */
#ifndef SWITCH_SLICE_DEMO_FIR_PAGE_H
#define SWITCH_SLICE_DEMO_FIR_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "SwitchSliceDemoPresenter.h"
#include "SwitchSliceDemoFragment.h"

namespace OHOS {

class SwitchSliceDemoFirPage : public SlicePage<SwitchSliceDemoPresenter> {
public:
    SwitchSliceDemoFirPage();
    ~SwitchSliceDemoFirPage();
    void OnStart(void* data) override;

private:
    SwitchSliceDemoFirPage(const SwitchSliceDemoFirPage &) = delete;
    SwitchSliceDemoFirPage &operator=(const SwitchSliceDemoFirPage &) = delete;
    SwitchSliceDemoFirPage(SwitchSliceDemoFirPage &&) = delete;
    SwitchSliceDemoFirPage &operator=(SwitchSliceDemoFirPage &&) = delete;
    SwitchSliceDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* SWITCH_SLICE_DEMO_FIR_PAGE_H */
