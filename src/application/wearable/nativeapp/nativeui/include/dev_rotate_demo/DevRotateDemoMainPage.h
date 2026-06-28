/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: DevRotateDemoMainPage
 * Create: 2026-02-12
 */
#ifndef DEV_ROTATE_DEMO_MAIN_PAGE_H
#define DEV_ROTATE_DEMO_MAIN_PAGE_H
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "SlicePage.h"
#include "DevRotateDemoPresenter.h"
#include "DevRotateDemoFragment.h"

namespace OHOS {

class DevRotateDemoMainPage : public SlicePage<DevRotateDemoPresenter> {
public:
    DevRotateDemoMainPage();
    ~DevRotateDemoMainPage();
    void OnStart(void* data) override;

private:
    DevRotateDemoMainPage(const DevRotateDemoMainPage &) = delete;
    DevRotateDemoMainPage &operator=(const DevRotateDemoMainPage &) = delete;
    DevRotateDemoMainPage(DevRotateDemoMainPage &&) = delete;
    DevRotateDemoMainPage &operator=(DevRotateDemoMainPage &&) = delete;
    DevRotateDemoFragment fragment_;
};
}  // namespace OHOS
#endif /* DEV_ROTATE_DEMO_MAIN_PAGE_H */
