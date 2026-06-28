/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoSecPage
 * Create: 2026-02-12
 */
#ifndef DEV_ROTATE_DEMO_SEC_PAGE_H
#define DEV_ROTATE_DEMO_SEC_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "DevRotateDemoPresenter.h"
#include "DevRotateDemoFragment.h"

namespace OHOS {

class DevRotateDemoSecPage : public SlicePage<DevRotateDemoPresenter> {
public:
    DevRotateDemoSecPage();
    ~DevRotateDemoSecPage();
    void OnStart(void* data) override;
    void OnOrientationChanged(AppRotationType rot) override;

private:
    DevRotateDemoSecPage(const DevRotateDemoSecPage &) = delete;
    DevRotateDemoSecPage &operator=(const DevRotateDemoSecPage &) = delete;
    DevRotateDemoSecPage(DevRotateDemoSecPage &&) = delete;
    DevRotateDemoSecPage &operator=(DevRotateDemoSecPage &&) = delete;
    DevRotateDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* DEV_ROTATE_DEMO_SEC_PAGE_H */
