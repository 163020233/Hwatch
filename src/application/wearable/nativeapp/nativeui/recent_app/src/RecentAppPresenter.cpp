/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: RecentAppPresenter.cpp
 * Create: 2025-08-12
 */

#include "recent_app/RecentAppPresenter.h"
#include "recent_app/RecentAppView.h"
#include "RecentManager.h"
#include "NativeRegisterManager.h"
#include "msg_center_customer.h"

namespace OHOS {

REGIST_SLICE(VIEW_RECENT_APP, RecentAppView, RecentAppPresenter);

void RecentAppPresenter::OnResume()
{
    if (view_ != nullptr) {
        view_->OnResume();
    }
}
}
