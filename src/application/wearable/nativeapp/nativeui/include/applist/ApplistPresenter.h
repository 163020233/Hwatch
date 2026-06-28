/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ApplistPresenter.h
 * Author:
 * Create: 2021-09-18
 */

#ifndef APPLIST_PRESENTER_H
#define APPLIST_PRESENTER_H

#include "Presenter.h"
#include "applist/ApplistView.h"
#include "applist/ApplistModel.h"

namespace OHOS {
class ApplistPresenter : public Presenter<ApplistView> {
public:
    ApplistPresenter();
    ~ApplistPresenter() override;
    void OnStart() override;
    void OnResume() override;
    void OnPause() override;
    ApplistView* GetAppListView();

private:
    ApplistModel *model{nullptr};
};
}

#endif // APPLIST_PRESENTER_H
