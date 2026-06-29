/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoFirPage
 * Create: 2026-02-12
 */
#ifndef DEV_ROTATE_DEMO_FIR_PAGE_H
#define DEV_ROTATE_DEMO_FIR_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "DevRotateDemoPresenter.h"
#include "DevRotateDemoFragment.h"

namespace OHOS {

class DevRotateDemoFirPage : public SlicePage<DevRotateDemoPresenter> {
public:
    DevRotateDemoFirPage();
    ~DevRotateDemoFirPage();
    void OnStart(void* data) override;
    void OnOrientationChanged(AppRotationType rot) override;

private:
    DevRotateDemoFirPage(const DevRotateDemoFirPage &) = delete;
    DevRotateDemoFirPage &operator=(const DevRotateDemoFirPage &) = delete;
    DevRotateDemoFirPage(DevRotateDemoFirPage &&) = delete;
    DevRotateDemoFirPage &operator=(DevRotateDemoFirPage &&) = delete;
    DevRotateDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* DEV_ROTATE_DEMO_FIR_PAGE_H */
