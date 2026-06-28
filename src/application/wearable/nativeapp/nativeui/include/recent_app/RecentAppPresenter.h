/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: RecentAppPresenter.h
 * Create: 2025-03-02
 */

#ifndef RECENT_APP_PRESENTER_H
#define RECENT_APP_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"

namespace OHOS {
class RecentAppView;
class RecentAppPresenter : public Presenter<RecentAppView> {
public:
    RecentAppPresenter() {}
    ~RecentAppPresenter() override {}
    void OnResume() override;
};
}

#endif // RECENT_APP_PRESENTER_H